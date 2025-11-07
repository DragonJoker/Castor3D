/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjImporterFile___
#define ___C3D_ObjImporterFile___

#include <AssimpImporter/AssimpImporterFile.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Obj_API
#else
#	ifdef ObjImporter_EXPORTS
#		define C3D_Obj_API __declspec(dllexport)
#	else
#		define C3D_Obj_API __declspec(dllimport)
#	endif
#endif

namespace c3d_obj
{
	struct ObjMaterialData
	{
		explicit ObjMaterialData( c3d::Vector< c3d::String > pmaterialLines )
			: materialLines{ c3d::move( pmaterialLines ) }
		{
		}

		c3d::Vector< c3d::String > materialLines;
	};

	struct ObjSceneData
	{
		c3d::StringMap< ObjMaterialData > materials;
	};

	class ObjImporterFile
		: public c3d_assimp::AssimpImporterFile
	{
	public:
		C3D_Obj_API ObjImporterFile( c3d::Engine & engine
			, c3d::Scene * scene
			, c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::ProgressBar * progress );

		static c3d::ImporterFileUPtr create( c3d::Engine & engine
			, c3d::Scene * scene
			, c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::ProgressBar * progress );

		using c3d::ImporterFile::getInternalName;

		C3D_Obj_API c3d::StringArray listMaterials()override;
		C3D_Obj_API c3d::Vector< uint32_t > listTextureAnimations( c3d::Material const & material
			, uint32_t pass )override;
		C3D_Obj_API c3d::MaterialImporterUPtr createMaterialImporter()override;

		bool isValid()const noexcept
		{
			return true;
		}

		auto & getObjMaterials()const noexcept
		{
			return m_sceneData.materials;
		}

	public:
		static c3d::MbString const Name;

	private:
		void doPrelistMaterials( c3d::Vector< c3d::Path > const & mtlFilesPaths );

	private:
		ObjSceneData m_sceneData;
	};
}

#endif
