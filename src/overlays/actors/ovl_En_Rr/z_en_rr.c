/*
 * File: z_en_rr.c
 * Overlay: ovl_En_Rr
 * Description: Like Like
 */

#include "z_en_rr.h"

#define FLAGS 0x00000405

#define THIS ((EnRr*)thisx)

void EnRr_Init(Actor* thisx, GlobalContext* globalCtx);
void EnRr_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnRr_Update(Actor* thisx, GlobalContext* globalCtx);
void EnRr_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808FAF94(EnRr* this, GlobalContext* globalCtx);
void func_808FB088(EnRr* this, GlobalContext* globalCtx);
void func_808FB1C0(EnRr* this, GlobalContext* globalCtx);
void func_808FB2C0(EnRr* this, GlobalContext* globalCtx);
void func_808FB42C(EnRr* this, GlobalContext* globalCtx);
void func_808FB680(EnRr* this, GlobalContext* globalCtx);
void func_808FB710(EnRr* this, GlobalContext* globalCtx);

void func_808FAD1C(EnRr* this, GlobalContext* globalCtx);
void func_808FB398(EnRr* this, GlobalContext* globalCtx);

extern Gfx D_06000470[];

const ActorInit En_Rr_InitVars = {
    ACTOR_EN_RR,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_RR,
    sizeof(EnRr),
    (ActorFunc)EnRr_Init,
    (ActorFunc)EnRr_Destroy,
    (ActorFunc)EnRr_Update,
    (ActorFunc)EnRr_Draw,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x20000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 45, 60, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x20000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_NONE,
    },
    { 30, 45, -30, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(3, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x1),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xE),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(3, 0x0),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(2, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(2, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x1),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(2, 0x5),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(2, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

static CollisionCheckInfoInit sColChkInfoInit = { 3, 45, 60, 250 };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, 55, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

void EnRr_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnRr* this = THIS;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitAndSetCylinder(globalCtx, &this->collider1, &this->actor, &sCylinderInit1);
    Collider_InitAndSetCylinder(globalCtx, &this->collider2, &this->actor, &sCylinderInit2);
    if (this->actor.params != ENRR_3) {
        this->actor.scale.y = 0.015f;
        this->actor.scale.x = 0.019f;
        this->actor.scale.z = 0.019f;
    } else {
        this->actor.scale.y = 0.022499999f;
        this->actor.scale.x = 0.028499998f;
        this->actor.scale.z = 0.028499998f;
        this->collider1.dim.radius = this->collider1.dim.radius * 1.5f;
        this->collider1.dim.height = this->collider1.dim.height * 1.5f;
        this->collider2.dim.radius = this->collider2.dim.radius * 1.5f;
        this->collider2.dim.height = this->collider2.dim.height * 1.5f;
        this->collider2.dim.yShift = this->collider2.dim.yShift * 1.5f;
    }

    Collider_UpdateCylinder(&this->actor, &this->collider2);
    Actor_SetFocus(&this->actor, this->actor.scale.y * 2000.0f);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    if ((this->actor.params == ENRR_2) || (this->actor.params == ENRR_3)) {
        this->actor.colChkInfo.health = 6;
        if (this->actor.params == ENRR_2) {
            this->actor.colChkInfo.mass = MASS_HEAVY;
        }
    }

    this->actionFunc = func_808FAF94;
    func_808FAD1C(this, globalCtx);
}

void EnRr_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnRr* this = THIS;

    Collider_DestroyCylinder(globalCtx, &this->collider1);
    Collider_DestroyCylinder(globalCtx, &this->collider2);
}

void func_808FA01C(EnRr* this, GlobalContext* globalCtx, ColliderCylinder* collider) {
    if (this->actor.colChkInfo.damageEffect == 2) {
        this->unk_220 = 0.85f;
        this->unk_21C = 4.0f;
        this->unk_1E0 = 0;
    } else if (this->actor.colChkInfo.damageEffect == 4) {
        this->unk_220 = 0.85f;
        this->unk_21C = 4.0f;
        this->unk_1E0 = 20;
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_CLEAR_TAG, collider->info.bumper.hitPos.x,
                    collider->info.bumper.hitPos.y, collider->info.bumper.hitPos.z, 0, 0, 0,
                    CLEAR_TAG_LARGE_LIGHT_RAYS);
    } else if (this->actor.colChkInfo.damageEffect == 5) {
        this->unk_220 = 0.85f;
        this->unk_21C = 4.0f;
        this->unk_1E0 = 30;
    }
}

