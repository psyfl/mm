/*
 * File: z_obj_syokudai.c
 * Overlay: ovl_Obj_Syokudai
 * Description: Torch
 */

#include "z_obj_syokudai.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"

#define FLAGS 0x00000410

#define THIS ((ObjSyokudai*)thisx)

void ObjSyokudai_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjSyokudai_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjSyokudai_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjSyokudai_Draw(Actor* thisx, GlobalContext* globalCtx);

extern Gfx D_060003A0[]; // sObjectSyokudaiTypeSwitchCausesFlameDL
extern Gfx D_06000870[]; // sObjectSyokudaiTypeNoSwitchDL
extern Gfx D_06000B90[]; // sObjectSyokudaiTypeFlameCausesSwitchDL

const ActorInit Obj_Syokudai_InitVars = {
    ACTOR_OBJ_SYOKUDAI,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_SYOKUDAI,
    sizeof(ObjSyokudai),
    (ActorFunc)ObjSyokudai_Init,
    (ActorFunc)ObjSyokudai_Destroy,
    (ActorFunc)ObjSyokudai_Update,
    (ActorFunc)ObjSyokudai_Draw,
};

static ColliderCylinderInit sStandColliderInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00100000, 0x00, 0x00 },
        { 0xF6CFFFFF, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 12, 45, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sFlameColliderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000820, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 15, 45, 45, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 800, ICHAIN_STOP),
};

static u8 sColTypes[] = { COLTYPE_METAL, COLTYPE_WOOD, COLTYPE_WOOD };

static Gfx* sDLists[] = { D_060003A0, D_06000B90, D_06000870 };

static s32 sNumLitTorchesInGroup;

void ObjSyokudai_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjSyokudai* this = THIS;
    s32 pad;
    s32 type = OBJ_SYOKUDAI_GET_TYPE(thisx);
    s32 switchFlag = OBJ_SYOKUDAI_GET_SWITCH_FLAG(thisx);

    Actor_ProcessInitChain(thisx, sInitChain);
    func_800B4AEC(globalCtx, thisx, 50.0f);
    ActorShape_Init(&thisx->shape, 0.0f, func_800B4B50, 1.0f);
    Collider_InitAndSetCylinder(globalCtx, &this->standCollider, thisx, &sStandColliderInit);
    this->standCollider.base.colType = sColTypes[OBJ_SYOKUDAI_GET_TYPE(thisx)];
    Collider_InitAndSetCylinder(globalCtx, &this->flameCollider, thisx, &sFlameColliderInit);
    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    Lights_PointGlowSetInfo(&this->lightInfo, thisx->world.pos.x, thisx->world.pos.y + OBJ_SYOKUDAI_GLOW_HEIGHT,
                            thisx->world.pos.z, 0xFF, 0xFF, 0xB4, -1);
    this->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &this->lightInfo);

    if (OBJ_SYOKUDAI_GET_START_LIT(thisx) ||
        ((type != OBJ_SYOKUDAI_TYPE_NO_SWITCH || switchFlag != 0x7F) && Flags_GetSwitch(globalCtx, switchFlag))) {

        s32 groupSize = OBJ_SYOKUDAI_GET_GROUP_SIZE(thisx);

        this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
        if (groupSize != 0) {
            sNumLitTorchesInGroup = groupSize;
        }
    } else {
        sNumLitTorchesInGroup = 0;
    }
    this->flameTexScroll = (u32)(Rand_ZeroOne() * OBJ_SYOKUDAI_SNUFF_DEFAULT);
    Actor_SetFocus(thisx, 60.0f);
}

void ObjSyokudai_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    ObjSyokudai* this = THIS;

    Collider_DestroyCylinder(globalCtx, &this->standCollider);
    Collider_DestroyCylinder(globalCtx, &this->flameCollider);
    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, this->lightNode);
}

