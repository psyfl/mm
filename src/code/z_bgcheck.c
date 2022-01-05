#include "global.h"
#include "vt.h"

#define DYNA_RAYCAST_FLOORS 1
#define DYNA_RAYCAST_WALLS 2
#define DYNA_RAYCAST_CEILINGS 4

static u32 sWallFlags[32] = {
    0, 1, 3, 5, 8, 16, 32, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static u16 sSurfaceTypeSfx[] = {
    /* 0x00 */ NA_SE_PL_WALK_GROUND - SFX_FLAG,
    /* 0x01 */ NA_SE_PL_WALK_SAND - SFX_FLAG,
    /* 0x02 */ NA_SE_PL_WALK_CONCRETE - SFX_FLAG,
    /* 0x03 */ NA_SE_PL_WALK_DIRT - SFX_FLAG,
    /* 0x04 */ NA_SE_PL_WALK_WATER0 - SFX_FLAG,
    /* 0x05 */ NA_SE_PL_WALK_WATER1 - SFX_FLAG,
    /* 0x06 */ NA_SE_PL_WALK_WATER2 - SFX_FLAG,
    /* 0x07 */ NA_SE_PL_WALK_MAGMA - SFX_FLAG,
    /* 0x08 */ NA_SE_PL_WALK_GRASS - SFX_FLAG,
    /* 0x09 */ NA_SE_PL_WALK_GLASS - SFX_FLAG,
    /* 0x0A */ NA_SE_PL_WALK_LADDER - SFX_FLAG,
    /* 0x0B */ NA_SE_PL_WALK_GROUND - SFX_FLAG,
    /* 0x0C */ NA_SE_PL_WALK_ICE - SFX_FLAG,
    /* 0x0D */ NA_SE_PL_WALK_IRON - SFX_FLAG,
    /* 0x0E */ NA_SE_PL_WALK_SNOW - SFX_FLAG
};

static u8 D_801B46C0[] = {
    /* 0x00 */ 1,
    /* 0x01 */ 1,
    /* 0x02 */ 0,
    /* 0x03 */ 1,
    /* 0x04 */ 0,
    /* 0x05 */ 0,
    /* 0x06 */ 0,
    /* 0x07 */ 0,
    /* 0x08 */ 0,
    /* 0x09 */ 0,
    /* 0x0A */ 0,
    /* 0x0B */ 0,
    /* 0x0C */ 0,
    /* 0x0D */ 0,
    /* 0x0E */ 1
};

static s16 sSmallMemScenes[] = {
    SCENE_F01,
};

typedef struct {
    s16 sceneId;
    u32 memSize;
} BgCheckSceneMemEntry;

static BgCheckSceneMemEntry sSceneMemList[] = {
    { SCENE_00KEIKOKU, 0xC800 },
};

BgCheckSceneSubdivisionEntry sSceneSubdivisionList[] = {
    { SCENE_00KEIKOKU, { 36, 1, 36 }, -1 },
    { SCENE_30GYOSON, { 40, 1, 40 }, -1 },
    { SCENE_31MISAKI, { 40, 1, 40 }, -1 },
};

BgSpecialSceneMaxObjects sCustomDynapolyMem[] = {
    { SCENE_21MITURINMAE, 1000, 600, 512 },
};

// TODO: All these bss variables are localized to one function and can
// likely be made into in-function static bss variables in the future
char D_801ED950[80];
char D_801ED9A0[80];
Vec3f D_801ED9F0[3]; // polyVerts
Vec3f D_801EDA18[3]; // polyVerts
MtxF D_801EDA40;
Vec3f D_801EDA80[3]; // polyVerts
char D_801EDAA8[80];
char D_801EDAF8[80];
Vec3f D_801EDB48[3]; // polyVerts
Vec3f D_801EDB70[3];
Plane D_801EDB98;

void BgCheck_GetStaticLookupIndicesFromPos(CollisionContext* colCtx, Vec3f* pos, Vec3i* sector);
f32 BgCheck_RaycastFloorDyna(DynaRaycast* dynaRaycast);
s32 BgCheck_SphVsDynaWall(CollisionContext* colCtx, u16 xpFlags, f32* outX, f32* outZ, Vec3f* pos, f32 radius,
                          CollisionPoly** outPoly, s32* outBgId, Actor* actor);
s32 BgCheck_CheckLineImpl(CollisionContext* colCtx, u16 xpFlags1, u16 xpFlags2, Vec3f* posA, Vec3f* posB,
                          Vec3f* posResult, CollisionPoly** outPoly, s32* outBgId, Actor* actor, f32 checkDist,
                          u32 bccFlags);
s32 BgCheck_CheckDynaCeiling(CollisionContext* colCtx, u16 xpFlags, f32* outY, Vec3f* pos, f32 checkDist,
                             CollisionPoly** outPoly, s32* outBgId, Actor* actor);
s32 BgCheck_CheckLineAgainstDyna(CollisionContext* colCtx, u16 xpFlags, Vec3f* posA, Vec3f* posB, Vec3f* posResult,
                                 CollisionPoly** outPoly, f32* distSq, s32* outBgId, Actor* actor, f32 checkDist,
                                 s32 bccFlags);
s32 BgCheck_SphVsFirstDynaPoly(CollisionContext* colCtx, u16 xpFlags, CollisionPoly** outPoly, s32* outBgId,
                               Vec3f* center, f32 radius, Actor* actor, u16 bciFlags);
void BgCheck_ResetPolyCheckTbl(SSNodeList* nodeList, s32 numPolys);
s32 BgCheck_PosInStaticBoundingBox(CollisionContext* colCtx, Vec3f* pos);

void SSNode_SetValue(SSNode* node, s16* polyIndex, u16 next) {
    node->polyId = *polyIndex;
    node->next = next;
}

void SSList_SetNull(SSList* ssList) {
    ssList->head = SS_NULL;
}

void SSNodeList_SetSSListHead(SSNodeList* list, SSList* ssList, s16* polyIndex) {
    u16 index;

    index = SSNodeList_GetNextNodeIdx(list);
    SSNode_SetValue(&list->tbl[index], polyIndex, ssList->head);
    ssList->head = index;
}

void DynaSSNodeList_SetSSListHead(DynaSSNodeList* list, SSList* ssList, s16* polyIndex) {
    u16 index;

    index = DynaSSNodeList_GetNextNodeIdx(list);
    SSNode_SetValue(&list->tbl[index], polyIndex, ssList->head);
    ssList->head = index;
}

void DynaSSNodeList_Init(GlobalContext* globalCtx, DynaSSNodeList* list) {
    list->tbl = NULL;
    list->count = 0;
}

void DynaSSNodeList_Alloc(GlobalContext* globalCtx, DynaSSNodeList* list, u32 numNodes) {
    list->tbl = (SSNode*)THA_AllocEndAlign(&globalCtx->state.heap, numNodes * sizeof(SSNode), -2);
    list->maxNodes = numNodes;
    list->count = 0;
}

void DynaSSNodeList_ResetCount(DynaSSNodeList* list) {
    list->count = 0;
}

u16 DynaSSNodeList_GetNextNodeIdx(DynaSSNodeList* list) {
    u16 index = list->count++;

    if (list->maxNodes <= index) {
        return SS_NULL;
    }

    return index;
}

void BgCheck_Vec3sToVec3f(Vec3s* src, Vec3f* dest) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

void BgCheck_Vec3fToVec3s(Vec3s* dest, Vec3f* src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

f32 func_800BFD84(CollisionPoly* poly, f32 arg1, f32 arg2) {
    return (COLPOLY_GET_NORMAL(poly->normal.x * arg1 + poly->normal.z * arg2) + poly->dist) /
           COLPOLY_GET_NORMAL(-poly->normal.y);
}

/**
 * Unused
 */
s32 func_800BFDEC(CollisionPoly* polyA, CollisionPoly* polyB, u32* outVtxId0, u32* outVtxId1) {
    s32 vtxIdA[3];
    s32 vtxIdB[3];
    s32 i;
    s32 j;
    s32 count;

    *outVtxId0 = *outVtxId1 = 0;
    for (i = 0; i < ARRAY_COUNT(vtxIdA); i++) {
        vtxIdA[i] = COLPOLY_VTX_INDEX(polyA->vtxData[i]);
        vtxIdB[i] = COLPOLY_VTX_INDEX(polyB->vtxData[i]);
    }

    count = 0;
    for (i = 0; i < 2; i++) {
        for (j = i + 1; j < 3; j++) {
            if (vtxIdA[i] == vtxIdB[j]) {
                if (count == 0) {
                    *outVtxId0 = vtxIdA[i];
                } else if (count == 1) {
                    *outVtxId1 = vtxIdA[i];
                }
                count++;
            }
        }
    }
    return count;
}

s16 CollisionPoly_GetMinY(CollisionPoly* poly, Vec3s* vertices) {
    s16 minY;
    s32 a = COLPOLY_VTX_INDEX(poly->flags_vIA);
    s32 b = COLPOLY_VTX_INDEX(poly->flags_vIB);
    s32 c = poly->vIC;

    minY = vertices[a].y;
    if (vertices[b].y < minY) {
        minY = vertices[b].y;
    }
    if (minY < vertices[c].y) {
        return minY;
    }
    return vertices[c].y;
}

void CollisionPoly_GetNormalF(CollisionPoly* poly, f32* nx, f32* ny, f32* nz) {
    *nx = COLPOLY_GET_NORMAL(poly->normal.x);
    *ny = COLPOLY_GET_NORMAL(poly->normal.y);
    *nz = COLPOLY_GET_NORMAL(poly->normal.z);
}

/**
 * Compute transform matrix mapping +y (up) to the collision poly's normal
 */
void func_800C0094(CollisionPoly* poly, f32 tx, f32 ty, f32 tz, MtxF* dest) {
    f32 nx;
    f32 ny;
    f32 nz;
    s32 pad;
    f32 z_f14;
    f32 phi_f14;
    f32 phi_f12;
    f32 inv_z_f14;

    if (poly == NULL) {
        return;
    }
    CollisionPoly_GetNormalF(poly, &nx, &ny, &nz);

    z_f14 = sqrtf(SQ(ny) + SQ(nz));
    if (!IS_ZERO(z_f14)) {
        inv_z_f14 = 1.0f / z_f14;
        phi_f14 = ny * inv_z_f14;
        phi_f12 = nz * inv_z_f14;
    } else {
        phi_f14 = 1.0f;
        phi_f12 = 0.0f;
    }
    dest->xx = z_f14;
    dest->xy = (-nx) * phi_f14;
    dest->xz = (-nx) * phi_f12;
    dest->yx = nx;
    dest->yy = ny;
    dest->yz = nz;
    dest->zx = 0.0f;
    dest->zy = -phi_f12;
    dest->zz = phi_f14;
    dest->wx = tx;
    dest->wy = ty;
    dest->wz = tz;
    dest->xw = 0.0f;
    dest->yw = 0.0f;
    dest->zw = 0.0f;
    dest->ww = 1.0f;
}

f32 CollisionPoly_GetPointDistanceFromPlane(CollisionPoly* poly, Vec3f* point) {
    return COLPOLY_GET_NORMAL(poly->normal.x * point->x + poly->normal.y * point->y + poly->normal.z * point->z) +
           poly->dist;
}

void CollisionPoly_GetVertices(CollisionPoly* poly, Vec3s* vtxList, Vec3f* dest) {
    BgCheck_Vec3sToVec3f(&vtxList[COLPOLY_VTX_INDEX(poly->flags_vIA)], &dest[0]);
    BgCheck_Vec3sToVec3f(&vtxList[COLPOLY_VTX_INDEX(poly->flags_vIB)], &dest[1]);
    BgCheck_Vec3sToVec3f(&vtxList[poly->vIC], &dest[2]);
}

void CollisionPoly_GetVerticesByBgId(CollisionPoly* poly, s32 bgId, CollisionContext* colCtx, Vec3f* dest) {
    Vec3s* vtxList;

    if (poly == NULL || bgId > BG_ACTOR_MAX || dest == NULL) {
        if (dest != NULL) {
            // @bug: dest[2] x and y are not set to 0
            dest[0].x = dest[0].y = dest[0].z = dest[1].x = dest[1].y = dest[1].z = dest[2].z = 0.0f;
        }
    } else {
        if (bgId == BGCHECK_SCENE) {
            vtxList = colCtx->colHeader->vtxList;
        } else {
            vtxList = colCtx->dyna.vtxList;
        }
        CollisionPoly_GetVertices(poly, vtxList, dest);
    }
}

s32 CollisionPoly_CheckYIntersectApprox1(CollisionPoly* poly, Vec3s* vtxList, f32 x, f32 z, f32* yIntersect,
                                         f32 checkDist) {
    f32 nx;
    f32 ny;
    f32 nz;
    Vec3s* vA;
    Vec3s* vB;
    Vec3s* vC;

    vA = &vtxList[COLPOLY_VTX_INDEX(poly->flags_vIA)];
    Math_Vec3s_ToVec3f(&D_801ED9F0[0], vA);
    vB = &vtxList[COLPOLY_VTX_INDEX(poly->flags_vIB)];
    Math_Vec3s_ToVec3f(&D_801ED9F0[1], vB);
    vC = &vtxList[poly->vIC];
    Math_Vec3s_ToVec3f(&D_801ED9F0[2], vC);

    nx = COLPOLY_GET_NORMAL(poly->normal.x);
    ny = COLPOLY_GET_NORMAL(poly->normal.y);
    nz = COLPOLY_GET_NORMAL(poly->normal.z);

    return Math3D_TriChkPointParaYIntersectDist(&D_801ED9F0[0], &D_801ED9F0[1], &D_801ED9F0[2], nx, ny, nz, poly->dist,
                                                z, x, yIntersect, checkDist);
}

/**
 * Checks if point (`x`,`z`) is within `checkDist` of `poly`, computing `yIntersect` if true
 * Determinant max 0.0f (checks if on or within poly)
 */
s32 CollisionPoly_CheckYIntersect(CollisionPoly* poly, Vec3s* vtxList, f32 x, f32 z, f32* yIntersect, f32 checkDist) {
    Vec3s* sVerts;
    f32 nx;
    f32 ny;
    f32 nz;

    sVerts = &vtxList[COLPOLY_VTX_INDEX(poly->flags_vIA)];
    D_801EDA18[0].x = sVerts->x;
    D_801EDA18[0].y = sVerts->y;
    D_801EDA18[0].z = sVerts->z;
    sVerts = &vtxList[COLPOLY_VTX_INDEX(poly->flags_vIB)];
    D_801EDA18[1].x = sVerts->x;
    D_801EDA18[1].y = sVerts->y;
    D_801EDA18[1].z = sVerts->z;
    sVerts = &vtxList[(s32)poly->vIC];
    D_801EDA18[2].x = sVerts->x;
    D_801EDA18[2].y = sVerts->y;
    D_801EDA18[2].z = sVerts->z;

    if (!func_8017A304(&D_801EDA18[0], &D_801EDA18[1], &D_801EDA18[2], z, x, checkDist)) {
        return 0;
    }
    nx = COLPOLY_GET_NORMAL(poly->normal.x);
    ny = COLPOLY_GET_NORMAL(poly->normal.y);
    nz = COLPOLY_GET_NORMAL(poly->normal.z);
    return Math3D_TriChkPointParaYIntersectInsideTri2(&D_801EDA18[0], &D_801EDA18[1], &D_801EDA18[2], nx, ny, nz,
                                                      poly->dist, z, x, yIntersect, checkDist);
}

s32 CollisionPoly_CheckYIntersectApprox2(CollisionPoly* poly, Vec3s* vtxList, f32 x, f32 z, f32* yIntersect) {
    return CollisionPoly_CheckYIntersectApprox1(poly, vtxList, x, z, yIntersect, 1.0f);
}

s32 CollisionPoly_CheckXIntersectApprox(CollisionPoly* poly, Vec3s* vtxList, f32 y, f32 z, f32* xIntersect) {
    f32 nx;
    f32 ny;
    f32 nz;

    CollisionPoly_GetVertices(poly, vtxList, D_801EDA80);
    CollisionPoly_GetNormalF(poly, &nx, &ny, &nz);
    return Math3D_TriChkPointParaXIntersect(&D_801EDA80[0], &D_801EDA80[1], &D_801EDA80[2], nx, ny, nz, poly->dist, y,
                                            z, xIntersect);
}

s32 CollisionPoly_CheckZIntersectApprox(CollisionPoly* poly, Vec3s* vtxList, f32 x, f32 y, f32* zIntersect) {
    f32 nx;
    f32 ny;
    f32 nz;

    CollisionPoly_GetVertices(poly, vtxList, D_801EDB48);
    CollisionPoly_GetNormalF(poly, &nx, &ny, &nz);
    return Math3D_TriChkPointParaZIntersect(&D_801EDB48[0], &D_801EDB48[1], &D_801EDB48[2], nx, ny, nz, poly->dist, x,
                                            y, zIntersect);
}

#ifdef NON_MATCHING
s32 CollisionPoly_LineVsPoly(BgLineVsPolyTest* a0) {
    static Vec3f polyVerts[3]; // D_801EDB70
    static Plane plane;        // D_801EDB98
    f32 planeDistA;
    f32 planeDistB;
    f32 planeDistDelta;

    plane.originDist = a0->poly->dist;
    planeDistA = COLPOLY_GET_NORMAL(a0->poly->normal.x * a0->posA->x + a0->poly->normal.y * a0->posA->y +
                                    a0->poly->normal.z * a0->posA->z) +
                 plane.originDist;
    planeDistB = COLPOLY_GET_NORMAL(a0->poly->normal.x * a0->posB->x + a0->poly->normal.y * a0->posB->y +
                                    a0->poly->normal.z * a0->posB->z) +
                 plane.originDist;

    planeDistDelta = planeDistA - planeDistB;
    if ((planeDistA >= 0.0f && planeDistB >= 0.0f) || (planeDistA < 0.0f && planeDistB < 0.0f) ||
        ((a0->checkOneFace != 0) && (planeDistA < 0.0f && planeDistB > 0.0f)) || IS_ZERO(planeDistDelta)) {
        return false;
    }

    CollisionPoly_GetNormalF(a0->poly, &plane.normal.x, &plane.normal.y, &plane.normal.z);
    CollisionPoly_GetVertices(a0->poly, a0->vtxList, polyVerts);
    Math3D_Lerp(a0->posA, a0->posB, planeDistA / planeDistDelta, a0->planeIntersect);

    if ((fabsf(plane.normal.x) > 0.5f &&
         Math3D_TriChkPointParaXDist(&polyVerts[0], &polyVerts[1], &polyVerts[2], a0->planeIntersect->y,
                                     a0->planeIntersect->z, 0.0f, a0->checkDist, plane.normal.x)) ||
        (fabsf(plane.normal.y) > 0.5f &&
         Math3D_TriChkPointParaYDist(&polyVerts[0], &polyVerts[1], &polyVerts[2], a0->planeIntersect->z,
                                     a0->planeIntersect->x, 0.0f, a0->checkDist, plane.normal.y)) ||
        (fabsf(plane.normal.z) > 0.5f &&
         Math3D_TriChkLineSegParaZDist(&polyVerts[0], &polyVerts[1], &polyVerts[2], a0->planeIntersect->x,
                                       a0->planeIntersect->y, 0.0f, a0->checkDist, plane.normal.z))) {
        return true;
    }
    return false;
}
#else
s32 CollisionPoly_LineVsPoly(BgLineVsPolyTest* a0);
#pragma GLOBAL_ASM("asm/non_matchings/code/z_bgcheck/CollisionPoly_LineVsPoly.s")
#endif

s32 CollisionPoly_SphVsPoly(CollisionPoly* poly, Vec3s* vtxList, Vec3f* center, f32 radius) {
    static Sphere16 sphere; // D_801EDBA8
    static TriNorm tri;     // D_801EDBB0
    Vec3f intersect;

    CollisionPoly_GetVertices(poly, vtxList, tri.vtx);
    CollisionPoly_GetNormalF(poly, &tri.plane.normal.x, &tri.plane.normal.y, &tri.plane.normal.z);
    tri.plane.originDist = poly->dist;
    sphere.center.x = center->x;
    sphere.center.y = center->y;
    sphere.center.z = center->z;
    sphere.radius = radius;
    return Math3D_ColSphereTri(&sphere, &tri, &intersect);
}

/**
 * Add poly to StaticLookup table
 * Table is sorted by poly's smallest y vertex component
 * `ssList` is the list to append a new poly to
 * `polyList` is the CollisionPoly lookup list
 * `vtxList` is the vertex lookup list
 * `polyId` is the index of the poly in polyList to insert into the lookup table
 */
void StaticLookup_AddPolyToSSList(CollisionContext* colCtx, SSList* ssList, CollisionPoly* polyList, Vec3s* vtxList,
                                  s16 polyId) {
    SSNode* curNode;
    SSNode* nextNode;
    s32 polyYMin;
    u16 newNodeId;
    s16 curPolyId;

    // if list is null
    if (ssList->head == SS_NULL) {
        SSNodeList_SetSSListHead(&colCtx->polyNodes, ssList, &polyId);
        return;
    }

    polyYMin = CollisionPoly_GetMinY(&polyList[polyId], vtxList);

    curNode = &colCtx->polyNodes.tbl[ssList->head];
    curPolyId = curNode->polyId;

    // if the poly being inserted has a lower y than the first poly
    if (polyYMin < vtxList[COLPOLY_VTX_INDEX(polyList[curPolyId].flags_vIA)].y &&
        polyYMin < vtxList[COLPOLY_VTX_INDEX(polyList[curPolyId].flags_vIB)].y &&
        polyYMin < vtxList[polyList[curPolyId].vIC].y) {
        SSNodeList_SetSSListHead(&colCtx->polyNodes, ssList, &polyId);
        return;
    }
    while (true) {
        // if at the end of the list
        if (curNode->next == SS_NULL) {
            s32 pad;

            newNodeId = SSNodeList_GetNextNodeIdx(&colCtx->polyNodes);
            SSNode_SetValue(&colCtx->polyNodes.tbl[newNodeId], &polyId, SS_NULL);
            curNode->next = newNodeId;
            break;
        }

        nextNode = &colCtx->polyNodes.tbl[curNode->next];
        curPolyId = nextNode->polyId;

        // if the poly being inserted is lower than the next poly
        if (polyYMin < vtxList[COLPOLY_VTX_INDEX(polyList[curPolyId].flags_vIA)].y &&
            polyYMin < vtxList[COLPOLY_VTX_INDEX(polyList[curPolyId].flags_vIB)].y &&
            polyYMin < vtxList[polyList[curPolyId].vIC].y) {
            newNodeId = SSNodeList_GetNextNodeIdx(&colCtx->polyNodes);
            SSNode_SetValue(&colCtx->polyNodes.tbl[newNodeId], &polyId, curNode->next);
            curNode->next = newNodeId;
            break;
        }
        curNode = nextNode;
    }
}

/**
 * Add CollisionPoly to StaticLookup list
 */
void StaticLookup_AddPoly(StaticLookup* lookup, CollisionContext* colCtx, CollisionPoly* polyList, Vec3s* vtxList,
                          s16 index) {
    if (polyList[index].normal.y > COLPOLY_SNORMAL(0.5f)) {
        StaticLookup_AddPolyToSSList(colCtx, &lookup->floor, polyList, vtxList, index);
    } else if (polyList[index].normal.y < COLPOLY_SNORMAL(-0.8f)) {
        StaticLookup_AddPolyToSSList(colCtx, &lookup->ceiling, polyList, vtxList, index);
    } else {
        StaticLookup_AddPolyToSSList(colCtx, &lookup->wall, polyList, vtxList, index);
    }
}

/**
 * Locates the closest static poly directly underneath `pos`, starting at list `ssList`
 * returns yIntersect of the closest poly, or `yIntersectMin`
 * stores the pointer of the closest poly to `outPoly`
 * if (flags & 1), ignore polys with a normal.y < 0 (from vertical walls to ceilings)
 */
f32 BgCheck_RaycastFloorStaticList(CollisionContext* colCtx, u16 xpFlags, SSList* ssList, CollisionPoly** outPoly,
                                   Vec3f* pos, f32 yIntersectMin, f32 checkDist, s32 flags, Actor* actor, s32 arg9) {
    SSNode* curNode;
    s32 polyId;
    f32 result;
    f32 yIntersect;
    CollisionPoly* colPoly;
    s32 pad;

    result = yIntersectMin;
    if (ssList->head == SS_NULL) {
        return result;
    }

    curNode = &colCtx->polyNodes.tbl[ssList->head];

    while (true) {
        polyId = curNode->polyId;
        colPoly = &colCtx->colHeader->polyList[polyId];

        if (((flags & 1) && (colPoly->normal.y < 0)) ||
            ((arg9 & 0x20) && ((colCtx->colHeader->surfaceTypeList[colPoly->type].data[0] >> 0x1E) & 1)) ||
            COLPOLY_VIA_FLAG_TEST(colPoly->flags_vIA, xpFlags) ||
            (COLPOLY_VIA_FLAG_TEST(colPoly->flags_vIB, 4) && ((actor != NULL && actor->category != ACTORCAT_PLAYER) ||
                                                              (actor == NULL && xpFlags != COLPOLY_IGNORE_CAMERA)))) {
            if (curNode->next == SS_NULL) {
                break;
            }
            curNode = &colCtx->polyNodes.tbl[curNode->next];
            continue;
        }

        if (pos->y < colCtx->colHeader->vtxList[COLPOLY_VTX_INDEX(colPoly->flags_vIA)].y &&
            pos->y < colCtx->colHeader->vtxList[COLPOLY_VTX_INDEX(colPoly->flags_vIB)].y &&
            pos->y < colCtx->colHeader->vtxList[colPoly->vIC].y) {
            break;
        }

        if (CollisionPoly_CheckYIntersect(colPoly, colCtx->colHeader->vtxList, pos->x, pos->z, &yIntersect,
                                          checkDist)) {
            // if poly is closer to pos without going over
            if (yIntersect < pos->y && result < yIntersect) {
                result = yIntersect;
                *outPoly = colPoly;
            }
        }

        if (curNode->next == SS_NULL) {
            break;
        }
        curNode = &colCtx->polyNodes.tbl[curNode->next];
    }
    return result;
}

/**
 * Locates the closest static poly directly underneath `pos` within `lookup`.
 * returns yIntersect of the closest poly, or `yIntersectMin`
 * stores the pointer of the closest poly to `outPoly`
 */
f32 BgCheck_RaycastFloorStatic(StaticLookup* lookup, CollisionContext* colCtx, u16 xpFlags, CollisionPoly** poly,
                               Vec3f* pos, u32 arg5, f32 checkDist, f32 yIntersectMin, Actor* actor) {
    s32 flag; // skip polys with normal.y < 0
    f32 yIntersect = yIntersectMin;

    if (arg5 & 4) {
        yIntersect = BgCheck_RaycastFloorStaticList(colCtx, xpFlags, &lookup->floor, poly, pos, yIntersect, checkDist,
                                                    0, actor, arg5);
    }

    if ((arg5 & 2) || (arg5 & 8)) {
        flag = 0;
        if (arg5 & 0x10) {
            flag = 1;
        }
        yIntersect = BgCheck_RaycastFloorStaticList(colCtx, xpFlags, &lookup->wall, poly, pos, yIntersect, checkDist,
                                                    flag, actor, arg5);
    }

    if (arg5 & 1) {
        flag = 0;
        if (arg5 & 0x10) {
            flag = 1;
        }
        yIntersect = BgCheck_RaycastFloorStaticList(colCtx, xpFlags, &lookup->ceiling, poly, pos, yIntersect, checkDist,
                                                    flag, actor, arg5);
    }

    return yIntersect;
}

/**
 * Compute wall displacement on `posX` and `posZ`
 * sets `wallPolyPtr` to `poly` if `wallPolyPtr` is NULL
 * returns true if `wallPolyPtr` was changed
 * `invXZlength` is 1 / sqrt( sq(poly.normal.x) + sq(poly.normal.z) )
 */
s32 BgCheck_ComputeWallDisplacement(CollisionContext* colCtx, CollisionPoly* poly, f32* posX, f32* posZ, f32 nx, f32 ny,
                                    f32 nz, f32 invXZlength, f32 planeDist, f32 radius, CollisionPoly** wallPolyPtr) {
    f32 displacement = (radius - planeDist) * invXZlength;

    *posX += displacement * nx;
    *posZ += displacement * nz;

    if (*wallPolyPtr == NULL) {
        *wallPolyPtr = poly;
        return true;
    } else {
        return false;
    }
}

/**
 * Performs collision detection on static poly walls within `lookup` on sphere `pos`, `radius`
 * returns true if a collision was detected
 * `outX` `outZ` return the displaced x,z coordinates,
 * `outPoly` returns the pointer to the nearest poly collided with, or NULL
 */
s32 BgCheck_SphVsStaticWall(StaticLookup* lookup, CollisionContext* colCtx, u16 xpFlags, f32* outX, f32* outZ,
                            Vec3f* pos, f32 radius, CollisionPoly** outPoly, Actor* actor) {
    Vec3f resultPos;
    f32 zTemp;
    f32 xTemp;
    f32 planeDist;
    f32 intersect;
    s32 result;
    CollisionPoly* curPoly;
    CollisionPoly* polyList;
    SSNode* curNode;
    f32 invNormalXZ;
    Vec3s* vtxA;
    Vec3s* vtxB;
    Vec3s* vtxC;
    s32 polyId;
    f32 normalXZ;
    f32 nx;
    f32 ny;
    f32 nz;
    Vec3s* vtxList;
    u16 pad;
    f32 temp_f16;
    f32 zMin;
    f32 zMax;
    f32 xMin;
    f32 xMax;

    result = false;
    if (lookup->wall.head == SS_NULL) {
        return result;
    }
    resultPos = *pos;

    polyList = colCtx->colHeader->polyList;
    vtxList = colCtx->colHeader->vtxList;
    curNode = &colCtx->polyNodes.tbl[lookup->wall.head];

    while (true) {
        polyId = curNode->polyId;
        curPoly = &polyList[polyId];
        vtxA = &vtxList[COLPOLY_VTX_INDEX(curPoly->flags_vIA)];
        vtxB = &vtxList[COLPOLY_VTX_INDEX(curPoly->flags_vIB)];
        vtxC = &vtxList[curPoly->vIC];

        if (pos->y < vtxA->y && pos->y < vtxB->y && pos->y < vtxC->y) {
            break;
        }

        nx = COLPOLY_GET_NORMAL(curPoly->normal.x);
        ny = COLPOLY_GET_NORMAL(curPoly->normal.y);
        nz = COLPOLY_GET_NORMAL(curPoly->normal.z);
        normalXZ = sqrtf(SQ(nx) + SQ(nz));
        planeDist = Math3D_DistPlaneToPos(nx, ny, nz, curPoly->dist, &resultPos);
        if (radius < fabsf(planeDist) || COLPOLY_VIA_FLAG_TEST(curPoly->flags_vIA, xpFlags) ||
            (COLPOLY_VIA_FLAG_TEST(curPoly->flags_vIB, 4) && ((actor != NULL && actor->category != ACTORCAT_PLAYER) ||
                                                              (actor == NULL && xpFlags != COLPOLY_IGNORE_CAMERA)))) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &colCtx->polyNodes.tbl[curNode->next];
                continue;
            }
        }

        invNormalXZ = 1.0f / normalXZ;
        temp_f16 = fabsf(nz) * invNormalXZ;
        if (temp_f16 < 0.4f) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &colCtx->polyNodes.tbl[curNode->next];
                continue;
            }
        }

        // compute curPoly zMin/zMax
        zTemp = vtxA->z;
        zMax = zMin = zTemp;
        zTemp = vtxB->z;

        if (zTemp < zMin) {
            zMin = zTemp;
        } else if (zMax < zTemp) {
            zMax = zTemp;
        }
        zTemp = vtxC->z;
        if (zTemp < zMin) {
            zMin = zTemp;
        } else if (zTemp > zMax) {
            zMax = zTemp;
        }

        zMin -= radius;
        zMax += radius;

        if (resultPos.z < zMin || resultPos.z > zMax) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &colCtx->polyNodes.tbl[curNode->next];
                continue;
            }
        }
        if (CollisionPoly_CheckZIntersectApprox(curPoly, vtxList, resultPos.x, pos->y, &intersect)) {
            f32 test = intersect - resultPos.z;

            if (fabsf(test) <= radius / temp_f16) {
                if (test * nz <= 4.0f) {
                    if (BgCheck_ComputeWallDisplacement(colCtx, curPoly, &resultPos.x, &resultPos.z, nx, ny, nz,
                                                        invNormalXZ, planeDist, radius, outPoly)) {
                        result = true;
                    }
                }
            }
        }
        if (curNode->next == SS_NULL) {
            break;
        } else {
            curNode = &colCtx->polyNodes.tbl[curNode->next];
        }
    }

    curNode = &colCtx->polyNodes.tbl[lookup->wall.head];

    while (true) {
        polyId = curNode->polyId;
        curPoly = &polyList[polyId];
        vtxA = &vtxList[COLPOLY_VTX_INDEX(curPoly->flags_vIA)];
        vtxB = &vtxList[COLPOLY_VTX_INDEX(curPoly->flags_vIB)];
        vtxC = &vtxList[curPoly->vIC];

        if (pos->y < vtxA->y && pos->y < vtxB->y && pos->y < vtxC->y) {
            break;
        }

        nx = COLPOLY_GET_NORMAL(curPoly->normal.x);
        ny = COLPOLY_GET_NORMAL(curPoly->normal.y);
        nz = COLPOLY_GET_NORMAL(curPoly->normal.z);
        normalXZ = sqrtf(SQ(nx) + SQ(nz));
        planeDist = Math3D_DistPlaneToPos(nx, ny, nz, curPoly->dist, &resultPos);
        if (radius < fabsf(planeDist) || COLPOLY_VIA_FLAG_TEST(curPoly->flags_vIA, xpFlags) ||
            (COLPOLY_VIA_FLAG_TEST(curPoly->flags_vIB, 4) && ((actor != NULL && actor->category != ACTORCAT_PLAYER) ||
                                                              (actor == NULL && xpFlags != COLPOLY_IGNORE_CAMERA)))) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &colCtx->polyNodes.tbl[curNode->next];
                continue;
            }
        }

        invNormalXZ = 1.0f / normalXZ;
        temp_f16 = fabsf(nx) * invNormalXZ;
        if (temp_f16 < 0.4f) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &colCtx->polyNodes.tbl[curNode->next];
                continue;
            }
        }

        // compute curPoly xMin/xMax
        xTemp = vtxA->x;
        xMax = xMin = xTemp;
        xTemp = vtxB->x;

        if (xTemp < xMin) {
            xMin = xTemp;
        } else if (xMax < xTemp) {
            xMax = xTemp;
        }
        xTemp = vtxC->x;
        if (xTemp < xMin) {
            xMin = xTemp;
        } else if (xMax < xTemp) {
            xMax = xTemp;
        }

        xMin -= radius;
        xMax += radius;

        if (resultPos.x < xMin || xMax < resultPos.x) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &colCtx->polyNodes.tbl[curNode->next];
                continue;
            }
        }
        if (CollisionPoly_CheckXIntersectApprox(curPoly, vtxList, pos->y, resultPos.z, &intersect)) {
            f32 test = intersect - resultPos.x;

            if (fabsf(test) <= radius / temp_f16) {
                if (test * nx <= 4.0f) {
                    if (BgCheck_ComputeWallDisplacement(colCtx, curPoly, &resultPos.x, &resultPos.z, nx, ny, nz,
                                                        invNormalXZ, planeDist, radius, outPoly)) {
                        result = true;
                    }
                }
            }
        }
        if (curNode->next == SS_NULL) {
            break;
        } else {
            curNode = &colCtx->polyNodes.tbl[curNode->next];
            continue;
        }
    }

    *outX = resultPos.x;
    *outZ = resultPos.z;
    return result;
}

