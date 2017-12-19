#include "SsaoConfigUbo.hpp"

#include "Engine.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Render/RenderInfo.hpp"
#include "Render/Viewport.hpp"

using namespace castor;

namespace castor3d
{
	String const SsaoConfigUbo::BufferSsaoConfig = cuT( "SsaoConfig" );
	String const SsaoConfigUbo::ProjScale = cuT( "c3d_projScale" );
	String const SsaoConfigUbo::Radius = cuT( "c3d_radius" );
	String const SsaoConfigUbo::Bias = cuT( "c3d_bias" );
	String const SsaoConfigUbo::Intensity = cuT( "c3d_intensityDivR6" );
	String const SsaoConfigUbo::ProjInfo = cuT( "c3d_projInfo" );

	SsaoConfigUbo::SsaoConfigUbo( Engine & engine )
		: m_ubo{ SsaoConfigUbo::BufferSsaoConfig
			, *engine.getRenderSystem()
			, SsaoConfigUbo::BindingPoint }
		, m_projScale{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::ProjScale ) }
		, m_projInfo{ *m_ubo.createUniform< UniformType::eVec4f >( SsaoConfigUbo::ProjInfo ) }
		, m_radius{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::Radius ) }
		, m_bias{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::Bias ) }
		, m_intensity{ *m_ubo.createUniform< UniformType::eFloat >( SsaoConfigUbo::Intensity ) }
	{
	}

	SsaoConfigUbo::~SsaoConfigUbo()
	{
	}

	void SsaoConfigUbo::update( SsaoConfig const & config
		, Viewport const & viewport )
	{
		auto & proj = viewport.getProjection();
		const float scale = std::abs( 2.0f * ( viewport.getFovY() * 0.5f ).tan() );
		m_projScale.setValue( 500.0f );// viewport.getHeight() / scale );
		m_projInfo.setValue( Point4f{ -2.0f / ( viewport.getWidth() * proj[0][0] )
			, -2.0f / ( viewport.getHeight() * proj[1][1] )
			, ( 1.0f - proj[2][0] ) / proj[0][0]
			, ( 1.0f + proj[2][1] ) / proj[1][1] } );
		m_radius.setValue( config.m_radius );
		m_bias.setValue( config.m_bias );
		m_intensity.setValue( config.m_intensity / std::pow( config.m_radius, 6.0f ) );
		m_ubo.update();
		m_ubo.bindTo( SsaoConfigUbo::BindingPoint );
	}
}
