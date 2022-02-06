#ifndef Z64_AUDIO_H
#define Z64_AUDIO_H

#define MK_AUDIO_CMD(b0,b1,b2,b3) ((((b0) & 0xFF) << 0x18) | (((b1) & 0xFF) << 0x10) | (((b2) & 0xFF) << 0x8) | (((b3) & 0xFF) << 0))

#define NO_LAYER ((SequenceLayer*)(-1))

#define TATUMS_PER_BEAT 48

#define IS_SEQUENCE_CHANNEL_VALID(ptr) ((uintptr_t)(ptr) != (uintptr_t)&gAudioContext.sequenceChannelNone)

#define MAX_CHANNELS_PER_BANK 3

#define AUDIO_LERPIMP(v0, v1, t) (v0 + ((v1 - v0) * t))

#define ADSR_DISABLE 0
#define ADSR_HANG -1
#define ADSR_GOTO -2
#define ADSR_RESTART -3

#define AIBUF_LEN 0x580

typedef enum {
    /* 0 */ AUDIO_MODE_STEREO,
    /* 1 */ AUDIO_MODE_HEADSET,
    /* 2 */ AUDIO_MODE_UNK,
    /* 3 */ AUDIO_MODE_MONO,
    /* 4 */ AUDIO_MODE_SURROUND,
} AudioSoundMode;

typedef enum {
    /* 0 */ ADSR_STATE_DISABLED,
    /* 1 */ ADSR_STATE_INITIAL,
    /* 2 */ ADSR_STATE_START_LOOP,
    /* 3 */ ADSR_STATE_LOOP,
    /* 4 */ ADSR_STATE_FADE,
    /* 5 */ ADSR_STATE_HANG,
    /* 6 */ ADSR_STATE_DECAY,
    /* 7 */ ADSR_STATE_RELEASE,
    /* 8 */ ADSR_STATE_SUSTAIN
} AdsrStatus;

typedef enum {
    /* 0 */ MEDIUM_RAM,
    /* 1 */ MEDIUM_UNK1,
    /* 2 */ MEDIUM_CART,
    /* 3 */ MEDIUM_DISK_DRIVE,
    /* 4 */ MEDIUM_UNK4,
    /* 5 */ MEDIUM_UNK5
} SampleMedium;

typedef enum {
    /* 0 */ CODEC_ADPCM,
    /* 1 */ CODEC_S8,
    /* 2 */ CODEC_S16_INMEMORY,
    /* 3 */ CODEC_SMALL_ADPCM,
    /* 4 */ CODEC_REVERB,
    /* 5 */ CODEC_S16
} SampleCodec;

typedef enum {
    /* 0 */ SEQUENCE_TABLE,
    /* 1 */ FONT_TABLE,
    /* 2 */ SAMPLE_TABLE
} SampleBankTableType;

typedef enum {
    /* 0 */ CACHE_TEMPORARY,
    /* 1 */ CACHE_PERSISTENT,
    /* 2 */ CACHE_EITHER,
    /* 3 */ CACHE_PERMANENT
} AudioCacheType;

typedef enum {
    /*  0 */ OCARINA_SONG_SONATA,
    /*  1 */ OCARINA_SONG_GORON_LULLABY,
    /*  2 */ OCARINA_SONG_NEW_WAVE,
    /*  3 */ OCARINA_SONG_ELEGY,
    /*  4 */ OCARINA_SONG_OATH,
    /*  5 */ OCARINA_SONG_SARIAS,
    /*  6 */ OCARINA_SONG_TIME,
    /*  7 */ OCARINA_SONG_HEALING,
    /*  8 */ OCARINA_SONG_EPONAS,
    /*  9 */ OCARINA_SONG_SOARING,
    /* 10 */ OCARINA_SONG_STORMS,
    /* 11 */ OCARINA_SONG_SUNS,
    /* 12 */ OCARINA_SONG_INVERTED_TIME,
    /* 13 */ OCARINA_SONG_DOUBLE_TIME,
    /* 14 */ OCARINA_SONG_GORON_LULLABY_INTRO,
    /* 15 */ OCARINA_SONG_WIND_FISH_HUMAN, // "Ballad of the Wind Fish"
    /* 16 */ OCARINA_SONG_WIND_FISH_GORON,
    /* 17 */ OCARINA_SONG_WIND_FISH_ZORA,
    /* 18 */ OCARINA_SONG_WIND_FISH_DEKU,
    /* 19 */ OCARINA_SONG_EVAN_PART1,
    /* 20 */ OCARINA_SONG_EVAN_PART2,
    /* 21 */ OCARINA_SONG_ZELDAS_LULLABY,
    /* 22 */ OCARINA_SONG_SCARECROW,
    /* 23 */ OCARINA_SONG_TERMINA_WALL,
    /* 24 */ OCARINA_SONG_MAX,
} OcarinaSongId;

typedef enum {
    /*  0 */ OCARINA_BTN_A,
    /*  1 */ OCARINA_BTN_C_DOWN,
    /*  2 */ OCARINA_BTN_C_RIGHT,
    /*  3 */ OCARINA_BTN_C_LEFT,
    /*  4 */ OCARINA_BTN_C_UP,
    /* -1 */ OCARINA_BTN_INVALID = 0xFF
} OcarinaButtonIdx;

typedef enum {
    /* 0x0 */ OCARINA_PITCH_C4,
    /* 0x1 */ OCARINA_PITCH_DFLAT4,
    /* 0x2 */ OCARINA_PITCH_D4,
    /* 0x3 */ OCARINA_PITCH_EFLAT4,
    /* 0x4 */ OCARINA_PITCH_E4,
    /* 0x5 */ OCARINA_PITCH_F4,
    /* 0x6 */ OCARINA_PITCH_GFLAT4,
    /* 0x7 */ OCARINA_PITCH_G4,
    /* 0x8 */ OCARINA_PITCH_AFLAT4,
    /* 0x9 */ OCARINA_PITCH_A4,
    /* 0xA */ OCARINA_PITCH_BFLAT4,
    /* 0xB */ OCARINA_PITCH_B4,
    /* 0xC */ OCARINA_PITCH_C5,
    /* 0xD */ OCARINA_PITCH_DFLAT5,
    /* 0xE */ OCARINA_PITCH_D5,
    /* 0xF */ OCARINA_PITCH_EFLAT5,
    /* -1  */ OCARINA_PITCH_NONE = 0xFF
} OcarinaNoteIdx;

typedef s32 (*DmaHandler)(OSPiHandle* handle, OSIoMesg* mb, s32 direction);

struct Note;
struct NotePool;
struct SequenceChannel;
struct SequenceLayer;

// A node in a circularly linked list. Each node is either a head or an item:
// - Items can be either detached (prev = NULL), or attached to a list.
//   'value' points to something of interest.
// - List heads are always attached; if a list is empty, its head points
//   to itself. 'count' contains the size of the list.
// If the list holds notes, 'pool' points back to the pool where it lives.
// Otherwise, that member is NULL.
typedef struct AudioListItem {
    /* 0x00 */ struct AudioListItem* prev;
    /* 0x04 */ struct AudioListItem* next;
    union {
        /* 0x08 */ void* value; // either Note* or SequenceLayer*
        /* 0x08 */ s32 count;
               } u;
    /* 0x0C */ struct NotePool* pool;
} AudioListItem; // size = 0x10