void func_808FA11C(EnRr* this) {
    this->unk_1E0 = 10;
    this->collider1.base.colType = COLTYPE_HIT3;
    this->collider1.info.elemType = ELEMTYPE_UNK0;
    this->unk_1EE = 80;
    this->unk_220 = 0.85f;
    this->unk_224 = 1.2750001f;
    this->unk_21C = 1.0f;
    this->actor.flags &= ~0x400;
    Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 80);
}

void func_808FA19C(EnRr* this, GlobalContext* globalCtx) {
    this->unk_1EE = 0;
    if (this->unk_1E0 == 10) {
        this->unk_1E0 = 0;
        this->collider1.base.colType = COLTYPE_HIT0;
        this->collider1.info.elemType = ELEMTYPE_UNK1;
        this->unk_21C = 0.0f;
        Actor_SpawnIceEffects(globalCtx, &this->actor, this->unk_234, 20, 2, this->actor.scale.y * 23.333334f,
                              this->actor.scale.y * 20.000002f);
        this->actor.flags |= 0x400;
    }
}

void func_808FA238(EnRr* this, f32 arg1) {
    this->actor.speedXZ = arg1;
    Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_LIKE_WALK);
}

void func_808FA260(EnRr* this) {
    static f32 D_808FC1E4[] = { 0.0f, 500.0f, 750.0f, 1000.0f, 1000.0f };
    s32 i;

    this->unk_1E1 = 1;
    this->unk_1E6 = 20;
    this->unk_1F6 = 2500;
    this->unk_210 = 0.0f;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = D_808FC1E4[i];
        this->unk_324[i].unk_14 = 6000;
        this->unk_324[i].unk_18 = 0;
        this->unk_324[i].unk_0C = 0.8f;
    }

    this->actionFunc = func_808FB088;

    Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_LIKE_UNARI);
}

void func_808FA344(EnRr* this) {
    s32 i;

    this->unk_1E1 = 0;
    this->unk_210 = 0.0f;
    this->unk_1F6 = 2500;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = 0.0f;
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_18 = 0;
        this->unk_324[i].unk_0C = 1.0f;
    }

    if (this->unk_1E2 != 0) {
        this->unk_1E6 = 100;
        this->actionFunc = func_808FB680;
    } else {
        this->unk_1E6 = 60;
        this->actionFunc = func_808FAF94;
    }
}

void func_808FA3F8(EnRr* this, Player* player) {
    s32 i;

    this->unk_1EA = 100;
    this->unk_1FC = 20;
    this->collider1.base.ocFlags1 &= ~OC1_TYPE_PLAYER;
    this->actor.flags &= ~1;
    this->unk_1F0 = 8;
    this->unk_1E1 = 0;
    this->actor.speedXZ = 0.0f;
    this->unk_218 = 0.0f;
    this->unk_210 = 0.0f;
    this->unk_204 = 0.15f;
    this->unk_20C = 0x200;
    this->unk_1F6 = 5000;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = 0.0f;
        this->unk_324[i].unk_0C = 1.0f;
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_18 = 0;
    }

    this->actionFunc = func_808FB1C0;
    Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_SUISEN_DRINK);
}

void func_808FA4F4(EnRr* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    u32 sp38;
    s32 sp34;
    f32 sp30;
    f32 sp2C;

    if (player->stateFlags2 & 0x80) {
        player->actor.parent = NULL;
        player->unk_AE8 = 100;
        this->actor.flags |= 1;
        this->unk_1F0 = 110;
        this->unk_1F6 = 2500;
        this->unk_210 = 0.0f;
        this->unk_20C = 0x800;

        if (((this->unk_1E2 == 0) && (gSaveContext.playerForm == PLAYER_FORM_HUMAN)) &&
            (CUR_EQUIP_VALUE_VOID(EQUIP_SHIELD) == EQUIP_SHIELD)) {
            sp34 = true;
            this->unk_1E2 = func_8012ED78(globalCtx, 1);
        } else {
            sp34 = false;
        }

        if (sp34 && (Message_GetState(&globalCtx->msgCtx) == 0)) {
            func_801518B0(globalCtx, 0xF6, NULL);
        }

        if (this->actor.params == ENRR_0) {
            sp38 = 8;
        } else {
            sp38 = 16;
        }

        sp30 = this->actor.scale.x * 210.52632f;
        sp2C = this->actor.scale.x * 631.579f;

        player->actor.world.pos.x += sp30 * Math_SinS(this->actor.shape.rot.y);
        player->actor.world.pos.y += sp2C;
        player->actor.world.pos.z += sp30 * Math_CosS(this->actor.shape.rot.y);

        func_800B8D50(globalCtx, &this->actor, sp30, this->actor.shape.rot.y, sp2C, sp38);
        Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_SUISEN_THROW);
    }
}

