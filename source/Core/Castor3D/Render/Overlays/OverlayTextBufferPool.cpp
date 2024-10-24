/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Overlays/OverlayTextBufferPool.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Shader/Ubos/FontUbo.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>

CU_ImplementSmartPtr( castor3d, OverlayTextBufferPool )

namespace castor3d
{
	//*************************************************************************

	namespace txtbufpool
	{
		template< typename DataT >
		OverlayTextBuffer::DataBufferT< DataT > makeDataBuffer( RenderDevice const & device
			, uint32_t count
			, castor::String const & name )
		{
			auto buffer = makeBuffer< DataT >( device
				, count
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, name );
			auto data = castor::makeArrayView( buffer->lock( 0u, ashes::WholeSize, 0u )
				, buffer->getCount() );
			return { castor::move( buffer ), data };
		}
	}

	//*************************************************************************

	OverlayTextBuffer::OverlayTextBuffer( Engine & engine
		, castor::String const & debugName
		, RenderDevice const & device )
		: engine{ engine }
		, device{ device }
		, name{ debugName }
		, charsBuffer{ txtbufpool::makeDataBuffer< TextChar >( device
			, MaxCharsPerBuffer
			, name + cuT( "-CharsData" ) ) }
		, wordsBuffer{ txtbufpool::makeDataBuffer< TextWord >( device
			, MaxWordsPerBuffer
			, name + cuT( "-WordsData" ) ) }
		, linesBuffer{ txtbufpool::makeDataBuffer< TextLine >( device
			, MaxLinesPerBuffer
			, name + cuT( "-LinesData" ) ) }
	{
	}

	OverlayTextBufferIndex OverlayTextBuffer::fill( uint32_t overlayIndex
		, TextOverlay const & overlay )noexcept
	{
		auto chars = overlay.getCharCount();
		auto words = overlay.getWordCount();
		auto lines = overlay.getLineCount();
		OverlayTextBufferIndex result{ this
			, wordsBuffer.allocated
			, linesBuffer.allocated };

		if ( !chars
			|| charsBuffer.allocated >= ( MaxCharsPerBuffer - chars )
			|| wordsBuffer.allocated >= ( MaxWordsPerBuffer - words )
			|| linesBuffer.allocated >= ( MaxLinesPerBuffer - lines ) )
		{
			if ( chars )
			{
				CU_Failure( ": Couldn't allocate overlay" );
				log::warn << name << ": Couldn't allocate overlay";
			}

			return result;
		}

		result.top = overlay.fillBuffer( overlayIndex
			, castor::makeArrayView( charsBuffer.data.begin() + charsBuffer.allocated, chars )
			, castor::makeArrayView( wordsBuffer.data.begin() + wordsBuffer.allocated, words )
			, castor::makeArrayView( linesBuffer.data.begin() + linesBuffer.allocated, lines ) );
		charsBuffer.allocated += chars;
		wordsBuffer.allocated += words;
		linesBuffer.allocated += lines;

		return result;
	}

	void OverlayTextBuffer::upload( UploadData & uploader )
	{
		charsBuffer.allocated = 0u;
		wordsBuffer.allocated = 0u;
		linesBuffer.allocated = 0u;
	}

	void OverlayTextBuffer::fillDescriptorSet( ashes::DescriptorSetLayout const & descriptorLayout
		, ashes::DescriptorSet & descriptorSet )const
	{
		descriptorSet.createBinding( descriptorLayout.getBinding( uint32_t( TextOverlay::ComputeBindingIdx::eChars ) )
			, *charsBuffer.buffer
			, 0u
			, uint32_t( charsBuffer.buffer->getCount() ) );
		descriptorSet.createBinding( descriptorLayout.getBinding( uint32_t( TextOverlay::ComputeBindingIdx::eWords ) )
			, *wordsBuffer.buffer
			, 0u
			, uint32_t( wordsBuffer.buffer->getCount() ) );
		descriptorSet.createBinding( descriptorLayout.getBinding( uint32_t( TextOverlay::ComputeBindingIdx::eLines ) )
			, *linesBuffer.buffer
			, 0u
			, uint32_t( linesBuffer.buffer->getCount() ) );
	}

	//*************************************************************************

	OverlayTextBufferPool::OverlayTextBufferPool( Engine & engine
		, castor::String const & debugName
		, RenderDevice const & device )
		: m_engine{ engine }
		, m_device{ device }
		, m_name{ debugName }
	{
	}

	void OverlayTextBufferPool::fillDescriptorSet( FontTexture const * fontTexture
		, ashes::DescriptorSetLayout const & descriptorLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		auto it = m_buffers.emplace( fontTexture, nullptr ).first;

		if ( !it->second )
		{
			it->second = castor::make_unique< OverlayTextBuffer >( m_engine
				, m_name + ( fontTexture ? cuT( "-" ) + fontTexture->getFontName() : castor::String{} )
				, m_device );
		}

		it->second->fillDescriptorSet( descriptorLayout, descriptorSet );
		fontTexture->getFontUbo().createSizedBinding( descriptorSet
			, descriptorLayout.getBinding( uint32_t( TextOverlay::ComputeBindingIdx::eFont ) ) );
	}

	OverlayTextBufferIndex OverlayTextBufferPool::fill( uint32_t overlayIndex
		, FontTexture const * fontTexture
		, TextOverlay const & overlay )noexcept
	{
		OverlayTextBufferIndex result{};

		try
		{
			auto it = m_buffers.emplace( fontTexture, nullptr ).first;

			if ( !it->second )
			{
				it->second = castor::make_unique< OverlayTextBuffer >( m_engine
					, m_name + ( fontTexture ? cuT( "-" ) + fontTexture->getFontName() : castor::String{} )
					, m_device );
			}

			result = it->second->fill( overlayIndex, overlay );
		}
		catch ( std::exception & exc )
		{
			CU_Failure( "Couldn't create an OverlayTextBuffer" );
			fprintf( stderr, "Couldn't create an OverlayTextBuffer: %s", exc.what() );
		}

		return result;
	}

	void OverlayTextBufferPool::upload( UploadData & uploader )
	{
		for ( auto & [_, buffer] : m_buffers )
		{
			buffer->upload( uploader );
		}
	}

	OverlayTextBuffer const * OverlayTextBufferPool::get( FontTexture const & fontTexture )
	{
		auto it = m_buffers.emplace( &fontTexture, nullptr ).first;

		if ( !it->second )
		{
			it->second = castor::make_unique< OverlayTextBuffer >( m_engine
				, m_name + cuT( "-" ) + fontTexture.getFontName()
				, m_device );
		}

		return it->second.get();
	}

	//*************************************************************************
}