typedef struct NotePool {
    /* 0x00 */ AudioListItem disabled;
    /* 0x10 */ AudioListItem decaying;
    /* 0x20 */ AudioListItem releasing;
    /* 0x30 */ AudioListItem active;
} NotePool; // size = 0x40

// Pitch sliding by up to one octave in the positive direction. Negative
// direction is "supported" by setting extent to be negative. The code
// extrapolates exponentially in the wrong direction in that case, but that
// doesn't prevent seqplayer from doing it, AFAICT.
typedef struct {
    /* 0x0 */ u8 mode; // bit 0x80 denotes something; the rest are an index 0-5
    /* 0x2 */ u16 cur;
    /* 0x4 */ u16 speed;
    /* 0x8 */ f32 extent;
} Portamento; // size = 0xC

typedef struct {
    /* 0x0 */ s16 delay;
    /* 0x2 */ s16 arg;
} AdsrEnvelope; // size = 0x4

typedef struct {
    /* 0x00 */ u32 start;
    /* 0x04 */ u32 end;
    /* 0x08 */ u32 count;
    /* 0x0C */ u32 unk_0C;
    /* 0x10 */ s16 state[16]; // only exists if count != 0. 8-byte aligned
} AdpcmLoop; // size = 0x30 (or 0x10)

typedef struct {
    /* 0x00 */ s32 order;
    /* 0x04 */ s32 npredictors;
    /* 0x08 */ s16 book[1]; // size 8 * order * npredictors. 8-byte aligned
} AdpcmBook; // size >= 0x8

typedef struct {
    /* 0x00 */ u32 unk_0 : 1;
    /* 0x00 */ u32 codec : 3;
    /* 0x00 */ u32 medium : 2;
    /* 0x00 */ u32 unk_bit26 : 1;
    /* 0x00 */ u32 unk_bit25 : 1;
    /* 0x01 */ u32 size : 24;
    /* 0x04 */ u8* sampleAddr;
    /* 0x08 */ AdpcmLoop* loop;
    /* 0x0C */ AdpcmBook* book;
} SoundFontSample; // size = 0x10

typedef struct {
    /* 0x00 */ SoundFontSample* sample;
    /* 0x04 */ f32 tuning; // frequency scale factor
} SoundFontSound; // size = 0x8

typedef struct {
    /* 0x00 */ s16 numSamplesAfterDownsampling; // never read
    /* 0x02 */ s16 chunkLen; // never read
    /* 0x04 */ s16* toDownsampleLeft;
    /* 0x08 */ s16* toDownsampleRight; // data pointed to by left and right are adjacent in memory
    /* 0x0C */ s32 startPos; // start pos in ring buffer
    /* 0x10 */ s16 lengthA; // first length in ring buffer (from startPos, at most until end)
    /* 0x12 */ s16 lengthB; // second length in ring buffer (from pos 0)
    /* 0x14 */ u16 unk_14;
    /* 0x16 */ u16 unk_16;
    /* 0x18 */ u16 unk_18;
} ReverbRingBufferItem; // size = 0x1C

typedef struct {
    /* 0x000 */ u8 resampleFlags;
    /* 0x001 */ u8 useReverb;
    /* 0x002 */ u8 framesToIgnore;
    /* 0x003 */ u8 curFrame;
    /* 0x004 */ u8 downsampleRate;
    /* 0x005 */ s8 unk_05;
    /* 0x006 */ u16 windowSize;
    /* 0x008 */ s16 unk_08;
    /* 0x00A */ s16 unk_0A;
    /* 0x00C */ u16 unk_0C;
    /* 0x00E */ u16 unk_0E;
    /* 0x010 */ s16 leakRtl;
    /* 0x012 */ s16 leakLtr;
    /* 0x014 */ u16 unk_14;
    /* 0x016 */ s16 unk_16;
    /* 0x018 */ u8 unk_18;
    /* 0x019 */ s8 unk_19;
    /* 0x01A */ u16 unk_1A;
    /* 0x01C */ u16 unk_1C;
    /* 0x01E */ u8 unk_1E;
    /* 0x020 */ s32 nextRingBufPos;
    /* 0x024 */ s32 unk_24; // May be bufSizePerChan
    /* 0x028 */ s16* leftRingBuf;
    /* 0x02C */ s16* rightRingBuf;
    /* 0x030 */ void* unk_30;
    /* 0x034 */ void* unk_34;
    /* 0x038 */ void* unk_38;
    /* 0x03C */ void* unk_3C;
    /* 0x040 */ ReverbRingBufferItem items[2][5];
    /* 0x158 */ ReverbRingBufferItem items2[2][5];
    /* 0x270 */ s16* filterLeft;
    /* 0x274 */ s16* filterRight;
    /* 0x278 */ s16* unk_278;
    /* 0x27C */ s16* unk_27C;
    /* 0x280 */ s16* filterLeftState;
    /* 0x284 */ s16* filterRightState;
    /* 0x288 */ SoundFontSound sound;
    /* 0x290 */ SoundFontSample sample;
    /* 0x2A0 */ AdpcmLoop loop;
} SynthesisReverb; // size = 0x2D0

typedef struct {
    /* 0x00 */ u8 loaded;
    /* 0x01 */ u8 normalRangeLo;
    /* 0x02 */ u8 normalRangeHi;
    /* 0x03 */ u8 releaseRate;
    /* 0x04 */ AdsrEnvelope* envelope;
    /* 0x08 */ SoundFontSound lowNotesSound;
    /* 0x10 */ SoundFontSound normalNotesSound;
    /* 0x18 */ SoundFontSound highNotesSound;
} Instrument; // size = 0x20

typedef struct {
    /* 0x00 */ u8 releaseRate;
    /* 0x01 */ u8 pan;
    /* 0x02 */ u8 loaded;
    /* 0x04 */ SoundFontSound sound;
    /* 0x14 */ AdsrEnvelope* envelope;
} Drum; // size = 0x18

typedef struct {
    /* 0x00 */ u8 numInstruments;
    /* 0x01 */ u8 numDrums;
    /* 0x02 */ u8 sampleBankId1;
    /* 0x03 */ u8 sampleBankId2;
    /* 0x04 */ u16 numSfx;
    /* 0x08 */ Instrument** instruments;
    /* 0x0C */ Drum** drums;
    /* 0x10 */ SoundFontSound* soundEffects;
} SoundFont; // size = 0x14

typedef struct {
    /* 0x00 */ u8* pc;
    /* 0x04 */ u8* stack[4];
    /* 0x14 */ u8 remLoopIters[4];
    /* 0x18 */ u8 depth;
    /* 0x19 */ s8 value;
} SeqScriptState; // size = 0x1C

