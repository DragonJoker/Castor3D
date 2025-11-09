/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PlyImporter_H___
#define ___C3D_PlyImporter_H___

#include <Castor3D/ImporterFile.hpp>

#include <Castor3D/Model/Mesh/MeshImporter.hpp>

namespace c3d_ply
{
	class PlyImporterFile
		: public c3d::ImporterFile
	{
	public:
		PlyImporterFile( c3d::Engine & engine
			, c3d::Scene * scene
			, c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::ProgressBar * progress );

		static c3d::ImporterFileUPtr create( c3d::Engine & engine
			, c3d::Scene * scene
			, c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::ProgressBar * progress );

		c3d::StringArray listMaterials()override;
		c3d::Vector< MeshData > listMeshes()override;
		c3d::StringArray listSkeletons()override;
		c3d::Vector< NodeData > listSceneNodes()override;
		c3d::Vector< LightData > listLights()override;
		c3d::Vector< LightGroupData > listLightGroups()override;
		c3d::Vector< GeometryData > listGeometries()override;
		c3d::Vector< CameraData > listCameras()override;
		c3d::StringArray listMeshAnimations( c3d::Mesh const & mesh )override;
		c3d::StringArray listSkeletonAnimations( c3d::Skeleton const & skeleton )override;
		c3d::StringArray listSceneNodeAnimations( c3d::SceneNode const & node )override;
		c3d::Vector< uint32_t > listTextureAnimations( c3d::Material const & material
			, uint32_t pass )override;
		uint32_t countAllMeshAnimations()const override;
		uint32_t countAllSkeletonAnimations()const override;
		uint32_t countAllSceneNodeAnimations()const override;
		uint32_t countAllTextureAnimations()const override;

		c3d::MaterialImporterUPtr createMaterialImporter()override;
		c3d::AnimationImporterUPtr createAnimationImporter()override;
		c3d::SkeletonImporterUPtr createSkeletonImporter()override;
		c3d::MeshImporterUPtr createMeshImporter()override;
		c3d::SceneNodeImporterUPtr createSceneNodeImporter()override;
		c3d::LightImporterUPtr createLightImporter()override;
		c3d::CameraImporterUPtr createCameraImporter()override;

	public:
		static c3d::MbString const Name;
	};

	class PlyMeshImporter
		: public c3d::MeshImporter
	{
	public:
		explicit PlyMeshImporter( c3d::Engine & engine );

	private:
		bool doImportMesh( c3d::Mesh & mesh, uint32_t submeshIndex )override;
	};
}

#endif
