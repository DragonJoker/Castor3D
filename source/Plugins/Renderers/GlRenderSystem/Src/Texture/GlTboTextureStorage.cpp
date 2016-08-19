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
		: m_glBuffer{ static_cast< GlTextureStorage< GlTboTextureStorageTraits > & >( p_storage ).GetOpenGl(), eGL_BUFFER_TARGET_TEXTURE }
	{
		REQUIRE( p_storage.GetType() == TextureStorageType::Buffer );
		bool l_return = m_glBuffer.Create();

		if ( l_return )
		{
			auto & l_storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > & >( p_storage );
			auto l_buffer = p_storage.GetOwner()->GetBuffer();
			m_glInternal = l_storage.GetOpenGl().GetInternal( l_buffer->format() );
			l_return = m_glBuffer.Initialise( l_buffer->const_ptr(), l_buffer->size(), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
		}
	}

	GlTboTextureStorageTraits::~GlTboTextureStorageTraits()
	{
		m_glBuffer.Cleanup();
		m_glBuffer.Destroy();
	}

	bool GlTboTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		auto const & l_storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > const & >( p_storage );
		return l_storage.GetOpenGl().TexBuffer( eGL_TEXDIM_BUFFER, m_glInternal, m_glBuffer.GetGlName() );
	}

	void GlTboTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		auto const & l_storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > const & >( p_storage );
		l_storage.GetOpenGl().TexBuffer( eGL_TEXDIM_BUFFER, m_glInternal, 0 );
	}

	uint8_t * GlTboTextureStorageTraits::Lock( TextureStorage & p_storage, uint32_t p_lock )
	{
		auto & l_storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > & >( p_storage );
		m_glBuffer.Bind();
		return m_glBuffer.Lock( l_storage.GetOpenGl().GetLockFlags( p_lock ) );
	}

	void GlTboTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified )
	{
		m_glBuffer.Unlock();
		m_glBuffer.Unbind();
	}

	void GlTboTextureStorageTraits::Fill( TextureStorage & p_storage, uint8_t const * p_buffer, Castor::Size const & p_size, Castor::PixelFormat p_format )
	{
		m_glBuffer.Fill( p_buffer, p_size.width() * p_size.height() * PF::GetBytesPerPixel( p_format ), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
	}
}
