/*
 * File: z_en_rg.c
 * Overlay: ovl_En_Rg
 * Description: Racing Goron
 */

#include "z_en_rg.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_oF1d_map/object_oF1d_map.h"

#define FLAGS (ACTOR_FLAG_10 | ACTOR_FLAG_80000000)

#define THIS ((EnRg*)thisx)

void EnRg_Init(Actor* thisx, GlobalContext* globalCtx);
void EnRg_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnRg_Update(Actor* thisx, GlobalContext* globalCtx);
void EnRg_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80BF4EBC(EnRg* this, GlobalContext* globalCtx);
void func_80BF4FC4(EnRg* this, GlobalContext* globalCtx);

s32 D_80BF5C10;

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

static ColliderSphereInit sSphereInit = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_PLAYER | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEMTYPE_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 20 }, 100 },
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT1,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 20, 62, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
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

s32 D_80BF57E4[][4] = {
    { 0, 0, 0, 0 },     { 0, 0, 0, 0 },     { 0, 0, 0, 0 },     { 0, 0, 0, 0 },     { 1, 1, 1, 1 },
    { 11, 11, 8, 10 },  { 16, 12, 12, 14 }, { 19, 15, 15, 18 }, { 25, 17, 18, 22 }, { 29, 20, 23, 28 },
    { 39, 24, 28, 33 }, { 43, 27, 33, 37 }, { 46, 28, 36, 37 }, { 51, 30, 38, 39 }, { 54, 33, 42, 40 },
    { 56, 34, 44, 41 }, { 60, 38, 50, 45 }, { 67, 42, 55, 49 }, { 74, 47, 61, 54 },
};

AnimationInfoS D_80BF5914[] = {
    { &object_oF1d_map_Anim_012DE0, 2.0f, 0, -1, ANIMMODE_ONCE, 0 },
    { &object_oF1d_map_Anim_012DE0, -2.0f, 0, -1, ANIMMODE_ONCE, 0 },
};

TexturePtr D_80BF5934[] = {
    gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex, gDust4Tex, gDust3Tex, gDust2Tex, gDust1Tex,
};

Color_RGBA8 D_80BF5954[] = {
    { 255, 255, 255, 0 },
    { 170, 130, 90, 0 },
    { 0, 0, 0, 0 },
};

Color_RGBA8 D_80BF5960[] = {
    { 255, 255, 255, 0 },
    { 100, 60, 20, 0 },
    { 0, 0, 0, 0 },
};

Vec3f D_80BF596C[] = {
    { -2473.0f, 39.0f, 7318.0f },  { -2223.0f, 142.0f, 7184.0f }, { -2281.0f, 41.0f, 7718.0f },
    { -2136.0f, 96.0f, 7840.0f },  { -2432.0f, 6.0f, 7857.0f },   { -2412.0f, 139.0f, 6872.0f },
    { -2719.0f, 39.0f, 7110.0f },  { -2289.0f, 67.0f, 7463.0f },  { -2820.0f, 85.0f, 6605.0f },
    { -2088.0f, 160.0f, 7584.0f }, { -2503.0f, 1.0f, 7643.0f },
};

EffectTireMarkInit D_80BF59F0 = {
    0,
    62,
    { 0, 0, 15, 100 },
};

TexturePtr D_80BF59F8[] = {
    object_oF1d_map_Tex_010438, object_oF1d_map_Tex_010C38, object_oF1d_map_Tex_011038,
    object_oF1d_map_Tex_010C38, object_oF1d_map_Tex_010838,
};