void func_808FA6B8(EnRr* this) {
    s32 i;

    this->unk_1E1 = 0;
    if (this->actor.colChkInfo.damageEffect == 5) {
        this->unk_1EC = 80;
    } else {
        this->unk_1EC = 40;
    }
    Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, this->unk_1EC);

    this->unk_1E6 = 20;
    this->unk_1F6 = 2500;
    this->unk_210 = 0.0f;
    this->unk_204 = 0.0f;
    this->unk_20C = 0.0f;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = 0.0f;
        this->unk_324[i].unk_0C = 1.0f;
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_18 = 0;
    }

    this->actionFunc = func_808FB398;
    Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_LIKE_DAMAGE);
}

void func_808FA7AC(EnRr* this) {
    static f32 D_808FC1F8[] = { 0.0f, 500.0f, 750.0f, 1000.0f, 1000.0f, 0.0f };
    s32 i;

    this->unk_1F6 = 2500;
    this->unk_1E1 = 1;
    this->unk_1E6 = 10;
    this->unk_210 = 0.0f;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = D_808FC1F8[i];
        this->unk_324[i].unk_0C = 0.8f;
        this->unk_324[i].unk_14 = 5000;
        this->unk_324[i].unk_18 = 0;
    }

    this->actionFunc = func_808FB2C0;
}

void func_808FA870(EnRr* this) {
    s32 i;

    this->unk_210 = 0.0f;
    this->unk_204 = 0.15f;
    this->unk_20C = 0x800;
    this->unk_1F6 = 2500;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = 0.0f;
        this->unk_324[i].unk_0C = 1.0f;
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_18 = 0;
    }

    this->actionFunc = func_808FAF94;
}

void func_808FA910(EnRr* this) {
    s32 i;

    this->unk_1E4 = 0;
    this->unk_214 = 0.0f;
    Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 40);
    this->unk_210 = 0.0f;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = 0.0f;
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_18 = 0;
    }

    this->actionFunc = func_808FB42C;
    Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_LIKE_DEAD);
    this->actor.flags &= ~1;
}

void func_808FA9CC(EnRr* this) {
    s32 i;

    this->unk_1F2 = 0;
    this->unk_1F4 = 0;
    this->unk_1F6 = 2500;
    this->unk_1F8 = 0;
    this->unk_1FA = 0;
    this->unk_200 = 0.0f;
    this->unk_204 = 0.15f;
    this->unk_208 = 0.0f;
    this->unk_20C = 0x800;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_08 = 1.0f;
        this->unk_324[i].unk_0C = 1.0f;
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_16 = 0;
        this->unk_324[i].unk_18 = 0;
    }

    this->actionFunc = func_808FB710;
}

s32 func_808FAA94(EnRr* this, GlobalContext* globalCtx) {
    ColliderCylinder* sp2C;
    s32 flag = (this->collider1.base.acFlags & AC_HIT) != 0;

    if (flag || (this->collider2.base.acFlags & AC_HIT)) {
        if (flag) {
            sp2C = &this->collider1;
        } else {
            sp2C = &this->collider2;
        }

        this->collider1.base.acFlags &= ~AC_HIT;
        this->collider2.base.acFlags &= ~AC_HIT;

        if ((this->unk_1E0 == 10) && (sp2C->info.acHitInfo->toucher.dmgFlags & 0xDB0B3)) {
            return false;
        }

        if (this->actor.colChkInfo.damageEffect == 14) {
            return false;
        }

        Actor_SetDropFlag(&this->actor, &sp2C->info);
        func_808FA4F4(this, globalCtx);
        func_808FA19C(this, globalCtx);

        if (!Actor_ApplyDamage(&this->actor)) {
            Enemy_StartFinishingBlow(globalCtx, &this->actor);
            if (this->actor.colChkInfo.damageEffect == 3) {
                func_808FA11C(this);
                this->collider1.base.acFlags &= ~AC_ON;
                this->collider2.base.acFlags &= ~AC_ON;
                func_808FA9CC(this);
            } else {
                func_808FA01C(this, globalCtx, sp2C);
                func_808FA910(this);
            }
        } else if (this->actor.colChkInfo.damageEffect == 1) {
            Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_COMMON_FREEZE);
            Actor_SetColorFilter(&this->actor, 0, 255, 0, 80);
            this->unk_1EE = 80;
            func_808FA9CC(this);
        } else if (this->actor.colChkInfo.damageEffect == 3) {
            func_808FA11C(this);
            func_808FA9CC(this);
        } else {
            func_808FA01C(this, globalCtx, sp2C);
            func_808FA6B8(this);
        }
        return true;
    }
    return false;
}

