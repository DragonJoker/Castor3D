#include "Castor3D/Render/Technique/SsaoPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"
#include "Castor3D/Render/Ssao/SsaoBlurPass.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Ssao/SsaoRawAOPass.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"

#include <RenderGraph/FrameGraph.hpp>

CU_ImplementCUSmartPtr( castor3d, SsaoPass )

#define C3D_DebugRawPass 0

namespace castor3d
{
	SsaoPass::SsaoPass( crg::FrameGraph & graph
		, RenderDevice const & device
		, ProgressBar * progress
		, crg::FramePass const & previousPass
		, castor::Size const & size
		, SsaoConfig & ssaoConfig
		, Texture const & depth
		, Texture const & normal
		, GpInfoUbo const & gpInfoUbo )
		: m_device{ device }
		, m_ssaoConfig{ ssaoConfig }
		, m_depth{ depth }
		, m_normal{ normal }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_size{ makeExtent2D( size ) }
		, m_matrixUbo{ m_device }
		, m_linearisePass{ castor::makeUnique< LineariseDepthPass >( graph
			, previousPass
			, m_device
			, progress
			, cuT( "Ssao" )
			, m_ssaoConfig
			, m_size
			, depth.sampledViewId ) }
		, m_ssaoConfigUbo{ m_device }
		, m_rawAoPass{ castor::makeUnique< SsaoRawAOPass >( graph
			, m_device
			, progress
			, m_linearisePass->getLastPass()
			, m_size
			, m_ssaoConfig
			, m_ssaoConfigUbo
			, m_gpInfoUbo
			, m_linearisePass->getResult()
			, m_normal ) }
#if !C3D_DebugRawPass
		, m_horizontalBlur{ castor::makeUnique< SsaoBlurPass >( graph
			, m_device
			, progress
			, m_rawAoPass->getLastPass()
			, cuT( "Horizontal" )
			, m_size
			, m_ssaoConfig
			, m_ssaoConfigUbo
			, m_gpInfoUbo
			, castor::Point2i{ 1, 0 }
			, m_rawAoPass->getResult()
			, m_rawAoPass->getBentResult()
			, m_normal ) }
		, m_verticalBlur{ castor::makeUnique< SsaoBlurPass >( graph
			, m_device
			, progress
			, m_horizontalBlur->getLastPass()
			, cuT( "Vertical" )
			, m_size
			, m_ssaoConfig
			, m_ssaoConfigUbo
			, m_gpInfoUbo
			, castor::Point2i{ 0, 1 }
			, m_horizontalBlur->getResult()
			, m_horizontalBlur->getBentResult()
			, m_normal ) }
		, m_lastPass{ &m_verticalBlur->getLastPass() }
#else
		, m_lastPass{ &m_rawAoPass->getLastPass() }
#endif
	{
	}

	void SsaoPass::update( CpuUpdater & updater )
	{
		m_linearisePass->update( updater );
		m_ssaoConfigUbo.cpuUpdate( m_ssaoConfig, *updater.camera );
#if !C3D_DebugRawPass
		m_horizontalBlur->update( updater );
		m_verticalBlur->update( updater );
#endif
		m_ssaoConfig.blurRadius.reset();
	}

	void SsaoPass::accept( PipelineVisitorBase & visitor )
	{
		m_linearisePass->accept( visitor );
		m_rawAoPass->accept( m_ssaoConfig, visitor );

#if !C3D_DebugRawPass
		if ( m_horizontalBlur )
		{
			m_horizontalBlur->accept( true, m_ssaoConfig, visitor );
		}

		if ( m_verticalBlur )
		{
			m_verticalBlur->accept( false, m_ssaoConfig, visitor );
		}
#endif
	}

	Texture const & SsaoPass::getResult()const
	{
#if C3D_DebugRawPass
		return m_rawAoPass->getResult();
#else
		return m_verticalBlur->getResult();
#endif
	}

	Texture const & SsaoPass::getBentNormals()const
	{
#if C3D_DebugRawPass
		return m_rawAoPass->getBentResult();
#else
		return m_rawAoPass->getBentResult();
#endif
	}
}
