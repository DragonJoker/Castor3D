#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>

namespace castor3d
{
	uint32_t const MatrixUbo::BindingPoint = 3u;
	castor::String const MatrixUbo::BufferMatrix = cuT( "Matrices" );
	castor::String const MatrixUbo::Projection = cuT( "c3d_projection" );
	castor::String const MatrixUbo::CurView = cuT( "c3d_curView" );
	castor::String const MatrixUbo::PrvView = cuT( "c3d_prvView" );
	castor::String const MatrixUbo::CurViewProj = cuT( "c3d_curViewProj" );
	castor::String const MatrixUbo::PrvViewProj = cuT( "c3d_prvViewProj" );
	castor::String const MatrixUbo::InvProjection = cuT( "c3d_invProjection" );
	castor::String const MatrixUbo::Jitter = cuT( "c3d_jitter" );

	MatrixUbo::MatrixUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
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
			m_ubo = makeUniformBuffer< Configuration >( *m_engine.getRenderSystem()
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "MatrixUbo" );
		}
	}

	void MatrixUbo::cleanup()
	{
		m_ubo.reset();
	}

	void MatrixUbo::update( castor::Matrix4x4f const & view
		, castor::Matrix4x4f const & projection
		, castor::Point2f const & jitter )const
	{
		CU_Require( m_ubo );
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

	void MatrixUbo::update( castor::Matrix4x4f const & view
		, castor::Matrix4x4f const & projection
		, castor::Point2f const & jitter
		, ashes::StagingBuffer & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer )const
	{
		CU_Require( m_ubo );
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

	void MatrixUbo::update( castor::Matrix4x4f const & projection )const
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.projection = projection;
		configuration.invProjection = projection.getInverse();
		m_ubo->upload();
	}
}
