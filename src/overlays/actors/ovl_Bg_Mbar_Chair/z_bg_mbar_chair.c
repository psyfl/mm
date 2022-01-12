/*
 * File: z_bg_mbar_chair.c
 * Overlay: ovl_Bg_Mbar_Chair
 * Description: Milk Bar - Chair
 */

#include "z_bg_mbar_chair.h"

#define FLAGS 0x00000000

#define THIS ((BgMbarChair*)thisx)

void BgMbarChair_Init(Actor* thisx, GlobalContext* globalCtx);
void BgMbarChair_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgMbarChair_Update(Actor* thisx, GlobalContext* globalCtx);
void BgMbarChair_Draw(Actor* thisx, GlobalContext* globalCtx);

const ActorInit Bg_Mbar_Chair_InitVars = {
    ACTOR_BG_MBAR_CHAIR,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MBAR_OBJ,
    sizeof(BgMbarChair),
    (ActorFunc)BgMbarChair_Init,
    (ActorFunc)BgMbarChair_Destroy,
    (ActorFunc)BgMbarChair_Update,
    (ActorFunc)BgMbarChair_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 60, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 80, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

extern CollisionHeader D_060019B4;
extern Gfx D_06000288[];

void BgMbarChair_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgMbarChair* this = THIS;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(globalCtx, &this->dyna, &D_060019B4);
}

void BgMbarChair_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgMbarChair* this = THIS;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

void BgMbarChair_Update(Actor* thisx, GlobalContext* globalCtx) {
}

void BgMbarChair_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, D_06000288);
}
