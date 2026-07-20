#include "ppp.h"

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ u8 bFollowCam;
    /* 0x5 */ u8 bStop;
    /* 0x6 */ u8 bDrawPriority;
} PRyjMngFlag;

void pppRyjMngFlagCalc(pppPObject* pobj, PRyjMngFlag* flags, pppCtrlTable* ctbl) {
    if (flags->cdt.time == pobj->time) {
        ppvMng->followCam = flags->bFollowCam;
        ppvMng->stop = flags->bStop;
        ppvMng->drawPriority = flags->bDrawPriority;
    }
}

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMngFlag", func_0019F698);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMngFlag", func_0019F810);
