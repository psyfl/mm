/*
 * File: z_en_fsn.c
 * Overlay: ovl_En_Fsn
 * Description: Curiosity Shop Man
 */

#include "z_en_fsn.h"

#define FLAGS 0x00000019

#define THIS ((EnFsn*)thisx)

#define SI_NONE 0

#define ENFSN_END_CONVERSATION (1 << 0)
#define ENFSN_GIVE_ITEM (1 << 1)
#define ENFSN_GAVE_KEATONS_MASK (1 << 2)
#define ENFSN_GAVE_LETTER_TO_MAMA (1 << 3)

void EnFsn_Init(Actor* thisx, GlobalContext* globalCtx);
void EnFsn_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnFsn_Update(Actor* thisx, GlobalContext* globalCtx);
void EnFsn_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnFsn_Idle(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_ConverseBackroom(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_Haggle(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_BeginInteraction(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_GiveItem(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_AskBuyOrSell(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_StartBuying(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_SetupEndInteractionImmediately(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_SetupResumeInteraction(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_FaceShopkeeperSelling(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_DeterminePrice(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_SetupDeterminePrice(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_SetupEndInteraction(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_MakeOffer(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_ResumeInteraction(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_BrowseShelf(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_ResumeShoppingInteraction(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_AskCanBuyMore(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_AskCanBuyAterRunningOutOfItems(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_SelectItem(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_LookToShopkeeperFromShelf(EnFsn* this, GlobalContext* globalCtx);
void EnFsn_PlayerCannotBuy(EnFsn* this, GlobalContext* globalCtx);

extern AnimationHeader D_06012C34;
extern AnimationHeader D_060131FC;
extern AnimationHeader D_0600C58C;
extern AnimationHeader D_0600E3EC;
extern AnimationHeader D_0600F00C;
extern AnimationHeader D_0600CB3C;
extern AnimationHeader D_0600D354;
extern AnimationHeader D_060138B0;
extern AnimationHeader D_0601430C;
extern AnimationHeader D_0600B9D8;
extern AnimationHeader D_0600C26C;
extern AnimationHeader D_0600DE34;
extern FlexSkeletonHeader D_06013320;
extern UNK_TYPE D_06005BC0;
extern UNK_TYPE D_06006D40;
extern UNK_TYPE D_06007140;
extern Gfx D_0600F180[];
extern Gfx D_0600F218[];

const ActorInit En_Fsn_InitVars = {
    ACTOR_EN_FSN,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_FSN,
    sizeof(EnFsn),
    (ActorFunc)EnFsn_Init,
    (ActorFunc)EnFsn_Destroy,
    (ActorFunc)EnFsn_Update,
    (ActorFunc)EnFsn_Draw,
};

static ActorAnimationEntryS sAnimations[] = {
    { &D_06012C34, 1.0f, 0, -1, 0, 0 },  { &D_060131FC, 1.0f, 0, -1, 0, 0 }, { &D_0600C58C, 1.0f, 0, -1, 2, 0 },
    { &D_0600C58C, -1.0f, 0, -1, 2, 0 }, { &D_0600E3EC, 1.0f, 0, -1, 2, 0 }, { &D_0600F00C, 1.0f, 0, -1, 0, 0 },
    { &D_0600CB3C, 1.0f, 0, -1, 2, 0 },  { &D_0600D354, 1.0f, 0, -1, 0, 0 }, { &D_060138B0, 1.0f, 0, -1, 2, 0 },
    { &D_0601430C, 1.0f, 0, -1, 0, 0 },  { &D_0600B9D8, 1.0f, 0, -1, 2, 0 }, { &D_0600C26C, 1.0f, 0, -1, 0, 0 },
    { &D_0600DE34, 1.0f, 0, -1, 2, 0 },
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 18, 64, 0, { 0, 0, 0 } },
};

static Vec3f sShopItemPositions[3] = {
    { -5.0f, 35.0f, -95.0f },
    { 13.0f, 35.0f, -95.0f },
    { 31.0f, 35.0f, -95.0f },
};

s32 EnFsn_TestItemSelected(GlobalContext* globalCtx) {
    MessageContext* msgCtx = &globalCtx->msgCtx;

    if (msgCtx->unk12020 == 0x10 || msgCtx->unk12020 == 0x11) {
        return CHECK_BTN_ALL(CONTROLLER1(globalCtx)->press.button, BTN_A);
    }
    return CHECK_BTN_ALL(CONTROLLER1(globalCtx)->press.button, BTN_A) ||
           CHECK_BTN_ALL(CONTROLLER1(globalCtx)->press.button, BTN_B) ||
           CHECK_BTN_ALL(CONTROLLER1(globalCtx)->press.button, BTN_CUP);
}

u16 EnFsn_GetWelcome(GlobalContext* globalCtx) {
    switch (Player_GetMask(globalCtx)) {
        case PLAYER_MASK_NONE:
            return 0x29CC;
        case PLAYER_MASK_DEKU:
            return 0x29FC;
        case PLAYER_MASK_GORON:
        case PLAYER_MASK_ZORA:
            return 0x29FD;
        case PLAYER_MASK_KAFEIS_MASK:
            return 0x2364;
        default:
            return 0x29FE;
    }
}

void EnFsn_HandleConversationBackroom(EnFsn* this, GlobalContext* globalCtx) {
    switch (this->textId) {
        case 0:
            if (!(gSaveContext.weekEventReg[0x50] & 0x10)) {
                this->textId = 0x29E0;
                break;
            } else {
                this->textId = 0x29E4;
                this->flags |= ENFSN_END_CONVERSATION;
                break;
            }
        case 0x29E0:
            if (INV_CONTENT(ITEM_MASK_KEATON) == ITEM_MASK_KEATON) {
                this->flags |= ENFSN_GIVE_ITEM;
                this->flags |= ENFSN_GAVE_LETTER_TO_MAMA;
                this->getItemId = GI_LETTER_TO_MAMA;
                gSaveContext.weekEventReg[0x50] |= 0x10;
                this->textId = 0x29F1;
                break;
            } else {
                this->textId = 0x29E1;
                break;
            }
        case 0x29E1:
            this->textId = 0x29E2;
            break;
        case 0x29E2:
            this->flags |= ENFSN_GIVE_ITEM;
            this->flags |= ENFSN_GAVE_KEATONS_MASK;
            this->getItemId = GI_MASK_KEATON;
            this->textId = 0x29E3;
            break;
        case 0x29E3:
            this->flags |= ENFSN_GIVE_ITEM;
            this->flags |= ENFSN_GAVE_LETTER_TO_MAMA;
            this->getItemId = GI_LETTER_TO_MAMA;
            gSaveContext.weekEventReg[0x50] |= 0x10;
            this->textId = 0x29F1;
            break;
        case 0x29F1:
            this->textId = 0x29E4;
            this->flags |= ENFSN_END_CONVERSATION;
            break;
    }
    func_801518B0(globalCtx, this->textId, &this->actor);
    if (this->flags & ENFSN_END_CONVERSATION) {
        if (this->flags & ENFSN_GAVE_LETTER_TO_MAMA) {
            this->flags &= ~ENFSN_GAVE_LETTER_TO_MAMA;
            func_80151BB4(globalCtx, 34);
        }
        if (this->flags & ENFSN_GAVE_KEATONS_MASK) {
            this->flags &= ~ENFSN_GAVE_KEATONS_MASK;
            func_80151BB4(globalCtx, 33);
        }
        func_80151BB4(globalCtx, 3);
    }
}

void EnFsn_HandleSetupResumeInteraction(EnFsn* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == 6 && func_80147624(globalCtx) && this->cutsceneState == 0) {
        Actor_ProcessTalkRequest(&this->actor, &globalCtx->state);
        func_800B85E0(&this->actor, globalCtx, 400.0f, EXCH_ITEM_MINUS1);
        if (ENFSN_IS_SHOP(&this->actor)) {
            this->actor.textId = 0;
        }
        this->actionFunc = EnFsn_ResumeInteraction;
    }
}

void EnFsn_UpdateCollider(EnFsn* this, GlobalContext* globalCtx) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
}

void EnFsn_HandleLookToShopkeeperBuyingCutscene(EnFsn* this) {
    if (this->cutsceneState == 2 && this->lookToShopkeeperBuyingCutscene != this->cutscene &&
        this->actor.textId == 0x29CE) {
        ActorCutscene_Stop(this->cutscene);
        if (ActorCutscene_GetCurrentIndex() == 0x7C) {
            ActorCutscene_Stop(0x7C);
        }
        this->cutscene = this->lookToShopkeeperBuyingCutscene;
        ActorCutscene_SetIntentToPlay(this->cutscene);
        this->cutsceneState = 1;
    } else if (this->cutsceneState == 1) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_Start(this->cutscene, &this->actor);
            this->cutsceneState = 2;
        } else {
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
}

u8 EnFsn_SetCursorIndexFromNeutral(EnFsn* this) {
    if (this->itemIds[0] != -1) {
        return 0;
    }
    if (this->itemIds[1] != -1) {
        return 1;
    }
    if (this->itemIds[2] != -1) {
        return 2;
    }
    return CURSOR_INVALID;
}

void EnFsn_CursorLeftRight(EnFsn* this) {
    u8 cursorScan = this->cursorIdx;

    if (this->stickAccumX > 0) {
        if (cursorScan != this->totalSellingItems - 1) {
            while (cursorScan != this->totalSellingItems - 1) {
                cursorScan++;
                if (this->itemIds[cursorScan] != -1) {
                    this->cursorIdx = cursorScan;
                    break;
                }
            }
        } else if (this->itemIds[cursorScan] != -1) {
            this->cursorIdx = cursorScan;
        }
    } else if (this->stickAccumX < 0) {
        if (cursorScan != 0) {
            while (cursorScan != 0) {
                cursorScan--;
                if (this->itemIds[cursorScan] != -1) {
                    this->cursorIdx = cursorScan;
                    break;
                } else if (cursorScan == 0) {
                    play_sound(NA_SE_SY_CURSOR);
                    this->drawCursor = 0;
                    this->actionFunc = EnFsn_LookToShopkeeperFromShelf;
                    break;
                }
            }
        } else {
            play_sound(NA_SE_SY_CURSOR);
            this->drawCursor = 0;
            this->actionFunc = EnFsn_LookToShopkeeperFromShelf;
            if (this->itemIds[cursorScan] != -1) {
                this->cursorIdx = cursorScan;
            }
        }
    }
}

s16 EnFsn_GetThirdDayItemId(void) {
    if (!(gSaveContext.weekEventReg[0x21] & 4) && CURRENT_DAY == 3) {
        if (!(gSaveContext.weekEventReg[0x21] & 8) && !(gSaveContext.weekEventReg[0x4F] & 0x40)) {
            return SI_BOMB_BAG_30_1;
        }
        return SI_MASK_ALL_NIGHT;
    }
    return SI_NONE;
}

s16 EnFsn_GetStolenItemId(u32 stolenItem) {
    switch (stolenItem) {
        case ITEM_BOTTLE:
            return SI_BOTTLE;
        case ITEM_SWORD_GREAT_FAIRY:
            return SI_SWORD_GREAT_FAIRY;
        case ITEM_SWORD_KOKIRI:
            return SI_SWORD_KOKIRI;
        case ITEM_SWORD_RAZOR:
            return SI_SWORD_RAZOR;
        case ITEM_SWORD_GILDED:
            return SI_SWORD_GILDED;
    }
    return SI_NONE;
}

s32 EnFsn_HasItemsToSell(void) {
    if (CURRENT_DAY != 3) {
        if (((gSaveContext.roomInf[126][5] & 0xFF000000) >> 0x18) ||
            ((gSaveContext.roomInf[126][5] & 0xFF0000) >> 0x10)) {
            return true;
        }
        return false;
    } else {
        if (((gSaveContext.roomInf[126][5] & 0xFF000000) >> 0x18) ||
            ((gSaveContext.roomInf[126][5] & 0xFF0000) >> 0x10) || !(gSaveContext.weekEventReg[0x21] & 4)) {
            return true;
        }
        return false;
    }
}

void EnFsn_GetShopItemIds(EnFsn* this) {
    u32 stolenItem1 = (gSaveContext.roomInf[126][5] & 0xFF000000) >> 0x18;
    u32 stolenItem2 = (gSaveContext.roomInf[126][5] & 0xFF0000) >> 0x10;
    s16 itemId;

    this->stolenItem1 = this->stolenItem2 = 0;
    this->itemIds[0] = this->itemIds[1] = this->itemIds[2] = 0;

    itemId = EnFsn_GetThirdDayItemId();
    this->itemIds[this->totalSellingItems] = itemId;
    if (itemId != SI_NONE) {
        this->totalSellingItems++;
    }
    itemId = EnFsn_GetStolenItemId(stolenItem1);
    this->itemIds[this->totalSellingItems] = itemId;
    if (itemId != SI_NONE) {
        this->stolenItem1 = this->totalSellingItems;
        this->totalSellingItems++;
    }
    itemId = EnFsn_GetStolenItemId(stolenItem2);
    this->itemIds[this->totalSellingItems] = itemId;
    if (itemId != SI_NONE) {
        this->stolenItem2 = this->totalSellingItems;
        this->totalSellingItems++;
    }
    this->numSellingItems = this->totalSellingItems;
}

void EnFsn_SpawnShopItems(EnFsn* this, GlobalContext* globalCtx) {
    s32 i;

    EnFsn_GetShopItemIds(this);
    for (i = 0; i < this->totalSellingItems; i++) {
        if (this->itemIds[i] < 0) {
            this->items[i] = NULL;
        } else {
            this->items[i] =
                (EnGirlA*)Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_GIRLA, sShopItemPositions[i].x,
                                      sShopItemPositions[i].y, sShopItemPositions[i].z, 0, 0, 0, this->itemIds[i]);
        }
    }
}

void EnFsn_EndInteraction(EnFsn* this, GlobalContext* globalCtx) {
    if (this->cutsceneState == 2) {
        ActorCutscene_Stop(this->cutscene);
        this->cutsceneState = 0;
    }
    Actor_ProcessTalkRequest(&this->actor, &globalCtx->state);
    globalCtx->msgCtx.unk11F22 = 0x43;
    globalCtx->msgCtx.unk12023 = 4;
    Interface_ChangeAlpha(50);
    this->drawCursor = 0;
    this->stickLeftPrompt.isEnabled = false;
    this->stickRightPrompt.isEnabled = false;
    globalCtx->interfaceCtx.unk_222 = 0;
    globalCtx->interfaceCtx.unk_224 = 0;
    this->actor.textId = 0;
    this->actionFunc = EnFsn_Idle;
}

s32 EnFsn_TestEndInteraction(EnFsn* this, GlobalContext* globalCtx, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actor.textId = (CURRENT_DAY == 3) ? 0x29DF : 0x29D1;
        func_801518B0(globalCtx, this->actor.textId, &this->actor);
        func_80151BB4(globalCtx, 3);
        this->actionFunc = EnFsn_SetupEndInteraction;
        return true;
    }
    return false;
}

s32 EnFsn_TestCancelOption(EnFsn* this, GlobalContext* globalCtx, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->actionFunc = this->tmpActionFunc;
        func_80151938(globalCtx, this->items[this->cursorIdx]->actor.textId);
        return true;
    }
    return false;
}

void EnFsn_UpdateCursorPos(EnFsn* this, GlobalContext* globalCtx) {
    s16 sp2E;
    s16 sp2C;
    f32 xOffset = 0.0f;
    f32 yOffset = 17.0f;

    Actor_GetScreenPos(globalCtx, &this->items[this->cursorIdx]->actor, &sp2E, &sp2C);
    this->cursorPos.x = sp2E + xOffset;
    this->cursorPos.y = sp2C + yOffset;
    this->cursorPos.z = 1.2f;
}

s32 EnFsn_FacingShopkeeperDialogResult(EnFsn* this, GlobalContext* globalCtx) {
    switch (globalCtx->msgCtx.choiceIndex) {
        case 0:
            func_8019F208();
            if (CURRENT_DAY != 3) {
                this->actor.textId = 0x29FB;
            } else if (gSaveContext.weekEventReg[0x21] & 4) {
                this->actor.textId = 0x29FF;
            } else if (!(gSaveContext.weekEventReg[0x21] & 8) && !(gSaveContext.weekEventReg[0x4F] & 0x40)) {
                this->actor.textId = 0x29D7;
            } else {
                this->actor.textId = 0x29D8;
            }
            func_801518B0(globalCtx, this->actor.textId, &this->actor);
            return true;
        case 1:
            func_8019F230();
            this->actor.textId = (CURRENT_DAY == 3) ? 0x29DF : 0x29D1;
            func_801518B0(globalCtx, this->actor.textId, &this->actor);
            func_80151BB4(globalCtx, 3);
            this->actionFunc = EnFsn_SetupEndInteraction;
            return true;
    }
    return false;
}

s32 EnFsn_HasPlayerSelectedItem(EnFsn* this, GlobalContext* globalCtx, Input* input) {
    if (EnFsn_TestEndInteraction(this, globalCtx, input)) {
        return true;
    }
    if (EnFsn_TestItemSelected(globalCtx)) {
        if (!this->items[this->cursorIdx]->isOutOfStock) {
            this->tmpActionFunc = this->actionFunc;
            func_80151938(globalCtx, this->items[this->cursorIdx]->choiceTextId);
            play_sound(NA_SE_SY_DECIDE);
            this->stickLeftPrompt.isEnabled = false;
            this->stickRightPrompt.isEnabled = false;
            this->drawCursor = 0;
            this->actionFunc = EnFsn_SelectItem;
        } else {
            play_sound(NA_SE_SY_ERROR);
        }
        return true;
    }
    return false;
}

void EnFsn_UpdateJoystickInputState(EnFsn* this, GlobalContext* globalCtx) {
    s8 stickX = CONTROLLER1(globalCtx)->rel.stick_x;
    s8 stickY = CONTROLLER1(globalCtx)->rel.stick_y;

    if (this->stickAccumX == 0) {
        if (stickX > 30 || stickX < -30) {
            this->stickAccumX = stickX;
        }
    } else if (stickX <= 30 && stickX >= -30) {
        this->stickAccumX = 0;
    } else if ((this->stickAccumX * stickX) < 0) { // Stick has swapped directions
        this->stickAccumX = stickX;
    } else {
        this->stickAccumX += stickX;
        if (this->stickAccumX > 2000) {
            this->stickAccumX = 2000;
        } else if (this->stickAccumX < -2000) {
            this->stickAccumX = -2000;
        }
    }
    if (this->stickAccumY == 0) {
        if (stickY > 30 || stickY < -30) {
            this->stickAccumY = stickY;
        }
    } else if (stickY <= 30 && stickY >= -30) {
        this->stickAccumY = 0;
    } else if ((this->stickAccumY * stickY) < 0) { // Stick has swapped directions
        this->stickAccumY = stickY;
    } else {
        this->stickAccumY += stickY;
        if (this->stickAccumY > 2000) {
            this->stickAccumY = 2000;
        } else if (this->stickAccumY < -2000) {
            this->stickAccumY = -2000;
        }
    }
}

void EnFsn_PositionSelectedItem(EnFsn* this) {
    Vec3f selectedItemPosition = { 13.0f, 38.0f, -71.0f };
    u8 i = this->cursorIdx;
    EnGirlA* item = this->items[i];
    Vec3f worldPos;

    VEC3F_LERPIMPDST(&worldPos, &sShopItemPositions[i], &selectedItemPosition, this->shopItemSelectedTween);

    item->actor.world.pos.x = worldPos.x;
    item->actor.world.pos.y = worldPos.y;
    item->actor.world.pos.z = worldPos.z;
}

/*
 *    Returns true if animation has completed
 */
s32 EnFsn_TakeItemOffShelf(EnFsn* this) {
    Math_ApproachF(&this->shopItemSelectedTween, 1.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween >= 0.85f) {
        this->shopItemSelectedTween = 1.0f;
    }
    EnFsn_PositionSelectedItem(this);
    if (this->shopItemSelectedTween == 1.0f) {
        return true;
    }
    return false;
}

/*
 *    Returns true if animation has completed
 */
s32 EnFsn_ReturnItemToShelf(EnFsn* this) {
    Math_ApproachF(&this->shopItemSelectedTween, 0.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween <= 0.15f) {
        this->shopItemSelectedTween = 0.0f;
    }
    EnFsn_PositionSelectedItem(this);
    if (this->shopItemSelectedTween == 0.0f) {
        return true;
    }
    return false;
}

void EnFsn_UpdateItemSelectedProperty(EnFsn* this) {
    EnGirlA** items;
    s32 i;

    for (items = this->items, i = 0; i < this->totalSellingItems; items++, i++) {
        if (this->actionFunc != EnFsn_SelectItem && this->actionFunc != EnFsn_PlayerCannotBuy &&
            this->drawCursor == 0) {
            (*items)->isSelected = false;
        } else {
            (*items)->isSelected = (i == this->cursorIdx) ? true : false;
        }
    }
}

void EnFsn_UpdateCursorAnim(EnFsn* this) {
    f32 t = this->cursorAnimTween;

    if (this->cursorAnimState == 0) {
        t += 0.05f;
        if (t >= 1.0f) {
            t = 1.0f;
            this->cursorAnimState = 1;
        }
    } else {
        t -= 0.05f;
        if (t <= 0.0f) {
            t = 0.0f;
            this->cursorAnimState = 0;
        }
    }
    this->cursorColor.r = COL_CHAN_MIX(0, 0.0f, t);
    this->cursorColor.g = COL_CHAN_MIX(80, 80.0f, t);
    this->cursorColor.b = COL_CHAN_MIX(255, 0.0f, t);
    this->cursorColor.a = COL_CHAN_MIX(255, 0.0f, t);
    this->cursorAnimTween = t;
}

void EnFsn_UpdateStickDirectionPromptAnim(EnFsn* this) {
    f32 arrowAnimTween = this->arrowAnimTween;
    f32 stickAnimTween = this->stickAnimTween;
    s32 maxColor = 255;
    f32 tmp;

    if (this->arrowAnimState == 0) {
        arrowAnimTween += 0.05f;
        if (arrowAnimTween > 1.0f) {
            arrowAnimTween = 1.0f;
            this->arrowAnimState = 1;
        }
    } else {
        arrowAnimTween -= 0.05f;
        if (arrowAnimTween < 0.0f) {
            arrowAnimTween = 0.0f;
            this->arrowAnimState = 0;
        }
    }
    this->arrowAnimTween = arrowAnimTween;

    if (this->stickAnimState == 0) {
        stickAnimTween += 0.1f;
        if (stickAnimTween > 1.0f) {
            stickAnimTween = 1.0f;
            this->stickAnimState = 1;
        }
    } else {
        stickAnimTween = 0.0f;
        this->stickAnimState = 0;
    }

    tmp = 155.0f * arrowAnimTween;

    this->stickAnimTween = stickAnimTween;

    this->stickLeftPrompt.arrowColor.r = COL_CHAN_MIX(255, 155.0f, arrowAnimTween);
    this->stickLeftPrompt.arrowColor.g = COL_CHAN_MIX(maxColor, 155.0f, arrowAnimTween);
    this->stickLeftPrompt.arrowColor.b = COL_CHAN_MIX(0, -100, arrowAnimTween);
    this->stickLeftPrompt.arrowColor.a = COL_CHAN_MIX(200, 50.0f, arrowAnimTween);

    this->stickRightPrompt.arrowColor.r = (maxColor - ((s32)tmp)) & 0xFF;
    this->stickRightPrompt.arrowColor.g = (255 - ((s32)tmp)) & 0xFF;
    this->stickRightPrompt.arrowColor.b = COL_CHAN_MIX(0, -100.0f, arrowAnimTween);
    this->stickRightPrompt.arrowColor.a = COL_CHAN_MIX(200, 50.0f, arrowAnimTween);

    this->stickRightPrompt.arrowTexX = 290.0f;
    this->stickLeftPrompt.arrowTexX = 33.0f;

    this->stickRightPrompt.stickTexX = 274.0f;
    this->stickRightPrompt.stickTexX += 8.0f * stickAnimTween;
    this->stickLeftPrompt.stickTexX = 49.0f;
    this->stickLeftPrompt.stickTexX -= 8.0f * stickAnimTween;

    this->stickLeftPrompt.arrowTexY = this->stickRightPrompt.arrowTexY = 91.0f;
    this->stickLeftPrompt.stickTexY = this->stickRightPrompt.stickTexY = 95.0f;
}

void EnFsn_InitShop(EnFsn* this, GlobalContext* globalCtx) {
    EnFsn* this2;
    s32 maxColor = 255;

    if (EnFsn_HasItemsToSell()) {
        EnFsn_SpawnShopItems(this, globalCtx);

        this2 = this;
        this2->cursorPos.x = 100.0f;
        this2->cursorPos.y = 100.0f;
        this2->stickAccumY = 0;
        this2->stickAccumX = 0;

        this->cursorPos.z = 1.2f;
        this->cursorColor.r = 0;
        this->cursorColor.g = 80;
        this->cursorColor.b = maxColor;
        this->cursorColor.a = maxColor;
        this->cursorAnimState = 0;
        this->drawCursor = 0;
        this->cursorAnimTween = 0.0f;

        this->stickLeftPrompt.stickColor.r = 200;
        this->stickLeftPrompt.stickColor.g = 200;
        this->stickLeftPrompt.stickColor.b = 200;
        this->stickLeftPrompt.stickColor.a = 180;
        this->stickLeftPrompt.stickTexX = 49.0f;
        this->stickLeftPrompt.stickTexY = 95.0f;
        this->stickLeftPrompt.arrowColor.r = maxColor;
        this->stickLeftPrompt.arrowColor.g = maxColor;
        this->stickLeftPrompt.arrowColor.b = 0;
        this->stickLeftPrompt.arrowColor.a = 200;
        this->stickLeftPrompt.arrowTexX = 33.0f;
        this->stickLeftPrompt.arrowTexY = 91.0f;
        this->stickLeftPrompt.texZ = 1.0f;
        this->stickLeftPrompt.isEnabled = 0;

        this->stickRightPrompt.stickColor.r = 200;
        this->stickRightPrompt.stickColor.g = 200;
        this->stickRightPrompt.stickColor.b = 200;
        this->stickRightPrompt.stickColor.a = 180;
        this->stickRightPrompt.stickTexX = 274.0f;
        this->stickRightPrompt.stickTexY = 95.0f;
        this->stickRightPrompt.arrowColor.r = maxColor;
        this->stickRightPrompt.arrowColor.g = 0;
        this->stickRightPrompt.arrowColor.b = 0;
        this->stickRightPrompt.arrowColor.a = 200;
        this->stickRightPrompt.arrowTexX = 290.0f;
        this->stickRightPrompt.arrowTexY = 91.0f;
        this->stickRightPrompt.texZ = 1.0f;
        this->stickRightPrompt.isEnabled = 0;

        this->arrowAnimState = 0;
        this->stickAnimState = 0;
        this->stickAnimTween = this->arrowAnimTween = 0.0f;
    }
    this->blinkTimer = 20;
    this->animationIdx = 4;
    this->eyeTextureIdx = 0;
    func_8013BC6C(&this->skelAnime, sAnimations, this->animationIdx);
    this->actionFunc = EnFsn_Idle;
}

void EnFsn_Idle(EnFsn* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (this->animationIdx == 4) {
        s16 curFrame = this->skelAnime.curFrame;
        s16 frameCount = Animation_GetLastFrame(sAnimations[this->animationIdx].animationSeg);
        if (curFrame == frameCount) {
            this->animationIdx = 5;
            func_8013BC6C(&this->skelAnime, sAnimations, this->animationIdx);
        }
    } else if (this->flags & ENFSN_HAGGLE) {
    dummy:;
        this->actionFunc = EnFsn_Haggle;
    } else {
    dummy2:;
        if (Actor_ProcessTalkRequest(&this->actor, &globalCtx->state)) {
            if (this->cutsceneState == 0) {
                if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                    ActorCutscene_Stop(0x7C);
                }
                this->cutscene = this->lookToShopkeeperCutscene;
                ActorCutscene_SetIntentToPlay(this->cutscene);
                this->cutsceneState = 1;
            }
            this->actor.textId = EnFsn_GetWelcome(globalCtx);
            func_801518B0(globalCtx, this->actor.textId, &this->actor);
            player->actor.world.pos.x = 1.0f;
            player->actor.world.pos.z = -34.0f;
            this->actionFunc = EnFsn_BeginInteraction;
        } else if (((player->actor.world.pos.x >= -50.0f) && (player->actor.world.pos.x <= 15.0f)) &&
                   (player->actor.world.pos.y > 0.0f) &&
                   ((player->actor.world.pos.z >= -35.0f) && (player->actor.world.pos.z <= -20.0f))) {
            func_800B8614(&this->actor, globalCtx, 400.0f);
        }
    }
}

void EnFsn_Haggle(EnFsn* this, GlobalContext* globalCtx) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 frameCount = Animation_GetLastFrame(sAnimations[this->animationIdx].animationSeg);

    if (this->flags & ENFSN_ANGRY) {
        this->flags &= ~ENFSN_ANGRY;
        this->animationIdx = 11;
        func_8013BC6C(&this->skelAnime, sAnimations, this->animationIdx);
    } else {
        if (this->animationIdx == 11 && Animation_OnFrame(&this->skelAnime, 18.0f)) {
            Actor_PlaySfxAtPos(&this->actor, NA_SE_EV_HANKO);
        }
        if (this->flags & ENFSN_CALM_DOWN) {
            this->flags &= ~ENFSN_CALM_DOWN;
            this->animationIdx = 5;
            func_8013BC6C(&this->skelAnime, sAnimations, this->animationIdx);
        } else if (this->flags & ENFSN_OFFER_FINAL_PRICE) {
            this->flags &= ~ENFSN_OFFER_FINAL_PRICE;
            this->animationIdx = 12;
            func_8013BC6C(&this->skelAnime, sAnimations, this->animationIdx);
        } else {
            if (this->animationIdx == 12) {
                if (curFrame == frameCount) {
                    this->animationIdx = 5;
                    func_8013BC6C(&this->skelAnime, sAnimations, this->animationIdx);
                } else {
                    if (Animation_OnFrame(&this->skelAnime, 28.0f)) {
                        Actor_PlaySfxAtPos(&this->actor, NA_SE_EV_HANKO);
                    }
                    return;
                }
            }
            if (!(this->flags & ENFSN_HAGGLE)) {
                this->actionFunc = EnFsn_Idle;
            }
        }
    }
}

void EnFsn_BeginInteraction(EnFsn* this, GlobalContext* globalCtx) {
    if (this->cutsceneState == 1) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_StartAndSetFlag(this->cutscene, &this->actor);
            this->cutsceneState = 2;
            if (Player_GetMask(globalCtx) == PLAYER_MASK_NONE) {
                func_8011552C(globalCtx, 16);
                if (EnFsn_HasItemsToSell()) {
                    this->actionFunc = EnFsn_AskBuyOrSell;
                } else {
                    this->isSelling = false;
                    this->actionFunc = EnFsn_StartBuying;
                }
            } else {
                this->actionFunc = EnFsn_SetupEndInteractionImmediately;
            }
        } else {
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
}

void EnFsn_StartBuying(EnFsn* this, GlobalContext* globalCtx) {
    u8 talkState = Message_GetState(&globalCtx->msgCtx);
    Player* player = GET_PLAYER(globalCtx);

    EnFsn_HandleLookToShopkeeperBuyingCutscene(this);
    if (talkState == 5 && func_80147624(globalCtx)) {
        switch (this->actor.textId) {
            case 0x29CC:
                this->actor.textId = 0x29CD;
                func_801518B0(globalCtx, this->actor.textId, &this->actor);
                break;
            case 0x29CD:
                this->actor.textId = 0x29CE;
                EnFsn_HandleLookToShopkeeperBuyingCutscene(this);
                func_801518B0(globalCtx, this->actor.textId, &this->actor);
                break;
            case 0x29CE:
                this->actor.textId = 0xFF;
                func_801518B0(globalCtx, this->actor.textId, &this->actor);
                this->actionFunc = EnFsn_DeterminePrice;
                break;
            case 0x29CF:
                player->exchangeItemId = EXCH_ITEM_NONE;
                this->actionFunc = EnFsn_SetupDeterminePrice;
                break;
        }
    }
}

void EnFsn_AskBuyOrSell(EnFsn* this, GlobalContext* globalCtx) {
    u8 talkState = Message_GetState(&globalCtx->msgCtx);

    if (talkState == 5) {
        if (func_80147624(globalCtx)) {
            switch (this->actor.textId) {
                case 0x29CC:
                    if (CURRENT_DAY != 3) {
                        this->actor.textId = 0x29FA;
                    } else {
                        this->actor.textId = 0x29D2;
                    }
                    func_801518B0(globalCtx, this->actor.textId, &this->actor);
                    break;
                case 0x29D2:
                    if (gSaveContext.weekEventReg[0x21] & 4) {
                        this->actor.textId = 0x2A01;
                    } else if (!(gSaveContext.weekEventReg[0x21] & 8) && !(gSaveContext.weekEventReg[0x4F] & 0x40)) {
                        this->actor.textId = 0x29D3;
                    } else {
                        this->actor.textId = 0x29D4;
                    }
                    func_801518B0(globalCtx, this->actor.textId, &this->actor);
                    break;
                case 0x29D3:
                case 0x29D4:
                case 0x29FA:
                case 0x2A01:
                    this->actor.textId = 0x29D5;
                    func_801518B0(globalCtx, this->actor.textId, &this->actor);
                    break;
            }
        }
    } else if (talkState == 4) {
        func_8011552C(globalCtx, 6);
        if (!EnFsn_TestEndInteraction(this, globalCtx, CONTROLLER1(globalCtx)) && func_80147624(globalCtx)) {
            u32 trueTmp = true;
            switch (globalCtx->msgCtx.choiceIndex) {
                case 0:
                    func_8019F208();
                    this->isSelling = trueTmp;
                    this->stickLeftPrompt.isEnabled = false;
                    this->stickRightPrompt.isEnabled = trueTmp;
                    this->actor.textId = 0x29D6;
                    func_801518B0(globalCtx, this->actor.textId, &this->actor);
                    this->actionFunc = EnFsn_FaceShopkeeperSelling;
                    break;
                case 1:
                    func_8019F208();
                    this->isSelling = false;
                    this->actor.textId = 0x29CE;
                    EnFsn_HandleLookToShopkeeperBuyingCutscene(this);
                    func_801518B0(globalCtx, this->actor.textId, &this->actor);
                    this->actionFunc = EnFsn_StartBuying;
                    break;
            }
        }
    }
}

void EnFsn_SetupDeterminePrice(EnFsn* this, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&this->actor, &globalCtx->state)) {
        this->actor.textId = 0xFF;
        func_801518B0(globalCtx, this->actor.textId, &this->actor);
        this->actionFunc = EnFsn_DeterminePrice;
    }
}

void EnFsn_DeterminePrice(EnFsn* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s32 itemActionParam;
    u8 buttonItem;

    if (Message_GetState(&globalCtx->msgCtx) == 16) {
        itemActionParam = func_80123810(globalCtx);
        if (itemActionParam > PLAYER_AP_NONE) {
            if (player->heldItemButton == 0) {
                buttonItem = CUR_FORM_EQUIP(player->heldItemButton);
            } else {
                buttonItem = gSaveContext.equips.buttonItems[0][player->heldItemButton];
            }
            this->price = (buttonItem < ITEM_MOON_TEAR) ? gItemPrices[buttonItem] : 0;
            if (this->price > 0) {
                player->actor.textId = 0x29EF;
                player->exchangeItemId = buttonItem;
                this->actionFunc = EnFsn_MakeOffer;
            } else {
                player->actor.textId = 0x29CF;
                this->actionFunc = EnFsn_StartBuying;
            }
            this->actor.textId = player->actor.textId;
            func_801477B4(globalCtx);
        } else if (itemActionParam < PLAYER_AP_NONE) {
            if (CURRENT_DAY == 3) {
                this->actor.textId = 0x29DF;
            } else {
                this->actor.textId = 0x29D1;
            }
            func_801518B0(globalCtx, this->actor.textId, &this->actor);
            func_80151BB4(globalCtx, 3);
            this->actionFunc = EnFsn_SetupEndInteraction;
        }
    }
}

void EnFsn_MakeOffer(EnFsn* this, GlobalContext* globalCtx) {
    u8 talkState = Message_GetState(&globalCtx->msgCtx);
    Player* player = GET_PLAYER(globalCtx);

    if (talkState == 4 && func_80147624(globalCtx)) {
        switch (globalCtx->msgCtx.choiceIndex) {
            case 0:
                func_8019F208();
                globalCtx->msgCtx.unk11F22 = 0x43;
                globalCtx->msgCtx.unk12023 = 4;
                if (this->cutsceneState == 2) {
                    ActorCutscene_Stop(this->cutscene);
                    this->cutsceneState = 0;
                }
                switch (this->price) {
                    case 5:
                        this->getItemId = GI_RUPEE_BLUE;
                        break;
                    case 10:
                        this->getItemId = GI_RUPEE_10;
                        break;
                    case 20:
                        this->getItemId = GI_RUPEE_RED;
                        break;
                    case 50:
                        this->getItemId = GI_RUPEE_PURPLE;
                        break;
                    case 200:
                        this->getItemId = GI_RUPEE_HUGE;
                        break;
                }
                this->actionFunc = EnFsn_GiveItem;
                break;
            case 1:
                func_8019F230();
                player->exchangeItemId = EXCH_ITEM_NONE;
                this->actionFunc = EnFsn_SetupDeterminePrice;
                break;
        }
    }
}

void EnFsn_GiveItem(EnFsn* this, GlobalContext* globalCtx) {
    if (Actor_HasParent(&this->actor, globalCtx)) {
        if (this->isSelling == true && this->items[this->cursorIdx]->getItemId == GI_MASK_ALL_NIGHT) {
            func_80151BB4(globalCtx, 45);
            func_80151BB4(globalCtx, 3);
        }
        this->actor.parent = NULL;
        if (ENFSN_IS_SHOP(&this->actor) && !this->isSelling) {
            func_80123D50(globalCtx, GET_PLAYER(globalCtx), ITEM_BOTTLE, PLAYER_AP_BOTTLE);
        }
        this->actionFunc = EnFsn_SetupResumeInteraction;
    } else if (this->isSelling == true) {
        Actor_PickUp(&this->actor, globalCtx, this->items[this->cursorIdx]->getItemId, 300.0f, 300.0f);
    } else {
        Actor_PickUp(&this->actor, globalCtx, this->getItemId, 300.0f, 300.0f);
    }
}

void EnFsn_SetupResumeInteraction(EnFsn* this, GlobalContext* globalCtx) {
    if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
        if (globalCtx->msgCtx.unk120B1 == 0) {
            EnFsn_HandleSetupResumeInteraction(this, globalCtx);
        }
    } else {
        EnFsn_HandleSetupResumeInteraction(this, globalCtx);
    }
}

void EnFsn_ResumeInteraction(EnFsn* this, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&this->actor, &globalCtx->state)) {
        if (ENFSN_IS_SHOP(&this->actor)) {
            if (!this->isSelling) {
                this->cutscene = this->lookToShopkeeperBuyingCutscene;
                this->actor.textId = 0x29D0;
            } else {
                this->cutscene = this->lookToShopkeeperCutscene;
                this->actor.textId = (this->numSellingItems <= 0) ? 0x29DE : 0x29D6;
            }
            func_801518B0(globalCtx, this->actor.textId, &this->actor);
            if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                ActorCutscene_Stop(0x7C);
            }
            ActorCutscene_SetIntentToPlay(this->cutscene);
            this->actionFunc = EnFsn_ResumeShoppingInteraction;
        } else {
            EnFsn_HandleConversationBackroom(this, globalCtx);
            this->actionFunc = EnFsn_ConverseBackroom;
        }
    } else {
        func_800B85E0(&this->actor, globalCtx, 400.0f, EXCH_ITEM_MINUS1);
    }
}

void EnFsn_ResumeShoppingInteraction(EnFsn* this, GlobalContext* globalCtx) {
    if (this->cutsceneState == 0) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_StartAndSetFlag(this->cutscene, &this->actor);
            this->cutsceneState = 2;
            if (!this->isSelling) {
                this->actionFunc = EnFsn_AskCanBuyMore;
            } else if (this->actor.textId != 0x29D6) {
                this->actionFunc = EnFsn_AskCanBuyAterRunningOutOfItems;
            } else {
                func_8011552C(globalCtx, 6);
                this->stickLeftPrompt.isEnabled = false;
                this->stickRightPrompt.isEnabled = true;
                this->actionFunc = EnFsn_FaceShopkeeperSelling;
            }
        } else {
            if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                ActorCutscene_Stop(0x7C);
            }
            this->cutscene = !this->isSelling ? this->lookToShopkeeperBuyingCutscene : this->lookToShopkeeperCutscene;
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
}

void EnFsn_LookToShelf(EnFsn* this, GlobalContext* globalCtx) {
    if (this->cutsceneState == 2) {
        ActorCutscene_Stop(this->cutscene);
        if (ActorCutscene_GetCurrentIndex() == 0x7C) {
            ActorCutscene_Stop(0x7C);
        }
        this->cutscene = this->lookToShelfCutscene;
        ActorCutscene_SetIntentToPlay(this->cutscene);
        this->cutsceneState = 1;
    } else if (this->cutsceneState == 1) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene) != 0) {
            ActorCutscene_StartAndSetFlag(this->cutscene, &this->actor);
            this->cutsceneState = 2;
            EnFsn_UpdateCursorPos(this, globalCtx);
            this->actionFunc = EnFsn_BrowseShelf;
            func_80151938(globalCtx, this->items[this->cursorIdx]->actor.textId);
        } else {
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
}

void EnFsn_BrowseShelf(EnFsn* this, GlobalContext* globalCtx) {
    u8 talkstate = Message_GetState(&globalCtx->msgCtx);
    s32 pad;
    u8 prevCursorIdx = this->cursorIdx;

    if (!EnFsn_ReturnItemToShelf(this)) {
        this->delayTimer = 3;
    } else if (this->delayTimer != 0) {
        this->delayTimer--;
    } else {
        this->drawCursor = 0xFF;
        this->stickLeftPrompt.isEnabled = true;
        EnFsn_UpdateCursorPos(this, globalCtx);
        if (talkstate == 5) {
            func_8011552C(globalCtx, 6);
            if (!EnFsn_HasPlayerSelectedItem(this, globalCtx, CONTROLLER1(globalCtx))) {
                EnFsn_CursorLeftRight(this);
                if (this->cursorIdx != prevCursorIdx) {
                    play_sound(NA_SE_SY_CURSOR);
                    func_80151938(globalCtx, this->items[this->cursorIdx]->actor.textId);
                }
            }
        }
    }
}

void EnFsn_LookToShopkeeperFromShelf(EnFsn* this, GlobalContext* globalCtx) {
    if (this->cutsceneState == 2) {
        ActorCutscene_Stop(this->cutscene);
        if (ActorCutscene_GetCurrentIndex() == 0x7C) {
            ActorCutscene_Stop(0x7C);
        }
        this->cutscene = this->lookToShopkeeperFromShelfCutscene;
        ActorCutscene_SetIntentToPlay(this->cutscene);
        this->cutsceneState = 1;
    } else if (this->cutsceneState == 1) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_StartAndSetFlag(this->cutscene, &this->actor);
            this->cutsceneState = 2;
            this->stickLeftPrompt.isEnabled = false;
            this->stickRightPrompt.isEnabled = true;
            this->actor.textId = 0x29D6;
            func_80151938(globalCtx, this->actor.textId);
            this->actionFunc = EnFsn_FaceShopkeeperSelling;
        } else {
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
}

void EnFsn_HandleCanPlayerBuyItem(EnFsn* this, GlobalContext* globalCtx) {
    EnGirlA* item = this->items[this->cursorIdx];

    switch (item->canBuyFunc(globalCtx, item)) {
        case CANBUY_RESULT_SUCCESS_2:
            func_8019F208();
            gSaveContext.weekEventReg[0x21] |= 4;
        case CANBUY_RESULT_SUCCESS_1:
            if (this->cutsceneState == 2) {
                ActorCutscene_Stop(this->cutscene);
                this->cutsceneState = 0;
            }
            func_8019F208();
            item = this->items[this->cursorIdx];
            item->buyFanfareFunc(globalCtx, item);
            Actor_PickUp(&this->actor, globalCtx, this->items[this->cursorIdx]->getItemId, 300.0f, 300.0f);
            globalCtx->msgCtx.unk11F22 = 0x43;
            globalCtx->msgCtx.unk12023 = 4;
            Interface_ChangeAlpha(50);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item = this->items[this->cursorIdx];
            item->boughtFunc(globalCtx, item);
            if (this->stolenItem1 == this->cursorIdx) {
                gSaveContext.roomInf[126][5] &= ~0xFF000000;
            } else if (this->stolenItem2 == this->cursorIdx) {
                gSaveContext.roomInf[126][5] &= ~0xFF0000;
            }
            this->numSellingItems--;
            this->itemIds[this->cursorIdx] = -1;
            this->actionFunc = EnFsn_GiveItem;
            break;
        case CANBUY_RESULT_NEED_RUPEES:
            play_sound(NA_SE_SY_ERROR);
            func_80151938(globalCtx, 0x29F0);
            this->actionFunc = EnFsn_PlayerCannotBuy;
            break;
        case CANBUY_RESULT_CANNOT_GET_NOW:
            play_sound(NA_SE_SY_ERROR);
            func_80151938(globalCtx, 0x29DD);
            this->actionFunc = EnFsn_PlayerCannotBuy;
            break;
    }
}

void EnFsn_SetupEndInteraction(EnFsn* this, GlobalContext* globalCtx) {
    u8 talkState = Message_GetState(&globalCtx->msgCtx);

    if ((talkState == 5 || talkState == 6) && func_80147624(globalCtx)) {
        if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
            if (globalCtx->msgCtx.unk120B1 == 0) {
                EnFsn_EndInteraction(this, globalCtx);
            } else {
                globalCtx->msgCtx.unk11F22 = 0x43;
                globalCtx->msgCtx.unk12023 = 4;
            }
        } else {
            EnFsn_EndInteraction(this, globalCtx);
        }
    }
}

void EnFsn_SelectItem(EnFsn* this, GlobalContext* globalCtx) {
    u8 talkState = Message_GetState(&globalCtx->msgCtx);

    if (EnFsn_TakeItemOffShelf(this) && talkState == 4) {
        func_8011552C(globalCtx, 6);
        if (!EnFsn_TestCancelOption(this, globalCtx, CONTROLLER1(globalCtx)) && func_80147624(globalCtx)) {
            switch (globalCtx->msgCtx.choiceIndex) {
                case 0:
                    EnFsn_HandleCanPlayerBuyItem(this, globalCtx);
                    break;
                case 1:
                    func_8019F230();
                    this->actionFunc = this->tmpActionFunc;
                    func_80151938(globalCtx, this->items[this->cursorIdx]->actor.textId);
            }
        }
    }
}

void EnFsn_PlayerCannotBuy(EnFsn* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == 5 && func_80147624(globalCtx)) {
        this->actionFunc = this->tmpActionFunc;
        func_80151938(globalCtx, this->items[this->cursorIdx]->actor.textId);
    }
}

void EnFsn_AskCanBuyMore(EnFsn* this, GlobalContext* globalCtx) {
    u8 talkState = Message_GetState(&globalCtx->msgCtx);

    if (this->cutsceneState == 0) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_StartAndSetFlag(this->cutscene, &this->actor);
            this->cutsceneState = 2;
        } else {
            if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                ActorCutscene_Stop(0x7C);
            }
            this->cutscene = this->lookToShopkeeperCutscene;
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
    if (talkState == 4) {
        if (func_80147624(globalCtx)) {
            switch (globalCtx->msgCtx.choiceIndex) {
                case 0:
                    func_8019F208();
                    this->actor.textId = 0xFF;
                    func_801518B0(globalCtx, this->actor.textId, &this->actor);
                    this->actionFunc = EnFsn_DeterminePrice;
                    break;
                case 1:
                    func_8019F230();
                    this->actor.textId = (CURRENT_DAY == 3) ? 0x29DF : 0x29D1;
                    func_801518B0(globalCtx, this->actor.textId, &this->actor);
                    func_80151BB4(globalCtx, 3);
                    break;
            }
        }
    } else if ((talkState == 5 || talkState == 6) && func_80147624(globalCtx)) {
        if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
            if (globalCtx->msgCtx.unk120B1 == 0) {
                EnFsn_EndInteraction(this, globalCtx);
            } else {
                globalCtx->msgCtx.unk11F22 = 0x43;
                globalCtx->msgCtx.unk12023 = 4;
            }
        } else {
            EnFsn_EndInteraction(this, globalCtx);
        }
    }
}

