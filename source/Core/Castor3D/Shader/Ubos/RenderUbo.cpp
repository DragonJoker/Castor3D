#include "Castor3D/Shader/Ubos/RenderUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace shader
	{
		sdw::Vec3 RenderData::removeGamma( sdw::Vec3 const & sRGB )const
		{
			return pow( max( sRGB, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( gamma() ) );
		}

		sdw::Vec3 RenderData::applyGamma( sdw::Vec3 const & hdr )const
		{
			return pow( max( hdr, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( 1.0_f / gamma() ) );
		}

		sdw::Vec2 RenderData::calcTexCoord( Utils & utils
			, sdw::Vec2 const & fragCoord )const
		{
			return utils.calcTexCoord( fragCoord
				, vec2( renderSize() ) );
		}
	}

	//*********************************************************************************************

	RenderUbo::RenderUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( MemoryPropertyFlags::eDeviceLocal ) }
	{
	}

	RenderUbo::~RenderUbo()noexcept
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void RenderUbo::cpuUpdate( RenderUbo const & parent
		, Size const & renderSize )
	{
		auto & parentData = parent.m_ubo.getData();
		auto & data = m_ubo.getData();
		data.renderSize->x = renderSize.getWidth();
		data.renderSize->y = renderSize.getHeight();
		data.invRenderSize->x = 1.0f / float( renderSize.getWidth() );
		data.invRenderSize->y = 1.0f / float( renderSize.getHeight() );
		data.exposure = parentData.exposure;
		data.gamma = parentData.gamma;
		data.debugIndex = parentData.debugIndex;
	}

	void RenderUbo::cpuUpdate( HdrConfig const & config
		, Size const & renderSize, bool safeBanded
		, uint32_t debugIndex )
	{
		CU_Require( m_ubo );
		auto size = ( safeBanded ? getSafeBandedSize( renderSize ) : renderSize );
		auto & data = m_ubo.getData();
		data.renderSize->x = size.getWidth();
		data.renderSize->y = size.getHeight();
		data.invRenderSize->x = 1.0f / float( size.getWidth() );
		data.invRenderSize->y = 1.0f / float( size.getHeight() );
		data.exposure = config.exposure;
		data.gamma = config.gamma;
		data.debugIndex = debugIndex;
	}
}
