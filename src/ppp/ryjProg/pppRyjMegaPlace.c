#include "ppp.h"

typedef struct {
    /* 0x00 */ u32 unShapeNum;
    /* 0x04 */ u32 unModelNum;
    /* 0x08 */ u32 unLampNum;
    /* 0x0C */ void* pvShape;
    /* 0x10 */ void* pvModel;
    /* 0x14 */ void* pvLamp;
} VRyjMegaPlace;

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaPlace", pppRyjMegaPlaceCalc);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaPlace", func_001A0140);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaPlace", func_001A0460);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaPlace", func_001A07A8);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaPlace", pppRyjMegaPlaceDraw);

INCLUDE_ASM("asm/nonmatchings/ppp/ryjProg/pppRyjMegaPlace", pppRyjMegaPlaceCon);

void pppRyjMegaPlaceDes(pppPObject* pobj, pppCtrlTable* ctbl) {
    VRyjMegaPlace* megaPlace = (VRyjMegaPlace*)&pobj->val[ctbl->useVal[0]];

    if (megaPlace->pvShape != NULL) {
        pppFree(ppvEnv, megaPlace->pvShape);
        megaPlace->pvShape = NULL;
    }
    if (megaPlace->pvModel != NULL) {
        pppFree(ppvEnv, megaPlace->pvModel);
        megaPlace->pvModel = NULL;
    }
    if (megaPlace->pvLamp != NULL) {
        pppFree(ppvEnv, megaPlace->pvLamp);
        megaPlace->pvLamp = NULL;
    }
}
