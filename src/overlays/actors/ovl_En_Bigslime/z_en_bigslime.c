/*
 * File: z_en_bigslime.c
 * Overlay: ovl_En_Bigslime
 * Description: Mad Jelly & Gekko Miniboss: Fused Jellies & Gekko
 */

#include "z_en_bigslime.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/object_bigslime/object_bigslime.h"

#define FLAGS 0x00000235

#define THIS ((EnBigslime*)thisx)

void EnBigslime_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBigslime_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBigslime_UpdateGekko(Actor* thisx, GlobalContext* globalCtx);
void EnBigslime_DrawGekko(Actor* thisx, GlobalContext* globalCtx);

void EnBigslime_EndCutscene(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_CutsceneStartBattle(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupCutsceneNoticePlayer(EnBigslime* this);
void EnBigslime_CutsceneNoticePlayer(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupCallMinislime(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_CallMinislime(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_MoveOnCeiling(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupDrop(EnBigslime* this);
void EnBigslime_Drop(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetTargetVtxToWideCone(EnBigslime* this);
void EnBigslime_SquishFlat(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupSquishFlat(EnBigslime* this);
void EnBigslime_SetTargetVtxToThinCone(EnBigslime* this);
void EnBigslime_SetTargetVtxToInverseCone(EnBigslime* this);
void EnBigslime_SetTargetVtxToStaticVtx(EnBigslime* this);
void EnBigslime_SetupRise(EnBigslime* this);
void EnBigslime_Rise(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_CutsceneGrabPlayer(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupAttackPlayerInBigslime(EnBigslime* this);
void EnBigslime_AttackPlayerInBigslime(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupWindupThrowPlayer(EnBigslime* this);
void EnBigslime_WindupThrowPlayer(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupSetDynamicVtxThrowPlayer(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetDynamicVtxThrowPlayer(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupFrozenGround(EnBigslime* this);
void EnBigslime_FrozenGround(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_Freeze(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_Melt(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupFrozenFall(EnBigslime* this);
void EnBigslime_FrozenFall(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupJumpGekko(EnBigslime* this);
void EnBigslime_JumpGekko(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupIdleLookAround(EnBigslime* this);
void EnBigslime_IdleLookAround(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupIdleNoticePlayer(EnBigslime* this);
void EnBigslime_IdleNoticePlayer(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupThrowMinislime(EnBigslime* this);
void EnBigslime_DamageGekko(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_StunGekko(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_CutsceneFormBigslime(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupFormBigslime(EnBigslime* this);
void EnBigslime_FormBigslime(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_CutsceneDefeat(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupGekkoDespawn(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_GekkoDespawn(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupFrogSpawn(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_FrogSpawn(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupDespawn(EnBigslime* this);
void EnBigslime_Despawn(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupInitEntrance(EnBigslime* this);
void EnBigslime_InitEntrance(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_ThrowMinislime(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_SetupCutscene(EnBigslime* this);
void EnBigslime_PlayCutscene(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_AddIceShardEffect(EnBigslime* this, GlobalContext* globalCtx);
void EnBigslime_UpdateBigslime(Actor* thisx, GlobalContext* globalCtx);
void EnBigslime_DrawBigslime(Actor* thisx, GlobalContext* globalCtx);
void EnBigslime_DrawShatteringEffects(EnBigslime* this, GlobalContext* globalCtx);

/*
 * Bigslime Spherical Vtx Data:
 *     - The vtx data for Bigslime is put directly into the overlay
 *     - This vtx is directly manipulated in a significant number of functions in this overlay
 *     - The initialized vtx data has the following properties: (some of this data may change as overlay functions are
 * called)
 *           -- It is a sphere with radius 1000
 *           -- There are 162 vertices numbered top to bottom along the y-axis,
 *              with vertex 0 being the top apex, and vertex 161 being the bottom apex
 *           -- There are 4 entire copies of the sphere
 *              --- The first 3 copies are identical
 *              --- All 4 copies have identical coordinates and number of vertices
 *              --- See below (variants) for the uses of these copies
 *
 * Vtx Rings:
 *     - The vtx mesh was designed to be seperated into distint "rings" of vertices and faces
 *       from top to bottom at discrete y-intervals
 *     - These different rings of vertices lie in the xzPlane along discrete y-direction steps of the sphere
 *          -- There are the single apex nodes at the top (vertex 0 y-coordiante 1000) and bottom (vertex 161
 * y-coordiante -1000)
 *          -- There are a cluster of vertices (vertex 1-5) which all have y-coordinate 962
 *             and form a ring of vertices in the xz-plane
 *          -- There are a cluster of vertices (vertex 6-15) which all have y-coordinate around 870
 *             and form a ring of vertices in the xz-plane
 *          -- So on and so forth (See sVtxRingStartIndex)
 *          -- BIGSLIME_NUM_RING_VTX: There are a total of 13 rings of vertices including the two single apex vertex
 *          -- BIGSLIME_NUM_RING_FACES: There are a total of 12 rings of faces between the rings of vertices
 *     - A collider is assigned to each of the 12 rings of faces for bigslime
 *
 * Three Variants of the Spherical Vtx:
 *      Static (1 copy):
 *          - Fixed mesh, no values are ever modified
 *          - Used as reference to store the original vertices
 *          - Never drawn, only used in calculations
 *      Dynamic (2 copies):
 *          - Coordinate are updated and deformed, mesh is drawn as the deforming Fused Jelly.
 *          - Initially Identical to Static.
 *          - Two copies are used that alternate every frame and copies over coordinates/normals immediately.
 *            One copy is the version that is drawn, and the other copy is updated in the background
 *          - Alpha values remain fixed
 *      Target (1 copy):
 *          - Used to define the target vertices/shape for the dynamic vertices to smoothly step to.
 *          - When frozen:
 *              -- The current target vertices are set to the current dynamic vertices
 *              -- These are the vertices that are used to draw the frozen Fused Jelly
 *              -- The alpha values are used for the frozen effects alpha
 */

// Reference data: used to store the original vertices
static Vtx sBigslimeStaticVtx[BIGSLIME_NUM_VTX] = {
#include "overlays/ovl_En_Bigslime/sBigslimeStaticVtx.vtx.inc"
};

// Dynamic data: used to draw the real shape and has 2 states
static Vtx sBigslimeDynamicVtx[2][BIGSLIME_NUM_VTX] = {
    {
#include "overlays/ovl_En_Bigslime/sBigslimeDynamicState0Vtx.vtx.inc"
    },
    {
#include "overlays/ovl_En_Bigslime/sBigslimeDynamicState1Vtx.vtx.inc"
    },
};

// Target data: used to define the shape the dynamic vertices morph to
static Vtx sBigslimeTargetVtx[BIGSLIME_NUM_VTX] = {
#include "overlays/ovl_En_Bigslime/sBigslimeTargetVtx.vtx.inc"
};

/*
 * Triangle face
 * Connectivity matrix for mesh above. Covers every triangle in the sphere
 * 320 triangles x 3 vertices per triangle
 * Purpose: indices to calculate/update normal vector for lighting calculations
 * "EnBigslimeTri" struct based on "Tri" struct from gbi.h, but without the flag
 */
static EnBigslimeTri sEnbigslimeTri[BIGSLIME_NUM_FACES] = {
    { 0, 2, 1 },       { 1, 7, 6 },       { 1, 2, 7 },       { 2, 8, 7 },       { 6, 17, 16 },     { 6, 7, 17 },
    { 8, 19, 18 },     { 7, 8, 18 },      { 7, 18, 17 },     { 9, 21, 20 },     { 0, 3, 2 },       { 2, 9, 8 },
    { 2, 3, 9 },       { 3, 10, 9 },      { 8, 20, 19 },     { 8, 9, 20 },      { 10, 22, 21 },    { 9, 10, 21 },
    { 0, 4, 3 },       { 3, 11, 10 },     { 3, 4, 11 },      { 4, 12, 11 },     { 10, 23, 22 },    { 10, 11, 23 },
    { 12, 25, 24 },    { 11, 12, 24 },    { 11, 24, 23 },    { 13, 27, 26 },    { 0, 5, 4 },       { 4, 13, 12 },
    { 4, 5, 13 },      { 5, 14, 13 },     { 12, 26, 25 },    { 12, 13, 26 },    { 14, 28, 27 },    { 13, 14, 27 },
    { 0, 1, 5 },       { 5, 15, 14 },     { 5, 1, 15 },      { 1, 6, 15 },      { 14, 29, 28 },    { 14, 15, 29 },
    { 6, 16, 30 },     { 15, 6, 30 },     { 15, 30, 29 },    { 16, 32, 31 },    { 16, 17, 32 },    { 17, 33, 32 },
    { 17, 18, 33 },    { 18, 34, 33 },    { 18, 19, 34 },    { 19, 35, 34 },    { 19, 20, 36 },    { 20, 37, 36 },
    { 20, 21, 37 },    { 21, 38, 37 },    { 21, 22, 38 },    { 22, 39, 38 },    { 19, 36, 35 },    { 22, 40, 39 },
    { 22, 23, 40 },    { 23, 41, 40 },    { 23, 24, 41 },    { 24, 42, 41 },    { 24, 25, 42 },    { 25, 43, 42 },
    { 25, 26, 44 },    { 26, 45, 44 },    { 26, 27, 45 },    { 27, 46, 45 },    { 27, 28, 46 },    { 28, 47, 46 },
    { 25, 44, 43 },    { 28, 48, 47 },    { 28, 29, 48 },    { 29, 49, 48 },    { 29, 30, 49 },    { 30, 50, 49 },
    { 30, 16, 50 },    { 16, 31, 50 },    { 31, 32, 51 },    { 32, 52, 51 },    { 32, 33, 52 },    { 33, 53, 52 },
    { 33, 34, 53 },    { 34, 54, 53 },    { 34, 35, 54 },    { 35, 55, 54 },    { 35, 36, 55 },    { 36, 56, 55 },
    { 36, 37, 56 },    { 37, 57, 56 },    { 37, 38, 57 },    { 38, 58, 57 },    { 38, 39, 58 },    { 39, 59, 58 },
    { 39, 40, 59 },    { 40, 60, 59 },    { 40, 41, 60 },    { 41, 61, 60 },    { 41, 42, 61 },    { 42, 62, 61 },
    { 42, 43, 62 },    { 43, 63, 62 },    { 51, 72, 71 },    { 51, 52, 72 },    { 52, 73, 72 },    { 52, 53, 73 },
    { 53, 74, 73 },    { 53, 54, 74 },    { 54, 75, 74 },    { 54, 55, 75 },    { 55, 76, 75 },    { 55, 56, 76 },
    { 56, 77, 76 },    { 56, 57, 77 },    { 57, 78, 77 },    { 57, 58, 78 },    { 58, 79, 78 },    { 58, 59, 79 },
    { 59, 80, 79 },    { 59, 60, 80 },    { 60, 81, 80 },    { 60, 61, 81 },    { 61, 82, 81 },    { 61, 62, 82 },
    { 62, 83, 82 },    { 62, 63, 83 },    { 71, 72, 91 },    { 72, 92, 91 },    { 72, 73, 92 },    { 73, 93, 92 },
    { 73, 74, 93 },    { 74, 94, 93 },    { 74, 75, 94 },    { 75, 95, 94 },    { 75, 76, 95 },    { 76, 96, 95 },
    { 76, 77, 96 },    { 77, 97, 96 },    { 77, 78, 97 },    { 78, 98, 97 },    { 78, 79, 98 },    { 79, 99, 98 },
    { 79, 80, 99 },    { 80, 100, 99 },   { 80, 81, 100 },   { 81, 101, 100 },  { 81, 82, 101 },   { 82, 102, 101 },
    { 82, 83, 102 },   { 83, 103, 102 },  { 91, 112, 111 },  { 91, 92, 112 },   { 92, 113, 112 },  { 92, 93, 113 },
    { 93, 114, 113 },  { 93, 94, 114 },   { 94, 115, 114 },  { 94, 95, 115 },   { 95, 116, 115 },  { 95, 96, 116 },
    { 96, 117, 116 },  { 96, 97, 117 },   { 97, 118, 117 },  { 97, 98, 118 },   { 98, 119, 118 },  { 98, 99, 119 },
    { 99, 120, 119 },  { 99, 100, 120 },  { 100, 121, 120 }, { 100, 101, 121 }, { 101, 122, 121 }, { 101, 102, 122 },
    { 102, 123, 122 }, { 102, 103, 123 }, { 43, 44, 63 },    { 44, 64, 63 },    { 44, 45, 64 },    { 45, 65, 64 },
    { 45, 46, 65 },    { 46, 66, 65 },    { 46, 47, 66 },    { 47, 67, 66 },    { 47, 48, 67 },    { 48, 68, 67 },
    { 48, 49, 68 },    { 49, 69, 68 },    { 49, 50, 69 },    { 50, 70, 69 },    { 50, 31, 70 },    { 31, 51, 70 },
    { 63, 84, 83 },    { 63, 64, 84 },    { 64, 85, 84 },    { 64, 65, 85 },    { 65, 86, 85 },    { 65, 66, 86 },
    { 66, 87, 86 },    { 66, 67, 87 },    { 67, 88, 87 },    { 67, 68, 88 },    { 68, 89, 88 },    { 68, 69, 89 },
    { 69, 90, 89 },    { 69, 70, 90 },    { 70, 71, 90 },    { 70, 51, 71 },    { 83, 84, 103 },   { 84, 104, 103 },
    { 84, 85, 104 },   { 85, 105, 104 },  { 85, 86, 105 },   { 86, 106, 105 },  { 86, 87, 106 },   { 87, 107, 106 },
    { 87, 88, 107 },   { 88, 108, 107 },  { 88, 89, 108 },   { 89, 109, 108 },  { 89, 90, 109 },   { 90, 110, 109 },
    { 90, 71, 110 },   { 71, 91, 110 },   { 103, 124, 123 }, { 103, 104, 124 }, { 104, 125, 124 }, { 104, 105, 125 },
    { 105, 126, 125 }, { 105, 106, 126 }, { 106, 127, 126 }, { 106, 107, 127 }, { 107, 128, 127 }, { 107, 108, 128 },
    { 108, 129, 128 }, { 108, 109, 129 }, { 109, 130, 129 }, { 109, 110, 130 }, { 110, 111, 130 }, { 110, 91, 111 },
    { 161, 160, 156 }, { 155, 146, 160 }, { 146, 156, 160 }, { 146, 147, 156 }, { 144, 145, 155 }, { 145, 146, 155 },
    { 145, 131, 146 }, { 131, 147, 146 }, { 131, 132, 147 }, { 134, 135, 149 }, { 161, 156, 157 }, { 147, 148, 156 },
    { 148, 157, 156 }, { 148, 149, 157 }, { 132, 133, 147 }, { 133, 148, 147 }, { 133, 134, 148 }, { 134, 149, 148 },
    { 161, 157, 158 }, { 149, 150, 157 }, { 150, 158, 157 }, { 150, 151, 158 }, { 135, 136, 149 }, { 136, 150, 149 },
    { 136, 137, 150 }, { 137, 151, 150 }, { 137, 138, 151 }, { 140, 141, 153 }, { 161, 158, 159 }, { 151, 152, 158 },
    { 152, 159, 158 }, { 152, 153, 159 }, { 138, 139, 151 }, { 139, 152, 151 }, { 139, 140, 152 }, { 140, 153, 152 },
    { 161, 159, 160 }, { 153, 154, 159 }, { 154, 160, 159 }, { 154, 155, 160 }, { 141, 142, 153 }, { 142, 154, 153 },
    { 142, 143, 154 }, { 143, 155, 154 }, { 143, 144, 155 }, { 111, 131, 145 }, { 111, 112, 131 }, { 112, 132, 131 },
    { 112, 113, 132 }, { 113, 114, 132 }, { 114, 133, 132 }, { 114, 115, 133 }, { 115, 116, 134 }, { 116, 135, 134 },
    { 116, 117, 135 }, { 117, 118, 135 }, { 118, 136, 135 }, { 118, 119, 136 }, { 115, 134, 133 }, { 119, 137, 136 },
    { 119, 120, 137 }, { 120, 138, 137 }, { 120, 121, 138 }, { 121, 122, 138 }, { 122, 139, 138 }, { 122, 123, 139 },
    { 139, 123, 140 }, { 140, 123, 124 }, { 140, 124, 141 }, { 141, 124, 125 }, { 141, 125, 126 }, { 141, 126, 142 },
    { 142, 127, 143 }, { 143, 127, 128 }, { 143, 128, 144 }, { 144, 128, 129 }, { 144, 129, 130 }, { 144, 130, 145 },
    { 142, 126, 127 }, { 130, 111, 145 },
};

const ActorInit En_Bigslime_InitVars = {
    ACTOR_EN_BIGSLIME,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_BIGSLIME,
    sizeof(EnBigslime),
    (ActorFunc)EnBigslime_Init,
    (ActorFunc)EnBigslime_Destroy,
    (ActorFunc)EnBigslime_UpdateGekko,
    (ActorFunc)EnBigslime_DrawGekko,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x20000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 60, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 10, 100, 100, 50 };

typedef enum {
    /* 0x0 */ BIGSLIME_DMGEFF_NONE,
    /* 0x0 */ BIGSLIME_DMGEFF_STUN,
    /* 0x2 */ BIGSLIME_DMGEFF_FIRE,
    /* 0x3 */ BIGSLIME_DMGEFF_ICE,
    /* 0x4 */ BIGSLIME_DMGEFF_LIGHT,
    /* 0x5 */ BIGSLIME_DMGEFF_ELECTRIC_STUN,
    /* 0xD */ BIGSLIME_DMGEFF_DEKU_STUN = 0xD,
    /* 0xE */ BIGSLIME_DMGEFF_HOOKSHOT,
    /* 0xF */ BIGSLIME_DMGEFF_BREAK_ICE,
} BigslimeDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, BIGSLIME_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, BIGSLIME_DMGEFF_BREAK_ICE),
    /* Zora boomerang */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, BIGSLIME_DMGEFF_HOOKSHOT),
    /* Goron punch    */ DMG_ENTRY(1, BIGSLIME_DMGEFF_BREAK_ICE),
    /* Sword          */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, BIGSLIME_DMGEFF_BREAK_ICE),
    /* Fire arrow     */ DMG_ENTRY(1, BIGSLIME_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(1, BIGSLIME_DMGEFF_ICE),
    /* Light arrow    */ DMG_ENTRY(2, BIGSLIME_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, BIGSLIME_DMGEFF_BREAK_ICE),
    /* Deku spin      */ DMG_ENTRY(0, BIGSLIME_DMGEFF_DEKU_STUN),
    /* Deku bubble    */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, BIGSLIME_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, BIGSLIME_DMGEFF_ELECTRIC_STUN),
    /* Normal shield  */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, BIGSLIME_DMGEFF_BREAK_ICE),
    /* Zora punch     */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, BIGSLIME_DMGEFF_BREAK_ICE),
};

// Start vtx numbers for each ring in the vtx sphere
static s32 sVtxRingStartIndex[] = {
    0, 1, 6, 16, 31, 51, 71, 91, 111, 131, 146, 156, 161, BIGSLIME_NUM_VTX,
};

static AnimationHeader* sGekkoAttackAnimations[] = {
    &gGekkoJabPunchAnim,
    &gGekkoHookPunchAnim,
    &gGekkoKickAnim,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, 95, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(targetArrowOffset, -13221, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 5, ICHAIN_STOP),
};

void EnBigslime_Init(Actor* thisx, GlobalContext* globalCtx) {
    // gSaveContext.weekEventReg[KEY] = VALUE
    // KEY | VALUE
    static s32 isFrogReturnedFlags[] = {
        (32 << 8) | 0x40, // Woodfall Temple Frog Returned
        (32 << 8) | 0x80, // Great Bay Temple Frog Returned
        (33 << 8) | 0x01, // Southern Swamp Frog Returned
        (33 << 8) | 0x02, // Laundry Pool Frog Returned
    };
    EnBigslime* this = THIS;
    GlobalContext* globalCtx2 = globalCtx;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gGekkoSkel, &gGekkoLookAroundAnim, this->jointTable,
                       this->morphTable, GEKKO_LIMB_MAX);

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        Collider_InitAndSetCylinder(globalCtx, &this->bigslimeCollider[i], &this->actor, &sCylinderInit);
    }

    this->bigslimeCollider[0].base.atFlags &= ~AT_ON;
    Collider_InitAndSetCylinder(globalCtx, &this->gekkoCollider, &this->actor, &sCylinderInit);
    this->gekkoCollider.base.colType = COLTYPE_HIT6;
    this->gekkoCollider.info.elemType = ELEMTYPE_UNK1;
    this->gekkoCollider.base.atFlags &= ~AT_ON;
    this->gekkoCollider.base.ocFlags1 &= ~OC1_NO_PUSH;
    this->actor.params = CLAMP(this->actor.params, 1, 4);

    if (Actor_GetRoomCleared(globalCtx, globalCtx->roomCtx.currRoom.num)) {
        Actor_MarkForDeath(&this->actor);
        if (!(gSaveContext.weekEventReg[isFrogReturnedFlags[this->actor.params - 1] >> 8] &
              (u8)isFrogReturnedFlags[this->actor.params - 1])) {
            Actor_Spawn(&globalCtx2->actorCtx, globalCtx, ACTOR_EN_MINIFROG, this->actor.world.pos.x,
                        this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                        this->actor.params);
        }
    } else {
        this->cutscene = this->actor.cutscene;
        this->actor.scale.x = this->actor.scale.z = 0.15f;
        this->actor.scale.y = 0.075f;
        this->vtxScaleX = this->vtxScaleZ = 0.015000001f;
        this->actor.home.pos.x = GBT_ROOM_5_CENTER_X;
        this->actor.home.pos.y = GBT_ROOM_5_MAX_Y - 75.0f;
        this->actor.home.pos.z = GBT_ROOM_5_CENTER_Z;
        for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
            this->minislime[i] = (EnMinislime*)Actor_SpawnAsChild(&globalCtx2->actorCtx, &this->actor, globalCtx,
                                                                  ACTOR_EN_MINISLIME, 0.0f, 0.0f, 0.0f, 0, 0, 0, i);
            if (this->minislime[i] == NULL) {
                for (i = i - 1; i >= 0; i--) {
                    Actor_MarkForDeath(&this->minislime[i]->actor);
                }

                Actor_MarkForDeath(&this->actor);
                return;
            }
        }

        this->minislimeFrozenTexAnim = Lib_SegmentedToVirtual(&gMinislimeFrozenTexAnim);
        this->bigslimeFrozenTexAnim = Lib_SegmentedToVirtual(&gBigslimeFrozenTexAnim);
        this->iceShardTexAnim = Lib_SegmentedToVirtual(&gBigslimeIceShardTexAnim);
        this->actor.world.pos.y = GBT_ROOM_5_MIN_Y;
        this->actor.flags &= ~1;
        this->actor.shape.shadowAlpha = 255;
        this->gekkoScale = 0.007f;
        this->actor.shape.rot.y = 0;
        this->minislimeToThrow = this->minislime[0];
        EnBigslime_SetupInitEntrance(this);
    }
}

void EnBigslime_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBigslime* this = THIS;
    s32 i;

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        Collider_DestroyCylinder(globalCtx, &this->bigslimeCollider[i]);
    }

    Collider_DestroyCylinder(globalCtx, &this->gekkoCollider);
    func_801A72CC(&this->gekkoProjectedPos);
}

void EnBigslime_DynamicVtxCopyState(EnBigslime* this) {
    Vtx* dynamicVtxDest;
    Vtx* dynamicVtxSrc;
    s32 i;
    s32 j;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtxDest = &sBigslimeDynamicVtx[this->dynamicVtxState][i];
        dynamicVtxSrc = &sBigslimeDynamicVtx[this->dynamicVtxState ^ 1][i];
        for (j = 0; j < 3; j++) {
            dynamicVtxDest->n.ob[j] = dynamicVtxSrc->n.ob[j];
            dynamicVtxDest->n.n[j] = dynamicVtxSrc->n.n[j];
        }
    }
}

void EnBigslime_Vec3fNormalize(Vec3f* vec) {
    f32 magnitude = Math3D_Vec3fMagnitude(vec);

    if (magnitude > 1.0f) {
        Math_Vec3f_Scale(vec, 1.0f / magnitude);
    }
}

/**
 * Updates the surface normal vector of each vertex in the fused jelly mesh
 * each frame as the dynamic vtx is constantly changing shape
 */
void EnBigslime_UpdateSurfaceNorm(EnBigslime* this) {
    Vec3f vtxNorm[BIGSLIME_NUM_VTX];
    Vec3f vecTriEdge1;
    Vec3f vecTriEdge2;
    Vec3f vecTriNorm;
    Vec3f* vtxNormAddr;
    Vtx* dynamicVtx;
    Vtx* dynamicVtx12;
    Vtx* dynamicVtx0;
    s32 i;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        Math_Vec3f_Copy(&vtxNorm[i], &gZeroVec3f);
    }

    for (i = 0; i < BIGSLIME_NUM_FACES; i++) {
        dynamicVtx0 = &sBigslimeDynamicVtx[this->dynamicVtxState][sEnbigslimeTri[i].v[0]];
        dynamicVtx12 = &sBigslimeDynamicVtx[this->dynamicVtxState][sEnbigslimeTri[i].v[1]];
        vecTriEdge1.x = dynamicVtx12->n.ob[0] - dynamicVtx0->n.ob[0];
        vecTriEdge1.y = dynamicVtx12->n.ob[1] - dynamicVtx0->n.ob[1];
        vecTriEdge1.z = dynamicVtx12->n.ob[2] - dynamicVtx0->n.ob[2];

        dynamicVtx12 = &sBigslimeDynamicVtx[this->dynamicVtxState][sEnbigslimeTri[i].v[2]];
        vecTriEdge2.x = dynamicVtx12->n.ob[0] - dynamicVtx0->n.ob[0];
        vecTriEdge2.y = dynamicVtx12->n.ob[1] - dynamicVtx0->n.ob[1];
        vecTriEdge2.z = dynamicVtx12->n.ob[2] - dynamicVtx0->n.ob[2];

        Math3D_CrossProduct(&vecTriEdge1, &vecTriEdge2, &vecTriNorm);
        EnBigslime_Vec3fNormalize(&vecTriNorm);

        Math_Vec3f_Sum(&vtxNorm[sEnbigslimeTri[i].v[0]], &vecTriNorm, &vtxNorm[sEnbigslimeTri[i].v[0]]);
        Math_Vec3f_Sum(&vtxNorm[sEnbigslimeTri[i].v[1]], &vecTriNorm, &vtxNorm[sEnbigslimeTri[i].v[1]]);
        Math_Vec3f_Sum(&vtxNorm[sEnbigslimeTri[i].v[2]], &vecTriNorm, &vtxNorm[sEnbigslimeTri[i].v[2]]);
    }

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        vtxNormAddr = &vtxNorm[i];
        EnBigslime_Vec3fNormalize(vtxNormAddr);
        dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];
        dynamicVtx->n.n[0] = vtxNormAddr->x * 120.0f;
        dynamicVtx->n.n[1] = vtxNormAddr->y * 120.0f;
        dynamicVtx->n.n[2] = vtxNormAddr->z * 120.0f;
    }
}

void EnBigslime_GetMaxMinVertices(EnBigslime* this, Vec3f* vtxMax, Vec3f* vtxMin) {
    Vtx* dynamicVtx;
    s16 vtxMaxX = 0;
    s16 vtxMaxY = 0;
    s16 vtxMaxZ = 0;
    s16 vtxMinX = 0;
    s16 vtxMinY = 0;
    s16 vtxMinZ = 0;
    s32 i;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];

        if (vtxMaxX < dynamicVtx->n.ob[0]) {
            vtxMaxX = dynamicVtx->n.ob[0];
        } else if (vtxMinX > dynamicVtx->n.ob[0]) {
            vtxMinX = dynamicVtx->n.ob[0];
        }

        if (vtxMaxY < dynamicVtx->n.ob[1]) {
            vtxMaxY = dynamicVtx->n.ob[1];
        } else if (vtxMinY > dynamicVtx->n.ob[1]) {
            vtxMinY = dynamicVtx->n.ob[1];
        }

        if (vtxMaxZ < dynamicVtx->n.ob[2]) {
            vtxMaxZ = dynamicVtx->n.ob[2];
        } else if (vtxMinZ > dynamicVtx->n.ob[2]) {
            vtxMinZ = dynamicVtx->n.ob[2];
        }
    }

    vtxMax->x = vtxMaxX * this->actor.scale.x;
    vtxMax->y = vtxMaxY * this->actor.scale.y;
    vtxMax->z = vtxMaxZ * this->actor.scale.z;

    vtxMin->x = vtxMinX * this->actor.scale.x;
    vtxMin->y = vtxMinY * this->actor.scale.y;
    vtxMin->z = vtxMinZ * this->actor.scale.z;
}

void EnBigslime_UpdateScale(EnBigslime* this, Vec3f* vtxMax, Vec3f* vtxMin) {
    if (vtxMin->x < vtxMax->x) {
        this->vtxScaleX = vtxMax->x * (0.100000005f / BIGSLIME_RADIUS_F);
    } else {
        this->vtxScaleX = -vtxMin->x * (0.100000005f / BIGSLIME_RADIUS_F);
    }

    if (vtxMin->z < vtxMax->z) {
        this->vtxScaleZ = vtxMax->z * (0.100000005f / BIGSLIME_RADIUS_F);
    } else {
        this->vtxScaleZ = -vtxMin->z * (0.100000005f / BIGSLIME_RADIUS_F);
    }
}

/**
 * Checks the world position against all walls, the floor, and the ceiling to ensure
 * bigslime is not outside this range. Accounts only for the world position, and the
 * maximum and minimum verticies. If any boundaries are crossed, the world position
 * and bgCheckFlags are updated accordingly.
 *
 * Differs from EnBigslime_CheckVtxWallBoundaries by checking and updating
 * a single world position instead of checking and updating individual vertices
 */
void EnBigslime_CheckRoomBoundaries(EnBigslime* this, Vec3f* vtxMax, Vec3f* vtxMin) {
    f32 worldPosX;
    f32 vtxMaxX;
    f32 vtxMinX;
    f32 worldPosZ;
    f32 vtxMaxZ;
    f32 vtxMinZ;

    this->actor.bgCheckFlags &= ~(0x2 | 0x8 | 0x10);
    if ((this->actor.world.pos.y + vtxMax->y) > GBT_ROOM_5_MAX_Y) {
        this->actor.world.pos.y = GBT_ROOM_5_MAX_Y - vtxMax->y;
        this->actor.bgCheckFlags |= 0x10;
    }

    if ((this->actor.world.pos.y + vtxMin->y) <= GBT_ROOM_5_MIN_Y) {
        this->actor.world.pos.y = GBT_ROOM_5_MIN_Y - vtxMin->y;
        if (!(this->actor.bgCheckFlags & 1)) {
            this->actor.bgCheckFlags |= 2;
        }

        this->actor.bgCheckFlags |= 1;
    } else {
        this->actor.bgCheckFlags &= ~1;
    }

    if ((this->actionFunc != EnBigslime_Freeze) || !(this->actor.bgCheckFlags & 1)) {
        worldPosX = this->actor.world.pos.x;
        vtxMaxX = vtxMax->x;
        if (GBT_ROOM_5_MAX_X < (worldPosX + vtxMaxX)) {
            this->actor.bgCheckFlags |= 8;
            this->actor.world.pos.x = GBT_ROOM_5_MAX_X - vtxMaxX;
        } else {
            vtxMinX = vtxMin->x;
            if ((worldPosX + vtxMinX) < GBT_ROOM_5_MIN_X) {
                this->actor.world.pos.x = GBT_ROOM_5_MIN_X - vtxMinX;
                this->actor.bgCheckFlags |= 8;
            }
        }

        worldPosZ = this->actor.world.pos.z;
        vtxMaxZ = vtxMax->z;
        if (GBT_ROOM_5_MAX_Z < (worldPosZ + vtxMaxZ)) {
            this->actor.bgCheckFlags |= 8;
            this->actor.world.pos.z = GBT_ROOM_5_MAX_Z - vtxMaxZ;
        } else {
            vtxMinZ = vtxMin->z;
            if ((worldPosZ + vtxMinZ) < GBT_ROOM_5_MIN_Z) {
                this->actor.world.pos.z = GBT_ROOM_5_MIN_Z - vtxMinZ;
                this->actor.bgCheckFlags |= 8;
            }
        }
    }
}

void EnBigslime_UpdateBigslimeCollider(EnBigslime* this, GlobalContext* globalCtx) {
    Vtx* dynamicVtx;
    f32 xzDist;
    s16 vtxRingMaxY[BIGSLIME_NUM_RING_VTX];
    s16 vtxRingMinY[BIGSLIME_NUM_RING_VTX];
    f32 vtxRingMaxXZDist[BIGSLIME_NUM_RING_VTX];
    Cylinder16* dim;
    f32 maxScaleXScaleZ = (this->actor.scale.z < this->actor.scale.x ? this->actor.scale.x : this->actor.scale.z);
    s32 i;
    s32 j;

    for (i = 0; i < BIGSLIME_NUM_RING_VTX; i++) {
        vtxRingMaxY[i] = -30000;
        vtxRingMinY[i] = 30000;
        vtxRingMaxXZDist[i] = 0.0f;

        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][j];
            if (vtxRingMaxY[i] < dynamicVtx->n.ob[1]) {
                vtxRingMaxY[i] = dynamicVtx->n.ob[1];
            }

            if (vtxRingMinY[i] > dynamicVtx->n.ob[1]) {
                vtxRingMinY[i] = dynamicVtx->n.ob[1];
            }

            xzDist = sqrtf(SQ(dynamicVtx->n.ob[0]) + SQ(dynamicVtx->n.ob[2]));
            if (vtxRingMaxXZDist[i] < xzDist) {
                vtxRingMaxXZDist[i] = xzDist;
            }
        }
    }

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        dim = &this->bigslimeCollider[i].dim;
        dim->pos.y = (vtxRingMinY[i + 1] * this->actor.scale.y) + this->actor.world.pos.y;
        dim->height = (vtxRingMaxY[i] - vtxRingMinY[i + 1]) * this->actor.scale.y;
        dim->radius = maxScaleXScaleZ *
                      (vtxRingMaxXZDist[i] < vtxRingMaxXZDist[i + 1] ? vtxRingMaxXZDist[i + 1] : vtxRingMaxXZDist[i]);
        dim->height = CLAMP_MIN(dim->height, 1);
        dim->pos.x = this->actor.world.pos.x;
        dim->pos.z = this->actor.world.pos.z;
    }

    if (this->bigslimeCollider[0].base.atFlags & AT_ON) {
        for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
            CollisionCheck_SetAT(globalCtx, &globalCtx->colChkCtx, &this->bigslimeCollider[i].base);
        }
    }

    if (this->bigslimeCollider[0].base.acFlags & AC_ON) {
        for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->bigslimeCollider[i].base);
        }
    }

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->bigslimeCollider[i].base);
    }
}

void EnBigslime_AddWavySurface(EnBigslime* this) {
    s32 randInt;
    f32 randFloat;
    s32 i;

    this->wavySurfaceTimer = 24;
    randInt = (s32)(Rand_ZeroOne() * 6.0f) >> 1;
    randFloat = (Rand_ZeroOne() * 40.0f) + 50.0f;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        if ((i % 6) == 0) {
            randFloat = (Rand_ZeroOne() * 40.0f) + 50.0f;
        }

        this->vtxSurfacePerturbation[i] = sin_rad((randInt + i) * (M_PI / 3)) * randFloat * 0.001f;
    }
}

void EnBigslime_UpdateWavySurface(EnBigslime* this) {
    f32 vtxSurfaceWave;
    Vtx* staticVtx;
    Vtx* dynamicVtx;
    s32 i;
    s32 j;

    if (this->wavySurfaceTimer == 0) {
        EnBigslime_AddWavySurface(this);
    }

    this->wavySurfaceTimer--;
    vtxSurfaceWave = sin_rad(this->wavySurfaceTimer * (M_PI / 12));
    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        staticVtx = &sBigslimeStaticVtx[i];
        dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];
        for (j = 0; j < 3; j++) {
            // Formula: dynamicVtx = staticVtx * (1 + sin * perturbation)
            dynamicVtx->n.ob[j] =
                staticVtx->n.ob[j] + (s32)(vtxSurfaceWave * staticVtx->n.ob[j] * this->vtxSurfacePerturbation[i]);
        }
    }
}

void EnBigslime_InitFallMinislime(EnBigslime* this) {
    EnMinislime* minislime;
    s32 i;

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        minislime = this->minislime[i];
        minislime->actor.params = MINISLIME_INIT_FALL;
        minislime->actor.world.pos.x = randPlusMinusPoint5Scaled(80.0f) + (GBT_ROOM_5_MIN_X + ((i % 4) + 1) * 192.0f);
        minislime->actor.world.pos.y = Rand_ZeroFloat(250.0f) + (GBT_ROOM_5_CENTER_Y + 50.0f);
        minislime->actor.world.pos.z = randPlusMinusPoint5Scaled(80.0f) + (GBT_ROOM_5_MIN_Z + ((i / 4) + 1) * 192.0f);
    }

    this->minislimeState = MINISLIME_ACTIVE_INIT_STATE;
}

void EnBigslime_SetMinislimeBreakLocation(EnBigslime* this) {
    // Minislime spawn at these vertices when Frozen Bigslime Shatters
    static s32 minislimeSpawnVtx[] = {
        7,  9,  11,  13,  15,  // 5 equally spaced vertices on vtx ring 2 from top
        52, 56, 60,  64,  68,  // 5 equally spaced vertices on vtx ring 5 from top
        94, 98, 102, 106, 110, // 5 equally spaced vertices on vtx ring 7 from top
    };
    Vtx* dynamicVtx;
    EnMinislime* minislime;
    s32 i;

    this->minislimeState = MINISLIME_ACTIVE_CONTINUE_STATE;
    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][minislimeSpawnVtx[i]];
        minislime = this->minislime[i];
        minislime->actor.params = MINISLIME_BREAK_BIGSLIME;
        minislime->actor.world.pos.x = (dynamicVtx->n.ob[0] * this->actor.scale.x) + this->actor.world.pos.x;
        minislime->actor.world.pos.y = (dynamicVtx->n.ob[1] * this->actor.scale.y) + this->actor.world.pos.y;
        minislime->actor.world.pos.z = (dynamicVtx->n.ob[2] * this->actor.scale.z) + this->actor.world.pos.z;
        minislime->actor.world.rot.x = 0x1000 + 0x333 * (MINISLIME_NUM_SPAWN - i);
    }
}

void EnBigslime_SetPlayerParams(EnBigslime* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (player->stateFlags2 & 0x80) {
        player->actor.parent = NULL;
        player->unk_AE8 = 100;
        func_800B8D98(globalCtx, &this->actor, 10.0f, this->actor.world.rot.y, 10.0f);
    }
}

void EnBigslime_EndThrowMinislime(EnBigslime* this) {
    if (this->actionFunc == EnBigslime_ThrowMinislime &&
        this->minislimeToThrow->actor.params == MINISLIME_SETUP_GEKKO_THROW) {
        this->minislimeToThrow->actor.params = MINISLIME_IDLE;
    }
}

void EnBigslime_BreakIntoMinislime(EnBigslime* this, GlobalContext* globalCtx) {
    s32 i;
    s16 quake = Quake_Add(GET_ACTIVE_CAM(globalCtx), 3);

    Quake_SetSpeed(quake, 20000);
    Quake_SetQuakeValues(quake, 15, 0, 0, 0);
    Quake_SetCountdown(quake, 15);
    func_8013ECE0(this->actor.xyzDistToPlayerSq, 180, 20, 100);
    this->bigslimeCollider[0].base.atFlags &= ~AT_ON;
    this->gekkoCollider.base.acFlags &= ~(AC_ON | AC_HIT);

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        this->bigslimeCollider[i].base.acFlags &= ~AC_HIT;
    }

    EnBigslime_SetMinislimeBreakLocation(this);
    this->actor.update = EnBigslime_UpdateGekko;
    this->actor.draw = EnBigslime_DrawGekko;
    this->actor.gravity = -2.0f;
    EnBigslime_SetPlayerParams(this, globalCtx);
    EnBigslime_EndCutscene(this, globalCtx);
    this->actor.colChkInfo.mass = 50;
    this->actor.flags &= ~(0x1 | 0x400);
    this->actor.flags |= 0x200;
    this->actor.hintId = 95;
    this->gekkoRot.x = 0;
    this->gekkoRot.y = 0;
    this->actor.bgCheckFlags &= ~1;
    this->formBigslimeTimer = 2;
    EnBigslime_AddIceShardEffect(this, globalCtx);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_B_SLIME_BREAK);
    EnBigslime_SetupJumpGekko(this);
}

/**
 * Smoothly moves the camera to a side view and keeps it there
 * as bigslime grabs player and the Gekko melee-attacks player
 */
void EnBigslime_UpdateCameraGrabPlayer(EnBigslime* this, GlobalContext* globalCtx) {
    Camera* subCam = Play_GetCamera(globalCtx, this->subCamId);
    Vec3f subCamEye;
    Vec3f subCamAt;

    Math_Vec3f_Copy(&subCamEye, &subCam->eye);
    Math_Vec3f_Copy(&subCamAt, &subCam->at);

    Math_StepToF(&subCamEye.x, (Math_SinS(this->subCamYawGrabPlayer) * 250.0f) + this->actor.world.pos.x, 10.0f);
    Math_StepToF(&subCamEye.y, GBT_ROOM_5_MIN_Y + 187.5f, 10.0f);
    Math_StepToF(&subCamEye.z, (Math_CosS(this->subCamYawGrabPlayer) * 250.0f) + this->actor.world.pos.z, 10.0f);

    Math_StepToF(&subCamAt.x, this->actor.world.pos.x, 10.0f);
    Math_StepToF(&subCamAt.y, GBT_ROOM_5_MIN_Y + 87.5f, 10.0f);
    Math_StepToF(&subCamAt.z, this->actor.world.pos.z, 10.0f);

    Play_CameraSetAtEye(globalCtx, this->subCamId, &subCamAt, &subCamEye);
}

/**
 * Takes the camera eye and instantaneously moves it 10% closer to the focus point "at".
 * This gives the camera a "jerk" feeling
 * Used everytime player is hit inside of bigslime while being grabbed
 */
void EnBigslime_JerkCameraPlayerHit(EnBigslime* this, GlobalContext* globalCtx) {
    Camera* subCam = Play_GetCamera(globalCtx, this->subCamId);
    Vec3f subCamEye;

    Math_Vec3f_Diff(&subCam->eye, &subCam->at, &subCamEye);
    Math_Vec3f_Scale(&subCamEye, 0.9f);
    Math_Vec3f_Sum(&subCamEye, &subCam->at, &subCamEye);
    Play_CameraSetAtEye(globalCtx, this->subCamId, &subCam->at, &subCamEye);
}

/**
 * Performs the camera motion for the intro cutscene as the player enters the room
 * and the battle starts. Positions the camera slightly offset from player,
 * then zooms into the Gekko until the Gekko calls the minislimes down from the ceiling
 */
void EnBigslime_UpdateCameraIntroCs(EnBigslime* this, GlobalContext* globalCtx, s32 noticeTimer) {
    Camera* subCam = Play_GetCamera(globalCtx, this->subCamId);
    Vec3f subCamEye;
    f32 zoom = (noticeTimer * 19.0f) + 67.0f;
    s16 yawOffset = this->actor.yawTowardsPlayer + (noticeTimer * 0x31);

    subCamEye.x = Math_SinS(yawOffset) * zoom + subCam->at.x;
    subCamEye.z = Math_CosS(yawOffset) * zoom + subCam->at.z;
    subCamEye.y = subCam->at.y + -4.0f + (noticeTimer * 2.0f);

    Play_CameraSetAtEye(globalCtx, this->subCamId, &subCam->at, &subCamEye);
}

/**
 * Takes the camera and makes the focus point (at) point at bigslime, who is on the
 * center of the roof. This is used when the minislimes merges into bigslime.
 */
void EnBigslime_UpdateCameraFormingBigslime(EnBigslime* this, GlobalContext* globalCtx) {
    Play_CameraSetAtEye(globalCtx, this->subCamId, &this->actor.focus.pos,
                        &Play_GetCamera(globalCtx, this->subCamId)->eye);
}

void EnBigslime_EndCutscene(EnBigslime* this, GlobalContext* globalCtx) {
    Camera* subCam;

    if (this->subCamId != MAIN_CAM) {
        subCam = Play_GetCamera(globalCtx, this->subCamId);
        Play_CameraSetAtEye(globalCtx, MAIN_CAM, &subCam->at, &subCam->eye);
        this->subCamId = MAIN_CAM;
        ActorCutscene_Stop(this->cutscene);
        this->cutscene = ActorCutscene_GetAdditionalCutscene(this->actor.cutscene);
        func_800B724C(globalCtx, &this->actor, 6);
    }
}

void EnBigslime_Scale(EnBigslime* this, s16 pitch, f32 xzScale, f32 yScale) {
    this->actor.scale.x = ((Math_SinS(pitch) * xzScale) + 1.5f) * 0.1f; // =  0.15f + 0.1f * xzScale * sin(pitch)
    this->actor.scale.y = ((Math_CosS(pitch) * yScale) + 0.75f) * 0.1f; // = 0.075f + 0.1f * yScale  * cos(pitch)
    this->actor.scale.z = 0.3f - this->actor.scale.x;                   // =  0.15f - 0.1f * xzScale * sin(pitch)
}

/**
 * Set the params used by the floor shockwave when bigslime shatters into minislime
 */
void EnBigslime_InitShockwave(EnBigslime* this, GlobalContext* globalCtx) {
    globalCtx->envCtx.unk_C3 = 3;
    Math_Vec3f_Copy(&this->frozenPos, &this->actor.world.pos);
    this->frozenPos.y = GBT_ROOM_5_MIN_Y;
    this->shockwaveAlpha = 235;
    this->shockwaveScale = 0.025f;
}

/**
 * Restores bigslime behaviour to the state before it was frozen
 */
void EnBigslime_SetTargetVtxFromPreFrozen(EnBigslime* this) {
    this->bigslimeCollider[0].base.acFlags |= AC_ON;
    this->actionFunc = this->actionFuncStored;
    this->skelAnime.playSpeed = 1.0f;
    if (this->actionFunc == EnBigslime_SquishFlat) {
        this->bigslimeCollider[0].base.atFlags |= AT_ON;
        EnBigslime_SetTargetVtxToWideCone(this);
    } else if (this->actionFunc == EnBigslime_Rise) {
        if (this->riseCounter == 0) {
            EnBigslime_SetTargetVtxToThinCone(this);
        } else if (this->riseCounter == 1) {
            EnBigslime_SetTargetVtxToInverseCone(this);
        } else {
            EnBigslime_SetTargetVtxToStaticVtx(this);
        }
    }
}

/**
 * Plays the standard Gekko sound effects without reverb
 */
void EnBigslime_GekkoSfxOutsideBigslime(EnBigslime* this, u16 sfxId) {
    func_8019F1C0(&this->gekkoProjectedPos, sfxId);
}

/**
 * Adds reverb to Gekko sound effects when enclosed by bigslime
 */
void EnBigslime_GekkoSfxInsideBigslime(EnBigslime* this, u16 sfxId) {
    func_8019F420(&this->gekkoProjectedPos, sfxId);
}

void EnBigslime_GekkoFreeze(EnBigslime* this) {
    this->gekkoDrawEffect = GEKKO_DRAW_EFFECT_FROZEN;
    this->gekkoCollider.base.colType = COLTYPE_HIT3;
    this->gekkoCollider.info.elemType = ELEMTYPE_UNK0;
    this->stunTimer = 2;
    this->unk_38C = 0.75f;
    this->unk_390 = 1.125f;
    this->unk_388 = 1.0f;
    this->actor.flags &= ~0x200;
}

void EnBigslime_GekkoThaw(EnBigslime* this, GlobalContext* globalCtx) {
    if (this->gekkoDrawEffect == GEKKO_DRAW_EFFECT_FROZEN) {
        this->gekkoDrawEffect = GEKKO_DRAW_EFFECT_THAW;
        this->gekkoCollider.base.colType = COLTYPE_HIT6;
        this->gekkoCollider.info.elemType = ELEMTYPE_UNK1;
        this->unk_388 = 0.0f;
        func_800BF7CC(globalCtx, &this->actor, this->limbPos, ARRAY_COUNT(this->limbPos), 2, 0.3f, 0.2f);
        this->actor.flags |= 0x200;
    }
}

void EnBigslime_SetupCutsceneStartBattle(EnBigslime* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Camera* subCam = Play_GetCamera(globalCtx, this->subCamId);

    globalCtx->envCtx.unk_C3 = 4;
    Animation_PlayLoop(&this->skelAnime, &gGekkoLookAroundAnim);

    this->bigslimeCollider[0].base.atFlags &= ~AT_ON;
    this->bigslimeCollider[0].base.acFlags &= ~AC_ON;

    Math_Vec3f_Copy(&subCam->at, &this->actor.focus.pos);
    func_800B7298(globalCtx, &this->actor, 4);

    player->actor.shape.rot.y = this->actor.yawTowardsPlayer + 0x8000;
    player->actor.world.pos.x = Math_SinS(this->actor.yawTowardsPlayer) * 347.0f + this->actor.world.pos.x;
    player->actor.world.pos.z = Math_CosS(this->actor.yawTowardsPlayer) * 347.0f + this->actor.world.pos.z;

    EnBigslime_UpdateCameraIntroCs(this, globalCtx, 25);

    this->gekkoRot.y = this->actor.yawTowardsPlayer + 0x8000;
    this->isInitJump = false;
    this->actionFunc = EnBigslime_CutsceneStartBattle;
}

void EnBigslime_CutsceneStartBattle(EnBigslime* this, GlobalContext* globalCtx) {
    if (this->isAnimUpdate) {
        EnBigslime_SetupCutsceneNoticePlayer(this);
    } else if (!this->isInitJump && Math_ScaledStepToS(&this->gekkoRot.y, this->actor.yawTowardsPlayer, 0x200)) {
        Animation_PlayOnce(&this->skelAnime, &gGekkoSurpriseJumpAnim);
        EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_JUMP);
        this->isInitJump = true;
    }
}