void EnFsn_AskCanBuyAterRunningOutOfItems(EnFsn* this, GlobalContext* globalCtx) {
    u8 talkState = Message_GetState(&globalCtx->msgCtx);

    if (this->cutsceneState == 0) {
        if (ActorCutscene_GetCanPlayNext(this->cutscene)) {
            ActorCutscene_StartAndSetFlag(this->cutscene, &this->actor);
            this->cutsceneState = 2;
        } else {
            if (ActorCutscene_GetCurrentIndex() == 0x7C) {
                ActorCutscene_Stop(0x7C);
            }
            this->cutscene = this->lookToShopkeeperCutscene;
            ActorCutscene_SetIntentToPlay(this->cutscene);
        }
    }
    if (talkState == 4) {
        if (func_80147624(globalCtx)) {
            switch (globalCtx->msgCtx.choiceIndex) {
                case 0:
                    func_8019F208();
                    this->isSelling = false;
                    this->actor.textId = 0x29CE;
                    func_801518B0(globalCtx, this->actor.textId, &this->actor);
                    this->actionFunc = EnFsn_StartBuying;
                    break;
                case 1:
                    func_8019F230();
                    this->actor.textId = (CURRENT_DAY == 3) ? 0x29DF : 0x29D1;
                    func_801518B0(globalCtx, this->actor.textId, &this->actor);
                    func_80151BB4(globalCtx, 3);
                    break;
            }
        }
    } else if ((talkState == 5 || talkState == 6) && func_80147624(globalCtx)) {
        if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
            if (globalCtx->msgCtx.unk120B1 == 0) {
                EnFsn_EndInteraction(this, globalCtx);
            } else {
                globalCtx->msgCtx.unk11F22 = 0x43;
                globalCtx->msgCtx.unk12023 = 4;
            }
        } else {
            EnFsn_EndInteraction(this, globalCtx);
        }
    }
}

