#include "Castor3D/ImporterFile.hpp"

#include "Castor3D/Binary/CmshImporter.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( c3d, ImporterFile )
CU_ImplementSmartPtr( c3d, ImporterFileFactory )

namespace c3d
{
	//*********************************************************************************************

	bool parseImportParameters( Parameters const & parameters
		, Point3f & scale
		, Quaternion & orientation )
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
			auto rot = Quaternion::fromAxisAngle( Point3f{ 1.0f, 0.0f, 0.0f }
			, Angle::fromDegrees( value ) );
			orientation *= rot;
			needsTransform = true;
		}

		if ( parameters.get( cuT( "yaw" ), value )
			&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
		{
			auto rot = Quaternion::fromAxisAngle( Point3f{ 0.0f, 1.0f, 0.0f }
			, Angle::fromDegrees( value ) );
			orientation *= rot;
			needsTransform = true;
		}

		if ( parameters.get( cuT( "roll" ), value )
			&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
		{
			auto rot = Quaternion::fromAxisAngle( Point3f{ 0.0f, 0.0f, 1.0f }
			, Angle::fromDegrees( value ) );
			orientation *= rot;
			needsTransform = true;
		}

		return needsTransform;
	}

	//*********************************************************************************************

	ImporterFile::ImporterFile( Engine & engine
		, Scene * scene
		, Path const & path
		, Parameters const & parameters
		, ProgressBar * progress )
		: OwnedBy< Engine >{ engine }
		, m_scene{ scene }
		, m_fileName{ path }
		, m_filePath{ m_fileName.getPath() }
		, m_extension{ string::lowerCase( m_fileName.getExtension() ) }
		, m_parameters{ parameters }
		, m_progress{ progress }
	{
		String prefix;

		if ( m_parameters.get( cuT( "prefix" ), prefix ) )
		{
			m_prefix = prefix + cuT( "-" );
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

	ImporterFileUPtr ImporterFileFactory::create( String const & type
		, Engine & engine
		, Path const & file
		, Parameters const & parameters
		, ProgressBar * progress )const
	{
		return create( type
			, cuT( "any" )
			, engine
			, file
			, parameters
			, progress );
	}

	ImporterFileUPtr ImporterFileFactory::create( String const & type
		, Scene & scene
		, Path const & file
		, Parameters const & parameters
		, ProgressBar * progress )const
	{
		return create( type
			, cuT( "any" )
			, scene
			, file
			, parameters
			, progress );
	}

	ImporterFileUPtr ImporterFileFactory::create( String const & type
		, String const & name
		, Engine & engine
		, Path const & file
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

	ImporterFileUPtr ImporterFileFactory::create( String const & type
		, String const & name
		, Scene & scene
		, Path const & file
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

	ImporterFileUPtr ImporterFileFactory::doCreate( String const & type
		, String const & name
		, Engine & engine
		, Scene * scene
		, Path const & file
		, Parameters const & parameters
		, ProgressBar * progress )const
	{
		auto it = m_registered.find( type );

		if ( it == m_registered.end() )
		{
			CU_Exception( c3d::ERROR_UNKNOWN_OBJECT );
		}

		if ( name == cuT( "any" ) )
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
			CU_Exception( c3d::ERROR_UNKNOWN_OBJECT );
		}

		return tit->second( engine, scene, file, parameters, progress );
	}

	//*********************************************************************************************
}
