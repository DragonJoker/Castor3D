#include "GpInfoUbo.hpp"

#include <Engine.hpp>
#include <Scene/Camera.hpp>

using namespace Castor;

namespace Castor3D
{
	const String GpInfoUbo::GPInfo = cuT( "GPInfo" );
	const String GpInfoUbo::InvViewProj = cuT( "c3d_mtxInvViewProj" );
	const String GpInfoUbo::InvView = cuT( "c3d_mtxInvView" );
	const String GpInfoUbo::InvProj = cuT( "c3d_mtxInvProj" );
	const String GpInfoUbo::View = cuT( "c3d_mtxGView" );
	const String GpInfoUbo::Proj = cuT( "c3d_mtxGProj" );
	const String GpInfoUbo::RenderSize = cuT( "c3d_renderSize" );

	GpInfoUbo::GpInfoUbo( Engine & p_engine )
		: m_gpInfoUbo{ GpInfoUbo::GPInfo, *p_engine.GetRenderSystem() }
		, m_invViewProjUniform{ m_gpInfoUbo.CreateUniform< UniformType::eMat4x4f >( GpInfoUbo::InvViewProj ) }
		, m_invViewUniform{ m_gpInfoUbo.CreateUniform< UniformType::eMat4x4f >( GpInfoUbo::InvView ) }
		, m_invProjUniform{ m_gpInfoUbo.CreateUniform< UniformType::eMat4x4f >( GpInfoUbo::InvProj ) }
		, m_gViewUniform{ m_gpInfoUbo.CreateUniform< UniformType::eMat4x4f >( GpInfoUbo::View ) }
		, m_gProjUniform{ m_gpInfoUbo.CreateUniform< UniformType::eMat4x4f >( GpInfoUbo::Proj ) }
		, m_renderSize{ m_gpInfoUbo.CreateUniform< UniformType::eVec2f >( GpInfoUbo::RenderSize ) }
	{
	}

	GpInfoUbo::~GpInfoUbo()
	{
		m_gpInfoUbo.Cleanup();
	}

	void GpInfoUbo::Update( Size const & p_size
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj )
	{
		m_invViewProjUniform->SetValue( p_invViewProj );
		m_invViewUniform->SetValue( p_invView );
		m_invProjUniform->SetValue( p_invProj );
		m_gViewUniform->SetValue( p_camera.GetView() );
		m_gProjUniform->SetValue( p_camera.GetViewport().GetProjection() );
		m_renderSize->SetValue( Point2f( p_size.width(), p_size.height() ) );
		m_gpInfoUbo.Update();
		m_gpInfoUbo.BindTo( GpInfoUbo::BindingPoint );
	}

	//************************************************************************************************
}
