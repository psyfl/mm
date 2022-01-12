/*
 * File: z_en_recepgirl.c
 * Overlay: ovl_En_Recepgirl
 * Description: Mayor's receptionist
 */

#include "z_en_recepgirl.h"

#define FLAGS 0x00000009

#define THIS ((EnRecepgirl*)thisx)

void EnRecepgirl_Init(Actor* thisx, GlobalContext* globalCtx);
void EnRecepgirl_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnRecepgirl_Update(Actor* thisx, GlobalContext* globalCtx);
void EnRecepgirl_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnRecepgirl_SetupWait(EnRecepgirl* this);
void EnRecepgirl_Wait(EnRecepgirl* this, GlobalContext* globalCtx);
void EnRecepgirl_SetupTalk(EnRecepgirl* this);
void EnRecepgirl_Talk(EnRecepgirl* this, GlobalContext* globalCtx);

const ActorInit En_Recepgirl_InitVars = {
    ACTOR_EN_RECEPGIRL,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_BG,
    sizeof(EnRecepgirl),
    (ActorFunc)EnRecepgirl_Init,
    (ActorFunc)EnRecepgirl_Destroy,
    (ActorFunc)EnRecepgirl_Update,
    (ActorFunc)EnRecepgirl_Draw,
};

extern TexturePtr D_0600F8F0;
extern TexturePtr D_0600FCF0;
extern TexturePtr D_060100F0;

static TexturePtr sEyeTextures[] = { &D_0600F8F0, &D_0600FCF0, &D_060100F0, &D_0600FCF0 };

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 6, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 1000, ICHAIN_STOP),
};

static s32 texturesDesegmented = false;

extern AnimationHeader D_06000968;
extern AnimationHeader D_06001384;
extern AnimationHeader D_06009890;
extern AnimationHeader D_0600A280;
extern AnimationHeader D_0600AD98;
extern FlexSkeletonHeader D_06011B60;

void EnRecepgirl_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnRecepgirl* this = THIS;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, -60.0f, NULL, 0.0f);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &D_06011B60, &D_06009890, this->jointTable, this->morphTable, 24);

    if (!texturesDesegmented) {
        for (i = 0; i < ARRAY_COUNT(sEyeTextures); i++) {
            sEyeTextures[i] = Lib_SegmentedToVirtual(sEyeTextures[i]);
        }
        texturesDesegmented = true;
    }

    this->eyeTexIndex = 2;

    if (Flags_GetSwitch(globalCtx, this->actor.params)) {
        this->actor.textId = 0x2ADC; // hear directions again?
    } else {
        this->actor.textId = 0x2AD9; // "Welcome..."
    }

    EnRecepgirl_SetupWait(this);
}

void EnRecepgirl_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnRecepgirl_UpdateEyes(EnRecepgirl* this) {
    if (this->eyeTexIndex != 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex == 4) {
            this->eyeTexIndex = 0;
        }
    } else if (Rand_ZeroOne() < 0.02f) {
        this->eyeTexIndex++;
    }
}

void EnRecepgirl_SetupWait(EnRecepgirl* this) {
    if (this->skelAnime.animation == &D_06001384) {
        Animation_MorphToPlayOnce(&this->skelAnime, &D_0600AD98, 5.0f);
    }
    this->actionFunc = EnRecepgirl_Wait;
}

void EnRecepgirl_Wait(EnRecepgirl* this, GlobalContext* globalCtx) {
    if (SkelAnime_Update(&this->skelAnime) != 0) {
        if (this->skelAnime.animation == &D_0600A280) {
            Animation_MorphToPlayOnce(&this->skelAnime, &D_0600AD98, 5.0f);
        } else {
            Animation_MorphToLoop(&this->skelAnime, &D_06009890, -4.0f);
        }
    }

    if (Actor_ProcessTalkRequest(&this->actor, &globalCtx->state)) {
        EnRecepgirl_SetupTalk(this);
    } else if (Actor_IsFacingPlayer(&this->actor, 0x2000)) {
        func_800B8614(&this->actor, globalCtx, 60.0f);
        if (Player_GetMask(globalCtx) == PLAYER_MASK_KAFEIS_MASK) {
            this->actor.textId = 0x2367; // "... doesn't Kafei want to break off his engagement ... ?"
        } else if (Flags_GetSwitch(globalCtx, this->actor.params)) {
            this->actor.textId = 0x2ADC; // hear directions again?
        } else {
            this->actor.textId = 0x2AD9; // "Welcome..."
        }
    }
}

