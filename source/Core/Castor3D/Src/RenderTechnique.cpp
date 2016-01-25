#include "RenderTechnique.hpp"

#include "Camera.hpp"
#include "DynamicTexture.hpp"
#include "Engine.hpp"
#include "ColourRenderBuffer.hpp"
#include "DepthStencilRenderBuffer.hpp"
#include "FrameBuffer.hpp"
#include "OverlayManager.hpp"
#include "PostEffect.hpp"
#include "RasteriserState.hpp"
#include "RenderBufferAttachment.hpp"
#include "RenderSystem.hpp"
#include "RenderTarget.hpp"
#include "SamplerManager.hpp"
#include "Scene.hpp"
#include "TextureAttachment.hpp"

using namespace Castor;

namespace Castor3D
{
	RenderTechniqueBase::RenderTechniqueBase( String const & p_name, RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & CU_PARAM_UNUSED( p_params ) )
		: OwnedBy< Engine >( *p_renderSystem->GetOwner() )
		, m_renderTarget( &p_renderTarget )
		, m_renderSystem( p_renderSystem )
		, m_name( p_name )
		, m_initialised( false )
	{
	}

	RenderTechniqueBase::~RenderTechniqueBase()
	{
	}

	bool RenderTechniqueBase::Create()
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoCreate();
		}

		return l_return;
	}

	void RenderTechniqueBase::Destroy()
	{
		DoDestroy();
	}

	bool RenderTechniqueBase::Initialise( uint32_t & p_index )
	{
		if ( !m_initialised )
		{
			m_size = m_renderTarget->GetSize();
			m_initialised = DoInitialise( p_index );
		}

		return m_initialised;
	}

	void RenderTechniqueBase::Cleanup()
	{
		m_initialised = false;
		DoCleanup();
	}

	bool RenderTechniqueBase::BeginRender()
	{
		return DoBeginRender();
	}

	bool RenderTechniqueBase::Render( Scene & p_scene, Camera & p_camera, double p_dFrameTime )
	{
		m_renderSystem->PushScene( &p_scene );
		return DoRender( p_scene, p_camera, p_dFrameTime );
	}

	void RenderTechniqueBase::EndRender()
	{
		DoEndRender();

		for ( auto && l_effect : m_renderTarget->GetPostEffects() )
		{
			l_effect->Apply();
		}

		GetOwner()->GetOverlayManager().Render( *m_renderSystem->GetTopScene(), m_renderTarget->GetSize() );
		m_renderTarget->GetFrameBuffer()->Unbind();
		m_renderSystem->PopScene();
	}

	String RenderTechniqueBase::GetPixelShaderSource( uint32_t p_flags )const
	{
		return DoGetPixelShaderSource( p_flags );
	}

	bool RenderTechniqueBase::DoRender( Size const & p_size, Scene & p_scene, Camera & p_camera, double p_dFrameTime )
	{
		p_camera.GetViewport().SetSize( p_size );
		p_camera.Render();
		p_scene.Render( *this, p_camera );
		p_camera.EndRender();
		return true;
	}
}