void EnFsn_FaceShopkeeperSelling(EnFsn* this, GlobalContext* globalCtx) {
    u8 talkState = Message_GetState(&globalCtx->msgCtx);
    u8 cursorIdx;

    if (talkState == 4) {
        func_8011552C(globalCtx, 6);
        if (!EnFsn_TestEndInteraction(this, globalCtx, CONTROLLER1(globalCtx)) &&
            (!func_80147624(globalCtx) || !EnFsn_FacingShopkeeperDialogResult(this, globalCtx)) &&
            this->stickAccumX > 0) {
            cursorIdx = EnFsn_SetCursorIndexFromNeutral(this);
            if (cursorIdx != CURSOR_INVALID) {
                this->cursorIdx = cursorIdx;
                this->actionFunc = EnFsn_LookToShelf;
                func_8011552C(globalCtx, 6);
                this->stickRightPrompt.isEnabled = false;
                play_sound(NA_SE_SY_CURSOR);
            }
        }
    } else if (talkState == 5 && func_80147624(globalCtx)) {
        this->actor.textId = 0x29D6;
        func_801518B0(globalCtx, this->actor.textId, &this->actor);
        if (globalCtx) {}
    }
}

void EnFsn_SetupEndInteractionImmediately(EnFsn* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == 5 && func_80147624(globalCtx)) {
        EnFsn_EndInteraction(this, globalCtx);
    }
}