/**
 * Tests for collision with a static poly ceiling
 * returns true if a collision occurs, else false
 * `outPoly` returns the poly collided with
 * `outY` returns the y coordinate needed to not collide with `outPoly`
 */
s32 BgCheck_CheckStaticCeiling(StaticLookup* lookup, u16 xpFlags, CollisionContext* colCtx, f32* outY, Vec3f* pos,
                               f32 checkHeight, CollisionPoly** outPoly, Actor* actor) {
    s32 result = false;
    CollisionPoly* curPoly;
    CollisionPoly* polyList;
    s32 curPolyId;
    f32 ceilingY;
    SSNode* curNode;
    Vec3s* vtxList;

    if (lookup->ceiling.head == SS_NULL) {
        return result;
    }
    polyList = colCtx->colHeader->polyList;
    vtxList = colCtx->colHeader->vtxList;
    curNode = &colCtx->polyNodes.tbl[lookup->ceiling.head];

    *outY = pos->y;

    while (true) {
        f32 intersectDist;
        f32 ny;

        curPolyId = curNode->polyId;
        curPoly = &polyList[curPolyId];
        if (COLPOLY_VIA_FLAG_TEST(colCtx->colHeader->polyList[curPolyId].flags_vIA, xpFlags) ||
            (COLPOLY_VIA_FLAG_TEST(colCtx->colHeader->polyList[curPolyId].flags_vIB, 4) &&
             ((actor != NULL && actor->category != ACTORCAT_PLAYER) ||
              (actor == NULL && xpFlags != COLPOLY_IGNORE_CAMERA)))) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &colCtx->polyNodes.tbl[curNode->next];
                continue;
            }
        }

        if (CollisionPoly_CheckYIntersectApprox2(curPoly, vtxList, pos->x, pos->z, &ceilingY)) {
            intersectDist = ceilingY - *outY;
            ny = COLPOLY_GET_NORMAL(curPoly->normal.y);

            if (intersectDist > 0 && intersectDist < checkHeight && intersectDist * ny <= 0) {
                *outY = ceilingY - checkHeight;
                *outPoly = curPoly;
                result = true;
            }
        }
        if (curNode->next == SS_NULL) {
            break;
        } else {
            curNode = &colCtx->polyNodes.tbl[curNode->next];
            continue;
        }
    }
    return result;
}

/**
 * Tests if line `posA` to `posB` intersects with a static poly in list `ssList`. Uses polyCheckTbl
 * returns true if such a poly exists, else false
 * `outPoly` returns the pointer of the poly intersected
 * `posB` and `outPos` returns the point of intersection with `outPoly`
 * `outDistSq` returns the squared distance from `posA` to the point of intersect
 */
#ifdef NON_MATCHING
s32 BgCheck_CheckLineAgainstSSList(StaticLineTest* arg0) {
    CollisionContext* colCtx;
    s32 result;
    Vec3f polyIntersect; // sp7C
    SSNode* curNode;
    u8* checkedPoly;
    f32 minY;
    f32 distSq;
    BgLineVsPolyTest test; // sp50
    s16 polyId;
    CollisionPoly* polyList;

    result = false;
    if (arg0->ssList->head == SS_NULL) {
        return result;
    }
    colCtx = arg0->colCtx;
    curNode = &colCtx->polyNodes.tbl[arg0->ssList->head];
    polyList = colCtx->colHeader->polyList;
    test.vtxList = colCtx->colHeader->vtxList;
    test.posA = arg0->posA;
    test.posB = arg0->posB;
    test.planeIntersect = &polyIntersect; // reorder maybe
    test.checkOneFace = (arg0->bccFlags & BGCHECK_CHECK_ONE_FACE) != 0;
    test.checkDist = arg0->checkDist;

    while (true) {
        polyId = curNode->polyId;
        test.poly = &polyList[polyId];
        checkedPoly = &arg0->colCtx->polyNodes.polyCheckTbl[polyId];

        if (*checkedPoly == true ||
            (arg0->xpFlags2 != 0 && !COLPOLY_VIA_FLAG_TEST(test.poly->flags_vIA, arg0->xpFlags2)) ||
            COLPOLY_VIA_FLAG_TEST(test.poly->flags_vIA, arg0->xpFlags1) ||
            (COLPOLY_VIA_FLAG_TEST(test.poly->flags_vIB, 4) &&
             ((arg0->actor != NULL && arg0->actor->category != ACTORCAT_PLAYER) ||
              (arg0->actor == NULL && arg0->xpFlags1 != COLPOLY_IGNORE_CAMERA)))) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &colCtx->polyNodes.tbl[curNode->next];
                continue;
            }
        }
        *checkedPoly = true;
        minY = CollisionPoly_GetMinY(test.poly, test.vtxList);
        if (test.posA->y < minY && test.posB->y < minY) {
            break;
        }
        if (CollisionPoly_LineVsPoly(&test)) {
            distSq = Math3D_Vec3fDistSq(test.posA, test.planeIntersect);
            if (distSq < arg0->outDistSq) {
                arg0->outDistSq = distSq;
                *arg0->outPos = *test.planeIntersect;
                *arg0->posB = *test.planeIntersect;
                *arg0->outPoly = test.poly;
                result = true;
            }
        }
        if (curNode->next == SS_NULL) {
            break;
        }
        curNode = &arg0->colCtx->polyNodes.tbl[curNode->next];
    }
    return result;
}
#else
s32 BgCheck_CheckLineAgainstSSList(StaticLineTest* arg0);
#pragma GLOBAL_ASM("asm/non_matchings/code/z_bgcheck/BgCheck_CheckLineAgainstSSList.s")
#endif

/**
 * Tests if line `posA` to `posB` intersects with a static poly in `lookup`. Uses polyCheckTbl
 * returns true if such a poly exists, else false
 * `outPoly` returns the pointer of the poly intersected
 * `posB` and `outPos` returns the point of intersection with `outPoly`
 * `outDistSq` returns the squared distance from `posA` to the point of intersect
 */
s32 BgCheck_CheckLineInSubdivision(StaticLineTest* arg0) {
    s32 result = false;

    if ((arg0->bccFlags & BGCHECK_CHECK_FLOOR) && arg0->lookup->floor.head != SS_NULL) {
        arg0->ssList = &arg0->lookup->floor;
        if (BgCheck_CheckLineAgainstSSList(arg0)) {
            result = true;
        }
    }

    if ((arg0->bccFlags & BGCHECK_CHECK_WALL) && arg0->lookup->wall.head != SS_NULL) {
        arg0->ssList = &arg0->lookup->wall;
        if (BgCheck_CheckLineAgainstSSList(arg0)) {
            result = true;
        }
    }

    if ((arg0->bccFlags & BGCHECK_CHECK_CEILING) && arg0->lookup->ceiling.head != SS_NULL) {
        arg0->ssList = &arg0->lookup->ceiling;
        if (BgCheck_CheckLineAgainstSSList(arg0)) {
            result = true;
        }
    }
    return result;
}

/**
 * Get first static poly intersecting sphere `center` `radius` from list `node`
 * returns true if any poly intersects the sphere, else returns false
 * `outPoly` returns the pointer of the first poly found that intersects
 */
s32 BgCheck_SphVsFirstStaticPolyList(SSNode* node, u16 xpFlags, CollisionContext* colCtx, Vec3f* center, f32 radius,
                                     CollisionPoly** outPoly, Actor* actor) {
    Vec3s* vtxList;
    CollisionPoly* polyList;
    CollisionPoly* curPoly;
    u16 nextId;
    s16 curPolyId;

    polyList = colCtx->colHeader->polyList;
    vtxList = colCtx->colHeader->vtxList;

    while (true) {
        curPolyId = node->polyId;
        curPoly = &polyList[curPolyId];
        if (COLPOLY_VIA_FLAG_TEST(colCtx->colHeader->polyList[curPolyId].flags_vIA, xpFlags) ||
            (COLPOLY_VIA_FLAG_TEST(colCtx->colHeader->polyList[curPolyId].flags_vIB, 4) &&
             ((actor != NULL && actor->category != ACTORCAT_PLAYER) ||
              (actor == NULL && xpFlags != COLPOLY_IGNORE_CAMERA)))) {
            if (node->next != SS_NULL) {
                node = &colCtx->polyNodes.tbl[node->next];
                continue;
            }
            break;
        }

        if (center->y + radius < vtxList[COLPOLY_VTX_INDEX(curPoly->flags_vIA)].y &&
            center->y + radius < vtxList[COLPOLY_VTX_INDEX(curPoly->flags_vIB)].y &&
            center->y + radius < vtxList[curPoly->vIC].y) {
            break;
        }

        if (CollisionPoly_SphVsPoly(curPoly, vtxList, center, radius)) {
            *outPoly = curPoly;
            return true;
        }
        if (node->next != SS_NULL) {
            node = &colCtx->polyNodes.tbl[node->next];
            { s32 pad; }

            continue;
        }
        break;
    }
    return false;
}

/**
 * Get first static poly intersecting sphere `center` `radius` within `lookup`
 * returns true if any poly intersects the sphere, else false
 * `outPoly` returns the first poly found that intersects
 */
s32 BgCheck_SphVsFirstStaticPoly(StaticLookup* lookup, u16 xpFlags, CollisionContext* colCtx, Vec3f* center, f32 radius,
                                 CollisionPoly** outPoly, u16 bciFlags, Actor* actor) {
    if (lookup->floor.head != SS_NULL && !(bciFlags & BGCHECK_IGNORE_FLOOR) &&
        BgCheck_SphVsFirstStaticPolyList(&colCtx->polyNodes.tbl[lookup->floor.head], xpFlags, colCtx, center, radius,
                                         outPoly, actor)) {
        return true;
    }

    if (lookup->wall.head != SS_NULL && !(bciFlags & BGCHECK_IGNORE_WALL) &&
        BgCheck_SphVsFirstStaticPolyList(&colCtx->polyNodes.tbl[lookup->wall.head], xpFlags, colCtx, center, radius,
                                         outPoly, actor)) {
        return true;
    }

    if (lookup->ceiling.head != SS_NULL && !(bciFlags & BGCHECK_IGNORE_CEILING) &&
        BgCheck_SphVsFirstStaticPolyList(&colCtx->polyNodes.tbl[lookup->ceiling.head], xpFlags, colCtx, center, radius,
                                         outPoly, actor)) {
        return true;
    }

    return false;
}

/**
 * Get StaticLookup from `pos`
 * Does not return NULL
 */
StaticLookup* BgCheck_GetNearestStaticLookup(CollisionContext* colCtx, StaticLookup* lookupTbl, Vec3f* pos) {
    Vec3i sector;
    s32 subdivAmountX;

    BgCheck_GetStaticLookupIndicesFromPos(colCtx, pos, &sector);
    subdivAmountX = colCtx->subdivAmount.x;
    return (sector.z * subdivAmountX) * colCtx->subdivAmount.y + lookupTbl + sector.x + sector.y * subdivAmountX;
}

/**
 * Get StaticLookup from `pos`
 * Returns NULL if just outside the mesh bounding box
 */
StaticLookup* BgCheck_GetStaticLookup(CollisionContext* colCtx, StaticLookup* lookupTbl, Vec3f* pos) {
    Vec3i sector;
    s32 subdivAmountX;

    if (!BgCheck_PosInStaticBoundingBox(colCtx, pos)) {
        return NULL;
    }
    BgCheck_GetStaticLookupIndicesFromPos(colCtx, pos, &sector);
    subdivAmountX = colCtx->subdivAmount.x;
    return (sector.z * subdivAmountX) * colCtx->subdivAmount.y + lookupTbl + sector.x + sector.y * subdivAmountX;
}

/**
 * Get StaticLookup subdivision indices from `pos`
 * `sector` returns the subdivision x,y,z indices containing or is nearest to `pos`
 */