void EnBigslime_SetupCutsceneNoticePlayer(EnBigslime* this) {
    Animation_PlayLoop(&this->skelAnime, &gGekkoNervousIdleAnim);
    this->noticeTimer = 25;
    this->actionFunc = EnBigslime_CutsceneNoticePlayer;
}

void EnBigslime_CutsceneNoticePlayer(EnBigslime* this, GlobalContext* globalCtx) {
    if (this->noticeTimer != 0) {
        this->noticeTimer--;
    }

    EnBigslime_UpdateCameraIntroCs(this, globalCtx, this->noticeTimer);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 4.0f)) {
        EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EV_WALK_WATER);
    }

    if (this->noticeTimer == 0) {
        EnBigslime_SetupCallMinislime(this, globalCtx);
    }
}

void EnBigslime_SetupCallMinislime(EnBigslime* this, GlobalContext* globalCtx) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGekkoCallAnim, 5.0f);
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_GREET);
    this->callTimer = 0;
    func_800B7298(globalCtx, &this->actor, 7);
    this->actionFunc = EnBigslime_CallMinislime;
}

void EnBigslime_CallMinislime(EnBigslime* this, GlobalContext* globalCtx) {
    if (this->callTimer > 0) {
        this->callTimer--;
        if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 4.0f)) {
            EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EV_WALK_WATER);
        }

        if (this->callTimer == 0) {
            EnBigslime_EndCutscene(this, globalCtx);
            this->formBigslimeTimer = 2;
            this->actor.flags |= 1;
            EnBigslime_SetupIdleNoticePlayer(this);
        }
    } else if (this->isAnimUpdate) {
        Animation_PlayLoop(&this->skelAnime, &gGekkoNervousIdleAnim);
        EnBigslime_UpdateCameraIntroCs(this, globalCtx, 25);
        func_801A2E54(0x38);
        EnBigslime_InitFallMinislime(this);
        globalCtx->envCtx.unk_C3 = 0xFF;
        this->callTimer = 35;
        func_800B7298(globalCtx, &this->actor, 4);
    }
}

