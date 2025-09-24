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

namespace c3d
{
	namespace txtexdata
	{
		static bool isReworkedImage( StringView texName )
		{
			return String::npos != texName.find( cuT( "/Compressed" ) )
				|| String::npos != texName.find( cuT( "/Mipped" ) )
				|| String::npos != texName.find( cuT( "/Tiled" ) )
				|| String::npos != texName.find( cuT( "/RGBA" ) )
				|| String::npos != texName.find( cuT( "/HResampled" ) )
				|| String::npos != texName.find( cuT( "/WResampled" ) );
		}

		static void reworkImageFileName( String texName, Path & path, bool & needsYInversion )
		{
			if ( path.getExtension() != cuT( "dds" ) )
			{
				needsYInversion = !needsYInversion;
			}

			// Remove TextureCache generated suffixes.
			string::replace( texName, cuT( "/Compressed" ), cuT( "" ) );
			string::replace( texName, cuT( "/Mipped" ), cuT( "" ) );
			string::replace( texName, cuT( "/Tiled" ), cuT( "" ) );
			string::replace( texName, cuT( "/RGBA" ), cuT( "" ) );
			string::replace( texName, cuT( "/HResampled" ), cuT( "" ) );
			string::replace( texName, cuT( "/WResampled" ), cuT( "" ) );

			texName = File::normaliseFileName( texName );
			path = Path{ Path{ texName }.getFileName() + cuT( ".dds" ) };
		}
	}

	TextWriter< TextureData >::TextWriter( String const & tabs
		, Engine const & engine
		, Path const & folder
		, String subFolder )
		: TextWriterT< TextureData >{ tabs, cuT( "TextureData" ) }
		, m_engine{ engine }
		, m_folder{ folder }
		, m_subFolder{ c3d::move( subFolder ) }
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
						result = false;
						log::info << tabs() << cuT( "\tCreating texture image" ) << std::endl;
						if ( object.image )
						{
							Path path{ cuT( "Textures" ) };
							if ( !m_subFolder.empty() )
								path /= m_subFolder;

							if ( !File::directoryExists( m_folder / path ) )
								File::directoryCreate( m_folder / path );

							Path imageFile = sourceInfo.isFileImage()
								? sourceInfo.relative()
								: Path{ name };
							txtexdata::reworkImageFileName( name, imageFile, config.needsYInversion );
							path /= imageFile;
							auto & writer = m_engine.getImageWriter();
							result = writer.write( m_folder / path, object.image->getPxBuffer() );
							checkError( result, cuT( "Image creation" ) );

							if ( result )
								result = writePath( file, cuT( "image" ), path );
						}
					}
					else
					{
						log::info << tabs() << cuT( "\tCopying texture image" ) << std::endl;

						if ( m_subFolder.empty() )
							result = writeFile( file, cuT( "image" ), sourceInfo.relative(), m_folder, cuT( "Textures" ) );
						else if ( sourceInfo.folder().empty() )
							result = writeFile( file, cuT( "image" ), sourceInfo.relative(), m_folder, String{ cuT( "Textures" ) } + Path::GenericSeparator + m_subFolder );
						else
							result = writeFile( file, cuT( "image" ), sourceInfo.folder() / sourceInfo.relative(), m_folder, String{ cuT( "Textures" ) } + Path::GenericSeparator + m_subFolder );
					}
				}

				if ( result )
				{
					result = writeSub( file, config, m_engine );
					checkError( result, cuT( "configuration" ) );
				}
			}
		}

		return result;
	}
}
