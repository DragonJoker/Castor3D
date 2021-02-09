#include "Castor3D/Text/TextTextureUnit.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Text/TextRenderTarget.hpp"
#include "Castor3D/Text/TextTextureConfiguration.hpp"

using namespace castor3d;

namespace castor
{
	namespace
	{
		String writeMask( uint32_t mask )
		{
			auto stream = castor::makeStringStream();
			stream << cuT( "0x" ) << std::hex << std::setw( 8u ) << std::setfill( cuT( '0' ) ) << mask;
			return stream.str();
		}

		void doUpdateShift( castor::Point2ui & mask )
		{
			if ( mask[0] )
			{
				for ( uint32_t i = 0u; i < 4u; ++i )
				{
					if ( mask[0] & ( 0x000000FFu << ( i * 8u ) ) )
					{
						mask[1] = i;
						i = 4u;
					}
				}
			}
		}
	}

	TextWriter< TextureUnit >::TextWriter( String const & tabs
		, MaterialType type )
		: TextWriterT< TextureUnit >{ tabs, "TextureUnit" }
		, m_type{ type }
	{
	}

	bool TextWriter< TextureUnit >::operator()( TextureUnit const & unit, TextFile & file )
	{
		bool result = true;
		auto hasTexture = unit.isTextured() && unit.getTexture();

		if ( hasTexture )
		{
			auto texture = unit.getTexture();
			auto image = texture->getPath();
			hasTexture = !image.empty() || !texture->isStatic();

			if ( hasTexture )
			{
				if ( auto block = beginBlock( cuT( "texture_unit" ), file ) )
				{
					if ( unit.getSampler() && unit.getSampler()->getName() != cuT( "Default" ) )
					{
						result = writeName( cuT( "sampler" ), unit.getSampler()->getName(), file );
					}

					if ( result && unit.getTexture()->getMipmapCount() > 1 )
					{
						result = write( cuT( "levels_count" ), unit.getTexture()->getMipmapCount(), file );
					}

					if ( result )
					{
						if ( !texture->isStatic() )
						{
							if ( unit.getRenderTarget() )
							{
								result = write( *unit.getRenderTarget(), file );
							}
							else
							{
								// Procedurally generated textures, certainly will go here
							}
						}
						else
						{
							result = writeFile( cuT( "image" ), Path{ image }, cuT( "Textures" ), file );
						}
					}

					if ( result )
					{
						result = TextWriter< TextureConfiguration >{ tabs(), m_type }( unit.getConfiguration(), file );
						checkError( result, "configuration" );
					}
				}
			}
		}

		return result;
	}
}
