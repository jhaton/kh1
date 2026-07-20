#include "common.h"
#include "eekernel.h"
#include <sifdev.h>
#include <sifrpc.h>
#include <sifcmd.h>
#include <stdarg.h>
#include <errno.h>
#include "lib/libkernl/filestub.h"

#define MAX_IOB_COUNT 32
#define MAX_ARG_SIZE 1024
#define UNCACHED(p) (((u32)p | 0x20000000))

vs32 _sceFs_q[MAX_IOB_COUNT] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
s32 _fs_init = 0;
s32 _fs_semid = -1;
s32 _fs_iob_semid = -1;
s32 _fs_fsq_semid = -1;

// BSS
u32 rcv_adr; // likely static
u32* ip0; // likely static
_sceFsData _send_data __attribute__((aligned (64)));
s32 _rcv_data_rpc __attribute__((aligned (64))); // unverified
_sceFsIntrData _rcv_data_cmd __attribute__((aligned (64)));
_sceFsIob _iob[MAX_IOB_COUNT];
sceSifClientData _cd;
char _fsversion[4];
_sceFsPoffData _sif_FsRcv_Data __attribute__((aligned (64)));
_sceFsPoffData _sif_FsPoff_Data __attribute__((aligned (64)));

static void _sceFsIobSemaMK(void) {
    struct SemaParam semaParam;

    if (_fs_iob_semid == -1) {
        semaParam.option = 0;
        semaParam.initCount = 1;
        semaParam.maxCount = 1;

        _fs_iob_semid = CreateSema(&semaParam);
        _fs_fsq_semid = CreateSema(&semaParam);
    }
}

static _sceFsIob* new_iob(void) {
    _sceFsIob* iob;
    s32 unusedIndex;
    
    _sceFsIobSemaMK();
    WaitSema(_fs_iob_semid);
    
    for (iob = &_iob[0]; iob < &_iob[MAX_IOB_COUNT]; iob++) {
        if (iob->i_flag == 0){
            iob->i_flag = 0x10000000;
            SignalSema(_fs_iob_semid);
            return iob;
        }
    }
    
    SignalSema(_fs_iob_semid);
    return NULL;
}

static _sceFsIob* get_iob(s32 fd) {
    _sceFsIob* iob;
    
    _sceFsIobSemaMK();
    WaitSema(_fs_iob_semid);
    
    if (ARRAY_COUNTU(_iob) <= fd) {
        SignalSema(_fs_iob_semid);
        return 0;
    }
    
    iob = &_iob[fd];
    SignalSema(_fs_iob_semid);
    return iob;
}

static void _sceFs_Rcv_Intr(void* packet, void* handlerData) {
    s32 semaphoreId;
    void* returnAddress;
    u32 returnSize;
    u32 returnMode;
    s32 index;
    u8* copyDestination;
    _sceFsReadIntrData* readData;
    void* resultAddress;
    u32 resultSize;
    
    memcpy(&semaphoreId, (void*)UNCACHED(&_rcv_data_cmd.rcvData.ee_semid), sizeof(s32));
    memcpy(&returnMode, (void*)UNCACHED(&_rcv_data_cmd.rcvData.ee_retmod), sizeof(s32));
    memcpy(&returnAddress, (void*)UNCACHED(&_rcv_data_cmd.rcvData.ee_retadr), sizeof(s32));
    memcpy(&returnSize, (void*)UNCACHED(&_rcv_data_cmd.rcvData.ee_retsiz), sizeof(s32));
    
    if (-0x1 < semaphoreId) {
        memcpy(returnAddress, (void*)UNCACHED(&_rcv_data_cmd.rcvData.data_top[0]), returnSize);
    }

    switch (returnMode) {
        case 0x2:
            // _sceFsReadIntrData
            readData = (_sceFsReadIntrData *)UNCACHED(&_rcv_data_cmd.rcvData.data_top[1]);
            if (readData->psize > 0) {
                copyDestination = (u8*)readData->paddr;
                for (index = 0; index < readData->psize; index++) {
                    copyDestination[index] = readData->pdata[index];
                }
            }
            
            if (readData->ssize > 0) {
                copyDestination = (u8*)readData->saddr;
                for (index = 0; index < readData->ssize; index++) {
                    copyDestination[index] = readData->sdata[index];
                }
            }
            break;
        case 0xb:
            // _sceFsIntrRcvDirData
            memcpy(&resultAddress, (void*)UNCACHED(&_rcv_data_cmd.rcvDirData.ee_dentadr), sizeof(s32));
            memcpy(resultAddress, (void*)UNCACHED(&_rcv_data_cmd.rcvDirData.dent), sizeof(struct sce_dirent));
            break;
        case 0xc:
            // unk
            memcpy(&resultAddress, (void*)UNCACHED(&_rcv_data_cmd.rcvData.data_top[1]), sizeof(s32));
            memcpy(resultAddress, (void*)UNCACHED(&_rcv_data_cmd.rcvData.data_top[2]), 0x40);
            break;
        case 0x17:
        case 0x19:
        case 0x1a:
            // _sceFsIntrRcvReadLData
            // _sceFsIntrRcvIoctlData
            // _sceFsIntrRcvDevctlData
            memcpy(&resultAddress, (void*)UNCACHED(&_rcv_data_cmd.rcvData.data_top[1]), sizeof(s32));
            memcpy(&resultSize, (void*)UNCACHED(&_rcv_data_cmd.rcvData.data_top[2]), sizeof(s32));
            if (resultSize > 0x400){
                resultSize = 0x400;
            }
            memcpy(resultAddress, (void*)UNCACHED(&_rcv_data_cmd.rcvData.data_top[3]), resultSize);
            break;
    }

    
    if (semaphoreId < 0x0) {
        semaphoreId = -semaphoreId;
        for (index = 0; index < ARRAY_COUNT(_sceFs_q); index++) {
            if (_sceFs_q[index] == semaphoreId) {
                _sceFs_q[index] = -1;
                break;
            }
        }
    } else {
        iSignalSema(semaphoreId);
    }
    return;
}

