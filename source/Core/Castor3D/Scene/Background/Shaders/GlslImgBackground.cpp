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

	BackgroundModelPtr ImgBackgroundModel::create( Engine const & engine
		, sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize
		, bool needsForeground
		, uint32_t & binding
		, uint32_t set )
	{
		return std::make_unique< ImgBackgroundModel >( writer
			, utils
			, std::move( targetSize )
			, binding
			, set );
	}
}