void BgCheck_GetStaticLookupIndicesFromPos(CollisionContext* colCtx, Vec3f* pos, Vec3i* sector) {
    sector->x = (pos->x - colCtx->minBounds.x) * colCtx->subdivLengthInv.x;
    sector->y = (pos->y - colCtx->minBounds.y) * colCtx->subdivLengthInv.y;
    sector->z = (pos->z - colCtx->minBounds.z) * colCtx->subdivLengthInv.z;

    if (sector->x < 0) {
        sector->x = 0;
    } else if (sector->x >= colCtx->subdivAmount.x) {
        sector->x = colCtx->subdivAmount.x - 1;
    }

    if (sector->y < 0) {
        sector->y = 0;
    } else if (sector->y >= colCtx->subdivAmount.y) {
        sector->y = colCtx->subdivAmount.y - 1;
    }

    if (sector->z < 0) {
        sector->z = 0;
    } else if (sector->z >= colCtx->subdivAmount.z) {
        sector->z = colCtx->subdivAmount.z - 1;
    }
}

/**
 * Get negative bias subdivision indices
 * decrements indices if `pos` is within BGCHECK_SUBDIV_OVERLAP units of the negative subdivision boundary
 * `sx`, `sy`, `sz` returns the subdivision x, y, z indices
 */
void BgCheck_GetSubdivisionMinBounds(CollisionContext* colCtx, Vec3f* pos, s32* sx, s32* sy, s32* sz) {
    f32 dx = pos->x - colCtx->minBounds.x;
    f32 dy = pos->y - colCtx->minBounds.y;
    f32 dz = pos->z - colCtx->minBounds.z;

    *sx = dx * colCtx->subdivLengthInv.x;
    *sy = dy * colCtx->subdivLengthInv.y;
    *sz = dz * colCtx->subdivLengthInv.z;

    if (((s32)dx % (s32)colCtx->subdivLength.x < BGCHECK_SUBDIV_OVERLAP) && (*sx > 0)) {
        *sx -= 1;
    }

    if (((s32)dy % (s32)colCtx->subdivLength.y < BGCHECK_SUBDIV_OVERLAP) && (*sy > 0)) {
        *sy -= 1;
    }

    if (((s32)dz % (s32)colCtx->subdivLength.z < BGCHECK_SUBDIV_OVERLAP) && (*sz > 0)) {
        *sz -= 1;
    }
}

/**
 * Get positive bias subdivision indices
 * increments indicies if `pos` is within BGCHECK_SUBDIV_OVERLAP units of the postive subdivision boundary
 * `sx`, `sy`, `sz` returns the subdivision x, y, z indices
 */
void BgCheck_GetSubdivisionMaxBounds(CollisionContext* colCtx, Vec3f* pos, s32* sx, s32* sy, s32* sz) {
    f32 dx = pos->x - colCtx->minBounds.x;
    f32 dy = pos->y - colCtx->minBounds.y;
    f32 dz = pos->z - colCtx->minBounds.z;

    *sx = dx * colCtx->subdivLengthInv.x;
    *sy = dy * colCtx->subdivLengthInv.y;
    *sz = dz * colCtx->subdivLengthInv.z;

    if (((s32)colCtx->subdivLength.x - BGCHECK_SUBDIV_OVERLAP < (s32)dx % (s32)colCtx->subdivLength.x) &&
        (*sx < colCtx->subdivAmount.x - 1)) {
        *sx += 1;
    }

    if (((s32)colCtx->subdivLength.y - BGCHECK_SUBDIV_OVERLAP < (s32)dy % (s32)colCtx->subdivLength.y) &&
        (*sy < colCtx->subdivAmount.y - 1)) {
        *sy += 1;
    }

    if (((s32)colCtx->subdivLength.z - BGCHECK_SUBDIV_OVERLAP < (s32)dz % (s32)colCtx->subdivLength.z) &&
        (*sz < colCtx->subdivAmount.z - 1)) {
        *sz += 1;
    }
}

/**
 * Calculate the subdivision index bounding box for CollisionPoly `polyId`
 * `subdivMinX`, `subdivMinY`, `subdivMinZ` returns the minimum subdivision x, y, z indices
 * `subdivMaxX`, `subdivMaxY`, `subdivMaxZ` returns the maximum subdivision x, y, z indices
 */
void BgCheck_GetPolySubdivisionBounds(CollisionContext* colCtx, Vec3s* vtxList, CollisionPoly* polyList,
                                      s32* subdivMinX, s32* subdivMinY, s32* subdivMinZ, s32* subdivMaxX,
                                      s32* subdivMaxY, s32* subdivMaxZ, s16 polyId) {
    u16* vtxDataTemp;
    Vec3f minVtx;
    Vec3f maxVtx;

    f32 x;
    f32 y;
    f32 z;

    Vec3s* vtx;
    s16 vtxId = COLPOLY_VTX_INDEX(polyList[polyId].vtxData[0]);

    Math_Vec3s_ToVec3f(&maxVtx, &vtxList[vtxId]);
    Math_Vec3f_Copy(&minVtx, &maxVtx);

    for (vtxDataTemp = polyList[polyId].vtxData + 1; vtxDataTemp < polyList[polyId].vtxData + 3; vtxDataTemp++) {
        vtxId = COLPOLY_VTX_INDEX(*vtxDataTemp);
        vtx = &vtxList[vtxId];
        x = vtx->x;
        y = vtx->y;
        z = vtx->z;

        if (minVtx.x > x) {
            minVtx.x = x;
        } else if (maxVtx.x < x) {
            maxVtx.x = x;
        }

        if (minVtx.y > y) {
            minVtx.y = y;
        } else if (maxVtx.y < y) {
            maxVtx.y = y;
        }

        if (minVtx.z > z) {
            minVtx.z = z;
        } else if (maxVtx.z < z) {
            maxVtx.z = z;
        }
    }
    BgCheck_GetSubdivisionMinBounds(colCtx, &minVtx, subdivMinX, subdivMinY, subdivMinZ);
    BgCheck_GetSubdivisionMaxBounds(colCtx, &maxVtx, subdivMaxX, subdivMaxY, subdivMaxZ);
}

/**
 * Test if poly `polyList`[`polyId`] intersects cube `min` `max`
 * returns true if the poly intersects the cube, else false
 */
s32 BgCheck_PolyIntersectsSubdivision(Vec3f* min, Vec3f* max, CollisionPoly* polyList, Vec3s* vtxList, s16 polyId) {
    f32 intersect;
    Vec3f va2;
    Vec3f vb2;
    Vec3f vc2;
    CollisionPoly* poly;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 dist;
    Vec3f va;
    Vec3f vb;
    Vec3f vc;
    s32 flags[3];

    flags[0] = flags[1] = 0;
    poly = &polyList[polyId];

    BgCheck_Vec3sToVec3f(&vtxList[COLPOLY_VTX_INDEX(poly->flags_vIA)], &va);
    flags[0] = Math3D_PointRelativeToCubeFaces(&va, min, max);
    if (flags[0] == 0) {
        return true;
    }

    BgCheck_Vec3sToVec3f(&vtxList[COLPOLY_VTX_INDEX(poly->flags_vIB)], &vb);
    flags[1] = Math3D_PointRelativeToCubeFaces(&vb, min, max);
    if (flags[1] == 0) {
        return true;
    }

    BgCheck_Vec3sToVec3f(&vtxList[poly->vIC], &vc);
    flags[2] = Math3D_PointRelativeToCubeFaces(&vc, min, max);
    if (flags[2] == 0) {
        return true;
    }

    if (flags[0] & flags[1] & flags[2]) {
        return false;
    }

    flags[0] |= Math3D_PointRelativeToCubeEdges(&va, min, max) << 8;
    flags[1] |= Math3D_PointRelativeToCubeEdges(&vb, min, max) << 8;
    flags[2] |= Math3D_PointRelativeToCubeEdges(&vc, min, max) << 8;
    if (flags[0] & flags[1] & flags[2]) {
        return false;
    }

    flags[0] |= Math3D_PointRelativeToCubeVertices(&va, min, max) << 0x18;
    flags[1] |= Math3D_PointRelativeToCubeVertices(&vb, min, max) << 0x18;
    flags[2] |= Math3D_PointRelativeToCubeVertices(&vc, min, max) << 0x18;
    if (flags[0] & flags[1] & flags[2]) {
        return false;
    }

    CollisionPoly_GetNormalF(poly, &nx, &ny, &nz);
    dist = poly->dist;

    if (Math3D_TriChkLineSegParaYIntersect(&va, &vb, &vc, nx, ny, nz, dist, min->z, min->x, &intersect, min->y,
                                           max->y) ||
        Math3D_TriChkLineSegParaYIntersect(&va, &vb, &vc, nx, ny, nz, dist, max->z, min->x, &intersect, min->y,
                                           max->y) ||
        Math3D_TriChkLineSegParaYIntersect(&va, &vb, &vc, nx, ny, nz, dist, min->z, max->x, &intersect, min->y,
                                           max->y) ||
        Math3D_TriChkLineSegParaYIntersect(&va, &vb, &vc, nx, ny, nz, dist, max->z, max->x, &intersect, min->y,
                                           max->y)) {
        return true;
    }
    if (Math3D_TriChkLineSegParaZIntersect(&va, &vb, &vc, nx, ny, nz, dist, min->x, min->y, &intersect, min->z,
                                           max->z) ||
        Math3D_TriChkLineSegParaZIntersect(&va, &vb, &vc, nx, ny, nz, dist, min->x, max->y, &intersect, min->z,
                                           max->z) ||
        Math3D_TriChkLineSegParaZIntersect(&va, &vb, &vc, nx, ny, nz, dist, max->x, min->y, &intersect, min->z,
                                           max->z) ||
        Math3D_TriChkLineSegParaZIntersect(&va, &vb, &vc, nx, ny, nz, dist, max->x, max->y, &intersect, min->z,
                                           max->z)) {
        return true;
    }
    if (Math3D_TriChkLineSegParaXIntersect(&va, &vb, &vc, nx, ny, nz, dist, min->y, min->z, &intersect, min->x,
                                           max->x) ||
        Math3D_TriChkLineSegParaXIntersect(&va, &vb, &vc, nx, ny, nz, dist, min->y, max->z, &intersect, min->x,
                                           max->x) ||
        Math3D_TriChkLineSegParaXIntersect(&va, &vb, &vc, nx, ny, nz, dist, max->y, min->z, &intersect, min->x,
                                           max->x) ||
        Math3D_TriChkLineSegParaXIntersect(&va, &vb, &vc, nx, ny, nz, dist, max->y, max->z, &intersect, min->x,
                                           max->x)) {
        return true;
    }

    BgCheck_Vec3sToVec3f(&vtxList[COLPOLY_VTX_INDEX(poly->flags_vIA)], &va2);
    BgCheck_Vec3sToVec3f(&vtxList[COLPOLY_VTX_INDEX(poly->flags_vIB)], &vb2);
    BgCheck_Vec3sToVec3f(&vtxList[poly->vIC], &vc2);
    if (Math3D_LineVsCube(min, max, &va2, &vb2) || Math3D_LineVsCube(min, max, &vb2, &vc2) ||
        Math3D_LineVsCube(min, max, &vc2, &va2)) {
        return true;
    }
    return false;
}

/**
 * Initialize StaticLookup Table
 * returns size of table, in bytes
 */
u32 BgCheck_InitStaticLookup(CollisionContext* colCtx, GlobalContext* globalCtx, StaticLookup* lookupTbl) {
    Vec3s* vtxList;
    CollisionPoly* polyList;
    s32 polyMax;
    s32 polyIdx;
    s32 sx;
    s32 sy;
    s32 sz;
    // subdivMin indices
    s32 sxMin;
    s32 syMin;
    s32 szMin;
    // subdivMax indices
    s32 sxMax;
    s32 syMax;
    s32 szMax;
    // subdiv min/max bounds for adding a poly
    Vec3f curSubdivMin;
    Vec3f curSubdivMax;
    CollisionHeader* colHeader = colCtx->colHeader;
    s32 pad[2];
    StaticLookup* iLookup;
    StaticLookup* jLookup;
    StaticLookup* lookup;
    s32 sp98;
    f32 subdivLengthX;
    f32 subdivLengthY;
    f32 subdivLengthZ;

    for (iLookup = lookupTbl;
         iLookup < (colCtx->subdivAmount.x * colCtx->subdivAmount.y * colCtx->subdivAmount.z + lookupTbl); iLookup++) {
        iLookup->floor.head = SS_NULL;
        iLookup->wall.head = SS_NULL;
        iLookup->ceiling.head = SS_NULL;
    }

    polyMax = colHeader->numPolygons;
    vtxList = colHeader->vtxList;
    polyList = colHeader->polyList;
    sp98 = colCtx->subdivAmount.x * colCtx->subdivAmount.y;
    subdivLengthX = colCtx->subdivLength.x + (2 * BGCHECK_SUBDIV_OVERLAP);
    subdivLengthY = colCtx->subdivLength.y + (2 * BGCHECK_SUBDIV_OVERLAP);
    subdivLengthZ = colCtx->subdivLength.z + (2 * BGCHECK_SUBDIV_OVERLAP);

    for (polyIdx = 0; polyIdx < polyMax; polyIdx++) {
        BgCheck_GetPolySubdivisionBounds(colCtx, vtxList, polyList, &sxMin, &syMin, &szMin, &sxMax, &syMax, &szMax,
                                         polyIdx);
        iLookup = szMin * sp98 + lookupTbl;
        curSubdivMin.z = (colCtx->subdivLength.z * szMin + colCtx->minBounds.z) - BGCHECK_SUBDIV_OVERLAP;
        curSubdivMax.z = curSubdivMin.z + subdivLengthZ;

        for (sz = szMin; sz < szMax + 1; sz++) {
            jLookup = (colCtx->subdivAmount.x * syMin) + iLookup;
            curSubdivMin.y = (colCtx->subdivLength.y * syMin + colCtx->minBounds.y) - BGCHECK_SUBDIV_OVERLAP;
            curSubdivMax.y = curSubdivMin.y + subdivLengthY;

            for (sy = syMin; sy < syMax + 1; sy++) {
                lookup = sxMin + jLookup;
                curSubdivMin.x = (colCtx->subdivLength.x * sxMin + colCtx->minBounds.x) - BGCHECK_SUBDIV_OVERLAP;
                curSubdivMax.x = curSubdivMin.x + subdivLengthX;

                for (sx = sxMin; sx < sxMax + 1; sx++) {
                    if (BgCheck_PolyIntersectsSubdivision(&curSubdivMin, &curSubdivMax, polyList, vtxList, polyIdx)) {
                        StaticLookup_AddPoly(lookup, colCtx, polyList, vtxList, polyIdx);
                    }
                    curSubdivMin.x += colCtx->subdivLength.x;
                    curSubdivMax.x += colCtx->subdivLength.x;
                    lookup++;
                }
                curSubdivMin.y += colCtx->subdivLength.y;
                curSubdivMax.y += colCtx->subdivLength.y;
                jLookup += colCtx->subdivAmount.x;
            }
            curSubdivMin.z += colCtx->subdivLength.z;
            curSubdivMax.z += colCtx->subdivLength.z;
            iLookup += sp98;
        }
    }
    return colCtx->polyNodes.count * sizeof(SSNode);
}

/**
 * Returns whether the current scene should reserve less memory for it's collision lookup
 */
s32 BgCheck_IsSmallMemScene(GlobalContext* globalCtx) {
    s16* i;

    for (i = sSmallMemScenes; i < sSmallMemScenes + ARRAY_COUNT(sSmallMemScenes); i++) {
        if (globalCtx->sceneNum == *i) {
            return true;
        }
    }
    return false;
}

/**
 * Get custom scene memSize
 */
s32 BgCheck_TryGetCustomMemsize(s32 sceneId, u32* memSize) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sSceneMemList); i++) {
        if (sceneId == sSceneMemList[i].sceneId) {
            *memSize = sSceneMemList[i].memSize;
            return true;
        }
    }
    return false;
}

/**
 * Compute subdivLength for scene mesh lookup, for a single dimension
 */
void BgCheck_SetSubdivisionDimension(f32 min, s32 subdivAmount, f32* max, f32* subdivLength, f32* subdivLengthInv) {
    f32 length = (*max - min);

    *subdivLength = (s32)(length / subdivAmount) + 1;
    *subdivLength = CLAMP_MIN(*subdivLength, BGCHECK_SUBDIV_MIN);
    *subdivLengthInv = 1.0f / *subdivLength;

    *max = *subdivLength * subdivAmount + min;
}

s32 BgCheck_GetSpecialSceneMaxObjects(GlobalContext* globalCtx, s32* maxNodes, s32* maxPolygons, s32* maxVertices) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sCustomDynapolyMem); i++) {
        if (globalCtx->sceneNum == sCustomDynapolyMem[i].sceneId) {
            *maxNodes = sCustomDynapolyMem[i].maxNodes;
            *maxPolygons = sCustomDynapolyMem[i].maxPolygons;
            *maxVertices = sCustomDynapolyMem[i].maxVertices;
            return true;
        }
    }
    return false;
}

/**
 * Allocate CollisionContext
 */
void BgCheck_Allocate(CollisionContext* colCtx, GlobalContext* globalCtx, CollisionHeader* colHeader) {
    u32 tblMax;
    u32 memSize;
    u32 lookupTblMemSize;
    SSNodeList* nodeList;
    s32 customNodeListMax;

    customNodeListMax = -1;
    colCtx->colHeader = colHeader;
    colCtx->flags = 0;

    if (BgCheck_IsSmallMemScene(globalCtx)) {
        colCtx->memSize = 0xF000;
        colCtx->dyna.polyNodesMax = 1000;
        colCtx->dyna.polyListMax = 512;
        colCtx->dyna.vtxListMax = 512;
        colCtx->subdivAmount.x = 16;
        colCtx->subdivAmount.y = 4;
        colCtx->subdivAmount.z = 16;
    } else {
        u32 customMemSize;
        s32 useCustomSubdivisions;
        s32 i;

        if (BgCheck_TryGetCustomMemsize(globalCtx->sceneNum, &customMemSize)) {
            colCtx->memSize = customMemSize;
        } else {
            colCtx->memSize = 0x23000;
        }
        colCtx->dyna.polyNodesMax = 1000;
        colCtx->dyna.polyListMax = 544;
        colCtx->dyna.vtxListMax = 512;
        BgCheck_GetSpecialSceneMaxObjects(globalCtx, &colCtx->dyna.polyNodesMax, &colCtx->dyna.polyListMax,
                                          &colCtx->dyna.vtxListMax);
        useCustomSubdivisions = false;

        for (i = 0; i < ARRAY_COUNT(sSceneSubdivisionList); i++) {
            if (globalCtx->sceneNum == sSceneSubdivisionList[i].sceneId) {
                colCtx->subdivAmount.x = sSceneSubdivisionList[i].subdivAmount.x;
                colCtx->subdivAmount.y = sSceneSubdivisionList[i].subdivAmount.y;
                colCtx->subdivAmount.z = sSceneSubdivisionList[i].subdivAmount.z;
                useCustomSubdivisions = true;
                customNodeListMax = sSceneSubdivisionList[i].nodeListMax;
            }
        }
        if (useCustomSubdivisions == false) {
            colCtx->subdivAmount.x = 16;
            colCtx->subdivAmount.y = 4;
            colCtx->subdivAmount.z = 16;
        }
    }
    colCtx->lookupTbl = THA_AllocEndAlign(
        &globalCtx->state.heap,
        colCtx->subdivAmount.x * sizeof(StaticLookup) * colCtx->subdivAmount.y * colCtx->subdivAmount.z, ~1);
    if (colCtx->lookupTbl == NULL) {
        Fault_AddHungupAndCrash("../z_bgcheck.c", 3955);
    }
    colCtx->minBounds.x = colCtx->colHeader->minBounds.x;
    colCtx->minBounds.y = colCtx->colHeader->minBounds.y;
    colCtx->minBounds.z = colCtx->colHeader->minBounds.z;
    colCtx->maxBounds.x = colCtx->colHeader->maxBounds.x;
    colCtx->maxBounds.y = colCtx->colHeader->maxBounds.y;
    colCtx->maxBounds.z = colCtx->colHeader->maxBounds.z;
    BgCheck_SetSubdivisionDimension(colCtx->minBounds.x, colCtx->subdivAmount.x, &colCtx->maxBounds.x,
                                    &colCtx->subdivLength.x, &colCtx->subdivLengthInv.x);
    BgCheck_SetSubdivisionDimension(colCtx->minBounds.y, colCtx->subdivAmount.y, &colCtx->maxBounds.y,
                                    &colCtx->subdivLength.y, &colCtx->subdivLengthInv.y);
    BgCheck_SetSubdivisionDimension(colCtx->minBounds.z, colCtx->subdivAmount.z, &colCtx->maxBounds.z,
                                    &colCtx->subdivLength.z, &colCtx->subdivLengthInv.z);
    memSize = colCtx->subdivAmount.x * sizeof(StaticLookup) * colCtx->subdivAmount.y * colCtx->subdivAmount.z +
              colCtx->colHeader->numPolygons * sizeof(u8) + colCtx->dyna.polyNodesMax * sizeof(SSNode) +
              colCtx->dyna.polyListMax * sizeof(CollisionPoly) + colCtx->dyna.vtxListMax * sizeof(Vec3s) +
              sizeof(CollisionContext);
    if (customNodeListMax > 0) {
        // tblMax is set without checking if customNodeListMax will result in a memory overflow
        // this is a non-issue as long as sSceneSubdivisionList.nodeListMax is -1
        tblMax = customNodeListMax;
    } else {
        if (colCtx->memSize < memSize) {
            Fault_AddHungupAndCrash("../z_bgcheck.c", 4011);
        }
        tblMax = (colCtx->memSize - memSize) / sizeof(SSNode);
    }

    SSNodeList_Init(&colCtx->polyNodes);
    SSNodeList_Alloc(globalCtx, &colCtx->polyNodes, tblMax, colCtx->colHeader->numPolygons);

    lookupTblMemSize = BgCheck_InitStaticLookup(colCtx, globalCtx, colCtx->lookupTbl);

    DynaPoly_Init(globalCtx, &colCtx->dyna);
    DynaPoly_Alloc(globalCtx, &colCtx->dyna);
}

/**
 * Enables CollisionContext wide flags
 * Only used to reverse water flow in Great Bay Temple
 */
void BgCheck_SetContextFlags(CollisionContext* colCtx, u32 flags) {
    colCtx->flags |= flags;
}

/**
 * Disables CollisionContext wide flags
 * Only used to revert the reverse water flow in Great Bay Temple
 */
void BgCheck_UnsetContextFlags(CollisionContext* colCtx, u32 flags) {
    colCtx->flags &= ~flags;
}

/**
 * original name: T_BGCheck_getBGDataInfo
 */
CollisionHeader* BgCheck_GetCollisionHeader(CollisionContext* colCtx, s32 bgId) {
    if (bgId == BGCHECK_SCENE) {
        return colCtx->colHeader;
    }
    if (bgId < 0 || bgId > BG_ACTOR_MAX) {
        return NULL;
    }
    if (!(colCtx->dyna.bgActorFlags[bgId] & 1)) {
        return NULL;
    }
    return colCtx->dyna.bgActors[bgId].colHeader;
}

/**
 * Test if pos is near collision boundaries
 */
s32 BgCheck_PosInStaticBoundingBox(CollisionContext* colCtx, Vec3f* pos) {
    if (pos->x < (colCtx->minBounds.x - BGCHECK_SUBDIV_OVERLAP) ||
        (colCtx->maxBounds.x + BGCHECK_SUBDIV_OVERLAP) < pos->x ||
        pos->y < (colCtx->minBounds.y - BGCHECK_SUBDIV_OVERLAP) ||
        (colCtx->maxBounds.y + BGCHECK_SUBDIV_OVERLAP) < pos->y ||
        pos->z < (colCtx->minBounds.z - BGCHECK_SUBDIV_OVERLAP) ||
        (colCtx->maxBounds.z + BGCHECK_SUBDIV_OVERLAP) < pos->z) {
        return false;
    }
    return true;
}

/**
 * Raycast Toward Floor
 * returns the yIntersect of the nearest poly found directly below `pos`, or BGCHECK_Y_MIN if no floor detected
 * returns the poly found in `outPoly`, and the bgId of the entity in `outBgId`
 */