// Also known as a Group, according to debug strings.
typedef struct {
    /* 0x000 */ u8 enabled : 1;
    /* 0x000 */ u8 finished : 1;
    /* 0x000 */ u8 muted : 1;
    /* 0x000 */ u8 seqDmaInProgress : 1;
    /* 0x000 */ u8 fontDmaInProgress : 1;
    /* 0x000 */ u8 recalculateVolume : 1;
    /* 0x000 */ u8 stopScript : 1;
    /* 0x000 */ u8 applyBend : 1;
    /* 0x001 */ u8 state;
    /* 0x002 */ u8 noteAllocPolicy;
    /* 0x003 */ u8 muteBehavior;
    /* 0x004 */ u8 seqId;
    /* 0x005 */ u8 defaultFont;
    /* 0x006 */ u8 unk_06[1];
    /* 0x007 */ s8 playerIdx;
    /* 0x008 */ u16 tempo; // tatums per minute
    /* 0x00A */ u16 tempoAcc;
    /* 0x00C */ s16 unk_0C;
    /* 0x00E */ s16 transposition;
    /* 0x010 */ u16 delay;
    /* 0x012 */ u16 fadeTimer;
    /* 0x014 */ u16 fadeTimerUnkEu;
    /* 0x016 */ u16 unk_16; // New to MM
    union {
        /* 0x018 */ u8* seqData;
        /* 0x018 */ u16* seqData16;
    };
    /* 0x01C */ f32 fadeVolume;
    /* 0x020 */ f32 fadeVelocity;
    /* 0x024 */ f32 volume;
    /* 0x028 */ f32 muteVolumeScale;
    /* 0x02C */ f32 fadeVolumeScale;
    /* 0x030 */ f32 appliedFadeVolume;
    /* 0x034 */ f32 bend;
    /* 0x038 */ struct SequenceChannel* channels[16];
    /* 0x078 */ SeqScriptState scriptState;
    /* 0x094 */ u8* shortNoteVelocityTable;
    /* 0x098 */ u8* shortNoteGateTimeTable;
    /* 0x09C */ NotePool notePool;
    /* 0x0DC */ s32 skipTicks;
    /* 0x0E0 */ u32 scriptCounter;
    /* 0x0E4 */ char unk_E4[0x74]; // unused struct members for sequence/sound font dma management, according to sm64 decomp
    /* 0x158 */ s8 soundScriptIO[8];
} SequencePlayer; // size = 0x160

typedef struct {
    /* 0x0 */ u8 releaseRate;
    /* 0x1 */ u8 sustain;
    /* 0x4 */ AdsrEnvelope* envelope;
} AdsrSettings; // size = 0x8

typedef struct {
    union {
        struct {
            /* 0x00 */ u8 unk_0b80 : 1;
            /* 0x00 */ u8 hang : 1;
            /* 0x00 */ u8 decay : 1;
            /* 0x00 */ u8 release : 1;
            /* 0x00 */ u8 state : 4;
        } s;
        /* 0x00 */ u8 asByte;
    } action;
    /* 0x01 */ u8 envIndex;
    /* 0x02 */ s16 delay;
    /* 0x04 */ f32 sustain;
    /* 0x08 */ f32 velocity;
    /* 0x0C */ f32 fadeOutVel;
    /* 0x10 */ f32 current;
    /* 0x14 */ f32 target;
    /* 0x18 */ char unk_18[4];
    /* 0x1C */ AdsrEnvelope* envelope;
} AdsrState; // size = 0x20

typedef struct {
    /* 0x0 */ u8 unused : 2;
    /* 0x0 */ u8 bit2 : 2;
    /* 0x0 */ u8 strongRight : 1;
    /* 0x0 */ u8 strongLeft : 1;
    /* 0x0 */ u8 stereoHeadsetEffects : 1;
    /* 0x0 */ u8 usesHeadsetPanEffects : 1;
} StereoData; // size = 0x1

typedef union {
    /* 0x0 */ StereoData s;
    /* 0x0 */ u8 asByte;
} Stereo; // size = 0x1

typedef struct {
    /* 0x00 */ u8 reverb;
    /* 0x01 */ u8 unk_1;
    /* 0x02 */ u8 pan;
    /* 0x03 */ u8 unk_3; // Possibly part of stereo?
    /* 0x04 */ Stereo stereo;
    /* 0x05 */ u8 unk_4;
    /* 0x06 */ u16 unk_6;
    /* 0x08 */ f32 freqScale;
    /* 0x0C */ f32 velocity;
    /* 0x10 */ s16* filter;
    /* 0x14 */ s16* filterBuf;
} NoteAttributes; // size = 0x18

typedef struct VibratoSubStruct {
    /* 0x0 */ u16 vibratoRateStart;
    /* 0x2 */ u16 vibratoExtentStart;
    /* 0x4 */ u16 vibratoRateTarget;
    /* 0x6 */ u16 vibratoExtentTarget;
    /* 0x8 */ u16 vibratoRateChangeDelay;
    /* 0xA */ u16 vibratoExtentChangeDelay;
    /* 0xC */ u16 vibratoDelay;
} VibratoSubStruct; // size = 0xE

// Also known as a SubTrack, according to sm64 debug strings.
typedef struct SequenceChannel {
    /* 0x00 */ u8 enabled : 1;
    /* 0x00 */ u8 finished : 1;
    /* 0x00 */ u8 stopScript : 1;
    /* 0x00 */ u8 stopSomething2 : 1; // sets SequenceLayer.stopSomething
    /* 0x00 */ u8 hasInstrument : 1;
    /* 0x00 */ u8 stereoHeadsetEffects : 1;
    /* 0x00 */ u8 largeNotes : 1; // notes specify duration and velocity
    /* 0x00 */ u8 unused : 1;
    union {
        struct {
            /* 0x01 */ u8 freqScale : 1;
            /* 0x01 */ u8 volume : 1;
            /* 0x01 */ u8 pan : 1;
        } s;
        /* 0x01 */ u8 asByte;
    } changes;
    /* 0x02 */ u8 noteAllocPolicy;
    /* 0x03 */ u8 muteBehavior;
    /* 0x04 */ u8 reverb;       // or dry/wet mix
    /* 0x05 */ u8 notePriority; // 0-3
    /* 0x06 */ u8 someOtherPriority;
    /* 0x07 */ u8 fontId;
    /* 0x08 */ u8 reverbIndex;
    /* 0x09 */ u8 bookOffset;
    /* 0x0A */ u8 newPan;
    /* 0x0B */ u8 panChannelWeight;  // proportion of pan that comes from the channel (0..128)
    /* 0x0C */ u8 unk_0C;
    /* 0x0D */ u8 velocityRandomVariance;
    /* 0x0E */ u8 gateTimeRandomVariance;
    /* 0x0F */ u8 unk_0F;
    /* 0x10 */ u8 unk_10; // New to MM
    /* 0x11 */ u8 unk_11; // New to MM
    /* 0x12 */ VibratoSubStruct vibrato;
    /* 0x20 */ u16 delay;
    /* 0x22 */ u16 unk_20;
    /* 0x24 */ u16 unk_22;
    /* 0x26 */ s16 instOrWave; // either 0 (none), instrument index + 1, or
                             // 0x80..0x83 for sawtooth/triangle/sine/square waves.
    /* 0x28 */ s16 transposition;
    /* 0x2C */ f32 volumeScale;
    /* 0x30 */ f32 volume;
    /* 0x34 */ s32 pan;
    /* 0x38 */ f32 appliedVolume;
    /* 0x3C */ f32 freqScale;
    /* 0x40 */ u8 (*dynTable)[][2];
    /* 0x44 */ struct Note* noteUnused;
    /* 0x48 */ struct SequenceLayer* layerUnused;
    /* 0x4C */ Instrument* instrument;
    /* 0x50 */ SequencePlayer* seqPlayer;
    /* 0x54 */ struct SequenceLayer* layers[4];
    /* 0x64 */ SeqScriptState scriptState;
    /* 0x80 */ AdsrSettings adsr;
    /* 0x88 */ NotePool notePool;
    /* 0xC8 */ s8 soundScriptIO[8]; // bridge between sound script and audio lib, "io ports"
    /* 0xD0 */ u8* unk_D0; // New to MM
    /* 0xD4 */ s16* filter;
    /* 0xD8 */ Stereo stereo;
    /* 0xDC */ s32 unk_DC; // New to MM
    /* 0xE0 */ s32 unk_E0; // New to MM
} SequenceChannel; // size = 0xE4

