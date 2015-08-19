#include "DirectRenderTechnique.hpp"
#include "RenderTarget.hpp"
#include "FrameBuffer.hpp"
#include "DepthStencilState.hpp"
#include "RasteriserState.hpp"
#include "RenderTarget.hpp"
#include "DepthStencilRenderBuffer.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	DirectRenderTechnique::DirectRenderTechnique()
		:	RenderTechniqueBase( cuT( "direct" ) )
	{
	}

	DirectRenderTechnique::DirectRenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
		:	RenderTechniqueBase( cuT( "direct" ), p_renderTarget, p_pRenderSystem, p_params )
	{
		Logger::LogInfo( "Using Direct rendering" );
	}

	DirectRenderTechnique::~DirectRenderTechnique()
	{
	}

	RenderTechniqueBaseSPtr DirectRenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueBaseSPtr( new DirectRenderTechnique( p_renderTarget, p_pRenderSystem, p_params ) );
	}

	bool DirectRenderTechnique::DoCreate()
	{
		return true;
	}

	void DirectRenderTechnique::DoDestroy()
	{
	}

	bool DirectRenderTechnique::DoInitialise( uint32_t & p_index )
	{
		return true;
	}

	void DirectRenderTechnique::DoCleanup()
	{
	}

	bool DirectRenderTechnique::DoBeginRender()
	{
		return m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
	}

	bool DirectRenderTechnique::DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
		m_pRenderTarget->GetDepthStencilState()->Apply();
		m_pRenderTarget->GetRasteriserState()->Apply();
		return RenderTechniqueBase::DoRender( p_scene, p_camera, p_ePrimitives, p_dFrameTime );
	}

	void DirectRenderTechnique::DoEndRender()
	{
		m_pFrameBuffer->Unbind();
	}
}
