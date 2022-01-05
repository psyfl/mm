#ifndef Z_EN_DOOR_H
#define Z_EN_DOOR_H

#include "global.h"

struct EnDoor;

typedef void (*EnDoorActionFunc)(struct EnDoor*, GlobalContext*);

typedef struct EnDoor {
    /* 0x000 */ Actor actor;
    /* 0x144 */ char unk_144[0x5C];
    /* 0x1A0 */ s8 unk_1A0;
    /* 0x1A1 */ s8 unk_1A1;
    /* 0x1A2 */ char unk_1A2[0x2];
    /* 0x1A4 */ u8 unk_1A4;
    /* 0x1A5 */ u8 unk_1A5;
    /* 0x1A6 */ s8 unk_1A6;
    /* 0x1A7 */ s8 unk_1A7;
    /* 0x1A8 */ char unk_1A8[0x20];
    /* 0x1C8 */ EnDoorActionFunc actionFunc;
} EnDoor; // size = 0x1CC

extern const ActorInit En_Door_InitVars;

#endif // Z_EN_DOOR_H
