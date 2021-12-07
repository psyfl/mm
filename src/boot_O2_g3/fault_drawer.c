#include "global.h"
#include "vt.h"

extern const u32 sFaultDrawerFont[];

FaultDrawer sFaultDrawerStruct;

FaultDrawer* sFaultDrawContext = &sFaultDrawerStruct;
FaultDrawer sFaultDrawerDefault = {
    FAULT_FB_ADDRESS,                   // fb
    SCREEN_WIDTH,                       // w
    SCREEN_HEIGHT,                      // h
    16,                                 // yStart
    223,                                // yEnd
    22,                                 // xStart
    297,                                // xEnd
    GPACK_RGBA5551(255, 255, 255, 255), // foreColor
    GPACK_RGBA5551(0, 0, 0, 0),         // backColor
    22,                                 // cursorX
    16,                                 // cursorY
    sFaultDrawerFont,                   // font
    8,                                  // charW
    8,                                  // charH
    0,                                  // charWPad
    0,                                  // charHPad
    {
        // printColors
        GPACK_RGBA5551(0, 0, 0, 1),       // BLACK
        GPACK_RGBA5551(255, 0, 0, 1),     // RED
        GPACK_RGBA5551(0, 255, 0, 1),     // GREEN
        GPACK_RGBA5551(255, 255, 0, 1),   // YELLOW
        GPACK_RGBA5551(0, 0, 255, 1),     // BLUE
        GPACK_RGBA5551(255, 0, 255, 1),   // MAGENTA
        GPACK_RGBA5551(0, 255, 255, 1),   // CYAN
        GPACK_RGBA5551(255, 255, 255, 1), // WHITE
        GPACK_RGBA5551(120, 120, 120, 1), // DARK GRAY
        GPACK_RGBA5551(176, 176, 176, 1), // LIGHT GRAY
    },
    0,    // escCode
    0,    // osSyncPrintfEnabled
    NULL, // inputCallback
};

#pragma GLOBAL_ASM("asm/non_matchings/boot/fault_drawer/sFaultDrawerFont.s")

void FaultDrawer_SetOsSyncPrintfEnabled(u32 enabled) {
    sFaultDrawContext->osSyncPrintfEnabled = enabled;
}

void FaultDrawer_DrawRecImpl(s32 xStart, s32 yStart, s32 xEnd, s32 yEnd, u16 color) {
    u16* fb;
    s32 x;
    s32 y;
    s32 xDiff = sFaultDrawContext->w - xStart;
    s32 yDiff = sFaultDrawContext->h - yStart;
    s32 xSize = xEnd - xStart + 1;
    s32 ySize = yEnd - yStart + 1;

    if (xDiff > 0 && yDiff > 0) {
        if (xDiff < xSize) {
            xSize = xDiff;
        }

        if (yDiff < ySize) {
            ySize = yDiff;
        }

        fb = sFaultDrawContext->fb + sFaultDrawContext->w * yStart + xStart;
        for (y = 0; y < ySize; y++) {
            for (x = 0; x < xSize; x++) {
                *fb++ = color;
            }
            fb += sFaultDrawContext->w - xSize;
        }

        osWritebackDCacheAll();
    }
}

void FaultDrawer_DrawChar(char c) {
    s32 x;
    s32 y;
    u32 data;
    s32 cursorX = sFaultDrawContext->cursorX;
    s32 cursorY = sFaultDrawContext->cursorY;
    s32 shift = c % 4;
    const u32* dataPtr = &sFaultDrawContext->font[(((c / 8) * 16) + ((c & 4) >> 2))];
    u16* fb = sFaultDrawContext->fb + (sFaultDrawContext->w * cursorY) + cursorX;

    if ((sFaultDrawContext->xStart <= cursorX) &&
        ((sFaultDrawContext->charW + cursorX - 1) <= sFaultDrawContext->xEnd) &&
        (sFaultDrawContext->yStart <= cursorY) &&
        ((sFaultDrawContext->charH + cursorY - 1) <= sFaultDrawContext->yEnd)) {
        for (y = 0; y < sFaultDrawContext->charH; y++) {
            u32 mask = 0x10000000 << shift;

            data = *dataPtr;
            for (x = 0; x < sFaultDrawContext->charW; x++) {
                if (mask & data) {
                    fb[x] = sFaultDrawContext->foreColor;
                } else if (sFaultDrawContext->backColor & 1) {
                    fb[x] = sFaultDrawContext->backColor;
                }
                mask >>= 4;
            }
            fb += sFaultDrawContext->w;
            dataPtr += 2;
        }
    }
}

s32 FaultDrawer_ColorToPrintColor(u16 color) {
    s32 i;

    for (i = 0; i < 10; i++) {
        if (color == sFaultDrawContext->printColors[i]) {
            return i;
        }
    }
    return -1;
}

