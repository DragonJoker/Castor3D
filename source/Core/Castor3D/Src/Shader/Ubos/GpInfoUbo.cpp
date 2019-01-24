#include "GpInfoUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const GpInfoUbo::BindingPoint = 5u;
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
		if ( engine.getRenderSystem()->hasCurrentDevice() )
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
			auto & device = getCurrentDevice( m_engine );
			m_ubo = ashes::makeUniformBuffer< Configuration >( device
				, 1u
				, ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eHostVisible );
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
