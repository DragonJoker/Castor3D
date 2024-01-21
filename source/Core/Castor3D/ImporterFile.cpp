#include "Castor3D/ImporterFile.hpp"

#include "Castor3D/Binary/CmshImporter.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( castor3d, ImporterFile )
CU_ImplementSmartPtr( castor3d, ImporterFileFactory )

namespace castor3d
{
	//*********************************************************************************************

	bool parseImportParameters( Parameters const & parameters
		, castor::Point3f & scale
		, castor::Quaternion & orientation )
	{
		float value = 1.0f;
		bool needsTransform = false;

		if ( parameters.get( cuT( "rescale" ), value )
			&& std::abs( value ) > std::numeric_limits< float >::epsilon()
			&& std::abs( value - 1.0f ) > std::numeric_limits< float >::epsilon() )
		{
			scale = { value, value, value };
			needsTransform = true;
		}

		if ( parameters.get( cuT( "pitch" ), value )
			&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
		{
			auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 1.0f, 0.0f, 0.0f }
			, castor::Angle::fromDegrees( value ) );
			orientation *= rot;
			needsTransform = true;
		}

		if ( parameters.get( cuT( "yaw" ), value )
			&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
		{
			auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.0f }
			, castor::Angle::fromDegrees( value ) );
			orientation *= rot;
			needsTransform = true;
		}

		if ( parameters.get( cuT( "roll" ), value )
			&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
		{
			auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 0.0f, 1.0f }
			, castor::Angle::fromDegrees( value ) );
			orientation *= rot;
			needsTransform = true;
		}

		return needsTransform;
	}

	//*********************************************************************************************

	ImporterFile::ImporterFile( Engine & engine
		, Scene * scene
		, castor::Path const & path
		, Parameters const & parameters
		, ProgressBar * progress )
		: castor::OwnedBy< Engine >{ engine }
		, m_scene{ scene }
		, m_fileName{ path }
		, m_filePath{ m_fileName.getPath() }
		, m_extension{ castor::string::lowerCase( m_fileName.getExtension() ) }
		, m_parameters{ parameters }
		, m_progress{ progress }
	{
		castor::String prefix;

		if ( m_parameters.get( cuT( "prefix" ), prefix ) )
		{
			m_prefix = prefix + "-";
		}
	}
	
	//*********************************************************************************************

	ImporterFileFactory::ImporterFileFactory()
	{
		registerType( CmshMeshImporter::Type, cuT( "cmsh" ), CmshImporterFile::create );
		registerType( CmshSkeletonImporter::Type, cuT( "cmsh" ), CmshImporterFile::create );
		registerType( CmshAnimationImporter::MeshAnimType, cuT( "cmsh" ), CmshImporterFile::create );
		registerType( CmshAnimationImporter::SkeletonAnimType, cuT( "cmsh" ), CmshImporterFile::create );
		registerType( CmshAnimationImporter::NodeAnimType, cuT( "cmsh" ), CmshImporterFile::create );
	}

	ImporterFileUPtr ImporterFileFactory::create( castor::String const & type
		, Engine & engine
		, castor::Path const & file
		, Parameters const & parameters
		, ProgressBar * progress )const
	{
		return create( type
			, "any"
			, engine
			, file
			, parameters
			, progress );
	}

	ImporterFileUPtr ImporterFileFactory::create( castor::String const & type
		, Scene & scene
		, castor::Path const & file
		, Parameters const & parameters
		, ProgressBar * progress )const
	{
		return create( type
			, "any"
			, scene
			, file
			, parameters
			, progress );
	}

	ImporterFileUPtr ImporterFileFactory::create( castor::String const & type
		, castor::String const & name
		, Engine & engine
		, castor::Path const & file
		, Parameters const & parameters
		, ProgressBar * progress )const
	{
		return doCreate( type
			, name
			, engine
			, nullptr
			, file
			, parameters
			, progress );
	}

	ImporterFileUPtr ImporterFileFactory::create( castor::String const & type
		, castor::String const & name
		, Scene & scene
		, castor::Path const & file
		, Parameters const & parameters
		, ProgressBar * progress )const
	{
		return doCreate( type
			, name
			, *scene.getEngine()
			, &scene
			, file
			, parameters
			, progress );
	}

	ImporterFileUPtr ImporterFileFactory::doCreate( castor::String const & type
		, castor::String const & name
		, Engine & engine
		, Scene * scene
		, castor::Path const & file
		, Parameters const & parameters
		, ProgressBar * progress )const
	{
		auto it = m_registered.find( type );

		if ( it == m_registered.end() )
		{
			CU_Exception( castor::ERROR_UNKNOWN_OBJECT );
		}

		if ( name == "any" )
		{
			if ( auto tit = it->second.find( type );
				tit != it->second.end() )
			{
				return tit->second( engine, scene, file, parameters, progress );
			}

			return it->second.begin()->second( engine, scene, file, parameters, progress );
		}

		auto tit = it->second.find( name );

		if ( tit == it->second.end() )
		{
			CU_Exception( castor::ERROR_UNKNOWN_OBJECT );
		}

		return tit->second( engine, scene, file, parameters, progress );
	}

	//*********************************************************************************************
}