f32 BgCheck_RaycastFloorImpl(GlobalContext* globalCtx, CollisionContext* colCtx, u16 xpFlags, CollisionPoly** outPoly,
                             s32* outBgId, Vec3f* pos, Actor* actor, u32 arg7, f32 checkDist, s32 arg9) {
    f32 yIntersectDyna;
    f32 yIntersect;
    StaticLookup* lookupTbl;
    Vec3f checkPos;
    StaticLookup* lookup;
    DynaRaycast dynaRaycast;

    *outBgId = BGCHECK_SCENE;
    *outPoly = NULL;
    lookupTbl = colCtx->lookupTbl;
    yIntersect = BGCHECK_Y_MIN;
    checkPos = *pos;

    while (true) {
        if (checkPos.y < colCtx->minBounds.y) {
            break;
        }
        lookup = BgCheck_GetStaticLookup(colCtx, lookupTbl, &checkPos);
        if (lookup == NULL) {
            checkPos.y -= colCtx->subdivLength.y;
            continue;
        }
        yIntersect =
            BgCheck_RaycastFloorStatic(lookup, colCtx, xpFlags, outPoly, pos, arg7, checkDist, BGCHECK_Y_MIN, actor);
        if (yIntersect > BGCHECK_Y_MIN) {
            break;
        }
        checkPos.y -= colCtx->subdivLength.y;
    }
    if (!(arg9 & 1)) {
        dynaRaycast.globalCtx = globalCtx;
        dynaRaycast.colCtx = colCtx;
        dynaRaycast.xpFlags = xpFlags;
        dynaRaycast.resultPoly = outPoly;
        dynaRaycast.yIntersect = yIntersect;
        dynaRaycast.pos = pos;
        dynaRaycast.bgId = outBgId;
        dynaRaycast.unk1C = 50;
        dynaRaycast.actor = actor;
        dynaRaycast.unk_24 = arg7;
        dynaRaycast.checkDist = checkDist;

        yIntersectDyna = BgCheck_RaycastFloorDyna(&dynaRaycast);
        if (yIntersect < yIntersectDyna) {
            yIntersect = yIntersectDyna;
        }
    }

    if (yIntersect != BGCHECK_Y_MIN && func_800C9B68(colCtx, *outPoly, *outBgId)) {
        yIntersect -= 1.0f;
    }
    return yIntersect;
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_CameraRaycastFloor1(CollisionContext* colCtx, CollisionPoly** outPoly, Vec3f* pos) {
    s32 bgId;

    return BgCheck_RaycastFloorImpl(NULL, colCtx, COLPOLY_IGNORE_CAMERA, outPoly, &bgId, pos, NULL, 0x1C, 1.0f, 0);
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_EntityRaycastFloor1(CollisionContext* colCtx, CollisionPoly** outPoly, Vec3f* pos) {
    s32 bgId;

    return BgCheck_RaycastFloorImpl(NULL, colCtx, COLPOLY_IGNORE_ENTITY, outPoly, &bgId, pos, NULL, 0x1C, 1.0f, 0);
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_EntityRaycastFloor2(GlobalContext* globalCtx, CollisionContext* colCtx, CollisionPoly** outPoly,
                                Vec3f* pos) {
    s32 bgId;

    return BgCheck_RaycastFloorImpl(globalCtx, colCtx, COLPOLY_IGNORE_ENTITY, outPoly, &bgId, pos, NULL, 0x1C, 1.0f, 0);
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_EntityRaycastFloor2_1(GlobalContext* globalCtx, CollisionContext* colCtx, CollisionPoly** outPoly,
                                  Vec3f* pos) {
    s32 bgId;

    return BgCheck_RaycastFloorImpl(globalCtx, colCtx, COLPOLY_IGNORE_ENTITY, outPoly, &bgId, pos, NULL, 0x1C, 1.0f, 1);
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_EntityRaycastFloor3(CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Vec3f* pos) {
    return BgCheck_RaycastFloorImpl(NULL, colCtx, COLPOLY_IGNORE_ENTITY, outPoly, bgId, pos, NULL, 0x1C, 1.0f, 0);
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_EntityRaycastFloor5(CollisionContext* colCtx, CollisionPoly** outPoly, s32* outBgId, Actor* actor,
                                Vec3f* pos) {
    return BgCheck_RaycastFloorImpl(NULL, colCtx, COLPOLY_IGNORE_ENTITY, outPoly, outBgId, pos, actor, 0x1C, 1.0f, 0);
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_EntityRaycastFloor5_2(GlobalContext* globalCtx, CollisionContext* colCtx, CollisionPoly** outPoly,
                                  s32* bgId, Actor* actor, Vec3f* pos) {
    return BgCheck_RaycastFloorImpl(globalCtx, colCtx, COLPOLY_IGNORE_ENTITY, outPoly, bgId, pos, actor, 0x1C, 1.0f, 0);
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_EntityRaycastFloor5_3(GlobalContext* globalCtx, CollisionContext* colCtx, CollisionPoly** outPoly,
                                  s32* bgId, Actor* actor, Vec3f* pos) {
    return BgCheck_RaycastFloorImpl(globalCtx, colCtx, COLPOLY_IGNORE_ENTITY, outPoly, bgId, pos, actor, 0x3C, 1.0f, 0);
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_EntityRaycastFloor6(CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Actor* actor, Vec3f* pos,
                                f32 checkDist) {
    return BgCheck_RaycastFloorImpl(NULL, colCtx, COLPOLY_IGNORE_ENTITY, outPoly, bgId, pos, actor, 0x1C, checkDist, 0);
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_EntityRaycastFloor7(CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Actor* actor,
                                Vec3f* pos) {
    return BgCheck_RaycastFloorImpl(NULL, colCtx, COLPOLY_IGNORE_ENTITY, outPoly, bgId, pos, actor, 0x06, 1.0f, 0);
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_AnyRaycastFloor1(CollisionContext* colCtx, CollisionPoly* outPoly, Vec3f* pos) {
    CollisionPoly* tempPoly;
    f32 result;
    s32 bgId;

    result = BgCheck_RaycastFloorImpl(NULL, colCtx, COLPOLY_IGNORE_NONE, &tempPoly, &bgId, pos, NULL, 0x1C, 1.0f, 0);

    if (tempPoly != NULL) {
        *outPoly = *tempPoly;
    }
    return result;
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_AnyRaycastFloor2(CollisionContext* colCtx, CollisionPoly* outPoly, s32* bgId, Vec3f* pos) {
    CollisionPoly* tempPoly;
    f32 result = BgCheck_RaycastFloorImpl(NULL, colCtx, COLPOLY_IGNORE_NONE, &tempPoly, bgId, pos, NULL, 0x1C, 1.0f, 0);

    if (tempPoly != NULL) {
        *outPoly = *tempPoly;
    }
    return result;
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_CameraRaycastFloor2(CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Vec3f* pos) {
    return BgCheck_RaycastFloorImpl(NULL, colCtx, COLPOLY_IGNORE_CAMERA, outPoly, bgId, pos, NULL, 0x06, 1.0f, 0);
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_EntityRaycastFloor8(CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Actor* actor,
                                Vec3f* pos) {
    return BgCheck_RaycastFloorImpl(NULL, colCtx, COLPOLY_IGNORE_ENTITY, outPoly, bgId, pos, actor, 0x02, 1.0f, 0);
}

/**
 * Public raycast toward floor
 * returns yIntersect of the poly found, or BGCHECK_Y_MIN if no poly detected
 */
f32 BgCheck_EntityRaycastFloor9(CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Vec3f* pos) {
    return BgCheck_RaycastFloorImpl(NULL, colCtx, COLPOLY_IGNORE_ENTITY, outPoly, bgId, pos, NULL, 0x06, 1.0f, 0);
}

#ifdef NON_MATCHING
/**
 * Tests if moving from `posPrev` to `posNext` will collide with a "wall"
 * `radius` is used to form a sphere for collision detection purposes
 * `checkHeight` is the positive height above posNext to perform certain checks
 * returns true if a collision is detected, else false
 * `outPoly` returns the closest poly detected, while `outBgId` returns the poly owner
 */
s32 BgCheck_CheckWallImpl(CollisionContext* colCtx, u16 xpFlags, Vec3f* posResult, Vec3f* posNext, Vec3f* posPrev,
                          f32 radius, CollisionPoly** outPoly, s32* outBgId, Actor* actor, f32 checkHeight, u8 argA) {
    StaticLookup* lookupTbl;
    f32 temp_f0;
    s32 result;
    CollisionPoly* poly;
    // change between posPrev to posNext
    f32 dx;
    f32 dy;
    f32 dz;
    Vec3f sphCenter;
    s32 dynaPolyCollision;
    Vec3f posIntersect;
    s32 bgId;
    f32 temp_f0_2;
    f32 f32temp;
    f32 nx2, nz2;
    Vec3f checkLineNext;
    Vec3f checkLinePrev;
    f32 n2XZDist;
    f32 n3XZDist;
    f32 nx3, nz3;
    s32 bccFlags;
    Vec3f posIntersect2;
    s32 bgId2;
    // unit normal of polygon
    f32 nx;
    f32 ny;
    f32 nz;

    result = false;
    *outBgId = BGCHECK_SCENE;
    *outPoly = NULL;
    lookupTbl = colCtx->lookupTbl;
    *posResult = *posNext;
    dx = posNext->x - posPrev->x;
    dy = posNext->y - posPrev->y;
    dz = posNext->z - posPrev->z;

    // if there's movement on the xz plane, and argA flag is 0,
    if ((dx != 0.0f || dz != 0.0f) && (argA & 1) == 0) {
        if ((checkHeight + dy) < 5.0f) {
            //! @bug checkHeight is not applied to posPrev/posNext
            result = BgCheck_CheckLineImpl(colCtx, xpFlags, COLPOLY_IGNORE_NONE, posPrev, posNext, &posIntersect, &poly,
                                           &bgId, actor, 1.0f, BGCHECK_CHECK_ALL & ~BGCHECK_CHECK_CEILING);
            if (result) {
                ny = COLPOLY_GET_NORMAL(poly->normal.y);
                // if poly is floor, push result underneath the floor
                if (ny > 0.5f) {
                    posResult->x = posIntersect.x;
                    if (checkHeight > 1.0f) {
                        posResult->y = posIntersect.y - 1.0f;
                    } else {
                        posResult->y = posIntersect.y - checkHeight;
                    }
                    posResult->z = posIntersect.z;
                    if (actor != NULL) {
                        actor->bgCheckFlags |= 0x1000;
                    }
                }
                // poly is wall
                else {
                    nx = COLPOLY_GET_NORMAL(poly->normal.x);
                    nz = COLPOLY_GET_NORMAL(poly->normal.z);
                    posResult->x = radius * nx + posIntersect.x;
                    posResult->y = radius * ny + posIntersect.y;
                    posResult->z = radius * nz + posIntersect.z;
                }
                *outPoly = poly;
                *outBgId = bgId;
            }
        } else {
            // if the radius is less than the distance travelled on the xz plane, also test for floor collisions
            bccFlags = SQ(radius) < (SQ(dx) + SQ(dz))
                           ? (BGCHECK_CHECK_ALL & ~BGCHECK_CHECK_CEILING)
                           : (BGCHECK_CHECK_ALL & ~BGCHECK_CHECK_FLOOR & ~BGCHECK_CHECK_CEILING);

            // perform a straight line test to see if a line at posNext.y + checkHeight from posPrev.xz to posNext.xz
            // passes through any wall and possibly floor polys
            checkLineNext = *posNext;
            checkLineNext.y += checkHeight;
            checkLinePrev = *posPrev;
            checkLinePrev.y = checkLineNext.y;
            result = BgCheck_CheckLineImpl(colCtx, xpFlags, COLPOLY_IGNORE_NONE, &checkLinePrev, &checkLineNext,
                                           &posIntersect, &poly, &bgId, actor, 1.0f, bccFlags);

            if (result) {
                nx2 = COLPOLY_GET_NORMAL(poly->normal.x);
                nz2 = COLPOLY_GET_NORMAL(poly->normal.z);
                n2XZDist = sqrtf(SQ(nx2) + SQ(nz2));

                // if poly is not a "flat" floor or "flat" ceiling
                if (!IS_ZERO(n2XZDist)) {
                    // normalize nx,nz and multiply each by the radius to go back to the other side of the wall
                    f32temp = 1.0f / n2XZDist;
                    temp_f0 = radius * f32temp;
                    posResult->x = temp_f0 * nx2 + posIntersect.x;
                    posResult->z = temp_f0 * nz2 + posIntersect.z;
                    *outPoly = poly;
                    *outBgId = bgId;
                    result = true;
                    if (COLPOLY_GET_NORMAL(poly->normal.y) > 0.5f) {
                        if (actor != NULL) {
                            actor->bgCheckFlags |= 0x1000;
                        }
                    }
                } else {
                    result = false;
                }
            }
        }
    }

    sphCenter = *posResult;
    dynaPolyCollision = false;
    sphCenter.y += checkHeight;
    // test if sphere (sphCenter, radius) collides with a dynamic wall, displacing the x/z coordinates
    if (BgCheck_SphVsDynaWall(colCtx, xpFlags, &posResult->x, &posResult->z, &sphCenter, radius, outPoly, outBgId,
                              actor)) {
        result = true;
        dynaPolyCollision = true;
        sphCenter = *posResult;
        sphCenter.y += checkHeight;
    }
    // test if sphere (sphCenter, radius) collides with a static wall, displacing the x/z coordinates
    if (BgCheck_PosInStaticBoundingBox(colCtx, posNext) &&
        // possible bug? if the sphere's radius is smaller than the distance to a subdivision boundary, some static
        // polys will be missed
        BgCheck_SphVsStaticWall(BgCheck_GetNearestStaticLookup(colCtx, lookupTbl, posResult), colCtx, xpFlags,
                                &posResult->x, &posResult->z, &sphCenter, radius, outPoly, actor)) {
        *outBgId = BGCHECK_SCENE;
        result = true;
    }
    // if a collision with a dyna poly was detected
    if (dynaPolyCollision || *outBgId != BGCHECK_SCENE) {
        if (BgCheck_CheckLineImpl(colCtx, xpFlags, COLPOLY_IGNORE_NONE, posPrev, posResult, &posIntersect2, &poly,
                                  &bgId2, actor, 1.0f, BGCHECK_CHECK_ONE_FACE | BGCHECK_CHECK_WALL)) {
            nx3 = COLPOLY_GET_NORMAL(poly->normal.x);
            nz3 = COLPOLY_GET_NORMAL(poly->normal.z);
            n3XZDist = sqrtf(SQ(nx3) + SQ(nz3));

            // if poly is not a "flat" floor or "flat" ceiling
            if (!IS_ZERO(n3XZDist)) {
                // normalize nx,nz and multiply each by the radius to go back to the other side of the wall
                f32temp = 1.0f / n3XZDist;
                temp_f0_2 = radius * f32temp;
                posResult->x = temp_f0_2 * nx3 + posIntersect2.x;
                posResult->z = temp_f0_2 * nz3 + posIntersect2.z;
                *outPoly = poly;
                *outBgId = bgId2;
                result = true;
            }
        }
    }
    return result;
}
#else
s32 BgCheck_CheckWallImpl(CollisionContext* colCtx, u16 xpFlags, Vec3f* posResult, Vec3f* posNext, Vec3f* posPrev,
                          f32 radius, CollisionPoly** outPoly, s32* outBgId, Actor* actor, f32 checkHeight, u8 argA);
#pragma GLOBAL_ASM("asm/non_matchings/code/z_bgcheck/BgCheck_CheckWallImpl.s")
#endif

/**
 * Public. Tests if moving from `posPrev` to `posNext` will collide with a "wall"
 * `radius` is used to form a sphere for collision detection purposes
 * `checkHeight` is the positive height above posNext to perform certain checks
 * returns true if a collision is detected, else false
 * `outPoly` returns the closest poly detected
 */
s32 BgCheck_EntitySphVsWall1(CollisionContext* colCtx, Vec3f* posResult, Vec3f* posNext, Vec3f* posPrev, f32 radius,
                             CollisionPoly** outPoly, f32 checkHeight) {
    s32 bgId;

    return BgCheck_CheckWallImpl(colCtx, COLPOLY_IGNORE_ENTITY, posResult, posNext, posPrev, radius, outPoly, &bgId,
                                 NULL, checkHeight, 0);
}

/**
 * Public. Tests if moving from `posPrev` to `posNext` will collide with a "wall"
 * `radius` is used to form a sphere for collision detection purposes
 * `checkHeight` is the positive height above posNext to perform certain checks
 * returns true if a collision is detected, else false
 * `outPoly` returns the closest poly detected, while `outBgId` returns the poly owner
 */
s32 BgCheck_EntitySphVsWall2(CollisionContext* colCtx, Vec3f* posResult, Vec3f* posNext, Vec3f* posPrev, f32 radius,
                             CollisionPoly** outPoly, s32* outBgId, f32 checkHeight) {
    return BgCheck_CheckWallImpl(colCtx, COLPOLY_IGNORE_ENTITY, posResult, posNext, posPrev, radius, outPoly, outBgId,
                                 NULL, checkHeight, 0);
}

/**
 * Public. Tests if *moving from `posPrev` to `posNext` will collide with a "wall"
 * `radius` is used to form a sphere for collision detection purposes
 * `checkHeight` is the positive height above posNext to perform certain checks
 * `actor` is the actor performing the check, allowing it to be skipped
 * returns true if a collision is detected, else false
 * `outPoly` returns the closest poly detected, while `outBgId` returns the poly owner
 */
s32 BgCheck_EntitySphVsWall3(CollisionContext* colCtx, Vec3f* posResult, Vec3f* posNext, Vec3f* posPrev, f32 radius,
                             CollisionPoly** outPoly, s32* outBgId, Actor* actor, f32 checkHeight) {
    return BgCheck_CheckWallImpl(colCtx, COLPOLY_IGNORE_ENTITY, posResult, posNext, posPrev, radius, outPoly, outBgId,
                                 actor, checkHeight, 0);
}

/**
 * Public. Tests if moving from `posPrev` to `posNext` will collide with a "wall"
 * Skips a check that occurs only when moving on the xz plane
 * `radius` is used to form a sphere for collision detection purposes
 * `checkHeight` is the positive height above posNext to perform certain checks
 * `actor` is the actor performing the check, allowing it to be skipped
 * returns true if a collision is detected, else false
 * `outPoly` returns the closest poly detected, while `outBgId` returns the poly owner
 */
s32 BgCheck_EntitySphVsWall4(CollisionContext* colCtx, Vec3f* posResult, Vec3f* posNext, Vec3f* posPrev, f32 radius,
                             CollisionPoly** outPoly, s32* outBgId, Actor* actor, f32 checkHeight) {
    return BgCheck_CheckWallImpl(colCtx, COLPOLY_IGNORE_ENTITY, posResult, posNext, posPrev, radius, outPoly, outBgId,
                                 actor, checkHeight, 1);
}

/**
 * Tests for collision with a ceiling poly
 * `checkHeight` should be a positive value
 * returns true if a collision occurs, else false
 * `outPoly` returns the poly collided with, while `outBgId` returns the owner of the poly
 * `outY` returns the y coordinate of pos needed to not collide with `outPoly`
 */
s32 BgCheck_CheckCeilingImpl(CollisionContext* colCtx, u16 xpFlags, f32* outY, Vec3f* pos, f32 checkHeight,
                             CollisionPoly** outPoly, s32* outBgId, Actor* actor) {
    StaticLookup* lookupTbl;
    StaticLookup* lookup;
    s32 result;
    Vec3f posTemp;
    f32 tempY;

    *outBgId = BGCHECK_SCENE;
    *outY = pos->y;
    lookupTbl = colCtx->lookupTbl;
    if (!BgCheck_PosInStaticBoundingBox(colCtx, pos)) {
        return false;
    }

    lookup = BgCheck_GetNearestStaticLookup(colCtx, lookupTbl, pos);
    result = BgCheck_CheckStaticCeiling(lookup, xpFlags, colCtx, outY, pos, checkHeight, outPoly, actor);

    posTemp = *pos;
    posTemp.y = *outY;
    tempY = *outY;

    if (BgCheck_CheckDynaCeiling(colCtx, xpFlags, &tempY, &posTemp, checkHeight, outPoly, outBgId, actor)) {
        *outY = tempY;
        result = true;
    }
    return result;
}

/**
 * Tests for collision with any ceiling poly
 * `checkHeight` must be a positive value
 * returns true if a collision occurs, else false
 * `outY` returns the displaced y coordinate needed to not collide with the poly
 */
s32 BgCheck_AnyCheckCeiling(CollisionContext* colCtx, f32* outY, Vec3f* pos, f32 checkHeight) {
    CollisionPoly* poly;
    s32 bgId;

    return BgCheck_CheckCeilingImpl(colCtx, COLPOLY_IGNORE_NONE, outY, pos, checkHeight, &poly, &bgId, NULL);
}

/**
 * Tests for collision with any entity solid ceiling poly
 * `checkHeight` must be a positive value
 * returns true if a collision occurs, else false
 * `outY` returns the displaced y coordinate needed to not collide with the poly
 */
s32 BgCheck_EntityCheckCeiling(CollisionContext* colCtx, f32* outY, Vec3f* pos, f32 checkHeight,
                               CollisionPoly** outPoly, s32* outBgId, Actor* actor) {
    return BgCheck_CheckCeilingImpl(colCtx, COLPOLY_IGNORE_ENTITY, outY, pos, checkHeight, outPoly, outBgId, actor);
}

/**
 * Tests if a line from `posA` to `posB` intersects with a poly
 * returns true if it does, else false
 * `posB`? `posResult` returns the point of intersection
 * `outPoly` returns the pointer to the intersected poly, while `outBgId` returns the entity the poly belongs to
 */
s32 BgCheck_CheckLineImpl(CollisionContext* colCtx, u16 xpFlags1, u16 xpFlags2, Vec3f* posA, Vec3f* posB,
                          Vec3f* posResult, CollisionPoly** outPoly, s32* outBgId, Actor* actor, f32 checkDist,
                          u32 bccFlags) {
    StaticLookup* lookupTbl;
    StaticLookup* iLookup;
    s32 subdivMin[3];
    s32 subdivMax[3];
    s32 i;
    s32 result;
    Vec3f posBTemp;
    StaticLineTest checkLine;
    Vec3f sectorMin;
    Vec3f sectorMax;
    s32 temp_lo;
    StaticLookup* lookup;
    s32 j;

    lookupTbl = colCtx->lookupTbl;
    posBTemp = *posB;

    *outBgId = BGCHECK_SCENE;

    BgCheck_ResetPolyCheckTbl(&colCtx->polyNodes, colCtx->colHeader->numPolygons);
    BgCheck_GetStaticLookupIndicesFromPos(colCtx, posA, (Vec3i*)&subdivMin);
    BgCheck_GetStaticLookupIndicesFromPos(colCtx, &posBTemp, (Vec3i*)&subdivMax);
    *posResult = *posB;
    checkLine.outDistSq = 1.0e38f;
    *outPoly = NULL;

    checkLine.colCtx = colCtx;
    checkLine.xpFlags2 = xpFlags2;
    checkLine.posA = posA;
    checkLine.posB = &posBTemp;
    checkLine.outPoly = outPoly;
    checkLine.xpFlags1 = xpFlags1;
    checkLine.outPos = posResult;
    checkLine.checkDist = checkDist;
    checkLine.bccFlags = bccFlags;
    checkLine.actor = actor;
    result = false;

    if (subdivMin[0] != subdivMax[0] || subdivMin[1] != subdivMax[1] || subdivMin[2] != subdivMax[2]) {
        for (i = 0; i < 3; i++) {
            if (subdivMax[i] < subdivMin[i]) {
                j = subdivMax[i];
                subdivMax[i] = subdivMin[i];
                subdivMin[i] = j;
            }
        }
        temp_lo = colCtx->subdivAmount.x * colCtx->subdivAmount.y;
        iLookup = lookupTbl + subdivMin[2] * temp_lo;
        sectorMin.z = subdivMin[2] * colCtx->subdivLength.z + colCtx->minBounds.z;
        sectorMax.z = colCtx->subdivLength.z + sectorMin.z;

        for (i = subdivMin[2]; i < subdivMax[2] + 1; i++) {
            StaticLookup* jLookup = iLookup + subdivMin[1] * colCtx->subdivAmount.x;
            s32 pad;

            sectorMin.y = subdivMin[1] * colCtx->subdivLength.y + colCtx->minBounds.y;
            sectorMax.y = colCtx->subdivLength.y + sectorMin.y;

            for (j = subdivMin[1]; j < subdivMax[1] + 1; j++) {
                s32 k;

                lookup = jLookup + subdivMin[0];
                sectorMin.x = subdivMin[0] * colCtx->subdivLength.x + colCtx->minBounds.x;
                sectorMax.x = colCtx->subdivLength.x + sectorMin.x;

                for (k = subdivMin[0]; k < subdivMax[0] + 1; k++) {
                    if (Math3D_LineVsCube(&sectorMin, &sectorMax, posA, &posBTemp)) {
                        checkLine.lookup = lookup;

                        if (BgCheck_CheckLineInSubdivision(&checkLine)) {
                            result = true;
                        }
                    }
                    lookup++;
                    sectorMin.x += colCtx->subdivLength.x;
                    sectorMax.x += colCtx->subdivLength.x;
                }
                jLookup += colCtx->subdivAmount.x;
                sectorMin.y += colCtx->subdivLength.y;
                sectorMax.y += colCtx->subdivLength.y;
            }
            iLookup += temp_lo;
            sectorMin.z += colCtx->subdivLength.z;
            sectorMax.z += colCtx->subdivLength.z;
        }
    } else if (BgCheck_PosInStaticBoundingBox(colCtx, posA) == false) {
        return false;
    } else {
        checkLine.lookup = BgCheck_GetNearestStaticLookup(colCtx, lookupTbl, posA);
        result = BgCheck_CheckLineInSubdivision(&checkLine);
        if (result) {
            checkLine.outDistSq = Math3D_Vec3fDistSq(posResult, posA);
        }
    }
    if ((bccFlags & BGCHECK_CHECK_DYNA) &&
        BgCheck_CheckLineAgainstDyna(colCtx, xpFlags1, posA, &posBTemp, posResult, outPoly, &checkLine.outDistSq,
                                     outBgId, actor, checkDist, bccFlags)) {
        result = true;
    }
    return result;
}

/**
 * Get bccFlags
 */
u32 BgCheck_GetBccFlags(s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace, s32 checkDyna) {
    u32 result = 0;

    if (checkWall) {
        result = BGCHECK_CHECK_WALL;
    }
    if (checkFloor) {
        result |= BGCHECK_CHECK_FLOOR;
    }
    if (checkCeil) {
        result |= BGCHECK_CHECK_CEILING;
    }
    if (checkOneFace) {
        result |= BGCHECK_CHECK_ONE_FACE;
    }
    if (checkDyna) {
        result |= BGCHECK_CHECK_DYNA;
    }
    return result;
}

/**
 * Public. Tests if a line from `posA` to `posB` intersects with a poly
 * returns true if it does, else false
 */
s32 BgCheck_CameraLineTest1(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult,
                            CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace,
                            s32* bgId) {
    return BgCheck_CheckLineImpl(colCtx, COLPOLY_IGNORE_CAMERA, COLPOLY_IGNORE_NONE, posA, posB, posResult, outPoly,
                                 bgId, NULL, 1.0f,
                                 BgCheck_GetBccFlags(checkWall, checkFloor, checkCeil, checkOneFace, true));
}

/**
 * Public. Tests if a line from `posA` to `posB` intersects with a poly
 * returns true if it does, else false
 */
s32 BgCheck_CameraLineTest2(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult,
                            CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace,
                            s32* bgId) {
    return BgCheck_CheckLineImpl(colCtx, COLPOLY_IGNORE_NONE, COLPOLY_IGNORE_CAMERA, posA, posB, posResult, outPoly,
                                 bgId, NULL, 1.0f,
                                 BgCheck_GetBccFlags(checkWall, checkFloor, checkCeil, checkOneFace, true));
}

/**
 * Public. Tests if a line from `posA` to `posB` intersects with a poly
 * returns true if it does, else false
 */
s32 BgCheck_EntityLineTest1(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult,
                            CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace,
                            s32* bgId) {
    return BgCheck_CheckLineImpl(colCtx, COLPOLY_IGNORE_ENTITY, COLPOLY_IGNORE_NONE, posA, posB, posResult, outPoly,
                                 bgId, NULL, 1.0f,
                                 BgCheck_GetBccFlags(checkWall, checkFloor, checkCeil, checkOneFace, true));
}

/**
 * Public. Tests if a line from `posA` to `posB` intersects with a poly
 * returns true if it does, else false
 */
s32 BgCheck_EntityLineTest2(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult,
                            CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace,
                            s32* bgId, Actor* actor) {
    return BgCheck_CheckLineImpl(colCtx, COLPOLY_IGNORE_ENTITY, COLPOLY_IGNORE_NONE, posA, posB, posResult, outPoly,
                                 bgId, actor, 1.0f,
                                 BgCheck_GetBccFlags(checkWall, checkFloor, checkCeil, checkOneFace, true));
}

/**
 * Public. Tests if a line from `posA` to `posB` intersects with a poly
 * returns true if it does, else false
 */
s32 BgCheck_EntityLineTest3(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult,
                            CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace,
                            s32* bgId, Actor* actor, f32 checkDist) {
    return BgCheck_CheckLineImpl(colCtx, COLPOLY_IGNORE_ENTITY, COLPOLY_IGNORE_NONE, posA, posB, posResult, outPoly,
                                 bgId, actor, checkDist,
                                 BgCheck_GetBccFlags(checkWall, checkFloor, checkCeil, checkOneFace, true));
}

/**
 * Public. Tests if a line from `posA` to `posB` intersects with a poly
 * returns true if it does, else false
 */
s32 BgCheck_ProjectileLineTest(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult,
                               CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace,
                               s32* bgId) {
    return BgCheck_CheckLineImpl(colCtx, COLPOLY_IGNORE_PROJECTILES, COLPOLY_IGNORE_NONE, posA, posB, posResult,
                                 outPoly, bgId, NULL, 1.0f,
                                 BgCheck_GetBccFlags(checkWall, checkFloor, checkCeil, checkOneFace, true));
}

/**
 * Public. Tests if a line from `posA` to `posB` intersects with a poly
 * returns true if it does, else false
 */
s32 BgCheck_AnyLineTest1(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly,
                         s32 checkOneFace) {
    return BgCheck_AnyLineTest2(colCtx, posA, posB, posResult, outPoly, true, true, true, checkOneFace);
}

/**
 * Public. Tests if a line from `posA` to `posB` intersects with a poly
 * returns true if it does, else false
 */
s32 BgCheck_AnyLineTest2(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly,
                         s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace) {
    s32 bgId;

    return BgCheck_CheckLineImpl(colCtx, COLPOLY_IGNORE_NONE, COLPOLY_IGNORE_NONE, posA, posB, posResult, outPoly,
                                 &bgId, NULL, 1.0f,
                                 BgCheck_GetBccFlags(checkWall, checkFloor, checkCeil, checkOneFace, true));
}

/**
 * Public. Tests if a line from `posA` to `posB` intersects with a poly
 * returns true if it does, else false
 */
s32 BgCheck_AnyLineTest3(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly,
                         s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace, s32* bgId) {
    return BgCheck_CheckLineImpl(colCtx, COLPOLY_IGNORE_NONE, COLPOLY_IGNORE_NONE, posA, posB, posResult, outPoly, bgId,
                                 NULL, 1.0f, BgCheck_GetBccFlags(checkWall, checkFloor, checkCeil, checkOneFace, true));
}

/**
 * Get first poly intersecting sphere `center` `radius`
 * ignores `actor` dyna poly
 * returns true if any poly intersects the sphere, else false
 * `outPoly` returns the pointer of the first poly found that intersects
 * `outBgId` returns the bgId of the entity that owns `outPoly`
 */
s32 BgCheck_SphVsFirstPolyImpl(CollisionContext* colCtx, u16 xpFlags, CollisionPoly** outPoly, s32* outBgId,
                               Vec3f* center, f32 radius, Actor* actor, u16 bciFlags) {
    StaticLookup* lookup;

    *outBgId = BGCHECK_SCENE;
    lookup = BgCheck_GetStaticLookup(colCtx, colCtx->lookupTbl, center);
    if (lookup == NULL) {
        return false;
    } else if (BgCheck_SphVsFirstStaticPoly(lookup, xpFlags, colCtx, center, radius, outPoly, bciFlags, actor) ||
               BgCheck_SphVsFirstDynaPoly(colCtx, xpFlags, outPoly, outBgId, center, radius, actor, bciFlags)) {
        return true;
    }
    return false;
}

/**
 * Public get first poly intersecting sphere `center` `radius`
 */
s32 BgCheck_SphVsFirstPoly(CollisionContext* colCtx, Vec3f* center, f32 radius) {
    CollisionPoly* poly;
    s32 bgId;

    return BgCheck_SphVsFirstPolyImpl(colCtx, COLPOLY_IGNORE_NONE, &poly, &bgId, center, radius, NULL,
                                      BGCHECK_IGNORE_NONE);
}

/**
 * Public get first wall poly intersecting sphere `center` `radius`
 */
s32 BgCheck_SphVsFirstWall(CollisionContext* colCtx, Vec3f* center, f32 radius) {
    CollisionPoly* poly;
    s32 bgId;

    return BgCheck_SphVsFirstPolyImpl(colCtx, COLPOLY_IGNORE_NONE, &poly, &bgId, center, radius, NULL,
                                      BGCHECK_IGNORE_FLOOR | BGCHECK_IGNORE_CEILING);
}

/**
 * Init SSNodeList
 */
void SSNodeList_Init(SSNodeList* this) {
    this->max = 0;
    this->count = 0;
    this->tbl = NULL;
    this->polyCheckTbl = NULL;
}

/**
 * Allocate SSNodeList
 * tblMax is the number of SSNode records to allocate
 * numPolys is the number of polygons defined within the CollisionHeader
 */
void SSNodeList_Alloc(GlobalContext* globalCtx, SSNodeList* this, s32 tblMax, s32 numPolys) {
    this->max = tblMax;
    this->count = 0;
    this->tbl = THA_AllocEndAlign(&globalCtx->state.heap, tblMax * sizeof(SSNode), -2);
    this->polyCheckTbl = THA_AllocEndAlign16(&globalCtx->state.heap, numPolys * sizeof(u8));

    if (this->polyCheckTbl == NULL) {
        sprintf(D_801ED950, "this->polygon_check == NULL(game_alloc() MemoryAllocationError.)\n");
        sprintf(D_801ED9A0, "short_slist_node_size = %d/polygon_num = %d\n", tblMax, numPolys);
        Fault_AddHungupAndCrashImpl(D_801ED950, D_801ED9A0);
    }
}

/**
 * Get next SSNodeList SSNode
 */
SSNode* SSNodeList_GetNextNode(SSNodeList* this) {
    SSNode* result = &this->tbl[this->count];

    this->count++;
    if (!(this->count < this->max)) {
        return NULL;
    }
    return result;
}

/**
 * Get next SSNodeList SSNode index
 */
u16 SSNodeList_GetNextNodeIdx(SSNodeList* this) {
    u16 new_index = this->count++;

    return new_index;
}

/**
 * Initialize ScaleRotPos
 */
void ScaleRotPos_Init(ScaleRotPos* srp) {
    srp->scale.x = srp->scale.y = srp->scale.z = 1;
    srp->rot.x = 0;
    srp->rot.y = 0;
    srp->rot.z = 0;
    srp->pos.x = srp->pos.y = srp->pos.z = 0;
}

/**
 * Set ScaleRotPos
 */
void ScaleRotPos_SetValue(ScaleRotPos* srp, Vec3f* scale, Vec3s* rot, Vec3f* pos) {
    srp->scale = *scale;
    srp->rot = *rot;
    srp->pos = *pos;
}

/**
 * ScaleRotPos equality test
 */
s32 ScaleRotPos_IsEqual(ScaleRotPos* a, ScaleRotPos* b) {
    if (a->scale.x != b->scale.x || a->scale.y != b->scale.y || a->scale.z != b->scale.z || a->rot.x != b->rot.x ||
        a->rot.y != b->rot.y || a->rot.z != b->rot.z || a->pos.x != b->pos.x || a->pos.y != b->pos.y ||
        a->pos.z != b->pos.z) {
        return false;
    }
    return true;
}

/**
 * Reset DynaLookup lists
 */
void DynaLookup_ResetLists(DynaLookup* dynaLookup) {
    SSList_SetNull(&dynaLookup->ceiling);
    SSList_SetNull(&dynaLookup->wall);
    SSList_SetNull(&dynaLookup->floor);
}

/**
 * Reset DynaLookup
 */
void DynaLookup_Reset(DynaLookup* dynaLookup) {
    dynaLookup->polyStartIndex = 0;
    DynaLookup_ResetLists(dynaLookup);
}

/**
 * Reset vtxStartIndex
 */
void DynaLookup_ResetVtxStartIndex(u16* vtxStartIndex) {
    *vtxStartIndex = 0;
}

/**
 * Reset waterBoxStartIndex
 */
void DynaLookup_ResetWaterBoxStartIndex(u16* waterBoxStartIndex) {
    *waterBoxStartIndex = 0;
}

/**
 * Initialize BgActor
 */
void BgActor_Init(GlobalContext* globalCtx, BgActor* bgActor) {
    bgActor->actor = NULL;
    bgActor->colHeader = NULL;
    ScaleRotPos_Init(&bgActor->prevTransform);
    ScaleRotPos_Init(&bgActor->curTransform);
    DynaLookup_Reset(&bgActor->dynaLookup);
    DynaLookup_ResetVtxStartIndex(&bgActor->vtxStartIndex);
    DynaLookup_ResetWaterBoxStartIndex(&bgActor->waterboxesStartIndex);
    bgActor->boundingSphere.center.x = bgActor->boundingSphere.center.y = bgActor->boundingSphere.center.z = 0;
    bgActor->boundingSphere.radius = 0;
}

/**
 * setActor internal
 */
void BgActor_SetActor(BgActor* bgActor, Actor* actor, CollisionHeader* colHeader) {
    bgActor->actor = actor;
    bgActor->colHeader = colHeader;
    bgActor->prevTransform.scale = actor->scale;
    bgActor->prevTransform.rot = actor->shape.rot;
    bgActor->prevTransform.rot.x--;
    bgActor->prevTransform.pos = actor->world.pos;
    bgActor->curTransform.scale = actor->scale;
    bgActor->curTransform.rot = actor->shape.rot;
    bgActor->curTransform.pos = actor->world.pos;
}

/**
 * Test if the BgActor transform is the same
 */
s32 BgActor_IsTransformUnchanged(BgActor* bgActor) {
    return ScaleRotPos_IsEqual(&bgActor->prevTransform, &bgActor->curTransform);
}

/**
 * NULL polyList
 */
void DynaPoly_NullPolyList(CollisionPoly** polyList) {
    *polyList = NULL;
}

/**
 * Allocate dyna.polyList
 */
void DynaPoly_AllocPolyList(GlobalContext* globalCtx, CollisionPoly** polyList, s32 numPolys) {
    *polyList = THA_AllocEndAlign(&globalCtx->state.heap, numPolys * sizeof(CollisionPoly), -2);
}

/**
 * NULL vtxList
 */
void DynaPoly_NullVtxList(Vec3s** vtxList) {
    *vtxList = NULL;
}

/**
 * Allocate dyna.vtxList
 */
void DynaPoly_AllocVtxList(GlobalContext* globalCtx, Vec3s** vtxList, s32 numVtx) {
    *vtxList = THA_AllocEndAlign(&globalCtx->state.heap, numVtx * sizeof(Vec3s), -2);
}

/**
 * Init dyna.waterBoxList
 */
void DynaPoly_InitWaterBoxList(DynaWaterBoxList* waterBoxList) {
    waterBoxList->unk0 = 0;
    waterBoxList->boxes = NULL;
}

/**
 * Allocate dyna.waterBoxList
 */
void DynaPoly_AllocWaterBoxList(GlobalContext* globalCtx, DynaWaterBoxList* waterBoxList, s32 numWaterBoxes) {
    waterBoxList->boxes = THA_AllocEndAlign(&globalCtx->state.heap, numWaterBoxes * sizeof(WaterBox), -2);
}

/**
 * Update BgActor's prevTransform
 */
void DynaPoly_SetBgActorPrevTransform(GlobalContext* globalCtx, BgActor* bgActor) {
    bgActor->prevTransform = bgActor->curTransform;
}

/**
 * Is BgActor Id
 */
s32 DynaPoly_IsBgIdBgActor(s32 bgId) {
    if (bgId < 0 || bgId >= BG_ACTOR_MAX) {
        return false;
    }
    return true;
}

/**
 * Init DynaCollisionContext
 */
void DynaPoly_Init(GlobalContext* globalCtx, DynaCollisionContext* dyna) {
    dyna->bitFlag = DYNAPOLY_INVALIDATE_LOOKUP;
    DynaPoly_NullPolyList(&dyna->polyList);
    DynaPoly_NullVtxList(&dyna->vtxList);
    DynaPoly_InitWaterBoxList(&dyna->waterBoxList);
    DynaSSNodeList_Init(globalCtx, &dyna->polyNodes);
}

/**
 * Set DynaCollisionContext
 */
void DynaPoly_Alloc(GlobalContext* globalCtx, DynaCollisionContext* dyna) {
    s32 i;

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        BgActor_Init(globalCtx, &dyna->bgActors[i]);
        dyna->bgActorFlags[i] = 0;
    }
    DynaPoly_NullPolyList(&dyna->polyList);
    DynaPoly_AllocPolyList(globalCtx, &dyna->polyList, dyna->polyListMax);

    DynaPoly_NullVtxList(&dyna->vtxList);
    DynaPoly_AllocVtxList(globalCtx, &dyna->vtxList, dyna->vtxListMax);

    DynaPoly_InitWaterBoxList(&dyna->waterBoxList);
    DynaPoly_AllocWaterBoxList(globalCtx, &dyna->waterBoxList, DYNA_WATERBOX_MAX);

    DynaSSNodeList_Init(globalCtx, &dyna->polyNodes);
    DynaSSNodeList_Alloc(globalCtx, &dyna->polyNodes, dyna->polyNodesMax);
}

/**
 * Set BgActor
 * original name: DynaPolyInfo_setActor
 */
s32 DynaPoly_SetBgActor(GlobalContext* globalCtx, DynaCollisionContext* dyna, Actor* actor,
                        CollisionHeader* colHeader) {
    s32 bgId;
    s32 foundSlot = false;

    for (bgId = 0; bgId < BG_ACTOR_MAX; bgId++) {
        if (!(dyna->bgActorFlags[bgId] & 1)) {
            dyna->bgActorFlags[bgId] |= 1;
            foundSlot = true;
            break;
        }
    }

    if (foundSlot == false) {
        return BG_ACTOR_MAX;
    }

    BgActor_SetActor(&dyna->bgActors[bgId], actor, colHeader);
    dyna->bitFlag |= DYNAPOLY_INVALIDATE_LOOKUP;

    dyna->bgActorFlags[bgId] &= ~2;
    return bgId;
}

/**
 * Gets the actor assigned to `bgId`
 */
DynaPolyActor* DynaPoly_GetActor(CollisionContext* colCtx, s32 bgId) {
    if (!DynaPoly_IsBgIdBgActor(bgId) || !(colCtx->dyna.bgActorFlags[bgId] & 1) ||
        colCtx->dyna.bgActorFlags[bgId] & 2) {
        return NULL;
    }
    return (DynaPolyActor*)colCtx->dyna.bgActors[bgId].actor;
}

void func_800C62BC(GlobalContext* globalCtx, DynaCollisionContext* dyna, s32 bgId) {
    if (DynaPoly_IsBgIdBgActor(bgId)) {
        dyna->bgActorFlags[bgId] |= 4;
        dyna->bitFlag |= DYNAPOLY_INVALIDATE_LOOKUP;
    }
}

void func_800C6314(GlobalContext* globalCtx, DynaCollisionContext* dyna, s32 bgId) {
    if (DynaPoly_IsBgIdBgActor(bgId)) {
        dyna->bgActorFlags[bgId] &= ~4;
        dyna->bitFlag |= DYNAPOLY_INVALIDATE_LOOKUP;
    }
}

void func_800C636C(GlobalContext* globalCtx, DynaCollisionContext* dyna, s32 bgId) {
    if (DynaPoly_IsBgIdBgActor(bgId)) {
        dyna->bgActorFlags[bgId] |= 8;
        dyna->bitFlag |= DYNAPOLY_INVALIDATE_LOOKUP;
    }
}

void func_800C63C4(GlobalContext* globalCtx, DynaCollisionContext* dyna, s32 bgId) {
    if (DynaPoly_IsBgIdBgActor(bgId)) {
        dyna->bgActorFlags[bgId] &= ~8;
        dyna->bitFlag |= DYNAPOLY_INVALIDATE_LOOKUP;
    }
}

void func_800C641C(GlobalContext* globalCtx, DynaCollisionContext* dyna, s32 bgId) {
    if (DynaPoly_IsBgIdBgActor(bgId)) {
        dyna->bgActorFlags[bgId] |= 0x20;
        dyna->bitFlag |= DYNAPOLY_INVALIDATE_LOOKUP;
    }
}

void func_800C6474(GlobalContext* globalCtx, DynaCollisionContext* dyna, s32 bgId) {
    if (DynaPoly_IsBgIdBgActor(bgId)) {
        dyna->bgActorFlags[bgId] &= ~0x20;
        dyna->bitFlag |= DYNAPOLY_INVALIDATE_LOOKUP;
    }
}

/**
 * original name: DynaPolyInfo_delReserve
 */
void DynaPoly_DeleteBgActor(GlobalContext* globalCtx, DynaCollisionContext* dyna, s32 bgId) {
    DynaPolyActor* actor;

    if (DynaPoly_IsBgIdBgActor(bgId) == false) {
        return;
    }
    actor = DynaPoly_GetActor(&globalCtx->colCtx, bgId);
    if (actor != NULL) {

        actor->bgId = BGACTOR_NEG_ONE;
        dyna->bgActors[bgId].actor = NULL;
        dyna->bgActorFlags[bgId] |= 2;
    }
}

void func_800C6554(GlobalContext* globalCtx, DynaCollisionContext* dyna) {
    dyna->bitFlag |= DYNAPOLY_INVALIDATE_LOOKUP;
}

void BgCheck_CalcWaterboxDimensions(Vec3f* minPos, Vec3f* maxXPos, Vec3f* maxZPos, Vec3s* minPosOut, s16* xLength,
                                    s16* zLength) {
    f32 temp;

    minPosOut->x = minPos->x;
    minPosOut->y = minPos->y;
    minPosOut->z = minPos->z;

    if (maxXPos->x < minPosOut->x) {
        minPosOut->x = maxXPos->x;
    }
    if (maxXPos->y < minPosOut->y) {
        minPosOut->y = maxXPos->y;
    }
    if (maxXPos->z < minPosOut->z) {
        minPosOut->z = maxXPos->z;
    }

    if (maxZPos->x < minPosOut->x) {
        minPosOut->x = maxZPos->x;
    }
    if (maxZPos->y < minPosOut->y) {
        minPosOut->y = maxZPos->y;
    }
    if (maxZPos->z < minPosOut->z) {
        minPosOut->z = maxZPos->z;
    }

    *xLength = minPos->x - minPosOut->x;
    temp = maxXPos->x - minPosOut->x;

    if (*xLength < temp) {
        *xLength = temp;
    }
    temp = maxZPos->x - minPosOut->x;
    if (*xLength < temp) {
        *xLength = temp;
    }
    *zLength = minPos->z - minPosOut->z;

    temp = maxXPos->z - minPosOut->z;
    if (*zLength < temp) {
        *zLength = temp;
    }
    temp = maxZPos->z - minPosOut->z;
    if (*zLength < temp) {
        *zLength = temp;
    }
}

#ifdef NON_MATCHING
/**
 * original name: DynaPolyInfo_expandSRT
 */
void DynaPoly_ExpandSRT(GlobalContext* globalCtx, DynaCollisionContext* dyna, s32 bgId, s32* vtxStartIndex,
                        s32* polyStartIndex, s32* waterBoxStartIndex) {
    Actor* actor;
    s32 pad;
    s32 pad2;
    f32 numVtxInverse;
    s32 i;
    Vec3f pos; // sp170
    Sphere16* sphere;
    Vec3s* dVtxList;
    Vec3s* point;
    Vec3f newCenterPoint; // sp158

    f32 newRadiusSq;
    CollisionHeader* pbgdata;
    Vec3f newVtx; // sp144
    Vec3f vtxA;   // sp138
    Vec3f vtxB;   // sp12C
    Vec3f vtxC;   // sp120
    Vec3f newNormal;
    s32 wi;
    Vec3f spB8; // waterbox ?
    Vec3f spAC;
    Vec3f spA0; // waterbox ?
    Vec3f sp94;
    Vec3f sp88; // waterbox ?
    Vec3f sp7C;
    WaterBox* waterBox;

    pbgdata = dyna->bgActors[bgId].colHeader;
    sphere = &dyna->bgActors[bgId].boundingSphere;
    actor = dyna->bgActors[bgId].actor;
    dyna->bgActors[bgId].dynaLookup.polyStartIndex = *polyStartIndex;
    dyna->bgActors[bgId].vtxStartIndex = *vtxStartIndex;
    dyna->bgActors[bgId].waterboxesStartIndex = *waterBoxStartIndex;
    pos = actor->world.pos;
    pos.y += actor->shape.yOffset * actor->scale.y;

    ScaleRotPos_SetValue(&dyna->bgActors[bgId].curTransform, &actor->scale, &actor->shape.rot, &pos);

    if (dyna->bgActorFlags[bgId] & 4) {
        return;
    }

    if (!(DYNA_WATERBOX_MAX >= *waterBoxStartIndex + pbgdata->numWaterBoxes)) {
        sprintf(D_801EDAA8, "water_poly Error:[MoveBG OSUGI!!!]");
        sprintf(D_801EDAF8, "num = %d > %d\n", *waterBoxStartIndex + pbgdata->numWaterBoxes, DYNA_WATERBOX_MAX);
        Fault_AddHungupAndCrashImpl(D_801EDAA8, D_801EDAF8);
    }

    if (!(dyna->polyListMax >= *polyStartIndex + pbgdata->numPolygons)) {
        sprintf(D_801EDAA8, "bg_poly Error:[MoveBG OSUGI!!!]");
        sprintf(D_801EDAF8, "num = %d > %d\n", *polyStartIndex + pbgdata->numPolygons, dyna->polyListMax);
        Fault_AddHungupAndCrashImpl(D_801EDAA8, D_801EDAF8);
    }

    if (!(dyna->vtxListMax >= *vtxStartIndex + pbgdata->numVertices)) {
        sprintf(D_801EDAA8, "bg_vert Error:[MoveBG OSUGI!!!]");
        sprintf(D_801EDAF8, "num = %d > %d\n", *vtxStartIndex + pbgdata->numVertices, dyna->vtxListMax);
        Fault_AddHungupAndCrashImpl(D_801EDAA8, D_801EDAF8);
    }

    if (!(dyna->bitFlag & DYNAPOLY_INVALIDATE_LOOKUP) && BgActor_IsTransformUnchanged(&dyna->bgActors[bgId])) {
        s32 pi;

        for (pi = *polyStartIndex; pi < *polyStartIndex + pbgdata->numPolygons; pi++) {
            CollisionPoly* poly = &dyna->polyList[pi];
            s16 normalY = poly->normal.y;

            if (normalY > COLPOLY_SNORMAL(0.5f)) {
                if (!(dyna->bgActorFlags[bgId] & 0x20)) {
                    s16 polyIndex = pi;

                    DynaSSNodeList_SetSSListHead(&dyna->polyNodes, &dyna->bgActors[bgId].dynaLookup.floor, &polyIndex);
                }
            } else if (normalY < COLPOLY_SNORMAL(-0.8f)) {
                if (!(dyna->bgActorFlags[bgId] & 8)) {
                    s16 polyIndex = pi;

                    DynaSSNodeList_SetSSListHead(&dyna->polyNodes, &dyna->bgActors[bgId].dynaLookup.ceiling,
                                                 &polyIndex);
                }
            } else {
                s16 polyIndex = pi;

                DynaSSNodeList_SetSSListHead(&dyna->polyNodes, &dyna->bgActors[bgId].dynaLookup.wall, &polyIndex);
            }
        }

        *polyStartIndex += pbgdata->numPolygons;
        *vtxStartIndex += pbgdata->numVertices;
        *waterBoxStartIndex += pbgdata->numWaterBoxes;
    } else {
        SkinMatrix_SetScaleRotateYRPTranslate(
            &D_801EDA40, dyna->bgActors[bgId].curTransform.scale.x, dyna->bgActors[bgId].curTransform.scale.y,
            dyna->bgActors[bgId].curTransform.scale.z, dyna->bgActors[bgId].curTransform.rot.x,
            dyna->bgActors[bgId].curTransform.rot.y, dyna->bgActors[bgId].curTransform.rot.z,
            dyna->bgActors[bgId].curTransform.pos.x, dyna->bgActors[bgId].curTransform.pos.y,
            dyna->bgActors[bgId].curTransform.pos.z);

        if (pbgdata->numVertices != 0 && pbgdata->numPolygons != 0) {
            numVtxInverse = 1.0f / pbgdata->numVertices;
            newCenterPoint.x = newCenterPoint.y = newCenterPoint.z = 0.0f;
            for (i = 0; i < pbgdata->numVertices; i++) {
                Vec3f vtx;  // spF4
                Vec3f vtxT; // spE8 Vtx after mtx transform

                Math_Vec3s_ToVec3f(&vtx, &pbgdata->vtxList[i]);
                SkinMatrix_Vec3fMtxFMultXYZ(&D_801EDA40, &vtx, &vtxT);
                BgCheck_Vec3fToVec3s(&dyna->vtxList[*vtxStartIndex + i], &vtxT);

                if (i == 0) {
                    dyna->bgActors[bgId].minY = dyna->bgActors[bgId].maxY = vtxT.y;
                } else if (vtxT.y < dyna->bgActors[bgId].minY) {
                    dyna->bgActors[bgId].minY = vtxT.y;
                } else if (dyna->bgActors[bgId].maxY < vtxT.y) {
                    dyna->bgActors[bgId].maxY = vtxT.y;
                }
                newCenterPoint.x += vtxT.x;
                newCenterPoint.y += vtxT.y;
                newCenterPoint.z += vtxT.z;
            }

            newCenterPoint.x *= numVtxInverse;
            newCenterPoint.y *= numVtxInverse;
            newCenterPoint.z *= numVtxInverse;
            sphere->center.x = newCenterPoint.x;
            sphere->center.y = newCenterPoint.y;
            sphere->center.z = newCenterPoint.z;
            newRadiusSq = -100.0f;

            for (i = 0; i < pbgdata->numVertices; i++) {
                f32 radiusSq;
                s32 idx = *vtxStartIndex + i;

                newVtx.x = dyna->vtxList[idx].x;
                newVtx.y = dyna->vtxList[idx].y;
                newVtx.z = dyna->vtxList[idx].z;
                radiusSq = Math3D_Vec3fDistSq(&newVtx, &newCenterPoint);
                if (newRadiusSq < radiusSq) {
                    newRadiusSq = radiusSq;
                }
            }

            sphere->radius = sqrtf(newRadiusSq) * 1.1f;

            for (i = 0; i < pbgdata->numPolygons; i++) {
                CollisionPoly* newPoly = &dyna->polyList[*polyStartIndex + i];
                f32 newNormMagnitude;
                u32 vIA;
                u32 vIB;
                u32 vIC;

                *newPoly = pbgdata->polyList[i];

                vIA = (COLPOLY_VTX_INDEX(newPoly->flags_vIA) + *vtxStartIndex);
                vIB = (COLPOLY_VTX_INDEX(newPoly->flags_vIB) + *vtxStartIndex);
                vIC = newPoly->vIC + *vtxStartIndex;

                newPoly->flags_vIA = vIA | ((*newPoly).flags_vIA & 0xE000);
                newPoly->flags_vIB = vIB | ((*newPoly).flags_vIB & 0xE000);
                newPoly->vIC = vIC;
                dVtxList = dyna->vtxList;
                vtxA.x = dVtxList[vIA].x;
                vtxA.y = dVtxList[vIA].y;
                vtxA.z = dVtxList[vIA].z;
                vtxB.x = dVtxList[vIB].x;
                vtxB.y = dVtxList[vIB].y;
                vtxB.z = dVtxList[vIB].z;
                vtxC.x = dVtxList[vIC].x;
                vtxC.y = dVtxList[vIC].y;
                vtxC.z = dVtxList[vIC].z;
                Math3D_SurfaceNorm(&vtxA, &vtxB, &vtxC, &newNormal);
                newNormMagnitude = Math3D_Vec3fMagnitude(&newNormal);

                if (!IS_ZERO(newNormMagnitude)) {
                    newNormal.x *= (1.0f / newNormMagnitude);
                    newNormal.y *= (1.0f / newNormMagnitude);
                    newNormal.z *= (1.0f / newNormMagnitude);
                    newPoly->normal.x = COLPOLY_SNORMAL(newNormal.x);
                    newPoly->normal.y = COLPOLY_SNORMAL(newNormal.y);
                    newPoly->normal.z = COLPOLY_SNORMAL(newNormal.z);
                }

                newPoly->dist = -DOTXYZ(newNormal, dVtxList[vIA]);
                if (newNormal.y > 0.5f) {
                    s16 polyId = *polyStartIndex + i;

                    DynaSSNodeList_SetSSListHead(&dyna->polyNodes, &dyna->bgActors[bgId].dynaLookup.floor, &polyId);
                } else if (newNormal.y < -0.8f) {
                    s16 polyId = *polyStartIndex + i;

                    DynaSSNodeList_SetSSListHead(&dyna->polyNodes, &dyna->bgActors[bgId].dynaLookup.ceiling, &polyId);
                } else {
                    s16 polyId = *polyStartIndex + i;

                    DynaSSNodeList_SetSSListHead(&dyna->polyNodes, &dyna->bgActors[bgId].dynaLookup.wall, &polyId);
                }
            }
        }

        for (wi = 0; wi < pbgdata->numWaterBoxes; wi++) {
            Math_Vec3s_ToVec3f(&spB8, &pbgdata->waterBoxes[wi].minPos);
            Math_Vec3f_Copy(&spA0, &spB8);
            spA0.x += pbgdata->waterBoxes[wi].xLength;
            Math_Vec3f_Copy(&sp88, &spB8);
            sp88.z += pbgdata->waterBoxes[wi].zLength;
            SkinMatrix_Vec3fMtxFMultXYZ(&D_801EDA40, &spB8, &spAC);
            SkinMatrix_Vec3fMtxFMultXYZ(&D_801EDA40, &spA0, &sp94);
            SkinMatrix_Vec3fMtxFMultXYZ(&D_801EDA40, &sp88, &sp7C);
            waterBox = &dyna->waterBoxList.boxes[*waterBoxStartIndex + wi];
            BgCheck_CalcWaterboxDimensions(&spAC, &sp94, &sp7C, &waterBox->minPos, &waterBox->xLength,
                                           &waterBox->zLength);
            waterBox->properties = pbgdata->waterBoxes[wi].properties;
        }

        *polyStartIndex += pbgdata->numPolygons;
        *vtxStartIndex += pbgdata->numVertices;
        *waterBoxStartIndex += pbgdata->numWaterBoxes;
    }
}
#else
#pragma GLOBAL_ASM("asm/non_matchings/code/z_bgcheck/DynaPoly_ExpandSRT.s")
#endif

void BgCheck_ResetFlagsIfLoadedActor(GlobalContext* globalCtx, DynaCollisionContext* dyna, Actor* actor) {
    DynaPolyActor* dynaActor;
    s32 i;

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        if ((dyna->bgActorFlags[i] & 1)) {
            dynaActor = DynaPoly_GetActor(&globalCtx->colCtx, i);
            if (dynaActor != NULL && &dynaActor->actor == actor) {
                DynaPolyActor_ResetState((DynaPolyActor*)actor);
                return;
            }
        }
    }
}

/**
 * original name: DynaPolyInfo_setup
 */
void DynaPoly_Setup(GlobalContext* globalCtx, DynaCollisionContext* dyna) {
    DynaPolyActor* actor;
    s32 vtxStartIndex;
    s32 polyStartIndex;
    s32 waterBoxStartIndex;
    s32 i;

    DynaSSNodeList_ResetCount(&dyna->polyNodes);

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        DynaLookup_ResetLists(&dyna->bgActors[i].dynaLookup);
    }

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        if (dyna->bgActorFlags[i] & 2) {
            // Initialize BgActor
            dyna->bgActorFlags[i] = 0;
            BgActor_Init(globalCtx, &dyna->bgActors[i]);
            dyna->bitFlag |= DYNAPOLY_INVALIDATE_LOOKUP;
        }
        if (dyna->bgActors[i].actor != NULL && dyna->bgActors[i].actor->update == NULL) {
            // Delete BgActor
            actor = DynaPoly_GetActor(&globalCtx->colCtx, i);
            if (actor == NULL) {
                return;
            }
            actor->bgId = BGACTOR_NEG_ONE;
            dyna->bgActorFlags[i] = 0;

            BgActor_Init(globalCtx, &dyna->bgActors[i]);
            dyna->bitFlag |= DYNAPOLY_INVALIDATE_LOOKUP;
        }
    }
    vtxStartIndex = 0;
    polyStartIndex = 0;
    waterBoxStartIndex = 0;
    for (i = 0; i < BG_ACTOR_MAX; i++) {
        if ((dyna->bgActorFlags[i] & 1) && !(dyna->bgActorFlags[i] & 2)) {
            DynaPoly_ExpandSRT(globalCtx, dyna, i, &vtxStartIndex, &polyStartIndex, &waterBoxStartIndex);
        }
    }
    dyna->bitFlag &= ~DYNAPOLY_INVALIDATE_LOOKUP;
}

/**
 * Compute the number of dynamic resources in use?
 */
void func_800C756C(DynaCollisionContext* dyna, s32* numPolygons, s32* numVertices, s32* numWaterBoxes) {
    s32 i;
    CollisionHeader* colHeader;

    *numPolygons = 0;
    *numVertices = 0;
    *numWaterBoxes = 0;

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        if ((dyna->bgActorFlags[i] & 1) && !(dyna->bgActorFlags[i] & 2) && !(dyna->bgActorFlags[i] & 4)) {
            colHeader = dyna->bgActors[i].colHeader;
            *numPolygons += colHeader->numPolygons;
            *numVertices += colHeader->numVertices;
            *numWaterBoxes += colHeader->numWaterBoxes;
        }
    }
}

/**
 * Update all BgActor's previous ScaleRotPos
 */
void DynaPoly_UpdateBgActorTransforms(GlobalContext* globalCtx, DynaCollisionContext* dyna) {
    s32 i;

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        if (dyna->bgActorFlags[i] & 1) {
            DynaPoly_SetBgActorPrevTransform(globalCtx, &dyna->bgActors[i]);
        }
    }
}

/**
 * Perform dyna poly raycast toward floor on a list of floor, wall, or ceiling polys
 * `listType` specifies the poly list type (e.g. DYNA_RAYCAST_FLOORS)
 */
f32 BgCheck_RaycastFloorDynaList(DynaRaycast* dynaRaycast, u32 listType) {
    CollisionPoly* polyList;
    SSNode* curNode;
    f32 result;
    f32 yIntersect;
    s16 id;

    result = dynaRaycast->yIntersect;
    if (dynaRaycast->ssList->head == SS_NULL) {
        return result;
    }
    polyList = dynaRaycast->dyna->polyList;
    curNode = &dynaRaycast->dyna->polyNodes.tbl[dynaRaycast->ssList->head];

    while (true) {
        id = curNode->polyId;
        if (COLPOLY_VIA_FLAG_TEST(polyList[id].flags_vIA, dynaRaycast->xpFlags) ||
            (COLPOLY_VIA_FLAG_TEST(polyList[id].flags_vIB, 4) &&
             ((dynaRaycast->actor != NULL && dynaRaycast->actor->category != ACTORCAT_PLAYER) ||
              (dynaRaycast->actor == NULL && dynaRaycast->xpFlags != COLPOLY_IGNORE_CAMERA))) ||
            ((dynaRaycast->unk_24 & 0x20) && func_800C9B68(dynaRaycast->colCtx, &polyList[id], dynaRaycast->unk1C))) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dynaRaycast->dyna->polyNodes.tbl[curNode->next];
                continue;
            }
        }
        if ((listType & (DYNA_RAYCAST_WALLS | DYNA_RAYCAST_CEILINGS)) && (dynaRaycast->unk_24 & 0x10) &&
            COLPOLY_GET_NORMAL(polyList[id].normal.y) < 0.0f) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dynaRaycast->dyna->polyNodes.tbl[curNode->next];
                continue;
            }
        }
        if (CollisionPoly_CheckYIntersectApprox1(&polyList[id], dynaRaycast->dyna->vtxList, dynaRaycast->pos->x,
                                                 dynaRaycast->pos->z, &yIntersect, dynaRaycast->checkDist) &&
            yIntersect < dynaRaycast->pos->y && result < yIntersect) {
            result = yIntersect;
            *dynaRaycast->resultPoly = &dynaRaycast->dyna->polyList[id];
        }
        if (curNode->next == SS_NULL) {
            break;
        } else {
            curNode = &dynaRaycast->dyna->polyNodes.tbl[curNode->next];
            continue;
        }
    }
    return result;
}

#ifdef NON_MATCHING
/**
 * Perform dyna poly raycast toward floor
 * returns the yIntersect of the poly found, or BGCHECK_Y_MIN if no poly is found
 */
f32 BgCheck_RaycastFloorDyna(DynaRaycast* dynaRaycast) {
    s32 i;
    f32 result;
    f32 intersect2;
    s32 i2;
    s32 pauseState;
    DynaPolyActor* dynaActor;
    s32 pad;
    Vec3f polyVtx[3];
    Vec3f polyNorm;
    u32 polyIndex;
    CollisionPoly* polyMin;
    MtxF srpMtx;
    f32 magnitude;
    Vec3s* vtxList;
    f32 polyDist;
    Vec3f vtx;
    f32 intersect;
    ScaleRotPos* curTransform;
    CollisionPoly* poly;

    result = BGCHECK_Y_MIN;
    *dynaRaycast->bgId = BGCHECK_SCENE;

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        if (!(dynaRaycast->colCtx->dyna.bgActorFlags[i] & 1) || (dynaRaycast->colCtx->dyna.bgActorFlags[i] & 2)) {
            continue;
        }

        if (dynaRaycast->actor == dynaRaycast->colCtx->dyna.bgActors[i].actor ||
            dynaRaycast->pos->y < dynaRaycast->colCtx->dyna.bgActors[i].minY ||
            !Math3D_XZInSphere(&dynaRaycast->colCtx->dyna.bgActors[i].boundingSphere, dynaRaycast->pos->x,
                               dynaRaycast->pos->z)) {
            continue;
        }

        dynaRaycast->unk1C = i;
        dynaRaycast->dyna = &dynaRaycast->colCtx->dyna;
        if (dynaRaycast->unk_24 & BGCHECK_IGNORE_FLOOR) {
            dynaRaycast->ssList = &dynaRaycast->colCtx->dyna.bgActors[i].dynaLookup.floor;
            intersect2 = BgCheck_RaycastFloorDynaList(dynaRaycast, DYNA_RAYCAST_FLOORS);

            if (dynaRaycast->yIntersect < intersect2) {
                dynaRaycast->yIntersect = intersect2;
                *dynaRaycast->bgId = i;
                result = intersect2;
            }
        }
        if ((dynaRaycast->unk_24 & BGCHECK_IGNORE_WALL) ||
            (*dynaRaycast->resultPoly == NULL && (dynaRaycast->unk_24 & 8))) {
            dynaRaycast->ssList = &dynaRaycast->colCtx->dyna.bgActors[i].dynaLookup.wall;
            intersect2 = BgCheck_RaycastFloorDynaList(dynaRaycast, DYNA_RAYCAST_WALLS);
            if (dynaRaycast->yIntersect < intersect2) {

                dynaRaycast->yIntersect = intersect2;
                *dynaRaycast->bgId = i;
                result = intersect2;
            }
        }

        if (dynaRaycast->unk_24 & BGCHECK_IGNORE_CEILING) {
            dynaRaycast->ssList = &dynaRaycast->colCtx->dyna.bgActors[i].dynaLookup.ceiling;
            intersect2 = BgCheck_RaycastFloorDynaList(dynaRaycast, DYNA_RAYCAST_CEILINGS);
            if (dynaRaycast->yIntersect < intersect2) {

                dynaRaycast->yIntersect = intersect2;
                *dynaRaycast->bgId = i;
                result = intersect2;
            }
        }
    }

    dynaActor = DynaPoly_GetActor(dynaRaycast->colCtx, *dynaRaycast->bgId);
    if ((result != BGCHECK_Y_MIN) && (dynaActor != NULL) && (dynaRaycast->globalCtx != NULL)) {
        pauseState = dynaRaycast->globalCtx->pauseCtx.state != 0;
        if (!pauseState) {
            pauseState = dynaRaycast->globalCtx->pauseCtx.debugState != 0;
        }
        if (!pauseState && (dynaRaycast->colCtx->dyna.bgActorFlags[*dynaRaycast->bgId] & 2)) {
            curTransform = &dynaRaycast->dyna->bgActors[*dynaRaycast->bgId].curTransform;
            polyMin = &dynaRaycast->dyna
                           ->polyList[(u16)dynaRaycast->dyna->bgActors[*dynaRaycast->bgId].dynaLookup.polyStartIndex];
            polyIndex = *dynaRaycast->resultPoly - polyMin;
            poly = &dynaRaycast->dyna->bgActors[*dynaRaycast->bgId].colHeader->polyList[polyIndex];

            SkinMatrix_SetScaleRotateYRPTranslate(&srpMtx, curTransform->scale.x, curTransform->scale.y,
                                                  curTransform->scale.z, curTransform->rot.x, curTransform->rot.y,
                                                  curTransform->rot.z, curTransform->pos.x, curTransform->pos.y,
                                                  curTransform->pos.z);

            vtxList = dynaRaycast->dyna->bgActors[*dynaRaycast->bgId].colHeader->vtxList;

            for (i2 = 0; i2 < 3; i2++) {
                Math_Vec3s_ToVec3f(&vtx, &vtxList[COLPOLY_VTX_INDEX(poly->vtxData[i2])]);
                SkinMatrix_Vec3fMtxFMultXYZ(&srpMtx, &vtx, &polyVtx[i2]);
            }
            Math3D_SurfaceNorm(&polyVtx[0], &polyVtx[1], &polyVtx[2], &polyNorm);
            magnitude = Math3D_Vec3fMagnitude(&polyNorm);

            if (!IS_ZERO(magnitude)) {
                polyNorm.x *= 1.0f / magnitude;
                polyNorm.y *= 1.0f / magnitude;
                polyNorm.z *= 1.0f / magnitude;
                polyDist = -DOTXYZ(polyNorm, polyVtx[0]);
                if (Math3D_TriChkPointParaYIntersectInsideTri(
                        &polyVtx[0], &polyVtx[1], &polyVtx[2], polyNorm.x, polyNorm.y, polyNorm.z, polyDist,
                        dynaRaycast->pos->z, dynaRaycast->pos->x, &intersect, dynaRaycast->checkDist)) {
                    if (fabsf(intersect - result) < 1.0f) {

                        result = intersect;
                    }
                }
            }
        }
    }
    return result;
}
#else
#pragma GLOBAL_ASM("asm/non_matchings/code/z_bgcheck/BgCheck_RaycastFloorDyna.s")
#endif

/**
 * Performs collision detection on a BgActor's wall polys on sphere `pos`, `radius`
 * returns true if a collision was detected
 * `outX` `outZ` return the displaced x,z coordinates
 * `outPoly` returns the pointer to the nearest poly collided with, or NULL
 * `outBgId` returns `bgId` if the poly SurfaceType's wall damage flag is not set, else ?
 */
s32 BgCheck_SphVsDynaWallInBgActor(CollisionContext* colCtx, u16 xpFlags, DynaCollisionContext* dyna, SSList* ssList,
                                   f32* outX, f32* outZ, CollisionPoly** outPoly, s32* outBgId, Vec3f* pos, f32 radius,
                                   s32 bgId, Actor* actor) {
    f32 temp;
    f32 intersect;
    s32 result = false;
    CollisionPoly* poly;
    SSNode* curNode;
    f32 nx;
    f32 ny;
    f32 nz;
    Vec3f resultPos;
    s16 polyId;
    f32 zTemp;
    f32 xTemp;
    f32 normalXZ;
    f32 invNormalXZ;
    f32 planeDist;
    f32 temp_f18;
    f32 zMin;
    f32 zMax;
    f32 xMin;
    f32 xMax;

    if (ssList->head == SS_NULL) {
        return result;
    }

    resultPos = *pos;
    curNode = &dyna->polyNodes.tbl[ssList->head];

    while (true) {
        polyId = curNode->polyId;
        poly = &dyna->polyList[polyId];
        CollisionPoly_GetNormalF(poly, &nx, &ny, &nz);
        normalXZ = sqrtf(SQ(nx) + SQ(nz));

        planeDist = Math3D_DistPlaneToPos(nx, ny, nz, poly->dist, &resultPos);
        if (radius < fabsf(planeDist) || COLPOLY_VIA_FLAG_TEST(poly->flags_vIA, xpFlags) ||
            ((COLPOLY_VIA_FLAG_TEST(poly->flags_vIB, 4)) && ((actor != NULL && actor->category != ACTORCAT_PLAYER) ||
                                                             (actor == NULL && xpFlags != COLPOLY_IGNORE_CAMERA)))) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dyna->polyNodes.tbl[curNode->next];
                continue;
            }
        }
        invNormalXZ = 1.0f / normalXZ;
        temp_f18 = fabsf(nz) * invNormalXZ;
        if (temp_f18 < 0.4f) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dyna->polyNodes.tbl[curNode->next];
                continue;
            }
        }

        // compute poly zMin/zMax
        zTemp = dyna->vtxList[COLPOLY_VTX_INDEX(poly->flags_vIA)].z;
        zMax = zMin = zTemp;

        zTemp = dyna->vtxList[COLPOLY_VTX_INDEX(poly->flags_vIB)].z;
        if (zTemp < zMin) {
            zMin = zTemp;
        } else if (zTemp > zMax) {
            zMax = zTemp;
        }

        zTemp = dyna->vtxList[poly->vIC].z;
        if (zTemp < zMin) {
            zMin = zTemp;
        } else if (zMax < zTemp) {
            zMax = zTemp;
        }

        zMin -= radius;
        zMax += radius;
        if (resultPos.z < zMin || zMax < resultPos.z) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dyna->polyNodes.tbl[curNode->next];
                continue;
            }
        }
        if (CollisionPoly_CheckZIntersectApprox(poly, dyna->vtxList, resultPos.x, pos->y, &intersect)) {
            s32 pad;

            if (fabsf(intersect - resultPos.z) <= radius / temp_f18) {
                if ((intersect - resultPos.z) * nz <= 4.0f) {
                    if (BgCheck_ComputeWallDisplacement(colCtx, poly, &resultPos.x, &resultPos.z, nx, ny, nz,
                                                        invNormalXZ, planeDist, radius, outPoly)) {
                        *outBgId = bgId;
                    }
                    result = true;
                }
            }
        }
        if (curNode->next == SS_NULL) {
            break;
        }
        curNode = &dyna->polyNodes.tbl[curNode->next];
    }

    curNode = &dyna->polyNodes.tbl[ssList->head];
    while (true) {
        polyId = curNode->polyId;
        poly = &dyna->polyList[polyId];
        CollisionPoly_GetNormalF(poly, &nx, &ny, &nz);
        normalXZ = sqrtf(SQ(nx) + SQ(nz));

        planeDist = Math3D_DistPlaneToPos(nx, ny, nz, poly->dist, &resultPos);
        if (radius < fabsf(planeDist) || COLPOLY_VIA_FLAG_TEST(poly->flags_vIA, xpFlags) ||
            (COLPOLY_VIA_FLAG_TEST(poly->flags_vIB, 4) && ((actor != NULL && actor->category != ACTORCAT_PLAYER) ||
                                                           (actor == NULL && xpFlags != COLPOLY_IGNORE_CAMERA)))) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dyna->polyNodes.tbl[curNode->next];
                continue;
            }
        }

        invNormalXZ = 1.0f / normalXZ;
        temp_f18 = fabsf(nx) * invNormalXZ;
        if (temp_f18 < 0.4f) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dyna->polyNodes.tbl[curNode->next];
                continue;
            }
        }

        // compute poly xMin/xMax
        xTemp = dyna->vtxList[COLPOLY_VTX_INDEX(poly->flags_vIA)].x;
        xMax = xMin = xTemp;
        xTemp = dyna->vtxList[COLPOLY_VTX_INDEX(poly->flags_vIB)].x;

        if (xTemp < xMin) {
            xMin = xTemp;
        } else if (xMax < xTemp) {
            xMax = xTemp;
        }
        xTemp = dyna->vtxList[poly->vIC].x;
        if (xTemp < xMin) {
            xMin = xTemp;
        } else if (xMax < xTemp) {
            xMax = xTemp;
        }

        xMin -= radius;
        xMax += radius;
        if (resultPos.x < xMin || xMax < resultPos.x) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dyna->polyNodes.tbl[curNode->next];
                continue;
            }
        }

        if (CollisionPoly_CheckXIntersectApprox(poly, dyna->vtxList, pos->y, resultPos.z, &intersect)) {
            f32 xIntersectDist = intersect - resultPos.x;

            if (fabsf(xIntersectDist) <= radius / temp_f18) {
                if (xIntersectDist * nx <= 4.0f) {
                    if (BgCheck_ComputeWallDisplacement(colCtx, poly, &resultPos.x, &resultPos.z, nx, ny, nz,
                                                        invNormalXZ, planeDist, radius, outPoly)) {
                        *outBgId = bgId;
                    }
                    result = true;
                }
            }
        }
        if (curNode->next == SS_NULL) {
            break;
        }
        curNode = &dyna->polyNodes.tbl[curNode->next];
    }
    *outX = resultPos.x;
    *outZ = resultPos.z;
    return result;
}

