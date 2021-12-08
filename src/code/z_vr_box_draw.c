#include "global.h"

Mtx* sSkyboxDrawMatrix;

Mtx* SkyboxDraw_UpdateMatrix(SkyboxContext* skyboxCtx, f32 x, f32 y, f32 z) {
    Matrix_InsertTranslation(x, y, z, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateStateAroundXAxis(skyboxCtx->rotX);
    Matrix_InsertYRotation_f(skyboxCtx->rotY, MTXMODE_APPLY);
    Matrix_InsertZRotation_f(skyboxCtx->rotZ, MTXMODE_APPLY);
    return Matrix_ToMtx(sSkyboxDrawMatrix);
}

void SkyboxDraw_SetColors(SkyboxContext* skyboxCtx, u8 primR, u8 primG, u8 primB, u8 envR, u8 envG, u8 envB) {
    skyboxCtx->primR = primR;
    skyboxCtx->primG = primG;
    skyboxCtx->primB = primB;
    skyboxCtx->envR = envR;
    skyboxCtx->envG = envG;
    skyboxCtx->envB = envB;
}

void SkyboxDraw_Draw(SkyboxContext* skyboxCtx, GraphicsContext* gfxCtx, s16 skyboxId, s16 blend, f32 x, f32 y, f32 z) {
    OPEN_DISPS(gfxCtx);

    func_8012C6AC(gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0B, skyboxCtx->skyboxPaletteStaticSegment);
    gSPTexture(POLY_OPA_DISP++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);

    sSkyboxDrawMatrix = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));

    Matrix_InsertTranslation(x, y, z, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateStateAroundXAxis(skyboxCtx->rotX);
    Matrix_InsertYRotation_f(skyboxCtx->rotY, MTXMODE_APPLY);
    Matrix_InsertZRotation_f(skyboxCtx->rotZ, MTXMODE_APPLY);
    Matrix_ToMtx(sSkyboxDrawMatrix);

    gSPMatrix(POLY_OPA_DISP++, sSkyboxDrawMatrix, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetColorDither(POLY_OPA_DISP++, G_CD_MAGICSQ);
    gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);
    gDPLoadTLUT_pal256(POLY_OPA_DISP++, skyboxCtx->skyboxPaletteStaticSegment);
    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_RGBA16);
    gDPSetTextureConvert(POLY_OPA_DISP++, G_TC_FILT);
    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL1, TEXEL0, PRIMITIVE_ALPHA, TEXEL0, TEXEL1, TEXEL0, PRIMITIVE, TEXEL0,
                      PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, 0, 0, 0, COMBINED);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, skyboxCtx->primR, skyboxCtx->primG, skyboxCtx->primB, blend);
    gDPSetEnvColor(POLY_OPA_DISP++, skyboxCtx->envR, skyboxCtx->envG, skyboxCtx->envB, 0);

    gSPDisplayList(POLY_OPA_DISP++, &skyboxCtx->dListBuf[0]);
    gSPDisplayList(POLY_OPA_DISP++, &skyboxCtx->dListBuf[300]);
    gSPDisplayList(POLY_OPA_DISP++, &skyboxCtx->dListBuf[600]);
    gSPDisplayList(POLY_OPA_DISP++, &skyboxCtx->dListBuf[900]);
    gSPDisplayList(POLY_OPA_DISP++, &skyboxCtx->dListBuf[1200]);

    if (skyboxId == SKYBOX_CUTSCENE_MAP) {
        gSPDisplayList(POLY_OPA_DISP++, &skyboxCtx->dListBuf[1500]);
    }

    gDPPipeSync(POLY_OPA_DISP++);

    CLOSE_DISPS(gfxCtx);
}

void SkyboxDraw_Noop(SkyboxContext* skyboxCtx) {
}
