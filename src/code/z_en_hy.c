/*
 * File: z_en_hy.c
 * Description: Unused System for NPCs (includes animation, door interaction, blinking, pathing, and collider helpers)
 */

#include "global.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"

extern AnimationHeader D_0600007C;
extern AnimationHeader D_0600066C;
extern AnimationHeader D_0600071C;
extern AnimationHeader D_060008C0;
extern AnimationHeader D_06000AB0;
extern AnimationHeader D_06000FDC;
extern AnimationHeader D_06001494;
extern AnimationHeader D_06001908;
extern AnimationHeader D_06001EE0;
extern AnimationHeader D_06005DC4;
extern AnimationHeader D_06005D9C;
extern AnimationHeader D_0600DED8;
extern AnimationHeader D_0600F920;
extern AnimationHeader D_0600FC1C;
extern AnimationHeader D_0600FEE4;
extern AnimationHeader D_06010330;

static ActorAnimationEntryS animations[] = {
    { &D_0600007C, 1.0f, 0, -1, 0, 0 },  { &D_06001494, 1.0f, 0, -1, 0, 0 },  { &D_06001494, 1.0f, 0, -1, 0, -8 },
    { &D_06001908, 1.0f, 0, -1, 0, 0 },  { &D_06001908, 1.0f, 0, -1, 0, -8 }, { &D_060008C0, 1.0f, 0, -1, 0, 0 },
    { &D_06005DC4, 1.0f, 0, -1, 0, 0 },  { &D_06000FDC, 1.0f, 0, -1, 0, 0 },  { &D_06000AB0, 1.0f, 0, -1, 0, -8 },
    { &D_0600066C, 1.0f, 0, -1, 0, 0 },  { &D_0600071C, 1.0f, 0, -1, 0, 0 },  { &D_06001EE0, 1.0f, 0, -1, 0, 0 },
    { &D_0600DED8, 1.5f, 0, -1, 2, 0 },  { &D_0600F920, 1.5f, 0, -1, 2, 0 },  { &D_0600FC1C, 1.0f, 0, -1, 0, 0 },
    { &D_0600FEE4, 1.0f, 0, -1, 0, 0 },  { &D_06010330, 1.0f, 0, -1, 0, 0 },  { &D_0600FC1C, 1.0f, 0, -1, 0, -8 },
    { &D_0600FEE4, 1.0f, 0, -1, 0, -8 }, { &D_06010330, 1.0f, 0, -1, 0, -8 }, { &D_06005D9C, 1.0f, 0, -1, 0, -8 },
};

s8 D_801BC3F0[] = { -1, 1, 12, 13, 14, 9, 10, 11, 0, 6, 7, 8, 3, 4, 5, 2 };

s8 D_801BC400[] = { 0, 0, 0, 0, 3, 4, 0, 6, 7, 0, 9, 10, 0, 12, 13, 0 };