void EnBigslime_SetupMoveOnCeiling(EnBigslime* this) {
    Animation_PlayLoop(&this->skelAnime, &gGekkoSwimForwardAnim);
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 20.0f;

    if (this->subCamId != MAIN_CAM) {
        this->actor.speedXZ = 0.0f;
        this->ceilingMoveTimer = 20;
    } else {
        this->ceilingMoveTimer = 320;
        this->actor.speedXZ = 5.0f;
    }

    this->wavySurfaceTimer = 0;
    this->bigslimeCollider[0].base.acFlags |= AC_ON;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.flags &= ~0x200;
    this->actionFunc = EnBigslime_MoveOnCeiling;
}

void EnBigslime_MoveOnCeiling(EnBigslime* this, GlobalContext* globalCtx) {
    s16 pitch; // polar (zenith) angle

    Math_ScaledStepToS(&this->gekkoRot.x, 0, 0x400);
    this->ceilingMoveTimer--;
    pitch = this->ceilingMoveTimer * 0x800;
    EnBigslime_Scale(this, pitch, 0.04f, 0.04f);
    EnBigslime_UpdateWavySurface(this);

    if (this->subCamId != MAIN_CAM) {
        if (this->ceilingMoveTimer == 0) {
            EnBigslime_EndCutscene(this, globalCtx);
            this->ceilingMoveTimer = 320;
        }
    } else if ((this->actor.xzDistToPlayer < 250.0f) || (this->ceilingMoveTimer == 0)) {
        EnBigslime_SetupDrop(this);
    } else {
        this->actor.speedXZ = (fabsf(Math_SinS(pitch)) * 3.0f) + 5.0f;
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 10, 0x800, 0x80);
        this->gekkoRot.y = this->actor.world.rot.y;
    }
}

void EnBigslime_SetupDrop(EnBigslime* this) {
    this->actor.velocity.y = 0.0f;
    this->ceilingDropTimer = 30;
    this->actor.speedXZ = 0.0f;
    this->actionFunc = EnBigslime_Drop;
}

void EnBigslime_Drop(EnBigslime* this, GlobalContext* globalCtx) {
    Vtx* staticVtx;
    Vtx* dynamicVtx;
    s32 i;

    Math_ScaledStepToS(&this->gekkoRot.x, 0x4000, 0x400);
    if (this->ceilingDropTimer != 0) {
        s32 requiredScopeTemp;

        this->ceilingDropTimer--;
        this->ceilingMoveTimer--;
        EnBigslime_UpdateWavySurface(this);
        EnBigslime_Scale(this, this->ceilingMoveTimer * 0x4000, 0.2f, 0.15);
        this->actor.scale.z = this->actor.scale.x;
        if (this->ceilingDropTimer == 0) {
            this->actor.gravity = -2.0f;
            EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_DOWN);
            this->rotation = 0;
        }
    } else if (this->actor.bgCheckFlags & 1) {
        EnBigslime_SetupSquishFlat(this);
    } else {
        Math_StepToF(&this->actor.scale.x, 0.15f, 0.0025f);
        Math_StepToF(&this->actor.scale.y, 0.080000006f, 0.0025f);
        Math_StepToF(&this->actor.scale.z, 0.15f, 0.0025f);
        for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
            staticVtx = &sBigslimeStaticVtx[i];
            dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];
            if (i > 145) {
                Math_StepToS(&dynamicVtx->n.ob[1], staticVtx->n.ob[1] * 0.9f, 5);
            } else if (i < 16) {
                Math_StepToS(&dynamicVtx->n.ob[1], staticVtx->n.ob[1] * 1.3f, 5);
            } else {
                Math_StepToS(&dynamicVtx->n.ob[1], staticVtx->n.ob[1], 5);
            }
            Math_StepToS(&dynamicVtx->n.ob[0], staticVtx->n.ob[0], 5);
            Math_StepToS(&dynamicVtx->n.ob[2], staticVtx->n.ob[2], 5);
        }
    }
}

/**
 * Checks each individual vertex to see if it is outside the walls of the room.
 * If it is, updates the vertex to be back inside the boundary of the room.
 * Also raises the vertex in the y-direction 100 units in model-space for a single wall
 * and 200 unites in the y-direction for two walls i.e a corner.
 *
 * Differs from EnBigslime_CheckRoomBoundaries by checking and updating
 * individual vertices instead of checking and updating a single world position
 */
