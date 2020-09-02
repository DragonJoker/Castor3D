#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"

namespace castor3d
{
	uint32_t const GpInfoUbo::BindingPoint = 6u;
	const castor::String GpInfoUbo::GPInfo = cuT( "GPInfo" );
	const castor::String GpInfoUbo::InvViewProj = cuT( "c3d_mtxInvViewProj" );
	const castor::String GpInfoUbo::InvView = cuT( "c3d_mtxInvView" );
	const castor::String GpInfoUbo::InvProj = cuT( "c3d_mtxInvProj" );
	const castor::String GpInfoUbo::View = cuT( "c3d_mtxGView" );
	const castor::String GpInfoUbo::Proj = cuT( "c3d_mtxGProj" );
	const castor::String GpInfoUbo::RenderSize = cuT( "c3d_renderSize" );

	GpInfoUbo::GpInfoUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
		{
			initialise();
		}
	}

	GpInfoUbo::~GpInfoUbo()
	{
	}

	void GpInfoUbo::initialise()
	{
		if ( !m_ubo )
		{
			m_ubo = makeUniformBuffer< Configuration >( *m_engine.getRenderSystem()
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "GpInfoUbo" );
		}
	}

	void GpInfoUbo::cleanup()
	{
		m_ubo.reset();
	}

	void GpInfoUbo::gpuUpdate( castor::Size const & renderSize
		, Camera const & camera )
	{
		CU_Require( m_ubo );

		auto invView = camera.getView().getInverse();
		auto invProj = camera.getProjection().getInverse();
		auto invViewProj = ( camera.getProjection() * camera.getView() ).getInverse();

		auto & configuration = m_ubo->getData( 0u );
		configuration.invViewProj = invViewProj;
		configuration.invView = invView;
		configuration.invProj = invProj;
		configuration.gView = camera.getView();
		configuration.gProj = camera.getProjection();
		configuration.renderSize = castor::Point2f( renderSize.getWidth(), renderSize.getHeight() );
		m_ubo->upload();
	}

	//************************************************************************************************
}
