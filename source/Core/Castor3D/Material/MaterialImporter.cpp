#include "Castor3D/Material/MaterialImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"

#include <CastorUtils/Graphics/HeightMapToNormalMap.hpp>

CU_ImplementSmartPtr( castor3d, MaterialImporter )

namespace castor3d
{
	namespace matimp
	{
		static bool findImage( castor::Path const & path
			, castor::Path const & filePath
			, castor::Path & folder
			, castor::Path & relative )
		{
			if ( castor::File::fileExists( path ) )
			{
				relative = path;
			}
			else if ( castor::File::fileExists( filePath / path ) )
			{
				auto fullPath = filePath / path;
				folder = fullPath.getPath();
				relative = fullPath.getFileName( true );
			}
			else
			{
				castor::PathArray files;
				castor::String fileName = path.getFileName( true );
				castor::File::listDirectoryFiles( filePath, files, true );
				auto it = std::find_if( files.begin()
					, files.end()
					, [&fileName]( castor::Path const & file )
					{
						return file.getFileName( true ) == fileName
							|| file.getFileName( true ).find( fileName ) == 0;
					} );

				folder = filePath;

				if ( it != files.end() )
				{
					relative = *it;
					relative = castor::Path{ relative.substr( folder.size() + 1 ) };
				}
				else
				{
					relative = castor::Path{ fileName };
				}
			}

			if ( !castor::File::fileExists( folder / relative ) )
			{
				log::error << cuT( "Couldn't load texture file [" ) << path << cuT( "]: File does not exist." ) << std::endl;
				return false;
			}

			return true;
		}
	}

	MaterialImporter::MaterialImporter( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	MaterialImporter::MaterialImporter( Engine & engine
		, ImporterFile * file )
		: OwnedBy< Engine >{ engine }
		, m_file{ file }
	{
	}

	bool MaterialImporter::import( Material & material
		, ImporterFile * file
		, Parameters const & parameters
		, std::map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps )
	{
		if ( !m_file )
		{
			m_file = file;
		}

		m_textureRemaps = textureRemaps;
		m_parameters = parameters;
		return doImportMaterial( material );
	}

	bool MaterialImporter::import( Material & material
		, castor::Path const & path
		, Parameters const & parameters
		, std::map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps )
	{
		auto & engine = *material.getEngine();
		auto extension = castor::string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		auto file = engine.getImporterFileFactory().create( extension
			, engine
			, path
			, parameters );

		if ( auto importer = file->createMaterialImporter() )
		{
			return importer->import( material
				, file.get()
				, parameters
				, textureRemaps );
		}

		return false;
	}

	castor::ImageRPtr MaterialImporter::loadImage( castor::String const & name
		, castor::ImageCreateParams const & params )const
	{
		castor::ImageRPtr result{};

		try
		{
			auto image = getEngine()->tryFindImage( name );

			if ( !image )
			{
				auto img = getEngine()->createImage( name, params );
				image = getEngine()->addImage( name, img );
			}

			result = image;
		}
		catch ( castor::Exception & exc )
		{
			log::error << cuT( "Error encountered while loading image file [" ) << name << cuT( "]: " ) << exc.what() << std::endl;
		}
		catch ( std::exception & exc )
		{
			log::error << cuT( "Error encountered while loading image file [" ) << name << cuT( "]: " ) << exc.what() << std::endl;
		}
		catch ( ... )
		{
			log::error << cuT( "Error encountered while loading image file [" ) << name << cuT( "]: Unknown error" ) << std::endl;
		}

		return result;
	}

	castor::ImageRPtr MaterialImporter::loadImage( castor::Path const & path )const
	{
		castor::ImageRPtr result{};
		castor::Path relative;

		if ( castor::Path folder;
			matimp::findImage( path, m_file->getFilePath(), folder, relative ) )
		{
			result = loadImage( relative.getFileName()
				, castor::ImageCreateParams{ folder / relative
					, { false, false, false } } );
		}

		return result;
	}

	castor::ImageRPtr MaterialImporter::loadImage( castor::String const & name
		, castor::String type
		, castor::ByteArray data )const
	{
		return loadImage( name
			, castor::ImageCreateParams{ std::move( type )
				, std::move( data )
				, { false, false, false } } );
	}

	TextureSourceInfo MaterialImporter::loadTexture( castor::Path const & path
		, TextureConfiguration const & config )const
	{
		auto image = loadImage( path );

		if ( !image )
		{
			CU_Exception( "Couldn't find image at path [" + path + "]" );
		}

		bool allowCompression = !checkFlag( config.textureSpace, TextureSpace::eTangentSpace );
		return TextureSourceInfo{ image->getPath().getFileName( true )
			, config
			, image->getPath().getPath()
			, image->getPath().getFileName( true )
			, { allowCompression, true, true } };
	}

	TextureSourceInfo MaterialImporter::loadTexture( castor::String name
		, castor::String type
		, castor::ByteArray data
		, TextureConfiguration const & config )const
	{
		if ( auto image = loadImage( name, type, data );
			!image )
		{
			CU_Exception( "Couldn't load image [" + name + "]" );
		}

		bool allowCompression = !checkFlag( config.textureSpace, TextureSpace::eTangentSpace );
		return TextureSourceInfo{ std::move( name )
			, config
			, std::move( type )
			, std::move( data )
			, { allowCompression, true, true } };
	}

	void MaterialImporter::loadTexture( castor::Path const & path
		, TextureConfiguration const & config
		, PassTextureConfig const & passConfig
		, Pass & pass )const
	{
		try
		{
			pass.registerTexture( loadTexture( path, config )
				, passConfig );
		}
		catch ( std::exception & exc )
		{
			log::error << exc.what() << std::endl;
		}
	}

	void MaterialImporter::loadTexture( castor::String name
		, castor::String type
		, castor::ByteArray data
		, TextureConfiguration const & config
		, PassTextureConfig const & passConfig
		, Pass & pass )const
	{
		try
		{
			pass.registerTexture( loadTexture( std::move( name )
					, std::move( type )
					, std::move( data )
					, config )
				, passConfig );
		}
		catch ( std::exception & exc )
		{
			log::error << exc.what() << std::endl;
		}
	}

	bool MaterialImporter::convertToNormalMap( castor::Path & path
		, TextureConfiguration & config )const
	{
		auto result = false;

		if ( !path.empty() )
		{
			if ( auto image = loadImage( path ) )
			{
				log::info << "Converting height map to normal map." << std::endl;

				if ( castor::convertToNormalMap( 3.0f, *image ) )
				{
					castor3d::addFlagConfiguration( config
						, { getEngine()->getPassComponentsRegister().getNormalMapFlags()
							, 0x00FFFFFF } );
					castor3d::addFlagConfiguration( config
						, { getEngine()->getPassComponentsRegister().getHeightMapFlags()
							, 0xFF000000 } );
					path = image->getPath();
					path = path.getPath() / ( "N_" + path.getFileName() + ".png" );
					getEngine()->getImageWriter().write( path, image->getPxBuffer() );
				}
			}
		}

		return result;
	}
}