void func_80BF3920(EnRgStruct* ptr, GlobalContext* globalCtx) {
    f32 temp_f20;
    u8 phi_fp = false;
    s32 i;
    s32 idx;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_8012C2DC(globalCtx->state.gfxCtx);

    for (i = 0; i < 32; i++, ptr++) {
        if ((ptr->unk_00 >= 4) && (ptr->unk_00 < 7)) {
            if (!phi_fp) {
                POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, sizeof(Gfx) * 0);
                gSPDisplayList(POLY_XLU_DISP++, object_oF1d_map_DL_014CF0);
                phi_fp = true;
            }

            Matrix_StatePush();

            if (globalCtx) {}
            temp_f20 = (f32)ptr->unk_02 / ptr->unk_01;

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, D_80BF5954[ptr->unk_00 - 4].r, D_80BF5954[ptr->unk_00 - 4].g,
                            D_80BF5954[ptr->unk_00 - 4].b, (u8)(temp_f20 * 255.0f));
            gDPSetEnvColor(POLY_XLU_DISP++, D_80BF5960[ptr->unk_00 - 4].r, D_80BF5960[ptr->unk_00 - 4].g,
                           D_80BF5960[ptr->unk_00 - 4].b, 0);

            Matrix_InsertTranslation(ptr->unk_10.x, ptr->unk_10.y, ptr->unk_10.z, MTXMODE_NEW);
            Matrix_Scale(ptr->unk_34, ptr->unk_34, 1.0f, MTXMODE_APPLY);
            Matrix_NormalizeXYZ(&globalCtx->billboardMtxF);

            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            idx = temp_f20 * 7.0f;
            gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(D_80BF5934[idx]));
            gSPDisplayList(POLY_XLU_DISP++, object_oF1d_map_DL_014D00);

            Matrix_StatePop();
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void func_80BF3C64(EnRg* this) {
    EnRgStruct* ptr = this->unk_34C;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->unk_34C); i++, ptr++) {
        if (ptr->unk_00) {
            if (!ptr->unk_02) {
                ptr->unk_00 = 0;
            } else {
                ptr->unk_10.x += ptr->unk_28.x;
                ptr->unk_10.y += ptr->unk_28.y;
                ptr->unk_10.z += ptr->unk_28.z;
                ptr->unk_28.x += ptr->unk_1C.x;
                ptr->unk_28.y += ptr->unk_1C.y;
                ptr->unk_28.z += ptr->unk_1C.z;
                ptr->unk_34 += ptr->unk_38;
                ptr->unk_02--;
            }
        }
    }
}

void func_80BF3DA0(EnRg* this, GlobalContext* globalCtx) {
    func_80BF3920(this->unk_34C, globalCtx);
}

void func_80BF3DC4(EnRg* this, GlobalContext* globalCtx) {
    this->collider2.dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider2.dim.worldSphere.center.y = this->actor.world.pos.y + this->actor.shape.yOffset;
    this->collider2.dim.worldSphere.center.z = this->actor.world.pos.z;

    if (this->actor.speedXZ >= 10.0f) {
        CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &this->collider2.base);
    }
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider2.base);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider2.base);
}

void func_80BF3E88(EnRg* this, GlobalContext* globalCtx) {
    Math_Vec3f_ToVec3s(&this->collider1.dim.pos, &this->actor.world.pos);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider1.base);
}

void func_80BF3ED4(EnRg* this, GlobalContext* globalCtx) {
    if (this->unk_310 & 0x10) {
        func_80BF3DC4(this, globalCtx);
    } else {
        func_80BF3E88(this, globalCtx);
    }
}

s32 func_80BF3F14(EnRg* this, GlobalContext* globalCtx) {
    this->unk_310 &= ~0x20;
    this->unk_310 &= ~0x40;
    this->unk_310 &= ~0x80;

    if ((this->collider1.base.atFlags & AT_HIT) || (this->collider2.base.atFlags & AT_HIT)) {
        this->collider1.base.atFlags &= ~AT_HIT;
        this->collider2.base.atFlags &= ~AT_HIT;
        this->unk_310 |= 0x20;
    }

    if ((this->collider1.base.acFlags & AC_HIT) || (this->collider2.base.acFlags & AC_HIT)) {
        this->collider1.base.acFlags &= ~AC_HIT;
        this->collider2.base.acFlags &= ~AC_HIT;
        this->unk_310 |= 0x40;
    }

    if ((this->collider1.base.ocFlags1 & OC1_HIT) || (this->collider2.base.ocFlags1 & OC1_HIT)) {
        this->collider1.base.ocFlags1 &= ~OC1_HIT;
        this->collider2.base.ocFlags1 &= ~OC1_HIT;
        this->unk_310 |= 0x80;
    }

    return false;
}

void func_80BF3FF8(EnRg* this) {
    this->skelAnime.playSpeed = this->unk_314;
    SkelAnime_Update(&this->skelAnime);
}

