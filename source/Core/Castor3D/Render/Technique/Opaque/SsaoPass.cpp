#include "Castor3D/Render/Technique/Opaque/SsaoPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Technique/Opaque/GeometryPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/SsaoBlurPass.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/SsaoRawAOPass.hpp"

using namespace castor;
using namespace castor3d;

#define C3D_DebugRawPass 0

namespace castor3d
{
	SsaoPass::SsaoPass( Engine & engine
		, VkExtent2D const & size
		, SsaoConfig & ssaoConfig
		, TextureUnit const & linearisedDepth
		, GeometryPassResult const & gpResult )
		: m_engine{ engine }
		, m_ssaoConfig{ ssaoConfig }
		, m_matrixUbo{ engine }
		, m_ssaoConfigUbo{ std::make_shared< SsaoConfigUbo >( engine ) }
		, m_rawAoPass{ std::make_shared< SsaoRawAOPass >( engine
			, size
			, m_ssaoConfig
			, *m_ssaoConfigUbo
			, linearisedDepth
			, gpResult.getViews()[size_t( DsTexture::eData1 )] ) }
#if !C3D_DebugRawPass
		, m_horizontalBlur{ std::make_shared< SsaoBlurPass >( engine
			, cuT( "Horizontal" )
			, size
			, m_ssaoConfig
			, *m_ssaoConfigUbo
			, Point2i{ 1, 0 }
			, m_rawAoPass->getResult()
			, m_rawAoPass->getBentResult()
			, gpResult.getViews()[size_t( DsTexture::eData1 )] ) }
		, m_verticalBlur{ std::make_shared< SsaoBlurPass >( engine
			, cuT( "Vertical" )
			, size
			, m_ssaoConfig
			, *m_ssaoConfigUbo
			, Point2i{ 0, 1 }
			, m_horizontalBlur->getResult()
			, m_horizontalBlur->getBentResult()
			, gpResult.getViews()[size_t( DsTexture::eData1 )] ) }
#endif
	{
	}

	SsaoPass::~SsaoPass()
	{
		m_ssaoConfigUbo->cleanup();
		m_ssaoConfigUbo.reset();
	}

	void SsaoPass::update( Camera const & camera )
	{
		m_ssaoConfigUbo->update( m_ssaoConfig, camera );
#if !C3D_DebugRawPass
		m_horizontalBlur->update();
		m_verticalBlur->update();
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

	void SsaoPass::accept( RenderTechniqueVisitor & visitor )
	{
		m_rawAoPass->accept( m_ssaoConfig, visitor );
#if !C3D_DebugRawPass
		m_horizontalBlur->accept( true, m_ssaoConfig, visitor );
		m_verticalBlur->accept( false, m_ssaoConfig, visitor );
#endif
	}

	TextureUnit const & SsaoPass::getResult()const
	{
#if C3D_DebugRawPass
		return m_rawSsaoPass->getResult();
#else
		return m_verticalBlur->getResult();
#endif
	}

	TextureUnit const & SsaoPass::getBentNormals()const
	{
#if C3D_DebugRawPass
		return m_rawSsaoPass->getBentResult();
#else
		return m_rawAoPass->getBentResult();
#endif
	}
}