void ObjSyokudai_Update(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    ObjSyokudai* this = THIS;
    s32 groupSize = OBJ_SYOKUDAI_GET_GROUP_SIZE(thisx);
    s32 switchFlag = OBJ_SYOKUDAI_GET_SWITCH_FLAG(thisx);
    s32 type = OBJ_SYOKUDAI_GET_TYPE(thisx);
    s32 pad0;
    WaterBox* waterBox;
    f32 waterSurface;
    s32 lightRadius = -1;
    u8 lightIntensity = 0;
    Player* player;
    s32 pad1;

    if (this->pendingAction != OBJ_SYOKUDAI_PENDING_ACTION_NONE) {
        if (ActorCutscene_GetCurrentIndex() != thisx->cutscene) {
            if (ActorCutscene_GetCanPlayNext(thisx->cutscene) != 0) {
                ActorCutscene_StartAndSetUnkLinkFields(thisx->cutscene, thisx);
                if (this->pendingAction >= OBJ_SYOKUDAI_PENDING_ACTION_CUTSCENE_AND_SWITCH) {
                    Flags_SetSwitch(globalCtx, switchFlag);
                }
            } else {
                ActorCutscene_SetIntentToPlay(thisx->cutscene);
            }
        } else if (func_800F22C4(thisx->cutscene, thisx) != 0) {
            this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
            this->pendingAction = OBJ_SYOKUDAI_PENDING_ACTION_NONE;
        }
    } else {
        if (WaterBox_GetSurface1_2(globalCtx, &globalCtx->colCtx, thisx->world.pos.x, thisx->world.pos.z, &waterSurface,
                                   &waterBox) &&
            ((waterSurface - thisx->world.pos.y) > OBJ_SYOKUDAI_FLAME_HEIGHT)) {

            this->snuffTimer = OBJ_SYOKUDAI_SNUFF_OUT;
            if (type == OBJ_SYOKUDAI_TYPE_FLAME_CAUSES_SWITCH) {
                Flags_UnsetSwitch(globalCtx, switchFlag);
                if (groupSize != 0) {
                    this->snuffTimer = OBJ_SYOKUDAI_SNUFF_GROUP_BY_WATER;
                }
            }
        } else {
            s32 interaction = OBJ_SYOKUDAI_INTERACTION_NONE;
            u32 flameColliderHurtboxDmgFlags = 0;
            player = GET_PLAYER(globalCtx);

            if (OBJ_SYOKUDAI_GET_START_LIT(thisx)) {
                this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
            }
            if (groupSize != 0) {
                if (Flags_GetSwitch(globalCtx, switchFlag)) {
                    if (this->snuffTimer == OBJ_SYOKUDAI_SNUFF_OUT) {
                        if (type != OBJ_SYOKUDAI_TYPE_SWITCH_CAUSES_FLAME) {
                            this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
                        } else {
                            this->pendingAction = OBJ_SYOKUDAI_PENDING_ACTION_CUTSCENE_NO_SWITCH;
                        }
                    } else if (this->snuffTimer > OBJ_SYOKUDAI_SNUFF_OUT) {
                        this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
                    }
                } else if (this->snuffTimer <= OBJ_SYOKUDAI_SNUFF_NEVER) {
                    this->snuffTimer = OBJ_SYOKUDAI_SNUFF_DEFAULT;
                }
            }
            if (this->flameCollider.base.acFlags & AC_HIT) {
                flameColliderHurtboxDmgFlags = this->flameCollider.info.acHitInfo->toucher.dmgFlags;
                if (this->flameCollider.info.acHitInfo->toucher.dmgFlags & 0x820) {
                    interaction = OBJ_SYOKUDAI_INTERACTION_ARROW_FA;
                }
            } else if (player->itemActionParam == PLAYER_AP_STICK) {
                Vec3f stickTipSeparationVec;

                Math_Vec3f_Diff(&player->swordInfo[0].tip, &thisx->world.pos, &stickTipSeparationVec);
                stickTipSeparationVec.y -= OBJ_SYOKUDAI_STICK_IGNITION_HEIGHT;
                if (SQXYZ(stickTipSeparationVec) < SQ(OBJ_SYOKUDAI_STICK_IGNITION_RADIUS)) {
                    interaction = OBJ_SYOKUDAI_INTERACTION_STICK;
                }
            }
            if (interaction != OBJ_SYOKUDAI_INTERACTION_NONE) {
                if (this->snuffTimer != OBJ_SYOKUDAI_SNUFF_OUT) {
                    if (interaction <= OBJ_SYOKUDAI_INTERACTION_STICK) {
                        if (player->unk_B28 == 0) {
                            player->unk_B28 = 0xD2;
                            Audio_PlaySfxAtPos(&thisx->projectedPos, NA_SE_EV_FLAME_IGNITION);
                        } else if (player->unk_B28 < 0xC8) {
                            player->unk_B28 = 0xC8;
                        }
                    } else if (flameColliderHurtboxDmgFlags & 0x20) {
                        Actor* flameColliderHurtboxActor = this->flameCollider.base.ac;

                        if ((flameColliderHurtboxActor->update != NULL) &&
                            (flameColliderHurtboxActor->id == ACTOR_EN_ARROW)) {

                            flameColliderHurtboxActor->params = 0;
                            ((EnArrow*)flameColliderHurtboxActor)->unk_1C0 = 0x800;
                        }
                    }
                    if ((this->snuffTimer > OBJ_SYOKUDAI_SNUFF_NEVER) &&
                        (this->snuffTimer < (OBJ_SYOKUDAI_SNUFF_TIMER_INITIAL(groupSize))) &&
                        (type != OBJ_SYOKUDAI_TYPE_SWITCH_CAUSES_FLAME)) {

                        this->snuffTimer = OBJ_SYOKUDAI_SNUFF_TIMER_INITIAL(groupSize);
                    }
                } else if ((type != OBJ_SYOKUDAI_TYPE_SWITCH_CAUSES_FLAME) &&
                           (((interaction >= OBJ_SYOKUDAI_INTERACTION_ARROW_FA) &&
                             (flameColliderHurtboxDmgFlags & 0x800)) ||
                            ((interaction <= OBJ_SYOKUDAI_INTERACTION_STICK) && (player->unk_B28 != 0)))) {
                    if ((interaction < OBJ_SYOKUDAI_INTERACTION_NONE) && (player->unk_B28 < 0xC8)) {
                        player->unk_B28 = 0xC8;
                    }
                    if (groupSize == 0) {
                        if ((type == OBJ_SYOKUDAI_TYPE_NO_SWITCH) && (switchFlag == 0x7F)) {
                            this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
                        } else if (thisx->cutscene >= 0) {
                            this->pendingAction = OBJ_SYOKUDAI_PENDING_ACTION_CUTSCENE_AND_SWITCH;
                        } else {
                            Flags_SetSwitch(globalCtx, switchFlag);
                            this->snuffTimer = OBJ_SYOKUDAI_SNUFF_NEVER;
                        }
                    } else {
                        if (++sNumLitTorchesInGroup >= groupSize) {
                            this->pendingAction = OBJ_SYOKUDAI_PENDING_ACTION_CUTSCENE_AND_SWITCH;
                        } else {
                            this->snuffTimer =
                                OBJ_SYOKUDAI_SNUFF_TIMER_INITIAL(groupSize) + OBJ_SYOKUDAI_SNUFF_TIMER_JUST_LIT_BONUS;
                        }
                    }
                    Audio_PlaySfxGeneral(NA_SE_EV_FLAME_IGNITION, &thisx->projectedPos, 4, &D_801DB4B0, &D_801DB4B0,
                                         &D_801DB4B8);
                }
            }
        }
    }
    Collider_UpdateCylinder(thisx, &this->standCollider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->standCollider.base);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->standCollider.base);
    Collider_UpdateCylinder(thisx, &this->flameCollider);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->flameCollider.base);
    if ((this->snuffTimer > OBJ_SYOKUDAI_SNUFF_OUT) && (--this->snuffTimer == OBJ_SYOKUDAI_SNUFF_OUT) &&
        (type != OBJ_SYOKUDAI_TYPE_SWITCH_CAUSES_FLAME)) {
        sNumLitTorchesInGroup--;
    }
    if (this->snuffTimer != OBJ_SYOKUDAI_SNUFF_OUT) {
        s32 pad2;

        if ((this->snuffTimer <= OBJ_SYOKUDAI_SNUFF_NEVER) || (this->snuffTimer >= OBJ_SYOKUDAI_SNUFF_DEFAULT)) {
            lightRadius = OBJ_SYOKUDAI_LIGHT_RADIUS_MAX;
        } else {
            lightRadius = (f32)this->snuffTimer * OBJ_SYOKUDAI_LIGHT_RADIUS_MAX / OBJ_SYOKUDAI_SNUFF_DEFAULT;
        }
        lightIntensity = Rand_ZeroOne() * 127;
        lightIntensity += 128;
        func_800B9010(thisx, NA_SE_EV_TORCH - SFX_FLAG);
    }
    Lights_PointSetColorAndRadius(&this->lightInfo, lightIntensity, lightIntensity * 0.7f, 0, lightRadius);
    this->flameTexScroll++;
}

