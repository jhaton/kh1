#include "ppp.h"

typedef struct {
    /* 0x0 */ pppCDT cdt;
    /* 0x4 */ s32 param;
} PDrawHook;

typedef struct {
    /* 0x0 */ s32 param;
} VDrawHook;

void pppDrawHookDraw(pppPObject* pobj, PDrawHook* params, pppCtrlTable* ctbl) {
    pppDrawFunc hook;
    
    ((VDrawHook*)&pobj->val[ctbl->useVal[0]])->param = params->param;

    if (ppvHookFuncTbl != NULL) {
        hook = ppvHookFuncTbl[pobj->pdtval->pdtno];
        if (hook != NULL) {
            hook(pobj, params->param);
        }
    }
}
