#ifndef _Z64ITEM_H_
#define _Z64ITEM_H_

#define DROP_TABLE_SIZE 16
#define DROP_TABLE_NUMBER 17

// TODO fill out these enums

typedef enum {
    /* 0x0 */ EQUIP_SWORD,
    /* 0x1 */ EQUIP_SHIELD
} EquipmentType;

typedef enum {
    /* 0x00 */ UPG_QUIVER,
    /* 0x01 */ UPG_BOMB_BAG,
    /* 0x02 */ UPG_STRENGTH,
    /* 0x03 */ UPG_SCALE,
    /* 0x04 */ UPG_WALLET,
    /* 0x05 */ UPG_BULLET_BAG,
    /* 0x06 */ UPG_STICKS,
    /* 0x07 */ UPG_NUTS
} UpgradeType;

typedef enum {
    /* 0x00 */ QUEST_REMAINS_ODOWLA,
    /* 0x01 */ QUEST_REMAINS_GOHT,
    /* 0x02 */ QUEST_REMAINS_GYORG,
    /* 0x03 */ QUEST_REMAINS_TWINMOLD,
    /* 0x04 */ QUEST_SHIELD,
    /* 0x05 */ QUEST_SWORD,
    /* 0x06 */ QUEST_SONG_SONATA,
    /* 0x07 */ QUEST_SONG_LULLABY,
    /* 0x08 */ QUEST_SONG_NOVA,
    /* 0x09 */ QUEST_SONG_ELEGY,
    /* 0x0A */ QUEST_SONG_OATH,
    /* 0x0B */ QUEST_SONG_SARIA,
    /* 0x0C */ QUEST_SONG_TIME,
    /* 0x0D */ QUEST_SONG_HEALING,
    /* 0x0E */ QUEST_SONG_EPONA,
    /* 0x0F */ QUEST_SONG_SOARING,
    /* 0x10 */ QUEST_SONG_STORMS,
    /* 0x11 */ QUEST_SONG_SUN,
    /* 0x12 */ QUEST_BOMBERS_NOTEBOOK,
    /* 0x18 */ QUEST_SONG_LULLABY_INTRO = 0x18,
    /* 0x19 */ QUEST_UNK_19, // Related to PictoBox
} QuestItem;

typedef enum {
    /* 0x00 */ SLOT_OCARINA,
    /* 0x01 */ SLOT_BOW,
    /* 0x02 */ SLOT_ARROW_FIRE,
    /* 0x03 */ SLOT_ARROW_ICE,
    /* 0x04 */ SLOT_ARROW_LIGHT,
    /* 0x05 */ SLOT_TRADE_DEED,
    /* 0x06 */ SLOT_BOMB,
    /* 0x07 */ SLOT_BOMBCHU,
    /* 0x08 */ SLOT_STICK,
    /* 0x09 */ SLOT_NUT,
    /* 0x0A */ SLOT_MAGIC_BEANS,
    /* 0x0B */ SLOT_TRADE_KEY_MAMA,
    /* 0x0C */ SLOT_POWDER_KEG,
    /* 0x0D */ SLOT_PICTO_BOX,
    /* 0x0E */ SLOT_LENS,
    /* 0x0F */ SLOT_HOOKSHOT,
    /* 0x10 */ SLOT_SWORD_GREAT_FAIRY,
    /* 0x11 */ SLOT_TRADE_COUPLE,
    /* 0x12 */ SLOT_BOTTLE_1,
    /* 0x13 */ SLOT_BOTTLE_2,
    /* 0x14 */ SLOT_BOTTLE_3,
    /* 0x15 */ SLOT_BOTTLE_4,
    /* 0x16 */ SLOT_BOTTLE_5,
    /* 0x17 */ SLOT_BOTTLE_6,
    /* 0x18 */ SLOT_MASK_POSTMAN,
    /* 0x19 */ SLOT_MASK_ALL_NIGHT,
    /* 0x1A */ SLOT_MASK_BLAST,
    /* 0x1B */ SLOT_MASK_STONE,
    /* 0x1C */ SLOT_MASK_GREAT_FAIRY,
    /* 0x1D */ SLOT_MASK_DEKU,
    /* 0x1E */ SLOT_MASK_KEATON,
    /* 0x1F */ SLOT_MASK_BREMEN,
    /* 0x20 */ SLOT_MASK_BUNNY,
    /* 0x21 */ SLOT_MASK_DON_GERO,
    /* 0x22 */ SLOT_MASK_SCENTS,
    /* 0x23 */ SLOT_MASK_GORON,
    /* 0x24 */ SLOT_MASK_ROMANI,
    /* 0x25 */ SLOT_MASK_CIRCUS_LEADER,
    /* 0x26 */ SLOT_MASK_KAFEIS_MASK,
    /* 0x27 */ SLOT_MASK_COUPLE,
    /* 0x28 */ SLOT_MASK_TRUTH,
    /* 0x29 */ SLOT_MASK_ZORA,
    /* 0x2A */ SLOT_MASK_KAMARO,
    /* 0x2B */ SLOT_MASK_GIBDO,
    /* 0x2C */ SLOT_MASK_GARO,
    /* 0x2D */ SLOT_MASK_CAPTAIN,
    /* 0x2E */ SLOT_MASK_GIANT,
    /* 0x2F */ SLOT_MASK_FIERCE_DEITY,
    /* 0xFF */ SLOT_NONE = 0xFF
} InventorySlot;

