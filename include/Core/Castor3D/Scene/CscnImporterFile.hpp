/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CscnImporterFile___
#define ___C3D_CscnImporterFile___

#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"

namespace c3d
{
	class CscnImporterFile
		: public ImporterFile
	{
	public:
		CscnImporterFile( Engine & engine
			, Scene * scene
			, Path const & path
			, Parameters const & parameters
			, ProgressBar * progress );
		~CscnImporterFile()override;

		static ImporterFileUPtr create( Engine & engine
			, Scene * scene
			, Path const & path
			, Parameters const & parameters
			, ProgressBar * progress );

		StringArray listMaterials()override
		{
			return {};
		}

		Vector< MeshData > listMeshes()override
		{
			return {};
		}

		StringArray listSkeletons()override
		{
			return {};
		}

		Vector< NodeData > listSceneNodes()override
		{
			return {};
		}

		Vector< LightData > listLights()override
		{
			return {};
		}

		Vector< LightGroupData > listLightGroups()override
		{
			return {};
		}

		Vector< GeometryData > listGeometries()override
		{
			return {};
		}

		Vector< CameraData > listCameras()override
		{
			return {};
		}

		StringArray listMeshAnimations( Mesh const & mesh )override
		{
			return {};
		}

		StringArray listSkeletonAnimations( Skeleton const & skeleton )override
		{
			return {};
		}

		StringArray listSceneNodeAnimations( SceneNode const & node )override
		{
			return {};
		}

		Vector< uint32_t > listTextureAnimations( Material const & material
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

		MaterialImporterUPtr createMaterialImporter()override
		{
			return {};
		}

		AnimationImporterUPtr createAnimationImporter()override
		{
			return {};
		}

		SkeletonImporterUPtr createSkeletonImporter()override
		{
			return {};
		}

		MeshImporterUPtr createMeshImporter()override
		{
			return {};
		}

		SceneNodeImporterUPtr createSceneNodeImporter()override
		{
			return {};
		}

		LightImporterUPtr createLightImporter()override
		{
			return {};
		}

		CameraImporterUPtr createCameraImporter()override
		{
			return {};
		}

	public:
		static MbString const Name;

	private:
		SceneFileParser m_parser;
	};
}

#endif
