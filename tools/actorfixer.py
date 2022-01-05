#!/usr/bin/env python3

import argparse, os

# There are a few commented out entries that would produce unexpected renames.
# They are left as a comment so people can just grab them.

# "old": "new"
animdict = {
    "Actor_GetSwitchFlag": "Flags_GetSwitch",
    "atan_flip": "Math_Acot2F",
    "atans": "Math_Atan2S",
    "SysMatrix_StateAlloc": "Matrix_StateAlloc",
    "SysMatrix_StatePush": "Matrix_StatePush",
    "SysMatrix_CopyCurrentState": "Matrix_CopyCurrentState",
    "SysMatrix_SetCurrentState": "Matrix_SetCurrentState",
    "SysMatrix_InsertTranslation": "Matrix_InsertTranslation",
    "SysMatrix_InsertMatrix": "Matrix_InsertMatrix",
    "SysMatrix_Scale": "Matrix_Scale",
    "SysMatrix_InsertXRotation_s": "Matrix_InsertXRotation_s",
    "SysMatrix_InsertXRotation_f": "Matrix_InsertXRotation_f",
    "SysMatrix_RotateStateAroundXAxis": "Matrix_RotateStateAroundXAxis",
    "SysMatrix_SetStateXRotation": "Matrix_SetStateXRotation",
    "SysMatrix_RotateY": "Matrix_RotateY",
    "SysMatrix_InsertYRotation_f": "Matrix_InsertYRotation_f",
    "SysMatrix_InsertZRotation_s": "Matrix_InsertZRotation_s",
    "SysMatrix_InsertZRotation_f": "Matrix_InsertZRotation_f",
    "SysMatrix_InsertRotation": "Matrix_InsertRotation",
    "SysMatrix_JointPosition": "Matrix_JointPosition",
    "SysMatrix_SetStateRotationAndTranslation": "Matrix_SetStateRotationAndTranslation",
    "SysMatrix_ToRSPMatrix": "Matrix_ToRSPMatrix",
    "SysMatrix_ToMtx": "Matrix_ToMtx",
    "SysMatrix_NewMtx": "Matrix_NewMtx",
    "SysMatrix_AppendToPolyOpaDisp": "Matrix_AppendToPolyOpaDisp",
    "SysMatrix_MultiplyVector3fByState": "Matrix_MultiplyVector3fByState",
    "SysMatrix_GetStateTranslation": "Matrix_GetStateTranslation",
    "SysMatrix_GetStateTranslationAndScaledX": "Matrix_GetStateTranslationAndScaledX",
    "SysMatrix_GetStateTranslationAndScaledY": "Matrix_GetStateTranslationAndScaledY",
    "SysMatrix_GetStateTranslationAndScaledZ": "Matrix_GetStateTranslationAndScaledZ",
    "SysMatrix_MultiplyVector3fXZByCurrentState": "Matrix_MultiplyVector3fXZByCurrentState",
    "SysMatrix_MtxFCopy": "Matrix_MtxFCopy",
    "SysMatrix_FromRSPMatrix": "Matrix_FromRSPMatrix",
    "SysMatrix_MultiplyVector3fByMatrix": "Matrix_MultiplyVector3fByMatrix",
    "SysMatrix_TransposeXYZ": "Matrix_TransposeXYZ",
    "SysMatrix_NormalizeXYZ": "Matrix_NormalizeXYZ",
    "SysMatrix_InsertRotationAroundUnitVector_f": "Matrix_InsertRotationAroundUnitVector_f",
    "SysMatrix_InsertRotationAroundUnitVector_s": "Matrix_InsertRotationAroundUnitVector_s",
    "SysMatrix_GetStateAsRSPMatrix": "Matrix_GetStateAsRSPMatrix",
    "func_800B78B8": "Actor_UpdateBgCheckInfo",
    "func_8012403C": "Player_GetMask",
    "func_8012404c": "Player_RemoveMask",
    "Actor_SpawnWithParentAndCutscene": "Actor_SpawnAsChildAndCutscene",
    "Actor_SpawnWithParent": "Actor_SpawnAsChild",
    "func_800BE22C": "Actor_ApplyDamage",
    "func_800F0568": "Audio_PlaySoundAtPosition",
    "func_8016970C": "Play_CameraSetAtEye",
    "func_800BBA88": "Enemy_StartFinishingBlow",
    "ShrinkWindow_Step": "ShrinkWindow_Update",
    "ShrinkWindow_Fini": "ShrinkWindow_Destroy",
    "func_801A89A8": "Audio_QueueSeqCmd",
    "SkelAnime_LodDrawLimb(": "SkelAnime_DrawLimbLod(",
    "SkelAnime_LodDraw(": "SkelAnime_DrawLod(",
    "SkelAnime_LodDrawLimbSV(": "SkelAnime_DrawFlexLimbLod(",
    "SkelAnime_LodDrawSV(": "SkelAnime_DrawFlexLod(",
    #"SkelAnime_DrawLimb(": "SkelAnime_DrawLimbOpa(",
    #"SkelAnime_Draw(": "SkelAnime_DrawOpa(",
    "SkelAnime_DrawLimbSV(": "SkelAnime_DrawFlexLimbOpa(",
    "SkelAnime_DrawSV(": "SkelAnime_DrawFlexOpa(",
    #"SkelAnime_AnimateFrame(": "SkelAnime_GetFrameData(",
    "SkelAnime_GetTotalFrames(": "Animation_GetLength(",
    "SkelAnime_GetFrameCount(": "Animation_GetLastFrame(",
    "SkelAnime_Draw2Limb(": "SkelAnime_DrawLimb(",
    "SkelAnime_Draw2(": "SkelAnime_Draw(",
    "SkelAnime_DrawLimbSV2(": "SkelAnime_DrawFlexLimb(",
    "SkelAnime_DrawSV2(": "SkelAnime_DrawFlex(",
    "func_80134FFC": "SkelAnime_GetFrameData2",
    "func_801353D4": "Animation_GetLimbCount2",
    "SkelAnime_GetTotalFrames2(": "Animation_GetLength2(",
    "SkelAnime_GetFrameCount2(": "Animation_GetLastFrame2(",
    "SkelAnime_InterpolateVec3s": "SkelAnime_InterpFrameTable",
    "SkelAnime_AnimationCtxReset": "AnimationContext_Reset",
    "func_801358D4": "AnimationContext_SetNextQueue",
    "func_801358F4": "AnimationContext_DisableQueue",
    "SkelAnime_NextEntry": "AnimationContext_AddEntry",
    "SkelAnime_LoadLinkAnimetion": "AnimationContext_SetLoadFrame",
    "SkelAnime_LoadAnimationType1": "AnimationContext_SetCopyAll",
    "SkelAnime_LoadAnimationType2": "AnimationContext_SetInterp",
    "SkelAnime_LoadAnimationType3": "AnimationContext_SetCopyTrue",
    "SkelAnime_LoadAnimationType4": "AnimationContext_SetCopyFalse",
    "SkelAnime_LoadAnimationType5": "AnimationContext_SetMoveActor",
    "SkelAnime_LinkAnimetionLoaded": "AnimationContext_LoadFrame",
    "SkelAnime_AnimationType1Loaded": "AnimationContext_CopyAll",
    "SkelAnime_AnimationType2Loaded": "AnimationContext_CopyInterp",
    "SkelAnime_AnimationType3Loaded": "AnimationContext_CopyTrue",
    "SkelAnime_AnimationType4Loaded": "AnimationContext_CopyFalse",
    "SkelAnime_AnimationType5Loaded": "AnimationContext_MoveActor",
    "func_80135EE8": "AnimationContext_Update",
    "SkelAnime_InitLinkAnimetion(": "SkelAnime_InitLink(",
    "func_801360A8": "LinkAnimation_SetUpdateFunction",
    "func_801360E0": "LinkAnimation_Update",
    "func_80136104": "LinkAnimation_Morph",
    "func_801361BC": "LinkAnimation_AnimateFrame",
    "func_80136288": "LinkAnimation_Loop",
    "func_8013631C": "LinkAnimation_Once",
    "SkelAnime_SetTransition": "Animation_SetMorph",
    "SkelAnime_ChangeLinkAnim": "LinkAnimation_Change",
    "SkelAnime_ChangeLinkAnimDefaultStop": "LinkAnimation_PlayOnce",
    "SkelAnime_ChangeLinkAnimPlaybackStop": "LinkAnimation_PlayOnceSetSpeed",
    "SkelAnime_ChangeLinkAnimDefaultRepeat": "LinkAnimation_PlayLoop",
    "SkelAnime_ChangeLinkAnimPlaybackRepeat": "LinkAnimation_PlayLoopSetSpeed",
    "func_8013670C": "LinkAnimation_CopyJointToMorph",
    "func_8013673C": "LinkAnimation_CopyMorphToJoint",
    "func_8013676C": "LinkAnimation_LoadToMorph",
    "func_801367B0": "LinkAnimation_LoadToJoint",
    "func_801367F4": "LinkAnimation_InterpJointMorph",
    "func_8013682C": "LinkAnimation_BlendToJoint",
    "func_801368CC": "LinkAnimation_BlendToMorph",
    "SkelAnime_SetModeStop": "LinkAnimation_EndLoop",
    "func_80136990": "Animation_OnFrameImpl",
    "func_80136A48": "LinkAnimation_OnFrame",
    "SkelAnime_InitSV": "SkelAnime_InitFlex",
    "func_80136C84": "SkelAnime_SetUpdate",
    "SkelAnime_FrameUpdateMatrix": "SkelAnime_Update",
    "func_80136CF4": "SkelAnime_Morph",
    "func_80136D98": "SkelAnime_MorphTaper",
    "func_80136F04": "SkelAnime_AnimateFrame",
    "func_8013702C": "SkelAnime_LoopFull",
    "func_801370B0": "SkelAnime_LoopPartial",
    "func_8013713C": "SkelAnime_Once",
    "SkelAnime_ChangeAnimImpl": "Animation_ChangeImpl",
    "SkelAnime_ChangeAnim(": "Animation_Change(",
    "SkelAnime_ChangeAnimDefaultStop": "Animation_PlayOnce",
    "SkelAnime_ChangeAnimTransitionStop": "Animation_MorphToPlayOnce",
    "SkelAnime_ChangeAnimPlaybackStop": "Animation_PlayOnceSetSpeed",
    "SkelAnime_ChangeAnimDefaultRepeat": "Animation_PlayLoop",
    "SkelAnime_ChangeAnimTransitionRepeat": "Animation_MorphToLoop",
    "SkelAnime_ChangeAnimPlaybackRepeat": "Animation_PlayLoopSetSpeed",
    "SkelAnime_AnimSetStop": "Animation_EndLoop",
    "SkelAnime_AnimReverse": "Animation_Reverse",
    "func_80137674": "SkelAnime_CopyFrameTableTrue",
    "func_801376DC": "SkelAnime_CopyFrameTableFalse",
    "func_80137748": "SkelAnime_UpdateTranslation",
    "func_801378B8": "Animation_OnFrame",
    "SkelAnime_CopyVec3s": "SkelAnime_CopyFrameTable",
    "SysMatrix_StatePop": "Matrix_StatePop",
    "SysMatrix_GetCurrentState": "Matrix_GetCurrentState",
    "zelda_malloc(": "ZeldaArena_Malloc(",
    "zelda_mallocR(": "ZeldaArena_MallocR(",
    "zelda_realloc": "ZeldaArena_Realloc",
    "zelda_free": "ZeldaArena_Free",
    "zelda_calloc": "ZeldaArena_Calloc",
    "MainHeap_AnalyzeArena": "ZeldaArena_GetSizes",
    "MainHeap_Check": "ZeldaArena_Check",
    "MainHeap_Init": "ZeldaArena_Init",
    "MainHeap_Cleanup": "ZeldaArena_Cleanup",
    "MainHeap_IsInitialized": "ZeldaArena_IsInitialized",
    # "BgCheck_RelocateMeshHeader": "CollisionHeader_GetVirtual",
    # "BgCheck_AddActorMesh": "DynaPoly_SetBgActor",
    # "BgCheck_RemoveActorMesh": "DynaPoly_DeleteBgActor",
    "BgCheck_PolygonLinkedListNodeInit": "SSNode_SetValue",
    "BgCheck_PolygonLinkedListResetHead": "SSList_SetNull",
    "BgCheck_ScenePolygonListsNodeInsert": "SSNodeList_SetSSListHead",
    "BgCheck_PolygonLinkedListNodeInsert": "DynaSSNodeList_SetSSListHead",
    "BgCheck_PolygonLinkedListInit": "DynaSSNodeList_Init",
    "BgCheck_PolygonLinkedListAlloc": "DynaSSNodeList_Alloc",
    "BgCheck_PolygonLinkedListReset": "DynaSSNodeList_ResetCount",
    "BgCheck_AllocPolygonLinkedListNode": "DynaSSNodeList_GetNextNodeIdx",
    "BgCheck_CreateVec3fFromVertex": "BgCheck_Vec3sToVec3f",
    "BgCheck_CreateVertexFromVec3f": "BgCheck_Vec3fToVec3s",
    "BgCheck_PolygonGetMinY": "CollisionPoly_GetMinY",
    "BgCheck_PolygonGetNormal": "CollisionPoly_GetNormalF",
    "func_800C01B8": "CollisionPoly_GetPointDistanceFromPlane",
    "BgCheck_CreateTriNormFromPolygon": "CollisionPoly_GetVertices",
    "func_800C02C0": "CollisionPoly_GetVerticesByBgId",
    "BgCheck_PolygonCollidesWithSphere": "CollisionPoly_SphVsPoly",
    "BgCheck_ScenePolygonListsInsertSorted": "StaticLookup_AddPolyToSSList",
    "BgCheck_ScenePolygonListsInsert": "StaticLookup_AddPoly",
    "BgCheck_GetPolyMinSubdivisions": "BgCheck_GetSubdivisionMinBounds",
    "BgCheck_GetPolyMaxSubdivisions": "BgCheck_GetSubdivisionMaxBounds",
    "BgCheck_GetPolyMinMaxSubdivisions": "BgCheck_GetPolySubdivisionBounds",
    "func_800C2BE0": "BgCheck_PolyIntersectsSubdivision",
    "BgCheck_SplitScenePolygonsIntoSubdivisions": "BgCheck_InitStaticLookup",
    "BgCheck_GetIsDefaultSpecialScene": "BgCheck_IsSmallMemScene",
    "BgCheck_GetSpecialSceneMaxMemory": "BgCheck_TryGetCustomMemsize",
    "BgCheck_CalcSubdivisionSize": "BgCheck_SetSubdivisionDimension",
    "BgCheck_Init(": "BgCheck_Allocate(",
    "func_800C3C00": "BgCheck_SetContextFlags",
    "func_800C3C14": "BgCheck_UnsetContextFlags",
    "BgCheck_GetActorMeshHeader": "BgCheck_GetCollisionHeader",
    "func_800C3D50": "BgCheck_RaycastFloorImpl",
    "func_800C3F40": "BgCheck_CameraRaycastFloor1",
    "func_800C3FA0": "BgCheck_EntityRaycastFloor1",
    "func_800C4000": "BgCheck_EntityRaycastFloor2",
    "func_800C4058": "BgCheck_EntityRaycastFloor2_1",
    "func_800C40B4": "BgCheck_EntityRaycastFloor3",
    "func_800C411C": "BgCheck_EntityRaycastFloor5",
    "func_800C4188": "BgCheck_EntityRaycastFloor5_2",
    "func_800C41E4": "BgCheck_EntityRaycastFloor5_3",
    "func_800C4240": "BgCheck_EntityRaycastFloor6",
    "func_800C42A8": "BgCheck_EntityRaycastFloor7",
    "func_800C4314": "BgCheck_AnyRaycastFloor1",
    "func_800C43CC": "BgCheck_AnyRaycastFloor2",
    "func_800C4488": "BgCheck_CameraRaycastFloor2",
    "func_800C44F0": "BgCheck_EntityRaycastFloor8",
    "func_800C455C": "BgCheck_EntityRaycastFloor9",
    "func_800C45C4": "BgCheck_CheckWallImpl",
    "func_800C4C74": "BgCheck_EntitySphVsWall1",
    "func_800C4CD8": "BgCheck_EntitySphVsWall2",
    "func_800C4D3C": "BgCheck_EntitySphVsWall3",
    "func_800C4DA4": "BgCheck_EntitySphVsWall4",
    "func_800C4E10": "BgCheck_CheckCeilingImpl",
    "func_800C4F38": "BgCheck_AnyCheckCeiling",
    "func_800C4F84": "BgCheck_EntityCheckCeiling",
    "func_800C54AC": "BgCheck_CameraLineTest1",
    "func_800C5538": "BgCheck_CameraLineTest2",
    "func_800C55C4": "BgCheck_EntityLineTest1",
    "func_800C5650": "BgCheck_EntityLineTest2",
    "func_800C56E0": "BgCheck_EntityLineTest3",
    "func_800C576C": "BgCheck_ProjectileLineTest",
    "func_800C57F8": "BgCheck_AnyLineTest1",
    "func_800C583C": "BgCheck_AnyLineTest2",
    "func_800C58C8": "BgCheck_AnyLineTest3",
    "func_800C5954": "BgCheck_SphVsFirstPolyImpl",
    "func_800C5A20": "BgCheck_SphVsFirstPoly",
    "func_800C5A64": "BgCheck_SphVsFirstWall",
    "BgCheck_ScenePolygonListsInit": "SSNodeList_Init",
    "BgCheck_ScenePolygonListsAlloc": "SSNodeList_Alloc",
    "func_800C5B80": "SSNodeList_GetNextNode",
    "BgCheck_ScenePolygonListsReserveNode": "SSNodeList_GetNextNodeIdx",
    "BgCheck_ActorMeshParamsInit": "ScaleRotPos_Init",
    "BgCheck_SetActorMeshParams": "ScaleRotPos_SetValue",
    "BgCheck_ActorMeshPolyListsHeadsInit": "DynaLookup_ResetLists",
    "BgCheck_ActorMeshPolyListsInit": "DynaLookup_Reset",
    "BgCheck_ActorMeshVerticesIndexInit": "DynaLookup_ResetVtxStartIndex",
    "BgCheck_ActorMeshWaterboxesIndexInit": "DynaLookup_ResetWaterBoxStartIndex",
    "BgCheck_ActorMeshInit": "BgActor_Init",
    "BgCheck_ActorMeshInitFromActor": "BgActor_SetActor",
    "BgCheck_HasActorMeshChanged": "BgActor_IsTransformUnchanged",
    "BgCheck_PolygonsInit": "DynaPoly_NullPolyList",
    "BgCheck_PolygonsAlloc": "DynaPoly_AllocPolyList",
    "BgCheck_VerticesInit": "DynaPoly_NullVtxList",
    "BgCheck_VerticesListAlloc": "DynaPoly_AllocVtxList",
    "BgCheck_WaterboxListInit": "DynaPoly_InitWaterBoxList",
    "BgCheck_WaterboxListAlloc": "DynaPoly_AllocWaterBoxList",
    "BgCheck_ActorMeshUpdateParams": "DynaPoly_SetBgActorPrevTransform",
    "BgCheck_IsActorMeshIndexValid": "DynaPoly_IsBgIdBgActor",
    "BgCheck_DynaInit": "DynaPoly_Init",
    "BgCheck_DynaAlloc": "DynaPoly_Alloc",
    "BgCheck_AddActorMesh": "DynaPoly_SetBgActor",
    "BgCheck_GetActorOfMesh": "DynaPoly_GetActor",
    "BgCheck_RemoveActorMesh": "DynaPoly_DeleteBgActor",
    "BgCheck_AddActorMeshToLists": "DynaPoly_ExpandSRT",
    "BgCheck_Update": "DynaPoly_Setup",
    "BgCheck_UpdateAllActorMeshes": "DynaPoly_UpdateBgActorTransforms",
    "BgCheck_RelocateMeshHeaderPointers": "CollisionHeader_SegmentedToVirtual",
    "BgCheck_RelocateMeshHeader": "CollisionHeader_GetVirtual",
    "BgCheck_RelocateAllMeshHeaders": "BgCheck_InitCollisionHeaders",
    "BgCheck_GetPolygonAttributes": "SurfaceType_GetData",
    "func_800C9704": "SurfaceType_GetCamDataIndex",
    "func_800C9924": "SurfaceType_GetCamPosData",
    "func_800C99AC": "SurfaceType_GetSceneExitIndex",
    "func_800C9B90": "SurfaceType_IsHorseBlocked",
    "func_800C9BDC": "SurfaceType_GetSfx",
    "func_800C9C74": "SurfaceType_GetSlope",
    "func_800C9C9C": "SurfaceType_GetLightSettingIndex",
    "func_800C9CC4": "SurfaceType_GetEcho",
    "func_800C9CEC": "SurfaceType_IsHookshotSurface",
    "func_800C9D14": "SurfaceType_IsIgnoredByEntities",
    "func_800C9D50": "SurfaceType_IsIgnoredByProjectiles",
    "func_800C9D8C": "SurfaceType_IsConveyor",
    "func_800C9E18": "SurfaceType_GetConveyorSpeed",
    "func_800C9E40": "SurfaceType_GetConveyorDirection",
    "func_800C9E88": "SurfaceType_IsWallDamage",
    "func_800C9EBC": "WaterBox_GetSurfaceImpl",
    "func_800CA1AC": "WaterBox_GetSurface1",
    "func_800CA1E8": "WaterBox_GetSurface1_2",
    "func_800CA22C": "WaterBox_GetSurface2",
    "func_800CA6D8": "WaterBox_GetLightSettingIndex",
    "func_80179678": "Math3D_PlaneVsLineSegClosestPoint",
    "Math3D_DistanceSquared": "Math3D_Vec3fDistSq",
    "Math3D_NormalVector": "Math3D_SurfaceNorm",
    "func_8017A954": "Math3D_PointRelativeToCubeFaces",
    "func_8017AA0C": "Math3D_PointRelativeToCubeEdges",
    "func_8017ABBC": "Math3D_PointRelativeToCubeVertices",
    "func_8017AD38": "Math3D_LineVsCube",
    "Math3D_NormalizedDistanceFromPlane": "Math3D_UDistPlaneToPos",
    "Math3D_NormalizedSignedDistanceFromPlane": "Math3D_DistPlaneToPos",
    "func_8017BAD0": "Math3D_TriChkPointParaYDist",
    "func_8017BE30": "Math3D_TriChkPointParaYIntersectDist",
    "func_8017BEE0": "Math3D_TriChkPointParaYIntersectInsideTri",
    "func_8017C008": "Math3D_TriChkLineSegParaYIntersect",
    "func_8017C494": "Math3D_TriChkPointParaYIntersectInsideTri2",
    "func_8017C540": "Math3D_TriChkPointParaXDist",
    "func_8017C850": "Math3D_TriChkPointParaXIntersect",
    "func_8017C980": "Math3D_TriChkLineSegParaXIntersect",
    "func_8017CB7C": "Math3D_TriChkLineSegParaZDist",
    "func_8017CEF0": "Math3D_TriChkPointParaZIntersect",
    "func_8017D020": "Math3D_TriChkLineSegParaZIntersect",
    "Math3D_ColSphereLineSeg": "Math3D_LineVsSph",
    "Math3D_ColSphereSphere(": "Math3D_SphVsSph(",
    "func_8017F9C0": "Math3D_XZInSphere",
    "func_8017FA34": "Math3D_XYInSphere",
    "func_8017FAA8": "Math3D_YZInSphere",
    "func_8013A7C0": "SubS_FindDoor",
    "func_8013E640": "SubS_FindActorCustom",
    "func_ActorCategoryIterateById": "SubS_FindActor",
    "func_8013BB7C": "SubS_FindNearestActor",
    "func_800A81F0": "EffectBlure_AddVertex",
    "func_800A8514": "EffectBlure_AddSpace",
    "Effect_GetParams": "Effect_GetByIndex",
    
    "skelAnime.unk03": "skelAnime.taper",
    "skelAnime.animCurrentSeg": "skelAnime.animation",
    "skelAnime.initialFrame": "skelAnime.startFrame",
    "skelAnime.animFrameCount": "skelAnime.endFrame",
    "skelAnime.totalFrames": "skelAnime.animLength",
    "skelAnime.animCurrentFrame": "skelAnime.curFrame",
    "skelAnime.animPlaybackSpeed": "skelAnime.playSpeed",
    "skelAnime.limbDrawTbl": "skelAnime.jointTable",
    "skelAnime.transitionDrawTbl": "skelAnime.morphTable",
    "skelAnime.transCurrentFrame": "skelAnime.morphWeight",
    "skelAnime.transitionStep": "skelAnime.morphRate",
    "skelAnime.animUpdate": "skelAnime.update",
    "skelAnime.flags": "skelAnime.moveFlags",
    "skelAnime.prevFrameRot": "skelAnime.prevRot",
    "skelAnime.prevFramePos": "skelAnime.prevTransl",
    "skelAnime.unk3E": "skelAnime.baseTransl",
    "actor.yDistToWater": "actor.depthInWater",
    "actor.yDistToPlayer": "actor.playerHeightRel",
    "globalCtx->mf_187FC": "globalCtx->billboardMtxF",
    "globalCtx->projectionMatrix": "globalCtx->viewProjectionMtxF",

    "D_0407D590": "gGameplayKeepDrawFlameDL",
    "D_801D15B0": "gZeroVec3f",
    "D_801D15BC": "gZeroVec3s",
    "D_801D1DE0": "gIdentityMtx",
    "D_801D1E20": "gIdentityMtxF",
}