void EnFsn_IdleBackroom(EnFsn* this, GlobalContext* globalCtx) {
    if (Actor_ProcessTalkRequest(&this->actor, &globalCtx->state)) {
        this->textId = 0;
        EnFsn_HandleConversationBackroom(this, globalCtx);
        this->actionFunc = EnFsn_ConverseBackroom;
    } else if (this->actor.xzDistToPlayer < 100.0f || this->actor.isTargeted) {
        func_800B8614(&this->actor, globalCtx, 100.0f);
    }
}

void EnFsn_ConverseBackroom(EnFsn* this, GlobalContext* globalCtx) {
    if (Message_GetState(&globalCtx->msgCtx) == 5 && func_80147624(globalCtx)) {
        if (this->flags & ENFSN_END_CONVERSATION) {
            this->flags &= ~ENFSN_END_CONVERSATION;
            globalCtx->msgCtx.unk11F22 = 0x43;
            globalCtx->msgCtx.unk12023 = 4;
            this->actionFunc = EnFsn_IdleBackroom;
        } else if (this->flags & ENFSN_GIVE_ITEM) {
            this->flags &= ~ENFSN_GIVE_ITEM;
            globalCtx->msgCtx.unk11F22 = 0x43;
            globalCtx->msgCtx.unk12023 = 4;
            this->actionFunc = EnFsn_GiveItem;
        } else {
            EnFsn_HandleConversationBackroom(this, globalCtx);
        }
    }
}

