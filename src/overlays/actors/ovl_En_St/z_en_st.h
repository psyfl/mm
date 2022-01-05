#ifndef Z_EN_ST_H
#define Z_EN_ST_H

#include "global.h"

struct EnSt;

typedef void (*EnStActionFunc)(struct EnSt*, GlobalContext*);

#define ENST_GET_3F(thisx) (((thisx)->params & 0x3F) & 0xFF)
#define ENST_GET_1C0(thisx) ((((thisx)->params & 0x1C0) >> 6) & 0xFF)

#define ENST_3F_63 63
#define ENST_1C0_1 1

typedef struct EnSt {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnStActionFunc actionFunc;
    /* 0x18C */ u16 unk_18C;
    /* 0x18E */ u8 unk_18E;
    /* 0x190 */ ColliderCylinder collider1;
    /* 0x1DC */ ColliderCylinder collider2;
    /* 0x228 */ ColliderCylinder collider3;
    /* 0x274 */ ColliderCylinder collider4;
    /* 0x2C0 */ s8 unk_2C0;
    /* 0x2C4 */ f32 unk_2C4;
    /* 0x2C8 */ f32 unk_2C8;
    /* 0x2CC */ f32 unk_2CC;
    /* 0x2D0 */ f32 unk_2D0;
    /* 0x2D4 */ f32 unk_2D4;
    /* 0x2D8 */ f32 unk_2D8;
    /* 0x2DC */ f32 unk_2DC[12];
    /* 0x30C */ s16 unk_30C;
    /* 0x30E */ s16 unk_30E;
    /* 0x310 */ s16 unk_310;
    /* 0x312 */ s16 unk_312;
    /* 0x314 */ s16 unk_314;
    /* 0x316 */ s16 unk_316;
    /* 0x318 */ s16 unk_318;
    /* 0x31A */ s16 unk_31A;
    /* 0x31C */ s16 unk_31C[12];
    /* 0x334 */ s16 unk_334[12];
    /* 0x34C */ f32 unk_34C;
    /* 0x350 */ UNK_TYPE1 unk_350[0x8];
    /* 0x358 */ Vec3f unk_358[12];
    /* 0x3E8 */ Vec3s jointTable[30];
    /* 0x49C */ Vec3s morphTable[30];
} EnSt; // size = 0x550

extern const ActorInit En_St_InitVars;

#endif // Z_EN_ST_H
