#include "LightStreaksPostEffect/KawaseUbo.hpp"

#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>

using namespace castor;
using namespace castor3d;

namespace light_streaks
{
	String const KawaseUbo::Name = cuT( "Kawase" );
	String const KawaseUbo::PixelSize = cuT( "c3d_pixelSize" );
	String const KawaseUbo::Direction = cuT( "c3d_direction" );
	String const KawaseUbo::Samples = cuT( "c3d_samples" );
	String const KawaseUbo::Attenuation = cuT( "c3d_attenuation" );
	String const KawaseUbo::Pass = cuT( "c3d_passCount" );

	KawaseUbo::KawaseUbo( Engine & engine )
		: m_engine{ engine }
	{
	}

	void KawaseUbo::initialise()
	{
		if ( m_ubo.empty() )
		{
			for ( uint32_t i = 0u; i < PostEffect::Count * 3u; ++i )
			{
				m_ubo.push_back( getCurrentRenderDevice( m_engine ).uboPools->getBuffer< Configuration >( 0u ) );
			}
		}
	}

	void KawaseUbo::cleanup()
	{
		for ( auto & ubo : m_ubo )
		{
			getCurrentRenderDevice( m_engine ).uboPools->putBuffer( ubo );
		}

		m_ubo.clear();
	}

	void KawaseUbo::update( uint32_t index
		, VkExtent2D const & size
		, castor::Point2f const & direction
		, uint32_t pass )
	{
		Point2f pixelSize{ 1.0f / size.width, 1.0f / size.height };
		auto & data = m_ubo[index].getData();
		data.samples = 4;
		data.attenuation = 0.9f;
		data.pixelSize = pixelSize;
		data.direction = direction;
		data.pass = int( pass );
	}

	//************************************************************************************************
}
