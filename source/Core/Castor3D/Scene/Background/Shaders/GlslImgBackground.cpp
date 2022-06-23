#include "Castor3D/Scene/Background/Shaders/GlslImgBackground.hpp"

#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/Writer.hpp>

namespace castor3d::shader
{
	castor::String const ImgBackgroundModel::Name = cuT( "c3d.img" );

	ImgBackgroundModel::ImgBackgroundModel( sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize
		, uint32_t & binding
		, uint32_t set )
		: BackgroundModel{ writer, utils, std::move( targetSize ) }
	{
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapBackground"
			, binding++
			, set );
	}

	BackgroundModelPtr ImgBackgroundModel::create( sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize
		, uint32_t & binding
		, uint32_t set )
	{
		return std::make_unique< ImgBackgroundModel >( writer
			, utils
			, std::move( targetSize )
			, binding
			, set );
	}

	sdw::Vec3 ImgBackgroundModel::computeReflections( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, LightMaterial const & pmaterial
		, sdw::CombinedImage2DRg32 const & pbrdf )
	{
		return vec3( 0.0_f );
	}

	sdw::Vec3 ImgBackgroundModel::computeRefractions( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::Float const & prefractionRatio
		, sdw::Vec3 const & ptransmission
		, LightMaterial const & pmaterial )
	{
		return vec3( 0.0_f );
	}

	sdw::Void ImgBackgroundModel::mergeReflRefr( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::Float const & prefractionRatio
		, sdw::Vec3 const & ptransmission
		, LightMaterial const & pmaterial
		, sdw::Vec3 & preflection
		, sdw::Vec3 & prefraction )
	{
		return sdw::Void{};
	}
}
