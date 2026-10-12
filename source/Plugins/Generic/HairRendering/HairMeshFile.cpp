#include "HairRendering/HairMeshFile.hpp"
#include "HairRendering/HairMeshImporter.hpp"

namespace hair
{
	c3d::MbString const MeshImporterFile::Name = "HAIR Importer";

	MeshImporterFile::MeshImporterFile( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::ImportConfig const & config
		, c3d::ProgressBar * progress )
		: c3d::ImporterFile{ engine, scene, path, config, progress }
	{
		m_meshName = path.getFileName();
	}

	c3d::ImporterFileUPtr MeshImporterFile::create( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::ImportConfig const & config
		, c3d::ProgressBar * progress )
	{
		return c3d::makeUniqueDerived< c3d::ImporterFile, MeshImporterFile >( engine, scene, path, config, progress );
	}

	c3d::Vector< c3d::ImporterFile::MeshData > MeshImporterFile::listMeshes()
	{
		c3d::Vector< c3d::ImporterFile::MeshData > result;
		result.emplace_back( m_meshName, c3d::String{} );
		return result;
	}

	c3d::MeshImporterUPtr MeshImporterFile::createMeshImporter()
	{
		return c3d::makeUniqueDerived< c3d::MeshImporter, MeshImporter >( *getEngine(), getConfig().meshConfig );
	}
}
