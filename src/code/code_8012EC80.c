#include "global.h"

// Bit Flag array in which gBitFlags[n] is literally (1 << n)
u32 gBitFlags[] = {
    (1 << 0),  (1 << 1),  (1 << 2),  (1 << 3),  (1 << 4),  (1 << 5),  (1 << 6),  (1 << 7),
    (1 << 8),  (1 << 9),  (1 << 10), (1 << 11), (1 << 12), (1 << 13), (1 << 14), (1 << 15),
    (1 << 16), (1 << 17), (1 << 18), (1 << 19), (1 << 20), (1 << 21), (1 << 22), (1 << 23),
    (1 << 24), (1 << 25), (1 << 26), (1 << 27), (1 << 28), (1 << 29), (1 << 30), (1 << 31),
};

u16 gEquipMasks[] = { 0x000F, 0x00F0, 0x0F00, 0xF000 };
u16 gEquipNegMasks[] = { 0xFFF0, 0xFF0F, 0xF0FF, 0x0FFF };
u32 gUpgradeMasks[] = {
    0x00000007, 0x00000038, 0x000001C0, 0x00000E00, 0x00003000, 0x0001C000, 0x000E0000, 0x00700000,
};
u32 gUpgradeNegMasks[] = {
    0xFFFFFFF8, 0xFFFFFFC7, 0xFFFFFE3F, 0xFFFFF1FF, 0xFFFFCFFF, 0xFFFE3FFF, 0xFFF1FFFF, 0xFF8FFFFF,
};
u8 gEquipShifts[] = { 0, 4, 8, 12 };
u8 gUpgradeShifts[] = { 0, 3, 6, 9, 12, 14, 17, 20 };

u16 gUpgradeCapacities[][4] = {
    { 0, 30, 40, 50 },     // Quivers
    { 0, 20, 30, 40 },     // Bomb Bags
    { 0, 0, 0, 0 },        // Unused (Scale)
    { 0, 0, 0, 0 },        // Unused (Strength)
    { 99, 200, 500, 500 }, // Wallets
    { 0, 30, 40, 50 },     // Unused (Deku Seed Bullet Bags)
    { 0, 10, 20, 30 },     // Unused (Deku Stick)
    { 0, 20, 30, 40 },     // Unused (Deku Nut)
};

