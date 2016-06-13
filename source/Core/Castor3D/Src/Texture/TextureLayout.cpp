#include "TextureLayout.hpp"

#include "Engine.hpp"

#include "TextureStorage.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		size_t GetImagesCount( TextureType p_type )
		{
			size_t l_return = 1;

			if ( p_type == TextureType::Cube )
			{
				l_return = size_t( CubeMapFace::Count );
			}

			return l_return;
		}
	}

	TextureLayout::TextureLayout( RenderSystem & p_renderSystem, TextureType p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_type{ p_type }
		, m_images{ GetImagesCount( p_type ) }
		, m_cpuAccess{ p_cpuAccess }
		, m_gpuAccess{ p_gpuAccess }
	{
		uint32_t l_index = 0u;

		for ( auto & l_image : m_images )
		{
			l_image = std::make_unique< TextureImage >( *p_renderSystem.GetEngine(), l_index++ );
		}
	}

	TextureLayout::~TextureLayout()
	{
	}

	bool TextureLayout::Initialise()
	{
		if ( !m_initialised )
		{
			bool l_return = DoBind( 0 );

			if ( l_return )
			{
				for ( auto const & l_image : m_images )
				{
					if ( l_return )
					{
						l_return = l_image->Initialise( m_type, m_cpuAccess, m_gpuAccess );
					}
				}

				DoUnbind( 0 );
			}

			m_initialised = l_return;
		}

		return m_initialised;
	}

	void TextureLayout::Cleanup()
	{
		if ( m_initialised )
		{
			for ( auto const & l_image : m_images )
			{
				l_image->Cleanup();
			}
		}

		m_initialised = false;
	}

	bool TextureLayout::Bind( uint32_t p_index )const
	{
		bool l_return = m_initialised;

		if ( l_return )
		{
			l_return = DoBind( p_index );

			for ( auto const & l_image : m_images )
			{
				if ( l_return )
				{
					l_return = l_image->Bind( p_index );
				}
			}
		}

		return l_return;
	}

	void TextureLayout::Unbind( uint32_t p_index )const
	{
		if ( m_initialised )
		{
			for ( auto const & l_image : m_images )
			{
				l_image->Unbind( p_index );
			}

			DoUnbind( p_index );
		}
	}
}
