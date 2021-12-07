/*
 * File: z_en_baba.c
 * Overlay: ovl_En_Baba
 * Description: Bomb Shop Lady
 */

#include "z_en_baba.h"

#define FLAGS 0x00000019

#define THIS ((EnBaba*)thisx)

void EnBaba_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBaba_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBaba_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBaba_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80BA9480(EnBaba* this, GlobalContext* globalCtx);
void func_80BA9758(EnBaba* this, GlobalContext* globalCtx);
void func_80BA9848(EnBaba* this, GlobalContext* globalCtx);
void func_80BA98EC(EnBaba* this, GlobalContext* globalCtx);
void func_80BA9AB8(EnBaba* this, GlobalContext* globalCtx);
void func_80BA9B24(EnBaba* this, GlobalContext* globalCtx);
void func_80BA9B80(EnBaba* this, GlobalContext* globalCtx);
void func_80BA9CD4(EnBaba* this, GlobalContext* globalCtx);
void func_80BA9E00(EnBaba* this, GlobalContext* globalCtx);
void func_80BA9E10(EnBaba* this, GlobalContext* globalCtx);
void func_80BA9E48(EnBaba* this, GlobalContext* globalCtx);

#if 0
const ActorInit En_Baba_InitVars = {
    ACTOR_EN_BABA,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_BBA,
    sizeof(EnBaba),
    (ActorFunc)EnBaba_Init,
    (ActorFunc)EnBaba_Destroy,
    (ActorFunc)EnBaba_Update,
    (ActorFunc)EnBaba_Draw,
};

// static ColliderCylinderInit sCylinderInit = {
static ColliderCylinderInit D_80BAA430 = {
    { COLTYPE_NONE, AT_NONE, AC_ON | AC_TYPE_PLAYER, OC1_ON | OC1_TYPE_ALL, OC2_TYPE_1, COLSHAPE_CYLINDER, },
    { ELEMTYPE_UNK1, { 0x00000000, 0x00, 0x00 }, { 0xF7CFFFFF, 0x00, 0x00 }, TOUCH_NONE | TOUCH_SFX_NORMAL, BUMP_ON, OCELEM_ON, },
    { 18, 64, 0, { 0, 0, 0 } },
};

// sColChkInfoInit
static CollisionCheckInfoInit2 D_80BAA45C = { 0, 0, 0, 0, MASS_IMMOVABLE };

// static DamageTable sDamageTable = {
static DamageTable D_80BAA468 = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0x0),
    /* Ice arrow      */ DMG_ENTRY(1, 0x0),
    /* Light arrow    */ DMG_ENTRY(1, 0x0),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x0),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(1, 0x0),
    /* Normal Roll    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(1, 0x0),
    /* Unblockable    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

#endif

extern ColliderCylinderInit D_80BAA430;
extern CollisionCheckInfoInit2 D_80BAA45C;
extern DamageTable D_80BAA468;

extern UNK_TYPE D_06006B10;
extern UNK_TYPE D_060092A0;

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA8820.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA886C.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA8C4C.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA8C90.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA8D2C.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA8DF4.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA8F88.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9110.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9160.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA93AC.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9480.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9758.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9848.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA98EC.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9AB8.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9B24.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9B80.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9CD4.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9E00.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9E10.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9E48.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/EnBaba_Init.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/EnBaba_Destroy.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/EnBaba_Update.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BA9FB0.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BAA198.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/func_80BAA20C.s")

#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Baba/EnBaba_Draw.s")