void FaultDrawer_UpdatePrintColor() {
    s32 idx;

    if (sFaultDrawContext->osSyncPrintfEnabled) {
        osSyncPrintf(VT_RST);
        idx = FaultDrawer_ColorToPrintColor(sFaultDrawContext->foreColor);
        if (idx >= 0 && idx < 8) {
            osSyncPrintf(VT_SGR("3%d"), idx);
        }
        idx = FaultDrawer_ColorToPrintColor(sFaultDrawContext->backColor);
        if (idx >= 0 && idx < 8) {
            osSyncPrintf(VT_SGR("4%d"), idx);
        }
    }
}

void FaultDrawer_SetForeColor(u16 color) {
    sFaultDrawContext->foreColor = color;
    FaultDrawer_UpdatePrintColor();
}

void FaultDrawer_SetBackColor(u16 color) {
    sFaultDrawContext->backColor = color;
    FaultDrawer_UpdatePrintColor();
}

void FaultDrawer_SetFontColor(u16 color) {
    FaultDrawer_SetForeColor(color | 1); // force alpha to be set
}

void FaultDrawer_SetCharPad(s8 padW, s8 padH) {
    sFaultDrawContext->charWPad = padW;
    sFaultDrawContext->charHPad = padH;
}

void FaultDrawer_SetCursor(s32 x, s32 y) {
    if (sFaultDrawContext->osSyncPrintfEnabled) {
        osSyncPrintf(VT_CUP("%d", "%d"),
                     (y - sFaultDrawContext->yStart) / (sFaultDrawContext->charH + sFaultDrawContext->charHPad),
                     (x - sFaultDrawContext->xStart) / (sFaultDrawContext->charW + sFaultDrawContext->charWPad));
    }
    sFaultDrawContext->cursorX = x;
    sFaultDrawContext->cursorY = y;
}

void FaultDrawer_FillScreen() {
    if (sFaultDrawContext->osSyncPrintfEnabled) {
        osSyncPrintf(VT_CLS);
    }

    FaultDrawer_DrawRecImpl(sFaultDrawContext->xStart, sFaultDrawContext->yStart, sFaultDrawContext->xEnd,
                            sFaultDrawContext->yEnd, sFaultDrawContext->backColor | 1);
    FaultDrawer_SetCursor(sFaultDrawContext->xStart, sFaultDrawContext->yStart);
}

void* FaultDrawer_FormatStringFunc(void* arg, const char* str, size_t count) {
    for (; count != 0; count--, str++) {
        if (sFaultDrawContext->escCode) {
            sFaultDrawContext->escCode = false;
            if (*str >= '1' && *str <= '9') {
                FaultDrawer_SetForeColor(sFaultDrawContext->printColors[*str - '0']);
            }
        } else {
            switch (*str) {
                case '\n':
                    if (sFaultDrawContext->osSyncPrintfEnabled) {
                        osSyncPrintf("\n");
                    }

                    sFaultDrawContext->cursorX = sFaultDrawContext->w;
                    break;
                case '\x1A':
                    sFaultDrawContext->escCode = true;
                    break;
                default:
                    if (sFaultDrawContext->osSyncPrintfEnabled) {
                        osSyncPrintf("%c", *str);
                    }

                    FaultDrawer_DrawChar(*str);
                    sFaultDrawContext->cursorX += sFaultDrawContext->charW + sFaultDrawContext->charWPad;
            }
        }

        if (sFaultDrawContext->cursorX >= (sFaultDrawContext->xEnd - sFaultDrawContext->charW)) {
            sFaultDrawContext->cursorX = sFaultDrawContext->xStart;
            sFaultDrawContext->cursorY += sFaultDrawContext->charH + sFaultDrawContext->charHPad;
            if (sFaultDrawContext->yEnd - sFaultDrawContext->charH <= sFaultDrawContext->cursorY) {
                if (sFaultDrawContext->inputCallback != NULL) {
                    sFaultDrawContext->inputCallback();
                    FaultDrawer_FillScreen();
                }
                sFaultDrawContext->cursorY = sFaultDrawContext->yStart;
            }
        }
    }

    osWritebackDCacheAll();

    return arg;
}

const char D_80099080[] = "(null)";

void FaultDrawer_VPrintf(const char* fmt, va_list ap) {
    _Printf(FaultDrawer_FormatStringFunc, sFaultDrawContext, fmt, ap);
}

void FaultDrawer_Printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    FaultDrawer_VPrintf(fmt, args);

    va_end(args);
}

void FaultDrawer_DrawText(s32 x, s32 y, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    FaultDrawer_SetCursor(x, y);
    FaultDrawer_VPrintf(fmt, args);

    va_end(args);
}

void FaultDrawer_SetDrawerFB(void* fb, u16 w, u16 h) {
    sFaultDrawContext->fb = (u16*)fb;
    sFaultDrawContext->w = w;
    sFaultDrawContext->h = h;
}

void FaultDrawer_SetInputCallback(FaultDrawerCallback callback) {
    sFaultDrawContext->inputCallback = callback;
}

void FaultDrawer_Init() {
    sFaultDrawContext = &sFaultDrawerStruct;
    bcopy(&sFaultDrawerDefault, sFaultDrawContext, sizeof(FaultDrawer));
    sFaultDrawContext->fb = (u16*)((osMemSize | 0x80000000) - 0x25800);
}
