/*
 * File: z_en_fr.c
 * Overlay: ovl_En_Fr
 * Description:
 */

#include "z_en_fr.h"

#define FLAGS 0x40000000

#define THIS ((EnFr*)thisx)

void EnFr_Init(Actor* thisx, GlobalContext* globalCtx);
void EnFr_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnFr_Update(Actor* thisx, GlobalContext* globalCtx);

const ActorInit En_Fr_InitVars = {
    ACTOR_EN_FR,
    ACTORCAT_ITEMACTION,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(EnFr),
    (ActorFunc)EnFr_Init,
    (ActorFunc)EnFr_Destroy,
    (ActorFunc)EnFr_Update,
    (ActorFunc)NULL,
};

void EnFr_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnFr* this = THIS;

    if (Flags_GetSwitch(globalCtx, ENFR_GET_SWITCHFLAG(&this->actor))) {
        Actor_MarkForDeath(&this->actor);
    } else {
        this->actor.targetMode = ENFR_GET_TARGETMODE(&this->actor);
    }
}

void EnFr_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnFr_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnFr* this = THIS;

    if (Flags_GetSwitch(globalCtx, ENFR_GET_SWITCHFLAG(&this->actor))) {
        Actor_MarkForDeath(&this->actor);
    } else if (this->actor.xyzDistToPlayerSq < SQ(IREG(29))) {
        this->actor.flags &= ~0x40000000;
    } else {
        this->actor.flags |= 0x40000000;
    }
}
