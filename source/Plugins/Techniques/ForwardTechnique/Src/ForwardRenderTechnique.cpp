#include "ForwardRenderTechnique.hpp"

#include <Cache/CameraCache.hpp>
#include <Cache/LightCache.hpp>
#include <Cache/MaterialCache.hpp>
#include <Cache/OverlayCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/TargetCache.hpp>

#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Scene.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Technique/ForwardRenderTechniquePass.hpp>

#include <Graphics/FontCache.hpp>
#include <Graphics/Image.hpp>
#include <Log/Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace forward
{
	String const RenderTechnique::Type = cuT( "forward" );
	String const RenderTechnique::Name = cuT( "Forward Render Technique" );

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique{ RenderTechnique::Type
			, p_renderTarget
			, p_renderSystem
		, std::make_unique< ForwardRenderTechniquePass >( cuT( "forward_opaque" ), p_renderTarget, *this, true, false )
		, std::make_unique< ForwardRenderTechniquePass >( cuT( "forward_transparent" ), p_renderTarget, *this, false, false )
			, p_params }
	{
		Logger::LogInfo( "Using Forward rendering" );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		return true;
	}

	void RenderTechnique::DoCleanup()
	{
	}

	void RenderTechnique::DoRenderOpaque( uint32_t & p_visible )
	{
		m_opaquePass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		m_frameBuffer.m_frameBuffer->SetClearColour( m_renderTarget.GetScene()->GetBackgroundColour() );
		m_frameBuffer.m_frameBuffer->Clear();
		m_opaquePass->Render( p_visible, m_renderTarget.GetScene()->HasShadows() );
	}

	void RenderTechnique::DoRenderTransparent( uint32_t & p_visible )
	{
		m_frameBuffer.m_frameBuffer->Unbind();
		m_transparentPass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
		m_transparentPass->Render( p_visible, m_renderTarget.GetScene()->HasShadows() );
		m_frameBuffer.m_frameBuffer->Unbind();

#if DEBUG_BUFFERS

		uint8_t * l_buffer = m_frameBuffer.m_colourTexture->Lock( 0u, eACCESS_TYPE_READ );

		if ( l_buffer )
		{
			Path l_name{ Engine::GetEngineDirectory() };
			l_name /= cuT( "ColourBuffer_Technique_Unbind.png" );
			Image::BinaryWriter()( Image{ cuT( "tmp" )
				, m_frameBuffer.m_colourTexture->GetImage().GetDimensions()
				, PixelFormat::eR8G8B8
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
