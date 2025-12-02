#include "PlyMeshImporter/PlyMeshFile.hpp"
#include "PlyMeshImporter/PlyMeshImporter.hpp"

namespace c3d_ply
{
	PlyMeshFile::PlyMeshFile( c3d::Path const & path
		, c3d::HashMap< c3d::String, c3d::HashMap< c3d::u32, c3d::String > > const & meshesNames )
	{
		m_meshName = path.getFileName();
		if ( auto it = meshesNames.find( m_meshName );
			it != meshesNames.end() )
			m_meshName = it->second.begin()->second;
	}

	c3d::Vector< c3d::ImporterFile::MeshData > PlyMeshFile::listMeshes()
	{
		c3d::Vector< c3d::ImporterFile::MeshData > result;
		result.emplace_back( m_meshName, c3d::String{} );
		return result;
	}

	c3d::MeshImporterUPtr PlyMeshFile::createMeshImporter( c3d::Engine & engine )
	{
		return c3d::makeUniqueDerived< c3d::MeshImporter, PlyMeshImporter >( engine );
	}
}
