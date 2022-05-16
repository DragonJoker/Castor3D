/*
See LICENSE file in root folder
*/
#ifndef ___PLY_IMPORTER_H___
#define ___PLY_IMPORTER_H___

#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Castor3D/ImporterFile.hpp>
#include <Castor3D/Animation/AnimationImporter.hpp>
#include <Castor3D/Material/MaterialImporter.hpp>
#include <Castor3D/Model/Mesh/MeshImporter.hpp>
#include <Castor3D/Model/Skeleton/SkeletonImporter.hpp>
#include <Castor3D/Scene/SceneNodeImporter.hpp>
#include <Castor3D/Scene/Light/LightImporter.hpp>

#pragma warning( pop )

namespace C3dPly
{
	class PlyImporterFile
		: public castor3d::ImporterFile
	{
	public:
		PlyImporterFile( castor3d::Engine & engine
			, castor::Path const & path
			, castor3d::Parameters const & parameters );

		std::vector< castor::String > listMaterials()override;
		std::vector< castor::String > listMeshes()override;
		std::vector< castor::String > listSkeletons()override;
		std::vector< castor::String > listSceneNodes()override;
		std::vector< std::pair< castor::String, castor3d::LightType > > listLights()override;
		std::vector< castor::String > listMeshAnimations( castor3d::Mesh const & mesh )override;
		std::vector< castor::String > listSkeletonAnimations( castor3d::Skeleton const & skeleton )override;
		std::vector< castor::String > listSceneNodeAnimations( castor3d::SceneNode const & node )override;

		castor3d::MaterialImporterUPtr createMaterialImporter()override;
		castor3d::AnimationImporterUPtr createAnimationImporter()override;
		castor3d::SkeletonImporterUPtr createSkeletonImporter()override;
		castor3d::MeshImporterUPtr createMeshImporter()override;
		castor3d::SceneNodeImporterUPtr createSceneNodeImporter()override;
		castor3d::LightImporterUPtr createLightImporter()override;

		static castor3d::ImporterFileUPtr create( castor3d::Engine & engine
			, castor::Path const & path
			, castor3d::Parameters const & parameters );
	};

	class PlyMeshImporter
		: public castor3d::MeshImporter
	{
	public:
		/**
		 * Constructor
		 */
		explicit PlyMeshImporter( castor3d::Engine & engine );

	private:
		/**
		 *\copydoc		castor3d::MeshImporter::doImportMesh
		 */
		bool doImportMesh( castor3d::Mesh & mesh )override;
	};
}

#endif