static void _sceFsSemInit(void) {
    struct SemaParam semaParam;
    if (_fs_semid == -1) {
        semaParam.option = 0;
        semaParam.initCount = 1;
        semaParam.maxCount = 1;
        _fs_semid = CreateSema(&semaParam);
    }
}

static s32 _sceFsWaitS(s32 unused) {
    _sceFsSemInit();
    WaitSema(_fs_semid);
    return 0;
}

static void _sceFsSigSema(void) {
    SignalSema(_fs_semid);
}

int* scePowerOffHandler(void (*callback)(void*), void* callbackData) {
    int* previousCallback;
    _sceFsPoffData* powerOffData;

    powerOffData = &_sif_FsPoff_Data;
    _sceFsWaitS(0x1b);
    if (_fs_init == 0x0) {
        sceFsInit();
    }
    DIntr();
    previousCallback = (int*)_sif_FsPoff_Data.sceFsPoffCbfunc;
    powerOffData->sceFsPoffCbdata = callbackData;
    powerOffData->sceFsPoffCbfunc = callback;
    EIntr();
    _sceFsSigSema();
    return previousCallback;
}

static void _sceFs_Poff_Intr(void* packet, void* handlerData) {
    _sceFsPoffData* powerOffData;
    powerOffData = (_sceFsPoffData*)handlerData;
    if (powerOffData->sceFsPoffCbfunc != NULL) {
        powerOffData->sceFsPoffCbfunc(powerOffData->sceFsPoffCbdata);
    }
    ExitHandler();
}

int sceFsInit(void) {
    s32 spinCount;
    s32 rpcStatus;
    s32 unusedBufferMode;
    _sceFsIob* iob;
    _sceFsPoffData* powerOffData;

    powerOffData = &_sif_FsPoff_Data;
    sceSifInitRpc(0x0);
    powerOffData->sceFsPoffCbfunc = NULL;
    powerOffData->sceFsPoffCbdata = NULL;
    DIntr();
    sceSifAddCmdHandler(SIF_CMDI_SYSTEM | 0x11, &_sceFs_Rcv_Intr, &_sif_FsRcv_Data);
    sceSifAddCmdHandler(SIF_CMDI_SYSTEM | 0x13, &_sceFs_Poff_Intr, &_sif_FsPoff_Data);
    EIntr();

    while( 1 ) {
        rpcStatus = sceSifBindRpc(&_cd, 0x80000001, 0x0);
        if (rpcStatus < 0x0) {
            return -1;
        }
        
        if (_cd.serve != NULL){
            break;
        }

        for (spinCount = 0x100000; spinCount != -1; spinCount--) {}
    }

    _sceFsIobSemaMK();
    WaitSema(_fs_iob_semid);
    
    for (iob = &_iob[0]; iob < &_iob[MAX_IOB_COUNT]; iob++) {
        iob->i_flag = 0;
    }
    
    SignalSema(_fs_iob_semid);
    rcv_adr = (u32)&_rcv_data_cmd;
    rpcStatus = sceSifCallRpc(&_cd, 0xff, 0x0, &rcv_adr, sizeof(rcv_adr), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        return -SCE_ECALLMISS;
    }
    else {
        memcpy(&_fsversion, (void*)UNCACHED(&_rcv_data_rpc), sizeof(_fsversion));
        _fs_init = 0x1;
        return 0; // SCE_OK doesn't seem to exist in this SDK version
    }
}

char* _fswildcard = "....";

// TODO these two vars aren't from this file
s32 D_00465374 = -1;
char* D_00465378 = "....\0";

// extern char __ps2_klibinfo_[16];
extern char D_00464B18; // TODO use the proper symbol for this


static s32 _fs_version(void) {
    s32 versionMismatch = FALSE;
    char* libraryVersion;

    libraryVersion = &D_00464B18; // TODO use the proper symbol for this
    // libver = &__ps2_klibinfo_[12];

    if (memcmp(&_fsversion, libraryVersion, 4) != 0 && memcmp(&_fsversion, _fswildcard, 4) != 0) {
        versionMismatch = memcmp(libraryVersion, _fswildcard, 4) != 0;
    }
    return versionMismatch;
}

/**
 * @brief Invalidate file service bind information. 
 * This function should always be called after the IOP is reset,
 * since the file service RPC BIND information will be invalid.
 * @return Always returns 0
 */
s32 sceFsReset(void) {
    _fs_init = 0;
    memset(_fsversion, 0, sizeof(_fsversion));
    return 0;
}

