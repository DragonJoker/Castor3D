#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
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
		if ( m_engine.getRenderSystem()->hasCurrentRenderDevice() )
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
			auto & device = getCurrentRenderDevice( m_engine );
			m_ubo = device.uboPools->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}
	}

	void MatrixUbo::cleanup()
	{
		if ( m_ubo )
		{
			auto & device = getCurrentRenderDevice( m_engine );
			device.uboPools->putBuffer( m_ubo );
		}
	}

	void MatrixUbo::cpuUpdate( castor::Matrix4x4f const & view
		, castor::Matrix4x4f const & projection
		, castor::Point2f const & jitter )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		configuration.prvView = configuration.curView;
		configuration.prvViewProj = configuration.curViewProj;
		configuration.curView = view;
		configuration.projection = projection;
		configuration.curViewProj = projection * view;
		configuration.invProjection = projection.getInverse();
		configuration.jitter = jitter;
	}

	void MatrixUbo::cpuUpdate( castor::Matrix4x4f const & projection )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		configuration.projection = projection;
		configuration.invProjection = projection.getInverse();
	}
}
