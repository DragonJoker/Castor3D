/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PlyMeshFile_H___
#define ___C3D_PlyMeshFile_H___

#include <Castor3D/ImporterFile.hpp>

#include <Castor3D/Model/Mesh/MeshImporter.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Ply_API
#else
#	ifdef PlyMeshImporter_EXPORTS
#		define C3D_Ply_API __declspec(dllexport)
#	else
#		define C3D_Ply_API __declspec(dllimport)
#	endif
#endif

namespace c3d_ply
{
	class PlyMeshFile
	{
	public:
		C3D_Ply_API PlyMeshFile( c3d::Path const & path
		, c3d::HashMap< c3d::String, c3d::String > const & meshesNames );

		C3D_Ply_API c3d::Vector< c3d::ImporterFile::MeshData > listMeshes();
		C3D_Ply_API c3d::MeshImporterUPtr createMeshImporter( c3d::Engine & engine );

	private:
		c3d::String m_meshName;
	};
}

#endif
