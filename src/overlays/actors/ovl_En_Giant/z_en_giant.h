#ifndef Z_EN_GIANT_H
#define Z_EN_GIANT_H

#include "global.h"

#define GIANT_TYPE(thisx) ((thisx)->params & 0xF)
#define GIANT_TYPE_IS_NOT_TERMINA_FIELD(type) (type > GIANT_TYPE_OCEAN_TERMINA_FIELD)
#define GIANT_TYPE_IS_TERMINA_FIELD(type) (type <= GIANT_TYPE_OCEAN_TERMINA_FIELD)
#define GIANT_TYPE_IS_CLOCK_TOWER_SUCCESS(type) (type >= GIANT_TYPE_MOUNTAIN_CLOCK_TOWER_SUCCESS && type <= GIANT_TYPE_OCEAN_CLOCK_TOWER_SUCCESS)
#define GIANT_TYPE_IS_CHAMBER_OR_ENDING(type) (type >= GIANT_TYPE_MOUNTAIN_GIANTS_CHAMBER_AND_ENDING && type <= GIANT_TYPE_OCEAN_GIANTS_CHAMBER_AND_ENDING)
#define GIANT_TYPE_IS_CLOCK_TOWER_FAILURE(type) (type >= GIANT_TYPE_MOUNTAIN_CLOCK_TOWER_FAILURE && type <= GIANT_TYPE_OCEAN_CLOCK_TOWER_FAILURE)

/**
 * The giants are divided into types based on where in the game they appear.
 * - TERMINA_FIELD: These appear in all non-ending cutscenes that take place in Termina Field.
 * - CLOCK_TOWER_SUCCESS: These appear in the Clock Tower when the Oath to Order is played and all giants are freed.
 * - GIANTS_CHAMBER_AND_ENDING: These giants appear in the Giant's Chamber or in all cutscenes that play after defeating Majora.
 * - CLOCK_TOWER_FAILURE: These appear in the Clock Tower when the Oath to Order is played and all giants are NOT freed.
 * 
 * The SWAMP, MOUNTAIN, OCEAN, and CANYON signifiers are used to identify which of the four giants it is,
 * but the devs were not always consistent about this. For example, the giants in the Giant's Chamber are
 * always GIANT_TYPE_MOUNTAIN_GIANTS_CHAMBER_AND_ENDING, regardless of which boss was just defeated. These
 * are named such that EnGiant_IsImprisoned has the expected behavior; the devs were only inconsistent in
 * the cases where they weren't calling this function.
 */
typedef enum {
    /*  0 */ GIANT_TYPE_MOUNTAIN_TERMINA_FIELD,
    /*  1 */ GIANT_TYPE_CANYON_TERMINA_FIELD,
    /*  2 */ GIANT_TYPE_SWAMP_TERMINA_FIELD,
    /*  3 */ GIANT_TYPE_OCEAN_TERMINA_FIELD,
    /*  4 */ GIANT_TYPE_MOUNTAIN_CLOCK_TOWER_SUCCESS,
    /*  5 */ GIANT_TYPE_CANYON_CLOCK_TOWER_SUCCESS,
    /*  6 */ GIANT_TYPE_SWAMP_CLOCK_TOWER_SUCCESS,
    /*  7 */ GIANT_TYPE_OCEAN_CLOCK_TOWER_SUCCESS,
    /*  8 */ GIANT_TYPE_MOUNTAIN_GIANTS_CHAMBER_AND_ENDING,
    /*  9 */ GIANT_TYPE_CANYON_GIANTS_CHAMBER_AND_ENDING,
    /* 10 */ GIANT_TYPE_SWAMP_GIANTS_CHAMBER_AND_ENDING,
    /* 11 */ GIANT_TYPE_OCEAN_GIANTS_CHAMBER_AND_ENDING,
    /* 12 */ GIANT_TYPE_MOUNTAIN_CLOCK_TOWER_FAILURE,
    /* 13 */ GIANT_TYPE_CANYON_CLOCK_TOWER_FAILURE,
    /* 14 */ GIANT_TYPE_SWAMP_CLOCK_TOWER_FAILURE,
    /* 15 */ GIANT_TYPE_OCEAN_CLOCK_TOWER_FAILURE,
} GiantType;

/**
 * These values are used to index into sAnimationTable to pick the appropriate animation.
 */