typedef enum {
    /* 0x00 */ ITEM_OCARINA,
    /* 0x01 */ ITEM_BOW,
    /* 0x02 */ ITEM_ARROW_FIRE,
    /* 0x03 */ ITEM_ARROW_ICE,
    /* 0x04 */ ITEM_ARROW_LIGHT,
    /* 0x05 */ ITEM_OCARINA_FAIRY,
    /* 0x06 */ ITEM_BOMB,
    /* 0x07 */ ITEM_BOMBCHU,
    /* 0x08 */ ITEM_STICK,
    /* 0x09 */ ITEM_NUT,
    /* 0x0A */ ITEM_MAGIC_BEANS,
    /* 0x0B */ ITEM_SLINGSHOT,
    /* 0x0C */ ITEM_POWDER_KEG,
    /* 0x0D */ ITEM_PICTO_BOX,
    /* 0x0E */ ITEM_LENS,
    /* 0x0F */ ITEM_HOOKSHOT,
    /* 0x10 */ ITEM_SWORD_GREAT_FAIRY,
    /* 0x11 */ ITEM_LONGSHOT, // OoT Leftover
    /* 0x12 */ ITEM_BOTTLE,
    /* 0x13 */ ITEM_POTION_RED,
    /* 0x14 */ ITEM_POTION_GREEN,
    /* 0x15 */ ITEM_POTION_BLUE,
    /* 0x16 */ ITEM_FAIRY,
    /* 0x17 */ ITEM_DEKU_PRINCESS,
    /* 0x18 */ ITEM_MILK_BOTTLE,
    /* 0x19 */ ITEM_MILK_HALF,
    /* 0x1A */ ITEM_FISH,
    /* 0x1B */ ITEM_BUG,
    /* 0x1C */ ITEM_BLUE_FIRE,
    /* 0x1D */ ITEM_POE,
    /* 0x1E */ ITEM_BIG_POE,
    /* 0x1F */ ITEM_SPRING_WATER,
    /* 0x20 */ ITEM_HOT_SPRING_WATER,
    /* 0x21 */ ITEM_ZORA_EGG,
    /* 0x22 */ ITEM_GOLD_DUST,
    /* 0x23 */ ITEM_MUSHROOM,
    /* 0x24 */ ITEM_SEA_HORSE,
    /* 0x25 */ ITEM_CHATEAU,
    /* 0x26 */ ITEM_HYLIAN_LOACH,
    /* 0x27 */ ITEM_OBABA_DRINK,
    /* 0x28 */ ITEM_MOON_TEAR,
    /* 0x29 */ ITEM_DEED_LAND,
    /* 0x2A */ ITEM_DEED_SWAMP,
    /* 0x2B */ ITEM_DEED_MOUNTAIN,
    /* 0x2C */ ITEM_DEED_OCEAN,
    /* 0x2D */ ITEM_ROOM_KEY,
    /* 0x2E */ ITEM_LETTER_MAMA,
    /* 0x2F */ ITEM_LETTER_TO_KAFEI,
    /* 0x30 */ ITEM_PENDANT_MEMORIES,
    /* 0x31 */ ITEM_TINGLE_MAP,
    /* 0x32 */ ITEM_MASK_DEKU,
    /* 0x33 */ ITEM_MASK_GORON,
    /* 0x34 */ ITEM_MASK_ZORA,
    /* 0x35 */ ITEM_MASK_FIERCE_DEITY,
    /* 0x36 */ ITEM_MASK_TRUTH,
    /* 0x37 */ ITEM_MASK_KAFEIS_MASK,
    /* 0x38 */ ITEM_MASK_ALL_NIGHT,
    /* 0x39 */ ITEM_MASK_BUNNY,
    /* 0x3A */ ITEM_MASK_KEATON,
    /* 0x3B */ ITEM_MASK_GARO,
    /* 0x3C */ ITEM_MASK_ROMANI,
    /* 0x3D */ ITEM_MASK_CIRCUS_LEADER,
    /* 0x3E */ ITEM_MASK_POSTMAN,
    /* 0x3F */ ITEM_MASK_COUPLE,
    /* 0x40 */ ITEM_MASK_GREAT_FAIRY,
    /* 0x41 */ ITEM_MASK_GIBDO,
    /* 0x42 */ ITEM_MASK_DON_GERO,
    /* 0x43 */ ITEM_MASK_KAMARO,
    /* 0x44 */ ITEM_MASK_CAPTAIN,
    /* 0x45 */ ITEM_MASK_STONE,
    /* 0x46 */ ITEM_MASK_BREMEN,
    /* 0x47 */ ITEM_MASK_BLAST,
    /* 0x48 */ ITEM_MASK_SCENTS,
    /* 0x49 */ ITEM_MASK_GIANT,
    /* 0x4A */ ITEM_BOW_ARROW_FIRE,
    /* 0x4B */ ITEM_BOW_ARROW_ICE,
    /* 0x4C */ ITEM_BOW_ARROW_LIGHT,
    /* 0x4D */ ITEM_SWORD_KOKIRI,
    /* 0x4E */ ITEM_SWORD_RAZOR,
    /* 0x4F */ ITEM_SWORD_GILDED,
    /* 0x50 */ ITEM_SWORD_DEITY,
    /* 0x51 */ ITEM_SHIELD_HERO,
    /* 0x52 */ ITEM_SHIELD_MIRROR,
    /* 0x53 */ ITEM_QUIVER_30,
    /* 0x54 */ ITEM_QUIVER_40,
    /* 0x55 */ ITEM_QUIVER_50,
    /* 0x56 */ ITEM_BOMB_BAG_20,
    /* 0x57 */ ITEM_BOMB_BAG_30,
    /* 0x58 */ ITEM_BOMB_BAG_40,
    /* 0x59 */ ITEM_WALLET_DEFAULT,
    /* 0x5A */ ITEM_WALLET_ADULT,
    /* 0x5B */ ITEM_WALLET_GIANT,
    /* 0x5C */ ITEM_FISHING_POLE,
    /* 0x5D */ ITEM_REMAINS_ODOLWA,
    /* 0x5E */ ITEM_REMAINS_GOHT,
    /* 0x5F */ ITEM_REMAINS_GYORG,
    /* 0x60 */ ITEM_REMAINS_TWINMOLD,
    /* 0x61 */ ITEM_SONG_SONATA,
    /* 0x62 */ ITEM_SONG_LULLABY,
    /* 0x63 */ ITEM_SONG_NOVA,
    /* 0x64 */ ITEM_SONG_ELEGY,
    /* 0x65 */ ITEM_SONG_OATH,
    /* 0x66 */ ITEM_SONG_SARIA,
    /* 0x67 */ ITEM_SONG_TIME,
    /* 0x68 */ ITEM_SONG_HEALING,
    /* 0x69 */ ITEM_SONG_EPONA,
    /* 0x6A */ ITEM_SONG_SOARING,
    /* 0x6B */ ITEM_SONG_STORMS,
    /* 0x6C */ ITEM_SONG_SUN,
    /* 0x6D */ ITEM_BOMBERS_NOTEBOOK,
    /* 0x6E */ ITEM_SKULL_TOKEN,
    /* 0x6F */ ITEM_HEART_CONTAINER,
    /* 0x70 */ ITEM_HEART_PIECE,
    /* 0x73 */ ITEM_SONG_LULLABY_INTRO = 0x73,
    /* 0x74 */ ITEM_KEY_BOSS,
    /* 0x75 */ ITEM_COMPASS,
    /* 0x76 */ ITEM_DUNGEON_MAP,
    /* 0x77 */ ITEM_STRAY_FAIRIES,
    /* 0x78 */ ITEM_KEY_SMALL,
    /* 0x79 */ ITEM_MAGIC_SMALL,
    /* 0x7A */ ITEM_MAGIC_LARGE,
    /* 0x7B */ ITEM_HEART_PIECE_2,
    /* 0x7C */ ITEM_INVALID_1,
    /* 0x7D */ ITEM_INVALID_2,
    /* 0x7E */ ITEM_INVALID_3,
    /* 0x7F */ ITEM_INVALID_4,
    /* 0x80 */ ITEM_INVALID_5,
    /* 0x81 */ ITEM_INVALID_6,
    /* 0x82 */ ITEM_INVALID_7,
    /* 0x83 */ ITEM_HEART,
    /* 0x84 */ ITEM_RUPEE_GREEN,
    /* 0x85 */ ITEM_RUPEE_BLUE,
    /* 0x86 */ ITEM_RUPEE_10,
    /* 0x87 */ ITEM_RUPEE_RED,
    /* 0x88 */ ITEM_RUPEE_PURPLE,
    /* 0x89 */ ITEM_RUPEE_SILVER,
    /* 0x8A */ ITEM_RUPEE_HUGE,
    /* 0x8B */ ITEM_STICKS_5,
    /* 0x8C */ ITEM_STICKS_10,
    /* 0x8D */ ITEM_NUTS_5,
    /* 0x8E */ ITEM_NUTS_10,
    /* 0x8F */ ITEM_BOMBS_5,
    /* 0x90 */ ITEM_BOMBS_10,
    /* 0x91 */ ITEM_BOMBS_20,
    /* 0x92 */ ITEM_BOMBS_30,
    /* 0x93 */ ITEM_ARROWS_10,
    /* 0x94 */ ITEM_ARROWS_30,
    /* 0x95 */ ITEM_ARROWS_40,
    /* 0x96 */ ITEM_ARROWS_50,
    /* 0x97 */ ITEM_BOMBCHUS_20,
    /* 0x98 */ ITEM_BOMBCHUS_10,
    /* 0x99 */ ITEM_BOMBCHUS_1,
    /* 0x9A */ ITEM_BOMBCHUS_5,
    /* 0x9B */ ITEM_STICK_UPGRADE_20,
    /* 0x9C */ ITEM_STICK_UPGRADE_30,
    /* 0x9D */ ITEM_NUT_UPGRADE_30,
    /* 0x9E */ ITEM_NUT_UPGRADE_40,
    /* 0x9F */ ITEM_CHATEAU_2,
    /* 0xA0 */ ITEM_MILK,
    /* 0xA1 */ ITEM_GOLD_DUST_2,
    /* 0xA2 */ ITEM_HYLIAN_LOACH_2,
    /* 0xA3 */ ITEM_SEA_HORSE_CAUGHT,
    /* 0xFF */ ITEM_NONE = 0xFF
} ItemID;

