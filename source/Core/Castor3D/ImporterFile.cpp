#include "Castor3D/ImporterFile.hpp"

#include "Castor3D/Binary/CmshImporter.hpp"

CU_ImplementCUSmartPtr( castor3d, ImporterFileFactory )

namespace castor3d
{
	//*********************************************************************************************

	bool parseImportParameters( Parameters const & parameters
		, castor::Point3f & scale
		, castor::Quaternion orientation )
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
		, castor::Path const & path
		, Parameters const & parameters )
		: castor::OwnedBy< Engine >{ engine }
		, m_fileName{ path }
		, m_filePath{ m_fileName.getPath() }
		, m_extension{ castor::string::lowerCase( m_fileName.getExtension() ) }
		, m_parameters{ parameters }
	{
		castor::String prefix;

		if ( m_parameters.get( cuT( "prefix" ), prefix ) )
		{
			m_prefix = prefix + "-";
		}
	}
	
	//*********************************************************************************************

	ImporterFileFactory::ImporterFileFactory()
		: Factory< ImporterFile
		, castor::String
		, ImporterFileUPtr
		, std::function< ImporterFileUPtr( Engine & , castor::Path const &, Parameters const & ) > >{}
	{
		registerType( CmshMeshImporter::Type, CmshImporterFile::create );
		registerType( CmshSkeletonImporter::Type, CmshImporterFile::create );
		registerType( CmshAnimationImporter::MeshAnimType, CmshImporterFile::create );
		registerType( CmshAnimationImporter::SkeletonAnimType, CmshImporterFile::create );
		registerType( CmshAnimationImporter::NodeAnimType, CmshImporterFile::create );
	}

	//*********************************************************************************************
}