void EnBigslime_CheckVtxWallBoundaries(EnBigslime* this) {
    Vtx* dynamicVtx;
    f32 vtxX;
    f32 vtxZ;
    s32 collisionCounter;
    s32 i;
    s16 updateVtxY;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];
        vtxX = dynamicVtx->n.ob[0] * this->actor.scale.x;
        vtxZ = dynamicVtx->n.ob[2] * this->actor.scale.z;
        collisionCounter = 0;
        if ((this->actor.world.pos.x + vtxX) > GBT_ROOM_5_MAX_X) {
            dynamicVtx->n.ob[0] = (GBT_ROOM_5_MAX_X - this->actor.world.pos.x) / this->actor.scale.x;
            collisionCounter++;
        } else if ((this->actor.world.pos.x + vtxX) < GBT_ROOM_5_MIN_X) {
            dynamicVtx->n.ob[0] = (GBT_ROOM_5_MIN_X - this->actor.world.pos.x) / this->actor.scale.x;
            collisionCounter++;
        }

        if ((this->actor.world.pos.z + vtxZ) > GBT_ROOM_5_MAX_Z) {
            dynamicVtx->n.ob[2] = (GBT_ROOM_5_MAX_Z - this->actor.world.pos.z) / this->actor.scale.z;
            collisionCounter++;
        } else if ((this->actor.world.pos.z + vtxZ) < GBT_ROOM_5_MIN_Z) {
            dynamicVtx->n.ob[2] = (GBT_ROOM_5_MIN_Z - this->actor.world.pos.z) / this->actor.scale.z;
            collisionCounter++;
        }

        if (dynamicVtx->n.ob[1] != -BIGSLIME_RADIUS_S) {
            updateVtxY = collisionCounter * 100;
            dynamicVtx->n.ob[1] += updateVtxY;
        }
    }
}

/**
 * This sets the target vertices into a wide cone shape
 *
 * Notes:
 *      - The flat surface of the cone is in the xz-plane at y = -1000
 *      - The radius of the flat surface of the cone is 1000 units
 *      - The apex/point of the cone is at the point (0, 1000, 0)
 *      - It is wide because it is smooth and thicker near the apex than a standard cone
 */
void EnBigslime_SetTargetVtxToWideCone(EnBigslime* this) {
    Vtx* staticVtx;
    Vtx* targetVtx;
    s16 vtxY;
    f32 xzDist;
    f32 xzScaleVtx;
    s32 i;
    s32 j;

    for (i = 0; i < BIGSLIME_NUM_RING_FACES / 2; i++) {
        vtxY = (((cos_rad(i * (M_PI / 6)) + 1.0f) * 0.925f) + -0.85f) * BIGSLIME_RADIUS_F;
        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            staticVtx = &sBigslimeStaticVtx[j];
            targetVtx = &sBigslimeTargetVtx[j];
            xzDist = sqrtf(SQ(staticVtx->n.ob[0]) + SQ(staticVtx->n.ob[2]));

            if (xzDist > 1.0f) {
                xzScaleVtx = (BIGSLIME_RADIUS_F / (5.0f * xzDist)) * i;
            } else {
                xzScaleVtx = 200.0f * i; // only taken when i = 0, making 200.0f useless
            }

            targetVtx->n.ob[0] = staticVtx->n.ob[0] * xzScaleVtx;
            targetVtx->n.ob[2] = staticVtx->n.ob[2] * xzScaleVtx;
            targetVtx->n.ob[1] = vtxY;
        }
    }

    for (; i < BIGSLIME_NUM_RING_VTX; i++) {
        vtxY = ((cos_rad((i - BIGSLIME_NUM_RING_FACES / 2) * (M_PI / 16)) * 0.05f) + -1.0f) * BIGSLIME_RADIUS_F;
        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            staticVtx = &sBigslimeStaticVtx[j];
            targetVtx = &sBigslimeTargetVtx[j];
            xzDist = sqrtf(SQ(staticVtx->n.ob[0]) + SQ(staticVtx->n.ob[2]));
            if (xzDist > 1.0f) {
                xzScaleVtx = (BIGSLIME_RADIUS_F / (8.0f * xzDist)) * (14 - i);
            } else {
                // only taken when j = 161, but gets immediately written over (bottom vtx of the sphere)
                xzScaleVtx = 125.0f * (14 - i);
            }

            targetVtx->n.ob[0] = staticVtx->n.ob[0] * xzScaleVtx;
            targetVtx->n.ob[2] = staticVtx->n.ob[2] * xzScaleVtx;
            targetVtx->n.ob[1] = vtxY;
        }
    }

    // Bottom vtx of the sphere
    sBigslimeTargetVtx[BIGSLIME_NUM_VTX - 1].n.ob[0] = 0;
    sBigslimeTargetVtx[BIGSLIME_NUM_VTX - 1].n.ob[2] = 0;
    sBigslimeTargetVtx[BIGSLIME_NUM_VTX - 1].n.ob[1] = -BIGSLIME_RADIUS_S;
}

void EnBigslime_SetupSquishFlat(EnBigslime* this) {
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_B_SLIME_JUMP2);
    this->squishFlatTimer = 20;
    this->actor.scale.x = 0.2f;
    this->actor.scale.z = 0.2f;
    this->actor.scale.y = 0.05f;
    this->actor.world.pos.y = GBT_ROOM_5_MIN_Y + 50.0f;
    EnBigslime_SetTargetVtxToWideCone(this);
    EnBigslime_CheckVtxWallBoundaries(this);
    this->bigslimeCollider[0].base.atFlags |= AT_ON;
    this->actionFunc = EnBigslime_SquishFlat;
}

/**
 * Squishes flat like a pancake to try and grab player
 *
 * Notes:
 *     - The vtx shape of Fused Jelly starts from a wide cone shape
 *     - The squishing occurs throught the large changes in &this->actor.scale
 */
void EnBigslime_SquishFlat(EnBigslime* this, GlobalContext* globalCtx) {
    Player* player;
    Vtx* dynamicVtx;
    Vtx* targetVtx;
    s32 i;

    this->squishFlatTimer--;
    Math_ScaledStepToS(&this->gekkoRot.x, 0, 0x400);
    Math_SmoothStepToF(&this->actor.scale.x, 0.35f, 0.4f, 0.035f, 0.0034999999f);
    Math_SmoothStepToF(&this->actor.scale.y, 0.030000001f, 0.4f, 0.0030000003f, 0.0003f);
    this->actor.scale.z = this->actor.scale.x;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];
        targetVtx = &sBigslimeTargetVtx[i];
        Math_SmoothStepToS(&dynamicVtx->n.ob[0], targetVtx->n.ob[0], 5, 40, 5);
        Math_SmoothStepToS(&dynamicVtx->n.ob[2], targetVtx->n.ob[2], 5, 40, 5);
        Math_SmoothStepToS(&dynamicVtx->n.ob[1], targetVtx->n.ob[1], 2, 600, 3);
    }

    EnBigslime_CheckVtxWallBoundaries(this);

    // Checks if any collider is interacting with player. If so, player is grabbed
    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        if (this->bigslimeCollider[i].base.atFlags & AT_HIT) {
            break;
        }
    }

    if (i != BIGSLIME_NUM_RING_FACES) {
        player = GET_PLAYER(globalCtx);
        if (globalCtx->grabPlayer(globalCtx, player)) {
            player->actor.parent = &this->actor;
            EnBigslime_SetupCutscene(this);
            return;
        }
    }

    if (this->squishFlatTimer == 0) {
        EnBigslime_SetupRise(this);
    }
}

/**
 * This sets the target vertices into a thin cone shape
 *
 * Notes:
 *      - The flat surface of the cone is in the xz-plane at y = -1000
 *      - The radius of the flat surface of the cone is 1000 units
 *      - The apex/point of the cone is at the point (0, 1000, 0)
 *      - It is thin because it is smooth and thinner near the apex than a standard cone
 */
void EnBigslime_SetTargetVtxToThinCone(EnBigslime* this) {
    Vtx* targetVtx;
    Vtx* staticVtx;
    f32 xzDistVtx;
    f32 xzScaleVtx;
    s32 i;
    s32 j;
    s16 upperSphereCos;
    f32 lowerSphereCos;
    s32 targetVtxY;

    // Top vtx of the sphere
    sBigslimeTargetVtx[0].n.ob[1] = BIGSLIME_RADIUS_S;
    sBigslimeTargetVtx[0].n.ob[0] = 0;
    sBigslimeTargetVtx[0].n.ob[2] = 0;

    for (i = 1; i < BIGSLIME_NUM_RING_FACES / 2; i++) {
        upperSphereCos = (((cos_rad((i - 1) * (M_PI / 5)) + 1.0f) * 0.925f) + -0.85f) * BIGSLIME_RADIUS_F;
        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            staticVtx = &sBigslimeStaticVtx[j];
            targetVtx = &sBigslimeTargetVtx[j];
            xzDistVtx = sqrtf(SQ(staticVtx->n.ob[0]) + SQ(staticVtx->n.ob[2]));
            xzDistVtx = (BIGSLIME_RADIUS_F / (5.0f * xzDistVtx)) * i;
            // xzDistVtx is always less than 500.0f
            xzScaleVtx =
                xzDistVtx < 500.0f ? xzDistVtx * 0.75f : BIGSLIME_RADIUS_F - ((BIGSLIME_RADIUS_F - xzDistVtx) * 0.75f);

            targetVtx->n.ob[0] = staticVtx->n.ob[0] * xzScaleVtx;
            targetVtx->n.ob[2] = staticVtx->n.ob[2] * xzScaleVtx;
            targetVtx->n.ob[1] = upperSphereCos;
        }
    }

    for (; i < BIGSLIME_NUM_RING_FACES; i++) {
        lowerSphereCos = cos_rad((i - BIGSLIME_NUM_RING_FACES / 2) * (M_PI / BIGSLIME_NUM_RING_FACES));
        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            staticVtx = &sBigslimeStaticVtx[j];
            targetVtx = &sBigslimeTargetVtx[j];
            targetVtx->n.ob[0] = staticVtx->n.ob[0];
            targetVtx->n.ob[2] = staticVtx->n.ob[2];
            targetVtxY = (s16)(((lowerSphereCos * 0.05f) + -1.0f) * BIGSLIME_RADIUS_F);
            targetVtx->n.ob[1] = targetVtxY;
        }
    }

    // Bottom vtx of the sphere
    sBigslimeTargetVtx[BIGSLIME_NUM_VTX - 1].n.ob[1] = -BIGSLIME_RADIUS_S;
    sBigslimeTargetVtx[BIGSLIME_NUM_VTX - 1].n.ob[0] = 0;
    sBigslimeTargetVtx[BIGSLIME_NUM_VTX - 1].n.ob[2] = 0;
}

/**
 * This sets the target vertices into a thin inverse cone shape
 *
 * Notes:
 *      - The flat surface of the cone is in the xz-plane at y = +1000
 *      - The radius of the flat surface of the cone is 1000 units
 *      - The apex/point of the cone is at the point (0, -1000, 0)
 *      - It is thin because it is smooth and thinner near the apex than a standard cone
 */
void EnBigslime_SetTargetVtxToInverseCone(EnBigslime* this) {
    Vtx* targetVtx;
    Vtx* staticVtx;
    f32 xzDistVtx;
    f32 upperSphereCos;
    s16 lowerSphereCos;
    f32 xzScaleVtx;
    s32 i;
    s32 j;
    s32 vtxY;

    // Top vtx of the sphere
    sBigslimeTargetVtx[0].n.ob[1] = BIGSLIME_RADIUS_S;
    sBigslimeTargetVtx[0].n.ob[0] = 0;
    sBigslimeTargetVtx[0].n.ob[2] = 0;

    for (i = 1; i < BIGSLIME_NUM_RING_FACES / 2; i++) {
        upperSphereCos = cos_rad((i - 1) * (M_PI / 10));
        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            staticVtx = &sBigslimeStaticVtx[j];
            targetVtx = &sBigslimeTargetVtx[j];
            targetVtx->n.ob[0] = staticVtx->n.ob[0];
            targetVtx->n.ob[2] = staticVtx->n.ob[2];
            vtxY = (s16)(((upperSphereCos * 0.1f) + 0.9f) * BIGSLIME_RADIUS_F);
            targetVtx->n.ob[1] = vtxY;
        }
    }

    for (; i < BIGSLIME_NUM_RING_FACES; i++) {
        lowerSphereCos =
            (((cos_rad((i - BIGSLIME_NUM_RING_FACES / 2) * (M_PI / 5)) + 1) * 0.925f) + -1.0f) * BIGSLIME_RADIUS_F;
        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            staticVtx = &sBigslimeStaticVtx[j];
            targetVtx = &sBigslimeTargetVtx[j];
            xzDistVtx = sqrtf(SQ(staticVtx->n.ob[0]) + SQ(staticVtx->n.ob[2]));
            xzDistVtx = (BIGSLIME_RADIUS_F / (6.0f * xzDistVtx)) * (BIGSLIME_NUM_RING_FACES - i);
            // xzDistVtx is always less than 500.0f
            xzScaleVtx =
                xzDistVtx < 500.0f ? xzDistVtx * 0.75f : BIGSLIME_RADIUS_F - ((BIGSLIME_RADIUS_F - xzDistVtx) * 0.75f);

            targetVtx->n.ob[0] = staticVtx->n.ob[0] * xzScaleVtx;
            targetVtx->n.ob[2] = staticVtx->n.ob[2] * xzScaleVtx;
            targetVtx->n.ob[1] = lowerSphereCos;
        }
    }

    // Bottom vtx of the sphere
    sBigslimeTargetVtx[BIGSLIME_NUM_VTX - 1].n.ob[1] = -BIGSLIME_RADIUS_S;
    sBigslimeTargetVtx[BIGSLIME_NUM_VTX - 1].n.ob[0] = 0;
    sBigslimeTargetVtx[BIGSLIME_NUM_VTX - 1].n.ob[2] = 0;
}

void EnBigslime_SetTargetVtxToStaticVtx(EnBigslime* this) {
    s32 i;
    Vtx* staticVtx;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        staticVtx = &sBigslimeStaticVtx[i];
        sBigslimeTargetVtx[i].n.ob[0] = staticVtx->n.ob[0];
        sBigslimeTargetVtx[i].n.ob[2] = staticVtx->n.ob[2];
        sBigslimeTargetVtx[i].n.ob[1] = staticVtx->n.ob[1];
    }
}

void EnBigslime_SetupRise(EnBigslime* this) {
    Animation_PlayLoop(&this->skelAnime, &gGekkoSwimForwardAnim);
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_JUMP_ABOVE);
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_UTSUBO_APPEAR_TRG);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_B_SLIME_JUMP1);
    this->riseCounter = 0;
    this->bigslimeCollider[0].base.atFlags &= ~AT_ON;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    EnBigslime_SetTargetVtxToThinCone(this);
    this->actor.world.rot.y = this->gekkoRot.y;
    this->actionFunc = EnBigslime_Rise;
}

/**
 * Moves Fused Jelly from a squish pancake on the floor to the ceiling
 *
 * Notes:
 *      - When riseCounter == 0, Fused Jelly is un-squishing into a thin cone shape
 *      - When riseCounter == 1, Fused Jelly is deforming into an inverse thin cone shape
 *      - When riseCounter == 2 to 9, Fused Jelly is returning to its default spherical shape
 *      - When riseCounter == 10, Fused Jelly starts moving on the ceiling
 */
void EnBigslime_Rise(EnBigslime* this, GlobalContext* globalCtx) {
    Vtx* dynamicVtx;
    Vtx* targetVtx;
    s32 i;

    if (this->riseCounter < 2) {
        Math_SmoothStepToF(&this->actor.scale.x, 0.13f, 0.2f, 0.039f, 0.0038999997f);
        Math_ScaledStepToS(&this->gekkoRot.x, -0x4000, 0x400);
    } else {
        Math_SmoothStepToF(&this->actor.scale.x, 0.15f, 0.4f, 0.015000001f, 0.0015f);
        Math_ScaledStepToS(&this->gekkoRot.x, 0, 0x400);
    }

    this->actor.scale.z = this->actor.scale.x;
    EnBigslime_CheckVtxWallBoundaries(this);
    if (this->riseCounter == 0) {
        if (Math_SmoothStepToF(&this->actor.scale.y, 0.3f, 0.5f, 0.015000001f, 0.00075f) < 0.01f) {
            this->riseCounter = 1;
            EnBigslime_SetTargetVtxToInverseCone(this);
        }

        for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
            dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];
            targetVtx = &sBigslimeTargetVtx[i];
            Math_SmoothStepToS(&dynamicVtx->n.ob[1], targetVtx->n.ob[1], 5, 550, 3);
            Math_SmoothStepToS(&dynamicVtx->n.ob[0], targetVtx->n.ob[0], 5, 40, 5);
            Math_SmoothStepToS(&dynamicVtx->n.ob[2], targetVtx->n.ob[2], 5, 40, 5);
        }

    } else if (this->riseCounter == 1) {
        if (Math_SmoothStepToF(&this->actor.scale.y, 0.075f, 0.4f, 0.0075000003f, 0.00075f) < 0.01f) {
            EnBigslime_SetTargetVtxToStaticVtx(this);
            this->riseCounter++;
        }

        for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
            dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];
            targetVtx = &sBigslimeTargetVtx[i];
            Math_SmoothStepToS(&dynamicVtx->n.ob[1], targetVtx->n.ob[1], 5, 50, 3);
            Math_SmoothStepToS(&dynamicVtx->n.ob[0], targetVtx->n.ob[0], 5, 40, 5);
            Math_SmoothStepToS(&dynamicVtx->n.ob[2], targetVtx->n.ob[2], 5, 40, 5);
        }

        this->actor.world.pos.y = GBT_ROOM_5_MAX_Y - (this->actor.scale.y * BIGSLIME_RADIUS_F);
    } else if (this->riseCounter == 10) {
        EnBigslime_SetupMoveOnCeiling(this);
    } else {
        this->riseCounter++;
        for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
            dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];
            targetVtx = &sBigslimeTargetVtx[i];
            Math_SmoothStepToS(&dynamicVtx->n.ob[1], targetVtx->n.ob[1], 5, 550, 3);
            Math_SmoothStepToS(&dynamicVtx->n.ob[0], targetVtx->n.ob[0], 5, 40, 5);
            Math_SmoothStepToS(&dynamicVtx->n.ob[2], targetVtx->n.ob[2], 5, 40, 5);
        }
    }

    EnBigslime_CheckVtxWallBoundaries(this);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 10, 0x800, 0x80);
}

void EnBigslime_SetupCutsceneGrabPlayer(EnBigslime* this, GlobalContext* globalCtx) {
    Camera* mainCam = Play_GetCamera(globalCtx, MAIN_CAM);
    s16 yaw;

    Play_CameraSetAtEye(globalCtx, this->subCamId, &mainCam->at, &mainCam->eye);
    this->grabPlayerTimer = 15;
    this->wavySurfaceTimer = 0;
    this->bigslimeCollider[0].base.atFlags &= ~AT_ON;
    this->actor.world.rot.y = Actor_YawToPoint(&this->actor, &this->actor.home.pos);
    yaw = func_800DFCDC(GET_ACTIVE_CAM(globalCtx)) - this->actor.world.rot.y;

    if (yaw > 0x4000) {
        this->subCamYawGrabPlayer = -0x2000;
    } else if (yaw > 0) {
        this->subCamYawGrabPlayer = -0x6000;
    } else if (yaw < -0x4000) {
        this->subCamYawGrabPlayer = 0x2000;
    } else {
        this->subCamYawGrabPlayer = 0x6000;
    }

    this->subCamYawGrabPlayer += this->actor.world.rot.y;
    Animation_PlayLoop(&this->skelAnime, &gGekkoBoxingStanceAnim);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_B_SLIME_EAT);
    this->actionFunc = EnBigslime_CutsceneGrabPlayer;
}

