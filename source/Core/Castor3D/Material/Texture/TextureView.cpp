#include "Castor3D/Material/Texture/TextureView.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureSource.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Graphics/PixelBufferBase.hpp>

#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Image/Image.hpp>

#include <CastorUtils/Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	TextureView::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< TextureView >{ tabs }
	{
	}

	bool TextureView::TextWriter::operator()( TextureView const & obj, TextFile & file )
	{
		return file.writeText( obj.m_source->toString() ) > 0;
	}

	//*********************************************************************************************

	TextureView::TextureView( TextureLayout & layout
		, ashes::ImageViewCreateInfo info
		, uint32_t index
		, castor::String debugName )
		: OwnedBy< TextureLayout >{ layout }
		, m_index{ index }
		, m_info{ std::move( info ) }
		, m_debugName{ std::move( debugName ) }
	{
	}

	bool TextureView::initialise()
	{
		auto & renderSystem = *getOwner()->getRenderSystem();
		auto & image = getOwner()->getTexture();
		auto & device = getCurrentRenderDevice( *getOwner() );
		m_info->subresourceRange.levelCount = std::min( m_info->subresourceRange.levelCount
			, image.getMipmapLevels() );
		m_info->image = image;
		m_needsMipmapsGeneration = false;

		if ( m_source && m_source->isStatic() )
		{
			auto staging = device->createStagingTexture( m_source->getPixelFormat()
				, { m_source->getDimensions().width, m_source->getDimensions().height } );
			ashes::ImageViewCreateInfo viewInfo
			{
				0u,
				m_info->image,
				m_info->viewType,
				m_info->format,
				m_info->components,
				m_info->subresourceRange
			};
			viewInfo->subresourceRange.baseMipLevel = 0u;
			viewInfo->subresourceRange.levelCount = 1u;
			m_needsYInversion = false;

			for ( auto & buffer : m_source->getBuffers() )
			{
				auto debugName = m_debugName + "Upload"
					+ "L(" + string::toString( viewInfo->subresourceRange.baseArrayLayer ) + "x" + string::toString( viewInfo->subresourceRange.layerCount ) + ")"
					+ "M(" + string::toString( viewInfo->subresourceRange.baseMipLevel ) + "x" + string::toString( viewInfo->subresourceRange.levelCount ) + ")";
				auto view = image.createView( debugName, viewInfo );
				staging->uploadTextureData( *device.graphicsQueue
					, *device.graphicsCommandPool
					, m_source->getPixelFormat()
					, buffer->getConstPtr()
					, view );
				m_needsYInversion = m_needsYInversion || buffer->isFlipped();
				viewInfo->subresourceRange.baseMipLevel++;
			}

			m_needsMipmapsGeneration = getLevelCount() <= 1u
				|| viewInfo->subresourceRange.baseMipLevel < getLevelCount();
		}
		else
		{
			m_needsMipmapsGeneration = m_info->subresourceRange.levelCount > 1u;
		}

		return true;
	}

	void TextureView::update( VkImage image
		, uint32_t baseArrayLayer
		, uint32_t layerCount
		, uint32_t baseMipLevel
		, uint32_t levelCount )
	{
		auto info = m_info;
		info->image = image;
		info->subresourceRange.baseArrayLayer = baseArrayLayer;
		info->subresourceRange.layerCount = layerCount;
		info->subresourceRange.baseMipLevel = baseMipLevel;
		info->subresourceRange.levelCount = levelCount;
		doUpdate( std::move( info ) );
	}

	void TextureView::cleanup()
	{
		m_sampledView = ashes::ImageView{};
		m_targetView = ashes::ImageView{};
	}

	void TextureView::initialiseSource( Path const & folder
		, Path const & relative )
	{
		m_source = TextureSource::create( *getOwner()->getRenderSystem()->getEngine()
			, folder
			, relative );
		m_info->format = m_source->getPixelFormat();
		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
	}

	void TextureView::initialiseSource( PxBufferBaseSPtr buffer )
	{
		m_source = TextureSource::create( *getOwner()->getRenderSystem()->getEngine()
			, buffer
			, getOwner()->getDepth() );
		m_info->format = m_source->getPixelFormat();
		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
	}

	void TextureView::initialiseSource()
	{
		m_source = TextureSource::create( *getOwner()->getRenderSystem()->getEngine()
			, VkExtent3D{ getOwner()->getWidth(), getOwner()->getHeight(), getOwner()->getDepth() }
			, getOwner()->getPixelFormat() );
		m_info->format = m_source->getPixelFormat();
		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
	}

	void TextureView::setBuffer( PxBufferBaseSPtr buffer )
	{
		m_source->setBuffer( buffer );
		m_info->format = m_source->getPixelFormat();
		getOwner()->doUpdateFromFirstImage( { m_source->getDimensions().width, m_source->getDimensions().height }
			, m_source->getPixelFormat() );
	}

	castor::String TextureView::toString()const
	{
		castor::String result;

		if ( m_source )
		{
			result = m_source->toString();
		}
		else
		{
			auto stream = castor::makeStringStream();
			stream << ashes::getName( m_info->format )
				<< cuT( "_" ) << m_info->subresourceRange.baseArrayLayer
				<< cuT( "_" ) << m_info->subresourceRange.layerCount
				<< cuT( "_" ) << m_info->subresourceRange.baseMipLevel
				<< cuT( "_" ) << m_info->subresourceRange.levelCount;
			result = stream.str();
		}

		return result;
	}

	castor::PxBufferBaseSPtr TextureView::getBuffer()const
	{
		return m_source->getBuffer();
	}

	castor::PxBufferPtrArray const & TextureView::getBuffers()const
	{
		return m_source->getBuffers();
	}

	castor::PxBufferPtrArray & TextureView::getBuffers()
	{
		return m_source->getBuffers();
	}

	bool TextureView::isStaticSource()const
	{
		return m_source->isStatic();
	}

	uint32_t TextureView::getLevelCount()const
	{
		return m_source->getLevelCount();
	}

	inline ashes::ImageView const & TextureView::getSampledView()const
	{
		if ( !m_sampledView )
		{
			CU_Require( m_info->image != VK_NULL_HANDLE );
			CU_Require( m_info->format != VK_FORMAT_UNDEFINED );
			auto & image = getOwner()->getTexture();
			auto debugName = m_debugName
				+ "L(" + string::toString( m_info->subresourceRange.baseArrayLayer ) + "x" + string::toString( m_info->subresourceRange.layerCount ) + ")"
				+ "M(" + string::toString( m_info->subresourceRange.baseMipLevel ) + "x" + string::toString( m_info->subresourceRange.levelCount ) + ")";
			auto createInfo = m_info;
			createInfo->subresourceRange.aspectMask = ( ( ashes::isDepthStencilFormat( m_info->format ) || ashes::isDepthFormat( m_info->format ) )
				? VK_IMAGE_ASPECT_DEPTH_BIT
				: ( ashes::isStencilFormat( m_info->format )
					? VK_IMAGE_ASPECT_STENCIL_BIT
					: createInfo->subresourceRange.aspectMask ) );
			m_sampledView = image.createView( debugName
				, createInfo );
		}

		return m_sampledView;
	}

	inline ashes::ImageView const & TextureView::getTargetView()const
	{
		if ( !m_targetView )
		{
			CU_Require( m_info->image != VK_NULL_HANDLE );
			CU_Require( m_info->format != VK_FORMAT_UNDEFINED );
			auto & image = getOwner()->getTexture();
			auto debugName = m_debugName
				+ "L(" + string::toString( m_info->subresourceRange.baseArrayLayer ) + "x" + string::toString( m_info->subresourceRange.layerCount ) + ")"
				+ "M(" + string::toString( m_info->subresourceRange.baseMipLevel ) + "x" + string::toString( m_info->subresourceRange.levelCount ) + ")";
			auto createInfo = m_info;

			if ( createInfo->viewType == VK_IMAGE_VIEW_TYPE_3D )
			{
				createInfo->viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				createInfo->subresourceRange.layerCount = image.getDimensions().depth;
			}

			m_targetView = image.createView( debugName
				, createInfo );
		}

		return m_targetView;
	}

	void TextureView::doUpdate( ashes::ImageViewCreateInfo info )
	{
		m_info = info;

		if ( m_sampledView )
		{
			m_sampledView = getOwner()->getTexture().createView( m_info );
		}

		if ( m_targetView )
		{
			m_targetView = getOwner()->getTexture().createView( m_info );
		}
	}
}
