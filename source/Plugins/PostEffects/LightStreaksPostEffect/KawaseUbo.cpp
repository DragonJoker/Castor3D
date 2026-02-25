#include "LightStreaksPostEffect/KawaseUbo.hpp"

#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

namespace light_streaks
{
	//*********************************************************************************************

	c3d::MbString const KawaseUbo::Buffer = "Kawase";
	c3d::MbString const KawaseUbo::Data = "c3d_kawaseData";

	KawaseUbo::KawaseUbo( c3d::RenderDevice const & device )
		: m_device{ device }
	{
		for ( uint32_t i = 0u; i < PostEffect::Count * 3u; ++i )
			m_ubo.emplace_back( device );
	}

	void KawaseUbo::update( uint32_t index
		, c3d::Extent2D const & size
		, c3d::Point2f const & direction
		, uint32_t pass )
	{
		c3d::Point2f pixelSize{ 1.0f / float( size.width )
			, 1.0f / float( size.height ) };
		auto data = m_ubo[index].getData();
		data.pixelSize = pixelSize;
		data.direction = direction;
		data.pass = int( pass );
		m_ubo[index].setData( c3d::move( data ) );
	}

	void KawaseUbo::update( KawaseConfig const & config )
	{
		for ( auto & ubo : m_ubo )
		{
			auto data = ubo.getData();
			data.samples = config.samples;
			data.attenuation = config.attenuation;
			ubo.setData( c3d::move( data ) );
		}
	}

	//************************************************************************************************
}
