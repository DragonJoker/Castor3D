#include "Castor3D/ImporterFile.hpp"

#include "Castor3D/Binary/CmshImporter.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( c3d, ImporterFile )
CU_ImplementSmartPtr( c3d, ImporterFileFactory )

namespace c3d
{
	//*********************************************************************************************

	namespace impcfg
	{
		float mergeFloats( float lhs, float rhs, float def = 0.0f )
		{
			return std::abs( rhs - def ) > 0.0001f ? rhs : lhs;
		}
	}

	//*********************************************************************************************

	MaterialImportConfig MaterialImportConfig::merge( MaterialImportConfig const & rhs )const
	{
		auto mergeRemaps = []( Map< PassComponentTextureFlag, TextureConfiguration > const & lhs
			, Map< PassComponentTextureFlag, TextureConfiguration > const & rhs )
			{
				Map< PassComponentTextureFlag, TextureConfiguration > result{ lhs };
				for ( auto const & [flag, config] : rhs )
					result.try_emplace( flag, config );
				return result;
			};

		return MaterialImportConfig{ .textureRemaps = mergeRemaps( textureRemaps, rhs.textureRemaps )
			, .emissiveMult = impcfg::mergeFloats( emissiveMult, rhs.emissiveMult, 1.0f )
			, .ignoreImages = ignoreImages || rhs.ignoreImages
			, .disableImageCompression = disableImageCompression || rhs.disableImageCompression };
	}

	bool MaterialImportConfig::fillFromParam( String const & param )
	{
		if ( param.find( cuT( "no_images" ) ) == 0 )
		{
			ignoreImages = true;
			return true;
		}

		if ( param.find( cuT( "emissive_mult" ) ) == 0 )
		{
			if ( auto eqIndex = param.find( cuT( '=' ) );
				eqIndex != String::npos )
				string::parse< float >( param.substr( eqIndex + 1 ), emissiveMult );
			else
				CU_Exception( cuT( "Malformed parameter -emissive_mult=<float>." ) );
			return true;
		}

		return false;
	}

	//*********************************************************************************************

	MeshImportConfig MeshImportConfig::merge( MeshImportConfig const & rhs )const
	{
		return MeshImportConfig{ .submeshIndex = submeshIndex == 0xFFFFFFFFu ? rhs.submeshIndex : submeshIndex
			, .ignoreVertexColour = ignoreVertexColour || rhs.ignoreVertexColour
			, .ignoreSkeleton = ignoreSkeleton || rhs.ignoreSkeleton
			, .invertNormals = invertNormals || rhs.invertNormals };
	}

	bool MeshImportConfig::fillFromParam( String const & param )
	{
		if ( param.find( cuT( "invert_normals" ) ) == 0 )
		{
			invertNormals = true;
			return true;
		}

		if ( param.find( cuT( "ignore_vertex_colour" ) ) == 0 )
		{
			ignoreVertexColour = true;
			return true;
		}

		if ( param.find( cuT( "no_skeleton" ) ) == 0 )
		{
			ignoreSkeleton = true;
			return true;
		}

		if ( param.find( cuT( "submesh" ) ) == 0 )
		{
			if ( auto eqIndex = param.find( cuT( '=' ) );
				eqIndex != String::npos )
				string::parse< uint32_t >( param.substr( eqIndex + 1 ), submeshIndex );
			else
				CU_Exception( cuT( "Malformed parameter -submesh=<uint>." ) );
			return true;
		}

		return false;
	}

	//*********************************************************************************************

	LightImportConfig LightImportConfig::merge( LightImportConfig const & rhs )const
	{
		return LightImportConfig{ .illuminationMultiplier = impcfg::mergeFloats( illuminationMultiplier, rhs.illuminationMultiplier, 1.0f ) };
	}

	bool LightImportConfig::fillFromParam( String const & param )
	{
		if ( param.find( cuT( "illumination_multiplier" ) ) == 0 )
		{
			if ( auto eqIndex = param.find( cuT( '=' ) );
				eqIndex != String::npos )
				string::parse< float >( param.substr( eqIndex + 1 ), illuminationMultiplier );
			else
				CU_Exception( cuT( "Malformed parameter -illumination_multiplier=<float>." ) );
			return true;
		}

		return false;
	}

	//*********************************************************************************************

