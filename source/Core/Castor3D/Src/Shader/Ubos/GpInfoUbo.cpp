#include "GpInfoUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
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
			auto & device = *m_engine.getRenderSystem()->getCurrentDevice();
			m_ubo = renderer::makeUniformBuffer< Configuration >( device
				, 1u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );
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
		REQUIRE( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.invViewProj = convert( invViewProj );
		configuration.invView = convert( invView );
		configuration.invProj = convert( invProj );
		configuration.gView = convert( camera.getView() );
		configuration.gProj = convert( camera.getViewport().getProjection() );
		configuration.renderSize = Point2f( renderSize.getWidth(), renderSize.getHeight() );
		m_ubo->upload();
	}

	//************************************************************************************************
}
