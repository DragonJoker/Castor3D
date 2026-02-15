/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Upscale/UpscaleWrapper.hpp"

#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Upscale/UpscaleBlit.hpp"
#include "Castor3D/Render/Upscale/UpscaleDLSS.hpp"

CU_ImplementSmartPtr( c3d, UpscalingWrapper )
CU_ImplementSmartPtr( c3d, UpscalingInstance )

namespace c3d
{
	//*********************************************************************************************

	UpscalingInstance::UpscalingInstance( UpscalingWrapper const & parent
		, RenderTarget & target
		, Extent2D const & renderSize
		, Extent2D const & displaySize
		, UpscaleConfig const & config )
		: m_sdk{ parent.m_sdk->createInstance() }
		, m_target{ target }
	{
		m_sdk->initialise( renderSize, displaySize, config );
	}

	UpscalingInstance::~UpscalingInstance()noexcept
	{
		m_sdk->cleanup();
		m_sdk.reset();
	}

	void UpscalingInstance::update()
	{
		m_target.setJitter( m_sdk->getJitter( m_frameIndex ) );
	}

	void UpscalingInstance::evaluateUpscaling( crg::RecordContext & recContext
		, VkCommandBuffer commandBuffer
		, crg::ImageViewId resolvedColor
		, crg::ImageViewId unresolvedColor
		, crg::ImageViewId motionVectors
		, crg::ImageViewId depth
		, bool resetAccumulation
		, Point2f const & jitterOffset
		, Point2f const & mVScale )const
	{
		m_sdk->evaluate( recContext, commandBuffer
			, resolvedColor, unresolvedColor, motionVectors, depth
			, resetAccumulation, jitterOffset, mVScale );
	}

	//*********************************************************************************************

	UpscalingWrapper::UpscalingWrapper( RenderDevice const & device )
	{
#if C3D_UseDLSS
		m_sdk = makeRawUnique< DLSSUpscalingSDK >();
		if ( !m_sdk->initialise( device ) )
#endif
		{
			m_sdk = makeRawUnique< BlitUpscalingSDK >();
			m_sdk->initialise( device );
		}
	}

	UpscalingWrapper::~UpscalingWrapper()noexcept
	{
		m_sdk->cleanup();
		m_sdk.reset();
	}

	bool UpscalingWrapper::queryOptimalSettings( Extent2D const & displaySize
		, UpscaleConfig const & config
		, Extent2D & recommendedSize )const
	{
		if ( !config.enabled )
		{
			recommendedSize = displaySize;
			return true;
		}

		return m_sdk->queryOptimalSettings( displaySize, config
			, recommendedSize );
	}

	UpscalingInstanceUPtr UpscalingWrapper::createInstance( RenderTarget & target
		, Extent2D const & renderSize
		, Extent2D const & displaySize
		, UpscaleConfig const & config )const
	{
		return makeUnique< UpscalingInstance >( *this, target
			, renderSize, displaySize, config );
	}

	//*********************************************************************************************
}