/**
 * Performs collision detection on all dyna poly walls using sphere `pos`, `radius`
 * returns true if a collision was detected
 * `outX` `outZ` return the displaced x,z coordinates
 * `outPoly` returns the pointer to the nearest poly collided with, or NULL
 * `outBgId` returns the index of the BgActor that owns `outPoly`
 * If `actor` is not NULL, an BgActor bound to that actor will be ignored
 */
s32 BgCheck_SphVsDynaWall(CollisionContext* colCtx, u16 xpFlags, f32* outX, f32* outZ, Vec3f* pos, f32 radius,
                          CollisionPoly** outPoly, s32* outBgId, Actor* actor) {
    Vec3f resultPos;
    s32 result;
    f32 r;
    f32 dz;
    f32 dx;
    BgActor* bgActor;
    s32 i;

    result = false;
    resultPos = *pos;

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        if (!(colCtx->dyna.bgActorFlags[i] & 1) || (colCtx->dyna.bgActorFlags[i] & 2)) {
            continue;
        }
        if ((colCtx->dyna.bgActors + i)->actor == actor) {
            continue;
        }
        bgActor = &colCtx->dyna.bgActors[i];

        if (bgActor->minY > resultPos.y || bgActor->maxY < resultPos.y) {
            continue;
        }

        bgActor->boundingSphere.radius += (s16)radius;

        r = bgActor->boundingSphere.radius;
        dx = bgActor->boundingSphere.center.x - resultPos.x;
        dz = bgActor->boundingSphere.center.z - resultPos.z;
        if (SQ(r) < (SQ(dx) + SQ(dz)) || (!Math3D_XYInSphere(&bgActor->boundingSphere, resultPos.x, resultPos.y) &&
                                          !Math3D_YZInSphere(&bgActor->boundingSphere, resultPos.y, resultPos.z))) {
            bgActor->boundingSphere.radius -= (s16)radius;
            continue;
        }
        bgActor->boundingSphere.radius -= (s16)radius;
        if (BgCheck_SphVsDynaWallInBgActor(colCtx, xpFlags, &colCtx->dyna,
                                           &(colCtx->dyna.bgActors + i)->dynaLookup.wall, outX, outZ, outPoly, outBgId,
                                           &resultPos, radius, i, actor)) {
            resultPos.x = *outX;
            resultPos.z = *outZ;
            result = true;
        }
    }
    return result;
}

