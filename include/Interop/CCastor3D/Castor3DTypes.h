/*
See LICENSE file in root folder
*/
#ifndef ___CC3D_Types_H___
#define ___CC3D_Types_H___

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

	typedef char C3DChar;
	typedef C3DChar const * C3DString;

	typedef struct C3DFont_ C3DFont;
	typedef struct C3DGlyph_ C3DGlyph;
	typedef struct C3DImage_ C3DImage;
	typedef struct C3DLogger_ C3DLogger;
	typedef struct C3DPixelBuffer_ C3DPixelBuffer;
	typedef struct C3DRgbaColour_ C3DRgbaColour;
	typedef struct C3DRgbColour_ C3DRgbColour;
	typedef struct C3DHdrRgbaColour_ C3DHdrRgbaColour;
	typedef struct C3DHdrRgbColour_ C3DHdrRgbColour;
	typedef struct C3DVec2_ C3DVec2;
	typedef struct C3DVec3_ C3DVec3;
	typedef struct C3DQuat_ C3DQuat;
	typedef struct C3DMat4_ C3DMat4;
	typedef struct C3DPosition_ C3DPosition;
	typedef struct C3DSize_ C3DSize;

	typedef struct C3DBorderPanelOverlay_ C3DBorderPanelOverlay;
	typedef struct C3DCamera_ C3DCamera;
	typedef struct C3DDirectionalLight_ C3DDirectionalLight;
	typedef struct C3DEngine_ C3DEngine;
	typedef struct C3DGeometry_ C3DGeometry;
	typedef struct C3DGuiCallbacks_ C3DGuiCallbacks;
	typedef struct C3DLight_ C3DLight;
	typedef struct C3DLightGroup_ C3DLightGroup;
	typedef struct C3DLineMapping_ C3DLineMapping;
	typedef struct C3DMaterial_ C3DMaterial;
	typedef struct C3DMesh_ C3DMesh;
	typedef struct C3DMovableObject_ C3DMovableObject;
	typedef struct C3DOverlay_ C3DOverlay;
	typedef struct C3DPanelOverlay_ C3DPanelOverlay;
	typedef struct C3DPass_ C3DPass;
	typedef struct C3DPointLight_ C3DPointLight;
	typedef struct C3DRenderTarget_ C3DRenderTarget;
	typedef struct C3DRenderWindow_ C3DRenderWindow;
	typedef struct C3DSampler_ C3DSampler;
	typedef struct C3DScene_ C3DScene;
	typedef struct C3DSceneNode_ C3DSceneNode;
	typedef struct C3DShadow_ C3DShadow;
	typedef struct C3DSkybox_ C3DSkybox;
	typedef struct C3DSpotLight_ C3DSpotLight;
	typedef struct C3DSubmesh_ C3DSubmesh;
	typedef struct C3DTextOverlay_ C3DTextOverlay;
	typedef struct C3DTriFaceMapping_ C3DTriFaceMapping;
	typedef struct C3DEvent_ C3DEvent;
	
#if defined( _WIN32 )
	typedef struct C3DWin32WindowHandle_ C3DWindowHandle;
#elif defined( __linux__ )
	typedef struct C3DX11WindowHandle_ C3DWindowHandle;
#elif defined( __APPLE__ )
	typedef struct C3DMacOSWindowHandle_ C3DWindowHandle;
#endif


	struct C3DRgbaColour_
	{
		float r;
		float g;
		float b;
		float a;
	};

	struct C3DRgbColour_
	{
		float r;
		float g;
		float b;
	};

	struct C3DHdrRgbaColour_
	{
		float r;
		float g;
		float b;
		float a;
	};

	struct C3DHdrRgbColour_
	{
		float r;
		float g;
		float b;
	};

	struct C3DPosition_
	{
		int32_t x;
		int32_t y;
	};

	struct C3DSize_
	{
		uint32_t width;
		uint32_t height;
	};

	struct C3DVec2_
	{
		float x;
		float y;
	};

	struct C3DVec3_
	{
		float x;
		float y;
		float z;
	};

	struct C3DVector4D_
	{
		float x;
		float y;
		float z;
		float w;
	};

	typedef struct C3DQuat_
	{
		float x;
		float y;
		float z;
		float w;
	}	C3DQuat;

	struct C3DMat4_
	{
		// column 1
		float m11;
		float m12;
		float m13;
		float m14;
		// column 2
		float m21;
		float m22;
		float m23;
		float m24;
		// column 3
		float m31;
		float m32;
		float m33;
		float m34;
		// column 4
		float m41;
		float m42;
		float m43;
		float m44;
	};

	struct C3DGuiCallbacks_
	{
		void * userContext;
		void ( * onGetClipBoardText )( C3DGuiCallbacks * callbacks, C3DString * text );
		void ( * onSetClipBoardText )( C3DGuiCallbacks * callbacks, C3DString text );
		void ( * onCursorChange )( C3DGuiCallbacks * callbacks, C3D_MOUSE_CURSOR cursor );
	};

	struct C3DEvent_
	{
		C3D_EVENT_TYPE type;
		void ( *func )( void );
	};

#ifdef __cplusplus
}
#endif

#endif
