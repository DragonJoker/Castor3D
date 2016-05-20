#include "TextureLayout.hpp"

#include "Engine.hpp"

#include "TextureStorage.hpp"

#include "Render/RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		size_t GetImagesCount( eTEXTURE_TYPE p_type )
		{
			size_t l_return = 1;

			if ( p_type == eTEXTURE_TYPE_CUBE )
			{
				l_return = 6;
			}

			return l_return;
		}
	}

	TextureLayout::TextureLayout( RenderSystem & p_renderSystem, eTEXTURE_TYPE p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, m_type{ p_type }
		, m_images{ GetImagesCount( p_type ) }
		, m_cpuAccess{ p_cpuAccess }
		, m_gpuAccess{ p_gpuAccess }
	{
	}

	TextureLayout::~TextureLayout()
	{
	}

	bool TextureLayout::Initialise()
	{
		if ( !m_initialised )
		{
			bool l_return = DoInitialise();

			if ( l_return )
			{
				for ( auto const & l_image : m_images )
				{
					l_return = l_image->Initialise( m_type, m_cpuAccess, m_gpuAccess );
				}
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

			DoCleanup();
		}
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
