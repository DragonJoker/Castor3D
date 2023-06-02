/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslReflectionModel_H___
#define ___C3D_GlslReflectionModel_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d::shader
{
	class ReflectionModel
	{
	public:
		C3D_API ReflectionModel( sdw::ShaderWriter & writer
			, Utils & utils
			, uint32_t & envMapBinding
			, uint32_t envMapSet
			, bool hasIblSupport );

		C3D_API sdw::RetVec3 computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera )const;
		C3D_API void computeCombined( BlendComponents & components
			, LightSurface const & lightSurface
			, sdw::Vec3 const & position
			, BackgroundModel & background
			, sdw::CombinedImage2DRgba32 const & mippedScene
			, CameraData const & camera
			, sdw::Vec2 const & sceneUv
			, sdw::UInt const & envMapIndex
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, sdw::Vec3 & coatReflected
			, sdw::Vec3 & sheenReflected
			, DebugOutput & debugOutput );
		C3D_API void computeCombined( BlendComponents & components
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & difF
			, sdw::Vec3 const & spcF
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, sdw::Vec3 const & position
			, BackgroundModel & background
			, sdw::CombinedImage2DRgba32 const & mippedScene
			, CameraData const & camera
			, sdw::Vec2 const & sceneUv
			, sdw::UInt const & envMapIndex
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, sdw::Vec3 & coatReflected
			, sdw::Vec3 & sheenReflected
			, DebugOutput & debugOutput );
		C3D_API void computeCombined( BlendComponents & pcomponents
			, LightSurface const & lightSurface
			, BackgroundModel & background
			, sdw::UInt const & envMapIndex
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, sdw::Vec3& coatReflected
			, sdw::Vec3& sheenReflected
			, DebugOutput & debugOutput );
		C3D_API void computeCombined( BlendComponents & pcomponents
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & difF
			, sdw::Vec3 const & spcF
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, BackgroundModel & background
			, sdw::UInt const & envMapIndex
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, sdw::Vec3& coatReflected
			, sdw::Vec3& sheenReflected
			, DebugOutput & debugOutput );
		C3D_API void computeCombined( BlendComponents & pcomponents
			, LightSurface const & lightSurface
			, BackgroundModel & background
			, sdw::UInt const & envMapIndex
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, DebugOutput & debugOutput );
		C3D_API void computeCombined( BlendComponents & pcomponents
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & difF
			, sdw::Vec3 const & spcF
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, BackgroundModel & background
			, sdw::UInt const & envMapIndex
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, DebugOutput & debugOutput );
		C3D_API void computeReflections( BlendComponents & components
			, LightSurface const & lightSurface
			, BackgroundModel & background
			, sdw::UInt envMapIndex
			, sdw::UInt const & reflection
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, DebugOutput & debugOutput );
		C3D_API void computeReflections( BlendComponents & components
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & difF
			, sdw::Vec3 const & spcF
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, BackgroundModel & background
			, sdw::UInt envMapIndex
			, sdw::UInt const & reflection
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, DebugOutput & debugOutput );
		C3D_API sdw::Vec3 computeRefractions( BlendComponents & components
			, LightSurface const & lightSurface
			, BackgroundModel & background
			, sdw::UInt envMapIndex
			, sdw::UInt const & refraction
			, sdw::Float const & refractionRatio
			, DebugOutput & debugOutput );
		C3D_API sdw::Vec3 computeRefractions( BlendComponents & components
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & V
			, BackgroundModel & background
			, sdw::UInt envMapIndex
			, sdw::Float const & refractionRatio
			, DebugOutput & debugOutput );
		C3D_API sdw::Vec4 computeScreenSpace( CameraData const & cameraData
			, sdw::Vec3 const & viewPosition
			, sdw::Vec3 const & worldNormal
			, sdw::Vec2 const & texcoord
			, sdw::Vec4 const & ssrSettings
			, sdw::CombinedImage2DR32 const & depthMap
			, sdw::CombinedImage2DRgba32 const & normalMap
			, sdw::CombinedImage2DRgba32 const & colourMap
			, DebugOutput & debugOutput );
		C3D_API sdw::Vec4 computeScreenSpace( CameraData const & cameraData
			, sdw::Vec3 const & viewPosition
			, sdw::Vec3 const & worldNormal
			, sdw::Vec2 const & texcoord
			, sdw::Vec4 const & ssrSettings
			, sdw::CombinedImage2DRgba32 const & depthObjMap
			, sdw::CombinedImage2DRgba32 const & normalMap
			, sdw::CombinedImage2DRgba32 const & colourMap
			, DebugOutput & debugOutput );
		/**
		*\param csOrigin
		*	Camera-space ray origin, which must be within the view volume and must have z < -0.01 and project within the valid screen rectangle.
		*\param csDirection
		*	Unit length camera-space ray direction.
		*\param projectToPixelMatrix
		*	A projection matrix that maps to pixel coordinates (not [-1, +1] normalized device coordinates).
		*\param csZBuffer
		*	The depth or camera-space Z buffer, depending on the value of \a csZBufferIsHyperbolic.
		*\param csZBufferSize
		*	Dimensions of csZBuffer.
		*\param csZThickness
		*	Camera space thickness to ascribe to each pixel in the depth buffer.
		*\param csZBufferIsHyperbolic
		*	True if csZBuffer is an OpenGL depth buffer, false (faster) if csZBuffer contains (negative) "linear" camera space z values.
		*	Const so that the compiler can evaluate the branch based on it at compile time.
		*\param clipInfo
		*	See G3D::Camera documentation.
		*\param nearPlaneZ
		*	Negative number.
		*\param stride
		*	Step in horizontal or vertical pixels between samples.
		*	This is a float because integer math is slow on GPUs, but should be set to an integer >= 1.
		*\param jitterFraction
		*	Number between 0 and 1 for how far to bump the ray in stride units to conceal banding artifacts.
		*\param maxSteps
		*	Maximum number of iterations.
		*	Higher gives better images but may be slow.
		*\param maxRayTraceDistance
		*	Maximum camera-space distance to trace before returning a miss.
		*\param hitPixel
		*	Pixel coordinates of the first intersection with the scene.
		*\param csHitPoint
		*	Camera space location of the ray hit.
		*/
		C3D_API sdw::RetBoolean traceScreenSpace( sdw::Vec3 csOrigin
			, sdw::Vec3 csDirection
			, sdw::Mat4 projectToPixelMatrix
			, sdw::CombinedImage2DR32 csZBuffer
			, sdw::Vec2 csZBufferSize
			, sdw::Float csZThickness
			, sdw::Boolean csZBufferIsHyperbolic
			, sdw::Vec3 clipInfo
			, sdw::Float nearPlaneZ
			, sdw::Float stride
			, sdw::Float jitterFraction
			, sdw::Float maxSteps
			, sdw::Float maxRayTraceDistance
			, sdw::Vec2 & hitPixel
			, sdw::Vec3 & csHitPoint );

	private:
		sdw::RetVec3 computeSpecularReflEnvMaps( sdw::Vec3 const & fresnel
			, sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & roughness
			, sdw::UInt const & envMapIndex
			, sdw::Vec3 const & f0
			, sdw::Float const & NdotV
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::CombinedImage2DRgba32 const & brdfMap
			, BackgroundModel & background );
		sdw::RetVec3 computeSheenReflEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::UInt const & envMapIndex
			, BlendComponents & components );
		sdw::RetVec3 computeRefrEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::UInt const & envMapIndex
			, sdw::Float const & refractionRatio
			, BlendComponents & f0 );
		sdw::RetVec3 computeRefrSceneMap( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImage2DRgba32 const & sceneMap
			, CameraData const & camera
			, sdw::Vec2 sceneUv
			, sdw::Float const & refractionRatio
			, BlendComponents & components );
		sdw::RetVec3 doComputeRefrEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::UInt const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & albedo
			, sdw::Float const & roughness );
		void doComputeReflections( sdw::CombinedImage2DRgba32 const & brdf
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::Boolean const & hasEnvMap
			, BackgroundModel & background
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & difF
			, sdw::Vec3 const & spcF
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, sdw::UInt const & hasReflection
			, BlendComponents & components
			, sdw::UInt & envMapIndex
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular );
		void doComputeRefractions( sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::Boolean const & hasEnvMap
			, BackgroundModel & background
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & V
			, sdw::Float const & refractionRatio
			, sdw::UInt const & envMapIndex
			, BlendComponents & components
			, sdw::Vec3 & refracted );
		void doComputeClearcoatReflections( sdw::CombinedImage2DRgba32 const & brdf
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::Boolean const & hasEnvMap
			, BackgroundModel & background
			, sdw::Vec3 const & fresnel
			, sdw::Vec3 const & V
			, sdw::UInt const & hasReflection
			, sdw::Float const & NdotV
			, BlendComponents & components
			, sdw::UInt & envMapIndex
			, sdw::Vec3 & coatReflected );
		void doComputeSheenReflections( sdw::CombinedImage2DRgba32 const & brdf
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::Boolean const & hasEnvMap
			, BackgroundModel & background
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, sdw::UInt const & hasReflection
			, BlendComponents & components
			, sdw::UInt & envMapIndex
			, sdw::Vec3 & sheenReflected );

	private:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
		bool m_hasIblSupport;
		sdw::Function< sdw::Void
			, InOutBlendComponents
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InCombinedImage2DRgba32
			, sdw::InVec2
			, sdw::InUInt
			, sdw::InUInt
			, sdw::InUInt
			, sdw::InFloat
			, sdw::OutVec3
			, sdw::OutVec3
			, sdw::OutVec3
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeForwardSceneRefr;
		sdw::Function< sdw::Void
			, InOutBlendComponents
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InUInt
			, sdw::InUInt
			, sdw::InUInt
			, sdw::InFloat
			, sdw::OutVec3
			, sdw::OutVec3
			, sdw::OutVec3
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeForwardEnvRefr;
		sdw::Function< sdw::Void
			, InOutBlendComponents
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InUInt
			, sdw::InUInt
			, sdw::InUInt
			, sdw::InFloat
			, sdw::OutVec3
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeDeferred;

		sdw::Function< sdw::Boolean
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InMat4
			, sdw::InCombinedImage2DR32
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InBoolean
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::OutVec2
			, sdw::OutVec3 > m_traceScreenSpace;
		sdw::Function< sdw::Vec4
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec2
			, sdw::InVec4
			, sdw::InCombinedImage2DR32
			, sdw::InCombinedImage2DRgba32
			, sdw::InCombinedImage2DRgba32 > m_computeScreenSpace;
		sdw::Function< sdw::Vec4
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec2
			, sdw::InVec4
			, sdw::InCombinedImage2DRgba32
			, sdw::InCombinedImage2DRgba32
			, sdw::InCombinedImage2DRgba32 > m_computeScreenSpace2;

		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InUInt
			, sdw::InVec3
			, sdw::InFloat
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeReflEnvMaps;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InUInt
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InCombinedImage2DRgba32> m_computeSpecularReflEnvMaps;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InUInt
			, sdw::InFloat > m_computeSheenReflEnvMaps;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InUInt
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat > m_computeRefrEnvMaps;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImage2DRgba32
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat > m_computeRefrSceneMap;
	};
}

#endif
