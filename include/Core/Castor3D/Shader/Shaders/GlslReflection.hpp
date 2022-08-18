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
			, PassFlags const & passFlags
			, bool hasIblSupport );
		C3D_API ReflectionModel( sdw::ShaderWriter & writer
			, Utils & utils
			, bool hasIblSupport );
		C3D_API virtual ~ReflectionModel() = default;

		C3D_API void computeCombined( LightMaterial & material
			, Surface const & surface
			, SceneData const & sceneData
			, BackgroundModel & background
			, sdw::UInt envMapIndex
			, sdw::UInt const & reflection
			, sdw::UInt const & refraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Vec3 & ambient
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted );
		C3D_API sdw::Vec3 computeReflections( LightMaterial & material
			, Surface const & surface
			, SceneData const & sceneData
			, BackgroundModel & background
			, sdw::UInt envMapIndex
			, sdw::UInt const & reflection );
		C3D_API sdw::Vec3 computeRefractions( LightMaterial & material
			, Surface const & surface
			, SceneData const & sceneData
			, BackgroundModel & background
			, sdw::UInt envMapIndex
			, sdw::UInt const & refraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission );
		C3D_API sdw::RetFloat computeFresnel( LightMaterial & material
			, Surface const & surface
			, SceneData const & sceneData
			, sdw::Float const & refractionRatio );
		C3D_API sdw::RetVec4 computeScreenSpace( MatrixData const & matrixData
			, sdw::Vec3 const & viewPosition
			, sdw::Vec3 const & worldNormal
			, sdw::Vec2 const & texcoord
			, sdw::Vec4 const & ssrSettings
			, sdw::CombinedImage2DR32 const & depthMap
			, sdw::CombinedImage2DRgba32 const & normalMap
			, sdw::CombinedImage2DRgba32 const & colourMap );
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
		sdw::RetBoolean traceScreenSpace( sdw::Vec3 csOrigin
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
		sdw::RetVec3 computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera )const;
		sdw::RetVec3 computeReflEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::UInt const & envMapIndex
			, LightMaterial const & material );
		sdw::RetVec3 computeRefrEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::UInt const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, LightMaterial & material );
		sdw::RetVec3 mergeReflRefrEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::UInt const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, LightMaterial & material
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction );

		virtual void doAdjustAmbient( sdw::Vec3 & ambient )const
		{
		}

		virtual void doAdjustAlbedo( sdw::Vec3 & albedo )const
		{
		}

		void doDeclareMergeReflRefrEnvMaps();
		virtual void doDeclareComputeReflEnvMaps() = 0;
		virtual void doDeclareComputeRefrEnvMaps() = 0;

	protected:
		C3D_API void declareTraceScreenSpace();
		C3D_API void declareComputeScreenSpace( MatrixData const & matrixData );
		C3D_API void declareComputeFresnel();

		C3D_API sdw::RetVec3 doComputeRefrEnvMaps( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::CombinedImageCubeArrayRgba32 const & envMap
			, sdw::UInt const & envMapIndex
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Vec3 & albedo
			, sdw::Float const & roughness );

	protected:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
		PassFlags m_passFlags;
		bool m_hasIblSupport;

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
		sdw::Function< sdw::Float
			, InSurface
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat > m_computeFresnel;

		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InUInt
			, sdw::InVec3
			, sdw::InFloat > m_computeReflEnvMaps;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InUInt
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InOutVec3
			, sdw::InFloat > m_computeRefrEnvMaps;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayRgba32
			, sdw::InUInt
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InOutVec3
			, sdw::OutVec3 > m_mergeReflRefrEnvMaps;
	};
}

#endif
