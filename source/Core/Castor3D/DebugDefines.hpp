/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugDefines_H___
#define ___C3D_DebugDefines_H___

#define C3D_DisableSSSTransmittance 0
#define C3D_MeasureShadowMapImpact 0

#define C3D_DebugPicking 0
#define C3D_DebugBackgroundPicking 0
#define C3D_DebugPickingTransfer 0
#define C3D_DebugTimers 0
#define C3D_DebugDisableShadowMaps 0
#define C3D_DebugDisableSafeBands 0
#define C3D_DebugUpload 0

#if C3D_DebugTimers
#	define C3D_DebugTime( x ) CU_TimeEx( x )
#else
#	define C3D_DebugTime( x )
#endif

#endif
