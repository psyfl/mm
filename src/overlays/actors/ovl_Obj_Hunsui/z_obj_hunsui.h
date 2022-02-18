#ifndef Z_OBJ_HUNSUI_H
#define Z_OBJ_HUNSUI_H

#include "global.h"

struct ObjHunsui;

typedef void (*ObjHunsuiActionFunc)(struct ObjHunsui*, GlobalContext*);

#define OBJHUNSUI_GET_F000(thisx) (((thisx)->params >> 0xC) & 0xF)

typedef struct ObjHunsui {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ char unk_144[0x18];
    /* 0x015C */ ObjHunsuiActionFunc actionFunc;
    /* 0x0160 */ char unk_160[0x58];
} ObjHunsui; // size = 0x1B8

extern const ActorInit Obj_Hunsui_InitVars;

#endif // Z_OBJ_HUNSUI_H