u8 D_801BC410[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

s32 EnHy_ChangeAnim(SkelAnime* skelAnime, s16 animIndex) {
    s16 frameCount;
    s32 isChanged = false;

    if (animIndex >= 0 && animIndex <= 20) {
        isChanged = true;
        frameCount = animations[animIndex].frameCount;
        if (frameCount < 0) {
            frameCount = Animation_GetLastFrame(&animations[animIndex].animationSeg->common);
        }
        Animation_Change(skelAnime, animations[animIndex].animationSeg, animations[animIndex].playbackSpeed,
                         animations[animIndex].frame, frameCount, animations[animIndex].mode,
                         animations[animIndex].transitionRate);
    }
    return isChanged;
}

//! @TODO: Return Door instance when c and h files are split
Actor* EnHy_FindNearestDoor(Actor* actor, GlobalContext* globalCtx) {
    Actor* nearestDoor = NULL;
    Actor* doorIter = NULL;
    Actor* door;
    f32 dist;
    s32 isSetup = false;
    f32 minDist = 0.0f;

    do {
        doorIter = SubS_FindActor(globalCtx, doorIter, ACTORCAT_DOOR, ACTOR_EN_DOOR);
        door = doorIter;
        dist = Actor_DistanceBetweenActors(actor, door);
        if (!isSetup || (dist < minDist)) {
            nearestDoor = door;
            minDist = dist;
            isSetup = true;
        }
        doorIter = door->next;
    } while (doorIter != NULL);

    if (1) {}

    return nearestDoor;
}

void EnHy_ChangeObjectAndAnim(EnHy* enHy, GlobalContext* globalCtx, s16 animIndex) {
    gSegments[6] = PHYSICAL_TO_VIRTUAL(globalCtx->objectCtx.status[enHy->animObjIndex].segment);
    EnHy_ChangeAnim(&enHy->skelAnime, animIndex);
}

s32 EnHy_UpdateSkelAnime(EnHy* enHy, GlobalContext* globalCtx) {
    s32 isUpdated = false;

    if (enHy->actor.draw != NULL) {
        gSegments[6] = PHYSICAL_TO_VIRTUAL(globalCtx->objectCtx.status[enHy->animObjIndex].segment);
        SkelAnime_Update(&enHy->skelAnime);
        isUpdated = true;
    }
    return isUpdated;
}

void EnHy_Blink(EnHy* enHy, s32 eyeTexMaxIndex) {
    if (DECR(enHy->blinkTimer) == 0) {
        enHy->eyeTexIndex++;
        if (enHy->eyeTexIndex >= eyeTexMaxIndex) {
            enHy->eyeTexIndex = 0;
            enHy->blinkTimer = Rand_S16Offset(30, 30);
        }
    }
}

s32 EnHy_Init(EnHy* enHy, GlobalContext* globalCtx, FlexSkeletonHeader* skeletonHeaderSeg, s16 animIndex) {
    s32 isInitialized = false;

    if ((func_8013D8DC(enHy->animObjIndex, globalCtx) == 1) && (func_8013D8DC(enHy->unk190, globalCtx) == 1) &&
        (func_8013D8DC(enHy->unk191, globalCtx) == 1) && (func_8013D8DC(enHy->unk192, globalCtx) == 1)) {
        enHy->actor.objBankIndex = enHy->unk192;
        isInitialized = true;
        ActorShape_Init(&enHy->actor.shape, 0.0f, NULL, 0.0f);
        gSegments[6] = PHYSICAL_TO_VIRTUAL(globalCtx->objectCtx.status[enHy->actor.objBankIndex].segment);
        SkelAnime_InitFlex(globalCtx, &enHy->skelAnime, skeletonHeaderSeg, NULL, enHy->jointTable, enHy->morphTable,
                           16);
        EnHy_ChangeObjectAndAnim(enHy, globalCtx, animIndex);
    }
    return isInitialized;
}

//! @TODO: Should just take EnDoor instead of actor when c and h are split
void func_800F0BB4(EnHy* enHy, GlobalContext* globalCtx, Actor* door, s16 arg3, s16 arg4) {
    s32 pad;
    s8 sp3B;
    Vec3f offset;
    f32 phi_f0;

    Actor_OffsetOfPointInActorCoords(door, &offset, &enHy->actor.world.pos);
    phi_f0 = (offset.z >= 0.0f) ? 1.0f : -1.0f;
    sp3B = ((s8)phi_f0 < 0) ? 0 : 2;
    EnHy_ChangeObjectAndAnim(enHy, globalCtx, (sp3B == 0) ? arg3 : arg4);
    enHy->skelAnime.baseTransl = *enHy->skelAnime.jointTable;
    enHy->skelAnime.prevTransl = *enHy->skelAnime.jointTable;
    enHy->skelAnime.moveFlags |= 3;
    AnimationContext_SetMoveActor(globalCtx, &enHy->actor, &enHy->skelAnime, 1.0f);
    ((EnDoor*)door)->unk_1A1 = 1;
    ((EnDoor*)door)->unk_1A0 = sp3B;
}

s32 func_800F0CE4(EnHy* enHy, GlobalContext* globalCtx, ActorFunc draw, s16 arg3, s16 arg4, f32 arg5) {
    s32 ret = false;
    s16 yaw;
    Actor* door;
    s32 pad;

    if (func_8013D68C(enHy->path, enHy->curPoint, &enHy->actor.world.pos)) {
        door = EnHy_FindNearestDoor(&enHy->actor, globalCtx);
        if (door != NULL) {
            ret = true;
            func_800F0BB4(enHy, globalCtx, door, arg3, arg4);
            yaw = Math_Vec3f_Yaw(&enHy->actor.world.pos, &door->world.pos);
            enHy->actor.world.pos.x += arg5 * Math_SinS(yaw);
            enHy->actor.world.pos.z += arg5 * Math_CosS(yaw);
            enHy->actor.world.rot.y = -yaw;
            enHy->actor.shape.rot.y = -yaw;
            enHy->actor.draw = draw;
        }
    }
    return ret;
}

s32 func_800F0DD4(EnHy* enHy, GlobalContext* globalCtx, s16 arg2, s16 arg3) {
    s32 ret = false;
    s32 pad;
    Actor* door;

    enHy->curPoint = 0;
    if (func_8013D68C(enHy->path, enHy->curPoint, &enHy->actor.world.pos)) {
        door = EnHy_FindNearestDoor(&enHy->actor, globalCtx);
        if (door != NULL) {
            ret = true;
            func_800F0BB4(enHy, globalCtx, door, arg2, arg3);
            enHy->actor.shape.rot.y = Math_Vec3f_Yaw(&enHy->actor.world.pos, &door->world.pos);
            enHy->actor.world.rot.y = enHy->actor.shape.rot.y;
            enHy->actor.gravity = 0.0f;
            enHy->actor.flags &= ~1;
        }
    }
    return ret;
}

s32 EnHy_SetPointFowards(EnHy* enHy, GlobalContext* globalCtx, f32 gravity, s16 animIndex) {
    enHy->actor.gravity = gravity;
    enHy->actor.flags |= 1;
    EnHy_ChangeObjectAndAnim(enHy, globalCtx, animIndex);
    enHy->curPoint++;
    return 0;
}

s32 EnHy_SetPointBackwards(EnHy* enHy, GlobalContext* globalCtx, s16 animIndex) {
    EnHy_ChangeObjectAndAnim(enHy, globalCtx, animIndex);
    enHy->curPoint--;
    return 0;
}

s32 EnHy_MoveForwards(EnHy* enHy, f32 speedTarget) {
    s16 rotStep;
    s32 reachedEnd = false;
    Vec3f curPointPos;

    Math_SmoothStepToF(&enHy->actor.speedXZ, speedTarget, 0.4f, 1000.0f, 0.0f);
    rotStep = enHy->actor.speedXZ * 400.0f;
    if (func_8013D68C(enHy->path, enHy->curPoint, &curPointPos) && func_8013D768(&enHy->actor, &curPointPos, rotStep)) {
        enHy->curPoint++;
        if (enHy->curPoint >= enHy->path->count) {
            reachedEnd = true;
        }
    }
    return reachedEnd;
}

s32 EnHy_MoveBackwards(EnHy* enHy, f32 speedTarget) {
    s16 rotStep;
    s32 reachedEnd = false;
    Vec3f curPointPos;

    Math_SmoothStepToF(&enHy->actor.speedXZ, speedTarget, 0.4f, 1000.0f, 0.0f);
    rotStep = enHy->actor.speedXZ * 400.0f;
    if (func_8013D68C(enHy->path, enHy->curPoint, &curPointPos) && func_8013D768(&enHy->actor, &curPointPos, rotStep)) {
        enHy->curPoint--;
        if (enHy->curPoint < 0) {
            reachedEnd = true;
        }
    }
    return reachedEnd;
}

void EnHy_UpdateCollider(EnHy* enHy, GlobalContext* globalCtx) {
    enHy->collider.dim.pos.x = enHy->actor.world.pos.x;
    enHy->collider.dim.pos.y = enHy->actor.world.pos.y;
    enHy->collider.dim.pos.z = enHy->actor.world.pos.z;

    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &enHy->collider.base);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &enHy->collider.base);
}