// Might also be known as a Track, according to sm64 debug strings (?).
typedef struct SequenceLayer {
    /* 0x00 */ u8 enabled : 1;
    /* 0x00 */ u8 finished : 1;
    /* 0x00 */ u8 stopSomething : 1;
    /* 0x00 */ u8 continuousNotes : 1; // keep the same note for consecutive notes with the same sound
    /* 0x00 */ u8 bit3 : 1; // "loaded"?
    /* 0x00 */ u8 ignoreDrumPan : 1;
    /* 0x00 */ u8 bit1 : 1; // "has initialized continuous notes"?
    /* 0x00 */ u8 notePropertiesNeedInit : 1;
    /* 0x01 */ Stereo stereo;
    /* 0x02 */ u8 instOrWave;
    /* 0x03 */ u8 gateTime;
    /* 0x04 */ u8 semitone;
    /* 0x05 */ u8 portamentoTargetNote;
    /* 0x06 */ u8 pan; // 0..128
    /* 0x07 */ u8 notePan;
    /* 0x08 */ u8 unk_08;
    /* 0x09 */ u8 unk_09;
    union {
        struct {
            /* 0x0A */ u16 bit_0 : 1;
            /* 0x0A */ u16 bit_1 : 1;
            /* 0x0A */ u16 bit_2 : 1;
            /* 0x0A */ u16 bit_3 : 1;
            /* 0x0A */ u16 bit_4 : 1;
            /* 0x0A */ u16 bit_5 : 1;
            /* 0x0A */ u16 bit_6 : 1;
            /* 0x0A */ u16 bit_7 : 1;
            /* 0x0A */ u16 bit_8 : 1;
            /* 0x0A */ u16 bit_9 : 1;
            /* 0x0A */ u16 bit_A : 1;
            /* 0x0A */ u16 bit_B : 1;
            /* 0x0A */ u16 bit_C : 1;
            /* 0x0A */ u16 bit_D : 1;
            /* 0x0A */ u16 bit_E : 1;
            /* 0x0A */ u16 bit_F : 1;
        } s;
        /* 0x0A */ u16 asByte;
    } unk_0A;
    /* 0x0C */ VibratoSubStruct vibrato;
    /* 0x1A */ s16 delay;
    /* 0x1C */ s16 gateDelay;
    /* 0x1E */ s16 delay2;
    /* 0x20 */ u16 portamentoTime;
    /* 0x22 */ s16 transposition; // #semitones added to play commands
                                  // (seq instruction encoding only allows referring to the limited range
                                  // 0..0x3F; this makes 0x40..0x7F accessible as well)
    /* 0x24 */ s16 shortNoteDefaultDelay;
    /* 0x26 */ s16 lastDelay;
    /* 0x28 */ AdsrSettings adsr;
    /* 0x30 */ Portamento portamento;
    /* 0x3C */ struct Note* note;
    /* 0x40 */ f32 freqScale;
    /* 0x44 */ f32 bend;
    /* 0x48 */ f32 velocitySquare2;
    /* 0x4C */ f32 velocitySquare; // not sure which one of those corresponds to the sm64 original
    /* 0x50 */ f32 noteVelocity;
    /* 0x54 */ f32 noteFreqScale;
    /* 0x58 */ Instrument* instrument;
    /* 0x5C */ SoundFontSound* sound;
    /* 0x60 */ SequenceChannel* channel; // Not SequenceChannel?
    /* 0x64 */ SeqScriptState scriptState;
    /* 0x80 */ AudioListItem listItem;
} SequenceLayer; // size = 0x90

typedef struct {
    /* 0x00 */ s16 adpcmdecState[0x10];
    /* 0x20 */ s16 finalResampleState[0x10];
    /* 0x40 */ s16 mixEnvelopeState[0x28];
    /* 0x90 */ s16 panResampleState[0x10];
    /* 0xB0 */ s16 panSamplesBuffer[0x20];
    /* 0xF0 */ s16 dummyResampleState[0x10];
} NoteSynthesisBuffers; // size = 0x110

typedef struct {
    /* 0x00 */ u8 restart_bit0 : 1;
    /* 0x00 */ u8 restart_bit1 : 1;
    /* 0x00 */ u8 restart_bit2 : 1;
    /* 0x00 */ u8 restart_bit3 : 1;
    /* 0x00 */ u8 restart_bit4 : 1;
    /* 0x00 */ u8 restart_bit5 : 1;
    /* 0x00 */ u8 restart_bit6 : 1;
    /* 0x00 */ u8 restart_bit7 : 1;
    /* 0x01 */ u8 sampleDmaIndex;
    /* 0x02 */ u8 prevHeadsetPanRight;
    /* 0x03 */ u8 prevHeadsetPanLeft;
    /* 0x04 */ u8 reverbVol;
    /* 0x05 */ u8 numParts;
    /* 0x06 */ u16 samplePosFrac;
    /* 0x08 */ u16 unk_08; // New to MM
    /* 0x0C */ s32 samplePosInt;
    /* 0x10 */ NoteSynthesisBuffers* synthesisBuffers;
    /* 0x14 */ s16 curVolLeft;
    /* 0x16 */ s16 curVolRight;
    /* 0x18 */ u16 unk_14;
    /* 0x1A */ u16 unk_16;
    /* 0x1C */ u16 unk_18;
    /* 0x1E */ u8 unk_1A;
    /* 0x1F */ u8 unk_1F;
    /* 0x20 */ u16 unk_1C;
    /* 0x22 */ u16 unk_1E;
} NoteSynthesisState; // size = 0x24

typedef struct {
    /* 0x00 */ struct VibratoSubStruct* vibSubStruct; // MM Something else?
    /* 0x04 */ u32 time;
    /* 0x08 */ s16* curve;
    /* 0x0C */ f32 extent;
    /* 0x10 */ f32 rate;
    /* 0x14 */ u8 active;
    /* 0x16 */ u16 rateChangeTimer;
    /* 0x18 */ u16 extentChangeTimer;
    /* 0x1A */ u16 delay;
} VibratoState; // size = 0x1C

