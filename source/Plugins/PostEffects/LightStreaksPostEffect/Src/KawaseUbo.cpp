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
		: m_ubo{ KawaseUbo::Name
			, *engine.getRenderSystem()
			, KawaseUbo::BindingPoint }
		, m_pixelSize{ m_ubo.createUniform< UniformType::eVec2f >( KawaseUbo::PixelSize ) }
		, m_direction{ m_ubo.createUniform< UniformType::eVec2f >( KawaseUbo::Direction ) }
		, m_samples{ m_ubo.createUniform< UniformType::eInt >( KawaseUbo::Samples ) }
		, m_attenuation{ m_ubo.createUniform< UniformType::eFloat >( KawaseUbo::Attenuation ) }
		, m_pass{ m_ubo.createUniform< UniformType::eInt >( KawaseUbo::Pass ) }
	{
		m_samples->setValue( 4 );
		m_attenuation->setValue( 0.9f );
	}

	KawaseUbo::~KawaseUbo()
	{
		m_ubo.cleanup();
	}

	void KawaseUbo::update( Size const & size
		, Point2f const & direction
		, uint32_t pass )
	{
		Point2f pixelSize{ 1.0f / size.getWidth(), 1.0f / size.getHeight() };
		m_pixelSize->setValue( pixelSize );
		m_direction->setValue( direction );
		m_pass->setValue( int( pass ) );
		m_ubo.update();
		m_ubo.bindTo( KawaseUbo::BindingPoint );
	}

	//************************************************************************************************
}
