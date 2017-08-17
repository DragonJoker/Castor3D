#include "GpInfoUbo.hpp"

#include "Engine.hpp"
#include "Scene/Camera.hpp"

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
		: m_ubo{ GpInfoUbo::GPInfo
			, *engine.getRenderSystem()
			, GpInfoUbo::BindingPoint }
		, m_invViewProjUniform{ m_ubo.createUniform< UniformType::eMat4x4f >( GpInfoUbo::InvViewProj ) }
		, m_invViewUniform{ m_ubo.createUniform< UniformType::eMat4x4f >( GpInfoUbo::InvView ) }
		, m_invProjUniform{ m_ubo.createUniform< UniformType::eMat4x4f >( GpInfoUbo::InvProj ) }
		, m_gViewUniform{ m_ubo.createUniform< UniformType::eMat4x4f >( GpInfoUbo::View ) }
		, m_gProjUniform{ m_ubo.createUniform< UniformType::eMat4x4f >( GpInfoUbo::Proj ) }
		, m_renderSize{ m_ubo.createUniform< UniformType::eVec2f >( GpInfoUbo::RenderSize ) }
	{
	}

	GpInfoUbo::~GpInfoUbo()
	{
		m_ubo.cleanup();
	}

	void GpInfoUbo::update( Size const & p_size
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj )
	{
		m_invViewProjUniform->setValue( p_invViewProj );
		m_invViewUniform->setValue( p_invView );
		m_invProjUniform->setValue( p_invProj );
		m_gViewUniform->setValue( p_camera.getView() );
		m_gProjUniform->setValue( p_camera.getViewport().getProjection() );
		m_renderSize->setValue( Point2f( p_size.getWidth(), p_size.getHeight() ) );
		m_ubo.update();
		m_ubo.bindTo( GpInfoUbo::BindingPoint );
	}

	//************************************************************************************************
}
