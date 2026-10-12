/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HairMeshFile_H___
#define ___C3D_HairMeshFile_H___

#include <Castor3D/ImporterFile.hpp>

#include <Castor3D/Model/Mesh/MeshImporter.hpp>

namespace hair
{
	class MeshImporterFile
		: public c3d::ImporterFile
	{
	public:
		MeshImporterFile( c3d::Engine & engine
			, c3d::Scene * scene
			, c3d::Path const & path
			, c3d::ImportConfig const & config
			, c3d::ProgressBar * progress );

		static c3d::ImporterFileUPtr create( c3d::Engine & engine
			, c3d::Scene * scene
			, c3d::Path const & path
			, c3d::ImportConfig const & config
			, c3d::ProgressBar * progress );

		c3d::Vector< MeshData > listMeshes()override;
		c3d::MeshImporterUPtr createMeshImporter()override;

		c3d::StringArray listMaterials()override
		{
			return {};
		}

		c3d::StringArray listSkeletons()override
		{
			return {};
		}

		c3d::Vector< NodeData > listSceneNodes()override
		{
			return {};
		}

		c3d::Vector< LightData > listLights()override
		{
			return {};
		}

		c3d::Vector< LightGroupData > listLightGroups()override
		{
			return {};
		}

		c3d::Vector< GeometryData > listGeometries()override
		{
			return {};
		}

		c3d::Vector< CameraData > listCameras()override
		{
			return {};
		}

		c3d::StringArray listMeshAnimations( c3d::Mesh const & mesh )override
		{
			return {};
		}

		c3d::StringArray listSkeletonAnimations( c3d::Skeleton const & skeleton )override
		{
			return {};
		}

		c3d::StringArray listSceneNodeAnimations( c3d::SceneNode const & node )override
		{
			return {};
		}

		c3d::Vector< uint32_t > listTextureAnimations( c3d::Material const & material
			, uint32_t pass )override
		{
			return {};
		}

		uint32_t countAllMeshAnimations()const override
		{
			return {};
		}

		uint32_t countAllSkeletonAnimations()const override
		{
			return {};
		}

		uint32_t countAllSceneNodeAnimations()const override
		{
			return {};
		}

		uint32_t countAllTextureAnimations()const override
		{
			return {};
		}

		c3d::MaterialImporterUPtr createMaterialImporter()override
		{
			return {};
		}

		c3d::AnimationImporterUPtr createAnimationImporter()override
		{
			return {};
		}

		c3d::SkeletonImporterUPtr createSkeletonImporter()override
		{
			return {};
		}

		c3d::SceneNodeImporterUPtr createSceneNodeImporter()override
		{
			return {};
		}

		c3d::LightImporterUPtr createLightImporter()override
		{
			return {};
		}

		c3d::CameraImporterUPtr createCameraImporter()override
		{
			return {};
		}

	public:
		static c3d::MbString const Name;

	private:
		c3d::String m_meshName;
	};
}

#endif
