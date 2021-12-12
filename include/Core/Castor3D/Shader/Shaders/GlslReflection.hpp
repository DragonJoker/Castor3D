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
			, PassFlags const & passFlags );
		C3D_API ReflectionModel( sdw::ShaderWriter & writer
			, Utils & utils );
		C3D_API virtual ~ReflectionModel() = default;
		C3D_API virtual void computeDeferred( LightMaterial & material
			, Surface const & surface
			, SceneData const & sceneData
			, sdw::Int envMapIndex
			, sdw::Int const & reflection
			, sdw::Int const & refraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Vec3 & ambient
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted ) = 0;
		C3D_API virtual void computeForward( LightMaterial & material
			, Surface const & surface
			, SceneData const & sceneData
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Vec3 & ambient
			, sdw::Vec3 & reflected
			, sdw::Vec3 & refracted ) = 0;
		C3D_API sdw::Float computeFresnel( LightMaterial & material
			, Surface const & surface
			, SceneData const & sceneData
			, sdw::Float const & refractionRatio );

	protected:
		C3D_API sdw::Vec4 computeScreenSpace( Surface const & surface
			, SceneData const & sceneData
			, MatrixData const & matrixData
			, sdw::SampledImage2DR32 const & depthMap
			, sdw::SampledImage2DRgba32 const & normalsMap
			, sdw::SampledImage2DRgba32 const & colourMap
			, sdw::Vec2 const & texcoord
			, sdw::Vec4 const & ssrSettings );

		void declareComputeScreenSpace( MatrixData const & matrixData );
		void declareComputeFresnel();

	protected:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
		PassFlags m_passFlags;
		sdw::Function< sdw::Vec4
			, InSurface
			, sdw::InVec3
			, sdw::InSampledImage2DR32
			, sdw::InSampledImage2DRgba32
			, sdw::InSampledImage2DRgba32
			, sdw::InVec2
			, sdw::InVec4 > m_computeScreenSpace;
		sdw::Function< sdw::Float
			, InSurface
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat > m_computeFresnel;
	};
}

#endif