s32 func_80BF4024(EnRg* this, GlobalContext* globalCtx) {
    if ((globalCtx->csCtx.state == 0) && (this->unk_334 == 1)) {
        if (Animation_OnFrame(&this->skelAnime, 2.0f)) {
            Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_GOLON_CIRCLE);
        }

        if (Animation_OnFrame(&this->skelAnime, 22.0f)) {
            Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_GOLON_SIT_IMT);
        }
    }

    return false;
}

s32 func_80BF409C(EnRg* this, s32 arg1) {
    s32 ret = false;

    if (arg1 != this->unk_334) {
        this->unk_334 = arg1;
        ret = SubS_ChangeAnimationByInfoS(&this->skelAnime, D_80BF5914, arg1);
        this->unk_314 = this->skelAnime.playSpeed;
    }
    return ret;
}

void func_80BF40F4(EnRg* this) {
    if (DECR(this->unk_31C) == 0) {
        this->unk_31E++;
        if (this->unk_31E >= 4) {
            this->unk_31C = Rand_S16Offset(30, 30);
            this->unk_31E = 0;
        }
    }
}

s32 func_80BF416C(EnRg* this, GlobalContext* globalCtx) {
    if ((this->actor.bgCheckFlags & 1) && (this->actor.speedXZ >= 0.01f)) {
        Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_GOLON_ROLLING - SFX_FLAG);
        func_800AE930(&globalCtx->colCtx, Effect_GetByIndex(this->unk_340), &this->actor.world.pos, 18.0f,
                      this->actor.shape.rot.y, this->actor.floorPoly, this->actor.floorBgId);
    } else {
        func_800AEF44(Effect_GetByIndex(this->unk_340));
    }
    return false;
}

s32 func_80BF4220(EnRg* this, GlobalContext* globalCtx, Actor* arg2) {
    CollisionPoly* sp44 = NULL;
    s32 sp40;
    Vec3f sp34;

    if (Actor_ActorAIsFacingAndNearActorB(&this->actor, arg2, 400.0f, 0x2000) &&
        !BgCheck_EntityLineTest1(&globalCtx->colCtx, &this->actor.world.pos, &arg2->world.pos, &sp34, &sp44, true,
                                 false, false, true, &sp40)) {
        return true;
    }
    return false;
}

s32 func_80BF42BC(EnRg* this, f32 arg1) {
    f32 sp2C;
    s32 sp24;

    Math_ApproachF(&this->actor.speedXZ, arg1, 0.3f, 0.5f);

    sp2C = this->actor.speedXZ * (1.0f / 26);
    if (sp2C > 1.0f) {
        sp2C = 1.0f;
    }
    sp24 = 5460.0f * sp2C;

    this->actor.scale.x = 0.01f - (Math_SinS(sp24) * 0.01f);
    this->actor.scale.y = (Math_SinS(sp24) * 0.01f) + 0.01f;
    this->actor.scale.z = (Math_SinS(sp24) * 0.01f) + 0.01f;

    this->actor.shape.yOffset = this->actor.scale.y * 100.0f * 14.0f;
    if (!(this->unk_310 & 0x1000)) {
        this->actor.shape.rot.x += (s16)(13650.0f * sp2C);
    }

    this->actor.world.rot.x = this->actor.shape.rot.x;
    return false;
}

#ifdef NON_MATCHING
s32 func_80BF43FC(EnRg* this) {
    Vec3f sp9C;
    Vec3f sp90;
    f32 sp8C;
    f32 sp88;
    f32 sp84;
    f32 phi_f20 = 0.0f;
    s32 temp_s7 = ENRG_GET_7F80(&this->actor);
    s32 phi_s4 = -1;
    s16 phi_s6 = 0;
    s32 temp_s5 = this->unk_344;
    s32 phi_s0 = D_80BF57E4[this->unk_344][temp_s7];

    while (true) {
        func_8013C8B8(this->path, phi_s0 - 1, &sp9C);
        func_8013C8B8(this->path, phi_s0 + 1, &sp90);
        if (Math3D_PointDistToLine2D(this->actor.world.pos.x, this->actor.world.pos.z, sp9C.x, sp9C.z, sp90.x, sp90.z,
                                     &sp8C, &sp88, &sp84) &&
            (!phi_s6 || ((phi_s4 + 1) == phi_s0) || (sp84 < phi_f20))) {
            phi_s6 = 1;
            phi_f20 = sp84;
            phi_s4 = phi_s0;
        }
        phi_s0++;
        if ((temp_s5 == 18) || (phi_s0 >= D_80BF57E4[temp_s5 + 1][temp_s7])) {
            break;
        }
    }

    return phi_s4;
}
#else
#pragma GLOBAL_ASM("asm/non_matchings/overlays/ovl_En_Rg/func_80BF43FC.s")
#endif

