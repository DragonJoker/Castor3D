/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslIblBackgroundModel_H___
#define ___C3D_GlslIblBackgroundModel_H___

#include "Castor3D/Shader/Shaders/GlslBackground.hpp"

namespace c3d::shader
{
	class IblBackgroundModel
		: public BackgroundModel
	{
	public:
		C3D_API IblBackgroundModel( sdw::ShaderWriter & writer
			, Utils & utils
			, Extent2D targetSize
			, uint32_t & binding
			, uint32_t set );

		static BackgroundModelPtr create( Engine const & engine
			, sdw::ShaderWriter & writer
			, Utils & utils
			, Extent2D targetSize
			, bool needsForeground
			, uint32_t & binding
			, uint32_t set );

		sdw::RetVec3 computeDiffuseReflection( sdw::Vec3 const & wsNormal )override;
		sdw::RetVec3 computeSpecularReflection( sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, sdw::Float const & roughness )override;
		sdw::RetVec4 computeSheenReflection( sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, BlendComponents & components
			, sdw::CombinedImage2DRgba32 const & brdf )override;

		sdw::RetVec3 computeRefraction( sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, BlendComponents & components )override;
		sdw::RetVec3 computeSpecularRefraction( sdw::Vec3 const & fresnel
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, sdw::Float const & roughness
			, BlendComponents & components
			, sdw::CombinedImage2DRgba32 const & brdfMap
			, DebugOutputCategory const & debugOutput )override;

		sdw::RetVec3 computeDiffuse( sdw::Vec3 const & wsDirection
			, BlendComponents & components )override;

	public:
		static String const Name;

	private:
		sdw::Function< sdw::Void
			, InLightSurface
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImage2DRgba32
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeReflection;
		sdw::Function< sdw::Vec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat > m_computeRefraction;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32 > m_computeDiffuseReflection;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InCombinedImageCubeRgba32 > m_computeSpecularReflection;
		sdw::Function< sdw::Vec4
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImage2DRgba32 > m_computeSheenReflection;
		sdw::Function< sdw::Vec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InCombinedImage2DRgba32
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat > m_computeSpecularRefraction;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32 > m_computeDiffuse;
	};
}

#endif
