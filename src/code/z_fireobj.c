#include "global.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "objects/gameplay_keep/gameplay_keep.h"

typedef enum {
    FIRE_STATE_0,
    FIRE_STATE_1,
    FIRE_STATE_2,
    FIRE_STATE_3,
} FireState;

ColliderCylinderInit sFireObjCollisionInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK4,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000820, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 18, 67, 0, { 0, 0, 0 } },
};

FireObjLightParams sFireObjLightParams[2] = {
    { 255, 255, 0, 255, 128, 255, 0, 0 },
    { 0, 200, 128, 128, 0, 127, 127, 0 },
};

void FireObj_InitLight(GlobalContext* globalCtx, FireObjLight* light, u8* param_3, Vec3f* pos);
void FireObj_UpdateLight(GlobalContext* globalCtx, FireObjLight* light, FireObj* fire);

void FireObj_InitWithParams(GlobalContext* globalCtx, FireObj* fire, FireObjInitParams* init) {
    fire->size = init->size;
    fire->sizeInv = 1.0f / init->size;
    fire->dynamicSizeStep = init->dynamicSizeStep;
    fire->state = init->state;
    fire->sizeGrowsCos2 = init->sizeGrowsCos2;
    fire->unk27 = init->unkA;
    fire->flags = init->flags;
    fire->xScale = 0.0f;
    fire->yScale = 0.0f;
    fire->dynamicSize = 0.0f;
    fire->unk26 = Rand_ZeroOne() * 20.0f;
    fire->ignitionDelay = -1;
}

void FireObj_SetState(FireObj* fire, f32 dynamicSizeStep, u8 newState) {
    fire->state = newState & 0xFF;
    if (fire->state == FIRE_STATE_3) {
        fire->yScale = 0.0f;
        fire->xScale = 0.0f;
        fire->dynamicSize = 0.0f;
    } else if (fire->state == FIRE_STATE_2) {
        fire->xScale = fire->yScale = fire->size;
        fire->dynamicSize = 1.0f;
    }
    fire->dynamicSizeStep = dynamicSizeStep;
}

void FireObj_SetPosition(FireObj* fire, Vec3f* pos) {
    Math_Vec3f_Copy(&fire->position, pos);
}

void FireObj_StepSize(FireObj* fire) {
    if (fire->state == FIRE_STATE_0) {
        if (Math_StepToF(&fire->dynamicSize, 1.0f, fire->dynamicSizeStep) != 0) {
            FireObj_SetState(fire, fire->dynamicSizeStep, FIRE_STATE_2);
        }
    } else if ((fire->state == FIRE_STATE_1) && (Math_StepToF(&fire->dynamicSize, 0.0f, fire->dynamicSizeStep) != 0)) {
        FireObj_SetState(fire, fire->dynamicSizeStep, 3);
    }
    if (fire->sizeGrowsCos2 == 1) {
        if ((fire->state == FIRE_STATE_0) || (fire->state == FIRE_STATE_1)) {
            fire->xScale = (1.0f - Math_CosS((fire->dynamicSize * fire->dynamicSize * 16384.0f))) * fire->size;
            fire->yScale = fire->dynamicSize * fire->size;
        } else {
            fire->yScale = fire->dynamicSize * fire->size;
            fire->xScale = fire->dynamicSize * fire->size;
        }
        return;
    } else {
        fire->yScale = fire->dynamicSize * fire->size;
    }
    fire->xScale = fire->dynamicSize * fire->size;
}

void FireObj_UpdateStateTransitions(GlobalContext* globalCtx, FireObj* fire) {
    Player* player = GET_PLAYER(globalCtx);
    WaterBox* waterBox;
    f32 waterY;
    s32 sp40 = 0;
    u8 nextState;
    Vec3f dist;

    FireObj_StepSize(fire);
    fire->unk26++;
    if (fire->ignitionDelay > 0) {
        fire->ignitionDelay--;
    } else if (fire->ignitionDelay == 0) {
        fire->ignitionDelay = -1;
        if ((fire->state == FIRE_STATE_3) || (fire->state == FIRE_STATE_1)) {
            nextState = FIRE_STATE_0;
        } else {
            nextState = FIRE_STATE_1;
        }
        FireObj_SetState(fire, fire->dynamicSizeStep, nextState);
    }
    if ((fire->flags & 1) && (fire->state != FIRE_STATE_3) &&
        WaterBox_GetSurface1_2(globalCtx, &globalCtx->colCtx, fire->position.x, fire->position.z, &waterY, &waterBox) &&
        ((fire->yScale * ((void)0, 6500.0f)) < (waterY - fire->position.y))) { // Fake but IDK what else
        FireObj_SetState(fire, fire->dynamicSizeStep, FIRE_STATE_3);
    }
    if ((fire->flags & 2) && (player->itemActionParam == PLAYER_AP_STICK)) {
        Math_Vec3f_Diff(&player->swordInfo[0].tip, &fire->position, &dist);
        if (Math3D_LengthSquared(&dist) < 400.0f) {
            sp40 = 1;
        }
    }
    if (sp40 != 0) {
        if (fire->state == FIRE_STATE_3) {
            if (player->unk_B28 > 0) {
                FireObj_SetState(fire, fire->dynamicSizeStep, FIRE_STATE_0);
            }
        } else if (player->unk_B28 == 0) {
            player->unk_B28 = 0xD2;
            SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &fire->position, 20, NA_SE_EV_FLAME_IGNITION);
        } else if (player->unk_B28 < 0xC8) {
            player->unk_B28 = 0xC8;
        }
    }
}

