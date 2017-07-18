#include "Texture/GlTboTextureStorage.hpp"

#include "Common/OpenGl.hpp"
#include "Texture/GlTexture.hpp"
#include "Texture/GlTextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTboTextureStorageTraits::GlTboTextureStorageTraits( TextureStorage & p_storage )
		: m_glBuffer{ static_cast< GlTextureStorage< GlTboTextureStorageTraits > & >( p_storage ).GetOpenGl(), GlBufferTarget::eTexture }
	{
		REQUIRE( p_storage.GetType() == TextureStorageType::eBuffer );
		bool result = m_glBuffer.Create();

		if ( result )
		{
			auto & storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > & >( p_storage );
			auto buffer = p_storage.GetOwner()->GetImage().GetBuffer();
			m_glInternal = storage.GetOpenGl().GetInternal( buffer->format() );
			m_glBuffer.InitialiseStorage( buffer->size(), BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			m_glBuffer.Upload( 0u, buffer->size(), buffer->const_ptr() );
		}
	}

	GlTboTextureStorageTraits::~GlTboTextureStorageTraits()
	{
		m_glBuffer.Destroy();
	}

	void GlTboTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		auto const & storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > const & >( p_storage );
		storage.GetOpenGl().TexBuffer( GlTexDim::eBuffer, m_glInternal, m_glBuffer.GetGlName() );
	}

	void GlTboTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		auto const & storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > const & >( p_storage );
		storage.GetOpenGl().TexBuffer( GlTexDim::eBuffer, m_glInternal, 0 );
	}

	uint8_t * GlTboTextureStorageTraits::Lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		auto & storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > & >( p_storage );
		m_glBuffer.Bind();
		return m_glBuffer.Lock( storage.GetOpenGl().GetLockFlags( p_lock ) );
	}

	void GlTboTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
		m_glBuffer.Unlock();
		m_glBuffer.Unbind();
	}

	void GlTboTextureStorageTraits::Fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto const size = p_image.GetBuffer()->dimensions();
		auto const format = p_image.GetBuffer()->format();
		m_glBuffer.Upload( 0u, size.width() * size.height() * PF::GetBytesPerPixel( format ), p_image.GetBuffer()->const_ptr() );
	}
}
