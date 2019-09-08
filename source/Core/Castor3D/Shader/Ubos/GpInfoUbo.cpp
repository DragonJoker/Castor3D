#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"

#include <ashespp/Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const GpInfoUbo::BindingPoint = 6u;
	const String GpInfoUbo::GPInfo = cuT( "GPInfo" );
	const String GpInfoUbo::InvViewProj = cuT( "c3d_mtxInvViewProj" );
	const String GpInfoUbo::InvView = cuT( "c3d_mtxInvView" );
	const String GpInfoUbo::InvProj = cuT( "c3d_mtxInvProj" );
	const String GpInfoUbo::View = cuT( "c3d_mtxGView" );
	const String GpInfoUbo::Proj = cuT( "c3d_mtxGProj" );
	const String GpInfoUbo::RenderSize = cuT( "c3d_renderSize" );

	GpInfoUbo::GpInfoUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->getCurrentRenderDevice() )
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
			auto & device = getCurrentRenderDevice( m_engine );
			m_ubo = makeUniformBuffer< Configuration >( device
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

	void GpInfoUbo::update( Size const & renderSize
		, Camera const & camera
		, Matrix4x4r const & invViewProj
		, Matrix4x4r const & invView
		, Matrix4x4r const & invProj )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.invViewProj = invViewProj;
		configuration.invView = invView;
		configuration.invProj = invProj;
		configuration.gView = camera.getView();
		configuration.gProj = camera.getProjection();
		configuration.renderSize = Point2f( renderSize.getWidth(), renderSize.getHeight() );
		m_ubo->upload();
	}

	//************************************************************************************************
}