s32 func_80BF4560(EnRg* this, GlobalContext* globalCtx) {
    s32 temp_v0 = SurfaceType_GetSceneExitIndex(&globalCtx->colCtx, this->actor.floorPoly, this->actor.floorBgId);

    if ((temp_v0 < 4) || (temp_v0 >= 19)) {
        temp_v0 = -1;
    }
    return temp_v0;
}

s32 func_80BF45B4(EnRg* this) {
    s32 sp24 = 0;
    s32 ret = false;
    s16 temp_v0;
    s16 temp_v1;

    if ((this->actor.bgCheckFlags & 8) && (this->actor.speedXZ >= 5.0f)) {
        temp_v1 = this->actor.world.rot.y;
        temp_v0 = temp_v1 - BINANG_ROT180(this->actor.wallYaw);

        if (ABS_ALT(temp_v0) >= 0x3400) {
            this->actor.world.rot.y = BINANG_ROT180(BINANG_ROT180(this->actor.wallYaw) - temp_v0);
            this->unk_322 = 0xA;
        } else if (ABS_ALT(temp_v0) >= 0x1000) {
            this->actor.world.rot.y = BINANG_ROT180(BINANG_ROT180(this->actor.wallYaw) - temp_v0);
            this->actor.speedXZ *= 0.75f;
            this->unk_322 = 0xA;
        } else {
            this->actor.world.rot.y = BINANG_ROT180(temp_v1);
            ret = 1;
            this->actor.speedXZ *= 0.25f;
        }
        sp24 = 1;
    } else if (this->unk_310 & 0x40) {
        s16 yaw = Actor_YawBetweenActors(&this->actor, ((void)0, this->collider2.base.ac));

        sp24 = 2;
        if (this->actor.colorFilterTimer == 0) {
            this->actor.speedXZ *= 0.5f;
            if ((s16)(yaw - this->actor.world.rot.y) > 0) {
                this->actor.world.rot.y -= 0x2000;
            } else {
                this->actor.world.rot.y += 0x2000;
            }
        }
        Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 40);
        this->unk_322 = 0xA;
    }

    if (sp24) {
        this->unk_310 &= ~0x800;
        this->unk_310 &= ~0x40;
        if ((this->unk_18C != NULL) && (this->unk_18C->id == ACTOR_PLAYER)) {
            D_80BF5C10 = 0;
        }
        this->unk_18C = NULL;
        this->unk_324 = 0;
    }
    return ret;
}

s32 func_80BF47AC(EnRg* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 phi_f0;
    f32 phi_f2;

    if (player->linearVelocity < 20.0f) {
        phi_f2 = 20.0f;
    } else {
        phi_f2 = player->linearVelocity;
    }

    if ((this->unk_310 & 0x400) || (this->unk_310 & 0x1000)) {
        phi_f0 = 0.0f;
    } else if (this->unk_348 >= 2) {
        phi_f0 = phi_f2 * 0.5f;
    } else if (this->unk_348 == 1) {
        phi_f0 = phi_f2 * 0.75f;
    } else if (this->unk_348 == 0) {
        s16 temp_v0_3 = this->actor.yawTowardsPlayer - this->actor.world.rot.y;

        if ((ABS_ALT(temp_v0_3) > 0x4000) || (this->unk_326 > 0)) {
            phi_f0 = phi_f2 * 0.72f;
        } else {
            phi_f0 = phi_f2 * 0.94f;
        }
    } else if (this->unk_348 == -1) {
        phi_f0 = phi_f2 * 1.6f;
    } else {
        phi_f0 = 2.0f * phi_f2;
    }

    if (phi_f0 > 0.0f) {
        if (this->unk_320 != 0) {
            this->unk_320--;
        }
        phi_f2 = this->unk_320 * 0.01f;
        phi_f0 += phi_f2;
    }

    func_80BF42BC(this, phi_f0);

    return false;
}

