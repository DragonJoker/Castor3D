#include "Castor3D/Material/MaterialImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"

#include <CastorUtils/Graphics/HeightMapToNormalMap.hpp>

CU_ImplementSmartPtr( c3d, MaterialImporter )

namespace c3d
{
	namespace matimp
	{
		static bool findImage( Path const & path
			, Path const & filePath
			, Path & folder
			, Path & relative )
		{
			if ( File::fileExists( path ) )
			{
				relative = path;
			}
			else if ( File::fileExists( filePath / path ) )
			{
				auto fullPath = filePath / path;
				folder = fullPath.getPath();
				relative = fullPath.getFileName( true );
			}
			else
			{
				PathArray files;
				auto fileName = path.getFileName( true );
				File::listDirectoryFiles( filePath, files, true );
				auto it = std::find_if( files.begin()
					, files.end()
					, [&fileName]( Path const & file )
					{
						return file.getFileName( true ) == fileName
							|| file.getFileName( true ).find( fileName ) == 0;
					} );

				folder = filePath;

				if ( it != files.end() )
				{
					relative = *it;
					relative = Path{ relative.substr( folder.size() + 1 ) };
				}
				else
				{
					relative = Path{ fileName };
				}
			}

			if ( !File::fileExists( folder / relative ) )
			{
				log::error << cuT( "Couldn't load texture file [" ) << path << cuT( "]: File does not exist." ) << std::endl;
				return false;
			}

			return true;
		}
	}

	MaterialImporter::MaterialImporter( Engine & engine
		, String const & prefix )
		: MaterialImporter{ engine, prefix, nullptr }
	{
	}

	MaterialImporter::MaterialImporter( Engine & engine
		, String const & prefix
		, ImporterFile * file )
		: OwnedBy< Engine >{ engine }
		, m_prefix{ prefix + cuT( " - " ) }
		, m_file{ file }
	{
	}

	void MaterialImporter::prepareImport( ImporterFile * file
		, Parameters const & parameters
		, Map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps )
	{
		if ( !m_file )
			m_file = file;
		m_textureRemaps = textureRemaps;
		m_parameters = parameters;
		m_loadConfig.allowCompression = !m_parameters.get< bool >( cuT( "disable_image_compression" ) );
		float fvalue;

		if ( m_parameters.get( cuT( "emissive_mult" ), fvalue )
			&& std::abs( fvalue - 1.0f ) > std::numeric_limits< float >::epsilon() )
			m_emissiveMult = fvalue;
	}

	MaterialPtr MaterialImporter::importData( String const & name
		, ImporterFile * file
		, Parameters const & parameters
		, Map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps )
	{
		prepareImport( file, parameters, textureRemaps );
		auto result = createMaterial( name );
		if ( !result || !importMaterial( *result ) )
			return nullptr;
		return result;
	}

	bool MaterialImporter::importData( Material & material
		, ImporterFile * file
		, Parameters const & parameters
		, Map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps )
	{
		prepareImport( file, parameters, textureRemaps );
		log::info << getPrefix() << cuT( "Loading Material [" ) << material.getName() << cuT( "]" ) << std::endl;

		bool result = importMaterial( material );

		if ( result )
			log::info << getPrefix() << cuT( "Loaded Material [" ) << material.getName() << cuT( "]" ) << std::endl;
		else
			log::info << getPrefix() << cuT( "Couldn't load Material [" ) << material.getName() << cuT( "]" ) << std::endl;
		return result;
	}

	bool MaterialImporter::importData( Material & material
		, Path const & path
		, Parameters const & parameters
		, Map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps )
	{
		auto & engine = *material.getEngine();
		auto extension = string::lowerCase( path.getExtension() );

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
			return importer->importData( material
				, file.get()
				, parameters
				, textureRemaps );
		}

