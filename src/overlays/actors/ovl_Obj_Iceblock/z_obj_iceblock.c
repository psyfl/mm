/*
 * File: z_obj_iceblock.c
 * Overlay: ovl_Obj_Iceblock
 * Description: Ice Block That Appears After Freezing Enemy
 */

#include "z_obj_iceblock.h"
#include "objects/object_ice_block/object_ice_block.h"

#define FLAGS 0x00000010

#define THIS ((ObjIceblock*)thisx)

void ObjIceblock_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjIceblock_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjIceblock_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjIceblock_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80A257A0(ObjIceblock* this);
void func_80A257B4(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A25824(ObjIceblock* this);
void func_80A2586C(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A25978(ObjIceblock* this);
void func_80A25994(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A25A8C(ObjIceblock* this);
void func_80A25AA8(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A25BA0(ObjIceblock* this);
void func_80A25BBC(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A25C5C(ObjIceblock* this);
void func_80A25C70(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A25CF4(ObjIceblock* this);
void func_80A25D28(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A25E3C(ObjIceblock* this);
void func_80A25E50(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A25FA0(ObjIceblock* this);
void func_80A25FD4(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A260E8(ObjIceblock* this);
void func_80A26144(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A262BC(ObjIceblock* this);
void func_80A262EC(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A26574(ObjIceblock* this);
void func_80A265C0(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A266E0(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A26B64(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A26B74(ObjIceblock* this, GlobalContext* globalCtx);
void func_80A26BF8(ObjIceblock* this, GlobalContext* globalCtx);

const ActorInit Obj_Iceblock_InitVars = {
    ACTOR_OBJ_ICEBLOCK,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_ICE_BLOCK,
    sizeof(ObjIceblock),
    (ActorFunc)ObjIceblock_Init,
    (ActorFunc)ObjIceblock_Destroy,
    (ActorFunc)ObjIceblock_Update,
    (ActorFunc)ObjIceblock_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000800, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 44, 62, -31, { 0, 0, 0 } },
};

static AnimatedMaterial* D_80A26E7C = NULL;

s16 func_80A23090(s16 arg0, s16 arg1, s16 arg2) {
    if (arg0 >= 0) {
        if (arg2 < arg0) {
            return arg2;
        } else if (arg0 < arg1) {
            return arg1;
        }
    } else if (arg0 < -arg2) {
        return -arg2;
    } else if (-arg1 < arg0) {
        return -arg1;
    }

    return arg0;
}

void func_80A2311C(Vec3f* arg0, Vec3f* arg1, s16 arg2) {
    f32 sp1C = Math_SinS(arg2);
    f32 sp18 = Math_CosS(arg2);

    arg0->x = (arg1->z * sp1C) + (arg1->x * sp18);
    arg0->y = arg1->y;
    arg0->z = (arg1->z * sp18) - (arg1->x * sp1C);
}

void func_80A2319C(ObjIceblock* this, f32 arg1) {
    s32 i;
    s16 temp_s1 = (u32)Rand_Next() >> 0x10;
    s16 temp_s2 = (u32)Rand_Next() >> 0x10;
    ObjIceBlockUnkStruct* ptr;

    for (i = 0; i < ARRAY_COUNT(this->unk_1B4); i++) {
        ptr = &this->unk_1B4[i];
        temp_s1 += Rand_S16Offset(2700, 10000);
        temp_s2 += Rand_S16Offset(2700, 10000);
        ptr->unk_00 = temp_s1;
        ptr->unk_02 = temp_s2;
        ptr->unk_04 = ((Rand_ZeroOne() * 0.2f) + 0.9f) * arg1;
        ptr->unk_08 = ((Rand_ZeroOne() * 0.2f) + 0.9f) * arg1;
        ptr->unk_0C = ((Rand_ZeroOne() * 0.2f) + 0.9f) * arg1;
    }
}

void func_80A232C4(ObjIceblock* this, s32 arg1) {
    static s16 D_80A26E80[] = { 1, -1, 0, 0 };
    static s16 D_80A26E88[] = { 0, 0, 1, -1 };

    this->unk_276 += D_80A26E80[arg1];
    this->unk_278 += D_80A26E88[arg1];

    if ((arg1 == 0) || (arg1 == 1)) {
        this->unk_264 = &this->dyna.actor.world.pos.x;
        this->unk_268 = this->dyna.actor.home.pos.x + (this->unk_276 * 60);
    } else {
        this->unk_264 = &this->dyna.actor.world.pos.z;
        this->unk_268 = this->dyna.actor.home.pos.z + (this->unk_278 * 60);
    }
    this->unk_26C = arg1;
}

void func_80A23370(ObjIceblock* this, s32 arg1) {
    if ((arg1 == 0) || (arg1 == 1)) {
        this->unk_264 = &this->dyna.actor.world.pos.x;
    } else {
        this->unk_264 = &this->dyna.actor.world.pos.z;
    }
    this->unk_26C = arg1;
}

void func_80A2339C(GlobalContext* globalCtx, Vec3f* arg1, f32 arg2, f32 arg3, s32 arg4) {
    Vec3f spBC;
    Vec3f spB0;
    Vec3f spA4;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    s32 i;
    f32 sp88;
    s16 temp_s4;
    s16 phi_s0;
    s16 temp;

    spA4.z = 0.0f;
    spA4.x = 0.0f;
    temp_f24 = ((Rand_ZeroOne() * 0.1656f) + 0.3312f) * arg3;
    sp88 = 10.0f * arg3;

    for (i = 0, phi_s0 = 0; i < arg4; i++, phi_s0 += temp) {
        temp = 0x10000 / arg4;

        temp_f20 = ((Rand_ZeroOne() * 0.4f) + 0.6f) * (arg2 * 3.5f);
        temp_s4 = temp_f20 * 100.0f;
        temp_f22 = ((Rand_ZeroOne() * 0.7f) + 0.3f) * arg2 * 300.0f;

        spBC.x = Math_SinS(Rand_S16Offset(phi_s0, temp)) * temp_f22;
        spBC.y = (Rand_ZeroOne() - 0.5f) * (600.0f * arg2);
        spBC.z = Math_CosS(Rand_S16Offset(phi_s0, temp)) * temp_f22;

        spB0.x = spBC.x * temp_f24;
        spB0.y = ((Rand_ZeroOne() * 1.2f) + 0.3f + (temp_f20 * 4.0f)) * sp88;
        spB0.z = spBC.z * temp_f24;

        spBC.x += arg1->x;
        spBC.y += arg1->y;
        spBC.z += arg1->z;

        spA4.y = -0.8f - (temp_f20 * 18.0f);

        EffectSsIceBlock_Spawn(globalCtx, &spBC, &spB0, &spA4, temp_s4);
    }
}

void func_80A23690(ObjIceblock* this) {
    this->dyna.actor.velocity.y += this->dyna.actor.gravity;
    if (this->dyna.actor.velocity.y < this->dyna.actor.minVelocityY) {
        this->dyna.actor.velocity.y = this->dyna.actor.minVelocityY;
    }
    this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;
}

s32 func_80A236D4(ObjIceblock* this, Vec3f* arg1) {
    f32 sp2C;
    f32 sp28;
    s16 sp26;
    s32 sp20;

    sp26 = Math_Vec3f_Yaw(&this->dyna.actor.world.pos, arg1);
    sp2C = Math_SinS(sp26) * this->dyna.actor.speedXZ;
    sp2C = fabsf(sp2C) + 0.01f;

    sp28 = Math_CosS(sp26) * this->dyna.actor.speedXZ;
    sp28 = fabsf(sp28) + 0.01f;

    sp20 = Math_StepToF(&this->dyna.actor.world.pos.x, arg1->x, sp2C);
    sp20 &= 1;
    sp20 &= Math_StepToF(&this->dyna.actor.world.pos.z, arg1->z, sp28);
    return sp20;
}

void func_80A237A4(ObjIceblock* this) {
    s32 pad[2];
    ObjIceBlockUnkStruct4* ptr = &this->unk_27C;
    f32 sp20;

    ptr->unk_10 += ptr->unk_0C;
    ptr->unk_12 += ptr->unk_0E;
    Math_StepToF(&this->unk_27C.unk_08, this->unk_27C.unk_04, 0.08f);

    if (this->unk_1B0 & 0x20) {
        Math_StepToF(&ptr->unk_00, -5.0f, (Math_CosS(fabsf(ptr->unk_00) * 3276.8f) * 0.3f) + 0.02f);
    } else {
        Math_StepToF(&ptr->unk_00, 0.0f, (Math_SinS(fabsf(ptr->unk_00) * 3276.8f) * 0.3f) + 0.02f);
    }

    sp20 = Math_SinS(ptr->unk_10);
    this->dyna.actor.world.pos.y =
        (Math_SinS(ptr->unk_12) * ptr->unk_08 * 0.5f) +
        ((this->unk_244 - ((600.0f * this->dyna.actor.scale.y) - 90.0f)) + (sp20 * ptr->unk_08)) + ptr->unk_00;
}

void func_80A23938(ObjIceblock* this2) {
    ObjIceblock* this = this2;

    if (this->unk_1B0 & 0x80) {
        Math_ScaledStepToS(&this->dyna.actor.shape.rot.x, 0, 400);
        Math_ScaledStepToS(&this->dyna.actor.shape.rot.z, 0, 400);
    } else {
        ObjIceBlockUnkStruct4* ptr = &this->unk_27C;
        f32 phi_f0;
        s16 temp;

        if (this->unk_1B0 & 0x20) {
            phi_f0 = 0.3f;
            ptr->unk_14 = this->dyna.actor.yawTowardsPlayer;
        } else {
            phi_f0 = 0.08f;
        }

        Math_StepToF(&ptr->unk_1C, phi_f0, 0.04f);
        ptr->unk_22 += (s16)(ptr->unk_20 * -0.02f);
        ptr->unk_22 = func_80A23090(ptr->unk_22, 50, 800);
        ptr->unk_20 += ptr->unk_22;

        temp = ptr->unk_16 - ptr->unk_14;

        ptr->unk_18 +=
            (s16)(temp * -0.04f * this->dyna.actor.xzDistToPlayer * this->dyna.actor.scale.x * (1.0f / 600.0f));
        ptr->unk_18 = func_80A23090(ptr->unk_18, 50, 800);
        ptr->unk_16 += ptr->unk_18;

        this->dyna.actor.shape.rot.x = Math_CosS(ptr->unk_16) * ptr->unk_20 * ptr->unk_1C;
        this->dyna.actor.shape.rot.x = CLAMP(this->dyna.actor.shape.rot.x, -2000, 2000);

        this->dyna.actor.shape.rot.z = -Math_SinS(ptr->unk_16) * ptr->unk_20 * ptr->unk_1C;
        this->dyna.actor.shape.rot.z = CLAMP(this->dyna.actor.shape.rot.z, -2000, 2000);
    }
}

void func_80A23B88(ObjIceblock* this) {
    f32 f4;
    f32 f2;
    f32 f14;
    f32 f16;
    f32 f6;

    f14 = this->dyna.actor.world.pos.x - this->dyna.actor.home.pos.x;
    f2 = (f14 >= 0.0f) ? (0.5f) : (-0.5f);
    f4 = (((s32)(f2 + f14)) / 30) * 30;
    f14 -= f4;
    if (fabsf(f14) < 3.0f) {
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + f4;
    }

    f16 = this->dyna.actor.world.pos.z - this->dyna.actor.home.pos.z;
    f2 = (f16 >= 0.0f) ? (0.5f) : (-0.5f);
    f6 = (((s32)(f2 + f16)) / 30) * 30;
    f16 -= f6;
    if (fabsf(f16) < 3.0f) {
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + f6;
    }
}

s32 func_80A23D08(ObjIceblock* this, GlobalContext* globalCtx) {
    static ObjIceBlockUnkStruct3 D_80A26E90[] = {
        { -300.0f, 300.0f }, { 300.0f, 300.0f }, { -300.0f, -300.0f }, { 300.0f, -300.0f }, { 0.0f, 0.0f },
    };
    static ObjIceBlockUnkStruct3 D_80A26EB8[] = {
        { 0.5f, -0.5f }, { -0.5f, -0.5f }, { 0.5f, 0.5f }, { -0.5f, 0.5f }, { 0.0f, 0.0f },
    };
    s32 pad3;
    ObjIceBlockUnkStruct2* ptr;
    WaterBox* spC4;
    s32 i;
    s32 spBC;
    s32 spB8;
    s32 pad;
    f32 phi_f20;
    Vec3f spA4;
    Vec3f sp98;
    s32 sp94 = 0;
    f32 phi_f22;

    sp98.y = this->dyna.actor.world.pos.y + 40.0f;
    spBC = spB8 = -1;
    phi_f20 = BGCHECK_Y_MIN;
    phi_f22 = BGCHECK_Y_MIN;

    for (i = 0; i < ARRAY_COUNT(this->unk_1F4); i++) {
        ptr = &this->unk_1F4[i];

        ptr->unk_00 = NULL;
        ptr->unk_04 = BGCHECK_Y_MIN;
        ptr->unk_08 = 50;

        sp98.x = (D_80A26E90[i].unk_00 * this->dyna.actor.scale.x) + D_80A26EB8[i].unk_00;
        sp98.z = (D_80A26E90[i].unk_04 * this->dyna.actor.scale.z) + D_80A26EB8[i].unk_04;

        func_80A2311C(&spA4, &sp98, this->dyna.actor.shape.rot.y);
        spA4.x += this->dyna.actor.world.pos.x;
        spA4.z += this->dyna.actor.world.pos.z;

        ptr->unk_04 =
            BgCheck_EntityRaycastFloor6(&globalCtx->colCtx, &ptr->unk_00, &ptr->unk_08, &this->dyna.actor, &spA4, 0.0f);
        if (ptr->unk_04 > BGCHECK_Y_MIN + 1) {
            sp94 = 1;
            if (phi_f22 < ptr->unk_04) {
                spBC = i;
                phi_f22 = ptr->unk_04;
            }
        }

        if (WaterBox_GetSurface1_2(globalCtx, &globalCtx->colCtx, spA4.x, spA4.z, &ptr->unk_0C, &spC4)) {
            if (phi_f20 < ptr->unk_0C) {
                spB8 = i;
                phi_f20 = ptr->unk_0C;
            }
        } else {
            ptr->unk_0C = BGCHECK_Y_MIN;
        }
    }

    if (spBC >= 0) {
        this->dyna.actor.floorPoly = this->unk_1F4[spBC].unk_00;
        this->dyna.actor.floorHeight = this->unk_1F4[spBC].unk_04;
        this->dyna.actor.floorBgId = this->unk_1F4[spBC].unk_08;
    } else {
        this->dyna.actor.floorPoly = NULL;
        this->dyna.actor.floorHeight = BGCHECK_Y_MIN;
        this->dyna.actor.floorBgId = BGCHECK_SCENE;
    }

    if (spB8 >= 0) {
        this->unk_244 = this->unk_1F4[spB8].unk_0C;
    } else {
        this->unk_244 = BGCHECK_Y_MIN;
    }

    return sp94;
}

s32 func_80A23F90(ObjIceblock* this, GlobalContext* globalCtx) {
    this->unk_1B0 &= ~4;

    if (func_80A23D08(this, globalCtx)) {
        f32 phi_f2 = BGCHECK_Y_MIN;
        s32 ret = false;
        s32 i;
        ObjIceBlockUnkStruct2* ptr;

        for (i = 0; i < ARRAY_COUNT(this->unk_1F4); i++) {
            ptr = &this->unk_1F4[i];
            if (((this->dyna.actor.world.pos.y - 10.0f) < ptr->unk_04) && (phi_f2 < ptr->unk_04)) {
                phi_f2 = ptr->unk_04;
                ret = true;
            }
        }

        if (ret) {
            this->dyna.actor.world.pos.y = phi_f2;
            this->unk_1B0 |= 4;
            this->dyna.actor.velocity.y = 0.0f;
        }
        return ret;
    }
    return false;
}

s32 func_80A24118(ObjIceblock* this, GlobalContext* globalCtx, f32 arg2, Vec3f* arg3) {
    static ObjIceBlockUnkStruct3 D_80A26EE0[] = {
        { -300.0f, -300.0f }, { 300.0f, -300.0f }, { -300.0f, 600.0f }, { 300.0f, 600.0f }, { 0.0f, 0.0f },
    };
    static ObjIceBlockUnkStruct3 D_80A26F08[] = {
        { 1.0f, 1.0f }, { -1.0f, 1.0f }, { 1.0f, -1.0f }, { -1.0f, -1.0f }, { 0.0f, 0.0f },
    };
    f32 temp_f20;
    f32 spF0;
    f32 spEC;
    f32 phi_f20;
    Vec3f spDC;
    Vec3f spD0;
    Vec3f spC4;
    Vec3f spB8;
    s32 pad[2];
    s32 spAC;
    CollisionPoly* spA8;
    s32 ret;
    s32 i;
    s16 sp9E;
    f32 temp_f24;
    f32 temp_f26;

    sp9E = this->dyna.yRotation;
    if (this->dyna.pushForce < 0.0f) {
        sp9E -= 0x8000;
    }

    spF0 = Math_SinS(sp9E);
    spEC = Math_CosS(sp9E);
    temp_f20 = Math3D_Distance(&this->dyna.actor.world.pos, &this->dyna.actor.prevPos) +
               (300.0f * this->dyna.actor.scale.z) + arg2;
    temp_f24 = temp_f20 * spF0;
    temp_f26 = temp_f20 * spEC;
    ret = false;
    phi_f20 = FLT_MAX;

    for (i = 0; i < ARRAY_COUNT(D_80A26EE0); i++) {
        spC4.x = (D_80A26EE0[i].unk_00 * this->dyna.actor.scale.x) + D_80A26F08[i].unk_00;
        spC4.y = (D_80A26EE0[i].unk_04 * this->dyna.actor.scale.y) + D_80A26F08[i].unk_04 +
                 (this->dyna.actor.shape.yOffset * this->dyna.actor.scale.y);
        spC4.z = 0.0f;

        func_80A2311C(&spDC, &spC4, sp9E);

        spDC.x += this->dyna.actor.prevPos.x;
        spDC.y += this->dyna.actor.prevPos.y;
        spDC.z += this->dyna.actor.prevPos.z;

        spD0.x = temp_f24 + spDC.x;
        spD0.y = spDC.y;
        spD0.z = temp_f26 + spDC.z;

        if (BgCheck_EntityLineTest3(&globalCtx->colCtx, &spDC, &spD0, &spB8, &spA8, 1, 0, 0, 1, &spAC,
                                    &this->dyna.actor, 0.0f)) {
            temp_f20 = Math3D_Vec3fDistSq(&spDC, &spB8);
            if (temp_f20 < phi_f20) {
                phi_f20 = temp_f20;
                ret = true;
                Math_Vec3f_Diff(&spB8, &spD0, arg3);
            }
        }
    }
    return ret;
}

s32 func_80A24384(ObjIceblock* this, GlobalContext* globalCtx) {
    Vec3f sp1C;

    if (func_80A24118(this, globalCtx, 0.0f, &sp1C)) {
        this->dyna.actor.world.pos.x += sp1C.x;
        this->dyna.actor.world.pos.z += sp1C.z;
        return true;
    }
    return false;
}

s32 func_80A243E0(ObjIceblock* this, GlobalContext* globalCtx, Vec3f* arg0) {
    static f32 D_80A26F30[] = { -300.0f, 300.0f };
    static f32 D_80A26F38[] = { 1.0f, -1.0f };
    s32 i;
    f32 sp100;
    f32 spFC;
    f32 phi_f22;
    Vec3f spEC;
    Vec3f spE0;
    Vec3f spD4;
    Vec3f spC8;
    s16 temp_s6;
    f32 temp_f28;
    s32 spBC;
    CollisionPoly* spB8;
    s32 ret;
    f32 temp_f30;
    f32 temp_f0;
    f32 temp_f12;

    temp_s6 = this->dyna.yRotation;
    if (this->dyna.pushForce < 0.0f) {
        temp_s6 -= 0x8000;
    }

    sp100 = Math_SinS(temp_s6);
    spFC = Math_CosS(temp_s6);

    temp_f0 = ((Math3D_Distance(&this->dyna.actor.world.pos, &this->dyna.actor.prevPos) +
                (300.0f * this->dyna.actor.scale.z)) +
               2.0f);
    temp_f12 = -temp_f0;
    ret = false;
    temp_f28 = temp_f12 * sp100;
    temp_f30 = temp_f12 * spFC;
    phi_f22 = FLT_MAX;

    for (i = 0; i < ARRAY_COUNT(D_80A26F30); i++) {
        spD4.x = (D_80A26F30[i] * this->dyna.actor.scale.x) + D_80A26F38[i];
        spD4.y = -10.0f;
        spD4.z = 0.0f;

        func_80A2311C(&spEC, &spD4, temp_s6);

        spEC.x += this->dyna.actor.world.pos.x;
        spEC.y += this->dyna.actor.world.pos.y;
        spEC.z += this->dyna.actor.world.pos.z;

        spE0.x = temp_f28 + spEC.x;
        spE0.y = spEC.y;
        spE0.z = temp_f30 + spEC.z;

        if (BgCheck_EntityLineTest3(&globalCtx->colCtx, &spEC, &spE0, &spC8, &spB8, 1, 0, 0, 1, &spBC,
                                    &this->dyna.actor, 0.0f)) {
            temp_f12 = Math3D_Vec3fDistSq(&spEC, &spC8);
            if (temp_f12 < phi_f22) {
                phi_f22 = temp_f12;
                ret = true;
                arg0->x = (spC8.x - spEC.x) + (300.0f * this->dyna.actor.scale.z * sp100);
                arg0->y = 0.0f;
                arg0->z = (spC8.z - spEC.z) + (300.0f * this->dyna.actor.scale.z * spFC);
            }
        }
    }
    return ret;
}

s32 func_80A24680(ObjIceblock* this, GlobalContext* globalCtx) {
    Vec3f sp1C;

    if (func_80A243E0(this, globalCtx, &sp1C)) {
        this->dyna.actor.world.pos.x += sp1C.x;
        this->dyna.actor.world.pos.z += sp1C.z;
        return true;
    }
    return false;
}

s32 func_80A246D8(ObjIceblock* this, GlobalContext* globalCtx, Vec3f* arg2) {
    static ObjIceBlockUnkStruct3 D_80A26F40[] = {
        { -300.0f, -300.0f }, { 300.0f, -300.0f }, { -300.0f, 600.0f }, { 300.0f, 600.0f }, { 0.0f, 0.0f },
    };
    static ObjIceBlockUnkStruct3 D_80A26F68[] = {
        { 1.0f, 1.0f }, { -1.0f, 1.0f }, { 1.0f, -1.0f }, { -1.0f, -1.0f }, { 0.0f, 0.0f },
    };
    s32 pad;
    s32 i;
    s32 j;
    Vec3f spC0;
    Vec3f spB4;
    Vec3f spA8;
    Vec3f sp9C;
    s32 sp98;
    CollisionPoly* sp94;
    ObjIceblock* temp_v0;
    f32 temp_f20 = (this->dyna.actor.scale.z * 300.0f) + 2.0f;
    s16 phi_s3;
    s32 ret = false;

    for (i = 0, phi_s3 = 0; i < 4; i++, phi_s3 += 0x4000) {
        for (j = 0; j < ARRAY_COUNT(D_80A26F40); j++) {
            spA8.x = (D_80A26F40[j].unk_00 * this->dyna.actor.scale.x) + D_80A26F68[j].unk_00;
            spA8.y = ((D_80A26F40[j].unk_04 * this->dyna.actor.scale.y) + D_80A26F68[j].unk_04) +
                     (this->dyna.actor.shape.yOffset * this->dyna.actor.scale.y);
            spA8.z = 0.0f;

            func_80A2311C(&spC0, &spA8, phi_s3);

            spC0.x += arg2->x;
            spC0.y += arg2->y;
            spC0.z += arg2->z;

            spB4.x = (Math_SinS(phi_s3) * temp_f20) + spC0.x;
            spB4.y = spC0.y;
            spB4.z = (Math_CosS(phi_s3) * temp_f20) + spC0.z;

            if (BgCheck_EntityLineTest3(&globalCtx->colCtx, &spC0, &spB4, &sp9C, &sp94, 1, 0, 0, 1, &sp98,
                                        &this->dyna.actor, 0.0f)) {
                ret = true;
                temp_v0 = (ObjIceblock*)DynaPoly_GetActor(&globalCtx->colCtx, sp98);
                if ((temp_v0 != NULL) && (temp_v0->dyna.actor.id == ACTOR_OBJ_ICEBLOCK) && (temp_v0->unk_2B0 == 3)) {
                    temp_v0->unk_1B0 |= 0x80;
                }
            }
        }
    }

    return ret;
}

void func_80A2491C(ObjIceblock* this) {
    Math_Vec3f_Copy(&this->dyna.actor.home.pos, &this->dyna.actor.world.pos);
    this->unk_278 = 0;
    this->unk_276 = this->unk_278;
}

s32 func_80A24954(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 pad[2];
    s8 sp27 = this->unk_2B0;
    f32 temp = (this->dyna.actor.scale.y * 600.0f) - 90.0f;

    if ((this->unk_244 > BGCHECK_Y_MIN + 1) && (temp < (this->unk_244 - this->dyna.actor.world.pos.y))) {
        func_80A262BC(this);
        this->unk_2B0 = 3;
    } else if (this->unk_1B0 & 4) {
        if (func_800C99D4(&globalCtx->colCtx, this->dyna.actor.floorPoly, this->dyna.actor.floorBgId) == 5) {
            func_80A25FA0(this);
            this->unk_2B0 = 2;
        } else {
            func_80A25CF4(this);
            this->unk_2B0 = 1;
        }
    }
    return sp27 != this->unk_2B0;
}

void func_80A24A48(ObjIceblock* this, GlobalContext* globalCtx) {
    if (!(this->unk_1B0 & 0x10) && !(this->collider.base.ocFlags1 & OC1_HIT)) {
        func_800C6314(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
        this->unk_1B0 |= 0x10;
    }
}

void func_80A24AA8(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 i;
    f32 x;
    ObjIceblock* temp_v0;

    if (this->unk_1B0 & 4) {
        for (i = 0; i < ARRAY_COUNT(this->unk_1F4); i++) {
            temp_v0 = (ObjIceblock*)DynaPoly_GetActor(&globalCtx->colCtx, ((void)0, this->unk_1F4[i]).unk_08);
            if ((temp_v0 != NULL) && (temp_v0->dyna.actor.id == ACTOR_OBJ_ICEBLOCK)) {
                x = this->dyna.actor.world.pos.y - this->unk_1F4[i].unk_04;

                if (fabsf(x) < 0.1f) {
                    temp_v0->unk_1B0 |= 2;
                }
            }
        }
    }
}

void func_80A24B74(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f sp20;

    if (this->dyna.actor.flags & 0x40) {
        if (1) {}
        sp20.x = this->dyna.actor.world.pos.x;
        sp20.y = this->unk_244;
        sp20.z = this->dyna.actor.world.pos.z;
        EffectSsGRipple_Spawn(globalCtx, &sp20, 480, 880, 0);
    }
}

void func_80A24BDC(ObjIceblock* this, GlobalContext* globalCtx, f32 arg2, f32 arg3, s32 arg4) {
    s32 i;
    Vec3f sp88;
    f32 temp_f20;
    s16 temp_f22;
    s16 phi_s0;
    s32 phi_s1 = 0;

    if (this->dyna.actor.flags & 0x40) {
        sp88.y = this->unk_244;
        temp_f22 = 0x10000 / arg4;

        for (i = 0, phi_s0 = 0; i < arg4; i++, phi_s0 += temp_f22) {
            temp_f20 = ((Rand_ZeroOne() * 5.0f) + 40.0f) * arg2;
            sp88.x = (Math_SinS((s32)(Rand_ZeroOne() * temp_f22) + phi_s0) * temp_f20) + this->dyna.actor.world.pos.x;
            sp88.z = (Math_CosS((s32)(Rand_ZeroOne() * temp_f22) + phi_s0) * temp_f20) + this->dyna.actor.world.pos.z;
            EffectSsGSplash_Spawn(globalCtx, &sp88, NULL, NULL, 0, ((Rand_ZeroOne() * 60.0f) + 320.0f) * arg3);
        }
    }
}

void func_80A24DC4(ObjIceblock* this) {
    this->unk_2A2 = 0;
}

void func_80A24DD0(ObjIceblock* this, GlobalContext* globalCtx) {
    static Vec3f D_80A26F90 = { 0.0f, 0.3f, 0.0f };
    static Color_RGBA8 D_80A26F9C = { 250, 250, 250, 255 };
    static Color_RGBA8 D_80A26FA0 = { 180, 180, 180, 255 };
    s32 pad;
    Vec3f spA8;
    Vec3f sp9C;
    s32 i;
    f32 temp_f20;
    f32 phi_f22;
    s32 pad2;
    s16 sp8A;

    sp8A = this->dyna.yRotation;
    if (this->dyna.pushForce < 0.0f) {
        sp8A -= 0x8000;
    }

    this->unk_2A2++;
    if (this->dyna.actor.flags & 0x40) {
        if (this->unk_2A2 >= 0x2E) {
            phi_f22 = 1.0f;
        } else {
            phi_f22 = (this->unk_2A2 * 0.017777778f) + 0.2f;
        }
        sp9C.y = 12.0f;

        for (i = 0; i < 2; i++) {
            if (phi_f22 < (Rand_ZeroOne() * 1.2f)) {
                continue;
            }

            this->unk_2A4 += Rand_ZeroOne();
            if (this->unk_2A4 > 1.0f) {
                this->unk_2A4 -= 1.0f;
            }

            sp9C.x = (this->unk_2A4 - 0.5f) * 600.0f * this->dyna.actor.scale.x;
            sp9C.z = (this->dyna.actor.scale.z * -300.0f) + 4.0f;

            func_80A2311C(&spA8, &sp9C, sp8A);

            spA8.x += this->dyna.actor.world.pos.x;
            spA8.y += this->dyna.actor.world.pos.y;
            spA8.z += this->dyna.actor.world.pos.z;

            temp_f20 = ((Rand_ZeroOne() * 800.0f) + (1600.0f * this->dyna.actor.scale.x)) * phi_f22;
            func_800B0E48(globalCtx, &spA8, &gZeroVec3f, &D_80A26F90, &D_80A26F9C, &D_80A26FA0, temp_f20,
                          (Rand_ZeroOne() * 20.0f) + 30.0f);
        }
    }
}

void func_80A2508C(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f sp40;
    Vec3f sp34;
    f32 temp_f0;
    s32 temp_v0;

    if (this->dyna.actor.flags & 0x40) {
        temp_v0 = (s32)(this->dyna.actor.scale.y * 130.0f) - 3;
        if (temp_v0 > 0) {
            this->unk_2AC += temp_v0;
        }

        if (this->unk_2AC >= 10) {
            this->unk_2AC = 0;
            sp34.x = (Rand_ZeroOne() * 600.0f) - 300.0f;
            sp34.y = 600.0f;
            sp34.z = (Rand_ZeroOne() * 600.0f) - 300.0f;

            temp_f0 = sqrtf(this->dyna.actor.scale.y + 0.01f);
            sp40.x = sp34.x * 0.006935f * temp_f0;
            sp40.y = 2.0f;
            sp40.z = sp34.z * 0.006935f * temp_f0;

            sp34.x = (this->dyna.actor.scale.x * sp34.x) + this->dyna.actor.world.pos.x;
            sp34.y = (this->dyna.actor.scale.y * sp34.y) + this->dyna.actor.world.pos.y;
            sp34.z = (this->dyna.actor.scale.z * sp34.z) + this->dyna.actor.world.pos.z;

            if ((this->unk_244 - 3.0f) < sp34.y) {
                EffectSsIceSmoke_Spawn(globalCtx, &sp34, &sp40, &gZeroVec3f,
                                       (s32)(this->dyna.actor.scale.y * 1300.0f) + 60);
            }
        }
    }
}

s32 func_80A25238(ObjIceblock* this) {
    s16 phi_v0;

    if (fabsf(this->dyna.pushForce) > 0.1f) {
        if (this->dyna.pushForce > 0.0f) {
            phi_v0 = this->dyna.yRotation + 0x2000;
        } else {
            phi_v0 = this->dyna.yRotation - 0x6000;
        }

        if (phi_v0 < -0x4000) {
            return 3;
        }

        if (phi_v0 < 0) {
            return 1;
        }

        if (phi_v0 < 0x4000) {
            return 2;
        }
        return 0;
    }
    return -1;
}

void func_80A252DC(ObjIceblock* this, s32 arg1) {
    s32 phi_v0;
    s32 i;

    if (this->unk_2B0 == 1) {
        phi_v0 = true;
    } else if (this->unk_2B0 == 2) {
        phi_v0 = (this->dyna.pushForce > 0.0f);
    } else {
        phi_v0 = false;
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_26E); i++) {
        if ((arg1 == i) && phi_v0) {
            this->unk_26E[i]++;
        } else if (this->unk_26E[i] > 0) {
            this->unk_26E[i]--;
        }
    }
}

void func_80A25404(ObjIceblock* this) {
    this->unk_26E[1] = 0;
    this->unk_26E[2] = 0;
    this->unk_26E[3] = 0;
    this->unk_26E[0] = 0;
}

void func_80A2541C(ObjIceblock* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    player->stateFlags2 &= ~0x10;
    this->dyna.pushForce = 0.0f;
}

void func_80A25440(ObjIceblock* this) {
    s32 pad[2];
    MtxF sp40;
    Vec3f sp34;
    Vec3f sp28;
    f32 sp24;
    Actor* temp_s1 = this->dyna.actor.parent;

    if (temp_s1 != NULL) {
        if (this->unk_2B0 == 3) {
            sp24 = this->dyna.actor.shape.yOffset * this->dyna.actor.scale.y;

            Matrix_RotateY(this->dyna.actor.shape.rot.y, MTXMODE_NEW);
            Matrix_InsertXRotation_s(this->dyna.actor.shape.rot.x, MTXMODE_APPLY);
            Matrix_InsertZRotation_s(this->dyna.actor.shape.rot.z, MTXMODE_APPLY);

            sp34.x = this->unk_248.x;
            sp34.y = this->unk_248.y - sp24;
            sp34.z = this->unk_248.z;

            Matrix_MultiplyVector3fByState(&sp34, &sp28);

            sp34.x = this->dyna.actor.world.pos.x;
            sp34.y = this->dyna.actor.world.pos.y + sp24;
            sp34.z = this->dyna.actor.world.pos.z;

            Math_Vec3f_Sum(&sp34, &sp28, &temp_s1->world.pos);
            Matrix_RotateY(this->unk_254.y + this->dyna.actor.home.rot.y, MTXMODE_APPLY);
            Matrix_InsertXRotation_s(this->unk_254.x + this->dyna.actor.home.rot.x, MTXMODE_APPLY);
            Matrix_InsertZRotation_s(this->unk_254.z + this->dyna.actor.home.rot.z, MTXMODE_APPLY);
            Matrix_CopyCurrentState(&sp40);
            func_8018219C(&sp40, &temp_s1->shape.rot, MTXMODE_APPLY);
        } else {
            Math_Vec3f_Sum(&this->dyna.actor.world.pos, &this->unk_248, &temp_s1->world.pos);
        }
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(speedXZ, 16000, ICHAIN_CONTINUE),       ICHAIN_F32_DIV1000(gravity, -1800, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(minVelocityY, -26000, ICHAIN_CONTINUE), ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),      ICHAIN_F32(uncullZoneScale, 150, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 200, ICHAIN_STOP),
};

void ObjIceblock_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    ObjIceblock* this = THIS;
    Actor* parent;
    s32 pad2;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->dyna.actor.shape.rot.x = 0;
    this->dyna.actor.shape.rot.z = 0;
    this->dyna.actor.world.rot.x = 0;
    this->dyna.actor.world.rot.z = 0;
    this->dyna.actor.home.rot.x = 0;
    this->dyna.actor.home.rot.z = 0;
    if (!OBJICEBLOCK_GET_2(&this->dyna.actor)) {
        this->dyna.actor.shape.rot.y = (this->dyna.actor.shape.rot.y + 0x2000) & 0xC000;
        this->dyna.actor.home.rot.y = this->dyna.actor.shape.rot.y;
        this->dyna.actor.world.rot.y = this->dyna.actor.shape.rot.y;
    }

    DynaPolyActor_Init(&this->dyna, 1);
    DynaPolyActor_LoadMesh(globalCtx, &this->dyna, &object_ice_block_Colheader_000438);
    func_800C62BC(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinder(globalCtx, &this->collider, &this->dyna.actor, &sCylinderInit);

    if (OBJICEBLOCK_GET_1(&this->dyna.actor)) {
        this->collider.dim.yShift = -100;
        this->collider.dim.height = 126;
        this->collider.dim.radius = 29;
    }

    if (D_80A26E7C == NULL) {
        D_80A26E7C = Lib_SegmentedToVirtual(&object_ice_block_Matanimheader_000328);
    }

    if (!(this->unk_1B0 & 8)) {
        parent = this->dyna.actor.parent;
        if (parent != NULL) {
            this->unk_1B0 |= 8;
            Math_Vec3f_Diff(&parent->world.pos, &this->dyna.actor.world.pos, &this->unk_248);
            this->unk_254.x = parent->shape.rot.x - this->dyna.actor.home.rot.x;
            this->unk_254.y = parent->shape.rot.y - this->dyna.actor.home.rot.y;
            this->unk_254.z = parent->shape.rot.z - this->dyna.actor.home.rot.z;
        }
    }

    if (OBJICEBLOCK_GET_1(&this->dyna.actor)) {
        this->unk_2B4 = -1.0f;
    }

    this->unk_2B0 = 0;
    this->unkFunc = func_80A26B64;
    func_80A257A0(this);
    this->unk_2AE = 450;
}

void ObjIceblock_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    ObjIceblock* this = THIS;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(globalCtx, &this->collider);
}

void func_80A257A0(ObjIceblock* this) {
    this->actionFunc = func_80A257B4;
}

void func_80A257B4(ObjIceblock* this, GlobalContext* globalCtx) {
    if (ActorCutscene_GetCanPlayNext(this->dyna.actor.cutscene)) {
        ActorCutscene_StartAndSetUnkLinkFields(this->dyna.actor.cutscene, &this->dyna.actor);
        this->unkFunc = func_80A26BF8;
        this->unk_2B1 = 80;
        func_80A25824(this);
    } else {
        ActorCutscene_SetIntentToPlay(this->dyna.actor.cutscene);
    }
}

void func_80A25824(ObjIceblock* this) {
    this->actionFunc = func_80A2586C;
    this->unk_2A0 = 2;
    Actor_SetScale(&this->dyna.actor, 0.01f);
    func_80A2319C(this, this->dyna.actor.scale.x);
}

void func_80A2586C(ObjIceblock* this, GlobalContext* globalCtx) {
    func_80A236D4(this, &this->dyna.actor.home.pos);

    if (this->unk_2A0 > 0) {
        this->unk_2A0--;
        return;
    }

    if (Math_StepToF(&this->dyna.actor.scale.x, 0.1f, 0.02f)) {
        Actor_SetScale(&this->dyna.actor, 0.1f);
        if (OBJICEBLOCK_GET_1(&this->dyna.actor)) {
            this->unk_2B4 = 0.05f;
        }
        func_80A25978(this);
    } else {
        Actor_SetScale(&this->dyna.actor, this->dyna.actor.scale.x);
        this->unk_2A0 = 2;
    }

    func_80A2319C(this, this->dyna.actor.scale.x);

    if (this->dyna.actor.flags & 0x40) {
        func_80A2339C(globalCtx, &this->dyna.actor.world.pos, (this->dyna.actor.scale.x + 0.05f) * 0.6666666f, 1.0f, 3);
    }
}

void func_80A25978(ObjIceblock* this) {
    this->actionFunc = func_80A25994;
    this->unk_2A0 = 4;
}

void func_80A25994(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 pad;
    Vec3f sp30;

    func_80A236D4(this, &this->dyna.actor.home.pos);
    if (this->unk_2A0 > 0) {
        this->unk_2A0--;
        return;
    }

    if (this->dyna.actor.flags & 0x40) {
        func_80A2339C(globalCtx, &this->dyna.actor.world.pos, this->dyna.actor.scale.x, 1.2f, 15);
        if (OBJICEBLOCK_GET_1(&this->dyna.actor)) {
            sp30.x = this->dyna.actor.world.pos.x;
            sp30.y = this->dyna.actor.world.pos.y - 30.0f;
            sp30.z = this->dyna.actor.world.pos.z;
            func_80A2339C(globalCtx, &sp30, this->dyna.actor.scale.x, 1.2f, 10);
        }
    }

    if (OBJICEBLOCK_GET_1(&this->dyna.actor)) {
        this->unk_2B4 = 0.1f;
    }

    func_80A25A8C(this);
    this->unkFunc = func_80A26B74;
}

void func_80A25A8C(ObjIceblock* this) {
    this->actionFunc = func_80A25AA8;
    this->unk_2A0 = 2;
}

void func_80A25AA8(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 pad[2];
    f32 sp24;
    s32 temp = func_80A236D4(this, &this->dyna.actor.home.pos);

    if (this->unk_2A0 > 0) {
        this->unk_2A0--;
        return;
    }

    if (temp) {
        sp24 = this->dyna.actor.scale.y * 300.0f;
        func_80A23D08(this, globalCtx);
        if (sp24 < (this->dyna.actor.world.pos.y - this->dyna.actor.floorHeight)) {
            func_80A25BA0(this);
        } else {
            func_80A25C5C(this);
        }

        this->dyna.actor.world.pos.y -= sp24;
        this->dyna.actor.shape.yOffset = 300.0f;
        this->unk_248.y += sp24;

        if (OBJICEBLOCK_GET_1(&this->dyna.actor)) {
            this->collider.dim.yShift = -69;
        } else {
            this->collider.dim.yShift = 0;
        }
        this->unk_1B0 |= 0x100;
    }
}

void func_80A25BA0(ObjIceblock* this) {
    this->actionFunc = func_80A25BBC;
    this->dyna.actor.velocity.y = 0.0f;
}

void func_80A25BBC(ObjIceblock* this, GlobalContext* globalCtx) {
    if (func_80A25238(this) != -1) {
        func_80A2541C(this, globalCtx);
    }

    func_80A23690(this);

    if (func_80A23F90(this, globalCtx)) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
    }

    if (func_80A24954(this, globalCtx)) {
        func_80A2491C(this);
        if (this->unk_2B0 == 3) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_DIVE_INTO_WATER_L);
        }
    }
}

void func_80A25C5C(ObjIceblock* this) {
    this->actionFunc = func_80A25C70;
}

void func_80A25C70(ObjIceblock* this, GlobalContext* globalCtx) {
    if (func_80A25238(this) != -1) {
        func_80A2541C(this, globalCtx);
    }

    if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.floorHeight - 0.1f, 14.0f)) {
        func_80A23F90(this, globalCtx);
        func_80A24954(this, globalCtx);
    }
}

void func_80A25CF4(ObjIceblock* this) {
    func_80A25404(this);
    this->actionFunc = func_80A25D28;
}

void func_80A25D28(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 sp34;
    s32 sp30;
    Vec3f sp24;

    func_80A23690(this);
    func_80A23F90(this, globalCtx);
    sp30 = func_80A25238(this);
    func_80A252DC(this, sp30);
    sp34 = true;
    if (sp30 == -1) {
        sp34 = false;
    } else if (!(this->unk_1B0 & 2) && (this->unk_26E[sp30] >= 11)) {
        sp34 = true;
        if (!func_80A24118(this, globalCtx, (this->dyna.pushForce > 0.0f) ? 59.9f : 89.9f, &sp24)) {
            func_80A232C4(this, sp30);
            func_80A25E3C(this);
            sp34 = false;
        }
    }

    if (sp34) {
        func_80A2541C(this, globalCtx);
    }

    func_80A24A48(this, globalCtx);
}

void func_80A25E3C(ObjIceblock* this) {
    this->actionFunc = func_80A25E50;
}

void func_80A25E50(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 pad;
    f32 temp_f0 = this->unk_268 - *this->unk_264;
    f32 temp_f2 = (Math_SinS(fabsf(temp_f0) * 546.13336f) * 2.8f) + 1.2f;
    s32 sp38 = Math_StepToF(this->unk_264, this->unk_268, CLAMP_MAX(temp_f2, 3.5f));
    s32 pad2;
    Vec3f sp28;

    func_80A23690(this);
    if (!func_80A23F90(this, globalCtx)) {
        func_80A24680(this, globalCtx);
        func_80A23B88(this);
        func_80A2541C(this, globalCtx);
        func_80A25BA0(this);
    } else if (sp38) {
        if (func_80A24118(this, globalCtx, 59.9f, &sp28)) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        }
        func_80A2541C(this, globalCtx);
        func_80A25CF4(this);
    } else {
        func_800B9010(&this->dyna.actor, 0xDF);
    }
}

void func_80A25FA0(ObjIceblock* this) {
    func_80A25404(this);
    this->actionFunc = func_80A25FD4;
}

void func_80A25FD4(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 pad;
    s32 sp30;
    s32 sp2C;
    Vec3f sp20;

    func_80A23690(this);
    func_80A23F90(this, globalCtx);
    sp2C = func_80A25238(this);
    func_80A252DC(this, sp2C);
    sp30 = true;

    if (sp2C == -1) {
        sp30 = false;
    } else if (!(this->unk_1B0 & 2) && (this->unk_26E[sp2C] >= 11) && !func_80A24118(this, globalCtx, 2.0f, &sp20) &&
               !func_801233E4(globalCtx)) {
        func_80A23370(this, sp2C);
        func_80A260E8(this);
        sp30 = false;
        func_800B7298(globalCtx, &this->dyna.actor, 7);
        this->unk_1B0 |= 1;
    }

    if (this) {}

    if (sp30) {
        func_80A2541C(this, globalCtx);
    }
    func_80A24A48(this, globalCtx);
}

void func_80A260E8(ObjIceblock* this) {
    static f32 D_80A26FC0[] = { 14.0, -14.0, 14.0, -14.0 };

    this->unk_260 = D_80A26FC0[this->unk_26C];
    this->unk_25C = 0.0f;
    this->dyna.actor.velocity.y = 0.0f;
    this->unk_2A0 = 15;
    func_80A24DC4(this);
    this->actionFunc = func_80A26144;
}

void func_80A26144(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 pad2;
    s32 sp28;
    s32 isBool;
    s32 sp24;

    if (this->unk_2A0 > 0) {
        this->unk_2A0--;
    }

    Math_StepToF(&this->unk_25C, this->unk_260, 0.75f);
    *this->unk_264 += this->unk_25C;
    sp28 = func_80A24384(this, globalCtx);
    func_80A23690(this);
    sp24 = func_80A23F90(this, globalCtx);
    isBool = sp24 == 0;

    if (isBool || sp28 || (this->unk_2A0 == 1)) {
        func_80A2541C(this, globalCtx);
    }

    if ((this->unk_1B0 & 1) && (isBool || sp28 || (this->dyna.actor.xzDistToPlayer > 400.0f))) {
        this->unk_1B0 &= ~1;
        func_800B7298(globalCtx, &this->dyna.actor, 6);
    }

    if (isBool) {
        func_80A24680(this, globalCtx);
        func_80A23B88(this);
        func_80A25BA0(this);
    } else if (sp28) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        func_80A23B88(this);
        func_80A25FA0(this);
    } else {
        func_800B9010(&this->dyna.actor, 0xDF);
    }

    func_80A24DD0(this, globalCtx);
}

void func_80A262BC(ObjIceblock* this) {
    this->actionFunc = func_80A262EC;
    this->dyna.actor.velocity.y *= 0.7f;
    this->unk_2A0 = -1;
}

void func_80A262EC(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 pad;
    f32 temp_f14;
    f32 temp_f18;
    Vec3f sp38;
    f32 temp;

    if (func_80A25238(this) != -1) {
        func_80A2541C(this, globalCtx);
    }

    if (this->unk_2A0 > 0) {
        this->unk_2A0--;
    }

    if (this->unk_2A0 == -1) {
        if ((this->dyna.actor.velocity.y < 0.0f) && (this->dyna.actor.world.pos.y < this->unk_244) &&
            ((this->unk_244 - (600.0f * this->dyna.actor.scale.y)) <= this->dyna.actor.world.pos.y)) {
            func_80A24BDC(this, globalCtx, 1.0f, 0.9f, 11);
            func_80A24B74(this, globalCtx);
            this->unk_2A0 = 3;
        } else {
            this->unk_2A0 = 0;
        }
    } else if (this->unk_2A0 == 1) {
        func_80A24BDC(this, globalCtx, 0.7f, 1.3f, 6);
        func_80A24B74(this, globalCtx);
    }

    this->dyna.actor.velocity.y += 5.0f;
    if (this->dyna.actor.velocity.y > 3.0f) {
        this->dyna.actor.velocity.y = 3.0f;
    }

    temp = this->dyna.actor.velocity.y;
    this->dyna.actor.world.pos.y += temp;
    temp_f14 = this->dyna.actor.world.pos.y + (600.0f * this->dyna.actor.scale.y);
    if (this->unk_244 < (temp_f14 - 90.0f)) {
        func_80A24B74(this, globalCtx);
        func_80A26574(this);
    } else {
        if ((this->dyna.actor.velocity.y > 0.0f) && (temp_f14 < (this->unk_244 + 3.0f)) &&
            (this->unk_244 <= temp_f14)) {
            func_80A24BDC(this, globalCtx, 0.8f, 1.0f, 11);
            func_80A24B74(this, globalCtx);
        }

        sp38.x = this->dyna.actor.world.pos.x;
        sp38.y = (this->unk_244 - (600.0f * this->dyna.actor.scale.y)) + 90.0f;
        sp38.z = this->dyna.actor.world.pos.z;

        if (func_80A246D8(this, globalCtx, &sp38)) {
            this->unk_1B0 |= 0x80;
        }
    }
}

void func_80A26574(ObjIceblock* this) {
    this->actionFunc = func_80A265C0;
    this->unk_27C.unk_10 = 0;
    this->unk_27C.unk_12 = 0;
    this->unk_27C.unk_0C = 2111;
    this->unk_27C.unk_0E = 3000;
    this->unk_27C.unk_18 = 500;
    this->unk_27C.unk_22 = 500;
    this->unk_27C.unk_08 = 7.0f;
    this->unk_2A0 = 40;
}

void func_80A265C0(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 pad;
    ObjIceBlockUnkStruct4* ptr;

    if (func_80A25238(this) != -1) {
        func_80A2541C(this, globalCtx);
    }

    if (this->unk_2A0 > 0) {
        this->unk_2A0--;
    } else {
        ptr = &this->unk_27C;
        this->unk_2A0 = Rand_S16Offset(30, 60);
        ptr->unk_0C = Rand_S16Offset(300, 300);
        ptr->unk_0E = Rand_S16Offset(900, 600);
        ptr->unk_04 = (2.0f * Rand_ZeroOne()) + 1.0f;
        ptr->unk_14 = (u32)Rand_Next() >> 0x10;
        func_80A24B74(this, globalCtx);
    }

    func_80A237A4(this);
    func_80A23938(this);

    if (this->unk_1B0 & 0x40) {
        func_80A24B74(this, globalCtx);
    }

    func_80A24A48(this, globalCtx);
}

void func_80A266C4(ObjIceblock* this) {
    this->actionFunc = func_80A266E0;
    this->unk_2A8 = 0.0f;
}

void func_80A266E0(ObjIceblock* this, GlobalContext* globalCtx) {
    f32 temp;
    s32 sp20;
    Actor* actor = &this->dyna.actor;

    Math_StepToF(&this->unk_2A8, (1.0f / 600.0f), (1.0f / 6000.0f));
    sp20 = Math_StepToF(&actor->scale.y, 0.0f, this->unk_2A8);
    temp = actor->scale.y * 10.0f * 300.0f;
    actor->shape.yOffset = temp;

    if (actor->scale.y < 0.075f) {
        actor->scale.x = Math_SinS(actor->scale.y * 218453.33f) * 0.1f;
        if (actor->scale.x < 0.0f) {
            actor->scale.x = 0.0f;
        }
        actor->scale.z = actor->scale.x;
    }

    if (OBJICEBLOCK_GET_1(&this->dyna.actor)) {
        this->unk_2B4 = actor->scale.y;
        this->collider.dim.height = (s32)(actor->scale.y * 1230.0f) + 1;
        this->collider.dim.yShift = actor->scale.y * -1000.0f;
        this->collider.dim.radius = actor->scale.x * 290.0f;
    }

    func_80A2508C(this, globalCtx);

    if (sp20) {
        Actor_MarkForDeath(&this->dyna.actor);
    }
}

void ObjIceblock_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    ObjIceblock* this = THIS;
    Actor* parent = this->dyna.actor.parent;

    if (parent != NULL) {
        if ((parent->update == NULL) || (&this->dyna.actor != parent->child)) {
            this->dyna.actor.parent = NULL;
            parent = NULL;
        }
    }

    if (DynaPolyActor_IsInRidingMovingState(&this->dyna)) {
        if (this->unk_1B0 & 0x20) {
            this->unk_1B0 &= ~0x40;
        } else {
            this->unk_1B0 |= 0x40;
            this->unk_1B0 |= 0x20;
        }
    } else if (this->unk_1B0 & 0x20) {
        this->unk_1B0 |= 0x40;
        this->unk_1B0 &= ~0x20;
    } else {
        this->unk_1B0 &= ~0x40;
    }

    if ((this->unk_2B0 == 1) || (this->unk_2B0 == 2) || (this->unk_2B0 == 3)) {
        if (this->unk_2AE > 0) {
            this->unk_2AE--;
        }
    }

    if (((this->collider.base.acFlags & AC_HIT) && (this->collider.info.acHitInfo->toucher.dmgFlags & 0x800)) ||
        (this->unk_2AE == 0)) {
        this->unk_2AE = -1;
        this->unk_2B0 = 4;
        this->unk_1B0 &= ~0x100;
        if (this->unk_1B0 & 1) {
            this->unk_1B0 &= ~1;
            func_800B7298(globalCtx, &this->dyna.actor, 6);
        }
        func_80A266C4(this);
    }

    if (this->unk_2B1 > 0) {
        this->unk_2B1--;
        if (this->unk_2B1 == 0) {
            ActorCutscene_Stop(this->dyna.actor.cutscene);
        }
    }

    this->actionFunc(this, globalCtx);
    if (this->dyna.actor.update == NULL) {
        if (parent != NULL) {
            parent->child = NULL;
            parent->shape.rot.x = this->unk_254.x + this->dyna.actor.home.rot.x;
            parent->shape.rot.y = this->unk_254.y + this->dyna.actor.home.rot.y;
            parent->shape.rot.z = this->unk_254.z + this->dyna.actor.home.rot.z;
            this->dyna.actor.parent = NULL;
        }
        return;
    }

    this->collider.base.ocFlags1 &= ~OC1_HIT;
    this->collider.base.acFlags &= ~AC_HIT;
    Collider_UpdateCylinder(&this->dyna.actor, &this->collider);

    if (this->unk_1B0 & 0x10) {
        if (OBJICEBLOCK_GET_1(&this->dyna.actor) && (this->unk_2B4 > 0.0f)) {
            this->collider.base.ocFlags1 &= ~OC1_NO_PUSH;
            this->collider.base.ocFlags1 |= (OC1_TYPE_2 | OC1_TYPE_1 | OC1_TYPE_PLAYER);
            this->collider.info.bumper.dmgFlags |=
                (0x800000 | 0x400000 | 0x10000 | 0x2000 | 0x1000 | 0x800 | 0x200 | 0x100 | 0x80 | 0x20 | 0x10 | 0x2);
            CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
        }
    } else {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    }

    if (this->unk_1B0 & 0x100) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    }

    func_80A25440(this);
    Actor_SetHeight(&this->dyna.actor, this->dyna.actor.shape.yOffset * this->dyna.actor.scale.y);
    this->unk_1B0 &= ~0x2;

    if (parent) {}
}

void func_80A26B64(ObjIceblock* this, GlobalContext* globalCtx) {
}

void func_80A26B74(ObjIceblock* this, GlobalContext* globalCtx) {
    func_800BE03C(globalCtx, object_ice_block_DL_0001A0);
    if (OBJICEBLOCK_GET_1(&this->dyna.actor) && (this->unk_2B4 > 0.0f)) {
        AnimatedMat_Draw(globalCtx, Lib_SegmentedToVirtual(&object_ice_block_Matanimheader_0009D0));
        func_800BE03C(globalCtx, object_ice_block_DL_0007F0);
    }
}

void func_80A26BF8(ObjIceblock* this, GlobalContext* globalCtx) {
    s32 pad[2];
    ObjIceBlockUnkStruct* ptr;
    s32 i;
    Vec3s sp70;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_8012C2DC(globalCtx->state.gfxCtx);
    sp70.z = 0;

    for (i = 0; i < ARRAY_COUNT(this->unk_1B4); i++) {
        ptr = &this->unk_1B4[i];
        sp70.x = ptr->unk_00;
        sp70.y = ptr->unk_02;
        Matrix_SetStateRotationAndTranslation(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                              this->dyna.actor.world.pos.z, &sp70);
        Matrix_Scale(ptr->unk_04, ptr->unk_08, ptr->unk_0C, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, object_ice_block_DL_0001A0);
    }

    if (OBJICEBLOCK_GET_1(&this->dyna.actor) && (this->unk_2B4 > 0.0f)) {
        AnimatedMat_Draw(globalCtx, Lib_SegmentedToVirtual(&object_ice_block_Matanimheader_0009D0));
        Matrix_SetStateRotationAndTranslation(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y - 20.0f,
                                              this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
        Matrix_Scale(this->unk_2B4, this->unk_2B4, this->unk_2B4, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, object_ice_block_DL_0007F0);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void ObjIceblock_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ObjIceblock* this = THIS;

    AnimatedMat_Draw(globalCtx, D_80A26E7C);
    this->unkFunc(this, globalCtx);
    func_80A24AA8(this, globalCtx);
}
