#ifndef Z_EN_GEG_H
#define Z_EN_GEG_H

#include "global.h"

struct EnGeg;

typedef void (*EnGegActionFunc)(struct EnGeg*, GlobalContext*);

typedef struct EnGeg {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnGegActionFunc actionFunc;
    /* 0x18C */ ColliderCylinder colliderCylinder;
    /* 0x1D8 */ ColliderSphere colliderSphere;
    /* 0x230 */ u16 unk_230;
    /* 0x232 */ s16 unk_232;
    /* 0x234 */ UNK_TYPE1 unk234[4];
    /* 0x238 */ s16 unk_238;
    /* 0x23A */ UNK_TYPE1 unk23A[0x4];
    /* 0x23E */ s16 unk_23E;
    /* 0x240 */ s16 unk_240;
    /* 0x242 */ s16 unk_242;
    /* 0x244 */ s16 unk_244;
    /* 0x248 */ s32 unk_248;
    /* 0x24C */ Vec3s jointTable[18];
    /* 0x2B8 */ Vec3s morphTable[18];
    /* 0x324 */ UNK_TYPE1 unk324[0x144];
    /* 0x468 */ s16 unk_468;
    /* 0x46A */ s16 unk_46A;
    /* 0x46C */ s16 unk_46C;
    /* 0x46E */ s16 unk_46E;
    /* 0x470 */ Vec3f unk_470;
    /* 0x47C */ Vec3s unk_47C;
    /* 0x484 */ Vec3f unk_484;
    /* 0x490 */ Vec3s unk_490;
    /* 0x496 */ u16 unk_496;
    /* 0x498 */ s16 unk_498;
    /* 0x49A */ s16 unk_49A;
    /* 0x49C */ s16 unk_49C[8];
    /* 0x4AC */ s32 unk_4AC;
    /* 0x4B0 */ s16 unk_4B0;
    /* 0x4B4 */ Vec3f unk_4B4;
    /* 0x4C0 */ Vec3f unk_4C0[2];
    /* 0x4D8 */ s16 unk_4D8;
    /* 0x4DC */ s32 unk_4DC;
    /* 0x4E0 */ s16 unk_4E0;
    /* 0x4E4 */ Vec3f unk_4E4;
} EnGeg; // size = 0x4F0

extern const ActorInit En_Geg_InitVars;

#endif // Z_EN_GEG_H