void EnFsn_GetCutscenes(EnFsn* this) {
    this->lookToShopkeeperCutscene = this->actor.cutscene;
    this->lookToShelfCutscene = ActorCutscene_GetAdditionalCutscene(this->lookToShopkeeperCutscene);
    this->lookToShopkeeperFromShelfCutscene = ActorCutscene_GetAdditionalCutscene(this->lookToShelfCutscene);
    this->lookToShopkeeperBuyingCutscene = ActorCutscene_GetAdditionalCutscene(this->lookToShopkeeperFromShelfCutscene);
}

void EnFsn_Blink(EnFsn* this) {
    s16 decr = this->blinkTimer - 1;

    if (decr >= 3) {
        this->eyeTextureIdx = 0;
        this->blinkTimer = decr;
    } else if (decr == 0) {
        this->eyeTextureIdx = 2;
        this->blinkTimer = (s32)(Rand_ZeroOne() * 60.0f) + 20;
    } else {
        this->eyeTextureIdx = 1;
        this->blinkTimer = decr;
    }
}

void EnFsn_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnFsn* this = THIS;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &D_06013320, &D_06012C34, this->jointTable, this->morphTable, 19);
    if (ENFSN_IS_SHOP(&this->actor)) {
        this->actor.shape.rot.y = BINANG_ROT180(this->actor.shape.rot.y);
        this->actor.flags &= ~1;
        EnFsn_GetCutscenes(this);
        EnFsn_InitShop(this, globalCtx);
    } else {
        if ((gSaveContext.weekEventReg[0x21] & 8) || (gSaveContext.weekEventReg[0x4F] & 0x40)) {
            Actor_MarkForDeath(&this->actor);
            return;
        }
        Collider_InitCylinder(globalCtx, &this->collider);
        Collider_InitAndSetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);
        this->blinkTimer = 20;
        this->eyeTextureIdx = 0;
        this->actor.flags |= 1;
        this->actor.targetMode = 0;
        this->animationIdx = 0;
        func_8013BC6C(&this->skelAnime, sAnimations, this->animationIdx);
        this->actionFunc = EnFsn_IdleBackroom;
    }
}