void func_80BF4934(EnRg* this) {
    if (this->unk_318 == 1) {
        Actor_PlaySfxAtPos(&this->actor, NA_SE_EN_GOLON_DASH);
    }
}

void func_80BF4964(EnRg* this) {
    Vec3s* sp3C;
    Vec3f sp30;

    if (this->path != NULL) {
        sp3C = Lib_SegmentedToVirtual(this->path->points);

        if (func_8013BD40(&this->actor, this->path, this->unk_33C)) {
            if ((this->path->count - 1) < (this->unk_33C + 1)) {
                this->unk_33C = this->path->count - 1;
            } else {
                this->unk_33C++;
            }
        }

        if (this->unk_322 != 0) {
            this->unk_322--;
        }

        if (this->unk_324 != 0) {
            this->unk_324--;
        }

        if (this->unk_326 != 0) {
            this->unk_326--;
        }

        if (!(this->unk_310 & 0x400) && !(this->unk_310 & 0x1000) && (this->unk_322 == 0) && (this->unk_324 == 0) &&
            (this->actor.bgCheckFlags & 1)) {
            if (this->unk_18C == NULL) {
                Math_Vec3s_ToVec3f(&sp30, &sp3C[this->unk_33C]);
            } else {
                Math_Vec3f_Copy(&sp30, &this->unk_18C->world.pos);
            }
            Math_ApproachS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &sp30), 10, 0x71C);
        }
    }
}

void func_80BF4AB8(EnRg* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* phi_s0;

    phi_s0 = NULL;
    if (!(this->unk_310 & 0x800)) {
        if (this->unk_320 == 0) {
            do {
                phi_s0 = SubS_FindActor(globalCtx, phi_s0, 6, ACTOR_OBJ_TSUBO);
                if (phi_s0 != NULL) {
                    if (func_80BF4220(this, globalCtx, phi_s0) && (phi_s0->update != NULL)) {
                        this->unk_18C = phi_s0;
                        this->unk_310 |= 0x800;
                        break;
                    }
                    phi_s0 = phi_s0->next;
                }
            } while (phi_s0 != NULL);
        }

        if ((phi_s0 == NULL) && (D_80BF5C10 == 0) && (this->unk_326 == 0) && (player->stateFlags3 & 0x80000) &&
            (player->invincibilityTimer == 0) && func_80BF4220(this, globalCtx, &player->actor)) {
            this->unk_18C = &player->actor;
            this->unk_310 |= 0x800;
            D_80BF5C10 = 1;
        }
    } else if ((this->unk_18C != NULL) && !func_80BF4220(this, globalCtx, this->unk_18C)) {
        if (this->unk_18C->id == ACTOR_PLAYER) {
            D_80BF5C10 = 0;
        }
        this->unk_18C = NULL;
        this->unk_310 &= ~0x800;
    }

    if (this->unk_310 & 0x20) {
        if (this->collider2.base.at != NULL) {
            if (this->collider2.base.at->id == ACTOR_OBJ_TSUBO) {
                this->collider2.base.at->params |= 0x3F;
                this->unk_320 += 0x190;
                this->unk_320 = CLAMP_MAX(this->unk_320, 0x190);
            } else if (this->collider2.base.at->id == ACTOR_PLAYER) {
                this->unk_326 = 0x28;
                if (player->stateFlags3 & 0x1000) {
                    player->linearVelocity *= 0.5f;
                    player->unk_B08[0] = player->linearVelocity;
                    player->unk_B08[1] += player->linearVelocity * 0.05f;
                    if (BINANG_SUB(this->actor.yawTowardsPlayer, player->currentYaw) > 0) {
                        player->currentYaw += 0x2000;
                    } else {
                        player->currentYaw -= 0x2000;
                    }
                    player->unk_B8C = 4;
                    player->invincibilityTimer = 20;
                    player->actor.home.rot.y = player->actor.world.rot.y;
                    player->actor.shape.rot.y = player->actor.world.rot.y;
                }

                if ((this->unk_18C != NULL) && (this->unk_18C->id == ACTOR_PLAYER)) {
                    D_80BF5C10 = 0;
                }
            }
        }
        this->unk_18C = NULL;
        this->unk_310 &= ~0x800;
        this->unk_310 &= ~0x20;
    }
}

