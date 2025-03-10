#ifndef _Z64_EFFECT_SS_DEAD_DB_H_
#define _Z64_EFFECT_SS_DEAD_DB_H_

#include "global.h"

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ s16 scale;
    /* 0x26 */ s16 scaleStep;
    /* 0x28 */ Color_RGBA8 primColor;
    /* 0x2C */ Color_RGBA8 envColor;
    /* 0x30 */ s32 unk_30;
} EffectSsDeadDbInitParams; // size = 0x30

extern const EffectSsInit Effect_Ss_Dead_Db_InitVars;

#endif