typedef enum {
    /* 0x00 */ GI_NONE,
    /* 0x01 */ GI_RUPEE_GREEN,
    /* 0x02 */ GI_RUPEE_BLUE,
    /* 0x03 */ GI_RUPEE_10,
    /* 0x04 */ GI_RUPEE_RED,
    /* 0x05 */ GI_RUPEE_PURPLE,
    /* 0x06 */ GI_RUPEE_SILVER,
    /* 0x07 */ GI_RUPEE_HUGE,
    /* 0x08 */ GI_WALLET_ADULT,
    /* 0x09 */ GI_WALLET_GIANT,
    /* 0x0A */ GI_RECOVERY_HEART,
    /* 0x0C */ GI_HEART_PIECE = 0x0C,
    /* 0x0D */ GI_HEART_CONTAINER,
    /* 0x16 */ GI_BOMBS_10 = 0x16,
    /* 0x19 */ GI_STICKS_1 = 0x19,
    /* 0x1A */ GI_BOMBCHUS_10,
    /* 0x1B */ GI_BOMB_BAG_20,
    /* 0x1C */ GI_BOMB_BAG_30,
    /* 0x1D */ GI_BOMB_BAG_40,
    /* 0x1E */ GI_ARROWS_10,
    /* 0x1F */ GI_ARROWS_30,
    /* 0x20 */ GI_ARROWS_40,
    /* 0x21 */ GI_ARROWS_50,
    /* 0x22 */ GI_QUIVER_30,
    /* 0x23 */ GI_QUIVER_40,
    /* 0x24 */ GI_QUIVER_50,
    /* 0x28 */ GI_NUTS_1 = 0x28,
    /* 0x2A */ GI_NUTS_10 = 0x2A,
    /* 0x32 */ GI_SHIELD_HERO = 0x32,
    /* 0x33 */ GI_SHIELD_MIRROR,
    /* 0x35 */ GI_MAGIC_BEANS = 0x35,
    /* 0x3C */ GI_KEY_SMALL = 0x3C,
    /* 0x3E */ GI_MAP = 0x3E,
    /* 0x3F */ GI_COMPASS,
    /* 0x52 */ GI_SCALE_GOLD = 0x52, // Assumed, used in En_Fishing
    /* 0x59 */ GI_BOTTLE_POTION_RED = 0x59,
    /* 0x5B */ GI_POTION_RED = 0x5B,
    /* 0x5C */ GI_POTION_GREEN,
    /* 0x5D */ GI_POTION_BLUE,
    /* 0x5E */ GI_FAIRY,
    /* 0x60 */ GI_MILK_BOTTLE = 0x60,
    /* 0x78 */ GI_MASK_DEKU = 0x78,
    /* 0x79 */ GI_MASK_GORON,
    /* 0x7A */ GI_MASK_ZORA,
    /* 0x7B */ GI_MASK_FIERCE_DEITY,
    /* 0x7C */ GI_MASK_TRUTH,
    /* 0x7D */ GI_MASK_KAFEIS_MASK,
    /* 0x7E */ GI_MASK_ALL_NIGHT,
    /* 0x7F */ GI_MASK_BUNNY,
    /* 0x80 */ GI_MASK_KEATON,
    /* 0x81 */ GI_MASK_GARO,
    /* 0x82 */ GI_MASK_ROMANI,
    /* 0x83 */ GI_MASK_CIRCUS_LEADER,
    /* 0x84 */ GI_MASK_POSTMAN,
    /* 0x85 */ GI_MASK_COUPLE,
    /* 0x86 */ GI_MASK_GREAT_FAIRY,
    /* 0x87 */ GI_MASK_GIBDO,
    /* 0x88 */ GI_MASK_DON_GERO,
    /* 0x89 */ GI_MASK_KAMARO,
    /* 0x8A */ GI_MASK_CAPTAIN,
    /* 0x8B */ GI_MASK_STONE,
    /* 0x8C */ GI_MASK_BREMEN,
    /* 0x8D */ GI_MASK_BLAST,
    /* 0x8E */ GI_MASK_SCENTS,
    /* 0x8F */ GI_MASK_GIANT,
    /* 0x92 */ GI_MILK = 0x92,
    /* 0x96 */ GI_MOON_TEAR = 0x96,
    /* 0x97 */ GI_DEED_LAND,
    /* 0x98 */ GI_DEED_SWAMP,
    /* 0x99 */ GI_DEED_MOUNTAIN,
    /* 0x9A */ GI_DEED_OCEAN,
    /* 0x9B */ GI_SWORD_GREAT_FAIRY = 0x9B,
    /* 0x9C */ GI_SWORD_KOKIRI,
    /* 0x9D */ GI_SWORD_RAZOR,
    /* 0x9E */ GI_SWORD_GILDED,
    /* 0x9F */ GI_SHIELD_HERO_2, // Code that treats this as hero's shield is unused, so take with a grain of salt
    /* 0xA1 */ GI_LETTER_TO_MAMA = 0xA1,
    /* 0xA9 */ GI_BOTTLE = 0xA9,
    /* 0xAB */ GI_PENDANT_OF_MEMORIES = 0xAB,
    /* 0xBA */ GI_MAX = 0xBA
} GetItemID;