s32 func_80BF4D64(Vec3f* arg0) {
    return Math3D_XZBoundCheck(-1261.0f, -901.0f, -1600.0f, -1520.0f, arg0->x, arg0->z);
}

s32 func_80BF4DA8(EnRg* this) {
    s32 pad[4];
    s32 ret = false;
    f32 temp_f20;
    s16 temp_v1;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(D_80BF596C); i++) {
        temp_f20 = Actor_XZDistanceToPoint(&this->actor, &D_80BF596C[i]);
        temp_v1 = Actor_YawToPoint(&this->actor, &D_80BF596C[i]) - this->actor.world.rot.y;

        if ((temp_f20 < 100.0f) && (ABS_ALT(temp_v1) < 0xC00)) {
            if (temp_v1 > 0) {
                this->actor.world.rot.y -= 0x1600;
            } else {
                this->actor.world.rot.y += 0x1600;
            }

            this->unk_324 = 20;
            this->unk_310 &= ~0x800;

            if ((this->unk_18C != NULL) && (this->unk_18C->id == ACTOR_PLAYER)) {
                D_80BF5C10 = 0;
            }

            this->unk_18C = NULL;
            ret = true;
            break;
        }
    }
    return ret;
}

void func_80BF4EBC(EnRg* this, GlobalContext* globalCtx) {
    if (this->unk_310 & 0x100) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            this->unk_310 &= ~0x100;
            this->unk_310 |= 0x10;
            this->unk_320 = 0;
            this->actor.shape.yOffset = 14.0f;
            this->actionFunc = func_80BF4FC4;
        }
    } else if (gSaveContext.weekEventReg[12] & 2) {
        if (DECR(this->unk_318) == 0) {
            func_80BF409C(this, 1);
            this->unk_310 &= ~8;
            this->unk_310 &= ~0x10;
            this->unk_310 |= 0x100;
            this->actor.shape.yOffset = 0.0f;
            this->unk_318 = Rand_S16Offset(0, 20);
        }
    }
    SubS_FillLimbRotTables(globalCtx, this->unk_32E, this->unk_328, ARRAY_COUNT(this->unk_328));
}

void func_80BF4FC4(EnRg* this, GlobalContext* globalCtx) {
    this->unk_344 = func_80BF4560(this, globalCtx);

    if (!func_801690CC(globalCtx)) {
        if (this->actor.bgCheckFlags & 2) {
            if (this->unk_310 & 0x400) {
                this->unk_310 &= ~0x400;
                this->actor.speedXZ = 0.0f;
            }

            if (this->unk_344 != -1) {
                this->unk_33C = func_80BF43FC(this);
                if (this->unk_33C >= 0) {
                    this->unk_33C++;
                }
            }
        } else if ((this->actor.bgCheckFlags & 1) && !(this->unk_310 & 0x1000) &&
                   func_80BF4D64(&this->actor.world.pos)) {
            this->unk_310 |= 0x1000;
        }

        if ((this->unk_18C != NULL) && (this->unk_18C->update == NULL)) {
            this->unk_18C = NULL;
            this->unk_310 &= ~0x800;
        }

        if (gSaveContext.eventInf[1] & 1) {
            if (DECR(this->unk_318) == 0) {
                func_80BF47AC(this, globalCtx);

                if ((this->unk_324 == 0) && !func_80BF4DA8(this)) {
                    func_80BF4AB8(this, globalCtx);
                }

                if (func_80BF45B4(this)) {
                    this->unk_310 |= 0x400;
                    this->actor.velocity.y = 7.0f;
                }

                func_80BF4964(this);
                Actor_UpdateVelocityWithGravity(&this->actor);
                Actor_UpdatePos(&this->actor);
                func_80BF416C(this, globalCtx);
                return;
            }
        }
        func_80BF4934(this);
    }
}