void EnBigslime_CutsceneGrabPlayer(EnBigslime* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 invgrabPlayerTimer;
    f32 magPosXZOffset;
    Vtx* dynamicVtx;
    s32 i;
    s32 j;

    player->unk_AE8 = 0;
    Math_ScaledStepToS(&this->gekkoRot.x, 0, 0x400);
    EnBigslime_UpdateCameraGrabPlayer(this, globalCtx);
    if (this->grabPlayerTimer > 0) {
        invgrabPlayerTimer = 1.0f / this->grabPlayerTimer;

        this->actor.scale.x = F32_LERPIMP(this->actor.scale.x, 0.15f, invgrabPlayerTimer);
        this->actor.scale.y = F32_LERPIMP(this->actor.scale.y, 0.075f, invgrabPlayerTimer);
        this->actor.scale.z = this->actor.scale.x;

        player->actor.world.pos.x = F32_LERPIMP(player->actor.world.pos.x, this->actor.world.pos.x, invgrabPlayerTimer);
        player->actor.world.pos.z = F32_LERPIMP(player->actor.world.pos.z, this->actor.world.pos.z, invgrabPlayerTimer);
        player->actor.world.pos.y = F32_LERPIMP(
            player->actor.world.pos.y, this->actor.world.pos.y + this->actor.scale.y * -500.0f, invgrabPlayerTimer);

        for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
            dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];

            // loop over x, y, z
            for (j = 0; j < 3; j++) {
                // Linearly interpolate dynamicVtx --> staticVtx
                dynamicVtx->n.ob[j] +=
                    (s16)((sBigslimeStaticVtx[i].n.ob[j] - dynamicVtx->n.ob[j]) * invgrabPlayerTimer);
            }
        }

        invgrabPlayerTimer = (15 - this->grabPlayerTimer) / 15.0f;
        magPosXZOffset = invgrabPlayerTimer * -50.0f;
        this->gekkoPosOffset.x = Math_SinS(this->gekkoRot.y) * magPosXZOffset;
        this->gekkoPosOffset.y = invgrabPlayerTimer * -40.0f;
        this->gekkoPosOffset.z = Math_CosS(this->gekkoRot.y) * magPosXZOffset;
        this->grabPlayerTimer--;
        if (this->grabPlayerTimer == 0) {
            EnBigslime_SetupAttackPlayerInBigslime(this);
        }
    }
}

void EnBigslime_SetupAttackPlayerInBigslime(EnBigslime* this) {
    Animation_PlayOnce(&this->skelAnime, sGekkoAttackAnimations[(s32)Rand_ZeroFloat(3.0f) % 3]);
    this->numGekkoMeleeAttacks = (s32)Rand_ZeroFloat(3.0f) + 1;
    this->numGekkoPosGrabPlayer = 6;
    this->actionFunc = EnBigslime_AttackPlayerInBigslime;
}

void EnBigslime_AttackPlayerInBigslime(EnBigslime* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 pitch = this->scaleFactor * 0x3333; // polar (zenith) angle

    player->unk_AE8 = 0;
    Math_ScaledStepToS(&this->gekkoRot.x, 0, 0x400);
    EnBigslime_UpdateCameraGrabPlayer(this, globalCtx);
    EnBigslime_UpdateWavySurface(this);

    if (this->scaleFactor != 0) {
        this->scaleFactor--;
    }

    // Checks to see if it's the frame of impact
    if (((this->skelAnime.animation == &gGekkoJabPunchAnim) &&
         Animation_OnFrame(&this->skelAnime, 2.0f)) || // Jab punch makes impact on frame 2 of animation
        ((this->skelAnime.animation == &gGekkoHookPunchAnim) &&
         Animation_OnFrame(&this->skelAnime, 9.0f)) || // Hook Punch makes impact on frames 9 of animation
        ((this->skelAnime.animation == &gGekkoKickAnim) &&
         Animation_OnFrame(&this->skelAnime, 2.0f))) { // Kick makes impact on frame 2 of animation
        this->scaleFactor = 10;
        player->actor.world.pos.x += 20.0f * Math_SinS(this->gekkoRot.y);
        player->actor.world.pos.z += 20.0f * Math_CosS(this->gekkoRot.y);
        EnBigslime_JerkCameraPlayerHit(this, globalCtx);
        if (this->skelAnime.animation == &gGekkoKickAnim) {
            EnBigslime_GekkoSfxInsideBigslime(this, NA_SE_EN_FROG_KICK);
        } else {
            EnBigslime_GekkoSfxInsideBigslime(this, NA_SE_EN_FROG_PUNCH1);
        }
    } else {
        Math_StepToF(&player->actor.world.pos.x, this->actor.world.pos.x, 4.0f);
        Math_StepToF(&player->actor.world.pos.z, this->actor.world.pos.z, 4.0f);
    }

    EnBigslime_Scale(this, pitch, ((this->scaleFactor * 0.08f) + 0.2f) * 0.15f,
                     ((this->scaleFactor * 0.08f) + 0.2f) * 0.05f);
    player->actor.world.pos.y = this->actor.world.pos.y + (this->actor.scale.y * -500.0f);
    if (this->isAnimUpdate) {
        this->numGekkoMeleeAttacks--;
        if (this->numGekkoMeleeAttacks == 0) {
            this->numGekkoPosGrabPlayer--;

            if ((gSaveContext.health < 5) || (this->numGekkoPosGrabPlayer == 0)) {
                this->numGekkoPosGrabPlayer = 0;
                this->gekkoRot.y = this->actor.world.rot.y;
                this->gekkoPosOffset.x = Math_SinS(this->gekkoRot.y) * -50.0f;
                this->gekkoPosOffset.z = Math_CosS(this->gekkoRot.y) * -50.0f;
                EnBigslime_SetupWindupThrowPlayer(this);
                return;
            }

            globalCtx->damagePlayer(globalCtx, -4);
            func_800B8E58(&player->actor, player->ageProperties->unk_92 + 0x6805);
            this->gekkoRot.y += (s16)(Rand_S16Offset(0x4000, 0x4000) * (Rand_ZeroOne() < 0.5f ? -1 : 1));
            this->gekkoPosOffset.x = Math_SinS(this->gekkoRot.y) * -50.0f;
            this->gekkoPosOffset.z = Math_CosS(this->gekkoRot.y) * -50.0f;
            this->numGekkoMeleeAttacks = (s32)Rand_ZeroFloat(3.0f) + 1;
        }
        Animation_PlayOnce(&this->skelAnime, sGekkoAttackAnimations[((s32)Rand_ZeroFloat(3.0f) % 3)]);
    }

    func_800B9010(&this->actor, NA_SE_EN_B_SLIME_PUNCH_MOVE - SFX_FLAG);
}

/**
 * Calculates the surface perturbation (multiplicative offset from the static vertices)
 * used to update dynamic vertices when bigslime is about to throw/eject player from inside
 * at the end of the grab-player cutscene
 *
 * A unit normal vector (unitVec) is used to represent the direction player will be thrown.
 * This unit vector has the following spherical coordinates:
 *     - radius = 1
 *     - yaw (azimuthal angle) = this->actor.world.rot.y
 *     - pitch (polar/zenith angle) = M_PI / 4
 *
 * This unit normal vector is converted into x-y-z coordinates and dot-producted with
 * the model coordinates of each individual vertex. The surface perturbation is then
 * set to be linearly proportional to this dot product
 *
 * This leads to the bending shape observed during windup as player is about being thrown out of bigslime
 */
void EnBigslime_SetupWindupThrowPlayer(EnBigslime* this) {
    Vtx* dynamicVtx;
    f32 dotXYZ;
    f32 unitVecX = Math_SinS(this->actor.world.rot.y) * M_SQRT1_2;
    f32 unitVecZ = Math_CosS(this->actor.world.rot.y) * M_SQRT1_2;
    s32 i;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];

        // vector dot product between each dynamicVtx and the unit normal vector describing player's thrown direction
        dotXYZ = (dynamicVtx->n.ob[0] * unitVecX + dynamicVtx->n.ob[1] * M_SQRT1_2 + dynamicVtx->n.ob[2] * unitVecZ) *
                 0.001f;
        if (dotXYZ < 0.01f) {
            this->vtxSurfacePerturbation[i] = 0.0f;
        } else {
            this->vtxSurfacePerturbation[i] = dotXYZ * 0.5f;
        }
    }

    this->windupPunchTimer = 27;
    Animation_PlayOnce(&this->skelAnime, &gGekkoWindupPunchAnim);
    EnBigslime_GekkoSfxInsideBigslime(this, NA_SE_EN_FROG_PUNCH2);
    this->actionFunc = EnBigslime_WindupThrowPlayer;
}

void EnBigslime_WindupThrowPlayer(EnBigslime* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    f32 scale;
    f32 invWindupPunchTimer;
    Vtx* dynamicVtx;
    Vtx* staticVtx;
    s32 i;
    s32 j;

    this->windupPunchTimer--;
    EnBigslime_UpdateCameraGrabPlayer(this, globalCtx);
    if (this->windupPunchTimer > 0) {
        invWindupPunchTimer = 1.0f / this->windupPunchTimer;
        scale = cos_rad(this->windupPunchTimer * (M_PI / 27)) + 1.0f;
        player->actor.world.pos.y = this->actor.world.pos.y + (this->actor.scale.y * -500.0f);

        // Linearly interpolate gekkoRot.y --> this->actor.world.rot.y
        this->gekkoRot.y += (s16)((s16)(this->actor.world.rot.y - this->gekkoRot.y) * invWindupPunchTimer);
        this->gekkoPosOffset.x = Math_SinS(this->gekkoRot.y) * -50.0f;
        this->gekkoPosOffset.z = Math_CosS(this->gekkoRot.y) * -50.0f;
    } else {
        if (this->windupPunchTimer == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_B_SLIME_REVERSE);
            EnBigslime_GekkoSfxInsideBigslime(this, NA_SE_EN_FROG_PUNCH1);
        }

        scale = 0.5f - cos_rad(-this->windupPunchTimer * (M_PI / 5)) * 0.5f;
        if (this->windupPunchTimer == -5) {
            if (player->stateFlags2 & 0x80) {
                player->actor.parent = NULL;
                player->unk_AE8 = 100;
            }

            player->actor.velocity.y = 0.0f;
            func_800B8D50(globalCtx, &this->actor, 10.0f, this->actor.world.rot.y, 10.0f, 4);
            EnBigslime_SetupSetDynamicVtxThrowPlayer(this, globalCtx);
        }

        player->actor.world.pos.x =
            (Math_SinS(this->actor.world.rot.y) * (1060.5f * this->actor.scale.x) * scale) + this->actor.world.pos.x;
        player->actor.world.pos.z =
            (Math_CosS(this->actor.world.rot.y) * (1060.5f * this->actor.scale.z) * scale) + this->actor.world.pos.z;
        player->actor.world.pos.y =
            this->actor.world.pos.y + (this->actor.scale.y * -500.0f) + (1060.5f * this->actor.scale.y * scale);
        invWindupPunchTimer = 1.0f / (this->windupPunchTimer + 6);
        // this->gekkoPosOffset *= (1 - invWindupPunchTimer)
        this->gekkoPosOffset.x -= this->gekkoPosOffset.x * invWindupPunchTimer;
        this->gekkoPosOffset.y -= this->gekkoPosOffset.y * invWindupPunchTimer;
        this->gekkoPosOffset.z -= this->gekkoPosOffset.z * invWindupPunchTimer;
        scale = 2.0f - (3.0f * scale);
    }

    // Deforming Bigslime during the final windup punch while grabbing player using vtxSurfacePerturbation
    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];
        staticVtx = &sBigslimeStaticVtx[i];
        if (this->vtxSurfacePerturbation[i] != 0.0f) {
            if (this->windupPunchTimer > 0) {
                // loop over x, y, z
                for (j = 0; j < 3; j++) {
                    // Linearly interpolate dynamicVtx --> staticVtx * (1 - scale * vtxSurfacePerturbation)
                    dynamicVtx->n.ob[j] += (s16)(
                        ((staticVtx->n.ob[j] - (s32)(scale * staticVtx->n.ob[j] * this->vtxSurfacePerturbation[i])) -
                         dynamicVtx->n.ob[j]) *
                        invWindupPunchTimer);
                }
            } else {
                // loop over x, y, z
                for (j = 0; j < 3; j++) {
                    // Directly set dynamicVtx --> staticVtx * (1 - scale * vtxSurfacePerturbation)
                    dynamicVtx->n.ob[j] =
                        staticVtx->n.ob[j] - (s32)((scale * staticVtx->n.ob[j]) * this->vtxSurfacePerturbation[i]);
                }
            }
        } else {
            if (this->windupPunchTimer > 0) {
                // loop over x, y, z
                for (j = 0; j < 3; j++) {
                    // Linearly interpolate dynamicVtx --> staticVtx
                    dynamicVtx->n.ob[j] += (s16)((staticVtx->n.ob[j] - dynamicVtx->n.ob[j]) * invWindupPunchTimer);
                }
            }
        }
    }
}

void EnBigslime_SetupSetDynamicVtxThrowPlayer(EnBigslime* this, GlobalContext* globalCtx) {
    this->grabPlayerTimer = 10;
    EnBigslime_SetTargetVtxToWideCone(this);
    EnBigslime_CheckVtxWallBoundaries(this);
    EnBigslime_EndCutscene(this, globalCtx);
    this->actionFunc = EnBigslime_SetDynamicVtxThrowPlayer;
}

/**
 * Restores bigslime to wide cone after player is thrown
 */
void EnBigslime_SetDynamicVtxThrowPlayer(EnBigslime* this, GlobalContext* globalCtx) {
    f32 invThrowPlayerTimer;
    Vtx* targetVtx;
    Vtx* dynamicVtx;
    s32 i;
    s32 j;

    this->throwPlayerTimer--;
    if (this->throwPlayerTimer > 0) {
        invThrowPlayerTimer = 1.0f / this->throwPlayerTimer;
        for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
            targetVtx = &sBigslimeTargetVtx[i];
            dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];

            // loop over x, y, z
            for (j = 0; j < 3; j++) {
                // Linearly interpolate dynamicVtx --> targetVtx
                dynamicVtx->n.ob[j] += (s16)((targetVtx->n.ob[j] - dynamicVtx->n.ob[j]) * invThrowPlayerTimer);
            }
        }

        this->actor.scale.x = F32_LERPIMP(this->actor.scale.x, 0.2f, invThrowPlayerTimer);
        this->actor.scale.y = F32_LERPIMP(this->actor.scale.y, 0.05f, invThrowPlayerTimer);
        this->actor.scale.z = this->actor.scale.x;
        EnBigslime_CheckVtxWallBoundaries(this);
    }

    if (this->throwPlayerTimer == 0) {
        EnBigslime_SetupRise(this);
    }
}

/**
 * Sets the frozen effect as a seed on the bottom 4 nodes of a sphere
 * The frozen effect is initially set to be dimmer the farther from the initial seed you are
 */
void EnBigslime_SetupFreeze(EnBigslime* this) {
    Vtx* targetVtx;
    Vtx* dynamicVtx;
    s32 i;
    s32 j;

    this->actor.speedXZ = 0.0f;
    this->freezeTimer = 40;
    if (this->actor.bgCheckFlags & 1) {
        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
    } else if ((this->actionFunc == EnBigslime_Rise) && (this->riseCounter < 2)) {
        this->actor.gravity = -2.0f;
    }

    this->bigslimeCollider[0].base.atFlags &= ~AT_ON;
    this->bigslimeCollider[0].base.acFlags &= ~AC_ON;

    // Resets frozen effect alpha to 0
    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][i];
        targetVtx = &sBigslimeTargetVtx[i];
        for (j = 0; j < 3; j++) {
            targetVtx->n.ob[j] = dynamicVtx->n.ob[j];
        }
        targetVtx->n.a = 0;
    }

    // Initalizes frozen effect alpha near bottom of sphere by increasing levels of alpha
    for (i = 0; i < 20; i++) {
        sBigslimeTargetVtx[i + 138].n.a = 10 * i;
    }

    // Initalizes/seeds frozen effect alpha in bottom 4 nodes in vtx sphere to highest level of alpha
    for (i = 0; i < 4; i++) {
        sBigslimeTargetVtx[i + 158].n.a = 200;
    }

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        this->bigslimeCollider[i].base.ocFlags1 &= ~OC1_NO_PUSH;
    }

    this->actionFuncStored = this->actionFunc;
    this->actionFunc = EnBigslime_Freeze;
}

/**
 * Propogates the frozen effect from the seed at the bottom out through all vertices
 */
void EnBigslime_Freeze(EnBigslime* this, GlobalContext* globalCtx) {
    f32 randFloat;
    Vtx* targetVtx;
    Vtx* dynamicVtx;
    s32 vtxIceUpdate;
    s32 vtxIceSeed;
    s32 j;

    if (this->freezeTimer) {
        this->freezeTimer--;
    }

    this->skelAnime.playSpeed = this->freezeTimer / 40.0f;
    vtxIceSeed = this->freezeTimer * 4;
    for (vtxIceUpdate = 0; vtxIceUpdate < 4; vtxIceUpdate++, vtxIceSeed++) {
        if (vtxIceSeed < BIGSLIME_NUM_VTX) {
            dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][vtxIceSeed];
            targetVtx = &sBigslimeTargetVtx[vtxIceSeed];
            randFloat = randPlusMinusPoint5Scaled(40.0f);
            dynamicVtx->n.ob[0] += (s16)(randFloat / this->actor.scale.x);
            dynamicVtx->n.ob[1] += (s16)(randFloat / this->actor.scale.y);
            dynamicVtx->n.ob[2] += (s16)(randFloat / this->actor.scale.z);
            if (((dynamicVtx->n.ob[1] * this->actor.scale.y) + this->actor.world.pos.y) < GBT_ROOM_5_MIN_Y) {
                dynamicVtx->n.ob[1] = ((GBT_ROOM_5_MIN_Y - this->actor.world.pos.y) / this->actor.scale.y) - 1.0f;
            }

            for (j = 0; j < 3; j++) {
                targetVtx->n.ob[j] = dynamicVtx->n.ob[j];
            }
        }
    }

    for (vtxIceUpdate = 4; vtxIceUpdate < BIGSLIME_NUM_VTX; vtxIceUpdate++) {
        sBigslimeTargetVtx[vtxIceUpdate - 4].n.a = sBigslimeTargetVtx[vtxIceUpdate].n.a;
    }

    func_800B9010(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
    if (this->actor.bgCheckFlags & 2) {
        if (this->freezeTimer == 0) {
            EnBigslime_BreakIntoMinislime(this, globalCtx);
        } else {
            this->bigslimeCollider[0].base.acFlags |= AC_ON;
            EnBigslime_AddIceShardEffect(this, globalCtx);
            EnBigslime_SetupSquishFlat(this);
        }
    } else if (this->freezeTimer == 0) {
        if (!(this->actor.bgCheckFlags & 1)) {
            EnBigslime_SetupFrozenFall(this);
        } else {
            EnBigslime_SetupFrozenGround(this);
        }
    }
}

