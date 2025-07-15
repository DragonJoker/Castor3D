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

	c3d::String const PostEffect::Type = cuT( "pbr_bloom" );
	c3d::MbString const PostEffect::Name = "PBR Bloom PostEffect";

	PostEffect::PostEffect( c3d::RenderTarget & renderTarget
		, c3d::RenderSystem & renderSystem
		, c3d::Parameters const & params )
		: c3d::PostEffect{ PostEffect::Type
			, cuT( "PbrBloom" )
			, c3d::makeString( PostEffect::Name )
			, renderTarget
			, renderSystem
			, params }
		, m_blurRadius{ BaseBlurRadius }
		, m_bloomStrength{ BaseBloomStrength }
		, m_duPassesCount{ BaseFilterCount }
	{
		PostEffect::setParameters( params );
	}

	c3d::PostEffectUPtr PostEffect::create( c3d::RenderTarget & renderTarget
		, c3d::RenderSystem & renderSystem
		, c3d::Parameters const & params )
	{
		return c3d::makeUniqueDerived< c3d::PostEffect, PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::accept( c3d::ConfigurationVisitorBase & visitor )
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

	void PostEffect::setParameters( c3d::Parameters parameters )
	{
		c3d::String count;

		if ( parameters.get( cuT( "bloomStrength" ), count ) )
		{
			m_bloomStrength = c3d::string::toFloat( count );
		}

		if ( parameters.get( cuT( "blurRadius" ), count ) )
		{
			m_blurRadius = uint32_t( c3d::string::toULong( count ) );
		}

		if ( parameters.get( cuT( "passes" ), count ) )
		{
			m_duPassesCount = uint32_t( c3d::string::toLong( count ) );
		}

		m_passesCount = m_duPassesCount * 2u + 1u;
	}

	bool PostEffect::doInitialise( c3d::RenderDevice const & device
		, c3d::Texture const & source
		, c3d::Texture const & target
		, crg::FramePass const & previousPass )
	{
		m_ubo = device.uboPool->getBuffer< c3d::Point2f >( 0u );
		auto & data = m_ubo.getData();
		m_extent = target.getExtent();
		data->x = float( m_blurRadius ) / float( std::max( m_extent.width, m_extent.height ) );
		data->y = m_bloomStrength;
		auto extent = ashes::getSubresourceDimensions( convert( m_extent ), 1u );
		auto mipCount = ashes::getMaxMipCount( extent );
		m_intermediateImg = m_graph.createImage( crg::ImageData{ "PBLInt"
			, c3d::ImageCreateFlags::eNone
			, c3d::ImageType::e2D
			, target.getFormat()
			, c3d::convert( extent )
			, ( c3d::ImageUsageFlags::eColorAttachment
				| c3d::ImageUsageFlags::eSampled
				| c3d::ImageUsageFlags::eTransferSrc
				| c3d::ImageUsageFlags::eTransferDst )
			, mipCount } );

		m_duPassesCount = std::min( m_duPassesCount, mipCount );
		m_downsamplePass = c3d::makeRawUnique< DownsamplePass >( m_graph
			, previousPass
			, device
			, crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, m_intermediateImg
			, m_duPassesCount
			, &isEnabled()
			, &m_passIndex );
		m_upsamplePass = c3d::makeRawUnique< UpsamplePass >( m_graph
			, m_downsamplePass->getPass()
			, device
			, m_intermediateImg
			, m_ubo
			, m_duPassesCount
			, &isEnabled() );
		m_combinePass = c3d::makeRawUnique< CombinePass >( m_graph
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

	void PostEffect::doCleanup( c3d::RenderDevice const & device )
	{
		m_combinePass.reset();
		m_upsamplePass.reset();
		m_downsamplePass.reset();
		device.uboPool->putBuffer( m_ubo );
	}

	void PostEffect::doCpuUpdate( c3d::CpuUpdater & updater )
	{
		auto & data = m_ubo.getData();
		data->x = float( m_blurRadius ) / float( std::max( m_extent.width, m_extent.height ) );
		data->y = m_bloomStrength;
	}

	bool PostEffect::doWriteInto( c3d::StringStream & file, c3d::String const & tabs )
	{
		file << cuT( "\n" )
			<< ( tabs + cuT( "pbr_bloom\n" ) )
			<< ( tabs + cuT( "{\n" ) )
			<< ( tabs + cuT( "\t" ) ) << cuT( "blurRadius " ) << c3d::string::toString( m_blurRadius ) << cuT( "\n" )
			<< ( tabs + cuT( "\t" ) ) << cuT( "bloomStrength " ) << c3d::string::toString( m_bloomStrength ) << cuT( "\n" )
			<< ( tabs + cuT( "\t" ) ) << cuT( "passes " ) << c3d::string::toString( m_duPassesCount ) << cuT( "\n" )
			<< ( tabs + cuT( "}\n" ) );
		return true;
	}
}
