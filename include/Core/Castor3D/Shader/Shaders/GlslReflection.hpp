/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslReflectionModel_H___
#define ___C3D_GlslReflectionModel_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace c3d::shader
{
	class ReflectionModel
	{
	public:
		C3D_API ReflectionModel( sdw::ShaderWriter & writer
			, Utils & utils
			, uint32_t & envMapBinding
			, uint32_t envMapSet
			, bool allowReflections
			, bool allowRefraction
			, bool allowIbl
			, bool hasEnvMap = true );

		C3D_API static sdw::RetVec3 computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera );
		C3D_API static RetDerivVec3 computeIncident( DerivVec3 const & wsPosition
			, sdw::Vec3 const & wsCamera );
		C3D_API static sdw::Vec3 getVolumeTransmissionRay( sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsIncident
			, sdw::Float const & thickness
			, sdw::Float const & ior );
		C3D_API static sdw::Vec3 applyVolumeAttenuation( sdw::Float const & transmissionDistance
			, sdw::Vec3 const & attenuationColor
			, sdw::Float const & attenuationDistance );
		C3D_API static sdw::Float applyIorToRoughness( sdw::Float const & roughness, sdw::Float const & ior );
		C3D_API void computeWithTransmission( BlendComponents const & components
			, LightSurface const & lightSurface
			, BackgroundModel & background
			, sdw::CombinedImage2DRgba32 const & mippedScene
			, CameraData const & camera
			, sdw::Vec2 const & sceneUv
			, sdw::UInt const & envMapIndex
			, ReflectionRefraction const & output
			, DebugOutputCategory const & debugOutput );
		C3D_API void computeWithTransmission( BlendComponents const & components
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, BackgroundModel & background
			, sdw::CombinedImage2DRgba32 const & mippedScene
			, CameraData const & camera
			, sdw::Vec2 const & sceneUv
			, sdw::UInt const & envMapIndex
			, ReflectionRefraction const & output
			, DebugOutputCategory const & debugOutput );
		C3D_API void computeWithoutTransmission( BlendComponents const & components
			, LightSurface const & lightSurface
			, BackgroundModel & background
			, sdw::UInt const & envMapIndex
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular );
		C3D_API sdw::Boolean computeWithoutTransmission( BlendComponents const & components
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, BackgroundModel & background
			, sdw::UInt envMapIndex
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular );
		C3D_API void computeWithoutTransmission( BlendComponents const & components
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, BackgroundModel & background
			, sdw::UInt const & envMapIndex
			, ReflectionRefraction const & output
			, DebugOutputCategory const & debugOutput );
		C3D_API void computeDiffuseBrdf( BlendComponents const & components
			, BackgroundModel & background
			, sdw::Vec3 const & reflectedDiffuse
			, sdw::Vec3 const & wsNormal
			, sdw::Boolean const & hasEnvMap
			, sdw::UInt const & envMapIndex
			, shader::ReflectionRefraction & output
			, DebugOutputCategory const & debugOutput );
		C3D_API sdw::Vec4 computeScreenSpace( CameraData const & cameraData
			, sdw::Vec3 const & viewPosition
			, sdw::Vec3 const & worldNormal
			, sdw::Vec2 const & texcoord
			, sdw::Vec4 const & ssrSettings
			, sdw::CombinedImage2DR32 const & depthMap
			, sdw::CombinedImage2DRgba32 const & normalMap
			, sdw::CombinedImage2DRgba32 const & colourMap
			, DebugOutputCategory const & debugOutput );
		C3D_API sdw::RetVec4 computeScreenSpace( CameraData const & cameraData
			, sdw::Vec3 const & viewPosition
			, sdw::Vec3 const & worldNormal
			, sdw::Vec2 const & texcoord
			, sdw::Vec4 const & ssrSettings
			, sdw::CombinedImage2DRgba32 const & depthObjMap
			, sdw::CombinedImage2DRgba32 const & normalMap
			, sdw::CombinedImage2DRgba32 const & colourMap
			, DebugOutputCategory const & debugOutput );
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
		C3D_API sdw::RetBoolean traceScreenSpace( sdw::Vec3 const & csOrigin
			, sdw::Vec3 const & csDirection
			, sdw::Mat4 const & projectToPixelMatrix
			, sdw::CombinedImage2DR32 const & csZBuffer
			, sdw::Vec2 const & csZBufferSize
			, sdw::Float const & csZThickness
			, sdw::Boolean const & csZBufferIsHyperbolic
			, sdw::Vec3 const & clipInfo
			, sdw::Float const & nearPlaneZ
			, sdw::Float const & stride
			, sdw::Float const & jitterFraction
			, sdw::Float const & maxSteps
			, sdw::Float const & maxRayTraceDistance
			, sdw::Vec2 const & hitPixel
			, sdw::Vec3 const & csHitPoint );

		Utils & getUtils()const
		{
			return m_utils;
		}

	private:
		sdw::RetVec3 computeSpecularReflEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & roughness
			, sdw::UInt const & envMapIndex
			, sdw::CombinedImageCubeArrayR11fG11fB10f const & envMap );
		sdw::RetVec4 computeSheenReflEnvMaps( sdw::CombinedImage2DRgba32 const & brdf
			, sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayR11fG11fB10f const & envMap
			, sdw::UInt const & envMapIndex
			, sdw::Float const & NdotV
			, BlendComponents const & components );
		sdw::RetVec3 computeRefrEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayR11fG11fB10f const & envMap
			, sdw::UInt const & envMapIndex
			, BlendComponents const & components );
		sdw::RetVec3 computeDiffuseEnvMaps( sdw::Vec3 const & wsDirection
			, sdw::CombinedImageCubeArrayR11fG11fB10f const & envMap
			, sdw::UInt const & envMapIndex
			, BlendComponents const & components );
		sdw::RetVec3 doComputeSpecularTransmission( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImage2DRgba32 const & sceneMap
			, CameraData const & camera
			, sdw::Vec2 const & sceneUv
			, BlendComponents const & components );
		sdw::RetVec3 doComputeRefrEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayR11fG11fB10f const & envMap
			, sdw::UInt const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & albedo
			, sdw::Float const & roughness );
		void doComputeReflection( sdw::CombinedImageCubeArrayR11fG11fB10f const & envMap
			, sdw::Boolean const & hasEnvMap
			, BackgroundModel & background
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, sdw::UInt const & envMapIndex
			, BlendComponents const & components
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular );
		void doComputeSpecularTransmission( sdw::CombinedImageCubeArrayR11fG11fB10f const & envMap
			, sdw::Boolean const & hasEnvMap
			, BackgroundModel & background
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, sdw::UInt const & envMapIndex
			, BlendComponents const & components
			, sdw::Vec3 & refracted );
		void doComputeDiffuse( sdw::CombinedImageCubeArrayR11fG11fB10f const & envMap
			, sdw::Boolean const & hasEnvMap
			, BackgroundModel & background
			, sdw::Vec3 const & wsDirection
			, sdw::UInt const & envMapIndex
			, BlendComponents const & components
			, sdw::Vec3 & result );
		void doComputeClearcoat( sdw::CombinedImageCubeArrayR11fG11fB10f const & envMap
			, sdw::Boolean const & hasEnvMap
			, BackgroundModel & background
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, sdw::UInt const & envMapIndex
			, BlendComponents const & components
			, sdw::Vec3 & coatReflected );
		void doComputeSheen( sdw::CombinedImage2DRgba32 const & brdf
			, sdw::CombinedImageCubeArrayR11fG11fB10f const & envMap
			, sdw::Boolean const & hasEnvMap
			, BackgroundModel & background
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, sdw::UInt const & envMapIndex
			, BlendComponents const & components
			, sdw::Vec4 & sheenReflected );

	private:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
		bool m_allowReflections;
		bool m_allowRefraction;
		bool m_allowIbl;
		bool m_hasEnvMap;
		sdw::Function< sdw::Void
			, InBlendComponents
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InCombinedImage2DRgba32
			, sdw::InVec2
			, sdw::InUInt
			, OutReflectionRefraction > m_computeWithTransmission;
		sdw::Function< sdw::Void
			, InBlendComponents
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InUInt
			, OutReflectionRefraction > m_computeWithoutTransmission;
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

		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InUInt
			, sdw::InFloat
			, sdw::InCombinedImageCubeArrayR11fG11fB10f > m_computeSpecularReflEnvMaps;
		sdw::Function< sdw::Vec4
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayR11fG11fB10f
			, sdw::InCombinedImage2DRgba32
			, sdw::InUInt
			, sdw::InFloat
			, sdw::InFloat > m_computeSheenReflEnvMaps;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayR11fG11fB10f
			, sdw::InUInt
			, sdw::InFloat
			, sdw::InFloat > m_computeRefrEnvMaps;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayR11fG11fB10f
			, sdw::InUInt
			, sdw::InFloat > m_computeDiffuseEnvMaps;
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
			, sdw::InFloat
			, sdw::InFloat > m_computeSpecularTransmission;
	};
}

#endif