void EnRg_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnRg* this = THIS;

    if (gSaveContext.entranceIndex == 0xD010) {
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
        SkelAnime_InitFlex(globalCtx, &this->skelAnime, &object_oF1d_map_Skel_011AC8, NULL, this->jointTable,
                           this->morphTable, 18);

        this->unk_334 = -1;
        func_80BF409C(this, 0);
        this->skelAnime.curFrame = this->skelAnime.endFrame;

        Collider_InitAndSetSphere(globalCtx, &this->collider2, &this->actor, &sSphereInit);
        Collider_InitAndSetCylinder(globalCtx, &this->collider1, &this->actor, &sCylinderInit);
        this->collider2.dim.worldSphere.radius = this->collider2.dim.modelSphere.radius;
        CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

        Effect_Add(globalCtx, &this->unk_340, EFFECT_TIRE_MARK, 0, 0, &D_80BF59F0);

        this->path = func_8013BEDC(globalCtx, ENRG_GET_7F80(&this->actor), 255, &this->unk_33C);
        if (this->path != NULL) {
            this->unk_33C = 1;
        }

        this->actor.flags &= ~ACTOR_FLAG_1;
        this->unk_310 = 8;
        this->actor.gravity = -1.0f;
        func_8013AED4(&this->unk_310, 3, 7);

        if (!(gSaveContext.weekEventReg[12] & 2)) {
            this->unk_318 = Rand_S16Offset(30, 30);
        }

        this->unk_31C = 0;
        this->unk_31E = 0;
        this->actionFunc = func_80BF4EBC;
    } else {
        Actor_MarkForDeath(&this->actor);
    }
}

void EnRg_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnRg* this = THIS;

    Collider_DestroyCylinder(globalCtx, &this->collider1);
    Collider_DestroySphere(globalCtx, &this->collider2);
    Effect_Destroy(globalCtx, this->unk_340);
}

void EnRg_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnRg* this = THIS;

    func_80BF3F14(this, globalCtx);

    this->actionFunc(this, globalCtx);

    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (!(this->unk_310 & 0x10)) {
        func_80BF40F4(this);
        func_80BF3FF8(this);
        func_80BF4024(this, globalCtx);
    }

    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 30.0f, 20.0f, 0.0f, 0x1D);

    if (this->actor.floorHeight <= BGCHECK_Y_MIN) {
        Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
    }

    func_80BF3ED4(this, globalCtx);

    if (!func_801690CC(globalCtx)) {
        func_80BF3C64(this);
    }
}

void func_80BF547C(EnRg* this, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_8012C28C(globalCtx->state.gfxCtx);
    Matrix_InsertTranslation(this->actor.world.pos.x, this->actor.world.pos.y + this->actor.shape.yOffset,
                             this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateY(this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_InsertTranslation(0.0f, -this->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
    Matrix_InsertZRotation_s(this->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_InsertTranslation(0.0f, this->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    Matrix_InsertXRotation_s(this->actor.shape.rot.x, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, object_oF1d_map_DL_0091A8);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

s32 func_80BF5588(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnRg* this = THIS;
    s32 phi_v0;

    switch (limbIndex) {
        case 10:
            phi_v0 = 0;
            break;

        case 11:
            phi_v0 = 1;
            break;

        case 14:
            phi_v0 = 2;
            break;

        default:
            phi_v0 = 9;
            break;
    }

    if (((this->unk_310 & 8) != 0) && (phi_v0 < 9)) {
        rot->y += (s16)(Math_SinS(this->unk_32E[phi_v0]) * 200.0f);
        rot->z += (s16)(Math_CosS(this->unk_328[phi_v0]) * 200.0f);
    }

    return false;
}

void EnRg_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnRg* this = THIS;

    if (!(this->unk_310 & 0x10)) {
        OPEN_DISPS(globalCtx->state.gfxCtx);

        func_8012C28C(globalCtx->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80BF59F8[this->unk_31E]));

        SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable,
                              this->skelAnime.dListCount, func_80BF5588, NULL, &this->actor);

        CLOSE_DISPS(globalCtx->state.gfxCtx);
    } else {
        func_80BF547C(this, globalCtx);
    }
    func_80BF3DA0(this, globalCtx);
}
