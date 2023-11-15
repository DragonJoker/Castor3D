#include "TextTextureData.hpp"

#include "TextRenderTarget.hpp"
#include "TextTextureConfiguration.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/Animation/TextureAnimation.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderSystem.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/File.hpp>

namespace castor
{
	namespace txtexdata
	{
		static bool isReworkedImage( String texName )
		{
			return String::npos != texName.find( "/Compressed" )
				|| String::npos != texName.find( "/Mipped" )
				|| String::npos != texName.find( "/Tiled" )
				|| String::npos != texName.find( "/RGBA" )
				|| String::npos != texName.find( "/HResampled" )
				|| String::npos != texName.find( "/WResampled" );
		}

		static void reworkImageFileName( String texName, Path & path, bool & needsYInversion )
		{
			if ( path.getExtension() != "dds" )
			{
				needsYInversion = !needsYInversion;
			}

			// Remove TextureCache generated suffixes.
			castor::string::replace( texName, "/Compressed", "" );
			castor::string::replace( texName, "/Mipped", "" );
			castor::string::replace( texName, "/Tiled", "" );
			castor::string::replace( texName, "/RGBA", "" );
			castor::string::replace( texName, "/HResampled", "" );
			castor::string::replace( texName, "/WResampled", "" );

			texName = File::normaliseFileName( texName );
			path = Path{ Path{ texName }.getFileName() + cuT( ".dds" ) };
		}
	}

	using namespace castor3d;

	TextWriter< TextureData >::TextWriter( String const & tabs
		, castor3d::Engine const & engine
		, Path const & folder
		, String subFolder )
		: TextWriterT< TextureData >{ tabs, "TextureData" }
		, m_engine{ engine }
		, m_folder{ folder }
		, m_subFolder{ subFolder }
	{
	}

	bool TextWriter< TextureData >::operator()( TextureData const & object
		, StringStream & file )
	{
		auto & sourceInfo = object.sourceInfo;
		bool result = true;

		if ( sourceInfo.isSerialisable()
			&& ( sourceInfo.isFileImage()
				|| sourceInfo.isBufferImage()
				|| sourceInfo.isRenderTarget() ) )
		{
			log::info << tabs() << cuT( "Writing TextureData" ) << std::endl;

			if ( auto block{ beginBlock( file, cuT( "texture" ), sourceInfo.name() ) } )
			{
				auto config = sourceInfo.textureConfig();

				if ( sourceInfo.isRenderTarget() )
				{
					result = writeSub( file, *sourceInfo.renderTarget() );
				}
				else
				{
					auto name = object.image ? object.image->getName() : sourceInfo.name();
					bool createImageFile = sourceInfo.isBufferImage()
						|| ( sourceInfo.isFileImage() && txtexdata::isReworkedImage( name ) );

					if ( createImageFile )
					{
						log::info << tabs() << cuT( "\tCreating texture image" ) << std::endl;
						castor::Path path{ cuT( "Textures" ) };

						if ( !m_subFolder.empty() )
						{
							path /= m_subFolder;
						}

						if ( !File::directoryExists( m_folder / path ) )
						{
							File::directoryCreate( m_folder / path );
						}

						castor::Path imageFile = sourceInfo.isFileImage()
							? sourceInfo.relative()
							: castor::Path{ name };
						txtexdata::reworkImageFileName( name, imageFile, config.needsYInversion );
						path /= imageFile;
						auto & writer = m_engine.getImageWriter();
						result = writer.write( m_folder / path, object.image->getPxBuffer() );
						checkError( result, "Image creation" );

						if ( result )
						{
							result = writePath( file, cuT( "image" ), path );
						}
					}
					else
					{
						log::info << tabs() << cuT( "\tCopying texture image" ) << std::endl;

						if ( m_subFolder.empty() )
						{
							result = writeFile( file, cuT( "image" ), sourceInfo.relative(), m_folder, cuT( "Textures" ) );
						}
						else
						{
							result = writeFile( file, cuT( "image" ), sourceInfo.relative(), m_folder, String{ cuT( "Textures" ) } + Path::GenericSeparator + m_subFolder );
						}
					}
				}

				if ( result )
				{
					result = writeSub( file, config, m_engine );
					checkError( result, "configuration" );
				}
			}
		}

		return result;
	}
}