u32 gGsFlagsMask[] = { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
u32 gGsFlagsShift[] = { 0, 8, 16, 24 };

// TODO: use symbols for these icon textures once textures are properly in C
void* gItemIcons[] = {
    0x08000000, // ITEM_OCARINA
    0x08001000, // ITEM_BOW
    0x08002000, // ITEM_ARROW_FIRE
    0x08003000, // ITEM_ARROW_ICE
    0x08004000, // ITEM_ARROW_LIGHT
    0x08005000, // ITEM_OCARINA_FAIRY
    0x08006000, // ITEM_BOMB
    0x08007000, // ITEM_BOMBCHU
    0x08008000, // ITEM_STICK
    0x08009000, // ITEM_NUT
    0x0800A000, // ITEM_MAGIC_BEANS
    0x0800B000, // ITEM_SLINGSHOT
    0x0800C000, // ITEM_POWDER_KEG
    0x0800D000, // ITEM_PICTO_BOX
    0x0800E000, // ITEM_LENS
    0x0800F000, // ITEM_HOOKSHOT
    0x08010000, // ITEM_SWORD_GREAT_FAIRY
    0x08011000, // ITEM_LONGSHOT
    0x08012000, // ITEM_BOTTLE
    0x08013000, // ITEM_POTION_RED
    0x08014000, // ITEM_POTION_GREEN
    0x08015000, // ITEM_POTION_BLUE
    0x08016000, // ITEM_FAIRY
    0x08017000, // ITEM_DEKU_PRINCESS
    0x08018000, // ITEM_MILK_BOTTLE
    0x08019000, // ITEM_MILK_HALF
    0x0801A000, // ITEM_FISH
    0x0801B000, // ITEM_BUG
    0x0801C000, // ITEM_BLUE_FIRE
    0x0801D000, // ITEM_POE
    0x0801E000, // ITEM_BIG_POE
    0x0801F000, // ITEM_SPRING_WATER
    0x08020000, // ITEM_HOT_SPRING_WATER
    0x08021000, // ITEM_ZORA_EGG
    0x08022000, // ITEM_GOLD_DUST
    0x08023000, // ITEM_MUSHROOM
    0x08024000, // ITEM_SEA_HORSE
    0x08025000, // ITEM_CHATEAU
    0x08026000, // ITEM_HYLIAN_LOACH
    0x08027000, // ITEM_OBABA_DRINK
    0x08028000, // ITEM_MOON_TEAR
    0x08029000, // ITEM_DEED_LAND
    0x0802A000, // ITEM_DEED_SWAMP
    0x0802B000, // ITEM_DEED_MOUNTAIN
    0x0802C000, // ITEM_DEED_OCEAN
    0x0802D000, // ITEM_ROOM_KEY
    0x0802E000, // ITEM_LETTER_MAMA
    0x0802F000, // ITEM_LETTER_TO_KAFEI
    0x08030000, // ITEM_PENDANT_MEMORIES
    0x08031000, // ITEM_TINGLE_MAP
    0x08032000, // ITEM_MASK_DEKU
    0x08033000, // ITEM_MASK_GORON
    0x08034000, // ITEM_MASK_ZORA
    0x08035000, // ITEM_MASK_FIERCE_DEITY
    0x08036000, // ITEM_MASK_TRUTH
    0x08037000, // ITEM_MASK_KAFEIS_MASK
    0x08038000, // ITEM_MASK_ALL_NIGHT
    0x08039000, // ITEM_MASK_BUNNY
    0x0803A000, // ITEM_MASK_KEATON
    0x0803B000, // ITEM_MASK_GARO
    0x0803C000, // ITEM_MASK_ROMANI
    0x0803D000, // ITEM_MASK_CIRCUS_LEADER
    0x0803E000, // ITEM_MASK_POSTMAN
    0x0803F000, // ITEM_MASK_COUPLE
    0x08040000, // ITEM_MASK_GREAT_FAIRY
    0x08041000, // ITEM_MASK_GIBDO
    0x08042000, // ITEM_MASK_DON_GERO
    0x08043000, // ITEM_MASK_KAMARO
    0x08044000, // ITEM_MASK_CAPTAIN
    0x08045000, // ITEM_MASK_STONE
    0x08046000, // ITEM_MASK_BREMEN
    0x08047000, // ITEM_MASK_BLAST
    0x08048000, // ITEM_MASK_SCENTS
    0x08049000, // ITEM_MASK_GIANT
    0x0804A000, // ITEM_BOW_ARROW_FIRE
    0x0804B000, // ITEM_BOW_ARROW_ICE
    0x0804C000, // ITEM_BOW_ARROW_LIGHT
    0x0804D000, // ITEM_SWORD_KOKIRI
    0x0804E000, // ITEM_SWORD_RAZOR
    0x0804F000, // ITEM_SWORD_GILDED
    0x08050000, // ITEM_SWORD_DEITY
    0x08051000, // ITEM_SHIELD_HERO
    0x08052000, // ITEM_SHIELD_MIRROR
    0x08053000, // ITEM_QUIVER_30
    0x08054000, // ITEM_QUIVER_40
    0x08055000, // ITEM_QUIVER_50
    0x08056000, // ITEM_BOMB_BAG_20
    0x08057000, // ITEM_BOMB_BAG_30
    0x08058000, // ITEM_BOMB_BAG_40
    0x08059000, // ITEM_WALLET_DEFAULT
    0x0805A000, // ITEM_WALLET_ADULT
    0x0805B000, // ITEM_WALLET_GIANT
    0x0805C000, // ITEM_FISHING_POLE
    0x0805D000, // ITEM_REMAINS_ODOLWA
    0x0805E000, // ITEM_REMAINS_GOHT
    0x0805F000, // ITEM_REMAINS_GYORG
    0x08060000, // ITEM_REMAINS_TWINMOLD
    0x08062000, // ITEM_SONG_SONATA
    0x08062000, // ITEM_SONG_LULLABY
    0x08062000, // ITEM_SONG_NOVA
    0x08062000, // ITEM_SONG_ELEGY
    0x08062000, // ITEM_SONG_OATH
    0x08062000, // ITEM_SONG_SARIA
    0x08062000, // ITEM_SONG_TIME
    0x08062000, // ITEM_SONG_HEALING
    0x08062000, // ITEM_SONG_EPONA
    0x08062000, // ITEM_SONG_SOARING
    0x08062000, // ITEM_SONG_STORMS
    0x08062000, // ITEM_SONG_SUN
    0x08061000, // ITEM_BOMBERS_NOTEBOOK
    0x09000000, // ITEM_SKULL_TOKEN
    0x09000900, // ITEM_HEART_CONTAINER
    0x09001200, // ITEM_HEART_PIECE
    0x08062000, //
    0x08062000, //
    0x08062000, // ITEM_SONG_LULLABY_INTRO
    0x09003600, // ITEM_KEY_BOSS
    0x09004800, // ITEM_COMPASS
    0x09003F00, // ITEM_DUNGEON_MAP
    0x09005100, // ITEM_STRAY_FAIRIES
    0x09005A00, // ITEM_KEY_SMALL
    0x09006300, // ITEM_MAGIC_SMALL
    0x09006C00, // ITEM_MAGIC_LARGE
    0x08062180, // ITEM_HEART_PIECE_2
    0x08062A80, // ITEM_INVALID_1
    0x08063380, // ITEM_INVALID_2
    0x020028A0, // ITEM_INVALID_3
    0x020025A0, // ITEM_INVALID_4
    0x020027A0, // ITEM_INVALID_5
    0x020026A0, // ITEM_INVALID_6
    0x020024A0, // ITEM_INVALID_7
};

// Used to map item IDs to inventory slots
u8 gItemSlots[] = {
    SLOT_OCARINA,            // ITEM_OCARINA
    SLOT_BOW,                // ITEM_BOW
    SLOT_ARROW_FIRE,         // ITEM_ARROW_FIRE
    SLOT_ARROW_ICE,          // ITEM_ARROW_ICE
    SLOT_ARROW_LIGHT,        // ITEM_ARROW_LIGHT
    SLOT_TRADE_DEED,         // ITEM_OCARINA_FAIRY
    SLOT_BOMB,               // ITEM_BOMB
    SLOT_BOMBCHU,            // ITEM_BOMBCHU
    SLOT_STICK,              // ITEM_STICK
    SLOT_NUT,                // ITEM_NUT
    SLOT_MAGIC_BEANS,        // ITEM_MAGIC_BEANS
    SLOT_TRADE_KEY_MAMA,     // ITEM_SLINGSHOT
    SLOT_POWDER_KEG,         // ITEM_POWDER_KEG
    SLOT_PICTO_BOX,          // ITEM_PICTO_BOX
    SLOT_LENS,               // ITEM_LENS
    SLOT_HOOKSHOT,           // ITEM_HOOKSHOT
    SLOT_SWORD_GREAT_FAIRY,  // ITEM_SWORD_GREAT_FAIRY
    SLOT_BOTTLE_1,           //
    SLOT_BOTTLE_1,           // ITEM_BOTTLE
    SLOT_BOTTLE_1,           // ITEM_POTION_RED
    SLOT_BOTTLE_1,           // ITEM_POTION_GREEN
    SLOT_BOTTLE_1,           // ITEM_POTION_BLUE
    SLOT_BOTTLE_1,           // ITEM_FAIRY
    SLOT_BOTTLE_1,           // ITEM_DEKU_PRINCESS
    SLOT_BOTTLE_1,           // ITEM_MILK_BOTTLE
    SLOT_BOTTLE_1,           // ITEM_MILK_HALF
    SLOT_BOTTLE_1,           // ITEM_FISH
    SLOT_BOTTLE_1,           // ITEM_BUG
    SLOT_BOTTLE_1,           // ITEM_BLUE_FIRE
    SLOT_BOTTLE_1,           // ITEM_POE
    SLOT_BOTTLE_1,           // ITEM_BIG_POE
    SLOT_BOTTLE_1,           // ITEM_SPRING_WATER
    SLOT_BOTTLE_1,           // ITEM_HOT_SPRING_WATER
    SLOT_BOTTLE_1,           // ITEM_ZORA_EGG
    SLOT_BOTTLE_1,           // ITEM_GOLD_DUST
    SLOT_BOTTLE_1,           // ITEM_MUSHROOM
    SLOT_BOTTLE_1,           // ITEM_SEA_HORSE
    SLOT_BOTTLE_1,           // ITEM_CHATEAU
    SLOT_BOTTLE_1,           // ITEM_HYLIAN_LOACH
    SLOT_BOTTLE_1,           // ITEM_OBABA_DRINK
    SLOT_TRADE_DEED,         // ITEM_MOON_TEAR
    SLOT_TRADE_DEED,         // ITEM_DEED_LAND
    SLOT_TRADE_DEED,         // ITEM_DEED_SWAMP
    SLOT_TRADE_DEED,         // ITEM_DEED_MOUNTAIN
    SLOT_TRADE_DEED,         // ITEM_DEED_OCEAN
    SLOT_TRADE_KEY_MAMA,     // ITEM_ROOM_KEY
    SLOT_TRADE_KEY_MAMA,     // ITEM_LETTER_MAMA
    SLOT_TRADE_COUPLE,       // ITEM_LETTER_TO_KAFEI
    SLOT_TRADE_COUPLE,       // ITEM_PENDANT_MEMORIES
    SLOT_TRADE_COUPLE,       // ITEM_TINGLE_MAP
    SLOT_MASK_DEKU,          // ITEM_MASK_DEKU
    SLOT_MASK_GORON,         // ITEM_MASK_GORON
    SLOT_MASK_ZORA,          // ITEM_MASK_ZORA
    SLOT_MASK_FIERCE_DEITY,  // ITEM_MASK_FIERCE_DEITY
    SLOT_MASK_TRUTH,         // ITEM_MASK_TRUTH
    SLOT_MASK_KAFEIS_MASK,   // ITEM_MASK_KAFEIS_MASK
    SLOT_MASK_ALL_NIGHT,     // ITEM_MASK_ALL_NIGHT
    SLOT_MASK_BUNNY,         // ITEM_MASK_BUNNY
    SLOT_MASK_KEATON,        // ITEM_MASK_KEATON
    SLOT_MASK_GARO,          // ITEM_MASK_GARO
    SLOT_MASK_ROMANI,        // ITEM_MASK_ROMANI
    SLOT_MASK_CIRCUS_LEADER, // ITEM_MASK_CIRCUS_LEADER
    SLOT_MASK_POSTMAN,       // ITEM_MASK_POSTMAN
    SLOT_MASK_COUPLE,        // ITEM_MASK_COUPLE
    SLOT_MASK_GREAT_FAIRY,   // ITEM_MASK_GREAT_FAIRY
    SLOT_MASK_GIBDO,         // ITEM_MASK_GIBDO
    SLOT_MASK_DON_GERO,      // ITEM_MASK_DON_GERO
    SLOT_MASK_KAMARO,        // ITEM_MASK_KAMARO
    SLOT_MASK_CAPTAIN,       // ITEM_MASK_CAPTAIN
    SLOT_MASK_STONE,         // ITEM_MASK_STONE
    SLOT_MASK_BREMEN,        // ITEM_MASK_BREMEN
    SLOT_MASK_BLAST,         // ITEM_MASK_BLAST
    SLOT_MASK_SCENTS,        // ITEM_MASK_SCENTS
    SLOT_MASK_GIANT,         // ITEM_MASK_GIANT
    SLOT_BOW,                // ITEM_BOW_ARROW_FIRE
    SLOT_BOW,                // ITEM_BOW_ARROW_ICE
    SLOT_BOW,                // ITEM_BOW_ARROW_LIGHT
};

s16 gItemPrices[] = {
    0,   // ITEM_OCARINA
    0,   // ITEM_BOW
    0,   // ITEM_ARROW_FIRE
    0,   // ITEM_ARROW_ICE
    0,   // ITEM_ARROW_LIGHT
    0,   // ITEM_OCARINA_FAIRY
    0,   // ITEM_BOMB
    0,   // ITEM_BOMBCHU
    0,   // ITEM_STICK
    0,   // ITEM_NUT
    0,   // ITEM_MAGIC_BEANS
    0,   // ITEM_SLINGSHOT
    0,   // ITEM_POWDER_KEG
    0,   // ITEM_PICTO_BOX
    0,   // ITEM_LENS
    0,   // ITEM_HOOKSHOT
    0,   // ITEM_SWORD_GREAT_FAIRY
    0,   // ITEM_LONGSHOT
    0,   // ITEM_BOTTLE
    20,  // ITEM_POTION_RED
    20,  // ITEM_POTION_GREEN
    20,  // ITEM_POTION_BLUE
    20,  // ITEM_FAIRY
    0,   // ITEM_DEKU_PRINCESS
    20,  // ITEM_MILK_BOTTLE
    5,   // ITEM_MILK_HALF
    20,  // ITEM_FISH
    20,  // ITEM_BUG
    50,  // ITEM_BLUE_FIRE
    50,  // ITEM_POE
    200, // ITEM_BIG_POE
    20,  // ITEM_SPRING_WATER
    20,  // ITEM_HOT_SPRING_WATER
    20,  // ITEM_ZORA_EGG
    200, // ITEM_GOLD_DUST
    5,   // ITEM_MUSHROOM
    0,   // ITEM_SEA_HORSE
    200, // ITEM_CHATEAU
    20,  // ITEM_HYLIAN_LOACH
    0,   // ITEM_OBABA_DRINK
    0,   // ITEM_MOON_TEAR
    0,   // ITEM_DEED_LAND
    0,   // ITEM_DEED_SWAMP
    0,   // ITEM_DEED_MOUNTAIN
    0,   // ITEM_DEED_OCEAN
    0,   // ITEM_ROOM_KEY
    0,   // ITEM_LETTER_MAMA
    0,   // ITEM_LETTER_KAFEI
    0,   // ITEM_PENDANT_MEMORIES
    0,   // ITEM_TINGLE_MAP
};

// Used to map scene indexes to their region in Termina
u16 gScenesPerRegion[11][27] = {
    // Great Bay
    {
        SCENE_30GYOSON,
        SCENE_LABO,
        SCENE_FISHERMAN,
        SCENE_SINKAI,
        SCENE_31MISAKI,
        SCENE_TORIDE,
        SCENE_KAIZOKU,
        SCENE_PIRATE,
        SCENE_35TAKI,
        SCENE_KINDAN2,
        -1,
    },
    // Zora Hall
    {
        SCENE_33ZORACITY,
        SCENE_BANDROOM,
        -1,
    },
    // Romani Ranch
    {
        SCENE_F01,
        SCENE_ROMANYMAE,
        SCENE_OMOYA,
        SCENE_F01C,
        SCENE_F01_B,
        SCENE_KOEPONARACE,
        -1,
    },
    // Deku Palace
    {
        SCENE_22DEKUCITY,
        SCENE_DEKU_KING,
        SCENE_26SARUNOMORI,
        SCENE_DANPEI,
        -1,
    },
    // Southern Swamp
    {
        SCENE_20SICHITAI,
        SCENE_20SICHITAI2,
        SCENE_MAP_SHOP,
        SCENE_WITCH_SHOP,
        SCENE_21MITURINMAE,
        SCENE_KINSTA1,
        -1,
    },
    // Clock Town
    {
        SCENE_00KEIKOKU,    SCENE_TENMON_DAI,   SCENE_13HUBUKINOMITI,
        SCENE_24KEMONOMITI, SCENE_SYATEKI_MORI, SCENE_IKANAMAE,
        SCENE_TOWN,         SCENE_SYATEKI_MIZU, SCENE_BOWLING,
        SCENE_TAKARAYA,     SCENE_YADOYA,       SCENE_SONCHONOIE,
        SCENE_MILK_BAR,     SCENE_ICHIBA,       SCENE_BOMYA,
        SCENE_AYASHIISHOP,  SCENE_8ITEMSHOP,    SCENE_DOUJOU,
        SCENE_POSTHOUSE,    SCENE_TAKARAKUJI,   SCENE_BACKTOWN,
        SCENE_CLOCKTOWER,   SCENE_INSIDETOWER,  SCENE_OKUJOU,
        SCENE_ALLEY,        SCENE_DEKUTES,      -1,
    },
    // Snowhead
    {
        SCENE_10YUKIYAMANOMURA,
        SCENE_10YUKIYAMANOMURA2,
        SCENE_KAJIYA,
        SCENE_12HAKUGINMAE,
        SCENE_14YUKIDAMANOMITI,
        SCENE_GORONRACE,
        SCENE_GORON_HAKA,
        SCENE_17SETUGEN,
        SCENE_17SETUGEN2,
        -1,
    },
    // Ikana Graveyard
    {
        SCENE_BOTI,
        SCENE_DANPEI2TEST,
        -1,
    },
    // Ikana Canyon
    {
        SCENE_CASTLE,
        SCENE_IKNINSIDE,
        SCENE_IKANA,
        SCENE_SECOM,
        SCENE_MUSICHOUSE,
        SCENE_RANDOM,
        SCENE_REDEAD,
        SCENE_TOUGITES,
        SCENE_HAKASHITA,
        -1,
    },
    // Goron Village
    {
        SCENE_11GORONNOSATO,
        SCENE_11GORONNOSATO2,
        SCENE_16GORON_HOUSE,
        SCENE_GORONSHOP,
        -1,
    },
    // Stone Tower
    {
        SCENE_F40,
        SCENE_F41,
        -1,
    },
};

s32 func_8012EC80(GlobalContext* globalCtx) {
    if (gSaveContext.buttonStatus[0] == BTN_DISABLED) {
        return ITEM_NONE;
    } else if (gSaveContext.unk_1015 == ITEM_NONE) {
        return ITEM_NONE;
    } else if (CUR_FORM_EQUIP(EQUIP_SLOT_B) == ITEM_NONE) {
        if (globalCtx->interfaceCtx.unk_21C != 0) {
            if (globalCtx->interfaceCtx.unk_21E != 0) {
                return globalCtx->interfaceCtx.unk_21E;
            }
        }
        return ITEM_NONE;
    } else {
        return CUR_FORM_EQUIP(EQUIP_SLOT_B);
    }
}

void func_8012ED34(s16 equipment);
#pragma GLOBAL_ASM("asm/non_matchings/code/code_8012EC80/func_8012ED34.s")

u8 func_8012ED78(GlobalContext* globalCtx, s16 equipment);
#pragma GLOBAL_ASM("asm/non_matchings/code/code_8012EC80/func_8012ED78.s")

void func_8012EDE8(s16 arg0, u32 arg1);
#pragma GLOBAL_ASM("asm/non_matchings/code/code_8012EC80/func_8012EDE8.s")

s32 func_8012EE34(s16 arg0) {
    s16 num = 0;

    if (arg0 >= 0x20) {
        if (arg0 < 0x40) {
            num = 1;
        } else if (arg0 < 0x60) {
            num = 2;
        } else if (arg0 < 0x80) {
            num = 3;
        } else if (arg0 < 0xA0) {
            num = 4;
        } else if (arg0 < 0xC0) {
            num = 5;
        } else if (arg0 < 0xE0) {
            num = 6;
        }
    }

    if (gSaveContext.roomInf[125][num] & gBitFlags[arg0 - (num << 5)]) {
        return 1;
    }

    return 0;
}

static u16 D_801C2380[6][12] = {
    { SCENE_00KEIKOKU, SCENE_BOTI, SCENE_13HUBUKINOMITI, SCENE_24KEMONOMITI, SCENE_IKANAMAE, SCENE_TOWN, SCENE_ICHIBA,
      SCENE_BACKTOWN, SCENE_CLOCKTOWER, SCENE_ALLEY, -1 },
    { SCENE_20SICHITAI, SCENE_20SICHITAI2, SCENE_21MITURINMAE, SCENE_22DEKUCITY, SCENE_DEKU_KING, SCENE_KINSTA1, -1 },
    { SCENE_10YUKIYAMANOMURA, SCENE_10YUKIYAMANOMURA2, SCENE_11GORONNOSATO, SCENE_11GORONNOSATO2, SCENE_16GORON_HOUSE,
      SCENE_12HAKUGINMAE, SCENE_14YUKIDAMANOMITI, SCENE_GORONRACE, SCENE_17SETUGEN, SCENE_17SETUGEN2, -1 },
    { SCENE_F01, SCENE_ROMANYMAE, SCENE_OMOYA, SCENE_F01C, SCENE_F01_B, SCENE_KOEPONARACE, -1 },
    { SCENE_30GYOSON, SCENE_SINKAI, SCENE_31MISAKI, SCENE_TORIDE, SCENE_KAIZOKU, SCENE_33ZORACITY, SCENE_35TAKI, -1 },
    { SCENE_F40, SCENE_F41, SCENE_CASTLE, SCENE_IKANA, SCENE_REDEAD, -1 },
};

void func_8012EF0C(s16 arg0) {
    s16 phi_v0 = 0;
    s16 phi_v1 = 0;
    u16(*new_var)[] = &D_801C2380[arg0];

    if ((arg0 >= 0) && (arg0 < 6)) {
        while (true) {
            if ((*new_var)[phi_v0] == 0xFFFF) {
                break;
            }

            if (((s16)(*new_var)[phi_v0]) < 0x20) {
                phi_v1 = 0;
            } else if (((s16)(*new_var)[phi_v0]) < 0x40) {
                phi_v1 = 1;
            } else if (((s16)(*new_var)[phi_v0]) < 0x60) {
                phi_v1 = 2;
            } else if (((s16)(*new_var)[phi_v0]) < 0x80) {
                phi_v1 = 3;
            } else if (((s16)(*new_var)[phi_v0]) < 0xA0) {
                phi_v1 = 4;
            } else if (((s16)(*new_var)[phi_v0]) < 0xC0) {
                phi_v1 = 5;
            } else if (((s16)(*new_var)[phi_v0]) < 0xE0) {
                phi_v1 = 6;
            }

            // Required to match
            gSaveContext.roomInf[125][phi_v1] =
                gSaveContext.roomInf[125][phi_v1] | gBitFlags[(s16)(*new_var)[phi_v0] - (phi_v1 << 5)];
            phi_v0++;
        }

        if ((*new_var) == D_801C2380[0]) {
            gSaveContext.unk_F60 |= 3;
        } else if ((*new_var) == D_801C2380[1]) {
            gSaveContext.unk_F60 |= 0x1C;
        } else if ((*new_var) == D_801C2380[2]) {
            gSaveContext.unk_F60 |= 0xE0;
        } else if ((*new_var) == D_801C2380[3]) {
            gSaveContext.unk_F60 |= 0x100;
        } else if ((*new_var) == D_801C2380[4]) {
            gSaveContext.unk_F60 |= 0x1E00;
        } else if ((*new_var) == D_801C2380[5]) {
            gSaveContext.unk_F60 |= 0x6000;
        }
    }

    XREG(95) = 0;
}

void func_8012F0EC(s16 arg0);
#pragma GLOBAL_ASM("asm/non_matchings/code/code_8012EC80/func_8012F0EC.s")

#ifdef NON_MATCHING
// regalloc
void func_8012F1BC(s16 sceneIndex) {
    if (sceneIndex == SCENE_KINSTA1) {
        gSaveContext.roomInf[126][0] = ((((gSaveContext.roomInf[126][0] & 0xFFFF0000) >> 0x10) + 1) << 0x10) |
                                       (gSaveContext.roomInf[126][0] & 0xFFFF);
    } else {
        gSaveContext.roomInf[126][0] =
            ((gSaveContext.roomInf[126][0] + 1) & 0xFFFF) | (gSaveContext.roomInf[126][0] & 0xFFFF0000);
    }
}
#else
void func_8012F1BC(s16 arg0);
#pragma GLOBAL_ASM("asm/non_matchings/code/code_8012EC80/func_8012F1BC.s")
#endif

s16 func_8012F22C(s16 sceneIndex) {
    if (sceneIndex == SCENE_KINSTA1) {
        return (gSaveContext.roomInf[126][0] & 0xFFFF0000) >> 0x10;
    } else {
        return gSaveContext.roomInf[126][0] & 0xFFFF;
    }
}

void func_8012F278(GlobalContext* globalCtx);
#pragma GLOBAL_ASM("asm/non_matchings/code/code_8012EC80/func_8012F278.s")