	ImportConfig ImportConfig::merge( ImportConfig const & rhs )const
	{
		Parameters params{ parameters };
		params.append( rhs.parameters );
		return ImportConfig{ .materialConfig = materialConfig.merge( rhs.materialConfig )
			, .meshConfig = meshConfig.merge( rhs.meshConfig )
			, .lightConfig = lightConfig.merge( rhs.lightConfig )
			, .prefix = prefix.empty() ? rhs.prefix : prefix
			, .centerCamera = centerCamera.empty() ? rhs.centerCamera : centerCamera
			, .preferredImporter = preferredImporter == cuT( "any" ) ? rhs.preferredImporter : preferredImporter
			, .parameters = params
			, .rescale = impcfg::mergeFloats( rescale, rhs.rescale, 1.0f )
			, .pitch = impcfg::mergeFloats( pitch, rhs.pitch )
			, .yaw = impcfg::mergeFloats( yaw, rhs.yaw )
			, .roll = impcfg::mergeFloats( roll, rhs.roll )
			, .disableOptimisations = disableOptimisations || rhs.disableOptimisations
			, .disableMeshMerge = disableMeshMerge || rhs.disableMeshMerge };
	}

	void ImportConfig::fillFromParam( String const & param )
	{
		if ( param.find( cuT( "pitch" ) ) == 0 )
		{
			if ( auto eqIndex = param.find( cuT( '=' ) );
				eqIndex != String::npos )
				string::parse< float >( param.substr( eqIndex + 1 ), pitch );
			else
				CU_Exception( cuT( "Malformed parameter -pitch=<degrees>." ) );
		}
		else if ( param.find( cuT( "yaw" ) ) == 0 )
		{
			if ( auto eqIndex = param.find( cuT( '=' ) );
				eqIndex != String::npos )
				string::parse< float >( param.substr( eqIndex + 1 ), yaw );
			else
				CU_Exception( cuT( "Malformed parameter -yaw=<degrees>." ) );
		}
		else if ( param.find( cuT( "roll" ) ) == 0 )
		{
			if ( auto eqIndex = param.find( cuT( '=' ) );
				eqIndex != String::npos )
				string::parse< float >( param.substr( eqIndex + 1 ), roll );
			else
				CU_Exception( cuT( "Malformed parameter -roll=<degrees>." ) );
		}
		else if ( param.find( cuT( "rescale" ) ) == 0 )
		{
			if ( auto eqIndex = param.find( cuT( '=' ) );
				eqIndex != String::npos )
				string::parse< float >( param.substr( eqIndex + 1 ), rescale );
			else
				CU_Exception( cuT( "Malformed parameter -rescale=<float>." ) );
		}
		else if ( param.find( cuT( "prefix" ) ) == 0 )
		{
			if ( auto eqIndex = param.find( cuT( '=' ) );
				eqIndex != String::npos )
				prefix = string::getTrimmed( param.substr( eqIndex + 1 ) );
			else
				CU_Exception( cuT( "Malformed parameter -prefix=name." ) );
		}
		else if ( param.find( cuT( "no_optimisations" ) ) == 0 )
		{
			disableOptimisations = true;
		}
		else if ( param.find( cuT( "no_merge" ) ) == 0 )
		{
			disableMeshMerge = true;
		}
		else if ( param.find( cuT( "preferred_importer" ) ) == 0 )
		{
			if ( auto eqIndex = param.find( cuT( '=' ) );
				eqIndex != String::npos )
				preferredImporter = string::getTrimmed( param.substr( eqIndex + 1 ) );
			else
				CU_Exception( cuT( "Malformed parameter -preferred_importer=\"name\"." ) );
		}
		else if ( !materialConfig.fillFromParam( param )
			&& !meshConfig.fillFromParam( param )
			&& !lightConfig.fillFromParam( param ) )
		{
			if ( auto eqIndex = param.find( cuT( '=' ) );
				eqIndex != String::npos )
				parameters.add( string::getTrimmed( param.substr( 0, eqIndex ) ), string::getTrimmed( param.substr( eqIndex + 1 ) ) );
			else
				parameters.add( param, true );
		}
	}

	void ImportConfig::fill( String const & params )
	{
		for ( auto const & param : string::split( params, cuT( "-" ), 20, false ) )
			fillFromParam( param );
	}

	//*********************************************************************************************

