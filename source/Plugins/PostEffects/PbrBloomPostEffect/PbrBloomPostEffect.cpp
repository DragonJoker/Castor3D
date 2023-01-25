#include "PbrBloomPostEffect/PbrBloomPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

namespace PbrBloom
{
	//*********************************************************************************************

	castor::String const PostEffect::Type = cuT( "pbr_bloom" );
	castor::String const PostEffect::Name = cuT( "PBR Bloom PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
		: castor3d::PostEffect{ PostEffect::Type
			, "PbrBloom"
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, params }
		, m_blurRadius{ BaseBlurRadius }
		, m_bloomStrength{ BaseBloomStrength }
		, m_duPassesCount{ BaseFilterCount }
	{
		setParameters( params );
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
	{
		return std::make_shared< PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		if ( m_downsamplePass )
		{
			m_downsamplePass->accept( visitor );
		}

		if ( m_upsamplePass )
		{
			m_upsamplePass->accept( visitor );
		}

		if ( m_combinePass )
		{
			m_combinePass->accept( visitor );
		}

		visitor.visit( cuT( "Blur Radius" )
			, m_blurRadius );
		visitor.visit( cuT( "Bloom Strength" )
			, m_bloomStrength );
	}

	void PostEffect::setParameters( castor3d::Parameters parameters )
	{
		castor::String count;

		if ( parameters.get( cuT( "bloomStrength" ), count ) )
		{
			m_bloomStrength = castor::string::toFloat( count );
		}

		if ( parameters.get( cuT( "blurRadius" ), count ) )
		{
			m_blurRadius = uint32_t( castor::string::toULong( count ) );
		}

		if ( parameters.get( cuT( "passes" ), count ) )
		{
			m_duPassesCount = uint32_t( castor::string::toLong( count ) );
		}

		m_passesCount = m_duPassesCount * 2u + 1u;
	}

	bool PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::Texture const & source
		, castor3d::Texture const & target
		, crg::FramePass const & previousPass )
	{
		m_ubo = device.uboPool->getBuffer< castor::Point2f >( 0u );
		auto & data = m_ubo.getData();
		m_extent = target.getExtent();
		data->x = float( m_blurRadius ) / float( std::max( m_extent.width, m_extent.height ) );
		data->y = m_bloomStrength;
		auto extent = ashes::getSubresourceDimensions( m_extent, 1u );
		auto mipCount = ashes::getMaxMipCount( extent );
		m_intermediateImg = m_graph.createImage( crg::ImageData{ "PBLInt"
			, 0u
			, VK_IMAGE_TYPE_2D
			, target.getFormat()
			, extent
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT )
			, mipCount } );

		m_duPassesCount = std::min( m_duPassesCount, mipCount );
		m_downsamplePass = std::make_unique< DownsamplePass >( m_graph
			, previousPass
			, device
			, crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, m_intermediateImg
			, m_duPassesCount
			, &isEnabled()
			, &m_passIndex );
		m_upsamplePass = std::make_unique< UpsamplePass >( m_graph
			, m_downsamplePass->getPass()
			, device
			, m_intermediateImg
			, m_ubo
			, m_duPassesCount
			, &isEnabled() );
		m_combinePass = std::make_unique< CombinePass >( m_graph
			, m_upsamplePass->getPass()
			, device
			, crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, m_intermediateImg
			, crg::ImageViewIdArray{ target.targetViewId, source.targetViewId }
			, m_ubo
			, &isEnabled()
			, &m_passIndex );

		m_pass = &m_combinePass->getPass();
		return true;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
		m_combinePass.reset();
		m_upsamplePass.reset();
		m_downsamplePass.reset();
	}

	void PostEffect::doCpuUpdate( castor3d::CpuUpdater & updater )
	{
		auto & data = m_ubo.getData();
		data->x = float( m_blurRadius ) / float( std::max( m_extent.width, m_extent.height ) );
		data->y = m_bloomStrength;
	}

	bool PostEffect::doWriteInto( castor::StringStream & file, castor::String const & tabs )
	{
		file << cuT( "\n" )
			<< ( tabs + cuT( "pbr_bloom\n" ) )
			<< ( tabs + cuT( "{\n" ) )
			<< ( tabs + cuT( "\t" ) ) << cuT( "blurRadius " ) << castor::string::toString( m_blurRadius ) << cuT( "\n" )
			<< ( tabs + cuT( "\t" ) ) << cuT( "bloomStrength " ) << castor::string::toString( m_bloomStrength ) << cuT( "\n" )
			<< ( tabs + cuT( "\t" ) ) << cuT( "passes " ) << castor::string::toString( m_duPassesCount ) << cuT( "\n" )
			<< ( tabs + cuT( "}\n" ) );
		return true;
	}
}
