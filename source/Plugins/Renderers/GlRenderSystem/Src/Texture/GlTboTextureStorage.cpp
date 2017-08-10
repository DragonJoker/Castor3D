#include "Texture/GlTboTextureStorage.hpp"

#include "Common/OpenGl.hpp"
#include "Texture/GlTexture.hpp"
#include "Texture/GlTextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlTboTextureStorageTraits::GlTboTextureStorageTraits( TextureStorage & p_storage )
		: m_glBuffer{ static_cast< GlTextureStorage< GlTboTextureStorageTraits > & >( p_storage ).getOpenGl(), GlBufferTarget::eTexture }
	{
		REQUIRE( p_storage.getType() == TextureStorageType::eBuffer );
		bool result = m_glBuffer.create();

		if ( result )
		{
			auto & storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > & >( p_storage );
			auto buffer = p_storage.getOwner()->getImage().getBuffer();
			m_glInternal = storage.getOpenGl().getInternal( buffer->format() );
			m_glBuffer.initialiseStorage( buffer->size(), BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			m_glBuffer.upload( 0u, buffer->size(), buffer->constPtr() );
		}
	}

	GlTboTextureStorageTraits::~GlTboTextureStorageTraits()
	{
		m_glBuffer.destroy();
	}

	void GlTboTextureStorageTraits::bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		auto const & storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > const & >( p_storage );
		storage.getOpenGl().TexBuffer( GlTexDim::eBuffer, m_glInternal, m_glBuffer.getGlName() );
	}

	void GlTboTextureStorageTraits::unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		auto const & storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > const & >( p_storage );
		storage.getOpenGl().TexBuffer( GlTexDim::eBuffer, m_glInternal, 0 );
	}

	uint8_t * GlTboTextureStorageTraits::lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		auto & storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > & >( p_storage );
		m_glBuffer.bind();
		return m_glBuffer.lock( storage.getOpenGl().getLockFlags( p_lock ) );
	}

	void GlTboTextureStorageTraits::unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
		m_glBuffer.unlock();
		m_glBuffer.unbind();
	}

	void GlTboTextureStorageTraits::fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto const size = p_image.getBuffer()->dimensions();
		auto const format = p_image.getBuffer()->format();
		m_glBuffer.upload( 0u, size.getWidth() * size.getHeight() * PF::getBytesPerPixel( format ), p_image.getBuffer()->constPtr() );
	}
}