void EnBigslime_SetupFrozenGround(EnBigslime* this) {
    this->freezeTimer = 80;
    this->bigslimeCollider[0].base.acFlags |= AC_ON;
    this->actionFunc = EnBigslime_FrozenGround;
}

void EnBigslime_FrozenGround(EnBigslime* this, GlobalContext* globalCtx) {
    f32 invFreezerTimer;
    s32 randSign;
    f32 randFloat;

    this->freezeTimer--;
    if (this->freezeTimer == 0) {
        EnBigslime_AddIceShardEffect(this, globalCtx);
        EnBigslime_SetTargetVtxFromPreFrozen(this);
    } else if (this->freezeTimer == 40) {
        Math_Vec3f_Copy(&this->frozenPos, &this->actor.world.pos);
    } else if ((this->freezeTimer < 20) || ((this->freezeTimer < 40) && ((this->freezeTimer % 2) != 0))) {
        invFreezerTimer = 1.0f / this->freezeTimer;

        // clang-format off
        randFloat = Rand_ZeroFloat(4.0f * invFreezerTimer);
        randSign = Rand_ZeroOne() < 0.5f ? -1 : 1; \
        this->actor.world.pos.x = randSign * (1.0f + invFreezerTimer + randFloat) + this->frozenPos.x;
        // clang-format on

        randFloat = Rand_ZeroFloat(4.0f * invFreezerTimer);
        randSign = Rand_ZeroOne() < 0.5f ? -1 : 1;
        this->actor.world.pos.z = randSign * (1.0f + invFreezerTimer + randFloat) + this->frozenPos.z;
    }
}

void EnBigslime_SetupMelt(EnBigslime* this) {
    s32 i;

    this->bigslimeCollider[0].base.acFlags &= ~AC_ON;
    for (i = 0; i < 2; i++) {
        sBigslimeTargetVtx[i].n.a = 0;
    }

    for (i = 0; i < 20; i++) {
        sBigslimeTargetVtx[i + 2].n.a = 10 * i;
    }

    this->meltCounter = 0;
    this->actionFunc = EnBigslime_Melt;
}

void EnBigslime_Melt(EnBigslime* this, GlobalContext* globalCtx) {
    static Vec3f iceSmokeVelocity = { 0.0f, 2.0f, 0.0f };
    Vec3f iceSmokePos;
    Vtx* targetVtx;
    Vtx* dynamicVtx;
    s32 i;

    this->meltCounter++;
    if ((this->meltCounter < 70) && ((this->meltCounter % 2) != 0)) {
        dynamicVtx =
            &sBigslimeDynamicVtx[this->dynamicVtxState][(s32)Rand_ZeroFloat(BIGSLIME_NUM_VTX) % BIGSLIME_NUM_VTX];
        iceSmokePos.x = (dynamicVtx->n.ob[0] * this->actor.scale.x) + this->actor.world.pos.x;
        iceSmokePos.y = (dynamicVtx->n.ob[1] * this->actor.scale.y) + this->actor.world.pos.y;
        iceSmokePos.z = (dynamicVtx->n.ob[2] * this->actor.scale.z) + this->actor.world.pos.z;
        EffectSsIceSmoke_Spawn(globalCtx, &iceSmokePos, &iceSmokeVelocity, &gZeroVec3f, 600);
    }

    func_800B9010(&this->actor, NA_SE_EV_ICE_MELT_LEVEL - SFX_FLAG);
    for (i = 159; i >= 0; i--) {
        sBigslimeTargetVtx[i + 2].n.a = sBigslimeTargetVtx[i].n.a;
    }

    if (this->meltCounter == 100) {
        EnBigslime_SetTargetVtxFromPreFrozen(this);
    } else if (this->meltCounter == 50) {
        globalCtx->envCtx.unk_C3 = 0xFF;
    }
}

void EnBigslime_SetupFrozenFall(EnBigslime* this) {
    s32 i;

    this->bigslimeCollider[0].base.atFlags |= AT_ON;
    this->actor.gravity = -2.0f;
    this->actor.velocity.y = 0.0f;

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        this->bigslimeCollider[i].base.ocFlags1 |= OC1_NO_PUSH;
    }

    this->actionFunc = EnBigslime_FrozenFall;
}

void EnBigslime_FrozenFall(EnBigslime* this, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        if (this->bigslimeCollider[i].base.atFlags & AT_HIT) {
            break;
        }
    }

    if (i != BIGSLIME_NUM_RING_FACES) {
        func_800B8D50(globalCtx, &this->actor, 7.0f, this->actor.yawTowardsPlayer, 5.0f, 0x10);
    }

    if (this->actor.bgCheckFlags & 1) {
        EnBigslime_BreakIntoMinislime(this, globalCtx);
    }
}

void EnBigslime_SetupJumpGekko(EnBigslime* this) {
    Animation_PlayLoop(&this->skelAnime, &gGekkoJumpForwardAnim);
    this->actor.speedXZ = 8.0f;
    this->jumpTimer = 100;
    this->actor.world.rot.y = this->gekkoRot.y;
    this->gekkoYaw = this->actor.yawTowardsPlayer + 0x8000;
    this->actionFunc = EnBigslime_JumpGekko;
}

void EnBigslime_JumpGekko(EnBigslime* this, GlobalContext* globalCtx) {
    s16 yaw;
    s16 yawDiff;

    if (this->actor.bgCheckFlags & 1) {
        this->gekkoCollider.base.acFlags |= AC_ON;
        this->actor.flags |= 1;
    }

    this->jumpTimer--;
    if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
        EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_JUMP);
    } else if (Animation_OnFrame(&this->skelAnime, 11.0f)) {
        EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EV_WALK_WATER);
    }

    if (!(this->actor.bgCheckFlags & 1) || ((this->skelAnime.curFrame > 1.0f) && (this->skelAnime.curFrame < 12.0f))) {
        this->actor.speedXZ = 8.0f;
    } else {
        this->actor.speedXZ = 0.0f;
    }

    if (Math_SmoothStepToS(&this->actor.world.rot.y, this->gekkoYaw, 5, 0x1000, 0x80) == 0) {
        if (Actor_XZDistanceToPoint(&this->actor, &this->actor.home.pos) > 240.0f) {
            yaw = Actor_YawToPoint(&this->actor, &this->actor.home.pos);
            yawDiff = yaw - (s16)(this->actor.yawTowardsPlayer + 0x8000);
            this->gekkoYaw =
                ABS_ALT(yawDiff) < 0x3000 ? yaw : (yawDiff / 2) + (s16)(this->actor.yawTowardsPlayer + 0x8000);
        } else {
            this->gekkoYaw = this->actor.yawTowardsPlayer + 0x8000;
        }
    }

    this->gekkoRot.y = this->actor.world.rot.y;
    if (this->jumpTimer == 0) {
        if (this->formBigslimeTimer == 0) {
            EnBigslime_SetupCutscene(this);
        } else {
            this->formBigslimeTimer--;
            EnBigslime_SetupIdleLookAround(this);
        }
    } else if ((this->actor.xzDistToPlayer > 300.0f) && (this->gekkoCollider.base.ocFlags1 & OC1_HIT)) {
        if ((this->gekkoCollider.base.oc->params == MINISLIME_IDLE) &&
            (this->gekkoCollider.base.oc->id == ACTOR_EN_MINISLIME)) {
            this->gekkoCollider.base.oc->params = MINISLIME_SETUP_GEKKO_THROW;
            this->minislimeToThrow = (EnMinislime*)this->gekkoCollider.base.oc;
            EnBigslime_SetupThrowMinislime(this);
        }
    }
}

void EnBigslime_SetupIdleLookAround(EnBigslime* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoNervousIdleAnim);
    this->idleTimer = 60;
    this->actor.speedXZ = 0.0f;
    if (BINANG_SUB(Actor_YawToPoint(&this->actor, &this->actor.home.pos), this->gekkoRot.y) > 0) {
        this->gekkoYaw = this->gekkoRot.y + ((u32)Rand_Next() >> 20) + 0x2000;
    } else {
        this->gekkoYaw = this->gekkoRot.y - ((u32)Rand_Next() >> 20) - 0x2000;
    }
    this->actionFunc = EnBigslime_IdleLookAround;
}

void EnBigslime_IdleLookAround(EnBigslime* this, GlobalContext* globalCtx) {
    s16 yawDiff;

    this->idleTimer--;
    if (this->isAnimUpdate) {
        if (Rand_ZeroOne() < 0.25f) {
            Animation_PlayOnce(&this->skelAnime, &gGekkoLookAroundAnim);
        } else {
            Animation_PlayOnce(&this->skelAnime, &gGekkoNervousIdleAnim);
        }
    }

    if ((this->skelAnime.animation == &gGekkoNervousIdleAnim) &&
        Math_ScaledStepToS(&this->gekkoRot.y, this->gekkoYaw, 0x400)) {
        if (BINANG_SUB(Actor_YawToPoint(&this->actor, &this->actor.home.pos), this->gekkoRot.y) > 0) {
            this->gekkoYaw = this->gekkoRot.y + ((u32)Rand_Next() >> 20) + 0x2000;
        } else {
            this->gekkoYaw = this->gekkoRot.y - ((u32)Rand_Next() >> 20) - 0x2000;
        }
    }

    yawDiff = this->actor.yawTowardsPlayer - this->gekkoRot.y;
    if ((this->idleTimer == 0) || ((ABS_ALT(yawDiff) < 0x2800) && (this->actor.xzDistToPlayer < 240.0f))) {
        EnBigslime_SetupIdleNoticePlayer(this);
    }
}

void EnBigslime_SetupIdleNoticePlayer(EnBigslime* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoSurpriseJumpAnim);
    this->gekkoYaw = this->gekkoRot.y + 0x8000;
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_JUMP_MID);
    this->actionFunc = EnBigslime_IdleNoticePlayer;
}

void EnBigslime_IdleNoticePlayer(EnBigslime* this, GlobalContext* globalCtx) {
    s16* yaw = &this->gekkoRot.y;

    if (this->skelAnime.curFrame > 10.0f) {
        Math_ScaledStepToS(yaw, this->gekkoYaw, 0x800);
    }
    if (this->isAnimUpdate) {
        EnBigslime_SetupJumpGekko(this);
    }
}

void EnBigslime_SetupThrowMinislime(EnBigslime* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoWindupPunchAnim);
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_HOLD_SLIME);
    this->actor.speedXZ = 0.0f;
    this->actionFunc = EnBigslime_ThrowMinislime;
}

void EnBigslime_ThrowMinislime(EnBigslime* this, GlobalContext* globalCtx) {
    s16 jumpTimerStored;

    Math_ScaledStepToS(&this->gekkoRot.y, this->actor.yawTowardsPlayer, 0x300);
    if (Animation_OnFrame(&this->skelAnime, 27.0f) &&
        (this->minislimeToThrow->actor.params == MINISLIME_SETUP_GEKKO_THROW)) {
        EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_THROW_SLIME);
        this->minislimeToThrow->actor.params = MINISLIME_GEKKO_THROW;
    }
    if (this->isAnimUpdate) {
        jumpTimerStored = this->jumpTimer; // Stores jumpTimer so it doesn't get overwritten back to 100
        EnBigslime_SetupJumpGekko(this);
        this->jumpTimer = jumpTimerStored;
    }
}

void EnBigslime_SetupDamageGekko(EnBigslime* this, s32 isNotFrozen) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGekkoDamagedAnim, -3.0f);
    this->gekkoCollider.base.acFlags &= ~AC_ON;
    this->damageSpinTimer = 20;
    this->actor.speedXZ = 10.0f;
    this->actor.gravity = -2.0f;
    this->actor.velocity.y = 0.0f;
    if (isNotFrozen) {
        func_800BE504(&this->actor, &this->gekkoCollider);
    }

    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_DAMAGE);
    if ((this->actor.flags & 0x2000) == 0x2000) {
        this->actor.flags &= ~0x2000;
    }

    this->actionFunc = EnBigslime_DamageGekko;
}

/**
 * Spins Gekko around as it takes damage
 */
void EnBigslime_DamageGekko(EnBigslime* this, GlobalContext* globalCtx) {
    s32 damageSpinTimer;

    this->damageSpinTimer--;
    damageSpinTimer = CLAMP_MAX(this->damageSpinTimer, 10);
    this->gekkoRot.y += 0x300 * damageSpinTimer;
    Math_StepToF(&this->actor.speedXZ, 0.0f, 0.5f);
    if (this->damageSpinTimer == 0) {
        EnBigslime_SetupCutscene(this);
    }
}

void EnBigslime_SetupStunGekko(EnBigslime* this) {
    if ((this->skelAnime.animation == &gGekkoJumpForwardAnim) ||
        (this->skelAnime.animation == &gGekkoSurpriseJumpAnim)) {
        this->skelAnime.curFrame = 1.0f;
    }
    this->actionFunc = EnBigslime_StunGekko;
    this->skelAnime.playSpeed = 0.0f;
    this->actor.speedXZ = 0.0f;
}

void EnBigslime_StunGekko(EnBigslime* this, GlobalContext* globalCtx) {
    this->stunTimer--;
    if (this->stunTimer == 0) {
        if (this->gekkoDrawEffect == GEKKO_DRAW_EFFECT_FROZEN) {
            EnBigslime_GekkoThaw(this, globalCtx);
            EnBigslime_SetupDamageGekko(this, false);
        } else {
            this->gekkoCollider.base.acFlags &= ~AC_ON;
            EnBigslime_SetupCutscene(this);
        }
    }
}

void EnBigslime_SetupCutsceneFormBigslime(EnBigslime* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoJumpUpAnim);
    this->gekkoCollider.base.acFlags &= ~AC_ON;
    this->actor.world.rot.x = -Actor_PitchToPoint(&this->actor, &this->actor.home.pos);
    this->actor.world.rot.y = Actor_YawToPoint(&this->actor, &this->actor.home.pos);
    this->actionFunc = EnBigslime_CutsceneFormBigslime;
    this->actor.speedXZ = 0.0f;
}

void EnBigslime_CutsceneFormBigslime(EnBigslime* this, GlobalContext* globalCtx) {
    EnBigslime_UpdateCameraFormingBigslime(this, globalCtx);
    Math_ScaledStepToS(&this->gekkoRot.y, this->actor.world.rot.y, 0x800);
    if (Animation_OnFrame(&this->skelAnime, 18.0f)) {
        EnBigslime_SetupFormBigslime(this);
    }
}

void EnBigslime_SetupFormBigslime(EnBigslime* this) {
    s32 i;

    this->actor.gravity = 0.0f;
    this->gekkoRot.x = 0x4000 - this->actor.world.rot.x;
    this->formBigslimeTimer = 0;
    this->wavySurfaceTimer = 0;
    this->actor.speedXZ = 25.0f;
    this->gekkoRot.y = this->actor.world.rot.y;
    this->formBigslimeCutsceneTimer = 2;

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        this->minislime[i]->actor.params = MINISLIME_FORM_BIGSLIME;
    }

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        this->bigslimeCollider[i].base.ocFlags1 |= OC1_NO_PUSH;
    }

    this->actor.update = EnBigslime_UpdateBigslime;
    this->actor.draw = EnBigslime_DrawBigslime;
    Actor_SetScale(&this->actor, 0.0f);
    this->vtxScaleX = 0.0f;
    this->vtxScaleZ = 0.0f;
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_JUMP_ABOVE);
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_UTSUBO_APPEAR_TRG);
    this->actionFunc = EnBigslime_FormBigslime;
}

void EnBigslime_FormBigslime(EnBigslime* this, GlobalContext* globalCtx) {
    f32 xzScale;
    f32 yScaleFactor;
    s32 i;

    EnBigslime_UpdateWavySurface(this);
    EnBigslime_UpdateCameraFormingBigslime(this, globalCtx);
    if (this->formBigslimeCutsceneTimer < 0) {
        Math_ScaledStepToS(&this->gekkoRot.x, 0, 0x400);
    } else if (this->actor.world.pos.y > (GBT_ROOM_5_MAX_Y - 100.0f)) {
        this->gekkoRot.x -= 0x4000;
        Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.speedXZ = 0.0f;
        Animation_PlayLoop(&this->skelAnime, &gGekkoSwimForwardAnim);
        this->formBigslimeCutsceneTimer--;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_B_SLIME_COMBINE);
    } else if (this->isAnimUpdate) {
        this->formBigslimeCutsceneTimer--;
        if (this->formBigslimeCutsceneTimer == 0) {
            Animation_PlayLoop(&this->skelAnime, &gGekkoSwimUpAnim);
        }
    }

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        if (this->minislime[i]->actor.params == MINISLIME_SETUP_DISAPPEAR) {
            this->minislime[i]->actor.params = MINISLIME_DISAPPEAR;
            this->minislimeCounter++;
        }
    }

    if (this->minislimeCounter > 0) {
        yScaleFactor = sqrtf(this->minislimeCounter * (2.0f / 30.0f)) * 0.6f + 0.4f;
        xzScale = 0.15f * yScaleFactor;
        this->actor.scale.x = xzScale;
        this->actor.scale.y = 0.079f * yScaleFactor;
        this->actor.scale.z = xzScale;
    }

    if (this->minislimeCounter == MINISLIME_NUM_SPAWN) {
        this->minislimeState = MINISLIME_INACTIVE_STATE;
        this->actor.hintId = 3;
        EnBigslime_SetupMoveOnCeiling(this);
    }
}

void EnBigslime_SetupCutsceneDefeat(EnBigslime* this, GlobalContext* globalCtx) {
    Vec3f subCamEye;
    Vec3f subCamAt;
    s32 i;
    s16 yawOffset;

    Animation_Change(&this->skelAnime, &gGekkoDamagedAnim, 0.5f, 0.0f,
                     Animation_GetLastFrame(&gGekkoDamagedAnim.common), 3, 0.0f);
    this->gekkoCollider.base.acFlags &= ~AC_ON;
    this->defeatTimer = 60;
    this->actor.speedXZ = 10.0f;
    this->actor.gravity = -2.0f;
    this->actor.velocity.y = 0.0f;
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_DEAD);

    // Points the camera at the defeated Gekko
    subCamAt.x = this->actor.world.pos.x;
    subCamAt.y = this->actor.world.pos.y + 40.0f;
    subCamAt.z = this->actor.world.pos.z;

    if (BINANG_SUB(Actor_YawToPoint(&this->actor, &this->actor.home.pos), this->actor.world.rot.y) > 0) {
        yawOffset = this->actor.world.rot.y + 0x4000;
    } else {
        yawOffset = this->actor.world.rot.y - 0x4000;
    }

    // Moves the camera to the side of player and Gekko
    subCamEye.x = (Math_SinS(yawOffset) * 250.0f) + subCamAt.x;
    subCamEye.y = subCamAt.y + 60.0f;
    subCamEye.z = (Math_CosS(yawOffset) * 250.0f) + subCamAt.z;
    Play_CameraSetAtEye(globalCtx, this->subCamId, &subCamAt, &subCamEye);

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        this->minislime[i]->actor.params = MINISLIME_DEFEAT_IDLE;
    }

    this->actor.flags &= ~1;
    EnBigslime_GekkoThaw(this, globalCtx);
    this->actionFunc = EnBigslime_CutsceneDefeat;
}

