#include "DirectRenderTechnique.hpp"

#include <CameraCache.hpp>
#include <LightCache.hpp>
#include <MaterialCache.hpp>
#include <OverlayCache.hpp>
#include <SceneCache.hpp>
#include <TargetCache.hpp>

#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Scene.hpp>
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/OneFrameVariable.hpp>
#include <Shader/PointFrameVariable.hpp>
#include <State/DepthStencilState.hpp>

#include <Graphics/FontCache.hpp>
#include <Graphics/Image.hpp>
#include <Log/Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Direct
{
	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( cuT( "direct" ), p_renderTarget, p_renderSystem, p_params )
	{
		Logger::LogInfo( "Using Direct rendering" );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
	}

	bool RenderTechnique::DoCreate()
	{
		return true;
	}

	void RenderTechnique::DoDestroy()
	{
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		return true;
	}

	void RenderTechnique::DoCleanup()
	{
	}

	bool RenderTechnique::DoBeginRender( Scene & p_scene )
	{
		bool l_return = m_frameBuffer.m_frameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );

		if ( l_return )
		{
			m_frameBuffer.m_frameBuffer->SetClearColour( p_scene.GetBackgroundColour() );
			m_frameBuffer.m_frameBuffer->Clear();
		}

		return l_return;
	}

	void RenderTechnique::DoRender( SceneRenderNodes & p_nodes, Camera & p_camera, uint32_t p_frameTime )
	{
		m_renderTarget.GetDepthStencilState().Apply();
		Castor3D::RenderTechnique::DoRender( m_size, p_nodes, p_camera, p_frameTime );
	}

	void RenderTechnique::DoEndRender( Scene & p_scene )
	{
		m_frameBuffer.m_frameBuffer->Unbind();

#if DEBUG_BUFFERS

		uint8_t * l_buffer = m_frameBuffer.m_colourTexture->Lock( 0u, eACCESS_TYPE_READ );

		if ( l_buffer )
		{
			Path l_name{ Engine::GetEngineDirectory() };
			l_name /= cuT( "ColourBuffer_Technique_Unbind.png" );
			Image::BinaryWriter()( Image{ cuT( "tmp" )
										  , m_frameBuffer.m_colourTexture->GetImage().GetDimensions()
										  , PixelFormat::R8G8B8
										  , l_buffer
										  , m_frameBuffer.m_colourTexture->GetImage().GetPixelFormat() }
								   , l_name );
			m_frameBuffer.m_colourTexture->Unlock( 0u, false );
		}

#endif
	}

	bool RenderTechnique::DoWriteInto( TextFile & p_file )
	{
		return true;
	}
}