typedef struct {
    /* 0x00 */ u8 priority;
    /* 0x01 */ u8 waveId;
    /* 0x02 */ u8 sampleCountIndex;
    /* 0x03 */ u8 fontId;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 stereoHeadsetEffects;
    /* 0x06 */ s16 adsrVolScaleUnused;
    /* 0x08 */ f32 portamentoFreqScale;
    /* 0x0C */ f32 vibratoFreqScale;
    /* 0x18 */ SequenceLayer* wantedParentLayer;
    /* 0x14 */ SequenceLayer* parentLayer;
    /* 0x10 */ SequenceLayer* prevParentLayer;
    /* 0x1C */ NoteAttributes attributes;
    /* 0x34 */ AdsrState adsr;
    /* 0x54 */ Portamento portamento;
    /* 0x60 */ VibratoState vibratoState;
    /* 0x7C */ char unk_7C[0x8];
    /* 0x84 */ u32 unk_84;
} NotePlaybackState; // size = 0x88

typedef struct {
    struct {
        /* 0x00 */ volatile u8 enabled : 1;
        /* 0x00 */ u8 needsInit : 1;
        /* 0x00 */ u8 finished : 1; // ?
        /* 0x00 */ u8 unused : 1;
        /* 0x00 */ u8 stereoStrongRight : 1;
        /* 0x00 */ u8 stereoStrongLeft : 1;
        /* 0x00 */ u8 stereoHeadsetEffects : 1;
        /* 0x00 */ u8 usesHeadsetPanEffects : 1; // ?
    } bitField0;
    struct {
        /* 0x01 */ u8 reverbIndex : 3;
        /* 0x01 */ u8 bookOffset : 2;
        /* 0x01 */ u8 isSyntheticWave : 1;
        /* 0x01 */ u8 hasTwoParts : 1;
        /* 0x01 */ u8 usesHeadsetPanEffects2 : 1;
    } bitField1;
    /* 0x02 */ u8 unk_2;
    /* 0x03 */ u8 headsetPanRight;
    /* 0x04 */ u8 headsetPanLeft;
    /* 0x05 */ u8 reverbVol;
    /* 0x06 */ u8 unk_06;
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ u16 targetVolLeft;
    /* 0x0A */ u16 targetVolRight;
    /* 0x0C */ u16 resamplingRateFixedPoint;
    /* 0x0E */ u16 unk_0E;
    union {
        /* 0x10 */ SoundFontSound* soundFontSound;
        /* 0x10 */ s16* samples; // used for synthetic waves
            } sound;
    /* 0x14 */ s16* filter;
    /* 0x18 */ u8 unk_18; // New to MM
    /* 0x19 */ u8 unk_19; // New to MM
    /* 0x1A */ UNK_TYPE1 pad_1A[0x6];
} NoteSubEu; // size = 0x20

typedef struct Note {
    /* 0x00 */ AudioListItem listItem;
    /* 0x10 */ NoteSynthesisState synthesisState;
    /* 0x34 */ NotePlaybackState playbackState;
    /* 0xBC */ char unk_BC[0x1C]; 
    /* 0xD8 */ NoteSubEu noteSubEu;
} Note; // size = 0xF8

typedef struct {
    /* 0x00 */ u8 downsampleRate;
    /* 0x02 */ u16 windowSize;
    /* 0x04 */ u16 unk_4;
    /* 0x06 */ u16 unk_6;
    /* 0x08 */ u16 unk_8;
    /* 0x0A */ u16 unk_A;
    /* 0x0C */ u16 leakRtl;
    /* 0x0E */ u16 leakLtr;
    /* 0x10 */ s8 unk_10;
    /* 0x12 */ u16 unk_12;
    /* 0x14 */ s16 lowPassFilterCutoffLeft;
    /* 0x16 */ s16 lowPassFilterCutoffRight;
} ReverbSettings; // size = 0x18

typedef struct {
    /* 0x00 */ u32 frequency;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 numNotes;
    /* 0x06 */ u8 numSequencePlayers;
    /* 0x07 */ u8 unk_07[0x2]; // unused, set to zero
    /* 0x09 */ u8 numReverbs;
    /* 0x0A */ u8 unk_0A[0x2];
    /* 0x0C */ ReverbSettings* reverbSettings;
    /* 0x10 */ u16 sampleDmaBufSize1;
    /* 0x12 */ u16 sampleDmaBufSize2;
    /* 0x14 */ u16 unk_14;
    /* 0x18 */ u32 persistentSeqMem;
    /* 0x1C */ u32 persistentFontMem;
    /* 0x20 */ u32 persistentSampleMem;
    /* 0x24 */ u32 temporarySeqMem;
    /* 0x28 */ u32 temporaryFontMem;
    /* 0x2C */ u32 temporarySampleMem;
    /* 0x30 */ s32 persistentSampleCacheMem;
    /* 0x34 */ s32 temporarySampleCacheMem;
} AudioSpec; // size = 0x38

typedef struct {
    /* 0x00 */ s16 specUnk4;
    /* 0x02 */ u16 frequency;
    /* 0x04 */ u16 aiFrequency;
    /* 0x06 */ s16 samplesPerFrameTarget;
    /* 0x08 */ s16 maxAiBufferLength;
    /* 0x0A */ s16 minAiBufferLength;
    /* 0x0C */ s16 updatesPerFrame;
    /* 0x0E */ s16 samplesPerUpdate;
    /* 0x10 */ s16 samplesPerUpdateMax;
    /* 0x12 */ s16 samplesPerUpdateMin;
    /* 0x14 */ s16 numSequencePlayers;
    /* 0x18 */ f32 resampleRate;
    /* 0x1C */ f32 updatesPerFrameInv;
    /* 0x20 */ f32 unkUpdatesPerFrameScaled;
    /* 0x24 */ f32 unk_24;
} AudioBufferParameters; // size = 0x28

typedef struct {
    /* 0x0 */ u8* start;
    /* 0x4 */ u8* cur;
    /* 0x8 */ s32 size;
    /* 0xC */ s32 count;
} AudioAllocPool; // size = 0x10

typedef struct {
    /* 0x0 */ u8* ptr;
    /* 0x4 */ u32 size;
    /* 0x8 */ s16 tableType;
    /* 0xA */ s16 id;
} AudioCacheEntry; // size = 0xC

typedef struct {
    /* 0x00 */ s8 inUse;
    /* 0x01 */ s8 origMedium;
    /* 0x02 */ u8 sampleBankId;
    /* 0x03 */ char unk_03[0x5];
    /* 0x08 */ u8* allocatedAddr;
    /* 0x0C */ void* sampleAddr;
    /* 0x10 */ u32 size;
} SampleCacheEntry; // size = 0x14

typedef struct {
    /* 0x000 */ AudioAllocPool pool;
    /* 0x010 */ SampleCacheEntry entries[128];
    /* 0xA10 */ s32 size;
} AudioSampleCache; // size = 0xA14

