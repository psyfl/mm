#ifndef Z_EN_RECEPGIRL_H
#define Z_EN_RECEPGIRL_H

#include "global.h"

struct EnRecepgirl;

typedef void (*EnRecepgirlActionFunc)(struct EnRecepgirl*, GlobalContext*);

typedef struct EnRecepgirl {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[24];
    /* 0x218 */ Vec3s morphTable[24];
    /* 0x2A8 */ EnRecepgirlActionFunc actionFunc;
    /* 0x2AC */ u8 eyeTexIndex;
    /* 0x2AE */ Vec3s headRot;
} EnRecepgirl; // size = 0x2B4

extern const ActorInit En_Recepgirl_InitVars;

#endif // Z_EN_RECEPGIRL_H
