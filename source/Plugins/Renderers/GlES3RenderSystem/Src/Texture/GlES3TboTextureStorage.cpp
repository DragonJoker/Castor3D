#include "Texture/GlES3TboTextureStorage.hpp"

#include "Common/OpenGlES3.hpp"
#include "Texture/GlES3Texture.hpp"
#include "Texture/GlES3TextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3TboTextureStorageTraits::GlES3TboTextureStorageTraits( TextureStorage & p_storage )
		: m_glBuffer{ static_cast< GlES3TextureStorage< GlES3TboTextureStorageTraits > & >( p_storage ).GetOpenGlES3(), GlES3BufferTarget::eTexture }
	{
		REQUIRE( p_storage.GetType() == TextureStorageType::eBuffer );
		bool l_return = m_glBuffer.Create();

		if ( l_return )
		{
			auto & l_storage = static_cast< GlES3TextureStorage< GlES3TboTextureStorageTraits > & >( p_storage );
			auto l_buffer = p_storage.GetOwner()->GetImage().GetBuffer();
			m_glInternal = l_storage.GetOpenGlES3().GetInternal( l_buffer->format() );
			m_glBuffer.InitialiseStorage( l_buffer->size(), BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			m_glBuffer.Upload( 0u, l_buffer->size(), l_buffer->const_ptr() );
		}
	}

	GlES3TboTextureStorageTraits::~GlES3TboTextureStorageTraits()
	{
		m_glBuffer.Destroy();
	}

	void GlES3TboTextureStorageTraits::Bind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		auto const & l_storage = static_cast< GlES3TextureStorage< GlES3TboTextureStorageTraits > const & >( p_storage );
		l_storage.GetOpenGlES3().TexBuffer( GlES3TexDim::eBuffer, m_glInternal, m_glBuffer.GetGlES3Name() );
	}

	void GlES3TboTextureStorageTraits::Unbind( TextureStorage const & p_storage, uint32_t p_index )const
	{
		auto const & l_storage = static_cast< GlES3TextureStorage< GlES3TboTextureStorageTraits > const & >( p_storage );
		l_storage.GetOpenGlES3().TexBuffer( GlES3TexDim::eBuffer, m_glInternal, 0 );
	}

	uint8_t * GlES3TboTextureStorageTraits::Lock( TextureStorage & p_storage, AccessTypes const & p_lock, uint32_t p_index )
	{
		auto & l_storage = static_cast< GlES3TextureStorage< GlES3TboTextureStorageTraits > & >( p_storage );
		m_glBuffer.Bind();
		return m_glBuffer.Lock( p_lock );
	}

	void GlES3TboTextureStorageTraits::Unlock( TextureStorage & p_storage, bool p_modified, uint32_t p_index )
	{
		m_glBuffer.Unlock();
		m_glBuffer.Unbind();
	}

	void GlES3TboTextureStorageTraits::Fill( TextureStorage & p_storage, TextureImage const & p_image )
	{
		auto const l_size = p_image.GetBuffer()->dimensions();
		auto const l_format = p_image.GetBuffer()->format();
		m_glBuffer.Upload( 0u, l_size.width() * l_size.height() * PF::GetBytesPerPixel( l_format ), p_image.GetBuffer()->const_ptr() );
	}
}