void func_808FAC80(EnRr* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if ((this->unk_1F0 == 0) && ((this->collider2.base.atFlags & AT_HIT) || (this->collider1.base.atFlags & AT_HIT))) {
        this->collider1.base.atFlags &= ~AT_HIT;
        this->collider2.base.atFlags &= ~AT_HIT;
        if (globalCtx->grabPlayer(globalCtx, player)) {
            player->actor.parent = &this->actor;
            func_808FA3F8(this, player);
        }
    }
}

void func_808FAD1C(EnRr* this, GlobalContext* globalCtx) {
    s32 i;
    EnRrStruct* ptr;

    this->unk_1F2 = 0;
    this->unk_1F4 = 0;
    this->unk_1F6 = 2500;
    this->unk_1F8 = 0;
    this->unk_1FA = 0;
    this->unk_200 = 0.0f;
    this->unk_204 = 0.15f;
    this->unk_208 = 0.0f;
    this->unk_20C = 0x800;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_16 = 0;
        this->unk_324[i].unk_18 = 0;

        this->unk_324[i].unk_08 = 1.0f;
        this->unk_324[i].unk_0C = 1.0f;
        this->unk_324[i].unk_10 = this->unk_200;
    }

    for (i = 1; i < ARRAY_COUNT(this->unk_324); i++) {
        ptr = &this->unk_324[i];
        ptr->unk_14 = Math_CosS(this->unk_1F8 * i) * this->unk_208;
        ptr->unk_18 = Math_SinS(this->unk_1FA * i) * this->unk_208;
    }
}

void func_808FAE50(EnRr* this, GlobalContext* globalCtx) {
    s32 i;
    EnRrStruct* ptr;

    if (this->actionFunc != func_808FB42C) {
        for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
            ptr = &this->unk_324[i];
            ptr->unk_10 = Math_CosS(this->unk_1F2 + (i * 0x4000)) * this->unk_200;
        }

        if (this->unk_1E1 == 0) {
            for (i = 1; i < ARRAY_COUNT(this->unk_324); i++) {
                ptr = &this->unk_324[i];
                ptr->unk_14 = Math_CosS(this->unk_1F2 + (i * this->unk_1F8)) * this->unk_208;
                ptr->unk_18 = Math_SinS(this->unk_1F2 + (i * this->unk_1FA)) * this->unk_208;
            }
        }
    }

    if (this->unk_1EE == 0) {
        this->unk_1F2 += this->unk_1F4;
    }
}

void func_808FAF94(EnRr* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 10, 500, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if ((this->unk_1E6 == 0) && !(player->stateFlags2 & 0x80) && (Player_GetMask(globalCtx) != PLAYER_MASK_STONE) &&
        (this->actor.xzDistToPlayer < (8421.053f * this->actor.scale.x))) {
        func_808FA260(this);
    } else if ((this->actor.xzDistToPlayer < 400.0f) && (this->actor.speedXZ == 0.0f)) {
        func_808FA238(this, 2.0f);
    }
}

void func_808FB088(EnRr* this, GlobalContext* globalCtx) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 10, 500, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Player_GetMask(globalCtx) == PLAYER_MASK_STONE) {
        func_808FA344(this);
        return;
    }

    switch (this->unk_1E1) {
        case 1:
            if (this->unk_1E6 == 0) {
                this->unk_1E1 = 2;
            }
            break;

        case 2:
            if (this->unk_1E6 == 0) {
                this->unk_324[4].unk_0C = 1.5f;
                this->unk_1E6 = 5;
                this->unk_1E1 = 3;
            }
            break;

        case 3:
            if (this->unk_1E6 == 0) {
                this->unk_1E6 = 2;
                this->unk_324[4].unk_04 = 2000.0f;
                this->unk_1E1 = 4;
            }
            break;

        case 4:
            if (this->unk_1E6 == 0) {
                this->unk_324[4].unk_0C = 0.8f;
                this->unk_1E6 = 20;
                this->unk_1E1 = 5;
            }
            break;

        case 5:
            if (this->unk_1E6 == 0) {
                func_808FA344(this);
            }
            break;
    }
}