void EnFsn_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnFsn* this = THIS;

    Collider_DestroyCylinder(globalCtx, &this->collider);
}

void EnFsn_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnFsn* this = THIS;

    this->actionFunc(this, globalCtx);
    Actor_MoveWithGravity(&this->actor);
    func_800E9250(globalCtx, &this->actor, &this->headRot, &this->unk27A, this->actor.focus.pos);
    func_8013D9C8(globalCtx, this->limbRotYTable, this->limbRotZTable, 19);
    EnFsn_Blink(this);
    if (ENFSN_IS_SHOP(&this->actor) && EnFsn_HasItemsToSell()) {
        EnFsn_UpdateJoystickInputState(this, globalCtx);
        EnFsn_UpdateItemSelectedProperty(this);
        EnFsn_UpdateStickDirectionPromptAnim(this);
        EnFsn_UpdateCursorAnim(this);
    }
    SkelAnime_Update(&this->skelAnime);
    if (ENFSN_IS_BACKROOM(&this->actor)) {
        EnFsn_UpdateCollider(this, globalCtx);
    }
}

void EnFsn_DrawCursor(EnFsn* this, GlobalContext* globalCtx, f32 x, f32 y, f32 z, u8 drawCursor) {
    s32 ulx, uly, lrx, lry;
    f32 w;
    s32 dsdx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    if (drawCursor != 0) {
        func_8012C654(globalCtx->state.gfxCtx);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, this->cursorColor.r, this->cursorColor.g, this->cursorColor.b,
                        this->cursorColor.a);
        gDPLoadTextureBlock_4b(OVERLAY_DISP++, &D_0401F740, G_IM_FMT_IA, 16, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                               G_TX_MIRROR | G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        w = 16.0f * z;
        ulx = (x - w) * 4.0f;
        uly = (y - w + -12.0f) * 4.0f;
        lrx = (x + w) * 4.0f;
        lry = (y + w + -12.0f) * 4.0f;
        dsdx = (1.0f / z) * 1024.0f;
        gSPTextureRectangle(OVERLAY_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, 0, 0, dsdx, dsdx);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void EnFsn_DrawTextRec(GlobalContext* globalCtx, s32 r, s32 g, s32 b, s32 a, f32 x, f32 y, f32 z, s32 s, s32 t, f32 dx,
                       f32 dy) {
    f32 unk;
    s32 ulx, uly, lrx, lry;
    f32 w, h;
    s32 dsdx, dtdy;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    gDPPipeSync(OVERLAY_DISP++);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, r, g, b, a);

    w = 8.0f * z;
    ulx = (x - w) * 4.0f;
    lrx = (x + w) * 4.0f;

    h = 12.0f * z;
    uly = (y - h) * 4.0f;
    lry = (y + h) * 4.0f;

    unk = 1024 * (1.0f / z);
    dsdx = unk * dx;
    dtdy = dy * unk;

    gSPTextureRectangle(OVERLAY_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, s, t, dsdx, dtdy);
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void EnFsn_DrawStickDirectionPrompts(EnFsn* this, GlobalContext* globalCtx) {
    s32 drawStickRightPrompt = this->stickLeftPrompt.isEnabled;
    s32 drawStickLeftPrompt = this->stickRightPrompt.isEnabled;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    if (drawStickRightPrompt || drawStickLeftPrompt) {
        func_8012C654(globalCtx->state.gfxCtx);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetTextureImage(OVERLAY_DISP++, G_IM_FMT_IA, G_IM_SIZ_16b, 1, &D_0401F8C0);
        gDPSetTile(OVERLAY_DISP++, G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_WRAP,
                   G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOLOD);
        gDPLoadSync(OVERLAY_DISP++);
        gDPLoadBlock(OVERLAY_DISP++, G_TX_LOADTILE, 0, 0, 191, 1024);
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetTile(OVERLAY_DISP++, G_IM_FMT_IA, G_IM_SIZ_8b, 2, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_WRAP,
                   G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOLOD);
        gDPSetTileSize(OVERLAY_DISP++, G_TX_RENDERTILE, 0, 0, 15 * 4, 23 * 4);
        if (drawStickRightPrompt) {
            EnFsn_DrawTextRec(globalCtx, this->stickLeftPrompt.arrowColor.r, this->stickLeftPrompt.arrowColor.g,
                              this->stickLeftPrompt.arrowColor.b, this->stickLeftPrompt.arrowColor.a,
                              this->stickLeftPrompt.arrowTexX, this->stickLeftPrompt.arrowTexY,
                              this->stickLeftPrompt.texZ, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickLeftPrompt) {
            EnFsn_DrawTextRec(globalCtx, this->stickRightPrompt.arrowColor.r, this->stickRightPrompt.arrowColor.g,
                              this->stickRightPrompt.arrowColor.b, this->stickRightPrompt.arrowColor.a,
                              this->stickRightPrompt.arrowTexX, this->stickRightPrompt.arrowTexY,
                              this->stickRightPrompt.texZ, 0, 0, 1.0f, 1.0f);
        }
        gDPSetTextureImage(OVERLAY_DISP++, G_IM_FMT_IA, G_IM_SIZ_16b, 1, &D_0401F7C0);
        gDPSetTile(OVERLAY_DISP++, G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_WRAP,
                   G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOLOD);
        gDPLoadSync(OVERLAY_DISP++);
        gDPLoadBlock(OVERLAY_DISP++, G_TX_LOADTILE, 0, 0, 127, 1024);
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetTile(OVERLAY_DISP++, G_IM_FMT_IA, G_IM_SIZ_8b, 2, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_WRAP,
                   G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOLOD);
        gDPSetTileSize(OVERLAY_DISP++, G_TX_RENDERTILE, 0, 0, 15 * 4, 15 * 4);
        if (drawStickRightPrompt) {
            EnFsn_DrawTextRec(globalCtx, this->stickLeftPrompt.stickColor.r, this->stickLeftPrompt.stickColor.g,
                              this->stickLeftPrompt.stickColor.b, this->stickLeftPrompt.stickColor.a,
                              this->stickLeftPrompt.stickTexX, this->stickLeftPrompt.stickTexY,
                              this->stickLeftPrompt.texZ, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickLeftPrompt) {
            EnFsn_DrawTextRec(globalCtx, this->stickRightPrompt.stickColor.r, this->stickRightPrompt.stickColor.g,
                              this->stickRightPrompt.stickColor.b, this->stickRightPrompt.stickColor.a,
                              this->stickRightPrompt.stickTexX, this->stickRightPrompt.stickTexY,
                              this->stickRightPrompt.texZ, 0, 0, 1.0f, 1.0f);
        }
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

s32 EnFsn_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnFsn* this = THIS;
    s16 tmp;
    s32 limbRotTableIdx;

    if (limbIndex == 16) {
        Matrix_InsertXRotation_s(this->headRot.y, MTXMODE_APPLY);
    }
    if (ENFSN_IS_BACKROOM(&this->actor)) {
        switch (limbIndex) {
            case 8:
                limbRotTableIdx = 0;
                break;
            case 11:
                limbRotTableIdx = 1;
                break;
            case 16:
                limbRotTableIdx = 2;
                break;
            default:
                limbRotTableIdx = 9;
                break;
        }
        if (limbRotTableIdx < 9) {
            tmp = (s32)(Math_SinS(this->limbRotYTable[limbRotTableIdx]) * 200.0f);
            rot->y += tmp;
            tmp = (s32)(Math_CosS(this->limbRotZTable[limbRotTableIdx]) * 200.0f);
            rot->z += tmp;
        }
    }
    if (limbIndex == 17) {
        *dList = NULL;
    }
    return 0;
}

void EnFsn_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnFsn* this = THIS;

    if (limbIndex == 16) {
        this->actor.focus.pos.x = this->actor.world.pos.x;
        this->actor.focus.pos.y = this->actor.world.pos.y + 60.0f;
        this->actor.focus.pos.z = this->actor.world.pos.z;

        OPEN_DISPS(globalCtx->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, D_0600F180);
        gSPDisplayList(POLY_OPA_DISP++, D_0600F218);
        CLOSE_DISPS(globalCtx->state.gfxCtx);
    }
}

void EnFsn_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static void* sEyeTextures[] = { &D_06005BC0, &D_06006D40, &D_06007140 };
    EnFsn* this = THIS;
    s32 pad;
    s16 i;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    func_8012C5B0(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTextureIdx]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTextureIdx]));
    SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnFsn_OverrideLimbDraw, EnFsn_PostLimbDraw, &this->actor);

    for (i = 0; i < this->totalSellingItems; i++) {
        this->items[i]->actor.scale.x = 0.2f;
        this->items[i]->actor.scale.y = 0.2f;
        this->items[i]->actor.scale.z = 0.2f;
    }

    EnFsn_DrawCursor(this, globalCtx, this->cursorPos.x, this->cursorPos.y, this->cursorPos.z, this->drawCursor);
    EnFsn_DrawStickDirectionPrompts(this, globalCtx);
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
