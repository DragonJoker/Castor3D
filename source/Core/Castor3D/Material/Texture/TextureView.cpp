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
		, uint32_t index )
		: OwnedBy< TextureLayout >{ layout }
		, m_index{ index }
		, m_info{ std::move( info ) }
	{
	}

	bool TextureView::initialise()
	{
		auto & renderSystem = *getOwner()->getRenderSystem();
		auto & device = getCurrentRenderDevice( *getOwner() );
		m_info->subresourceRange.levelCount = std::min( m_info->subresourceRange.levelCount
			, getOwner()->getTexture().getMipmapLevels() );
		m_info->image = getOwner()->getTexture();
		m_view = getOwner()->getTexture().createView( m_info );
		m_needsMipmapsGeneration = false;

		if ( m_source && m_source->isStatic() )
		{
			auto staging = device->createStagingTexture( m_source->getPixelFormat()
				, { m_source->getDimensions().width, m_source->getDimensions().height } );
			ashes::ImageViewCreateInfo viewInfo
			{
				0u,
				m_info->image,
				m_view->viewType,
				m_view->format,
				m_view->components,
				m_view->subresourceRange
			};
			viewInfo->subresourceRange.baseMipLevel = 0u;
			viewInfo->subresourceRange.levelCount = 1u;
			m_needsYInversion = false;

			for ( auto & buffer : m_source->getBuffers() )
			{
				auto view = m_view.image->createView( viewInfo );
				staging->uploadTextureData( *device.graphicsQueue
					, *device.graphicsCommandPool
					, m_source->getPixelFormat()
					, buffer->getConstPtr()
					, view );
				m_needsYInversion = m_needsYInversion || buffer->isFlipped();
				viewInfo->subresourceRange.baseMipLevel++;
			}

			m_needsMipmapsGeneration = getLevelCount() <= 1u
				|| ( getLevelCount() > 1
					&& viewInfo->subresourceRange.baseMipLevel < getLevelCount() );
		}
		else
		{
			m_needsMipmapsGeneration = m_info->subresourceRange.levelCount > 1u;
		}

		return m_view != nullptr;
	}

	void TextureView::cleanup()
	{
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

	void TextureView::doUpdate( ashes::ImageViewCreateInfo info )
	{
		m_info = info;

		if ( m_view )
		{
			m_view = getOwner()->getTexture().createView( m_info );
		}
	}
}