void EnRecepgirl_SetupTalk(EnRecepgirl* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &D_0600A280, -4.0f);
    this->actionFunc = EnRecepgirl_Talk;
}

void EnRecepgirl_Talk(EnRecepgirl* this, GlobalContext* globalCtx) {
    u8 temp_v0_2;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->skelAnime.animation == &D_0600A280) {
            Animation_PlayLoop(&this->skelAnime, &D_06001384);
        } else if (this->skelAnime.animation == &D_0600AD98) {
            if (this->actor.textId == 0x2ADA) { // Mayor's office is on the left (meeting ongoing)
                Animation_MorphToPlayOnce(&this->skelAnime, &D_06000968, 10.0f);
            } else {
                Animation_MorphToLoop(&this->skelAnime, &D_06009890, 10.0f);
            }
        } else {
            if (this->actor.textId == 0x2ADA) { // Mayor's office is on the left (meeting ongoing)
                Animation_MorphToLoop(&this->skelAnime, &D_06009890, 10.0f);
            } else {
                Animation_MorphToPlayOnce(&this->skelAnime, &D_0600A280, -4.0f);
            }
        }
    }

    temp_v0_2 = Message_GetState(&globalCtx->msgCtx);
    if (temp_v0_2 == 2) {
        this->actor.textId = 0x2ADC; // hear directions again?
        EnRecepgirl_SetupWait(this);
    } else if ((temp_v0_2 == 5) && (func_80147624(globalCtx) != 0)) {
        if (this->actor.textId == 0x2AD9) { // "Welcome..."
            Flags_SetSwitch(globalCtx, this->actor.params);
            Animation_MorphToPlayOnce(&this->skelAnime, &D_0600AD98, 10.0f);

            if (gSaveContext.weekEventReg[63] & 0x80) { // showed Couple's Mask to meeting
                this->actor.textId = 0x2ADF;            // Mayor's office is on the left (meeting ended)
            } else {
                this->actor.textId = 0x2ADA; // Mayor's office is on the left (meeting ongoing)
            }
        } else if (this->actor.textId == 0x2ADC) { // hear directions again?
            Animation_MorphToPlayOnce(&this->skelAnime, &D_0600AD98, 10.0f);
            this->actor.textId = 0x2ADD; // "So..."
        } else {
            Animation_MorphToPlayOnce(&this->skelAnime, &D_06000968, 10.0f);

            if (this->actor.textId == 0x2ADD) {        // "So..."
                this->actor.textId = 0x2ADE;           // Mayor's office is on the left, drawing room on the right
            } else if (this->actor.textId == 0x2ADA) { // Mayor's office is on the left (meeting ongoing)
                this->actor.textId = 0x2ADB;           // drawing room on the right
            } else {
                this->actor.textId = 0x2AE0; // drawing room on the right, don't go in without an appointment
            }
        }
        func_80151938(globalCtx, this->actor.textId);
    }
}

void EnRecepgirl_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnRecepgirl* this = THIS;
    Vec3s sp30;

    this->actionFunc(this, globalCtx);
    func_800E9250(globalCtx, &this->actor, &this->headRot, &sp30, this->actor.focus.pos);
    EnRecepgirl_UpdateEyes(this);
}

s32 EnRecepgirl_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                 Actor* thisx) {
    EnRecepgirl* this = THIS;

    if (limbIndex == 5) {
        rot->x += this->headRot.y;
    }
    return false;
}

void EnRecepgirl_UnkLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Actor* thisx) {
    EnRecepgirl* this = THIS;

    if (limbIndex == 5) {
        Matrix_RotateY(0x400 - this->headRot.x, MTXMODE_APPLY);
        Matrix_GetStateTranslationAndScaledX(500.0f, &this->actor.focus.pos);
    }
}

void EnRecepgirl_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnRecepgirl* this = THIS;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_8012C28C(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, sEyeTextures[this->eyeTexIndex]);

    func_801343C0(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                  EnRecepgirl_OverrideLimbDraw, NULL, EnRecepgirl_UnkLimbDraw, &this->actor);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