void EnBigslime_CutsceneDefeat(EnBigslime* this, GlobalContext* globalCtx) {
    s32 defeatTimer;
    Camera* subCam;
    Vec3f subCamAt;

    this->defeatTimer--;
    defeatTimer = CLAMP_MAX(this->defeatTimer, 10);
    this->gekkoRot.y += 0x300 * defeatTimer;
    if (Math_StepToF(&this->actor.speedXZ, 0.0f, 0.5f)) {
        EnBigslime_SetupGekkoDespawn(this, globalCtx);
    } else {
        // Continue for the camera to follow Gekko as it spins in defeat
        subCam = Play_GetCamera(globalCtx, this->subCamId);
        subCamAt.x = this->actor.world.pos.x;
        subCamAt.y = this->actor.world.pos.y + 40.0f;
        subCamAt.z = this->actor.world.pos.z;
        Play_CameraSetAtEye(globalCtx, this->subCamId, &subCamAt, &subCam->eye);
    }
}

void EnBigslime_SetupGekkoDespawn(EnBigslime* this, GlobalContext* globalCtx) {
    Camera* subCam = Play_GetCamera(globalCtx, this->subCamId);
    f32 magnitude;
    f32 invMagnitude;

    Math_Vec3f_Diff(&subCam->eye, &subCam->at, &this->subCamDistToFrog);
    magnitude = Math3D_Vec3fMagnitude(&this->subCamDistToFrog);
    if (magnitude > 78.0f) {
        invMagnitude = 1.0f / magnitude;
        magnitude -= 77.0f;
        magnitude /= 20.0f;
        Math_Vec3f_Scale(&this->subCamDistToFrog, magnitude * invMagnitude);
    } else {
        Math_Vec3f_Copy(&this->subCamDistToFrog, &gZeroVec3f);
    }

    this->shockwaveAlpha = 0;
    this->despawnTimer = 20;
    this->actionFunc = EnBigslime_GekkoDespawn;
}

void EnBigslime_GekkoDespawn(EnBigslime* this, GlobalContext* globalCtx) {
    Vec3f subCamEye;
    Vec3f subCamAt;
    Camera* subCam;

    this->despawnTimer--;
    this->gekkoScale = this->despawnTimer * 0.00035000002f;
    if (this->despawnTimer == 0) {
        EnBigslime_SetupFrogSpawn(this, globalCtx);
    } else {
        subCam = Play_GetCamera(globalCtx, this->subCamId);
        Math_Vec3f_Copy(&subCamAt, &subCam->at);
        Math_Vec3f_Diff(&subCam->eye, &this->subCamDistToFrog, &subCamEye);
        subCamEye.y -= 1.8f;
        subCamAt.y -= 1.7f;
        Play_CameraSetAtEye(globalCtx, this->subCamId, &subCamAt, &subCamEye);
    }
}

void EnBigslime_SetupFrogSpawn(EnBigslime* this, GlobalContext* globalCtx) {
    static Color_RGBA8 dustPrimColor = { 250, 250, 250, 255 };
    static Color_RGBA8 dustEnvColor = { 180, 180, 180, 255 };
    static Vec3f hahenAccel = { 0.0f, -0.5f, 0.0f };
    Camera* subCam = Play_GetCamera(globalCtx, this->subCamId);
    Vec3f* worldPos;
    Vec3f dustPos;
    Vec3f hahenVel;
    s16 yaw = func_800DFCDC(GET_ACTIVE_CAM(globalCtx));
    s16 yawReverse = yaw + 0x8000;
    s32 i;

    this->gekkoCollider.base.ocFlags1 &= ~OC1_ON;

    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_MINIFROG, this->actor.world.pos.x, this->actor.world.pos.y,
                this->actor.world.pos.z, 0, yawReverse, 0, this->actor.params);

    dustPos.x = (Math_SinS(yawReverse) * 20.0f) + this->actor.world.pos.x;
    dustPos.y = this->actor.world.pos.y + 20.0f;
    worldPos = &this->actor.world.pos;
    dustPos.z = (Math_CosS(yawReverse) * 20.0f) + this->actor.world.pos.z;

    Audio_PlaySoundAtPosition(globalCtx, worldPos, 40, NA_SE_EN_NPC_APPEAR);

    // dust cloud where the red frog appears
    func_800B0DE0(globalCtx, &dustPos, &gZeroVec3f, &gZeroVec3f, &dustPrimColor, &dustEnvColor, 500, 50);

    for (i = 0; i < 25; i++) {
        hahenVel.x = randPlusMinusPoint5Scaled(5.0f);
        hahenVel.y = Rand_ZeroFloat(3.0f) + 4.0f;
        hahenVel.z = randPlusMinusPoint5Scaled(5.0f);
        EffectSsHahen_Spawn(globalCtx, worldPos, &hahenVel, &hahenAccel, 0, Rand_S16Offset(12, 3), HAHEN_OBJECT_DEFAULT,
                            10, 0);
    }

    this->spawnFrogTimer = 40;
    Math_Vec3f_Diff(&subCam->eye, &subCam->at, &this->subCamDistToFrog);
    this->actionFunc = EnBigslime_FrogSpawn;
}

void EnBigslime_FrogSpawn(EnBigslime* this, GlobalContext* globalCtx) {
    Camera* subCam = Play_GetCamera(globalCtx, this->subCamId);
    Vec3f subCamEye;
    f32 subCamZoom;

    this->spawnFrogTimer--;

    // Zoom the camera in and out at the newly spawned red frog
    subCamZoom = sin_rad(this->spawnFrogTimer * (M_PI / 5)) * ((0.04f * (this->spawnFrogTimer * 0.1f)) + 0.02f) + 1.0f;
    subCamEye.x = subCam->at.x + (this->subCamDistToFrog.x * subCamZoom);
    subCamEye.z = subCam->at.z + (this->subCamDistToFrog.z * subCamZoom);
    subCamEye.y = subCam->at.y + (this->subCamDistToFrog.y * subCamZoom);
    Play_CameraSetAtEye(globalCtx, this->subCamId, &subCam->at, &subCamEye);

    if (this->spawnFrogTimer == 0) {
        EnBigslime_EndCutscene(this, globalCtx);
        EnBigslime_SetupDespawn(this);
    }
}

void EnBigslime_SetupDespawn(EnBigslime* this) {
    s32 i;

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        this->minislime[i]->actor.params = MINISLIME_DEFEAT_MELT;
    }

    this->isDespawned = false;
    this->gekkoCollider.base.ocFlags1 &= ~OC1_ON;
    this->actionFunc = EnBigslime_Despawn;
}

void EnBigslime_Despawn(EnBigslime* this, GlobalContext* globalCtx) {
    s32 i;
    s32 counter = 0;

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        if (this->minislime[i]->actor.shape.shadowAlpha == 0) {
            counter++;
        }
    }

    if (!this->isDespawned) {
        Actor_SetRoomClearedTemp(globalCtx, globalCtx->roomCtx.currRoom.num);
        this->isDespawned = true;
    }

    if (counter == MINISLIME_NUM_SPAWN) {
        for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
            this->minislime[i]->actor.params = MINISLIME_DESPAWN;
        }

        Actor_MarkForDeath(&this->actor);
    }
}

void EnBigslime_SetupInitEntrance(EnBigslime* this) {
    this->actionFunc = EnBigslime_InitEntrance;
}

void EnBigslime_InitEntrance(EnBigslime* this, GlobalContext* globalCtx) {
    if (globalCtx->roomCtx.prevRoom.num == -1) {
        EnBigslime_SetupCutscene(this);
    }
}

void EnBigslime_SetupCutscene(EnBigslime* this) {
    if (ActorCutscene_GetCurrentIndex() == 0x7D) {
        ActorCutscene_Stop(0x7D);
    }

    if (this->actor.colChkInfo.health == 0) {
        this->cutscene = this->actor.cutscene;
    }

    ActorCutscene_SetIntentToPlay(this->cutscene);
    this->actionFuncStored = this->actionFunc;
    this->actionFunc = EnBigslime_PlayCutscene;
    this->actor.speedXZ = 0.0f;
}

void EnBigslime_PlayCutscene(EnBigslime* this, GlobalContext* globalCtx) {
    if (ActorCutscene_GetCurrentIndex() == 0x7D) {
        ActorCutscene_Stop(0x7D);
        ActorCutscene_SetIntentToPlay(this->cutscene);
    } else if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
        ActorCutscene_Start(this->cutscene, &this->actor);
        if (this->actionFuncStored != EnBigslime_SquishFlat) {
            func_800B724C(globalCtx, &this->actor, 7);
        }

        this->subCamId = ActorCutscene_GetCurrentCamera(this->cutscene);
        if (this->actor.colChkInfo.health == 0) {
            EnBigslime_SetupCutsceneDefeat(this, globalCtx);
        } else if ((this->actionFuncStored == EnBigslime_DamageGekko) ||
                   (this->actionFuncStored == EnBigslime_JumpGekko) ||
                   (this->actionFuncStored == EnBigslime_StunGekko)) {
            EnBigslime_SetupCutsceneFormBigslime(this);
        } else if (this->actionFuncStored == EnBigslime_SquishFlat) {
            EnBigslime_SetupCutsceneGrabPlayer(this, globalCtx);
        } else {
            EnBigslime_SetupCutsceneStartBattle(this, globalCtx);
        }
    } else {
        ActorCutscene_SetIntentToPlay(this->cutscene);
    }
}

void EnBigslime_ApplyDamageEffectBigslime(EnBigslime* this, GlobalContext* globalCtx) {
    s32 i;

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        if (this->bigslimeCollider[i].base.acFlags & AC_HIT) {
            this->bigslimeCollider[i].base.acFlags &= ~AC_HIT;
            if (this->actionFunc == EnBigslime_FrozenGround) {
                if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_BREAK_ICE) {
                    EnBigslime_BreakIntoMinislime(this, globalCtx);
                    break;
                } else if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_FIRE) {
                    EnBigslime_SetPlayerParams(this, globalCtx);
                    EnBigslime_SetupMelt(this);
                    break;
                }
            } else {
                if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_ICE) {
                    EnMinislime* minislime;

                    globalCtx->envCtx.unk_C3 = 2;
                    EnBigslime_SetPlayerParams(this, globalCtx);
                    this->rotation = 0;
                    EnBigslime_SetupFreeze(this);
                    minislime = (EnMinislime*)SubS_FindActor(globalCtx, NULL, ACTORCAT_ITEMACTION, ACTOR_ARROW_ICE);
                    if (minislime != NULL) {
                        minislime->shakeRefPos.z = -100.0f;
                    }
                    break;
                }

                if ((this->actor.colChkInfo.damageEffect != BIGSLIME_DMGEFF_STUN) && (this->itemDropTimer == 0)) {
                    f32 randFloat = Rand_ZeroOne();

                    if (randFloat < 0.15f) {
                        Item_DropCollectible(globalCtx, &this->actor.world.pos, ITEM00_ARROWS_10);
                    } else if (randFloat < 0.3f) {
                        Item_DropCollectible(globalCtx, &this->actor.world.pos, ITEM00_MAGIC_SMALL);
                    }
                    this->itemDropTimer = 40;
                }
            }
        }
    }

    for (; i < BIGSLIME_NUM_RING_FACES; i++) {
        this->bigslimeCollider[i].base.acFlags &= ~AC_HIT;
    }
}

void EnBigslime_ApplyDamageEffectGekko(EnBigslime* this, GlobalContext* globalCtx) {
    if (this->gekkoCollider.base.acFlags & AC_HIT) {
        this->gekkoCollider.base.acFlags &= ~AC_HIT;
        if ((this->gekkoDrawEffect != GEKKO_DRAW_EFFECT_FROZEN) ||
            !(this->gekkoCollider.info.acHitInfo->toucher.dmgFlags & 0xDB0B3)) {
            EnBigslime_EndThrowMinislime(this);
            if (this->actor.colChkInfo.damageEffect != BIGSLIME_DMGEFF_HOOKSHOT) {
                if (Actor_ApplyDamage(&this->actor) == 0) {
                    func_800BE504(&this->actor, &this->gekkoCollider);
                    func_801A2ED8();
                    Enemy_StartFinishingBlow(globalCtx, &this->actor);
                    this->gekkoCollider.base.acFlags &= ~AC_ON;
                    EnBigslime_GekkoThaw(this, globalCtx);
                    if ((this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_FIRE) ||
                        (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_LIGHT)) {
                        this->unk_388 = 4.0f;
                        this->unk_38C = 0.75f;
                        if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_FIRE) {
                            this->gekkoDrawEffect = GEKKO_DRAW_EFFECT_THAW;
                        } else {
                            this->gekkoDrawEffect = GEKKO_DRAW_EFFECT_LIGHT_ORBS;
                            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_CLEAR_TAG,
                                        this->gekkoCollider.info.bumper.hitPos.x,
                                        this->gekkoCollider.info.bumper.hitPos.y,
                                        this->gekkoCollider.info.bumper.hitPos.z, 0, 0, 0, CLEAR_TAG_LARGE_LIGHT_RAYS);
                        }
                    } else if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_ICE) {
                        EnBigslime_GekkoFreeze(this);
                    }
                    EnBigslime_SetupCutscene(this);
                } else if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_ELECTRIC_STUN) {
                    this->stunTimer = 40;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    this->gekkoDrawEffect = GEKKO_DRAW_EFFECT_ELECTRIC_STUN;
                    this->unk_38C = 0.75f;
                    this->unk_388 = 2.0f;
                    EnBigslime_SetupStunGekko(this);
                } else if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_STUN ||
                           this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_DEKU_STUN) {
                    this->stunTimer = 40;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    EnBigslime_SetupStunGekko(this);
                } else if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_ICE) {
                    EnBigslime_GekkoFreeze(this);
                    func_800BE504(&this->actor, &this->gekkoCollider);
                    EnBigslime_SetupStunGekko(this);
                } else {
                    EnBigslime_GekkoThaw(this, globalCtx);
                    if ((this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_FIRE) ||
                        (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_LIGHT)) {
                        this->unk_388 = 3.0f;
                        this->unk_38C = 0.75f;
                        if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_FIRE) {
                            this->gekkoDrawEffect = GEKKO_DRAW_EFFECT_THAW;
                        } else {
                            this->gekkoDrawEffect = GEKKO_DRAW_EFFECT_LIGHT_ORBS;
                            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_CLEAR_TAG,
                                        this->gekkoCollider.info.bumper.hitPos.x,
                                        this->gekkoCollider.info.bumper.hitPos.y,
                                        this->gekkoCollider.info.bumper.hitPos.z, 0, 0, 0, CLEAR_TAG_LARGE_LIGHT_RAYS);
                        }
                    }
                    EnBigslime_SetupDamageGekko(this, true);
                }
            }
        }
    }
}

/**
 * Adds ice shard effects and calls EnBigslime_InitShockwave
 */
void EnBigslime_AddIceShardEffect(EnBigslime* this, GlobalContext* globalCtx) {
    Vtx* targetVtx = &sBigslimeTargetVtx[0];
    EnBigslimeIceShardEffect* iceShardEffect;
    s32 i;
    f32 randFloat;
    s16 randPitch; // Elevation Angle
    s16 vtxX;
    s16 vtxZ;
    f32 xzDist;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++, targetVtx++) {
        if (targetVtx->n.a > 0) {
            iceShardEffect = &this->iceShardEffect[i];
            iceShardEffect->pos.x = (targetVtx->n.ob[0] * this->actor.scale.x) + this->actor.world.pos.x;
            iceShardEffect->pos.y = (targetVtx->n.ob[1] * this->actor.scale.y) + this->actor.world.pos.y;
            iceShardEffect->pos.z = (targetVtx->n.ob[2] * this->actor.scale.z) + this->actor.world.pos.z;
            iceShardEffect->rotation.x = Rand_Next() >> 0x10;
            iceShardEffect->rotation.y = Rand_Next() >> 0x10;
            iceShardEffect->rotation.z = Rand_Next() >> 0x10;
            iceShardEffect->isActive = true;
            randPitch = Rand_S16Offset(0x1000, 0x3000);
            vtxZ = targetVtx->n.ob[2];
            vtxX = targetVtx->n.ob[0];
            xzDist = sqrtf(SQ(vtxZ) + SQ(vtxX));
            randFloat = Rand_ZeroFloat(5.0f) + 14.0f;
            if (xzDist > 1.0f) {
                xzDist = Math_CosS(randPitch) * randFloat / xzDist;
            } else {
                xzDist = Math_CosS(randPitch) * randFloat;
            }

            iceShardEffect->vel.x = targetVtx->n.ob[0] * xzDist;
            iceShardEffect->vel.y = Math_SinS(randPitch) * randFloat;
            iceShardEffect->vel.z = targetVtx->n.ob[2] * xzDist;
            iceShardEffect->scale = 0.001f * (Rand_ZeroFloat(6.0f) + 2.0f);
        }
    }

    Audio_PlayActorSound2(&this->actor, NA_SE_EV_ICE_BROKEN);
    EnBigslime_InitShockwave(this, globalCtx);
}

/**
 * Updates ice shard effects, shockwave effects, and actor damage draw effects
 */