void func_808FB1C0(EnRr* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    func_8013ECE0(this->actor.xyzDistToPlayerSq, 120, 2, 120);
    if (!(this->unk_1E4 & 7)) {
        Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_EYEGOLE_DEMO_EYE);
    }

    player->unk_AE8 = 0;
    this->unk_1F0 = 8;
    this->unk_1EA--;

    if (this->unk_1EA == 0) {
        func_808FA7AC(this);
    } else {
        Math_StepToF(&player->actor.world.pos.x, this->unk_228.x, 30.0f);
        Math_StepToF(&player->actor.world.pos.y, this->unk_228.y + this->unk_218, 30.0f);
        Math_StepToF(&player->actor.world.pos.z, this->unk_228.z, 30.0f);
        Math_StepToF(&this->unk_218, -(f32)this->collider1.dim.height, 5.0f);
    }
}

void func_808FB2C0(EnRr* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    this->unk_1E6--;
    player->unk_AE8 = 0;
    Math_StepToF(&player->actor.world.pos.x, this->unk_228.x, 30.0f);
    Math_StepToF(&player->actor.world.pos.y, this->unk_228.y + this->unk_218, 30.0f);
    Math_StepToF(&player->actor.world.pos.z, this->unk_228.z, 30.0f);
    Math_StepToF(&this->unk_218, -(f32)this->collider1.dim.height, 5.0f);
    if (this->unk_1E6 == 0) {
        this->unk_1E1 = 0;
        func_808FA4F4(this, globalCtx);
        func_808FA344(this);
    }
}

void func_808FB398(EnRr* this, GlobalContext* globalCtx) {
    s32 i;
    s16 phi_v1;

    this->unk_1EC--;
    if (this->unk_1EC == 0) {
        func_808FA870(this);
        return;
    }

    if (this->unk_1E0 == 30) {
        if (this->unk_1EC & 2) {
            phi_v1 = 1000;
        } else {
            phi_v1 = -1000;
        }
    } else {
        if (this->unk_1EC & 8) {
            phi_v1 = 5000;
        } else {
            phi_v1 = -5000;
        }
    }

    for (i = 1; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_18 = phi_v1;
    }
}

void func_808FB42C(EnRr* this, GlobalContext* globalCtx) {
    s32 pad;
    s32 i;
    EnRrStruct* ptr;
    f32 temp_f20;

    this->actor.colorFilterTimer = 40;
    if (this->unk_1E4 < 40) {
        for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
            ptr = &this->unk_324[i];
            Math_StepToF(&ptr->unk_04, (i + 59) - (this->unk_1E4 * 25.0f), 50.0f);
            ptr->unk_0C = (SQ((f32)(4 - i)) * this->unk_1E4 * 0.003f) + 1.0f;
        }
        return;
    }

    if (this->unk_1E4 >= 95) {
        if (this->unk_1E2 != 0) {
            Item_DropCollectible(globalCtx, &this->actor.world.pos, ITEM00_SHIELD_HERO);
        }
        Item_DropCollectibleRandom(globalCtx, &this->actor, &this->actor.world.pos, 0x90);
        Actor_MarkForDeath(&this->actor);
    } else if (this->unk_1E4 == 88) {
        Vec3f sp74;

        sp74.x = this->actor.world.pos.x;
        sp74.y = this->actor.world.pos.y + 20.0f;
        sp74.z = this->actor.world.pos.z;
        func_800B3030(globalCtx, &sp74, &gZeroVec3f, &gZeroVec3f, 100, 0, 0);
        SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &sp74, 11, NA_SE_EN_EXTINCT);
    } else {
        temp_f20 = this->actor.scale.y * 66.66667f;

        Math_StepToF(&this->actor.scale.x, 0.0f, this->unk_214);
        Math_StepToF(&this->unk_214, 0.001f * temp_f20, 0.00001f * temp_f20);
        this->actor.scale.z = this->actor.scale.x;
    }
}

