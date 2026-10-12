#include "Castor3D/Scene/Light/LightImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/LightGroup.hpp"

CU_ImplementSmartPtr( c3d, LightImporter )

namespace c3d
{
	LightImporter::LightImporter( Engine & engine
		, String const & prefix
		, c3d::LightImportConfig const & config )
		: OwnedBy< Engine >{ engine }
		, m_prefix{ prefix + cuT( " - " ) }
		, m_config{ config }
	{
	}

	LightUPtr LightImporter::importData( String const & name
		, LightCreateInfo const & createInfo
		, ImporterFile * file )
	{
		if ( !m_file )
		{
			m_file = file;
		}

		auto result = doCreateLight( name, createInfo );

		if ( !result
			|| !importData( *result, file ) )
		{
			return nullptr;
		}

		return result;
	}

	bool LightImporter::importData( Light & light
		, ImporterFile * file )
	{
		m_file = file;
		log::info << getPrefix() << cuT( "Loading Light [" ) << light.getName() << cuT( "]" ) << std::endl;
		bool result = doImportLight( light );

		if ( result )
		{
			log::info << getPrefix() << cuT( "Light found: [" ) << light.getName() << cuT( "]" ) << std::endl;
		}
		else
		{
			log::info << getPrefix() << cuT( "Couldn't load Light [" ) << light.getName() << cuT( "]" ) << std::endl;
		}

		return result;
	}

	LightGroupUPtr LightImporter::importData( String const & name
		, LightGroupCreateInfo const & createInfo
		, ImporterFile * file )
	{
		if ( !m_file )
		{
			m_file = file;
		}

		auto result = doCreateLightGroup( name, createInfo );

		if ( !result
			|| !importData( *result, file ) )
		{
			return nullptr;
		}

		return result;
	}

	bool LightImporter::importData( LightGroup & light
		, ImporterFile * file )
	{
		m_file = file;
		log::info << getPrefix() << cuT( "Loading LightGroup [" ) << light.getName() << cuT( "]" ) << std::endl;
		bool result = doImportLightGroup( light );

		if ( result )
		{
			log::info << getPrefix() << cuT( "LightGroup found: [" ) << light.getName() << cuT( "]" ) << std::endl;
		}
		else
		{
			log::info << getPrefix() << cuT( "Couldn't load LightGroup [" ) << light.getName() << cuT( "]" ) << std::endl;
		}

		return result;
	}

	bool LightImporter::importData( Light & light
		, Path const & path
		, ImportConfig const & config )
	{
		auto & engine = *light.getOwner()->getEngine();
		auto extension = string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		auto file = engine.getImporterFileFactory().create( extension
			, engine
			, path
			, config );

		if ( auto importer = file->createLightImporter() )
		{
			return importer->importData( light, file.get() );
		}

		return false;
	}

	bool LightImporter::importData( LightGroup & light
		, Path const & path
		, ImportConfig const & config )
	{
		auto & engine = *light.getOwner()->getEngine();
		auto extension = string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		auto file = engine.getImporterFileFactory().create( extension
			, engine
			, path
			, config );

		if ( auto importer = file->createLightImporter() )
		{
			return importer->importData( light, file.get() );
		}

		return false;
	}

	LightUPtr LightImporter::doCreateLight( String const & name
		, LightCreateInfo const & createInfo )
	{
		return createInfo.scene->createLight( name, createInfo );
	}

	LightGroupUPtr LightImporter::doCreateLightGroup( String const & name
		, LightGroupCreateInfo const & createInfo )
	{
		return createInfo.scene->createLightGroup( name, createInfo );
	}
}
