#include "DirectRenderTechnique.hpp"

#include <RenderTarget.hpp>
#include <FrameBuffer.hpp>
#include <DepthStencilState.hpp>
#include <RasteriserState.hpp>
#include <RenderTarget.hpp>
#include <DepthStencilRenderBuffer.hpp>

#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Direct
{
	RenderTechnique::RenderTechnique()
		: RenderTechniqueBase( cuT( "direct" ) )
	{
	}

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
		: RenderTechniqueBase( cuT( "direct" ), p_renderTarget, p_pRenderSystem, p_params )
	{
		Logger::LogInfo( "Using Direct rendering" );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueBaseSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueBaseSPtr( new RenderTechnique( p_renderTarget, p_pRenderSystem, p_params ) );
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

	bool RenderTechnique::DoBeginRender()
	{
		return m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
	}

	bool RenderTechnique::DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
		m_pRenderTarget->GetDepthStencilState()->Apply();
		m_pRenderTarget->GetRasteriserState()->Apply();
		return RenderTechniqueBase::DoRender( p_scene, p_camera, p_ePrimitives, p_dFrameTime );
	}

	void RenderTechnique::DoEndRender()
	{
		m_pFrameBuffer->Unbind();
	}
}