typedef struct {
    /* 0x00 */ u32 numEntries;
    /* 0x04 */ AudioAllocPool pool;
    /* 0x14 */ AudioCacheEntry entries[16];
} AudioPersistentCache; // size = 0xD4

typedef struct {
    /* 0x00 */ u32 nextSide;
    /* 0x04 */ AudioAllocPool pool;
    /* 0x14 */ AudioCacheEntry entries[2];
} AudioTemporaryCache; // size = 0x3C

typedef struct {
    /* 0x000 */ AudioPersistentCache persistent;
    /* 0x0D4 */ AudioTemporaryCache temporary;
    /* 0x100 */ u8 unk_100[0x10];
} AudioCache; // size = 0x110

typedef struct {
    /* 0x0 */ u32 wantPersistent;
    /* 0x4 */ u32 wantTemporary;
} AudioPoolSplit2; // size = 0x8

typedef struct {
    /* 0x0 */ u32 wantSeq;
    /* 0x4 */ u32 wantFont;
    /* 0x8 */ u32 wantSample;
} AudioPoolSplit3; // size = 0xC

typedef struct {
    /* 0x0 */ u32 wantSeq;
    /* 0x4 */ u32 wantFont;
    /* 0x8 */ u32 wantSample;
    /* 0xC */ u32 wantCustom;
} AudioPoolSplit4; // size = 0x10

typedef struct {
    /* 0x00 */ u32 endAndMediumKey;
    /* 0x04 */ SoundFontSample* sample;
    /* 0x08 */ u8* ramAddr;
    /* 0x0C */ u32 encodedInfo;
    /* 0x10 */ s32 isFree;
} AudioPreloadReq; // size = 0x14

typedef struct {
    union {
    /* 0x0 */ u32 opArgs;
        struct {
            /* 0x0 */ u8 op;
            /* 0x1 */ u8 arg0;
            /* 0x2 */ u8 arg1;
            /* 0x3 */ u8 arg2;
        };
    };
    union {
        /* 0x4 */ void* data;
        /* 0x4 */ f32 asFloat;
        /* 0x4 */ s32 asInt;
        /* 0x4 */ u16 asUShort;
        /* 0x4 */ s8 asSbyte;
        /* 0x4 */ u8 asUbyte;
        /* 0x4 */ u32 asUInt;
    };
} AudioCmd; // size = 0x8

typedef struct {
    /* 0x00 */ s8 status;
    /* 0x01 */ s8 delay;
    /* 0x02 */ s8 medium;
    /* 0x04 */ u8* ramAddr;
    /* 0x08 */ u32 curDevAddr;
    /* 0x0C */ u8* curRamAddr;
    /* 0x10 */ u32 bytesRemaining;
    /* 0x14 */ u32 chunkSize;
    /* 0x18 */ s32 unkMediumParam;
    /* 0x1C */ u32 retMsg;
    /* 0x20 */ OSMesgQueue* retQueue;
    /* 0x24 */ OSMesgQueue msgQueue;
    /* 0x3C */ OSMesg msg;
    /* 0x40 */ OSIoMesg ioMesg;
} AudioAsyncLoad; // size = 0x58

typedef struct {
    /* 0x00 */ u8 medium;
    /* 0x01 */ u8 seqOrFontId;
    /* 0x02 */ u16 instId;
    /* 0x04 */ s32 unkMediumParam;
    /* 0x08 */ s32 curDevAddr;
    /* 0x0C */ u8* curRamAddr;
    /* 0x10 */ u8* ramAddr;
    /* 0x14 */ s32 status;
    /* 0x18 */ s32 bytesRemaining;
    /* 0x1C */ s8* isDone;
    /* 0x20 */ SoundFontSample sample;
    /* 0x30 */ OSMesgQueue msgqueue;
    /* 0x48 */ OSMesg msg;
    /* 0x4C */ OSIoMesg ioMesg;
} AudioSlowLoad; // size = 0x64

typedef struct {
    /* 0x00 */ u32 romAddr;
    /* 0x04 */ u32 size;
    /* 0x08 */ s8 medium;
    /* 0x09 */ s8 cachePolicy;
    /* 0x0A */ s16 shortData1;
    /* 0x0C */ s16 shortData2;
    /* 0x0E */ s16 shortData3;
} AudioTableEntry; // size = 0x10

typedef struct {
    /* 0x00 */ s16 numEntries;
    /* 0x02 */ s16 unkMediumParam;
    /* 0x04 */ u32 romAddr;
    /* 0x08 */ char pad[0x8];
    /* 0x10 */ AudioTableEntry entries[1]; // (dynamic size)
} AudioTable; // size >= 0x20

typedef struct {
    /* 0x00 */ OSTask task;
    /* 0x40 */ OSMesgQueue* taskQueue;
    /* 0x44 */ void* unk_44; // probably a message that gets unused.
    /* 0x48 */ char unk_48[0x8];
} AudioTask; // size = 0x50

typedef struct {
    /* 0x00 */ u8* ramAddr;
    /* 0x04 */ u32 devAddr;
    /* 0x08 */ u16 sizeUnused;
    /* 0x0A */ u16 size;
    /* 0x0C */ u8 unused;
    /* 0x0D */ u8 reuseIndex; // position in sSampleDmaReuseQueue1/2, if ttl == 0
    /* 0x0E */ u8 ttl;        // duration after which the DMA can be discarded
} SampleDma; // size = 0x10

