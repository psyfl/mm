/*
 * File: z_en_rg.c
 * Overlay: ovl_En_Rg
 * Description: Racing Goron
 */

#include "z_en_rg.h"

#define FLAGS 0x80000010

#define THIS ((EnRg*)thisx)

void EnRg_Init(Actor* thisx, GlobalContext* globalCtx);
void EnRg_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnRg_Update(Actor* thisx, GlobalContext* globalCtx);
void EnRg_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80BF4EBC(EnRg* this, GlobalContext* globalCtx);
void func_80BF4FC4(EnRg* this, GlobalContext* globalCtx);

#if 0
const ActorInit En_Rg_InitVars = {
    ACTOR_EN_RG,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_OF1D_MAP,
    sizeof(EnRg),
    (ActorFunc)EnRg_Init,
    (ActorFunc)EnRg_Destroy,
    (ActorFunc)EnRg_Update,
    (ActorFunc)EnRg_Draw,
};

// static ColliderSphereInit sSphereInit = {
static ColliderSphereInit D_80BF5760 = {
    { COLTYPE_METAL, AT_ON | AT_TYPE_PLAYER | AT_TYPE_ENEMY, AC_ON | AC_HARD | AC_TYPE_PLAYER, OC1_ON | OC1_TYPE_ALL, OC2_TYPE_1, COLSHAPE_SPHERE, },
    { ELEMTYPE_UNK0, { 0xF7CFFFFF, 0x00, 0x00 }, { 0xF7CFFFFF, 0x00, 0x00 }, TOUCH_ON | TOUCH_SFX_NORMAL, BUMP_ON, OCELEM_ON, },
    { 0, { { 0, 0, 0 }, 20 }, 100 },
};

// static ColliderCylinderInit sCylinderInit = {
static ColliderCylinderInit D_80BF578C = {
    { COLTYPE_HIT1, AT_NONE, AC_NONE, OC1_ON | OC1_TYPE_ALL, OC2_TYPE_1, COLSHAPE_CYLINDER, },
    { ELEMTYPE_UNK1, { 0x00000000, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 }, TOUCH_NONE | TOUCH_SFX_NORMAL, BUMP_NONE, OCELEM_ON, },
    { 20, 62, 0, { 0, 0, 0 } },
};

// sColChkInfoInit
static CollisionCheckInfoInit2 D_80BF57B8 = { 0, 0, 0, 0, MASS_IMMOVABLE };

// static DamageTable sDamageTable = {
static DamageTable D_80BF57C4 = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0xF),
    /* Fire arrow     */ DMG_ENTRY(1, 0x0),
    /* Ice arrow      */ DMG_ENTRY(1, 0x0),
    /* Light arrow    */ DMG_ENTRY(1, 0x0),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

#endif

extern ColliderSphereInit D_80BF5760;
extern ColliderCylinderInit D_80BF578C;
extern CollisionCheckInfoInit2 D_80BF57B8;
extern DamageTable D_80BF57C4;

extern UNK_TYPE D_060091A8;
extern UNK_TYPE D_06011AC8;
extern UNK_TYPE D_06014D00;

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF3920.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF3C64.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF3DA0.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF3DC4.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF3E88.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF3ED4.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF3F14.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF3FF8.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF4024.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF409C.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF40F4.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF416C.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF4220.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF42BC.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF43FC.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF4560.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF45B4.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF47AC.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF4934.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF4964.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF4AB8.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF4D64.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF4DA8.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF4EBC.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF4FC4.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/EnRg_Init.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/EnRg_Destroy.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/EnRg_Update.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF547C.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF5588.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/EnRg_Draw.s")