def replace_anim(file):
    with open(file, 'r', encoding='utf-8') as infile:
        srcdata = infile.read()

    funcs = list(animdict.keys())
    fixes = 0
    for func in funcs:
        newfunc = animdict.get(func)
        if(newfunc is None):
            print("How did this happen?")
            return -1
        if(func in srcdata):
            fixes += 1
            print(func)
            srcdata = srcdata.replace(func, newfunc)

    if(fixes > 0):
        print('Changed', fixes,'entr' + ('y' if fixes == 1 else 'ies') + ' in',file)
        with open(file, 'w', encoding = 'utf-8', newline = '\n') as outfile:
            outfile.write(srcdata)
    return 1

def replace_anim_all(repo):
    for subdir, dirs, files in os.walk(repo + os.sep + 'src'):
        for filename in files:
            if(filename.endswith('.c')):
                file = subdir + os.sep + filename
                replace_anim(file)

    for subdir, dirs, files in os.walk(repo + os.sep + 'asm'):
        for filename in files:
            if(filename.endswith('.s')):
                file = subdir + os.sep + filename
                replace_anim(file)                

    for subdir, dirs, files in os.walk(repo + os.sep + 'data'):
        for filename in files:
            if(filename.endswith('.s')):
                file = subdir + os.sep + filename
                replace_anim(file)
                
    for subdir, dirs, files in os.walk(repo + os.sep + 'docs'):
        for filename in files:
            if(filename.endswith('.md')):
                file = subdir + os.sep + filename
                replace_anim(file)
              
    for subdir, dirs, files in os.walk(repo + os.sep + 'tools' + os.sep + 'sizes'):
        for filename in files:
            if(filename.endswith('.csv')):
                file = subdir + os.sep + filename
                replace_anim(file)
    return 1

def dictSanityCheck():
    keys = animdict.keys()
    values = animdict.values()
    for k in keys:
        if k in values:
            print(f"Key '{k}' found in values")
            print(f"This would produce unintended renames")
            print(f"Fix this by removing said key from the dictionary")
            exit(-1)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Update to the new animation names')
    parser.add_argument('file', help="source file to be processed. use . to process the whole repo", default = None)

    args = parser.parse_args()

    dictSanityCheck()

    if(args.file == '.'):
        replace_anim_all(os.curdir)
    else:
        replace_anim(args.file)
