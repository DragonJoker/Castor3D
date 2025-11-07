#include "ObjImporter/ObjImporterFile.hpp"

#include "ObjImporter/ObjMaterialImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>

namespace c3d_obj
{
	//*********************************************************************************************

	c3d::MbString const ObjImporterFile::Name = "OBJ Importer";

	ObjImporterFile::ObjImporterFile( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::ProgressBar * progress )
		: c3d_assimp::AssimpImporterFile{ engine, scene, path, parameters, progress }
	{
		std::ifstream file( c3d::toUtf8( path ).c_str() );
		std::string line;
		c3d::Vector< c3d::Path > mtlFilesNames;
		while ( std::getline( file, line ) )
		{
			c3d::string::trim( line );
			std::stringstream stream{ line };
			std::string ident;
			stream >> ident;
			if ( ident == "mtllib" )
			{
				auto mtlFileName = line.substr( line.find_first_of( " " ) + 1 );
				c3d::string::trim( mtlFileName );
				if ( c3d::File::fileExists( path.getPath() / mtlFileName ) )
					mtlFilesNames.emplace_back( path.getPath() / mtlFileName );
			}
		}

		if ( !mtlFilesNames.empty() )
			doPrelistMaterials( mtlFilesNames );
	}

	c3d::ImporterFileUPtr ObjImporterFile::create( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::ProgressBar * progress )
	{
		return c3d::makeUniqueDerived< c3d::ImporterFile, ObjImporterFile >( engine, scene, path, parameters, progress );
	}

	c3d::StringArray ObjImporterFile::listMaterials()
	{
		c3d::StringArray result;
		if ( isValid() )
			for ( auto const & [name, _] : m_sceneData.materials )
				result.emplace_back( name );
		return result;
	}

	c3d::Vector< uint32_t > ObjImporterFile::listTextureAnimations( c3d::Material const & material
		, uint32_t pass )
	{
		c3d::Vector< uint32_t > result;
		return result;
	}

	c3d::MaterialImporterUPtr ObjImporterFile::createMaterialImporter()
	{
		return c3d::makeUniqueDerived< c3d::MaterialImporter, ObjMaterialImporter >( *getOwner() );
	}

	void ObjImporterFile::doPrelistMaterials( c3d::Vector< c3d::Path > const & mtlFilesPaths )
	{
		for ( auto const & mtlFilePath : mtlFilesPaths )
		{
			c3d::TextFile mtlFile{ mtlFilePath, c3d::File::OpenMode::eRead };
			c3d::String materialName;
			c3d::String mtlLine;
			c3d::Vector< c3d::String > materialLines;

			while ( mtlFile.isOk() )
			{
				mtlFile.readLine( mtlLine, 1000 );
				auto idx = mtlLine.find( cuT( "newmtl" ) );
				if ( idx != c3d::String::npos )
				{
					if ( !materialLines.empty() && !materialName.empty() )
						m_sceneData.materials.try_emplace( materialName, c3d::move( materialLines ) );

					materialLines = {};
					idx = mtlLine.find_first_not_of( ' ', 6u );
					materialName = getMaterialName( mtlLine.substr( idx ) );
				}
				else if ( !mtlLine.empty() )
				{
					materialLines.push_back( mtlLine );
				}
			}

			if ( !materialLines.empty() )
				m_sceneData.materials.try_emplace( materialName, c3d::move( materialLines ) );
		}
	}

	//*********************************************************************************************
}
