/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CscnImporterFile___
#define ___C3D_CscnImporterFile___

#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"

namespace castor3d
{
	class CscnImporterFile
		: public ImporterFile
	{
	public:
		CscnImporterFile( Engine & engine
			, Scene * scene
			, castor::Path const & path
			, Parameters const & parameters
			, ProgressBar * progress );
		~CscnImporterFile()override;

		static ImporterFileUPtr create( Engine & engine
			, Scene * scene
			, castor::Path const & path
			, Parameters const & parameters
			, ProgressBar * progress );

		castor::StringArray listMaterials()override
		{
			return {};
		}

		castor::Vector< MeshData > listMeshes()override
		{
			return {};
		}

		castor::StringArray listSkeletons()override
		{
			return {};
		}

		castor::Vector< NodeData > listSceneNodes()override
		{
			return {};
		}

		castor::Vector< LightData > listLights()override
		{
			return {};
		}

		castor::Vector< LightGroupData > listLightGroups()override
		{
			return {};
		}

		castor::Vector< GeometryData > listGeometries()override
		{
			return {};
		}

		castor::Vector< CameraData > listCameras()override
		{
			return {};
		}

		castor::StringArray listMeshAnimations( Mesh const & mesh )override
		{
			return {};
		}

		castor::StringArray listSkeletonAnimations( Skeleton const & skeleton )override
		{
			return {};
		}

		castor::StringArray listSceneNodeAnimations( SceneNode const & node )override
		{
			return {};
		}

		castor::Vector< uint32_t > listTextureAnimations( Material const & material
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
		static castor::MbString const Name;

	private:
		SceneFileParser m_parser;
	};
}

#endif
