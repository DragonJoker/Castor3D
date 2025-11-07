/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FbxImporterFile___
#define ___C3D_FbxImporterFile___

#include <AssimpImporter/AssimpImporterFile.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <fbxsdk.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Fbx_API
#else
#	ifdef FbxImporter_EXPORTS
#		define C3D_Fbx_API __declspec(dllexport)
#	else
#		define C3D_Fbx_API __declspec(dllimport)
#	endif
#endif

namespace fbx = fbxsdk;

namespace c3d_fbx
{
	struct FbxMaterialData
	{
		explicit FbxMaterialData( fbx::FbxSurfaceMaterial * pmaterial )
			: fbxMaterial{ pmaterial }
		{
		}

		fbx::FbxSurfaceMaterial * fbxMaterial;
	};

	struct FbxSceneData
	{
		c3d::StringMap< FbxMaterialData > materials;
	};

	class FbxImporterFile
		: public c3d_assimp::AssimpImporterFile
	{
	public:
		C3D_Fbx_API FbxImporterFile( c3d::Engine & engine
			, c3d::Scene * scene
			, c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::ProgressBar * progress );
		C3D_Fbx_API ~FbxImporterFile()noexcept override;

		static c3d::ImporterFileUPtr create( c3d::Engine & engine
			, c3d::Scene * scene
			, c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::ProgressBar * progress );

		using c3d::ImporterFile::getInternalName;

		C3D_Fbx_API c3d::StringArray listMaterials()override;
		C3D_Fbx_API c3d::Vector< uint32_t > listTextureAnimations( c3d::Material const & material
			, uint32_t pass )override;
		C3D_Fbx_API c3d::MaterialImporterUPtr createMaterialImporter()override;

		FbxManager & getFbxManager()const noexcept
		{
			return *m_fbxManager;
		}

		FbxScene & getFbxScene()const noexcept
		{
			return *m_fbxScene;
		}

		bool isValid()const noexcept
		{
			return m_fbxManager && m_fbxScene;
		}

		auto & getMaterials()const noexcept
		{
			return m_sceneData.materials;
		}

	public:
		static c3d::MbString const Name;

	private:
		void doPrelistMaterials();

	private:
		FbxManager * m_fbxManager;
		FbxScene * m_fbxScene;
		FbxSceneData m_sceneData;
	};
}

#endif
