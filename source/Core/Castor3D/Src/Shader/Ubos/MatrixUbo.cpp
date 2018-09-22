#include "MatrixUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/StagingBuffer.hpp>
#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const MatrixUbo::BindingPoint = 2u;
	String const MatrixUbo::BufferMatrix = cuT( "Matrices" );
	String const MatrixUbo::Projection = cuT( "c3d_projection" );
	String const MatrixUbo::CurView = cuT( "c3d_curView" );
	String const MatrixUbo::PrvView = cuT( "c3d_prvView" );
	String const MatrixUbo::CurViewProj = cuT( "c3d_curViewProj" );
	String const MatrixUbo::PrvViewProj = cuT( "c3d_prvViewProj" );
	String const MatrixUbo::InvProjection = cuT( "c3d_invProjection" );
	String const MatrixUbo::Jitter = cuT( "c3d_jitter" );

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
			m_ubo = ashes::makeUniformBuffer< Configuration >( device
				, 1u
				, ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eHostVisible );
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
		configuration.curView = view;
		configuration.projection = projection;
		configuration.curViewProj = projection * view;
		configuration.invProjection = projection.getInverse();
		configuration.jitter = jitter;
		m_ubo->upload();
	}

	void MatrixUbo::update( Matrix4x4r const & view
		, Matrix4x4r const & projection
		, Point2r const & jitter
		, ashes::StagingBuffer & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer )const
	{
		REQUIRE( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.prvView = configuration.curView;
		configuration.prvViewProj = configuration.curViewProj;
		configuration.curView = view;
		configuration.projection = projection;
		configuration.curViewProj = projection * view;
		configuration.invProjection = projection.getInverse();
		configuration.jitter = jitter;
		stagingBuffer.uploadUniformData( commandBuffer
			, &configuration
			, 1u
			, 0u
			, *m_ubo );
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
