#include "KawaseUbo.hpp"

#include <Engine.hpp>

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
		: m_ubo{ renderer::makeUniformBuffer< Configuration >( *engine.getRenderSystem()->getCurrentDevice()
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible ) }
	{
	}

	KawaseUbo::~KawaseUbo()
	{
	}

	void KawaseUbo::update( Size const & size
		, Point2f const & direction
		, uint32_t pass )
	{
		Point2f pixelSize{ 1.0f / size.getWidth(), 1.0f / size.getHeight() };
		auto & data = m_ubo->getData();
		data.pixelSize = pixelSize;
		data.direction = direction;
		data.pass = int( pass );
		m_ubo->upload();
	}

	//************************************************************************************************
}