s32 EnHy_PlayWalkingSound(EnHy* enHy, GlobalContext* globalCtx, f32 distAboveThreshold) {
    u8 wasLeftFootOnGround = enHy->isLeftFootOnGround;
    u8 wasRightFootOnGround = enHy->isRightFootOnGround;
    s32 waterSfxId;
    u16 sfxId;
    u8 isFootOnGround;

    if (enHy->actor.bgCheckFlags & 0x20) {
        if (enHy->actor.depthInWater < 20.0f) {
            waterSfxId = NA_SE_PL_WALK_WATER0 - SFX_FLAG;
        } else {
            waterSfxId = NA_SE_PL_WALK_WATER1 - SFX_FLAG;
        }
        sfxId = waterSfxId + SFX_FLAG;
    } else {
        sfxId = SurfaceType_GetSfx(&globalCtx->colCtx, enHy->actor.floorPoly, enHy->actor.floorBgId) + SFX_FLAG;
    }

    enHy->isLeftFootOnGround = isFootOnGround = func_8013DB90(globalCtx, &enHy->leftFootPos, distAboveThreshold);
    if (enHy->isLeftFootOnGround && !wasLeftFootOnGround && isFootOnGround) {
        Actor_PlaySfxAtPos(&enHy->actor, sfxId);
    }

    enHy->isRightFootOnGround = isFootOnGround = func_8013DB90(globalCtx, &enHy->rightFootPos, distAboveThreshold);
    if (enHy->isRightFootOnGround && !wasRightFootOnGround && isFootOnGround) {
        Actor_PlaySfxAtPos(&enHy->actor, sfxId);
    }
    return 0;
}