typedef enum {
    /* 0x00 */ GID_BOTTLE,
    /* 0x02 */ GID_02 = 0x02,
    /* 0x03 */ GID_03,
    /* 0x0A */ GID_COMPASS = 0x0A,
    /* 0x0E */ GID_0E = 0x0E,
    /* 0x0F */ GID_0F,
    /* 0x10 */ GID_MASK_ALL_NIGHT = 0x10,
    /* 0x11 */ GID_NUTS,
    /* 0x17 */ GID_BOMB_BAG_20 = 0x17,
    /* 0x18 */ GID_BOMB_BAG_30,
    /* 0x19 */ GID_BOMB_BAG_40,
    /* 0x1A */ GID_STICK = 0x1A,
    /* 0x1B */ GID_DUNGEON_MAP,
    /* 0x1E */ GID_BOMB = 0x1E,
    /* 0x23 */ GID_ARROWS_SMALL = 0x23,
    /* 0x24 */ GID_ARROWS_MEDIUM,
    /* 0x25 */ GID_ARROWS_LARGE,
    /* 0x26 */ GID_BOMBCHU,
    /* 0x27 */ GID_SHIELD_HERO,
    /* 0x30 */ GID_POTION_GREEN = 0x30,
    /* 0x31 */ GID_POTION_RED,
    /* 0x32 */ GID_POTION_BLUE,
    /* 0x33 */ GID_SHIELD_MIRROR,
    /* 0x3A */ GID_3A = 0x3A,
    /* 0x3B */ GID_FAIRY,
    /* 0x3E */ GID_3E = 0x3E,
    /* 0x44 */ GID_44 = 0x44,
    /* 0x45 */ GID_45,
    /* 0x55 */ GID_SWORD_KOKIRI = 0x55,
    /* 0x66 */ GID_SWORD_RAZOR = 0x66,
    /* 0x67 */ GID_SWORD_GILDED,
    /* 0x68 */ GID_SWORD_GREAT_FAIRY,
    /* 0x6A */ GID_6A = 0x6A,
    /* 0x6B */ GID_6B,
    /* 0x70 */ GID_70 = 0x70,
    /* 0x71 */ GID_71,
} GetItemDrawID;

// TODO: fill
typedef enum {
    /*   -1 */ EXCH_ITEM_MINUS1 = -1, // Unknown usage or function
    /* 0x00 */ EXCH_ITEM_NONE,
    /* 0x1E */ EXCH_ITEM_1E = 0x1E, // BOTTLE_MUSHROOM
    /* 0x2A */ EXCH_ITEM_2A = 0x2A, // MOON_TEAR?
    /* 0x2E */ EXCH_ITEM_2E = 0x2E
} ExchangeItemID;

#endif