		return false;
	}

	ImageRPtr MaterialImporter::loadImage( String const & name
		, ImageCreateParams const & params )const
	{
		ImageRPtr result{};

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
		catch ( Exception & exc )
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

	ImageRPtr MaterialImporter::loadImage( Path const & path
		, ImageLoaderConfig const & loadConfig )const
	{
		ImageRPtr result{};
		Path relative;

		if ( Path folder;
			matimp::findImage( path, m_file->getFilePath(), folder, relative ) )
		{
			result = loadImage( relative.getFileName()
				, ImageCreateParams{ folder / relative, loadConfig } );
		}

		return result;
	}

	ImageRPtr MaterialImporter::loadImage( Path const & path )const
	{
		return loadImage( path
			, { false, false, false } );
	}

	ImageRPtr MaterialImporter::loadImage( String const & name
		, String type
		, ByteArray data
		, ImageLoaderConfig const & loadConfig )const
	{
		return loadImage( name
			, ImageCreateParams{ c3d::move( type )
				, c3d::move( data )
				, loadConfig } );
	}

	ImageRPtr MaterialImporter::loadImage( String const & name
		, String type
		, ByteArray data )const
	{
		return loadImage( name
			, std::move( type )
			, std::move( data )
			, { false, false, false } );
	}

	TextureSourceInfo MaterialImporter::loadTexture( Path const & path
		, TextureConfiguration const & config
		, ImageLoaderConfig const & loadConfig )const
	{
		auto image = loadImage( path );

		if ( !image )
		{
			CU_Exception( "Couldn't find image at path [" + toUtf8( path ) + "]" );
		}

		auto loaderConfig = loadConfig;
		loaderConfig.allowCompression = loaderConfig.allowCompression
			&& !checkFlag( config.textureSpace, TextureSpace::eTangentSpace );
		return TextureSourceInfo{ image->getName()
			, config
			, image->getPath().getPath()
			, image->getPath().getFileName( true )
			, std::move( loaderConfig ) };
	}

	TextureSourceInfo MaterialImporter::loadTexture( Path const & path
		, TextureConfiguration const & config )const
	{
		return loadTexture( path
			, config
			, { true, true, true } );
	}

	TextureSourceInfo MaterialImporter::loadTexture( String name
		, String type
		, ByteArray data
		, TextureConfiguration const & config
		, ImageLoaderConfig const & loadConfig )const
	{
		auto loaderConfig = loadConfig;
		loaderConfig.allowCompression = loaderConfig.allowCompression
			&& !checkFlag( config.textureSpace, TextureSpace::eTangentSpace );
		return TextureSourceInfo{ c3d::move( name )
			, config
			, c3d::move( type )
			, c3d::move( data )
			, c3d::move( loaderConfig ) };
	}

	TextureSourceInfo MaterialImporter::loadTexture( String name
		, String type
		, ByteArray data
		, TextureConfiguration const & config )const
	{
		return loadTexture( std::move( name )
			, std::move( type )
			, std::move( data )
			, config
			, { true, true, true } );
	}

	void MaterialImporter::loadTexture( Path const & path
		, TextureConfiguration const & config
		, PassTextureConfig const & passConfig
		, Pass & pass
		, ImageLoaderConfig const & loadConfig )const
	{
		try
		{
			pass.registerTexture( loadTexture( path, config, loadConfig )
				, passConfig );
		}
		catch ( std::exception & exc )
		{
			log::error << exc.what() << std::endl;
		}
	}

	void MaterialImporter::loadTexture( Path const & path
		, TextureConfiguration const & config
		, PassTextureConfig const & passConfig
		, Pass & pass )const
	{
		return loadTexture( path
			, config
			, passConfig
			, pass
			, { true, true, true } );
	}

	void MaterialImporter::loadTexture( String name
		, String type
		, ByteArray data
		, TextureConfiguration const & config
		, PassTextureConfig const & passConfig
		, Pass & pass
		, ImageLoaderConfig const & loadConfig )const
	{
		try
		{
			pass.registerTexture( loadTexture( c3d::move( name )
					, c3d::move( type )
					, c3d::move( data )
					, config
					, loadConfig )
				, passConfig );
		}
		catch ( std::exception & exc )
		{
			log::error << exc.what() << std::endl;
		}
	}

	void MaterialImporter::loadTexture( String name
		, String type
		, ByteArray data
		, TextureConfiguration const & config
		, PassTextureConfig const & passConfig
		, Pass & pass )const
	{
		return loadTexture( std::move( name )
			, std::move( type )
			, std::move( data )
			, config
			, passConfig
			, pass
			, { true, true, true } );
	}

	bool MaterialImporter::convertToNormalMap( Path & path
		, TextureConfiguration & config )const
	{
		auto result = false;

		if ( !path.empty() )
		{
			if ( auto image = loadImage( path ) )
			{
				log::info << getPrefix() << "Converting height map to normal map." << std::endl;

				if ( c3d::convertToNormalMap( 3.0f, *image ) )
				{
					c3d::addFlagConfiguration( config
						, { getEngine()->getPassComponentsRegister().getNormalMapFlags()
							, 0x00FFFFFF } );
					c3d::addFlagConfiguration( config
						, { getEngine()->getPassComponentsRegister().getHeightMapFlags()
							, 0xFF000000 } );
					path = image->getPath();
					path = path.getPath() / ( cuT( "N_" ) + path.getFileName() + cuT( ".png" ) );
					getEngine()->getImageWriter().write( path, image->getPxBuffer() );
				}
			}
		}

		return result;
	}

	MaterialPtr MaterialImporter::createMaterial( String const & name )
	{
		return getOwner()->createMaterial( name
			, *getOwner()
			, getOwner()->getDefaultLightingModel() );
	}
}
