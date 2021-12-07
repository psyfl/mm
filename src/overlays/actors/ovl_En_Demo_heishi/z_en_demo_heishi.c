/*
 * File: z_en_demo_heishi.c
 * Overlay: ovl_En_Demo_heishi
 * Description: Unused(?) version of Shiro
 */

#include "z_en_demo_heishi.h"

#define FLAGS 0x00000009

#define THIS ((EnDemoheishi*)thisx)

void EnDemoheishi_Init(Actor* thisx, GlobalContext* globalCtx);
void EnDemoheishi_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnDemoheishi_Update(Actor* thisx, GlobalContext* globalCtx);
void EnDemoheishi_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80BE975C(EnDemoheishi* this, GlobalContext* globalCtx);
void func_80BE980C(EnDemoheishi* this, GlobalContext* globalCtx);

#if 0
const ActorInit En_Demo_heishi_InitVars = {
    ACTOR_EN_DEMO_HEISHI,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_SDN,
    sizeof(EnDemoheishi),
    (ActorFunc)EnDemoheishi_Init,
    (ActorFunc)EnDemoheishi_Destroy,
    (ActorFunc)EnDemoheishi_Update,
    (ActorFunc)EnDemoheishi_Draw,
};

// static ColliderCylinderInit sCylinderInit = {
static ColliderCylinderInit D_80BE9A50 = {
    { COLTYPE_NONE, AT_NONE, AC_NONE, OC1_ON | OC1_TYPE_ALL, OC2_TYPE_2, COLSHAPE_CYLINDER, },
    { ELEMTYPE_UNK0, { 0x00000000, 0x00, 0x00 }, { 0xF7CFFFFF, 0x00, 0x00 }, TOUCH_NONE | TOUCH_SFX_NORMAL, BUMP_NONE, OCELEM_ON, },
    { 40, 40, 0, { 0, 0, 0 } },
};

#endif

extern ColliderCylinderInit D_80BE9A50;

extern UNK_TYPE D_06003BFC;

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Demo_heishi/EnDemoheishi_Init.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Demo_heishi/EnDemoheishi_Destroy.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Demo_heishi/func_80BE95EC.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Demo_heishi/func_80BE9678.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Demo_heishi/func_80BE970C.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Demo_heishi/func_80BE975C.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Demo_heishi/func_80BE97F0.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Demo_heishi/func_80BE980C.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Demo_heishi/EnDemoheishi_Update.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Demo_heishi/func_80BE9974.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Demo_heishi/EnDemoheishi_Draw.s")