/**
 * Tests for collision with a dyna poly ceiling, starting at `ssList`
 * returns true if a collision occurs, else false
 * `outPoly` returns the poly collided with
 * `outY` returns the y coordinate needed to not collide with `outPoly`
 */
s32 BgCheck_CheckDynaCeilingList(CollisionContext* colCtx, u16 xpFlags, DynaCollisionContext* dyna, SSList* ssList,
                                 f32* outY, Vec3f* pos, f32 checkHeight, CollisionPoly** outPoly, Actor* actor,
                                 s32 bgId) {
    s16 polyId;
    SSNode* curNode;
    CollisionPoly* poly;
    Vec3f testPos;
    f32 ceilingY;
    f32 sign;
    f32 nx;
    f32 ny;
    f32 nz;
    s32 result = false;
    f32 intersectDist;
    u16 padding;

    if (ssList->head == SS_NULL) {
        return false;
    }
    curNode = &dyna->polyNodes.tbl[ssList->head];
    testPos = *pos;

    while (true) {
        polyId = curNode->polyId;
        poly = &dyna->polyList[polyId];
        if (COLPOLY_VIA_FLAG_TEST(poly->flags_vIA, xpFlags) ||
            (COLPOLY_VIA_FLAG_TEST(poly->flags_vIB, 4) && ((actor != NULL && actor->category != ACTORCAT_PLAYER) ||
                                                           (actor == NULL && xpFlags != COLPOLY_IGNORE_CAMERA)))) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dyna->polyNodes.tbl[curNode->next];
                continue;
            }
        }
        CollisionPoly_GetNormalF(poly, &nx, &ny, &nz);
        if (checkHeight < Math3D_UDistPlaneToPos(nx, ny, nz, poly->dist, &testPos)) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dyna->polyNodes.tbl[curNode->next];
                continue;
            }
        }
        if (CollisionPoly_CheckYIntersectApprox2(poly, dyna->vtxList, testPos.x, testPos.z, &ceilingY)) {
            intersectDist = ceilingY - testPos.y;
            if (testPos.y < ceilingY && intersectDist < checkHeight && intersectDist * ny <= 0.0f) {
                sign = (0.0f <= ny) ? 1.0f : -1.0f;
                testPos.y = (sign * checkHeight) + ceilingY;
                result = true;
                *outPoly = poly;
            }
        }
        if (curNode->next == SS_NULL) {
            break;
        } else {
            curNode = &dyna->polyNodes.tbl[curNode->next];
            continue;
        }
    }
    *outY = testPos.y;
    return result;
}

