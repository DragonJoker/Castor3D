#include "Castor3D/Render/Ssao/SsaoPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"
#include "Castor3D/Render/Ssao/SsaoBlurPass.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Ssao/SsaoRawAOPass.hpp"

#include <RenderGraph/FrameGraph.hpp>

CU_ImplementSmartPtr( castor3d, SsaoPass )

#define C3D_DebugRawPass 0

namespace castor3d
{
	SsaoPass::SsaoPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, ProgressBar * progress
		, crg::FramePassArray const & previousPasses
		, castor::Size const & size
		, SsaoConfig & ssaoConfig
		, Texture const & depthObj
		, Texture const & normal
		, CameraUbo const & cameraUbo )
		: m_device{ device }
		, m_ssaoConfig{ ssaoConfig }
		, m_group{ graph.createPassGroup( "SSAO" ) }
		, m_size{ makeExtent2D( size ) }
		, m_linearisePass{ castor::makeUnique< LineariseDepthPass >( *depthObj.resources
			, m_group
			, previousPasses
			, m_device
			, progress
			, cuT( "Ssao" )
			, m_ssaoConfig
			, m_size
			, depthObj ) }
		, m_ssaoConfigUbo{ m_device }
		, m_rawAoPass{ castor::makeUnique< SsaoRawAOPass >( m_group
			, m_device
			, progress
			, m_linearisePass->getLastPass()
			, m_size
			, m_ssaoConfig
			, m_ssaoConfigUbo
			, cameraUbo
			, m_linearisePass->getResult()
			, normal
			, m_passIndex ) }
#if !C3D_DebugRawPass
		, m_horizontalBlur{ castor::makeUnique< SsaoBlurPass >( m_group
			, m_device
			, progress
			, m_rawAoPass->getLastPass()
			, cuT( "Horizontal" )
			, m_size
			, m_ssaoConfig
			, m_ssaoConfigUbo
			, cameraUbo
			, castor::Point2i{ 1, 0 }
			, m_rawAoPass->getResult()
			, m_rawAoPass->getBentResult()
			, normal
			, m_passIndex ) }
		, m_verticalBlur{ castor::makeUnique< SsaoBlurPass >( m_group
			, m_device
			, progress
			, m_horizontalBlur->getLastPass()
			, cuT( "Vertical" )
			, m_size
			, m_ssaoConfig
			, m_ssaoConfigUbo
			, cameraUbo
			, castor::Point2i{ 0, 1 }
			, m_horizontalBlur->getResult()
			, m_horizontalBlur->getBentResult()
			, normal
			, m_passIndex ) }
		, m_lastPass{ &m_verticalBlur->getLastPass() }
#else
		, m_lastPass{ &m_rawAoPass->getLastPass() }
#endif
	{
	}

	void SsaoPass::update( CpuUpdater & updater )
	{
		m_linearisePass->update( updater );
		m_passIndex = m_ssaoConfig.useNormalsBuffer ? 1u : 0u;
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
