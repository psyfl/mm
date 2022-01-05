#ifndef Z_EN_GB2_H
#define Z_EN_GB2_H

#include "global.h"

struct EnGb2;

typedef void (*EnGb2ActionFunc)(struct EnGb2*, GlobalContext*);

#define ENGB2_GET_7(thisx) ((thisx)->params & 7)
#define ENGB2_GET_7F8(thisx) (((thisx)->params & 0x7F8) >> 3)

enum {
    /* 0 */ ENGB2_7_0,
    /* 1 */ ENGB2_7_1,
    /* 2 */ ENGB2_7_2,
};

typedef struct EnGb2 {
    /* 0x0000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[12];
    /* 0x1D0 */ Vec3s morphTable[12];
    /* 0x218 */ EnGb2ActionFunc actionFunc;
    /* 0x21C */ ColliderCylinder collider;
    /* 0x268 */ Actor* unk_268;
    /* 0x26C */ u16 unk_26C;
    /* 0x26E */ u16 unk_26E;
    /* 0x270 */ Vec3s unk_270;
    /* 0x276 */ Vec3s unk_276;
    /* 0x27C */ s16 unk_27C;
    /* 0x27E */ s16 unk_27E;
    /* 0x280 */ s16 unk_280;
    /* 0x282 */ s16 unk_282[3];
    /* 0x288 */ s16 unk_288;
    /* 0x28A */ u8 unk_28A;
    /* 0x28C */ s32 unk_28C;
    /* 0x290 */ s16 unk_290;
} EnGb2; // size = 0x294

extern const ActorInit En_Gb2_InitVars;

#endif // Z_EN_GB2_H
