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
		bool l_return = m_glBuffer.Create();

		if ( l_return )
		{
			auto & l_storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > & >( p_storage );
			auto l_buffer = p_storage.GetOwner()->GetImage().GetBuffer();
			m_glInternal = l_storage.GetOpenGl().GetInternal( l_buffer->format() );
			m_glBuffer.InitialiseStorage( l_buffer->size(), BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			m_glBuffer.Upload( 0u, l_buffer->size(), l_buffer->const_ptr() );
		}
	}

	GlTboTextureStorageTraits::~GlTboTextureStorageTraits()
	{
		m_glBuffer.Destroy();
	}

	void GlTboTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		auto const & l_storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > const & >( p_storage );
		l_storage.GetOpenGl().TexBuffer( GlTexDim::eBuffer, m_glInternal, m_glBuffer.GetGlName() );
	}

	void GlTboTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		auto const & l_storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > const & >( p_storage );
		l_storage.GetOpenGl().TexBuffer( GlTexDim::eBuffer, m_glInternal, 0 );
	}

	uint8_t * GlTboTextureStorageTraits::Lock( TextureStorage & p_storage, FlagCombination< AccessType > const & p_lock, uint32_t p_index )
	{
		auto & l_storage = static_cast< GlTextureStorage< GlTboTextureStorageTraits > & >( p_storage );
		m_glBuffer.Bind();
		return m_glBuffer.Lock( l_storage.GetOpenGl().GetLockFlags( p_lock ) );
	}

	void GlTboTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
		m_glBuffer.Unlock();
		m_glBuffer.Unbind();
	}

	void GlTboTextureStorageTraits::Fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto const l_size = p_image.GetBuffer()->dimensions();
		auto const l_format = p_image.GetBuffer()->format();
		m_glBuffer.Upload( 0u, l_size.width() * l_size.height() * PF::GetBytesPerPixel( l_format ), p_image.GetBuffer()->const_ptr() );
	}
}
