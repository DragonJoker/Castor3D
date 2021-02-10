#include "Castor3D/Text/TextSkyboxBackground.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< SkyboxBackground >::TextWriter( String const & tabs )
		: TextWriterT< SkyboxBackground >{ tabs }
	{
	}

	bool TextWriter< SkyboxBackground >::operator()( SkyboxBackground const & background
		, TextFile & file )
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

		auto result = file.writeText( cuT( "\n" ) + tabs() + cuT( "//Skybox\n" ) ) > 0;

		if ( !background.getEquiTexturePath().empty()
			&& castor::File::fileExists( background.getEquiTexturePath() ) )
		{
			result = false;

			if ( auto block = beginBlock( file, "skybox" ) )
			{
				Path subfolder{ cuT( "Textures" ) };
				String relative = copyFile( background.getEquiTexturePath()
					, file.getFilePath()
					, subfolder );
				string::replace( relative, cuT( "\\" ), cuT( "/" ) );
				auto & size = background.getEquiSize();
				result = file.writeText( tabs() + cuT( "equirectangular" )
					+ cuT( " \"" ) + relative + cuT( "\" " )
					+ string::toString( size.getWidth() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< SkyboxBackground >::checkError( result, "Skybox equi-texture" );
			}
		}
		else if ( !background.getCrossTexturePath().empty()
			&& castor::File::fileExists( background.getCrossTexturePath() ) )
		{
			result = false;

			if ( auto block = beginBlock( file, "skybox" ) )
			{
				result = writeFile( file, cuT( "cross" ), background.getCrossTexturePath(), cuT( "Textures" ) );
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

			if ( auto block = beginBlock( file, "skybox" ) )
			{
				result = true;

				Path subfolder{ cuT( "Textures" ) };

				for ( uint32_t i = 0; i < 6 && result; ++i )
				{
					result = writeFile( file
						, faces[i]
						, Path{ background.getTexture().getLayerCubeFaceView( 0u, CubeMapFace( i ) ).toString() }
						, cuT( "Textures" ) );
				}
			}
		}

		return result;
	}
}