void ObjSyokudai_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ObjSyokudai* this = THIS;
    s32 pad;
    s32 groupSize = OBJ_SYOKUDAI_GET_GROUP_SIZE(thisx);
    f32 flameScale;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    func_8012C28C(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, sDLists[OBJ_SYOKUDAI_GET_TYPE(thisx)]);
    if (this->snuffTimer != OBJ_SYOKUDAI_SNUFF_OUT) {
        s32 snuffTimerInitial = OBJ_SYOKUDAI_SNUFF_TIMER_INITIAL(groupSize);

        flameScale = 1.0f;
        if (snuffTimerInitial < this->snuffTimer) {
            flameScale = (f32)(snuffTimerInitial - this->snuffTimer + OBJ_SYOKUDAI_SNUFF_TIMER_JUST_LIT_BONUS) /
                         OBJ_SYOKUDAI_SNUFF_TIMER_JUST_LIT_BONUS;
        } else if ((this->snuffTimer > OBJ_SYOKUDAI_SNUFF_OUT) && (this->snuffTimer < OBJ_SYOKUDAI_SNUFF_DEFAULT)) {
            flameScale = (f32)this->snuffTimer / OBJ_SYOKUDAI_SNUFF_DEFAULT;
        }
        flameScale *= 0.0027f;
        func_8012C2DC(globalCtx->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                    (this->flameTexScroll * -OBJ_SYOKUDAI_SNUFF_DEFAULT) & 0x1FF, 0x20, 0x80));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
        Matrix_InsertTranslation(0.0f, OBJ_SYOKUDAI_FLAME_HEIGHT, 0.0f, MTXMODE_APPLY);
        Matrix_RotateY(BINANG_ROT180(func_800DFCDC(GET_ACTIVE_CAM(globalCtx)) - thisx->shape.rot.y), MTXMODE_APPLY);
        Matrix_Scale(flameScale, flameScale, flameScale, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gGameplayKeepDrawFlameDL);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