typedef enum {
    /*  0 */ GIANT_ANIMATION_LOOK_UP_START,
    /*  1 */ GIANT_ANIMATION_LOOK_UP_LOOP,
    /*  2 */ GIANT_ANIMATION_FALLING_OVER,
    /*  3 */ GIANT_ANIMATION_RAISED_ARMS_START,
    /*  4 */ GIANT_ANIMATION_RAISED_ARMS_LOOP,
    /*  5 */ GIANT_ANIMATION_STRUGGLE_START,
    /*  6 */ GIANT_ANIMATION_STRUGGLE_LOOP,
    /*  7 */ GIANT_ANIMATION_IDLE_LOOP,
    /*  8 */ GIANT_ANIMATION_WALKING_LOOP,
    /*  9 */ GIANT_ANIMATION_BIG_CALL_START,
    /* 10 */ GIANT_ANIMATION_BIG_CALL_LOOP,
    /* 11 */ GIANT_ANIMATION_BIG_CALL_END,
    /* 12 */ GIANT_ANIMATION_SMALL_CALL_START,
    /* 13 */ GIANT_ANIMATION_SMALL_CALL_LOOP,
    /* 14 */ GIANT_ANIMATION_SMALL_CALL_END,
    /* 15 */ GIANT_ANIMATION_MAX
} GiantAnimationIndex;

/**
 * Used as values for csAction. The UNKNOWN ones are never used in-game.
 */
typedef enum {
    /*  0 */ GIANT_CS_ACTION_NONE,
    /*  1 */ GIANT_CS_ACTION_IDLE,
    /*  2 */ GIANT_CS_ACTION_WALKING,
    /*  3 */ GIANT_CS_ACTION_LOOKING_UP,
    /*  4 */ GIANT_CS_ACTION_RAISING_ARMS,
    /*  5 */ GIANT_CS_ACTION_STRUGGLING,
    /*  6 */ GIANT_CS_ACTION_FALLING_OVER,
    /*  7 */ GIANT_CS_ACTION_IDLE_FADE_IN,
    /*  8 */ GIANT_CS_ACTION_TALKING,
    /*  9 */ GIANT_CS_ACTION_DONE_TALKING,
    /* 10 */ GIANT_CS_ACTION_TEACHING_OATH_TO_ORDER,
    /* 11 */ GIANT_CS_ACTION_PLAYER_LEARNED_OATH_TO_ORDER,
    /* 12 */ GIANT_CS_ACTION_UNKNOWN_12,
    /* 13 */ GIANT_CS_ACTION_UNKNOWN_13,
    /* 14 */ GIANT_CS_ACTION_UNKNOWN_14,
    /* 15 */ GIANT_CS_ACTION_HOLDING_UP_MOON_IN_CLOCK_TOWER
} GiantCsActionIndex;

typedef enum {
    /*  0 */ GIANT_LIMB_NONE,
    /*  1 */ GIANT_LIMB_HEAD,
    /*  2 */ GIANT_LIMB_LEFT_THIGH,
    /*  3 */ GIANT_LIMB_LEFT_LOWER_LEG,
    /*  4 */ GIANT_LIMB_LEFT_FOOT,
    /*  5 */ GIANT_LIMB_RIGHT_THIGH,
    /*  6 */ GIANT_LIMB_RIGHT_LOWER_LEG,
    /*  7 */ GIANT_LIMB_RIGHT_FOOT,
    /*  8 */ GIANT_LIMB_LEFT_SHOULDER,
    /*  9 */ GIANT_LIMB_LEFT_UPPER_ARM,
    /* 10 */ GIANT_LIMB_LEFT_FOREARM,
    /* 11 */ GIANT_LIMB_LEFT_HAND,
    /* 12 */ GIANT_LIMB_RIGHT_SHOULDER,
    /* 13 */ GIANT_LIMB_RIGHT_UPPER_ARM,
    /* 14 */ GIANT_LIMB_RIGHT_FOREARM,
    /* 15 */ GIANT_LIMB_RIGHT_HAND,
    /* 16 */ GIANT_LIMB_MAX,
} EnGiantLimbs;

struct EnGiant;

typedef void (*EnGiantActionFunc)(struct EnGiant*, GlobalContext*);

typedef struct EnGiant {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[GIANT_LIMB_MAX];
    /* 0x1E8 */ Vec3s morphTable[GIANT_LIMB_MAX];
    /* 0x248 */ s16 animationId;
    /* 0x24A */ u16 actorActionCommand;
    /* 0x24C */ u16 csAction;
    /* 0x24E */ s16 alpha;
    /* 0x250 */ u16 sfxId;
    /* 0x254 */ MtxF headDrawMtxF;
    /* 0x294 */ s16 faceIndex;
    /* 0x296 */ s16 blinkTimer;
    /* 0x298 */ EnGiantActionFunc actionFunc;
} EnGiant; // size = 0x29C

extern const ActorInit En_Giant_InitVars;

#endif // Z_EN_GIANT_H