void func_808FB680(EnRr* this, GlobalContext* globalCtx) {
    if (this->unk_1E6 == 0) {
        this->actionFunc = func_808FAF94;
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, BINANG_ROT180(this->actor.yawTowardsPlayer), 10, 1000, 0);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (this->actor.speedXZ == 0.0f) {
            func_808FA238(this, 2.0f);
        }
    }
}

void func_808FB710(EnRr* this, GlobalContext* globalCtx) {
    this->unk_1EE--;
    if (this->unk_1EE == 0) {
        func_808FA19C(this, globalCtx);
        func_808FA870(this);
        this->actionFunc = func_808FAF94;
    } else if ((this->actor.colChkInfo.health == 0) && (this->unk_1EE == 77)) {
        func_808FA19C(this, globalCtx);
        func_808FA910(this);
    }
}

void func_808FB794(EnRr* this, GlobalContext* globalCtx) {
    Vec3f sp2C;

    if ((this->actor.depthInWater < this->collider1.dim.height) && (this->actor.depthInWater > 1.0f) &&
        ((globalCtx->gameplayFrames % 9) == 0)) {
        sp2C.x = this->actor.world.pos.x;
        sp2C.y = this->actor.world.pos.y + this->actor.depthInWater;
        sp2C.z = this->actor.world.pos.z;
        EffectSsGRipple_Spawn(globalCtx, &sp2C, this->actor.scale.x * 34210.527f, this->actor.scale.x * 60526.316f, 0);
    }
}

