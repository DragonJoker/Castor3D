#include "Castor3D/Text/TextSkyboxBackground.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< SkyboxBackground >::TextWriter( String const & tabs
		, Path const & folder )
		: TextWriterT< SkyboxBackground >{ tabs }
		, m_folder{ folder }
	{
	}

	bool TextWriter< SkyboxBackground >::operator()( SkyboxBackground const & background
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing SkyboxBackground" ) << std::endl;
		static String const faces[]
		{
			cuT( "right" ),
			cuT( "left" ),
			cuT( "bottom" ),
			cuT( "top" ),
			cuT( "back" ),
			cuT( "front" ),
		};

		auto result = true;
		file << ( cuT( "\n" ) + tabs() + cuT( "//Skybox\n" ) );

		if ( !background.getEquiTexturePath().empty()
			&& castor::File::fileExists( background.getEquiTexturePath() ) )
		{
			if ( auto block{ beginBlock( file, "skybox" ) } )
			{
				Path subfolder{ cuT( "Textures" ) };
				String relative = copyFile( background.getEquiTexturePath()
					, m_folder
					, subfolder );
				string::replace( relative, cuT( "\\" ), cuT( "/" ) );
				auto & size = background.getEquiSize();
				file << ( tabs() + cuT( "equirectangular" )
					+ cuT( " \"" ) + relative + cuT( "\" " )
					+ string::toString( size.getWidth() ) + cuT( "\n" ) );
				castor::TextWriter< SkyboxBackground >::checkError( result, "Skybox equi-texture" );
			}
		}
		else if ( !background.getCrossTexturePath().empty()
			&& castor::File::fileExists( background.getCrossTexturePath() ) )
		{
			result = false;

			if ( auto block{ beginBlock( file, "skybox" ) } )
			{
				result = writeFile( file, cuT( "cross" ), background.getCrossTexturePath(), m_folder, cuT( "Textures" ) );
			}
		}
		else if ( castor::File::fileExists( Path{ background.getTexture().getLayerCubeFaceView( 0, CubeMapFace( 0u ) ).toString() } )
			&& castor::File::fileExists( Path{ background.getTexture().getLayerCubeFaceView( 0, CubeMapFace( 1u ) ).toString() } )
			&& castor::File::fileExists( Path{ background.getTexture().getLayerCubeFaceView( 0, CubeMapFace( 2u ) ).toString() } )
			&& castor::File::fileExists( Path{ background.getTexture().getLayerCubeFaceView( 0, CubeMapFace( 3u ) ).toString() } )
			&& castor::File::fileExists( Path{ background.getTexture().getLayerCubeFaceView( 0, CubeMapFace( 4u ) ).toString() } )
			&& castor::File::fileExists( Path{ background.getTexture().getLayerCubeFaceView( 0, CubeMapFace( 5u ) ).toString() } ) )
		{
			result = false;

			if ( auto block{ beginBlock( file, "skybox" ) } )
			{
				result = true;

				Path subfolder{ cuT( "Textures" ) };

				for ( uint32_t i = 0; i < 6 && result; ++i )
				{
					result = writeFile( file
						, faces[i]
						, Path{ background.getTexture().getLayerCubeFaceView( 0u, CubeMapFace( i ) ).toString() }
						, m_folder
						, cuT( "Textures" ) );
				}
			}
		}

		return result;
	}
}