void FireObj_Draw(GlobalContext* globalCtx, FireObj* fire) {
    s32 pad;
    FireObjLightParams* lightParams = &sFireObjLightParams[fire->unk27];

    if (fire->state != FIRE_STATE_3) {
        Vec3s vec;

        OPEN_DISPS(globalCtx->state.gfxCtx);
        func_8012C2DC(globalCtx->state.gfxCtx);
        gSPSegment(
            POLY_XLU_DISP++, 0x08,
            Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0, (fire->unk26 * -20) & 511, 32, 128));

        gDPSetPrimColor(POLY_XLU_DISP++, 0, lightParams->unk4, lightParams->primColor.r, lightParams->primColor.g,
                        lightParams->primColor.b, lightParams->primColor.a);

        gDPSetEnvColor(POLY_XLU_DISP++, lightParams->envColor.r, lightParams->envColor.g, lightParams->envColor.b, 0);

        vec.x = 0;
        vec.y = func_800DFCDC(GET_ACTIVE_CAM(globalCtx)) + 0x8000;
        vec.z = 0;
        Matrix_SetStateRotationAndTranslation(fire->position.x, fire->position.y, fire->position.z, &vec);
        Matrix_Scale(fire->xScale, fire->yScale, 1.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gGameplayKeepDrawFlameDL);
        CLOSE_DISPS(globalCtx->state.gfxCtx);
    }
}

#ifdef NON_EQUIVALENT
// Accesses data incorrectly
void FireObj_InitLight(GlobalContext* globalCtx, FireObjLight* light, u8* param_3, Vec3f* pos) {
    FireObjLightParams* objectParams = &sFireObjLightParams[*param_3];

    Lights_PointGlowSetInfo(&light->lightInfo, pos->x, pos->y, pos->z, objectParams->primColor.g,
                            objectParams->primColor.b, objectParams->primColor.a, objectParams->primColor.r);
    light->light = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, (LightInfo*)&light->lightInfo);
    light->unk12 = *param_3;
}
#else
#pragma GLOBAL_ASM("asm/non_matchings/code/z_fireobj/FireObj_InitLight.s")
#endif

void FireObj_DestroyLight(GlobalContext* globalCtx, FireObjLight* light) {
    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, light->light);
}

#ifdef NON_EQUIVALENT
// Accesses data incorrectly
void FireObj_UpdateLight(GlobalContext* globalCtx, FireObjLight* light, FireObj* fire) {
    FireObjLightParams* lightParams = &sFireObjLightParams[light->unk12];
    s16 radius;

    if (fire->state == FIRE_STATE_3) {
        Lights_PointSetColorAndRadius((LightInfo*)&light->lightInfo, 0, 0, 0, -1);
    } else {
        radius = ((fire->yScale * 140.0f * fire->sizeInv) + 60.0f);

        Lights_PointGlowSetInfo((LightInfo*)&light->lightInfo, fire->position.x,
                                (fire->position.y + (fire->yScale * 6500.0f)), fire->position.z,
                                ((s32)(Rand_ZeroOne() * lightParams->envColor.r) + lightParams->primColor.b),
                                ((s32)(Rand_ZeroOne() * lightParams->envColor.g) + lightParams->primColor.a),
                                ((s32)(Rand_ZeroOne() * lightParams->envColor.b) + lightParams->unk4), radius);
    }
}
#else
#pragma GLOBAL_ASM("asm/non_matchings/code/z_fireobj/FireObj_UpdateLight.s")
#endif

void FireObj_Init(GlobalContext* globalCtx, FireObj* fire, FireObjInitParams* init, Actor* actor) {
    FireObj* fire2 = fire;

    FireObj_InitWithParams(globalCtx, fire, init);
    Collider_InitCylinder(globalCtx, &fire->collision);
    Collider_SetCylinder(globalCtx, &fire->collision, actor, &sFireObjCollisionInit);

    fire2->collision.dim.radius = (fire->size * 4000.0f) + 2.5f;
    fire2->collision.dim.height = fire->size * 16000.0f;
    fire->collision.dim.yShift = fire->size * -1728.0f;
    FireObj_InitLight(globalCtx, &fire->light, &init->unkC, &fire->position);
}

void FireObj_Destroy(GlobalContext* globalCtx, FireObj* fire) {
    Collider_DestroyCylinder(globalCtx, &fire->collision);
    FireObj_DestroyLight(globalCtx, &fire->light);
}

void FireObj_SetState2(FireObj* fire, f32 dynamicSizeStep, u8 newState) {
    FireObj_SetState(fire, dynamicSizeStep, newState);
}

void FireObj_Update(GlobalContext* globalCtx, FireObj* fire, Actor* actor) {
    s32 pad; // Gamestate cast?
    EnArrow* arrow = (EnArrow*)fire->collision.base.ac;

    FireObj_UpdateStateTransitions(globalCtx, fire);
    if (fire->state == 3) {
        if ((fire->collision.base.acFlags & 2) && (fire->collision.info.acHitInfo->toucher.dmgFlags & 0x800)) {
            FireObj_SetState(fire, fire->dynamicSizeStep, 0);
        }
    } else if ((fire->collision.base.acFlags & 2) && (arrow->actor.update != NULL) &&
               (arrow->actor.id == ACTOR_EN_ARROW)) {
        arrow->actor.params = 0;
        arrow->unk_1C0 = 0x800;
    }
    fire->collision.dim.pos.x = fire->position.x;
    fire->collision.dim.pos.y = fire->position.y;
    fire->collision.dim.pos.z = fire->position.z;
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &fire->collision.base);
    FireObj_UpdateLight(globalCtx, &fire->light, fire);
}