typedef struct {
    /* 0x0000 */ char unk_0000;
    /* 0x0001 */ s8 numSynthesisReverbs;
    /* 0x0002 */ u16 unk_2;
    /* 0x0004 */ u16 unk_4;
    /* 0x0006 */ char unk_0006[0xA];
    /* 0x0010 */ s16* curLoadedBook;
    /* 0x0014 */ NoteSubEu* noteSubsEu;
    /* 0x0018 */ SynthesisReverb synthesisReverbs[4];
    /* 0x0B58 */ char unk_0B58[0x30];
    /* 0x0B88 */ SoundFontSample* usedSamples[128];
    /* 0x0D88 */ AudioPreloadReq preloadSampleStack[128];
    /* 0x1788 */ s32 numUsedSamples;
    /* 0x178C */ s32 preloadSampleStackTop;
    /* 0x1790 */ AudioAsyncLoad asyncLoads[0x10];
    /* 0x1D10 */ OSMesgQueue asyncLoadUnkMediumQueue;
    /* 0x1D28 */ char unk_1D08[0x40];
    /* 0x1D68 */ AudioAsyncLoad* curUnkMediumLoad;
    /* 0x1D6C */ u32 slowLoadPos;
    /* 0x1D70 */ AudioSlowLoad slowLoads[2];
    /* 0x1E38 */ OSPiHandle* cartHandle;
    /* 0x1E2C */ OSPiHandle* driveHandle;
    /* 0x1E40 */ OSMesgQueue externalLoadQueue;
    /* 0x1E58 */ OSMesg externalLoadMesgBuf[0x10];
    /* 0x1E98 */ OSMesgQueue preloadSampleQueue;
    /* 0x1EB0 */ OSMesg preloadSampleMesgBuf[0x10];
    /* 0x1EF0 */ OSMesgQueue currAudioFrameDmaQueue;
    /* 0x1F08 */ OSMesg currAudioFrameDmaMesgBuf[0x40];
    /* 0x2008 */ OSIoMesg currAudioFrameDmaIoMesgBuf[0x40];
    /* 0x2608 */ OSMesgQueue syncDmaQueue;
    /* 0x2620 */ OSMesg syncDmaMesg;
    /* 0x2624 */ OSIoMesg syncDmaIoMesg;
    /* 0x263C */ SampleDma* sampleDmas;
    /* 0x2640 */ u32 sampleDmaCount;
    /* 0x2644 */ u32 sampleDmaListSize1;
    /* 0x2648 */ s32 unused2628;
    /* 0x264C */ u8 sampleDmaReuseQueue1[0x100]; // read pos <= write pos, wrapping mod 256
    /* 0x274C */ u8 sampleDmaReuseQueue2[0x100];
    /* 0x284C */ u8 sampleDmaReuseQueue1RdPos;
    /* 0x284D */ u8 sampleDmaReuseQueue2RdPos;
    /* 0x284E */ u8 sampleDmaReuseQueue1WrPos;
    /* 0x284F */ u8 sampleDmaReuseQueue2WrPos;
    /* 0x2850 */ AudioTable* sequenceTable;
    /* 0x2854 */ AudioTable* soundFontTable;
    /* 0x2858 */ AudioTable* sampleBankTable;
    /* 0x285C */ char unk_285C[0x4];
    /* 0x2860 */ u8* sequenceFontTable;
    /* 0x2864 */ u16 numSequences;
    /* 0x2868 */ SoundFont* soundFonts;
    /* 0x286C */ AudioBufferParameters audioBufferParameters;
    /* 0x2994 */ f32 unk_2870;
    /* 0x2898 */ s32 sampleDmaBufSize1;
    /* 0x289C */ s32 sampleDmaBufSize2;
    /* 0x28A0 */ char unk_287C[0x10];
    /* 0x28B0 */ s32 sampleDmaBufSize;
    /* 0x28B4 */ s32 maxAudioCmds;
    /* 0x28B8 */ s32 numNotes;
    /* 0x2898 */ s16 tempoInternalToExternal;
    /* 0x28BE */ s8 soundMode;
    /* 0x28C0 */ s32 totalTaskCnt;
    /* 0x28C4 */ s32 curAudioFrameDmaCount;
    /* 0x28C8 */ s32 rspTaskIdx;
    /* 0x28CC */ s32 curAIBufIdx;
    /* 0x28AC */ Acmd* abiCmdBufs[2];
    /* 0x28B4 */ Acmd* curAbiCmdBuf;
    /* 0x28DC */ AudioTask* currTask;
    /* 0x28C0 */ AudioTask rspTask[2];
    /* 0x2980 */ f32 unk_2960;
    /* 0x2984*/ s32 refreshRate;
    /* 0x2988 */ s16* aiBuffers[3];
    /* 0x2994 */ s16 aiBufLengths[3];
    /* 0x299C */ u32 audioRandom;
    /* 0x29A0 */ s32 audioErrorFlags;
    /* 0x29A4 */ volatile u32 resetTimer;
    /* 0x29A8 */ u32 (*unk_29A8[4])(s8 value, SequenceChannel* channel);
    /* 0x29B8 */ s8 unk_29B8;
    /* 0x29BC */ s32 unk_29BC; // sMaxAbiCmdCnt
    /* 0x29C0 */ AudioAllocPool audioSessionPool;
    /* 0x29D0 */ AudioAllocPool externalPool;
    /* 0x29E0 */ AudioAllocPool audioInitPool;
    /* 0x29F0 */ AudioAllocPool notesAndBuffersPool;
    /* 0x2A00 */ char unk_29D0[0x20]; // probably two unused pools
    /* 0x2A20 */ AudioAllocPool cachePool;
    /* 0x2A30 */ AudioAllocPool persistentCommonPool;
    /* 0x2A40 */ AudioAllocPool temporaryCommonPool;
    /* 0x2A50 */ AudioCache seqCache;
    /* 0x2B60 */ AudioCache fontCache;
    /* 0x2C70 */ AudioCache sampleBankCache;
    /* 0x2D80 */ AudioAllocPool permanentPool;
    /* 0x2D90 */ AudioCacheEntry permanentCache[32];
    /* 0x3690 */ AudioSampleCache persistentSampleCache;
    /* 0x40A4 */ AudioSampleCache temporarySampleCache;
    /* 0x4338 */ AudioPoolSplit4 sessionPoolSplit;
    /* 0x4348 */ AudioPoolSplit2 cachePoolSplit;
    /* 0x4350 */ AudioPoolSplit3 persistentCommonPoolSplit;
    /* 0x435C */ AudioPoolSplit3 temporaryCommonPoolSplit;
    /* 0x4368 */ u8 sampleFontLoadStatus[0x30];
    /* 0x4398 */ u8 fontLoadStatus[0x30];
    /* 0x43C8 */ u8 seqLoadStatus[0x80];
    /* 0x4448 */ volatile u8 resetStatus;
    /* 0x4449 */ u8 audioResetSpecIdToLoad;
    /* 0x444C */ s32 audioResetFadeOutFramesLeft;
    /* 0x4450 */ f32* unk_3520;
    /* 0x4454 */ u8* audioHeap;
    /* 0x4458 */ u32 audioHeapSize;
    /* 0x445C */ Note* notes;
    /* 0x4460 */ SequencePlayer seqPlayers[5];
    /* 0x4B40 */ SequenceLayer sequenceLayers[80];
    /* 0x7840 */ SequenceChannel sequenceChannelNone;
    /* 0x7924 */ s32 noteSubEuOffset;
    /* 0x7928 */ AudioListItem layerFreeList;
    /* 0x7938 */ NotePool noteFreeLists;
    /* 0x7978 */ u8 cmdWrPos;
    /* 0x7979 */ u8 cmdRdPos;
    /* 0x797A */ u8 cmdQueueFinished;
    /* 0x797C */ u16 unk_5BDC[4];
    /* 0x7984 */ char unk_7984[4];
    /* 0x7988 */ OSMesgQueue* audioResetQueueP;
    /* 0x798C */ OSMesgQueue* taskStartQueueP;
    /* 0x7990 */ OSMesgQueue* cmdProcQueueP;
    /* 0x7994 */ OSMesgQueue taskStartQueue;
    /* 0x79AC */ OSMesgQueue cmdProcQueue;
    /* 0x79C4 */ OSMesgQueue audioResetQueue;
    /* 0x79DC */ OSMesg taskStartMsgs[1];
    /* 0x79E0 */ OSMesg audioResetMesgs[1];
    /* 0x79E4 */ OSMesg cmdProcMsgs[4];
    /* 0x79F4 */ AudioCmd cmdBuf[0x100];
    /* 0x81F4 */ char unk_81F4[4];
} AudioContext; // size = 0x81F8