int sceOpen(const char* filename, int flag, ...) {
    u32 mode;
    s32 descriptorIndex;
    s32 result;
    s32 remoteFd;
    _sceFsOpenData* openData;
    _sceFsIob* iob;
    struct SemaParam semaParam;
    s32 nameLength;
    va_list arguments;
    s32 semaphoreId;

    openData = &_send_data.openData;
    
    _sceFsWaitS(0x0);
    if (_fs_init == 0x0) {
        sceFsInit();
    }

    
    if (_fs_version() != 0x0) {
        _sceFsSigSema();
        return -SCE_EVERSIONMISS;
    } 
        
    iob = new_iob();
    if (iob == NULL) {
        _sceFsSigSema();
        return -ENODEV;
    }

    va_start (arguments, flag);
    mode = va_arg (arguments, int);
    va_end (arguments);
    
    for (nameLength = 0; nameLength < MAX_ARG_SIZE && (openData->name[nameLength] = filename[nameLength]) != 0; nameLength++) { }
    if (nameLength == MAX_ARG_SIZE) {
        openData->name[MAX_ARG_SIZE-1] = 0x0;
    }

    descriptorIndex = iob - _iob;
    openData->flag = flag & ~0x90000000;
    openData->ee_fds = descriptorIndex;
    openData->mode = mode;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    openData->ee_semid = semaphoreId;
    openData->ee_retadr = (u32)&remoteFd;
    openData->ee_retsiz = sizeof(remoteFd);

    result = sceSifCallRpc(&_cd, 0x0, 0x0, &_send_data, sizeof(_sceFsOpenData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), 0x0, 0x0);
    if (result < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
    
    result = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (result == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }
        
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    if (remoteFd < 0x0) {
        WaitSema(_fs_iob_semid);
        iob->i_flag = 0x0;
        SignalSema(_fs_iob_semid);
        return remoteFd;
    }
    
    result = descriptorIndex;
    WaitSema(_fs_iob_semid);
    iob->i_fd = remoteFd;
    iob->i_flag |= flag;
    SignalSema(_fs_iob_semid);

    return result;
}

int sceClose(int fd) {
    _sceFsCloseData* closeData;
    _sceFsIob* iob;
    s32 rpcStatus;
    s32 descriptorIndex;
    s32 closeResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    closeData = &_send_data.closeData;
    
    iob = get_iob(fd);
    _sceFsWaitS(0x1);
    if (_fs_init == 0x0) {
        _sceFsSigSema();
        return -0x1; // errno.h says this would be "Not super-user" but that doesn't make sense
    }
    
    if ((iob == NULL) || (iob->i_flag == 0x0)) {
        _sceFsSigSema();
        return -EBADF;
    }
    
    descriptorIndex = iob - _iob;
    closeData->fd = iob->i_fd;
    closeData->ee_fds = descriptorIndex;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    closeData->ee_semid = semaphoreId;
    closeData->ee_retadr = (u32)&closeResult;
    closeData->ee_retsiz = sizeof(closeResult);
    rpcStatus = sceSifCallRpc(&_cd, 0x1, 0x0, &_send_data, sizeof(_sceFsCloseData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    iob->i_flag = 0x0;
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }
        
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    if (closeResult < 0) {
        return closeResult;
    }
    
    return 0; // SCE_OK
}

int sceLseek(int fd, int offset, int how) {
    _sceFsLseekData* seekData;
    s32 seekResult;
    s32 unusedResult;
    s32 queueIndex;
    s32 fileFlags;
    _sceFsIob* iob;
    s32 rpcStatus;
    struct SemaParam semaParam;
    s32 semaphoreId;

    seekData = &_send_data.lSeekData;
    
    iob = get_iob(fd);
    _sceFsWaitS(0x4);
    if (_fs_init == 0x0) {
        _sceFsSigSema();
        return -0x1; // errno.h says this would be "Not super-user" but that doesn't make sense
    }
    
    if ((iob == NULL) || (iob->i_flag == 0x0)) {
        _sceFsSigSema();
        return -EBADF;
    }
    
    fileFlags = iob->i_flag;
    seekData->fd = iob->i_fd;
    seekData->offset = offset;
    seekData->how = how;
    seekData->ee_fds = iob - _iob;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    seekData->ee_semid = semaphoreId;
    seekData->ee_retadr = (u32)&seekResult;
    seekData->ee_retsiz = sizeof(seekResult);
        
    if ((fileFlags & 0x8000) != 0x0) {
        WaitSema(_fs_fsq_semid);
        for (queueIndex = 0; queueIndex < 32; queueIndex++) {
            if (_sceFs_q[queueIndex] == -1) {
                _sceFs_q[queueIndex] = seekData->ee_semid;
                seekData->ee_semid = -seekData->ee_semid;
                break;
            }
        }
        SignalSema(_fs_fsq_semid);
    }
    
    rpcStatus = sceSifCallRpc(&_cd, 0x4, 0x0, &_send_data, sizeof(_sceFsLseekData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }

    if ((fileFlags & 0x8000) != 0x0) {
        DeleteSema(semaphoreId);
        return 0; // SCE_OK
    } 
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return seekResult;
}

int sceRead(int fd, void* buffer, int size) {
    _sceFsReadData* readData;
    _sceFsIob* iob;
    s32 rpcStatus;
    s32 readResult;
    s32 unusedCount;
    s32 queueIndex;
    s32 fileFlags;
    struct SemaParam semaParam;
    s32 semaphoreId;

    readData = &_send_data.readData;
    
    iob = get_iob(fd);
    _sceFsWaitS(0x2);
    if (_fs_init == 0x0) {
        _sceFsSigSema();
        return -0x1; // errno.h says this would be "Not super-user" but that doesn't make sense
    }
    
    if ((iob == NULL) || (iob->i_flag == 0x0)) {
        _sceFsSigSema();
        return -EBADF;
    }
    
    fileFlags = iob->i_flag;
    readData->fd = iob->i_fd;
    readData->addr = (u32)buffer;
    readData->size = size;
    readData->ee_fds = iob - _iob;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    readData->ee_semid = semaphoreId;
    readData->ee_retadr = (u32)&readResult;
    readData->ee_retsiz = sizeof(readResult);
        
    if ((fileFlags & 0x8000) != 0x0) {
        WaitSema(_fs_fsq_semid);
        for (queueIndex = 0; queueIndex < 32; queueIndex++) {
            if (_sceFs_q[queueIndex] == -1) {
                _sceFs_q[queueIndex] = readData->ee_semid;
                readData->ee_semid = -readData->ee_semid;
                break;
            }
        }
        SignalSema(_fs_fsq_semid);
    }
    if ((fileFlags & 0x20000000) == 0x0) {
        sceSifWriteBackDCache(buffer, size);
    }
    sceSifWriteBackDCache(&_rcv_data_cmd, 0xA4);
    sceSifWriteBackDCache(readData, sizeof(_sceFsReadData));
    rpcStatus = sceSifCallRpc(&_cd, 0x2, 0x0, &_send_data, sizeof(_sceFsReadData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }

    if ((fileFlags & 0x8000) != 0x0) {
        DeleteSema(semaphoreId);
        return 0; // SCE_OK
    } 
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return readResult;
}

int sceWrite(int fd, const void* buffer, int size) {
    _sceFsWriteData* writeData;
    _sceFsIob* iob;
    s32 prefixIndex;
    s32 queueIndex;
    s32 prefixSize;
    s32 rpcStatus;
    s32 writeResult;
    struct SemaParam semaParam;
    s32 fileFlags;
    s32 semaphoreId;
    void* unusedPointer;

    writeData = &_send_data.writeData;
    
    iob = get_iob(fd);
    _sceFsWaitS(0x3);
    if (_fs_init == 0x0) {
        _sceFsSigSema();
        return -0x1; // errno.h says this would be "Not super-user" but that doesn't make sense
    }
    
    if ((iob == NULL) || (iob->i_flag == 0x0)) {
        _sceFsSigSema();
        return -EBADF;
    }
    
    fileFlags = iob->i_flag;
    writeData->fd = iob->i_fd;
    writeData->size = size;
    writeData->addr = (u32)buffer;
    writeData->ee_fds = iob - _iob;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    writeData->ee_semid = semaphoreId;
    writeData->ee_retadr = (u32)&writeResult;
    writeData->ee_retsiz = sizeof(writeResult);
    
    if ((fileFlags & 0x8000) != 0x0) {
        WaitSema(_fs_fsq_semid);
        for (queueIndex = 0; queueIndex < 32; queueIndex++) {
            if (_sceFs_q[queueIndex] == -1) {
                _sceFs_q[queueIndex] = writeData->ee_semid;
                writeData->ee_semid = -writeData->ee_semid;
                break;
            }
        }
        SignalSema(_fs_fsq_semid);
    }

    if (((u32)buffer & 15) == 0x0) {
        prefixSize = 0x0;
    } else {
        int prefixBase;
        prefixBase = ((u32)buffer - 0x10);
        prefixSize = ((u32)buffer / 16) * 16 - prefixBase;
    }
    
    if (size < prefixSize) {
        prefixSize = size;
    }
    if ((fileFlags & 0x20000000) == 0x0) {
        sceSifWriteBackDCache(buffer, size);
    }
    
    writeData->psize = prefixSize;
    buffer = (void*)UNCACHED(buffer);
    for (prefixIndex = 0; prefixIndex < prefixSize; prefixIndex++) {
        writeData->pdata[prefixIndex] = *(char*)(buffer + prefixIndex);
    }    
    
    rpcStatus = sceSifCallRpc(&_cd, 0x3, 0x0, &_send_data, sizeof(_sceFsWriteData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }

    if ((fileFlags & 0x8000) != 0x0) {
        DeleteSema(semaphoreId);
        return 0; // SCE_OK
    } 
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return writeResult;
}

int sceIoctl(int fd, int cmd, void* argument) {
    _sceFsIoctlData* ioctlData;
    _sceFsIob* iob;
    s32 rpcStatus;
    s32 ioctlResult;
    s32 unusedCount;
    s32 unusedWait;
    struct SemaParam semaParam;
    s32 queueIndex;
    s32 semaphoreId;
    s32 unusedSize;
    s32 rpcAccepted;

    rpcStatus = 1;
    ioctlData = &_send_data.ioctlData;

    iob = get_iob(fd);
    _sceFsWaitS(5);
    ip0 = argument;
    if (_fs_init == 0x0) {
        sceFsInit();
    }

    if ((iob == NULL) || (iob->i_flag == 0x0)) {
        _sceFsSigSema();
        return -EBADF;
    }

    ioctlData->ret_argadr = 0;
    ioctlData->ret_argsiz = 0;
    
    switch(cmd) {
        case 1:
            WaitSema(_fs_fsq_semid);
            queueIndex = 0;
            for (; queueIndex < 32 && (_sceFs_q[queueIndex] == -1); queueIndex++) {
            }
            if (queueIndex == 32) {
                *ip0 = 0x0;
            }
            else {
                *ip0 = 0x1;
            }
            SignalSema(_fs_fsq_semid);
            _sceFsSigSema();
            return 0x0;
        break;
        case 2:
            *(u32*)argument = *(u32*)UNCACHED(&_rcv_data_cmd.rcvIoctlData.ee_ret);
            _sceFsSigSema();
            return 0x0;
        break;
        case 3:
            *(u64*)argument = *(u64*)UNCACHED(&_rcv_data_cmd.rcvIoctlData.ee_ret);
            _sceFsSigSema();
            return 0x0;
        break;
    }
    
    ioctlData->fd = iob->i_fd;
    ioctlData->cmd = cmd;
    
    if (argument == NULL) {
        ioctlData->arglen = 0;
    } else {
        ioctlData->arglen = 0x400;
        memcpy(&ioctlData->arg, argument, 0x400);
    }
    
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    ioctlData->ee_semid = semaphoreId;
    ioctlData->ee_retadr = (u32)&ioctlResult;
    ioctlData->ee_retsiz = sizeof(ioctlResult);
    
    sceSifWriteBackDCache(&_send_data, sizeof(_sceFsIoctlData));
    rpcStatus = sceSifCallRpc(&_cd, 0x5, 0x0, &_send_data, sizeof(_sceFsIoctlData), &_rcv_data_rpc, 0x4, NULL, NULL);
    
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcAccepted = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcAccepted == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return ioctlResult;
}

int sceIoctl2(int fd, int cmd, const void* input, unsigned int inputLength, void* output, unsigned int outputLength) {
    _sceFsIoctlData* ioctlData;
    _sceFsIob* iob;
    s32 rpcStatus;
    s32 ioctlResult;
    struct SemaParam semaParam;
    s32 semaphoreId;
    
    ioctlData = &_send_data.ioctlData;

    iob = get_iob(fd);
    _sceFsWaitS(0x1A);
    if (_fs_init == 0x0) {
        sceFsInit();
    }

    if ((iob == NULL) || (iob->i_flag == 0x0)) {
        _sceFsSigSema();
        return -EBADF;
    }
    
    if ((inputLength > MAX_ARG_SIZE) || (outputLength > MAX_ARG_SIZE)) {
        _sceFsSigSema();
        return -EINVAL;
    }
    
    if (input == NULL) {
        ioctlData->arglen = 0;
    } else {
        memcpy(&ioctlData->arg, input, inputLength);
    }
    
    ioctlData->fd = iob->i_fd;
    ioctlData->cmd = cmd;
    ioctlData->arglen = inputLength;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    ioctlData->ee_semid = semaphoreId;
    ioctlData->ee_retadr = (u32)&ioctlResult;
    ioctlData->ee_retsiz = sizeof(ioctlResult);
    ioctlData->ret_argadr = output;
    ioctlData->ret_argsiz = outputLength;
    
    sceSifWriteBackDCache(&_send_data, sizeof(_sceFsIoctlData));
    rpcStatus = sceSifCallRpc(&_cd, 0x1a, 0x0, &_send_data, sizeof(_sceFsIoctlData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return ioctlResult;
}

static s32 _sceCallCode(const char* name, u32 callcode) {
    _sceFsNameData* nameData;
    s32 nameLength;
    s32 rpcStatus;
    s32 callResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    nameData = &_send_data.nameData;
    
    _sceFsWaitS(callcode);
    if (_fs_init == 0x0) {
        sceFsInit();
    }
    
    for (nameLength = 0; nameLength < MAX_ARG_SIZE && (nameData->name[nameLength] = name[nameLength]) != 0; nameLength++) { }
    if (nameLength == MAX_ARG_SIZE) {
        nameData->name[MAX_ARG_SIZE-1] = 0x0;
        nameLength = MAX_ARG_SIZE-1;
    }
    
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    nameData->ee_semid = semaphoreId;
    nameData->ee_retadr = (u32)&callResult;
    nameData->ee_retsiz = sizeof(callResult);
    
    rpcStatus = sceSifCallRpc(&_cd, callcode, 0x0, &_send_data, nameLength + 0xC + 1, &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return callResult;
}

/**
 * @brief Delete file
 * @param filename File pathname (includes device name and ':')
 * @return 0 on success, -1 * error code on failure 
 */
s32 sceRemove(const char* filename) {
    return _sceCallCode(filename, 6);
}

int sceMkdir(const char* name, int flag) {
    _sceFsMkdirData* mkdirData;
    s32 nameLength;
    s32 rpcStatus;
    s32 mkdirResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    mkdirData = &_send_data.mkdirData;
    
    _sceFsWaitS(0x7);
    if (_fs_init == 0x0) {
        sceFsInit();
    }

    for (nameLength = 0; nameLength < MAX_ARG_SIZE && (mkdirData->name[nameLength] = name[nameLength]) != 0; nameLength++) { }
    if (nameLength == MAX_ARG_SIZE) {
        mkdirData->name[MAX_ARG_SIZE-1] = 0x0;
        nameLength = MAX_ARG_SIZE-1;
    }
    
    mkdirData->flag = flag;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    mkdirData->ee_semid = semaphoreId;
    mkdirData->ee_retadr = (u32)&mkdirResult;
    mkdirData->ee_retsiz = sizeof(mkdirResult);
    
    rpcStatus = sceSifCallRpc(&_cd, 0x7, 0x0, &_send_data, nameLength + 0x10 + 1, &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return mkdirResult;
}

/**
 * @brief Delete directory
 * @param dirname Directory pathname (includes device name and ':')
 * @return 0 on success, -1 * error code on failure
 */
s32 sceRmdir(const char* dirname) {
    return _sceCallCode(dirname, 8);
}

int sceFormat(const char* path, const char* blkdevname, void* argument, int argumentLength) {
    _sceFsFormatData* formatData;
    s32 copyIndex;
    s32 rpcStatus;
    s32 formatResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    formatData = &_send_data.formatData;
    
    _sceFsWaitS(0xE);
    if (_fs_init == 0x0) {
        sceFsInit();
    }

    for (copyIndex = 0; copyIndex < MAX_ARG_SIZE && (formatData->path[copyIndex] = path[copyIndex]) != 0; copyIndex++) { }
    if (copyIndex == MAX_ARG_SIZE) {
        formatData->path[MAX_ARG_SIZE-1] = 0x0;
    }

    if (blkdevname == NULL) {
        formatData->blkdevname[0] = 0x0;
    } else {   
        for (copyIndex = 0; copyIndex < MAX_ARG_SIZE && (formatData->blkdevname[copyIndex] = blkdevname[copyIndex]) != 0; copyIndex++) { }
        if (copyIndex == MAX_ARG_SIZE) {
            formatData->blkdevname[MAX_ARG_SIZE-1] = 0x0;
        }
    }

    if (argumentLength > MAX_ARG_SIZE) {
        _sceFsSigSema();
        return -E2BIG;
    }

    for (copyIndex = 0; copyIndex < argumentLength; copyIndex++) {
        formatData->arg[copyIndex] = ((char*)argument)[copyIndex];
    }

    formatData->arglen = argumentLength;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    formatData->ee_semid = semaphoreId;
    formatData->ee_retadr = (u32)&formatResult;
    formatData->ee_retsiz = sizeof(formatResult);

    sceSifWriteBackDCache(&_send_data, sizeof(_sceFsFormatData));
    rpcStatus = sceSifCallRpc(&_cd, 0xE, 0x0, &_send_data, sizeof(_sceFsFormatData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return formatResult;
}

int sceAddDrv(void* driverAddress) {
    _sceFsAddrData* addressData;
    s32 unusedNameSize;
    s32 rpcStatus;
    s32 addDriverResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    addressData = &_send_data.addrData;
    
    _sceFsWaitS(0xF);
    if (_fs_init == 0x0) {
        sceFsInit();
    }

    addressData->addr = driverAddress;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    addressData->ee_semid = semaphoreId;
    addressData->ee_retadr = (u32)&addDriverResult;
    addressData->ee_retsiz = sizeof(addDriverResult);

    rpcStatus = sceSifCallRpc(&_cd, 0xF, 0x0, &_send_data, sizeof(_sceFsAddrData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -1;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -1;
    }
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return addDriverResult;
}

s32 sceDelDrv(const char* name) {
    return _sceCallCode(name, 16);
}

int sceDopen(const char* name) {
    _sceFsIob* iob;
    s32 descriptor;
    
    _sceFsWaitS(0x9);
    if (_fs_init == 0x0) {
        sceFsInit();
    }
    
    _sceFsSigSema();
    iob = new_iob();
    if (iob == NULL) {
        return -ENODEV;
    }
    
    descriptor = _sceCallCode(name, 0x9);
    if (descriptor < 0x0) {
        WaitSema(_fs_iob_semid);
        iob->i_flag = 0x0;
        SignalSema(_fs_iob_semid);
        return descriptor;
    }
        
    WaitSema(_fs_iob_semid);
    iob->i_fd = descriptor;
    descriptor = iob - _iob;
    SignalSema(_fs_iob_semid);
    
    return descriptor;
}

int sceDclose(int fd) {
    _sceFsCloseData* closeData;
    _sceFsIob* iob;
    s32 rpcStatus;
    s32 closeResult;
    struct SemaParam semaParam;
    s32 semaphoreId;
    s32 errorThreshold;

    iob = get_iob(fd);
    closeData = &_send_data.closeData;
    
    _sceFsWaitS(0xA);
    if (_fs_init == 0x0) {
        _sceFsSigSema();
        return -1;
    }

    if ((iob == NULL) || (iob->i_flag == 0x0)) {
        _sceFsSigSema();
        return -EBADF;
    }

    closeData->fd = iob->i_fd;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    closeData->ee_semid = semaphoreId;
    closeData->ee_retadr = (u32)&closeResult;
    closeData->ee_retsiz = sizeof(closeResult);

    rpcStatus = sceSifCallRpc(&_cd, 0xA, 0x0, &_send_data, sizeof(_sceFsCloseData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    iob->i_flag = 0;
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }

    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    errorThreshold = -1;
    if (errorThreshold < closeResult) {
        return 0;
    }
    return closeResult;
}

int sceDread(int fd, struct sce_dirent* dirEntry) {
    _sceFsReadData* readData;
    _sceFsIob* iob;
    s32 rpcStatus;
    s32 readResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    iob = get_iob(fd);
    readData = &_send_data.readData;
    
    _sceFsWaitS(0xB);
    if (_fs_init == 0x0) {
        _sceFsSigSema();
        return -1;
    }

    if ((iob == NULL) || (iob->i_flag == 0x0)) {
        _sceFsSigSema();
        return -EBADF;
    }

    readData->fd = iob->i_fd;
    readData->addr = (u32)dirEntry;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    readData->ee_semid = semaphoreId;
    readData->ee_retadr = (u32)&readResult;
    readData->ee_retsiz = sizeof(readResult);

    rpcStatus = sceSifCallRpc(&_cd, 0xB, 0x0, &_send_data, sizeof(_sceFsReadData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        WaitSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }

    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return readResult;
}

int sceGetstat(const char* name, struct sce_stat* stat) {
    _sceFsGStatData* statData;
    s32 rpcStatus;
    s32 nameLength;
    s32 getStatResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    statData = &_send_data.gStatData;
    
    _sceFsWaitS(0xC);
    if (_fs_init == 0x0) {
        sceFsInit();
    }

    for (nameLength = 0; nameLength < MAX_ARG_SIZE && (statData->name[nameLength] = name[nameLength]) != 0; nameLength++) { }
    if (nameLength == MAX_ARG_SIZE) {
        statData->name[MAX_ARG_SIZE-1] = 0x0;
        nameLength = MAX_ARG_SIZE-1;
    }

    statData->addr = stat;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    statData->ee_semid = semaphoreId;
    statData->ee_retadr = (u32)&getStatResult;
    statData->ee_retsiz = sizeof(getStatResult);

    rpcStatus = sceSifCallRpc(&_cd, 0xC, 0x0, &_send_data, nameLength + 0x10 + 1, &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }

    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return getStatResult;
}

int sceChstat(const char* name, struct sce_stat* stat, unsigned int cbit) {
    _sceFsCStatData* statData;
    s32 rpcStatus;
    s32 nameLength;
    s32 changeStatResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    statData = &_send_data.cStatData;
    
    _sceFsWaitS(0xD);
    if (_fs_init == 0x0) {
        sceFsInit();
    }

    for (nameLength = 0; nameLength < MAX_ARG_SIZE && (statData->name[nameLength] = name[nameLength]) != 0; nameLength++) { }
    if (nameLength == MAX_ARG_SIZE) {
        statData->name[MAX_ARG_SIZE-1] = 0x0;
        nameLength = MAX_ARG_SIZE-1;
    }

    statData->stat = *stat;
    statData->cbit = cbit;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    statData->ee_semid = semaphoreId;
    statData->ee_retadr = (u32)&changeStatResult;
    statData->ee_retsiz = sizeof(changeStatResult);
    
    sceSifWriteBackDCache(&_send_data, sizeof(_sceFsCStatData));
    rpcStatus = sceSifCallRpc(&_cd, 0xD, 0x0, &_send_data, nameLength + 0x50 + 1, &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }

    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return changeStatResult;
}

int sceRename(const char* oldname, const char* newname) {
    _sceFsRenameData* renameData;
    s32 rpcStatus;
    s32 pathLength;
    s32 renameResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    renameData = &_send_data.renameData;
    
    _sceFsWaitS(0x11);
    if (_fs_init == 0x0) {
        sceFsInit();
    }

    for (pathLength = 0; pathLength < MAX_ARG_SIZE && (renameData->oldpath[pathLength] = oldname[pathLength]) != 0; pathLength++) { }
    if (pathLength == MAX_ARG_SIZE) {
        renameData->oldpath[MAX_ARG_SIZE-1] = 0x0;
    }
    
    for (pathLength = 0; pathLength < MAX_ARG_SIZE && (renameData->newpath[pathLength] = newname[pathLength]) != 0; pathLength++) { }
    if (pathLength == MAX_ARG_SIZE) {
        renameData->newpath[MAX_ARG_SIZE-1] = 0x0;
    }

    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    renameData->ee_semid = semaphoreId;
    renameData->ee_retadr = (u32)&renameResult;
    renameData->ee_retsiz = sizeof(renameResult);
    
    sceSifWriteBackDCache(&_send_data, sizeof(_sceFsRenameData));
    rpcStatus = sceSifCallRpc(&_cd, 0x11, 0x0, &_send_data, sizeof(_sceFsRenameData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }

    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return renameResult;
}

/**
 * @brief Change current directory
 * @param name filepath name
 * @return 0 on success, -1 * error code on failure
 */
s32 sceChdir(const char* name) {
    return _sceCallCode(name, 18);
}

int sceSync(const char* path, int flag) {
    _sceFsSyncData* syncData;
    s32 unusedPathLength;
    s32 rpcStatus;
    s32 syncResult;
    struct SemaParam semaParam;
    s32 pathLength;
    s32 semaphoreId;

    syncData = &_send_data.syncData;
    
    _sceFsWaitS(0x13);
    if (_fs_init == 0x0) {
        sceFsInit();
    }
    
    for (pathLength = 0; pathLength < MAX_ARG_SIZE && (syncData->path[pathLength] = path[pathLength]) != 0; pathLength++) { }
    if (pathLength == MAX_ARG_SIZE) {
        syncData->path[MAX_ARG_SIZE-1] = 0x0;
    }
    
    syncData->flag = flag;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    syncData->ee_semid = semaphoreId;
    syncData->ee_retadr = (u32)&syncResult;
    syncData->ee_retsiz = sizeof(syncResult);
    
    rpcStatus = sceSifCallRpc(&_cd, 0x13, 0x0, &_send_data, sizeof(_sceFsSyncData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return syncResult;
}

int sceMount(const char* fsdevname, const char* blkdevname, int flag, void* argument, int argumentLength) {
    _sceFsMountData* mountData;
    s32 copyIndex;
    s32 rpcStatus;
    s32 mountResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    mountData = &_send_data.mountData;
    
    _sceFsWaitS(0x14);
    if (_fs_init == 0x0) {
        sceFsInit();
    }
    
    for (copyIndex = 0; copyIndex < MAX_ARG_SIZE && (mountData->fsdevname[copyIndex] = fsdevname[copyIndex]) != 0; copyIndex++) { }
    if (copyIndex == MAX_ARG_SIZE) {
        mountData->fsdevname[MAX_ARG_SIZE-1] = 0x0;
    }
    for (copyIndex = 0; copyIndex < MAX_ARG_SIZE && (mountData->blkdevname[copyIndex] = blkdevname[copyIndex]) != 0; copyIndex++) { }
    if (copyIndex == MAX_ARG_SIZE) {
        mountData->blkdevname[MAX_ARG_SIZE-1] = 0x0;
    }

    if (argumentLength > MAX_ARG_SIZE) {
        _sceFsSigSema();
        return -E2BIG;
    }

    for (copyIndex = 0; copyIndex < argumentLength; copyIndex++) {
        mountData->arg[copyIndex] = ((char*)argument)[copyIndex];
    }
    
    mountData->flag = flag;
    mountData->arglen = argumentLength;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    mountData->ee_semid = semaphoreId;
    mountData->ee_retadr = (u32)&mountResult;
    mountData->ee_retsiz = sizeof(mountResult);
    
    sceSifWriteBackDCache(&_send_data, sizeof(_sceFsMountData));
    rpcStatus = sceSifCallRpc(&_cd, 0x14, 0x0, &_send_data, sizeof(_sceFsMountData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return mountResult;
}

/**
 * @brief Unmount filesystem
 * @param name string specifying mounted filesystem device name and unit number
 * @return 0 on success, -1 * error code on failure
 */
s32 sceUmount(const char* name) {
    return _sceCallCode(name, 21);
}

long sceLseek64(int fd, long offset, int how) {
    _sceFsLseek64Data* seekData;
    _sceFsIob* iob;
    s32 unusedCount;
    s32 rpcStatus;
    s64 seekResult;
    s32 queueIndex;
    s32 fileFlags;
    struct SemaParam semaParam;
    s32 semaphoreId;

    seekData = &_send_data.lSeek64Data;
    
    iob = get_iob(fd);
    _sceFsWaitS(0x16);
    if (_fs_init == 0x0) {
        _sceFsSigSema();
        return -0x1; // errno.h says this would be "Not super-user" but that doesn't make sense
    }
    
    if ((iob == NULL) || (iob->i_flag == 0x0)) {
        _sceFsSigSema();
        return -EBADF;
    }
    
    fileFlags = iob->i_flag;
    seekData->fd = iob->i_fd;
    seekData->offset = offset;
    seekData->how = how;
    seekData->ee_fds = iob - _iob;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    seekData->ee_semid = semaphoreId;
    seekData->ee_retadr = (u32)&seekResult;
    seekData->ee_retsiz = sizeof(seekResult);
        
    if ((fileFlags & 0x8000) != 0x0) {
        WaitSema(_fs_fsq_semid);
        for (queueIndex = 0; queueIndex < 32; queueIndex++) {
            if (_sceFs_q[queueIndex] == -1) {
                _sceFs_q[queueIndex] = seekData->ee_semid;
                seekData->ee_semid = -seekData->ee_semid;
                break;
            }
        }
        SignalSema(_fs_fsq_semid);
    }
    
    rpcStatus = sceSifCallRpc(&_cd, 0x16, 0x0, &_send_data, sizeof(_sceFsLseek64Data), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }

    if ((fileFlags & 0x8000) != 0x0) {
        DeleteSema(semaphoreId);
        return 0; // SCE_OK
    } 
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return seekResult;
}

int sceDevctl(const char* devname, int cmd, const void* input, unsigned int inputLength, void* output, unsigned int outputLength) {
    _sceFsDevctlData* devctlData;
    s32 copyIndex;
    s32 rpcStatus;
    s32 devctlResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    devctlData = &_send_data.devctlData;
    
    _sceFsWaitS(0x17);
    if (_fs_init == 0x0) {
        sceFsInit();
    }

    for (copyIndex = 0; copyIndex < MAX_ARG_SIZE && (devctlData->path[copyIndex] = devname[copyIndex]) != 0; copyIndex++) { }
    if (copyIndex == MAX_ARG_SIZE) {
        devctlData->path[MAX_ARG_SIZE-1] = 0x0;
    }

    if ((inputLength > MAX_ARG_SIZE) || (outputLength > MAX_ARG_SIZE)) {
        _sceFsSigSema();
        return -EINVAL;
    }

    for (copyIndex = 0; copyIndex < inputLength; copyIndex++) {
        devctlData->arg_buf[copyIndex] = ((char*)input)[copyIndex];
    }
    
    devctlData->cmd = cmd;
    devctlData->arglen = inputLength;
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    devctlData->ee_semid = semaphoreId;
    devctlData->ee_retadr = (u32)&devctlResult;
    devctlData->ee_retsiz = sizeof(devctlResult);
    devctlData->ret_argadr = output;
    devctlData->ret_arglen = outputLength;
    
    sceSifWriteBackDCache(&_send_data, sizeof(_sceFsDevctlData));
    rpcStatus = sceSifCallRpc(&_cd, 0x17, 0x0, &_send_data, sizeof(_sceFsDevctlData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return devctlResult;
}

int sceSymlink(const char* existing, const char* new) {
    _sceFsSymlinkData* symlinkData;
    s32 pathLength;
    s32 rpcStatus;
    s32 linkResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    symlinkData = &_send_data.symlinkData;
    
    _sceFsWaitS(0x11);
    if (_fs_init == 0x0) {
        sceFsInit();
    }

    for (pathLength = 0; pathLength < MAX_ARG_SIZE && (symlinkData->existing[pathLength] = existing[pathLength]) != 0; pathLength++) { }
    if (pathLength == MAX_ARG_SIZE) {
        symlinkData->existing[MAX_ARG_SIZE-1] = 0x0;
    }
    
    for (pathLength = 0; pathLength < MAX_ARG_SIZE && (symlinkData->new[pathLength] = new[pathLength]) != 0; pathLength++) { }
    if (pathLength == MAX_ARG_SIZE) {
        symlinkData->new[MAX_ARG_SIZE-1] = 0x0;
    }

    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    symlinkData->ee_semid = semaphoreId;
    symlinkData->ee_retadr = (u32)&linkResult;
    symlinkData->ee_retsiz = sizeof(linkResult);
    
    rpcStatus = sceSifCallRpc(&_cd, 0x18, 0x0, &_send_data, sizeof(_sceFsSymlinkData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return linkResult;
}

int sceReadlink(const char* path, char* buffer, unsigned int bufferSize) {
    _sceFsReadlinkData* readlinkData;
    s32 pathLength;
    s32 rpcStatus;
    s32 linkResult;
    struct SemaParam semaParam;
    s32 semaphoreId;

    readlinkData = &_send_data.readLinkData;
    
    _sceFsWaitS(0x11);
    if (_fs_init == 0x0) {
        sceFsInit();
    }

    for (pathLength = 0; pathLength < MAX_ARG_SIZE && (readlinkData->path[pathLength] = path[pathLength]) != 0; pathLength++) { }
    
    if (pathLength == MAX_ARG_SIZE) {
        readlinkData->path[MAX_ARG_SIZE-1] = 0x0;
    }

    if (bufferSize >= MAX_ARG_SIZE) {
        bufferSize = MAX_ARG_SIZE-1;
    }

    readlinkData->bufsize = bufferSize;
    readlinkData->bufaddr = (u32)buffer;
    sceSifWriteBackDCache(buffer, bufferSize);
    semaParam.maxCount = 0x1;
    semaParam.initCount = 0x0;
    semaParam.option = 0x0;
    semaphoreId = CreateSema(&semaParam);
    readlinkData->ee_semid = semaphoreId;
    readlinkData->ee_retadr = (u32)&linkResult;
    readlinkData->ee_retsiz = sizeof(linkResult);
    
    rpcStatus = sceSifCallRpc(&_cd, 0x19, 0x0, &_send_data, sizeof(_sceFsSymlinkData), &_rcv_data_rpc, sizeof(_rcv_data_rpc), NULL, NULL);
    if (rpcStatus < 0x0) {
        DeleteSema(semaphoreId);
        _sceFsSigSema();
        return -EAGAIN;
    }
   
    rpcStatus = *(u32*)UNCACHED(&_rcv_data_rpc);
    _sceFsSigSema();
    if (rpcStatus == 0x0) {
        DeleteSema(semaphoreId);
        return -EAGAIN;
    }
    
    WaitSema(semaphoreId);
    DeleteSema(semaphoreId);
    return linkResult;
}
