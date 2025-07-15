/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslNoIblBackgroundModel_H___
#define ___C3D_GlslNoIblBackgroundModel_H___

#include "Castor3D/Shader/Shaders/GlslBackground.hpp"

namespace c3d::shader
{
	class NoIblBackgroundModel
		: public BackgroundModel
	{
	public:
		C3D_API NoIblBackgroundModel( sdw::ShaderWriter & writer
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

		void computeReflection( sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, BlendComponents & components
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular )override;
		sdw::RetVec3 computeRefraction( sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, BlendComponents & components )override;

		sdw::RetVec3 computeDiffuse( sdw::Vec3 const & wsDirection
			, BlendComponents & components )override;

	public:
		static String const Name;

	private:
		sdw::Function< sdw::Void
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::OutVec3
			, sdw::OutVec3 > m_computeReflection;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat
			, sdw::InFloat > m_computeRefraction;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InCombinedImageCubeRgba32
			, sdw::InFloat > m_computeDiffuse;
	};
}

#endif
