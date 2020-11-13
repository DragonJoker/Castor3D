/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugDefines_H___
#define ___C3D_DebugDefines_H___

#define C3D_UseWeightedBlendedRendering 1
#define C3D_UseDeferredRendering 0

#if C3D_UseDeferredRendering
#define C3D_UseDepthPrepass 1
#else
#define C3D_UseDepthPrepass 0
#endif

#define C3D_DebugPicking 0
#define C3D_DebugBackgroundPicking 0
#define C3D_DebugPickingTransfer 1

#endif
