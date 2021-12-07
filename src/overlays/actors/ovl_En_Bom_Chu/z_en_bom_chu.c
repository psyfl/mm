/*
 * File: z_en_bom_chu.c
 * Overlay: ovl_En_Bom_Chu
 * Description: Bombchus
 */

#include "z_en_bom_chu.h"

#define FLAGS 0x00000010

#define THIS ((EnBomChu*)thisx)

void EnBomChu_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBomChu_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBomChu_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBomChu_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808F7868(EnBomChu* this, GlobalContext* globalCtx);
void func_808F7A84(EnBomChu* this, GlobalContext* globalCtx);
void func_808F7FA0(EnBomChu* this, GlobalContext* globalCtx);

#if 0
const ActorInit En_Bom_Chu_InitVars = {
    ACTOR_EN_BOM_CHU,
    ACTORCAT_EXPLOSIVES,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(EnBomChu),
    (ActorFunc)EnBomChu_Init,
    (ActorFunc)EnBomChu_Destroy,
    (ActorFunc)EnBomChu_Update,
    (ActorFunc)EnBomChu_Draw,
};

// static ColliderSphereInit sSphereInit = {
static ColliderSphereInit D_808F88E0 = {
    { COLTYPE_NONE, AT_NONE, AC_ON | AC_TYPE_PLAYER, OC1_ON | OC1_TYPE_1 | OC1_TYPE_2, OC2_TYPE_2, COLSHAPE_SPHERE, },
    { ELEMTYPE_UNK0, { 0x00000000, 0x00, 0x00 }, { 0xF7CFFFFF, 0x00, 0x00 }, TOUCH_NONE | TOUCH_SFX_NORMAL, BUMP_ON, OCELEM_ON, },
    { 1, { { 0, 0, 0 }, 13 }, 100 },
};

// static InitChainEntry sInitChain[] = {
static InitChainEntry D_808F890C[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

#endif

extern ColliderSphereInit D_808F88E0;
extern InitChainEntry D_808F890C[];

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/EnBomChu_Init.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/EnBomChu_Destroy.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/func_808F75D0.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/func_808F77E4.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/func_808F7868.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/func_808F7944.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/func_808F79D4.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/func_808F7A84.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/func_808F7E74.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/func_808F7FA0.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/func_808F7FD0.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/func_808F8080.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/func_808F818C.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/EnBomChu_Update.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Bom_Chu/EnBomChu_Draw.s")
