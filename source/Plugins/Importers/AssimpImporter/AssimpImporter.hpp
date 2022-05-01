/*
See LICENSE file in root folder
*/
#ifndef ___C3DAssimp_Importer___
#define ___C3DAssimp_Importer___

#include "SceneImporter.hpp"

#include <Castor3D/Model/Mesh/Importer.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#pragma warning( pop )

namespace c3d_assimp
{
	class AssimpImporter
		: public castor3d::MeshImporter
	{
	public:
		explicit AssimpImporter( castor3d::Engine & engine );

		static castor3d::MeshImporterUPtr create( castor3d::Engine & engine );

	private:
		/**
		 *\copydoc		castor3d::MeshImporter::doImportMesh
		 */
		bool doImportMesh( castor3d::Mesh & mesh )override;
		bool doImportScene( castor3d::Scene & scene )override;

		aiScene const * doLoadScene();

	public:
		static castor::String const Name;

	private:
		Assimp::Importer m_importer;
		MaterialImporter m_materialsImp;
		SkeletonImporter m_skeletonsImp;
		MeshesImporter m_meshesImp;
		SceneImporter m_scenesImp;
		uint32_t m_importFlags{};
		castor::String m_prefix;
	};
}

#endif