void EnBigslime_UpdateEffects(EnBigslime* this) {
    EnBigslimeIceShardEffect* iceShardEffect;
    s32 i;

    // Update ice shards
    for (i = 0; i < BIGSLIME_NUM_ICE_SHARD; i++) {
        iceShardEffect = &this->iceShardEffect[i];
        if (iceShardEffect->isActive > false) {
            iceShardEffect->vel.y += -1.0f;
            Math_Vec3f_Sum(&iceShardEffect->pos, &iceShardEffect->vel, &iceShardEffect->pos);
            if (iceShardEffect->pos.y < (GBT_ROOM_5_MIN_Y - 20.0f)) {
                iceShardEffect->isActive = false;
            }
            iceShardEffect->rotation.x += (s16)(((u32)Rand_Next() >> 0x17) + 0x700);
            iceShardEffect->rotation.y += (s16)(((u32)Rand_Next() >> 0x17) + 0x900);
            iceShardEffect->rotation.z += (s16)(((u32)Rand_Next() >> 0x17) + 0xB00);
        }
    }

    // update shockwave
    if (this->shockwaveAlpha > 0) {
        this->shockwaveAlpha = (this->shockwaveAlpha - 10 < 0) ? 0 : (this->shockwaveAlpha - 10);
        this->shockwaveScale += 0.0013f;
    }

    // update actor damage draw effects
    if (this->unk_388 > 0.0f) {
        if ((this->gekkoDrawEffect != GEKKO_DRAW_EFFECT_FROZEN) && (this->actionFunc != EnBigslime_PlayCutscene)) {
            Math_StepToF(&this->unk_388, 0.0f, 0.05f);
            this->unk_38C = 0.375f * (this->unk_388 + 1.0f);
            this->unk_38C = CLAMP_MAX(this->unk_38C, 0.75f);
        } else if (!Math_StepToF(&this->unk_390, 0.75f, 0.01875f)) {
            func_800B9010(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

void EnBigslime_UpdateBigslime(Actor* thisx, GlobalContext* globalCtx) {
    EnBigslime* this = THIS;
    s32 i;
    Vec3f vtxMax;
    Vec3f vtxMin;

    if (globalCtx->envCtx.unk_C3 == 3) {
        globalCtx->envCtx.unk_C3 = 0xFF;
    }

    func_8019F540(1);
    this->dynamicVtxState ^= 1;
    EnBigslime_DynamicVtxCopyState(this);
    this->isAnimUpdate = SkelAnime_Update(&this->skelAnime);
    if (this->actionFunc != EnBigslime_PlayCutscene) {
        EnBigslime_ApplyDamageEffectBigslime(this, globalCtx);
    } else {
        for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
            this->bigslimeCollider[i].base.acFlags &= ~AC_HIT;
        }
    }

    this->actionFunc(this, globalCtx);

    if (this->actionFunc != EnBigslime_FormBigslime) {
        Actor_SetVelocityAndMoveYRotationAndGravity(&this->actor);
    } else {
        Actor_SetVelocityAndMoveXYRotation(&this->actor);
    }

    if (this->actionFunc != EnBigslime_JumpGekko) {
        EnBigslime_GetMaxMinVertices(this, &vtxMax, &vtxMin);
        EnBigslime_UpdateScale(this, &vtxMax, &vtxMin);
        EnBigslime_CheckRoomBoundaries(this, &vtxMax, &vtxMin);
        EnBigslime_UpdateSurfaceNorm(this);
        EnBigslime_UpdateBigslimeCollider(this, globalCtx);
    }

    EnBigslime_UpdateEffects(this);

    if (this->itemDropTimer > 0) {
        this->itemDropTimer--;
    }
}

void EnBigslime_UpdateGekko(Actor* thisx, GlobalContext* globalCtx) {
    static s32 isGekkoOnGround = false;
    EnBigslime* this = THIS;
    Player* player;
    s32 pad;

    if (globalCtx->envCtx.unk_C3 == 3) {
        globalCtx->envCtx.unk_C3 = 0xFF;
    }

    func_8019F540(0);
    this->isAnimUpdate = SkelAnime_Update(&this->skelAnime);
    if (this->actionFunc != EnBigslime_PlayCutscene) {
        EnBigslime_ApplyDamageEffectGekko(this, globalCtx);
    } else {
        this->gekkoCollider.base.acFlags &= ~AC_HIT;
    }

    this->actionFunc(this, globalCtx);
    if (this->actionFunc != EnBigslime_FormBigslime) {
        Actor_SetVelocityAndMoveYRotationAndGravity(&this->actor);
    } else {
        Actor_SetVelocityAndMoveXYRotation(&this->actor);
    }

    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 20.0f, 40.0f, 80.0f, 0x1F);
    this->gekkoCollider.dim.pos.x = (s16)this->actor.world.pos.x;
    this->gekkoCollider.dim.pos.z = (s16)this->actor.world.pos.z;
    if (this->gekkoCollider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->gekkoCollider.base);
    }

    if ((this->actor.update == EnBigslime_UpdateGekko) && (this->gekkoCollider.base.ocFlags1 & OC1_ON)) {
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->gekkoCollider.base);
    }

    EnBigslime_UpdateEffects(this);

    if ((this->actionFunc != EnBigslime_StunGekko) &&
        (this->gekkoCollider.dim.pos.y < (s16)(3.0f + GBT_ROOM_5_MIN_Y))) {
        Vec3f vtxNorm;

        if (((globalCtx->gameplayFrames % 4) == 0) || !isGekkoOnGround) {
            player = GET_PLAYER(globalCtx);
            vtxNorm.x = this->actor.world.pos.x;
            vtxNorm.z = this->actor.world.pos.z;
            vtxNorm.y = player->actor.world.pos.y + player->actor.depthInWater;
            EffectSsGRipple_Spawn(globalCtx, &vtxNorm, 150, 550, 0);
            isGekkoOnGround = true;
        }
    } else {
        isGekkoOnGround = false;
    }
}

/**
 * Related to transforming and drawing shadows
 */
void EnBigslime_SetSysMatrix(Vec3f* pos, GlobalContext* globalCtx, Gfx* shadowDList, f32 scaleX, f32 scalez, f32 scaleY,
                             s16 rotation, f32 alpha) {
    f32 yDistMinY;
    f32 xz;
    MtxF* sysMatrix = Matrix_GetCurrentState();

    yDistMinY = pos->y - scaleY - GBT_ROOM_5_MIN_Y;
    yDistMinY = CLAMP((yDistMinY), 0.0f, (GBT_ROOM_5_CENTER_Y - GBT_ROOM_5_MIN_Y) / 2);
    xz = 1.0f - (yDistMinY * (1.0f / 1550.0f));

    OPEN_DISPS(globalCtx->state.gfxCtx);
    POLY_OPA_DISP = Gfx_CallSetupDL(POLY_OPA_DISP, 0x2C);
    sysMatrix->xx = xz;
    sysMatrix->yy = 1.0f;
    sysMatrix->zz = xz;
    sysMatrix->xz = sysMatrix->xy = 0.0f;
    sysMatrix->yz = sysMatrix->yx = 0.0f;
    sysMatrix->zy = sysMatrix->zx = 0.0f;
    sysMatrix->wx = pos->x;
    sysMatrix->wy = GBT_ROOM_5_MIN_Y;
    sysMatrix->wz = pos->z;
    sysMatrix->xw = sysMatrix->yw = sysMatrix->zw = 0.0f;
    sysMatrix->ww = 1.0f;

    Matrix_RotateY(rotation, MTXMODE_APPLY);
    Matrix_Scale(scaleX, 1.0f, scalez, MTXMODE_APPLY);
    if (shadowDList != gBigslimeShadowDL) {
        gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED, 0, 0, 0,
                          COMBINED);
    }

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, (u8)(alpha * xz));
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, shadowDList);
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void EnBigslime_DrawMinislime(EnBigslime* this, GlobalContext* globalCtx) {
    EnMinislime* minislime;
    GlobalContext* globalCtx2 = globalCtx;
    s32 pad;
    s32 currIndex;
    s32 i;
    Lights* lights;
    s32 indices[MINISLIME_NUM_SPAWN];

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        indices[i] = i;
    }

    // Insertion sorting algorithm based on z projected Pos
    for (i = 0; i < MINISLIME_NUM_SPAWN - 1; i++) {
        if (this->minislime[indices[i]]->actor.projectedPos.z < this->minislime[indices[i + 1]]->actor.projectedPos.z) {
            currIndex = indices[i];
            indices[i] = indices[i + 1];
            indices[i + 1] = currIndex;
            if (i != 0) {
                i -= 2;
            }
        }
    }

    OPEN_DISPS(globalCtx->state.gfxCtx);
    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        minislime = this->minislime[indices[i]];
        lights = LightContext_NewLights(&globalCtx2->lightCtx, globalCtx->state.gfxCtx);
        Lights_BindAll(lights, globalCtx2->lightCtx.listHead, &minislime->actor.world.pos, globalCtx);
        Lights_Draw(lights, globalCtx->state.gfxCtx);
        func_8012C2DC(globalCtx->state.gfxCtx);
        func_800B8118(&minislime->actor, globalCtx, 0);
        Matrix_SetStateRotationAndTranslation(minislime->actor.world.pos.x, minislime->actor.world.pos.y,
                                              minislime->actor.world.pos.z, &minislime->actor.shape.rot);
        Matrix_Scale(minislime->actor.scale.x, minislime->actor.scale.y, minislime->actor.scale.z, MTXMODE_APPLY);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, minislime->actor.shape.shadowAlpha);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, &gMinislimeNormalDL);
        if (minislime->frozenAlpha > 0) {
            Matrix_InsertTranslation(0.0f, (0.1f - minislime->frozenScale) * -4000.0f, 0.0f, 1);
            Matrix_Scale(0.1f, minislime->frozenScale, 0.1f, MTXMODE_APPLY);
            AnimatedMat_Draw(globalCtx, this->minislimeFrozenTexAnim);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, minislime->frozenAlpha);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, &gMinislimeFrozenDL);
        }

        EnBigslime_SetSysMatrix(&minislime->actor.world.pos, globalCtx, gBigslimeShadowDL,
                                (minislime->actor.scale.x * 0.4f) * 0.1f, (minislime->actor.scale.z * 0.4f) * 0.1f,
                                minislime->actor.scale.y * 400.0f, minislime->actor.shape.rot.y,
                                minislime->actor.shape.shadowAlpha * (175.0f / 255.0f));
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void EnBigslime_DrawBigslime(Actor* thisx, GlobalContext* globalCtx) {
    // 28 equidistance-spaced vtx Points (uniformally over the sphere)
    static EnBigslimeBubbles bubblesInfo[] = {
        { 0, 0.3f },    { 6, 0.1f },    { 12, 0.45f }, { 18, 0.5f },   { 24, 0.6f },   { 30, 0.2f },   { 36, 0.4f },
        { 42, 0.25f },  { 48, 0.35f },  { 54, 0.65f }, { 60, 0.25f },  { 66, 0.55f },  { 72, 0.3f },   { 78, 0.1f },
        { 84, 0.45f },  { 90, 0.5f },   { 96, 0.6f },  { 102, 0.2f },  { 108, 0.4f },  { 114, 0.15f }, { 120, 0.35f },
        { 126, 0.65f }, { 132, 0.25f }, { 138, 0.3f }, { 144, 0.15f }, { 150, 0.45f }, { 156, 0.3f },  { 161, 0.25f },
    };
    EnBigslime* this = THIS;
    EnBigslimeBubbles* bubblesInfoPtr;
    Vtx* dynamicVtx;
    MtxF* billboardMtxF;
    s32 i;

    func_8012C2DC(globalCtx->state.gfxCtx);
    func_800B8118(&this->actor, globalCtx, 0);
    OPEN_DISPS(globalCtx->state.gfxCtx);

    // Draw Bigslime
    gSPSegment(POLY_XLU_DISP++, 0x09, sBigslimeDynamicVtx[this->dynamicVtxState]);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, &gBigslimeNormalDL);
    gSPDisplayList(POLY_XLU_DISP++, &gBigslimeVtxDL);

    // Draw frozen Bigslime
    if ((this->actionFunc == EnBigslime_Freeze) || (this->actionFunc == EnBigslime_FrozenGround) ||
        (this->actionFunc == EnBigslime_FrozenFall) || (this->actionFunc == EnBigslime_Melt)) {
        AnimatedMat_Draw(globalCtx, this->bigslimeFrozenTexAnim);
        gSPSegment(POLY_XLU_DISP++, 0x09, sBigslimeTargetVtx);
        gSPDisplayList(POLY_XLU_DISP++, &gBigslimeFrozenVtxDL);
        gSPDisplayList(POLY_XLU_DISP++, &gBigslimeVtxDL);
    }

    // Draw bubbles inside Bigslime
    if (this->actor.scale.x > 0.0f) {
        Matrix_SetCurrentState(&globalCtx->billboardMtxF);
        Matrix_Scale(0.0050000003f, 0.0050000003f, 0.0050000003f, MTXMODE_APPLY);
        billboardMtxF = Matrix_GetCurrentState();

        for (i = 0; i < 28; i++) {
            bubblesInfoPtr = &bubblesInfo[i];
            dynamicVtx = &sBigslimeDynamicVtx[this->dynamicVtxState][bubblesInfoPtr->v];
            billboardMtxF->wx =
                dynamicVtx->n.ob[0] * this->actor.scale.x * bubblesInfoPtr->scaleVtx + this->actor.world.pos.x;
            billboardMtxF->wy =
                dynamicVtx->n.ob[1] * this->actor.scale.y * bubblesInfoPtr->scaleVtx + this->actor.world.pos.y;
            billboardMtxF->wz =
                dynamicVtx->n.ob[2] * this->actor.scale.z * bubblesInfoPtr->scaleVtx + this->actor.world.pos.z;
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, &gBigslimeBubbleDL);
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx);

    EnBigslime_SetSysMatrix(&this->actor.world.pos, globalCtx, gBigslimeShadowDL, this->vtxScaleX, this->vtxScaleZ,
                            this->actor.scale.y * BIGSLIME_RADIUS_F, this->rotation, 175.0f);
    EnBigslime_DrawGekko(&this->actor, globalCtx);
}

void EnBigslime_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    /* value -1: Limb Not used
     * value 0:  GEKKO_LIMB_WAIST
     * value 1:  GEKKO_LIMB_L_SHIN
     * value 2:  GEKKO_LIMB_L_FOOT
     * value 3:  GEKKO_LIMB_R_SHIN
     * value 4:  GEKKO_LIMB_R_FOOT
     * value 5:  GEKKO_LIMB_L_UPPER_ARM
     * value 6:  GEKKO_LIMB_L_FOREARM
     * value 7:  GEKKO_LIMB_L_HAND
     * value 8:  GEKKO_LIMB_R_UPPER_ARM
     * value 9:  GEKKO_LIMB_R_FOREARM
     * value 10: GEKKO_LIMB_R_HAND
     * value 11: GEKKO_LIMB_JAW
     */
    static s8 limbPosIndex[] = {
        -1, -1, 0, -1, 1, -1, 2, -1, 3, -1, 4, -1, 5, 6, -1, 7, 8, 9, -1, 10, -1, 11, -1, -1,
    };
    // Some kind of offset for the position of the Gekkos right foot
    static Vec3f rightFootOffsetRef = { 1500.0f, 2200.0f, 0.0f };
    EnBigslime* this = THIS;
    Vec3f rightFootOffset;

    if (limbIndex == GEKKO_LIMB_HEAD) {
        Matrix_GetStateTranslation(&this->actor.focus.pos);
        this->actor.focus.rot.y = this->gekkoRot.y;
    }

    if (limbPosIndex[limbIndex] != -1) {
        Matrix_GetStateTranslation(&this->limbPos[limbPosIndex[limbIndex]]);
    }

    if (limbIndex == GEKKO_LIMB_R_ANKLE) {
        Matrix_MultiplyVector3fByState(&rightFootOffsetRef, &rightFootOffset);
        this->gekkoCollider.dim.pos.y = rightFootOffset.y;
    }
}

void EnBigslime_DrawGekko(Actor* thisx, GlobalContext* globalCtx) {
    static Color_RGBA8 gekkoDamageColor = { 255, 0, 0, 0 };
    static Color_RGBA8 gekkoStunColor = { 0, 0, 255, 0 };
    Vec3f gekkoPos;
    EnBigslime* this = THIS;
    s32 pad;

    func_8012C28C(globalCtx->state.gfxCtx);
    if (this->actionFunc == EnBigslime_DamageGekko) {
        func_800AE434(globalCtx, &gekkoDamageColor, this->damageSpinTimer, 20);
    } else if ((this->actionFunc == EnBigslime_CutsceneDefeat) || (this->actionFunc == EnBigslime_GekkoDespawn)) {
        func_800AE434(globalCtx, &gekkoDamageColor, 20, 20);
    } else if (this->actionFunc == EnBigslime_StunGekko) {
        if (this->gekkoDrawEffect == GEKKO_DRAW_EFFECT_FROZEN) {
            func_800AE434(globalCtx, &gekkoDamageColor, this->stunTimer, 80);
        } else if (this->gekkoDrawEffect == GEKKO_DRAW_EFFECT_ELECTRIC_STUN) {
            func_800AE434(globalCtx, &gekkoStunColor, this->stunTimer, 40);
        } else {
            func_800AE434(globalCtx, &gekkoStunColor, this->stunTimer, 40);
        }
    }

    OPEN_DISPS(globalCtx->state.gfxCtx);

    Math_Vec3f_Sum(&this->actor.world.pos, &this->gekkoPosOffset, &gekkoPos);
    Matrix_SetStateRotationAndTranslation(gekkoPos.x, gekkoPos.y, gekkoPos.z, &this->gekkoRot);
    Matrix_Scale(this->gekkoScale, this->gekkoScale, this->gekkoScale, MTXMODE_APPLY);
    SkinMatrix_Vec3fMtxFMultXYZ(&globalCtx->viewProjectionMtxF, &gekkoPos, &this->gekkoProjectedPos);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          NULL, EnBigslime_PostLimbDraw, &this->actor);

    CLOSE_DISPS(globalCtx->state.gfxCtx);

    if ((this->actionFunc == EnBigslime_DamageGekko) || (this->actionFunc == EnBigslime_CutsceneDefeat) ||
        (this->actionFunc == EnBigslime_GekkoDespawn) || (this->actionFunc == EnBigslime_StunGekko)) {
        func_800AE5A0(globalCtx);
    }

    EnBigslime_SetSysMatrix(&gekkoPos, globalCtx, D_04076BC0, this->gekkoScale * (550.0f / 7.0f),
                            this->gekkoScale * (550.0f / 7.0f), 0.0f, 0, 255.0f);

    if (this->minislimeState != MINISLIME_INACTIVE_STATE) {
        EnBigslime_DrawMinislime(this, globalCtx);
    }

    EnBigslime_DrawShatteringEffects(this, globalCtx);

    // Draw actor damage effects
    func_800BE680(globalCtx, &this->actor, this->limbPos, ARRAY_COUNT(this->limbPos),
                  this->gekkoScale * (999.99991f / 7.0f) * this->unk_38C, this->unk_390, this->unk_388,
                  this->gekkoDrawEffect);
}

void EnBigslime_DrawShatteringEffects(EnBigslime* this, GlobalContext* globalCtx) {
    EnBigslimeIceShardEffect* iceShardEffect;
    s32 i;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_8012C2DC(globalCtx->state.gfxCtx);

    // Draw Shockwave
    if (this->shockwaveAlpha > 0) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, this->shockwaveAlpha);
        gSPSegment(POLY_XLU_DISP++, 0x0D,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, globalCtx->gameplayFrames % 128,
                                    (u8)(globalCtx->gameplayFrames * 8), 32, 64, 1,
                                    (-globalCtx->gameplayFrames * 2) % 64, 0, 16, 16));
        Matrix_InsertTranslation(this->frozenPos.x, this->frozenPos.y, this->frozenPos.z, MTXMODE_NEW);
        Matrix_Scale(this->shockwaveScale, this->shockwaveScale, this->shockwaveScale, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, &gBigslimeShockwaveDL);
    }

    // Draw Ice Shards
    AnimatedMat_Draw(globalCtx, this->iceShardTexAnim);
    gSPDisplayList(POLY_XLU_DISP++, &gBigslimeIceShardDL);

    for (i = 0; i < BIGSLIME_NUM_ICE_SHARD; i++) {
        iceShardEffect = &this->iceShardEffect[i];
        if (iceShardEffect->isActive > false) {
            Matrix_SetStateRotationAndTranslation(iceShardEffect->pos.x, iceShardEffect->pos.y, iceShardEffect->pos.z,
                                                  &iceShardEffect->rotation);
            Matrix_Scale(iceShardEffect->scale, iceShardEffect->scale, iceShardEffect->scale, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, &gBigslimeIceShardVtxDL);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
