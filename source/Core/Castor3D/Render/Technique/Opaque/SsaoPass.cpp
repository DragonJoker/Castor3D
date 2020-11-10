#include "Castor3D/Render/Technique/Opaque/SsaoPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/Ssao/SsaoBlurPass.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Ssao/SsaoRawAOPass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Shader/Ubos/SsaoConfigUbo.hpp"

using namespace castor;
using namespace castor3d;

#define C3D_DebugRawPass 0

namespace castor3d
{
	SsaoPass::SsaoPass( Engine & engine
		, castor::Size const & size
		, SsaoConfig & ssaoConfig
		, TextureUnit const & linearisedDepth
		, OpaquePassResult const & gpResult
		, GpInfoUbo const & gpInfoUbo )
		: m_engine{ engine }
		, m_ssaoConfig{ ssaoConfig }
		, m_linearisedDepth{ linearisedDepth }
		, m_gpResult{ gpResult }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_size{ makeExtent2D( size ) }
		, m_matrixUbo{ engine }
	{
	}

	void SsaoPass::initialise( RenderDevice const & device )
	{
		m_matrixUbo.initialise( device );
		m_ssaoConfigUbo = std::make_shared< SsaoConfigUbo >( m_engine );
		m_rawAoPass = std::make_shared< SsaoRawAOPass >( m_engine
			, device
			, m_size
			, m_ssaoConfig
			, *m_ssaoConfigUbo
			, m_gpInfoUbo
			, m_linearisedDepth
			, m_gpResult[DsTexture::eData1].getTexture()->getDefaultView().getSampledView() );
#if !C3D_DebugRawPass
		m_horizontalBlur = std::make_shared< SsaoBlurPass >( m_engine
			, device
			, cuT( "Horizontal" )
			, m_size
			, m_ssaoConfig
			, *m_ssaoConfigUbo
			, m_gpInfoUbo
			, Point2i{ 1, 0 }
			, m_rawAoPass->getResult()
			, m_rawAoPass->getBentResult()
			, m_gpResult[DsTexture::eData1].getTexture()->getDefaultView().getSampledView() );
		m_verticalBlur = std::make_shared< SsaoBlurPass >( m_engine
			, device
			, cuT( "Vertical" )
			, m_size
			, m_ssaoConfig
			, *m_ssaoConfigUbo
			, m_gpInfoUbo
			, Point2i{ 0, 1 }
			, m_horizontalBlur->getResult()
			, m_horizontalBlur->getBentResult()
			, m_gpResult[DsTexture::eData1].getTexture()->getDefaultView().getSampledView() );
#endif
	}

	void SsaoPass::cleanup( RenderDevice const & device )
	{
		m_verticalBlur.reset();
		m_horizontalBlur.reset();
		m_rawAoPass.reset();
		m_ssaoConfigUbo->cleanup( device );
		m_ssaoConfigUbo.reset();
		m_matrixUbo.cleanup( device );
	}

	void SsaoPass::update( CpuUpdater & updater )
	{
		m_ssaoConfigUbo->cpuUpdate( m_ssaoConfig, *updater.camera );
#if !C3D_DebugRawPass
		m_horizontalBlur->update( updater );
		m_verticalBlur->update( updater );
#endif
		m_ssaoConfig.blurRadius.reset();
	}

	ashes::Semaphore const & SsaoPass::render( ashes::Semaphore const & toWait )const
	{
		auto * result = &toWait;
		result = &m_rawAoPass->compute( *result );
#if !C3D_DebugRawPass
		result = &m_horizontalBlur->blur( *result );
		result = &m_verticalBlur->blur( *result );
#endif
		return *result;
	}

	void SsaoPass::accept( PipelineVisitorBase & visitor )
	{
		if ( m_rawAoPass )
		{
			m_rawAoPass->accept( m_ssaoConfig, visitor );
		}

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

	TextureUnit const & SsaoPass::getResult()const
	{
#if C3D_DebugRawPass
		return m_rawAoPass->getResult();
#else
		return m_verticalBlur->getResult();
#endif
	}

	TextureUnit const & SsaoPass::getBentNormals()const
	{
#if C3D_DebugRawPass
		return m_rawAoPass->getBentResult();
#else
		return m_rawAoPass->getBentResult();
#endif
	}
}
