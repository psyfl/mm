/*
 * File: z_en_stream.c
 * Overlay: ovl_En_Stream
 * Description: Unused water vortex from OoT
 */

#include "z_en_stream.h"
#include "objects/object_stream/object_stream.h"

#define FLAGS 0x00000010

#define THIS ((EnStream*)thisx)

void EnStream_Init(Actor* thisx, GlobalContext* globalCtx);
void EnStream_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnStream_Update(Actor* thisx, GlobalContext* globalCtx);
void EnStream_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnStream_WaitForPlayer(EnStream* this, GlobalContext* globalCtx);

const ActorInit En_Stream_InitVars = {
    ACTOR_EN_STREAM,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_STREAM,
    sizeof(EnStream),
    (ActorFunc)EnStream_Init,
    (ActorFunc)EnStream_Destroy,
    (ActorFunc)EnStream_Update,
    (ActorFunc)EnStream_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 20, ICHAIN_STOP),
};

void EnStream_SetupAction(EnStream* this, EnStreamActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnStream_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnStream* this = THIS;

    this->size = EN_STREAM_SIZE(&this->actor);
    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (this->size != EN_STREAM_SIZE_NORMAL && this->size == EN_STREAM_SIZE_SMALL) {
        this->actor.scale.y = 0.01f;
    }
    EnStream_SetupAction(this, EnStream_WaitForPlayer);
}

void EnStream_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

s32 EnStream_PlayerIsInRange(Vec3f* vortexWorldPos, Vec3f* playerWorldPos, Vec3f* posDifference, f32 vortexYScale) {
    s32 ret = EN_STREAM_PLAYER_OUTSIDE_RANGE;
    f32 smallConstant = 28.0f;
    f32 upperBounds = 160 * vortexYScale * 50.0f;
    f32 lowerBounds = 0 * vortexYScale * 50.0f;
    f32 xzDist;
    f32 range;

    posDifference->x = playerWorldPos->x - vortexWorldPos->x;
    posDifference->y = playerWorldPos->y - vortexWorldPos->y;
    posDifference->z = playerWorldPos->z - vortexWorldPos->z;
    xzDist = sqrtf(SQ(posDifference->x) + SQ(posDifference->z));

    if (lowerBounds <= posDifference->y && posDifference->y <= upperBounds) {
        posDifference->y -= lowerBounds;

        range = ((75.0f - smallConstant) * (posDifference->y / (upperBounds - lowerBounds))) + 28.0f;
        if (xzDist <= range) {
            ret = EN_STREAM_PLAYER_WITHIN_RANGE_INSIDE_VORTEX;
        }
    }

    if (posDifference->y <= lowerBounds && xzDist <= 28.0f) {
        ret = EN_STREAM_PLAYER_WITHIN_RANGE_BELOW_VORTEX;
    }

    return ret;
}

void EnStream_SuckPlayer(EnStream* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad48;
    Vec3f posDifference;
    f32 xzDist;
    f32 yDistWithOffset;
    s32 pad30[2];

    if (EnStream_PlayerIsInRange(&this->actor.world.pos, &player->actor.world.pos, &posDifference,
                                 this->actor.scale.y) != EN_STREAM_PLAYER_OUTSIDE_RANGE) {
        xzDist = sqrtf(SQ(posDifference.x) + SQ(posDifference.z));
        yDistWithOffset = player->actor.world.pos.y - (this->actor.world.pos.y - 90.0f);
        player->unk_B84 = Math_Atan2S(-posDifference.x, -posDifference.z);
        if (xzDist > 3.0f) {
            Math_SmoothStepToF(&player->unk_B80, 3.0f, 0.5f, xzDist, 0.0f);
        } else {
            player->unk_B80 = 0.0f;
            Math_SmoothStepToF(&player->actor.world.pos.x, this->actor.world.pos.x, 0.5f, 3.0f, 0.0f);
            Math_SmoothStepToF(&player->actor.world.pos.z, this->actor.world.pos.z, 0.5f, 3.0f, 0.0f);
        }
        if (yDistWithOffset > 0.0f) {
            Math_SmoothStepToF(&player->actor.velocity.y, -3.0f, 0.7f, yDistWithOffset, 0.0f);
            if (posDifference.y < -70.0f) {
                player->stateFlags2 |= 0x80000000;
            }
        }
    } else {
        EnStream_SetupAction(this, EnStream_WaitForPlayer);
    }
}

void EnStream_WaitForPlayer(EnStream* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 pad;
    Vec3f temp;

    if (EnStream_PlayerIsInRange(&this->actor.world.pos, &player->actor.world.pos, &temp, this->actor.scale.y) !=
        EN_STREAM_PLAYER_OUTSIDE_RANGE) {
        EnStream_SetupAction(this, EnStream_SuckPlayer);
    }
}

void EnStream_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnStream* this = THIS;

    this->actionFunc(this, globalCtx);
    func_800B8FE8(&this->actor, NA_SE_EV_WHIRLPOOL - SFX_FLAG);
}

void EnStream_Draw(Actor* thisx, GlobalContext* globalCtx) {
    u32 multipliedFrames;
    u32 frames = globalCtx->gameplayFrames;
    Gfx* gfx;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    func_8012C2DC(globalCtx->state.gfxCtx);
    gfx = POLY_XLU_DISP;
    gSPMatrix(&gfx[0], Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    multipliedFrames = frames * 20;
    gSPSegment(&gfx[1], 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, frames * 30, -multipliedFrames, 64, 64, 1, multipliedFrames,
                                -multipliedFrames, 64, 64));
    gSPDisplayList(&gfx[2], gWaterVortexDL);
    POLY_XLU_DISP = &gfx[3];
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
