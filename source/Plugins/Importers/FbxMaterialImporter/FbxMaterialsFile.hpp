/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FbxMaterialsFile___
#define ___C3D_FbxMaterialsFile___

#include <Castor3D/Material/MaterialModule.hpp>
#include <Castor3D/Miscellaneous/MiscellaneousModule.hpp>

#ifndef CU_PlatformWindows
#	define C3D_FbxMat_API
#else
#	ifdef FbxMaterialImporter_EXPORTS
#		define C3D_FbxMat_API __declspec(dllexport)
#	else
#		define C3D_FbxMat_API __declspec(dllimport)
#	endif
#endif

namespace fbxsdk
{
	class FbxManager;
	class FbxScene;
	class FbxSurfaceMaterial;
}

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

	class FbxMaterialsFile
	{
	public:
		C3D_FbxMat_API FbxMaterialsFile( c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::HashMap< c3d::String, c3d::HashMap< c3d::u32, c3d::String > > const & materialsNames );
		C3D_FbxMat_API ~FbxMaterialsFile()noexcept;
		C3D_FbxMat_API c3d::MaterialImporterUPtr createMaterialImporter( c3d::Engine & engine );

		fbx::FbxManager & getFbxManager()const noexcept
		{
			return *m_fbxManager;
		}

		fbx::FbxScene & getFbxScene()const noexcept
		{
			return *m_fbxScene;
		}

		bool isValid()const noexcept
		{
			return m_fbxManager && m_fbxScene;
		}

		auto & getMaterials()const noexcept
		{
			return m_materials;
		}

	private:
		void doPrelistMaterials( c3d::Parameters const & parameters
			, c3d::HashMap< c3d::String, c3d::HashMap< c3d::u32, c3d::String > > const & materialsNames );

	private:
		fbx::FbxManager * m_fbxManager;
		fbx::FbxScene * m_fbxScene;
		c3d::StringMap< FbxMaterialData > m_materials;
	};
}

#endif
