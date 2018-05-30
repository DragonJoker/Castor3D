#include "MatrixUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	String const MatrixUbo::BufferMatrix = cuT( "Matrices" );
	String const MatrixUbo::Projection = cuT( "c3d_projection" );
	String const MatrixUbo::CurView = cuT( "c3d_curView" );
	String const MatrixUbo::PrvView = cuT( "c3d_prvView" );
	String const MatrixUbo::CurViewProj = cuT( "c3d_curViewProj" );
	String const MatrixUbo::PrvViewProj = cuT( "c3d_prvViewProj" );
	String const MatrixUbo::InvProjection = cuT( "c3d_invProjection" );
	String const MatrixUbo::CurJitter = cuT( "c3d_curJitter" );
	String const MatrixUbo::PrvJitter = cuT( "c3d_prvJitter" );

	MatrixUbo::MatrixUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentDevice() )
		{
			initialise();
		}
	}

	MatrixUbo::~MatrixUbo()
	{
	}

	void MatrixUbo::initialise()
	{
		if ( !m_ubo )
		{
			auto & device = getCurrentDevice( m_engine );
			m_ubo = renderer::makeUniformBuffer< Configuration >( device
				, 1u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );
		}
	}

	void MatrixUbo::cleanup()
	{
		m_ubo.reset();
	}

	void MatrixUbo::update( Matrix4x4r const & view
		, Matrix4x4r const & projection
		, Point2r const & jitter )const
	{
		REQUIRE( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.prvView = configuration.curView;
		configuration.prvViewProj = configuration.curViewProj;
		configuration.prvJitter = configuration.curJitter;
		configuration.curView = view;
		configuration.projection = projection;
		configuration.curViewProj = projection * view;
		configuration.invProjection = projection.getInverse();
		configuration.curJitter = jitter;
		m_ubo->upload();
	}

	void MatrixUbo::update( Matrix4x4r const & projection )const
	{
		REQUIRE( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.projection = projection;
		configuration.invProjection = projection.getInverse();
		m_ubo->upload();
	}
}
