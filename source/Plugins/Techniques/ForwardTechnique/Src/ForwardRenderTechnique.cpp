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

#define DEBUG_BUFFERS 1

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

	void RenderTechnique::DoRenderOpaque( RenderInfo & p_info )
	{
		m_opaquePass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_frameBuffer->SetClearColour( m_renderTarget.GetScene()->GetBackgroundColour() );
		m_frameBuffer.m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_opaquePass->Render( p_info, m_renderTarget.GetScene()->HasShadows() );
	}

	void RenderTechnique::DoRenderTransparent( RenderInfo & p_info )
	{
		m_frameBuffer.m_frameBuffer->Unbind();
		m_transparentPass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_transparentPass->Render( p_info, m_renderTarget.GetScene()->HasShadows() );
		m_frameBuffer.m_frameBuffer->Unbind();

#if DEBUG_BUFFERS && !defined( NDEBUG )

		int l_width = int( m_size.width() ) / 6;
		int l_height = int( m_size.height() ) / 6;
		int l_left = int( m_size.width() ) - l_width;
		auto l_size = Size( l_width, l_height );
		auto & l_context = *m_renderSystem.GetCurrentContext();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind();
		l_context.RenderDepth( Position{ l_width * 0, 0 }, l_size, *m_frameBuffer.m_depthBuffer );
		m_frameBuffer.m_frameBuffer->Unbind();

#endif
	}

	bool RenderTechnique::DoWriteInto( TextFile & p_file )
	{
		return true;
	}
}
