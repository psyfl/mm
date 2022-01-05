#ifndef Z_EN_FALL_H
#define Z_EN_FALL_H

#include "global.h"

#define EN_FALL_SCALE(thisx) ((thisx)->params & 0x7F)
#define EN_FALL_TYPE(thisx) (((thisx)->params & 0xF80) >> 7)

#define EN_FALL_FLAG_FIRE_BALL_INTENSIFIES (1 << 0)
#define EN_FALL_FLAG_FIRE_RING_APPEARS (1 << 1)

typedef enum {
    /*  0 */ EN_FALL_TYPE_TERMINA_FIELD_MOON,
    /*  1 */ EN_FALL_TYPE_TITLE_SCREEN_MOON,
    /*  2 */ EN_FALL_TYPE_CRASHING_MOON,
    /*  3 */ EN_FALL_TYPE_CRASH_FIRE_BALL,
    /*  4 */ EN_FALL_TYPE_CRASH_RISING_DEBRIS,
    /*  5 */ EN_FALL_TYPE_LODMOON_NO_LERP, // unused in final game
    /*  6 */ EN_FALL_TYPE_LODMOON,
    /*  7 */ EN_FALL_TYPE_STOPPED_MOON_CLOSED_MOUTH,
    /*  8 */ EN_FALL_TYPE_MOONS_TEAR,
    /*  9 */ EN_FALL_TYPE_CLOCK_TOWER_MOON,
    /* 10 */ EN_FALL_TYPE_STOPPED_MOON_OPEN_MOUTH,
    /* 11 */ EN_FALL_TYPE_CRASH_FIRE_RING,
    /* 12 */ EN_FALL_TYPE_LODMOON_INVERTED_STONE_TOWER,
} EnFallType;

struct EnFall;

typedef void (*EnFallActionFunc)(struct EnFall*, GlobalContext*);

typedef struct EnFall {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 dayStartTime;
    /* 0x146 */ s16 currentDay;
    /* 0x148 */ u8 objIndex;
    /* 0x14C */ f32 scale;
    /* 0x150 */ union {
                    f32 eyeGlowIntensity;
                    f32 fireBallIntensity;
                    f32 fireRingAlpha;
                };
    /* 0x154 */ u16 flags;
    /* 0x158 */ union {
                    s32 fireBallAlpha;
                    s32 activeDebrisParticleCount;
                };
    /* 0x15C */ union {
                    s16 fireBallYTexScroll1;
                    s16 fireWallYScale;
                };
    /* 0x15E */ s16 fireBallYTexScroll2;
    /* 0x160 */ EnFallActionFunc actionFunc;
} EnFall; // size = 0x164

extern const ActorInit En_Fall_InitVars;

#endif // Z_EN_FALL_H