	bool parseImportParameters( ImportConfig const & config
		, Point3f & scale
		, Quaternion & orientation )
	{
		bool needsTransform = false;

		if ( config.rescale != 1.0f
			&& std::abs( config.rescale ) > std::numeric_limits< float >::epsilon()
			&& std::abs( config.rescale - 1.0f ) > std::numeric_limits< float >::epsilon() )
		{
			scale = { config.rescale, config.rescale, config.rescale };
			needsTransform = true;
		}

		if ( std::abs( config.pitch ) > std::numeric_limits< float >::epsilon() )
		{
			auto rot = Quaternion::fromAxisAngle( Point3f{ 1.0f, 0.0f, 0.0f }
			, Angle::fromDegrees( config.pitch ) );
			orientation *= rot;
			needsTransform = true;
		}

		if ( std::abs( config.yaw ) > std::numeric_limits< float >::epsilon() )
		{
			auto rot = Quaternion::fromAxisAngle( Point3f{ 0.0f, 1.0f, 0.0f }
			, Angle::fromDegrees( config.yaw ) );
			orientation *= rot;
			needsTransform = true;
		}

		if ( std::abs( config.roll ) > std::numeric_limits< float >::epsilon() )
		{
			auto rot = Quaternion::fromAxisAngle( Point3f{ 0.0f, 0.0f, 1.0f }
			, Angle::fromDegrees( config.roll ) );
			orientation *= rot;
			needsTransform = true;
		}

		return needsTransform;
	}

	//*********************************************************************************************

	ImporterFile::ImporterFile( Engine & engine
		, Scene * scene
		, Path const & path
		, ImportConfig const & config
		, ProgressBar * progress )
		: OwnedBy< Engine >{ engine }
		, m_scene{ scene }
		, m_fileName{ path }
		, m_filePath{ m_fileName.getPath() }
		, m_extension{ string::lowerCase( m_fileName.getExtension() ) }
		, m_config{ config }
		, m_progress{ progress }
	{
		if ( !m_config.prefix.empty() )
			m_prefix = m_config.prefix + cuT( "-" );
	}

	c3d::String ImporterFile::reworkName( c3d::String const & name
		, c3d::StringView baseName
		, size_t index )
	{
		c3d::StringView separators = cuT( " \t\r_$|/:\\*!?&#\"()[]{}@+." );
		auto split = c3d::string::split( name, separators, ~0u, false );
		c3d::Set< int > insNumbers;
		c3d::Set< c3d::String > insNames;
		c3d::Vector< c3d::String > numbers;
		c3d::Vector< c3d::String > names;

		for ( auto s : split )
		{
			c3d::string::trim( s, true, true, separators );

			if ( !s.empty() )
			{
				if ( c3d::string::isInteger( s ) && insNumbers.emplace( c3d::string::toInt( s ) ).second )
					numbers.push_back( s );
				else if ( insNames.insert( s ).second )
					names.push_back( s );
			}
		}

		c3d::String result;
		c3d::String sep;

		for ( auto const & s : names )
		{
			result += sep + s;
			sep = cuT( "_" );
		}

		for ( auto const & s : numbers )
		{
			result += sep + s;
			sep = cuT( "_" );
		}

		if ( result.empty() )
			result = c3d::string::toString( index );

		if ( result.size() > 150u )
			result = c3d::String{ baseName } + c3d::string::toString( index );

		return c3d::File::normaliseFileName( result, cuT( "-"_sv ) );
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
		, ImportConfig const & config
		, ProgressBar * progress )const
	{
		return doCreate( type
			, engine
			, nullptr
			, file
			, config
			, progress );
	}

	ImporterFileUPtr ImporterFileFactory::create( String const & type
		, Scene & scene
		, Path const & file
		, ImportConfig const & config
		, ProgressBar * progress )const
	{
		return doCreate( type
			, *scene.getEngine()
			, &scene
			, file
			, config
			, progress );
	}

	ImporterFileUPtr ImporterFileFactory::doCreate( String const & type
		, Engine & engine
		, Scene * scene
		, Path const & file
		, ImportConfig const & config
		, ProgressBar * progress )const
	{
		auto it = m_registered.find( type );

		if ( it == m_registered.end() )
		{
			CU_Exception( c3d::ERROR_UNKNOWN_OBJECT );
		}

		if ( config.preferredImporter == cuT( "any" ) )
		{
			if ( auto tit = it->second.find( type );
				tit != it->second.end() )
			{
				return tit->second( engine, scene, file, config, progress );
			}

			return it->second.begin()->second( engine, scene, file, config, progress );
		}

		auto tit = it->second.find( config.preferredImporter );

		if ( tit == it->second.end() )
		{
			CU_Exception( c3d::ERROR_UNKNOWN_OBJECT );
		}

		return tit->second( engine, scene, file, config, progress );
	}

	//*********************************************************************************************
}