/**
 * Tests collision with a dyna poly ceiling
 * returns true if a collision occurs, else false
 * `outPoly` returns the poly collided with, while `outBgId` returns the id of the BgActor that owns the poly
 * `outY` returns the y coordinate needed to not collide with `outPoly`, or `pos`.y + `checkDist` if no collision occurs
 */
s32 BgCheck_CheckDynaCeiling(CollisionContext* colCtx, u16 xpFlags, f32* outY, Vec3f* pos, f32 checkDist,
                             CollisionPoly** outPoly, s32* outBgId, Actor* actor) {
    s32 i;
    s32 result = false;
    f32 resultY;
    f32 tempY = checkDist + pos->y;
    BgActor* bgActor;
    CollisionPoly* poly;

    resultY = tempY;

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        if (!(colCtx->dyna.bgActorFlags[i] & 1) || (colCtx->dyna.bgActorFlags[i] & 2)) {
            continue;
        }
        if (actor == colCtx->dyna.bgActors[i].actor) {
            continue;
        }
        if (!Math3D_XZInSphere(&colCtx->dyna.bgActors[i].boundingSphere, pos->x, pos->z)) {
            continue;
        }
        if (BgCheck_CheckDynaCeilingList(colCtx, xpFlags, &colCtx->dyna, &colCtx->dyna.bgActors[i].dynaLookup.ceiling,
                                         &tempY, pos, checkDist, &poly, actor, i) &&
            tempY < resultY) {

            resultY = tempY;
            *outPoly = poly;
            *outBgId = i;
            result = true;
        }
    }
    *outY = resultY;
    return result;
}

/**
 * Tests if DynaLineTest intersects with a poly
 * returns true if a poly was intersected, else false
 */
s32 BgCheck_CheckLineAgainstBgActorSSList(DynaLineTest* dynaLineTest) {
    f32 distSq;
    s32 result;
    SSNode* curNode;
    Vec3f polyIntersect;
    s16 polyId;
    BgLineVsPolyTest test;

    result = false;
    if (dynaLineTest->ssList->head == SS_NULL) {
        return result;
    }
    curNode = &dynaLineTest->dyna->polyNodes.tbl[dynaLineTest->ssList->head];

    test.vtxList = dynaLineTest->dyna->vtxList;
    test.posA = dynaLineTest->posA;
    test.posB = dynaLineTest->posB;
    test.planeIntersect = &polyIntersect;
    test.checkOneFace = dynaLineTest->checkOneFace;
    test.checkDist = dynaLineTest->checkDist;

    while (true) {
        polyId = curNode->polyId;
        test.poly = &dynaLineTest->dyna->polyList[polyId];
        if (COLPOLY_VIA_FLAG_TEST(test.poly->flags_vIA, dynaLineTest->xpFlags) ||
            (COLPOLY_VIA_FLAG_TEST(test.poly->flags_vIB, 4) &&
             ((dynaLineTest->actor != NULL && dynaLineTest->actor->category != ACTORCAT_PLAYER) ||
              (dynaLineTest->actor == NULL && dynaLineTest->xpFlags != COLPOLY_IGNORE_CAMERA)))) {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dynaLineTest->dyna->polyNodes.tbl[curNode->next];
            }
        } else {
            if (CollisionPoly_LineVsPoly(&test)) {
                distSq = Math3D_Vec3fDistSq(dynaLineTest->posA, test.planeIntersect);
                if (distSq < *dynaLineTest->distSq) {
                    *dynaLineTest->distSq = distSq;
                    *dynaLineTest->posResult = *test.planeIntersect;
                    *dynaLineTest->posB = *test.planeIntersect;
                    *dynaLineTest->resultPoly = test.poly;
                    result = true;
                }
            }
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dynaLineTest->dyna->polyNodes.tbl[curNode->next];
            }
        }
    }
    return result;
}

/**
 * Tests if line `posA` `posB` intersects with a dyna poly within BgActor `bgId`
 * `distSq` is the maximum squared distance to check for a collision
 * returns true if an intersection occurred, else false
 * `posB`? and `posResult` return the point of intersection
 * `outPoly` returns the poly intersected
 * `distSq` returns the squared distance of the intersection
 */
s32 BgCheck_CheckLineAgainstBgActor(CollisionContext* colCtx, u16 xpFlags, Vec3f* posA, Vec3f* posB, Vec3f* posResult,
                                    CollisionPoly** outPoly, f32* distSq, s32 bgId, f32 checkDist, s32 bccFlags,
                                    Actor* actor) {
    s32 result = false;
    DynaLineTest dynaLineTest;

    dynaLineTest.colCtx = colCtx;
    dynaLineTest.xpFlags = xpFlags;
    dynaLineTest.dyna = &colCtx->dyna;
    dynaLineTest.posA = posA;
    dynaLineTest.posB = posB;
    dynaLineTest.posResult = posResult;
    dynaLineTest.resultPoly = outPoly;
    dynaLineTest.checkOneFace = (bccFlags & BGCHECK_CHECK_ONE_FACE) != 0;
    dynaLineTest.distSq = distSq;
    dynaLineTest.checkDist = checkDist;
    dynaLineTest.actor = actor;
    dynaLineTest.bgId = bgId;

    dynaLineTest.ssList = &colCtx->dyna.bgActors[bgId].dynaLookup.wall;
    if (bccFlags & BGCHECK_CHECK_WALL) {
        if (BgCheck_CheckLineAgainstBgActorSSList(&dynaLineTest)) {
            result = true;
        }
    }
    dynaLineTest.ssList = &colCtx->dyna.bgActors[bgId].dynaLookup.floor;
    if (bccFlags & BGCHECK_CHECK_FLOOR) {
        if (BgCheck_CheckLineAgainstBgActorSSList(&dynaLineTest)) {
            result = true;
        }
    }
    dynaLineTest.ssList = &colCtx->dyna.bgActors[bgId].dynaLookup.ceiling;
    if (bccFlags & BGCHECK_CHECK_CEILING) {
        if (BgCheck_CheckLineAgainstBgActorSSList(&dynaLineTest)) {
            result = true;
        }
    }
    return result;
}

/**
 * Tests if line from `posA` to `posB` passes through a dyna poly.
 * returns true if so, otherwise false
 * `outPoly` returns the pointer of the poly intersected.
 * `outBgId` returns the BgActor index of the poly
 */
