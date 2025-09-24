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

		uint32_t index{};
		for ( auto const & layerViews : m_downSampled )
		{
			visitor.visit( cuT( "PostFX: PBRB - Down " ) + c3d::string::toString( index )
				, layerViews.getSampledViewId()
				, m_graph.getFinalLayoutState( layerViews.getSampledViewId() ).layout
				, c3d::TextureFactors{}.invert( true ) );
			++index;
		}

		index = {};
		for ( auto const & layerViews : m_upSampled )
		{
			visitor.visit( cuT( "PostFX: PBRB - Up " ) + c3d::string::toString( index )
				, layerViews.getSampledViewId()
				, m_graph.getFinalLayoutState( layerViews.getSampledViewId() ).layout
				, c3d::TextureFactors{}.invert( true ) );
			++index;
		}
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
		, c3d::Texture & target )
	{
		m_ubo = device.uboPool->getBuffer< c3d::Point2f >( c3d::MemoryPropertyFlags::eNone );
		auto & data = m_ubo.getData();
		m_extent = target.getExtent();
		data->x = float( m_blurRadius ) / float( std::max( m_extent.width, m_extent.height ) );
		data->y = m_bloomStrength;
		auto extent = ashes::getSubresourceDimensions( convert( m_extent ), 1u );
		auto mipCount = ashes::getMaxMipCount( extent );
		m_duPassesCount = std::min( m_duPassesCount, mipCount );

		for ( uint32_t i = 0u; i < m_duPassesCount - 1u; ++i )
		{
			m_downSampled.emplace_back( device.renderSystem.getRenderDevice()
				, m_renderTarget.getResources()
				, "PBRBloomDownsample" + c3d::string::toString( i )
				, c3d::TextureCreateInfo{ c3d::ImageCreateFlags::eNone
				, c3d::convert( extent ), 1u, 1u
				, target.getFormat()
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled
					| c3d::ImageUsageFlags::eTransferSrc
					| c3d::ImageUsageFlags::eTransferDst ) }
				, c3d::TextureSamplerInfo{} );
			m_upSampled.emplace_back( device.renderSystem.getRenderDevice()
				, m_renderTarget.getResources()
				, "PBRBloomUpsample" + c3d::string::toString( i )
				, c3d::TextureCreateInfo{ c3d::ImageCreateFlags::eNone
				, c3d::convert( extent ), 1u, 1u
				, target.getFormat()
				, ( c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eSampled
					| c3d::ImageUsageFlags::eTransferSrc
					| c3d::ImageUsageFlags::eTransferDst ) }
				, c3d::TextureSamplerInfo{} );
			extent.width >>= 1u;
			extent.height >>= 1u;
		}

		m_downSampled.emplace_back( device.renderSystem.getRenderDevice()
			, m_renderTarget.getResources()
			, "PBRBloomDownsample" + c3d::string::toString( m_duPassesCount - 1u )
			, c3d::TextureCreateInfo{ c3d::ImageCreateFlags::eNone
			, c3d::convert( extent ), 1u, 1u
			, target.getFormat()
			, ( c3d::ImageUsageFlags::eColorAttachment
				| c3d::ImageUsageFlags::eSampled
				| c3d::ImageUsageFlags::eTransferSrc
				| c3d::ImageUsageFlags::eTransferDst ) }
			, c3d::TextureSamplerInfo{} );

		m_downsamplePass = c3d::makeRawUnique< DownsamplePass >( m_graph
			, device
			, source
			, m_downSampled
			, m_duPassesCount
			, &isEnabled()
			, &m_passIndex );
		m_upsamplePass = c3d::makeRawUnique< UpsamplePass >( m_graph
			, device
			, m_downSampled.back()
			, m_upSampled
			, m_ubo
			, m_duPassesCount
			, &isEnabled() );
		m_combinePass = c3d::makeRawUnique< CombinePass >( m_graph
			, device
			, m_upSampled.front()
			, source
			, target
			, c3d::makeExtent2D( m_extent )
			, m_ubo
			, &isEnabled()
			, &m_passIndex );

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