typedef struct {
    /* 0x00 */ u8 reverbVol;
    /* 0x01 */ u8 unk_1;
    /* 0x02 */ u8 pan;
    /* 0x03 */ u8 unk_3;
    /* 0x04 */ Stereo stereo;
    /* 0x08 */ f32 frequency;
    /* 0x0C */ f32 velocity;
    /* 0x10 */ char unk_0C[0x4];
    /* 0x14 */ s16* filter;
    /* 0x18 */ u8 unk_14;
    /* 0x1A */ u16 unk_16;
} NoteSubAttributes; // size = 0x1A

typedef struct {
    /* 0x0 */ u32 heapSize;
    /* 0x4 */ u32 initPoolSize;
    /* 0x8 */ u32 permanentPoolSize;
} AudioContextInitSizes; // size = 0xC

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_10;
    /* 0x10 */ f32 unk_14;
    /* 0x14 */ f32 unk_18;
    /* 0x18 */ u16 unk_0C;
    /* 0x1A */ u16 unk_1C;
} unk_50_s; // size = 0x1C

typedef struct {
    /* 0x000 */ unk_50_s unk_50[0x10];
    /* 0x1C0 */ f32 volCur;
    /* 0x1C4 */ f32 volTarget;
    /* 0x1C8 */ f32 unk_08;
    /* 0x1CC */ u32 unk_14;
    /* 0x1D0 */ f32 unk_1C;
    /* 0x1D4 */ f32 unk_20;
    /* 0x1D8 */ f32 unk_24;
    /* 0x1DC */ u32 unk_2C[8];
    /* 0x1FC */ u32 unk_25C;
    /* 0x200 */ u16 unk_0C;
    /* 0x202 */ u16 unk_18;
    /* 0x204 */ u16 unk_28;
    /* 0x206 */ u16 unk_250;
    /* 0x208 */ u16 unk_252;
    /* 0x20A */ u16 unk_254;
    /* 0x20C */ u16 unk_256;
    /* 0x20E */ u16 unk_258;
    /* 0x210 */ u8 unk_260;
    /* 0x211 */ u8 unk_261[0x1];
    /* 0x212 */ u8 volScales[0x4];
    /* 0x216 */ u8 volFadeTimer;
    /* 0x217 */ u8 fadeVolUpdate;
    /* 0x218 */ u8 unk_4C;
    /* 0x219 */ u8 unk_4D;
    /* 0x21A */ u8 unk_4E;
    /* 0x21B */ u8 unk_21B; // New to MM
} unk_D_8016E750; // size = 0x21C

typedef struct {
    /* 0x0 */ u8 unk_0;
    /* 0x1 */ u8 unk_1; // importance?
} Struct_8016E320; // size = 0x02

typedef enum {
    /* 0 */ BANK_PLAYER,
    /* 1 */ BANK_ITEM,
    /* 2 */ BANK_ENV,
    /* 3 */ BANK_ENEMY,
    /* 4 */ BANK_SYSTEM,
    /* 5 */ BANK_OCARINA,
    /* 6 */ BANK_VOICE
} SoundBankTypes;

typedef enum {
    /* 0 */ SFX_STATE_EMPTY,
    /* 1 */ SFX_STATE_QUEUED,
    /* 2 */ SFX_STATE_READY,
    /* 3 */ SFX_STATE_PLAYING_REFRESH,
    /* 4 */ SFX_STATE_PLAYING_1,
    /* 5 */ SFX_STATE_PLAYING_2
} SfxState;

typedef struct {
    /* 0x00 */ f32*     posX;
    /* 0x04 */ f32*     posY;
    /* 0x08 */ f32*     posZ;
    /* 0x0C */ f32*     freqScale;
    /* 0x10 */ f32*     vol;
    /* 0x14 */ s8*      reverbAdd;
    /* 0x18 */ f32      dist;
    /* 0x1C */ u32      priority; // lower is more prioritized
    /* 0x20 */ u16      sfxParams;
    /* 0x22 */ u16      sfxId;
    /* 0x25 */ u8       sfxFlags;
    /* 0x24 */ u8       sfxImportance;
    /* 0x26 */ u8       state; // uses SfxState enum
    /* 0x27 */ u8       freshness;
    /* 0x28 */ u8       prev;
    /* 0x29 */ u8       next;
    /* 0x2A */ u8       channelIdx;
    /* 0x2B */ u8       unk_2F;
    /* 0x2C */ u8       token;
} SfxBankEntry; // size = 0x30

/*
 * SFX IDs
 *
 * index    0000000111111111    observed in audio code
 * & 200    0000001000000000    single bit
 * & 400    0000010000000000    single bit
 * & 800    0000100000000000    single bit, what we currently call SFX_FLAG
 * & 600    0000011000000000    2 bits
 * & A00    0000101000000000    2 bits
 * & C00    0000110000000000    2 bits, observed in audio code
 * & E00    0000111000000000    all 3 bits
 * bank     1111000000000000    observed in audio code
 */

#define SFX_BANK_SHIFT(sfxId)   (((sfxId) >> 12) & 0xFF)

#define SFX_BANK_MASK(sfxId)    ((sfxId) & 0xF000)

#define SFX_INDEX(sfxId)    ((sfxId) & 0x03FF)
#define SFX_BANK(sfxId)     SFX_BANK_SHIFT(SFX_BANK_MASK(sfxId))

typedef struct {
    /* 0x0 */ u32 priority; // lower is more prioritized
    /* 0x4 */ u8 entryIndex;
} ActiveSfx; // size = 0x08

typedef struct {
    /* 0x0 */ u8 importance;
    /* 0x1 */ u8 flags;
    /* 0x2 */ u16 params;
} SfxParams; // size = 0x4

typedef struct {
    /* 0x00 */ u16 sfxId;
    /* 0x02 */ u8 token;
    /* 0x04 */ s8* reverbAdd;
    /* 0x08 */ Vec3f* pos;
    /* 0x0C */ f32* freqScale;
    /* 0x10 */ f32* vol;
} SoundRequest; // size = 0x14

/**
 * Note:
 * Flag for resolving C_RIGHT and C_LEFT only being two semitones apart
 * 0x40 - BTN_Z is pressed to lower note by a semitone
 * 0x80 - BTN_R is pressed to raise note by a semitone
 */ 

typedef struct {
    /* 0x0 */ u8 noteIdx;
    /* 0x2 */ u16 length; // number of frames the note is sustained
    /* 0x4 */ u8 volume;
    /* 0x5 */ u8 vibrato;
    /* 0x6 */ s8 bend;
    /* 0x7 */ u8 BFlat4Flag; // BFlat4Flag See note above
} OcarinaNote;  // size = 0x8

typedef struct {
    /* 0x0 */ u8 numButtons;
    /* 0x1 */ u8 buttonIdx[8];
} OcarinaSongButtons; // size = 0x9

typedef struct {
    /* 0x0 */ u8 buttonIdx;
    /* 0x1 */ u8 state;
    /* 0x2 */ u8 pos;
} OcarinaStaff; // size = 0x3

#endif