s32 BgCheck_CheckLineAgainstDyna(CollisionContext* colCtx, u16 xpFlags, Vec3f* posA, Vec3f* posB, Vec3f* posResult,
                                 CollisionPoly** outPoly, f32* distSq, s32* outBgId, Actor* actor, f32 checkDist,
                                 s32 bccFlags) {
    s32 pad;
    s32 i;
    s32 result = false;
    LineSegment line;
    f32 ay;
    f32 by;

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        if ((colCtx->dyna.bgActorFlags[i] & 1) && !(colCtx->dyna.bgActorFlags[i] & 2)) {
            if (actor != colCtx->dyna.bgActors[i].actor) {
                ay = posA->y;
                by = posB->y;
                if (!(ay < colCtx->dyna.bgActors[i].minY) || !(by < colCtx->dyna.bgActors[i].minY)) {
                    if (!(colCtx->dyna.bgActors[i].maxY < ay) || !(colCtx->dyna.bgActors[i].maxY < by)) {
                        line.a = *posA;
                        line.b = *posB;
                        if (Math3D_LineVsSph(&colCtx->dyna.bgActors[i].boundingSphere, &line) != 0) {
                            if (BgCheck_CheckLineAgainstBgActor(colCtx, xpFlags, posA, posB, posResult, outPoly, distSq,
                                                                i, checkDist, bccFlags, actor)) {
                                *outBgId = i;
                                result = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

/**
 * Get first dyna poly intersecting sphere `center` `radius` from list `ssList`
 * returns true if any poly intersects the sphere, else returns false
 * `outPoly` returns the pointer of the first poly found that intersects
 */
s32 BgCheck_SphVsFirstDynaPolyList(CollisionContext* colCtx, u16 xpFlags, CollisionPoly** outPoly, Vec3f* center,
                                   f32 radius, SSList* ssList, Actor* actor, s32 bgId) {
    CollisionPoly* curPoly;
    DynaCollisionContext* dyna;
    SSNode* curNode;
    s32 curPolyId;

    if (ssList->head == SS_NULL) {
        return false;
    }
    dyna = &colCtx->dyna;
    curNode = &dyna->polyNodes.tbl[ssList->head];
    while (true) {
        curPolyId = curNode->polyId;
        curPoly = &dyna->polyList[curPolyId];
        if (COLPOLY_VIA_FLAG_TEST(curPoly->flags_vIA, xpFlags) ||
            (COLPOLY_VIA_FLAG_TEST(curPoly->flags_vIB, 4) && ((actor != NULL && actor->category != ACTORCAT_PLAYER) ||
                                                              (actor == NULL && xpFlags != COLPOLY_IGNORE_CAMERA))))

        {
            if (curNode->next == SS_NULL) {
                break;
            } else {
                curNode = &dyna->polyNodes.tbl[curNode->next];
                continue;
            }
        }
        if (CollisionPoly_SphVsPoly(curPoly, dyna->vtxList, center, radius)) {
            *outPoly = curPoly;
            return true;
        }
        if (curNode->next == SS_NULL) {
            break;
        } else {
            curNode = &dyna->polyNodes.tbl[curNode->next];
            continue;
        }
    }
    return false;
}

/**
 * Get first dyna poly intersecting sphere `center` `radius` from BgActor `bgId`
 * returns true if any poly intersects the sphere, else false
 * `outPoly` returns the pointer of the first poly found that intersects
 */
s32 BgCheck_SphVsFirstDynaPolyInBgActor(CollisionContext* colCtx, u16 xpFlags, CollisionPoly** outPoly, Vec3f* center,
                                        f32 radius, s32 bgId, u16 bciFlags, Actor* actor) {
    if (!(bciFlags & BGCHECK_IGNORE_CEILING)) {
        if (BgCheck_SphVsFirstDynaPolyList(colCtx, xpFlags, outPoly, center, radius,
                                           &colCtx->dyna.bgActors[bgId].dynaLookup.ceiling, actor, bgId)) {
            return true;
        }
    }
    if (!(bciFlags & BGCHECK_IGNORE_WALL)) {
        if (BgCheck_SphVsFirstDynaPolyList(colCtx, xpFlags, outPoly, center, radius,
                                           &colCtx->dyna.bgActors[bgId].dynaLookup.wall, actor, bgId)) {
            return true;
        }
    }
    if (!(bciFlags & BGCHECK_IGNORE_FLOOR)) {
        if (BgCheck_SphVsFirstDynaPolyList(colCtx, xpFlags, outPoly, center, radius,
                                           &colCtx->dyna.bgActors[bgId].dynaLookup.floor, actor, bgId)) {
            return true;
        }
    }
    return false;
}

/**
 * Gets first dyna poly intersecting sphere `center` `radius`
 * returns true if poly detected, else false
 * `outPoly` returns the first intersecting poly, while `outBgId` returns the BgActor index of that poly
 */
s32 BgCheck_SphVsFirstDynaPoly(CollisionContext* colCtx, u16 xpFlags, CollisionPoly** outPoly, s32* outBgId,
                               Vec3f* center, f32 radius, Actor* actor, u16 bciFlags) {
    s32 i = 0;
    Sphere16 testSphere;

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        if (!(colCtx->dyna.bgActorFlags[i] & 1) || (colCtx->dyna.bgActorFlags[i] & 2)) {
            continue;
        }
        if (colCtx->dyna.bgActors[i].actor == actor) {
            continue;
        }
        testSphere.center.x = center->x;
        testSphere.center.y = center->y;
        testSphere.center.z = center->z;
        testSphere.radius = radius;
        if (!Math3D_SphVsSph(&testSphere, &colCtx->dyna.bgActors[i].boundingSphere)) {
            continue;
        }
        if (BgCheck_SphVsFirstDynaPolyInBgActor(colCtx, xpFlags, outPoly, center, radius, i, bciFlags, actor)) {
            return true;
        }
    }
    return false;
}

/**
 * SEGMENTED_TO_VIRTUAL CollisionHeader members
 */
void CollisionHeader_SegmentedToVirtual(CollisionHeader* colHeader) {
    colHeader->vtxList = Lib_SegmentedToVirtual(colHeader->vtxList);
    colHeader->polyList = Lib_SegmentedToVirtual(colHeader->polyList);
    if (colHeader->surfaceTypeList) {
        colHeader->surfaceTypeList = Lib_SegmentedToVirtual(colHeader->surfaceTypeList);
    }
    if (colHeader->cameraDataList) {
        colHeader->cameraDataList = Lib_SegmentedToVirtual(colHeader->cameraDataList);
    }
    if (colHeader->waterBoxes) {
        colHeader->waterBoxes = Lib_SegmentedToVirtual(colHeader->waterBoxes);
    }
}

/**
 * Convert CollisionHeader Segmented to Virtual addressing
 */
void CollisionHeader_GetVirtual(CollisionHeader* colHeader, CollisionHeader** dest) {
    *dest = Lib_SegmentedToVirtual(colHeader);
    CollisionHeader_SegmentedToVirtual(*dest);
}

/**
 * SEGMENT_TO_VIRTUAL all active BgActor CollisionHeaders
 */
void BgCheck_InitCollisionHeaders(CollisionContext* colCtx, GlobalContext* globalCtx) {
    DynaCollisionContext* dyna = &colCtx->dyna;
    s32 i;
    u16 flag;

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        flag = dyna->bgActorFlags[i];
        if ((flag & 1) && !(flag & 2)) {
            Actor_SetObjectSegment(globalCtx, dyna->bgActors[i].actor);
            CollisionHeader_SegmentedToVirtual(dyna->bgActors[i].colHeader);
        }
    }
}

/**
 * Reset SSNodeList polyCheckTbl
 */
void BgCheck_ResetPolyCheckTbl(SSNodeList* nodeList, s32 numPolys) {
    u8* end = nodeList->polyCheckTbl + numPolys;
    u8* t;

    for (t = nodeList->polyCheckTbl; t < end; t++) {
        *t = 0;
    }
}

/**
 * Get SurfaceType property set
 */
u32 SurfaceType_GetData(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId, s32 dataIdx) {
    CollisionHeader* colHeader;
    SurfaceType* surfaceTypes;

    colHeader = BgCheck_GetCollisionHeader(colCtx, bgId);
    if (colHeader == NULL || poly == NULL) {
        return 0;
    }
    surfaceTypes = colHeader->surfaceTypeList;
    if (surfaceTypes == NULL) {
        return 0;
    }
    return surfaceTypes[poly->type].data[dataIdx];
}

/**
 * SurfaceType return CamData Index
 */
u32 SurfaceType_GetCamDataIndex(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 0) & 0xFF;
}

/**
 * CamData return camera setting
 */
u16 func_800C9728(CollisionContext* colCtx, u32 camId, s32 bgId) {
    u16 result;
    CollisionHeader* colHeader;
    CamData* camData;

    colHeader = BgCheck_GetCollisionHeader(colCtx, bgId);
    if (colHeader == NULL) {
        return 0;
    }
    camData = colHeader->cameraDataList;
    result = camData[camId].cameraSType;
    return result;
}

/**
 * SurfaceType return camera setting
 */
u16 SurfaceType_GetCameraSetting(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    CollisionHeader* colHeader = BgCheck_GetCollisionHeader(colCtx, bgId);
    CamData* camData;
    SurfaceType* surfaceTypes;

    if (colHeader == NULL) {
        return 0;
    }
    camData = colHeader->cameraDataList;
    if (camData == NULL) {
        return 0;
    }
    surfaceTypes = colHeader->surfaceTypeList;
    if (surfaceTypes == NULL) {
        return 0;
    }
    return func_800C9728(colCtx, SurfaceType_GetCamDataIndex(colCtx, poly, bgId), bgId);
}

/**
 * CamData get number of camera data blocks
 */
u16 func_800C97F8(CollisionContext* colCtx, u32 camId, s32 bgId) {
    CollisionHeader* colHeader = BgCheck_GetCollisionHeader(colCtx, bgId);
    CamData* camData;

    if (colHeader == NULL) {
        return 0;
    }

    camData = colHeader->cameraDataList;
    if (camData == NULL) {
        return 0;
    }
    return camData[camId].unk_02;
}

/**
 * SurfaceType get number of camera data blocks
 */
u16 func_800C9844(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    CollisionHeader* colHeader = BgCheck_GetCollisionHeader(colCtx, bgId);
    CamData* camData;
    SurfaceType* surfaceTypes;

    if (colHeader == NULL) {
        return 0;
    }
    camData = colHeader->cameraDataList;
    if (camData == NULL) {
        return 0;
    }
    surfaceTypes = colHeader->surfaceTypeList;
    if (surfaceTypes == NULL) {
        return 0;
    }
    return func_800C97F8(colCtx, SurfaceType_GetCamDataIndex(colCtx, poly, bgId), bgId);
}

/**
 * CamData get camPosData
 */
Vec3s* func_800C98CC(CollisionContext* colCtx, s32 camId, s32 bgId) {
    CollisionHeader* colHeader = BgCheck_GetCollisionHeader(colCtx, bgId);
    CamData* cameraDataList;

    if (colHeader == NULL) {
        return NULL;
    }
    cameraDataList = colHeader->cameraDataList;
    if (cameraDataList == NULL) {
        return NULL;
    }
    return Lib_SegmentedToVirtual(cameraDataList[camId].camPosData);
}

/**
 * SurfaceType Get camPosData
 */
Vec3s* SurfaceType_GetCamPosData(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    CollisionHeader* colHeader = BgCheck_GetCollisionHeader(colCtx, bgId);
    CamData* camData;
    SurfaceType* surfaceTypes;

    if (colHeader == NULL) {
        return NULL;
    }
    camData = colHeader->cameraDataList;
    if (camData == NULL) {
        return NULL;
    }
    surfaceTypes = colHeader->surfaceTypeList;
    if (surfaceTypes == NULL) {
        return NULL;
    }
    return func_800C98CC(colCtx, SurfaceType_GetCamDataIndex(colCtx, poly, bgId), bgId);
}

/**
 * SurfaceType Get Scene Exit Index
 */
u32 SurfaceType_GetSceneExitIndex(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 0) >> 8 & 0x1F;
}

/**
 * SurfaceType Get ? Property (& 0x0003_E000)
 */
u32 func_800C99D4(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 0) >> 13 & 0x1F;
}

/**
 * SurfaceType Get ? Property (& 0x001C_0000)
 */
u32 func_800C99FC(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 0) >> 18 & 7;
}

/**
 * SurfaceType Get Wall Property (Internal)
 */
u32 func_800C9A24(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 0) >> 21 & 0x1F;
}

/**
 * SurfaceType Get Wall Flags
 */
s32 func_800C9A4C(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return sWallFlags[func_800C9A24(colCtx, poly, bgId)];
}

/**
 * SurfaceType Is Wall Flag (1 << 0) Set
 */
s32 func_800C9A7C(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return (func_800C9A4C(colCtx, poly, bgId) & 1) ? true : false;
}

/**
 * SurfaceType Is Wall Flag (1 << 1) Set
 */
s32 func_800C9AB0(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return (func_800C9A4C(colCtx, poly, bgId) & 2) ? true : false;
}

/**
 * SurfaceType Is Wall Flag (1 << 2) Set
 */
s32 func_800C9AE4(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return (func_800C9A4C(colCtx, poly, bgId) & 4) ? true : false;
}

/**
 * unused
 */
u32 func_800C9B18(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 0) >> 26 & 0xF;
}

/**
 * SurfaceType Get Floor Property
 */
u32 func_800C9B40(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 0) >> 26 & 0xF;
}

/**
 * SurfaceType Is Floor Minus 1
 */
u32 func_800C9B68(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 0) >> 30 & 1;
}

/**
 * SurfaceType Is Horse Blocked
 */
u32 SurfaceType_IsHorseBlocked(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 0) >> 31 & 1;
}

u32 func_800C9BB8(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 1) & 0xF;
}

/**
 * SurfaceType Get Poly Sfx
 */
u16 SurfaceType_GetSfx(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    s32 id = func_800C9BB8(colCtx, poly, bgId);

    if (id < 0 || id > 14) {
        return NA_SE_PL_WALK_GROUND - SFX_FLAG;
    }
    return sSurfaceTypeSfx[id];
}

/**
 * SurfaceType Get ? (same indexer as Get Poly Sfx)
 */
s32 func_800C9C24(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId, s32 arg3) {
    s32 id = func_800C9BB8(colCtx, poly, bgId);

    if (id < 0 || id > 14) {
        return 0;
    }
    return D_801B46C0[id] & arg3;
}

/**
 * SurfaceType get terrain slope surface
 */
u32 SurfaceType_GetSlope(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 1) >> 4 & 3;
}

/**
 * SurfaceType get surface lighting setting
 */
u32 SurfaceType_GetLightSettingIndex(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 1) >> 6 & 0x1F;
}

/**
 * SurfaceType get echo
 */
u32 SurfaceType_GetEcho(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 1) >> 11 & 0x3F;
}

/**
 * SurfaceType Is Hookshot Surface
 */
u32 SurfaceType_IsHookshotSurface(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 1) >> 17 & 1;
}

/**
 * CollisionPoly is ignored by entities
 * Returns true if poly is ignored by entities, else false
 */
s32 SurfaceType_IsIgnoredByEntities(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    u32 flags;

    if (BgCheck_GetCollisionHeader(colCtx, bgId) == NULL) {
        return true;
    }
    flags = poly->flags_vIA & 0x4000;
    return !!flags;
}

/**
 * CollisionPoly is ignored by projectiles
 * Returns true if poly is ignored by projectiles, else false
 */
s32 SurfaceType_IsIgnoredByProjectiles(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    u32 flags;

    if (BgCheck_GetCollisionHeader(colCtx, bgId) == NULL) {
        return true;
    }
    flags = poly->flags_vIA & 0x8000;
    return !!flags;
}

/**
 * CollisionPoly is conveyor enabled
 * Returns true if `poly` is a conveyor surface, else false
 */
s32 SurfaceType_IsConveyor(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    u32 flags;

    if (BgCheck_GetCollisionHeader(colCtx, bgId) == NULL) {
        return true;
    }
    if (poly == NULL) {
        return false;
    }
    flags = poly->flags_vIB & 0x2000;
    return !!flags;
}

s32 func_800C9DDC(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    u32 flags;

    if (BgCheck_GetCollisionHeader(colCtx, bgId) == NULL) {
        return true;
    }
    flags = poly->flags_vIB & 0x4000;
    return !!flags;
}

/**
 * SurfaceType Get Conveyor Surface Speed
 */
u32 SurfaceType_GetConveyorSpeed(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return SurfaceType_GetData(colCtx, poly, bgId, 1) >> 18 & 7;
}

/**
 * SurfaceType Get Conveyor Direction
 * returns a value between 0-63, representing 360 / 64 degrees of rotation
 */
u32 SurfaceType_GetConveyorDirection(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    u32 data;

    data = SurfaceType_GetData(colCtx, poly, bgId, 1) >> 21;
    if (colCtx->flags & 1) {
        data += 0x20;
    }
    return data & 0x3F;
}

/**
 * SurfaceType is Wall Damage
 */
u32 SurfaceType_IsWallDamage(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return (SurfaceType_GetData(colCtx, poly, bgId, 1) & 0x8000000) ? true : false;
}

/**
 * Internal. Get the water surface at point (`x`, `ySurface`, `z`). `ySurface` doubles as position y input
 * returns true if point is within the xz boundaries of an active water box, else false
 * `ySurface` returns the water box's surface, while `outWaterBox` returns a pointer to the WaterBox
 */
s32 WaterBox_GetSurfaceImpl(GlobalContext* globalCtx, CollisionContext* colCtx, f32 x, f32 z, f32* ySurface,
                            WaterBox** outWaterBox, s32* bgId) {
    CollisionHeader* colHeader;
    u32 room;
    WaterBox* curWaterBox;
    s32 i;

    *bgId = BGCHECK_SCENE;
    colHeader = colCtx->colHeader;

    if (colHeader->numWaterBoxes != 0 && colHeader->waterBoxes != NULL) {
        for (curWaterBox = colHeader->waterBoxes; curWaterBox < colHeader->waterBoxes + colHeader->numWaterBoxes;
             curWaterBox++) {
            room = 0x3F & (curWaterBox->properties >> 13);
            if (room == (u32)globalCtx->roomCtx.currRoom.num || room == 0x3F) {
                if (curWaterBox->properties & 0x80000) {
                    continue;
                }
                if (curWaterBox->minPos.x < x && x < curWaterBox->minPos.x + curWaterBox->xLength) {
                    if (curWaterBox->minPos.z < z && z < curWaterBox->minPos.z + curWaterBox->zLength) {
                        *outWaterBox = curWaterBox;
                        *ySurface = curWaterBox->minPos.y;
                        return true;
                    }
                }
            }
        }
    }

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        if (colCtx->dyna.bgActorFlags[i] & 1) {
            BgActor* bgActor;
            if (colCtx->dyna.bgActorFlags[i] & 4) {
                continue;
            }
            bgActor = &colCtx->dyna.bgActors[i];
            if (bgActor->colHeader->numWaterBoxes != 0 && bgActor->colHeader->waterBoxes != NULL) {
                for (curWaterBox = colCtx->dyna.waterBoxList.boxes + bgActor->waterboxesStartIndex;
                     curWaterBox < colCtx->dyna.waterBoxList.boxes + bgActor->waterboxesStartIndex +
                                       bgActor->colHeader->numWaterBoxes;
                     curWaterBox++) {
                    if (curWaterBox->properties & 0x80000) {
                        continue;
                    }
                    if (curWaterBox->minPos.x < x && x < curWaterBox->minPos.x + curWaterBox->xLength) {
                        if (curWaterBox->minPos.z < z && z < curWaterBox->minPos.z + curWaterBox->zLength) {
                            *outWaterBox = curWaterBox;
                            *ySurface = curWaterBox->minPos.y;
                            *bgId = i;
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

s32 WaterBox_GetSurface1(GlobalContext* globalCtx, CollisionContext* colCtx, f32 x, f32 z, f32* ySurface,
                         WaterBox** outWaterBox) {
    WaterBox_GetSurface1_2(globalCtx, colCtx, x, z, ySurface, outWaterBox);
}

s32 WaterBox_GetSurface1_2(GlobalContext* globalCtx, CollisionContext* colCtx, f32 x, f32 z, f32* ySurface,
                           WaterBox** outWaterBox) {
    s32 bgId;
    return WaterBox_GetSurfaceImpl(globalCtx, colCtx, x, z, ySurface, outWaterBox, &bgId);
}

#ifdef NON_MATCHING
/**
 * Gets the first active WaterBox at `pos` where WaterBox.properties & 0x80000 == 0
 * `surfaceCheckDist` is the absolute y distance from the water surface to check
 * returns the index of the waterbox found, or -1 if no waterbox is found
 * `outWaterBox` returns the pointer to the waterbox found, or NULL if none is found
 */
s32 WaterBox_GetSurface2(GlobalContext* globalCtx, CollisionContext* colCtx, Vec3f* pos, f32 surfaceCheckDist,
                         WaterBox** outWaterBox, s32* bgId) {
    CollisionHeader* colHeader;
    s32 room;
    s32 i;
    WaterBox* waterBox;

    *bgId = BGCHECK_SCENE;
    colHeader = colCtx->colHeader;

    //! @bug: check skips testing BgActor waterboxes
    if (colHeader->numWaterBoxes == 0 || colHeader->waterBoxes == NULL) {
        *outWaterBox = NULL;
        return -1;
    }

    for (i = 0; i < colHeader->numWaterBoxes; i++) {
        waterBox = &colHeader->waterBoxes[i];

        room = WATERBOX_ROOM(waterBox->properties);
        if (!(room == globalCtx->roomCtx.currRoom.num || room == 0x3F)) {
            continue;
        }
        if ((waterBox->properties & 0x80000)) {
            continue;
        }
        if ((waterBox->minPos.x < pos->x && pos->x < waterBox->minPos.x + waterBox->xLength)) {
            if ((waterBox->minPos.z < pos->z && pos->z < waterBox->minPos.z + waterBox->zLength)) {
                if (pos->y - surfaceCheckDist < waterBox->minPos.y && waterBox->minPos.y < pos->y + surfaceCheckDist) {
                    *outWaterBox = waterBox;
                    return i;
                }
            }
        }
    }

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        WaterBox* iterator;
        WaterBox* start;
        BgActor* bgActor;
        if (!(colCtx->dyna.bgActorFlags[i] & 1) || (colCtx->dyna.bgActorFlags[i] & 4) ||
            (colCtx->dyna.bgActorFlags[i] & 2)) {
            continue;
        }
        bgActor = &colCtx->dyna.bgActors[i];
        for (iterator = colCtx->dyna.waterBoxList.boxes + bgActor->waterboxesStartIndex;
             iterator <
             colCtx->dyna.waterBoxList.boxes + bgActor->waterboxesStartIndex + bgActor->colHeader->numWaterBoxes;
             iterator++) {
            if (iterator->properties & 0x80000) {
                continue;
            }
            if ((iterator->minPos.x < pos->x) && pos->x < iterator->minPos.x + iterator->xLength) {
                if ((iterator->minPos.z < pos->z) && pos->z < iterator->minPos.z + iterator->zLength) {
                    if (pos->y - surfaceCheckDist < iterator->minPos.y &&
                        iterator->minPos.y < pos->y + surfaceCheckDist) {
                        *bgId = i;
                        *outWaterBox = iterator;
                        return i;
                    }
                }
            }
        }
    }

    *outWaterBox = NULL;
    return -1;
}
#else
#pragma GLOBAL_ASM("asm/non_matchings/code/z_bgcheck/WaterBox_GetSurface2.s")
#endif

f32 func_800CA568(CollisionContext* colCtx, s32 waterBoxId, s32 bgId) {
    CollisionHeader* colHeader;

    colHeader = BgCheck_GetCollisionHeader(colCtx, bgId);
    if ((colHeader == NULL) || (waterBoxId < 0) || (waterBoxId >= (s32)colHeader->numWaterBoxes)) {
        return 0.0f;
    }
    if (bgId == BGCHECK_SCENE) {
        return colCtx->colHeader->waterBoxes[waterBoxId].minPos.y;
    }
    return (&colCtx->dyna.waterBoxList.boxes[colCtx->dyna.bgActors[bgId].waterboxesStartIndex])[waterBoxId].minPos.y;
}

/**
 * WaterBox get CamData index
 */
u32 WaterBox_GetCamDataIndex(CollisionContext* colCtx, WaterBox* waterBox) {
    u32 prop = waterBox->properties >> 0;

    return prop & 0xFF;
}

/**
 * WaterBox get CamData camera setting
 */
u16 WaterBox_GetCameraSetting(CollisionContext* colCtx, WaterBox* waterBox, s32 bgId) {
    s32 pad[2];
    u16 result = 0;
    CollisionHeader* colHeader;
    CamData* camData;
    s32 camId;

    colHeader = BgCheck_GetCollisionHeader(colCtx, bgId);
    if (colHeader == NULL) {
        return result;
    }
    camId = WaterBox_GetCamDataIndex(colCtx, waterBox);
    camData = colHeader->cameraDataList;

    if (camData == NULL) {
        return result;
    }

    result = camData[camId].cameraSType;
    return result;
}

void func_800CA6B8(CollisionContext* colCtx, WaterBox* waterBox) {
    WaterBox_GetCameraSetting(colCtx, waterBox, BGCHECK_SCENE);
}

/**
 * WaterBox get lighting settings
 */
u32 WaterBox_GetLightSettingIndex(CollisionContext* colCtx, WaterBox* waterBox) {
    u32 prop = waterBox->properties >> 8;

    return prop & 0x1F;
}

#ifdef NON_MATCHING
/**
 * Get the water surface at point (`x`, `ySurface`, `z`). `ySurface` doubles as position y input
 * same as WaterBox_GetSurfaceImpl, but tests if WaterBox properties & 0x80000 != 0
 * returns true if point is within the xz boundaries of an active water box, else false
 * `ySurface` returns the water box's surface, while `outWaterBox` returns a pointer to the WaterBox
 */
s32 func_800CA6F0(GlobalContext* globalCtx, CollisionContext* colCtx, f32 x, f32 z, f32* ySurface,
                  WaterBox** outWaterBox, s32* bgId) {
    CollisionHeader* colHeader;
    u32 room;
    WaterBox* curWaterBox;
    s32 i;
    BgActor* bgActor;

    colHeader = colCtx->colHeader;
    *outWaterBox = NULL;
    *bgId = BGCHECK_SCENE;

    if (colHeader->numWaterBoxes == 0 || colHeader->waterBoxes == NULL) {
        return false;
    }
    for (curWaterBox = colHeader->waterBoxes; curWaterBox < colHeader->waterBoxes + colHeader->numWaterBoxes;
         curWaterBox++) {
        room = WATERBOX_ROOM(curWaterBox->properties);
        if (room == (u32)globalCtx->roomCtx.currRoom.num || room == 0x3F) {
            if ((curWaterBox->properties & 0x80000) != 0) {
                if (curWaterBox->minPos.x < x && x < curWaterBox->minPos.x + curWaterBox->xLength) {
                    if (curWaterBox->minPos.z < z && z < curWaterBox->minPos.z + curWaterBox->zLength) {
                        *outWaterBox = curWaterBox;
                        *ySurface = curWaterBox->minPos.y;
                        return true;
                    }
                }
            }
        }
    }

    for (i = 0; i < BG_ACTOR_MAX; i++) {
        if ((colCtx->dyna.bgActorFlags[i] & 1) && !(colCtx->dyna.bgActorFlags[i] & 2)) {
            WaterBox* boxes;
            boxes = colCtx->dyna.waterBoxList.boxes;
            bgActor = &colCtx->dyna.bgActors[i];
            if (bgActor->colHeader->numWaterBoxes != 0 && bgActor->colHeader->waterBoxes != NULL) {
                for (curWaterBox = boxes + bgActor->waterboxesStartIndex;
                     curWaterBox < boxes + bgActor->waterboxesStartIndex + bgActor->colHeader->numWaterBoxes;
                     curWaterBox++) {
                    if ((curWaterBox->properties & 0x80000) != 0) {
                        if (curWaterBox->minPos.x < x && x < curWaterBox->minPos.x + curWaterBox->xLength) {
                            if (curWaterBox->minPos.z < z && z < curWaterBox->minPos.z + curWaterBox->zLength) {
                                *outWaterBox = curWaterBox;
                                *ySurface = curWaterBox->minPos.y;
                                *bgId = i;
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
#else
#pragma GLOBAL_ASM("asm/non_matchings/code/z_bgcheck/func_800CA6F0.s")
#endif

s32 func_800CA9D0(GlobalContext* globalCtx, CollisionContext* colCtx, f32 x, f32 z, f32* ySurface,
                  WaterBox** outWaterBox) {
    s32 bgId;

    return func_800CA6F0(globalCtx, colCtx, x, z, ySurface, outWaterBox, &bgId);
}

/**
 * Get the `closestPoint` to line (`pointA`, `pointB`) formed from the intersection of planes `polyA` and `polyB`
 * returns true if the `closestPoint` exists, else returns false
 */
s32 func_800CAA14(CollisionPoly* polyA, CollisionPoly* polyB, Vec3f* pointA, Vec3f* pointB, Vec3f* closestPoint) {
    f32 n1X;
    f32 n1Y;
    f32 n1Z;
    f32 n2X;
    f32 n2Y;
    f32 n2Z;
    s32 result;

    CollisionPoly_GetNormalF(polyA, &n1X, &n1Y, &n1Z);
    CollisionPoly_GetNormalF(polyB, &n2X, &n2Y, &n2Z);
    result = Math3D_PlaneVsLineSegClosestPoint(n1X, n1Y, n1Z, polyA->dist, n2X, n2Y, n2Z, polyB->dist, pointA, pointB,
                                               closestPoint);
    return result;
}