void EnRr_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnRr* this = THIS;
    EnRrStruct* ptr;
    s32 i;

    this->unk_1E4++;

    if (this->unk_1EE == 0) {
        this->unk_1E8++;
    }

    if (this->unk_1E6 != 0) {
        this->unk_1E6--;
    }

    if (this->unk_1F0 != 0) {
        this->unk_1F0--;
    }

    Actor_SetFocus(&this->actor, this->actor.scale.y * 2000.0f);
    func_808FAE50(this, globalCtx);

    if (!func_808FAA94(this, globalCtx)) {
        func_808FAC80(this, globalCtx);
    }

    this->actionFunc(this, globalCtx);

    if (this->actor.params == ENRR_2) {
        this->actor.speedXZ = 0.0f;
    } else {
        Math_StepToF(&this->actor.speedXZ, 0.0f, 0.1f);
    }

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 30.0f, this->collider1.dim.radius, 0.0f, 0x5D);
    func_808FB794(this, globalCtx);

    if (this->unk_1FC > 0) {
        Player* player = GET_PLAYER(globalCtx);

        if (!(player->stateFlags2 & 0x80)) {
            this->unk_1FC--;
            if (this->unk_1FC == 0) {
                this->collider1.base.ocFlags1 |= OC1_TYPE_PLAYER;
            }
        }
    }

    Collider_UpdateCylinder(&this->actor, &this->collider1);

    if ((this->actionFunc != func_808FB42C) && (this->actionFunc != func_808FB398)) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider1.base);
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider2.base);
        if ((this->unk_1F0 == 0) && (this->actionFunc == func_808FB088) && (this->unk_1EE == 0)) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &this->collider1.base);
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &this->collider2.base);
        } else {
            this->collider2.base.atFlags &= ~AT_HIT;
            this->collider1.base.atFlags &= ~AT_HIT;
        }
    } else {
        this->collider2.base.atFlags &= ~AT_HIT;
        this->collider1.base.atFlags &= ~AT_HIT;
        this->collider2.base.acFlags &= ~AC_HIT;
        this->collider1.base.acFlags &= ~AC_HIT;
    }

    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider1.base);

    if (this->unk_1EE == 0) {
        Math_ScaledStepToS(&this->unk_1F4, this->unk_1F6, 50);
        Math_ScaledStepToS(&this->unk_1F8, 0x3000, 0xA4);
        Math_ScaledStepToS(&this->unk_1FA, 0x1000, 0x29);
        Math_StepToF(&this->unk_200, this->unk_204, 0.0015f);
        Math_StepToF(&this->unk_208, this->unk_20C, 20.0f);

        for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
            ptr = &this->unk_324[i];
            Math_SmoothStepToS(&ptr->unk_1A.x, ptr->unk_14, 5, this->unk_210 * 1000.0f, 0);
            Math_SmoothStepToS(&ptr->unk_1A.z, ptr->unk_18, 5, this->unk_210 * 1000.0f, 0);
            Math_StepToF(&ptr->unk_08, ptr->unk_0C, this->unk_210 * 0.2f);
            Math_StepToF(&ptr->unk_00, ptr->unk_04, this->unk_210 * 300.0f);
        }

        Math_StepToF(&this->unk_210, 1.0f, 0.2f);
    }

    if (this->unk_21C > 0.0f) {
        if (this->unk_1E0 != 10) {
            Math_StepToF(&this->unk_21C, 0.0f, 0.05f);
            this->unk_220 = (this->unk_21C + 1.0f) * 0.425f;
            this->unk_220 = CLAMP_MAX(this->unk_220, 0.85f);
        } else if (!Math_StepToF(&this->unk_224, 0.85f, 0.02125f)) {
            func_800B9010(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

void EnRr_Draw(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnRr* this = THIS;
    Mtx* matrix;
    Vec3f* vecPtr;
    s32 i;
    EnRrStruct* ptr;
    Vec3f spA4;
    f32 temp_f20;

    matrix = GRAPH_ALLOC(globalCtx->state.gfxCtx, 256);

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_8012C28C(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0C, matrix);
    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (this->unk_1E8 * 0) & 0x7F, (this->unk_1E8 * 0) & 0x3F,
                                0x20, 0x10, 1, (this->unk_1E8 * 0) & 0x3F, (this->unk_1E8 * -6) & 0x7F, 0x20, 0x10));

    Matrix_StatePush();
    Matrix_Scale((1.0f + this->unk_324[0].unk_10) * this->unk_324[0].unk_08, 1.0f,
                 (1.0f + this->unk_324[0].unk_10) * this->unk_324[0].unk_08, MTXMODE_APPLY);

    vecPtr = &this->unk_234[0];

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    Matrix_GetStateTranslationAndScaledZ(1842.1053f, vecPtr++);
    Matrix_GetStateTranslationAndScaledZ(-1842.1053f, vecPtr++);
    Matrix_GetStateTranslationAndScaledX(1842.1053f, vecPtr++);
    Matrix_GetStateTranslationAndScaledX(-1842.1053f, vecPtr++);
    Matrix_StatePop();

    for (i = 1; i < ARRAY_COUNT(this->unk_324); i++) {
        temp_f20 = this->unk_324[i].unk_08 * (this->unk_324[i].unk_10 + 1.0f);
        ptr = &this->unk_324[i];

        Matrix_InsertTranslation(0.0f, ptr->unk_00 + 1000.0f, 0.0f, MTXMODE_APPLY);
        Matrix_InsertRotation(ptr->unk_1A.x, ptr->unk_1A.y, ptr->unk_1A.z, MTXMODE_APPLY);
        Matrix_StatePush();
        Matrix_Scale(temp_f20, 1.0f, temp_f20, MTXMODE_APPLY);
        Matrix_ToMtx(matrix);

        if ((i & 1) != 0) {
            Matrix_RotateY(0x2000, MTXMODE_APPLY);
        }

        Matrix_GetStateTranslationAndScaledZ(1842.1053f, vecPtr++);
        Matrix_GetStateTranslationAndScaledZ(-1842.1053f, vecPtr++);
        Matrix_GetStateTranslationAndScaledX(1842.1053f, vecPtr++);
        Matrix_GetStateTranslationAndScaledX(-1842.1053f, vecPtr++);
        Matrix_StatePop();
        matrix++;
        if (i == 3) {
            Matrix_GetStateTranslation(&spA4);
        }
    }

    Matrix_GetStateTranslation(&this->unk_228);
    this->collider2.dim.pos.x = ((this->unk_228.x - spA4.x) * 0.85f) + spA4.x;
    this->collider2.dim.pos.y = ((this->unk_228.y - spA4.y) * 0.85f) + spA4.y;
    this->collider2.dim.pos.z = ((this->unk_228.z - spA4.z) * 0.85f) + spA4.z;

    gSPDisplayList(POLY_OPA_DISP++, D_06000470);

    func_800BE680(globalCtx, &this->actor, this->unk_234, ARRAY_COUNT(this->unk_234),
                  this->actor.scale.y * 66.66667f * this->unk_220, this->unk_224, this->unk_21C, this->unk_1E0);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
