/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CmshImporter_H___
#define ___C3D_CmshImporter_H___

#include "BinaryModule.hpp"

#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Animation/AnimationImporter.hpp"
#include "Castor3D/Material/MaterialImporter.hpp"
#include "Castor3D/Model/Mesh/MeshImporter.hpp"
#include "Castor3D/Model/Skeleton/SkeletonImporter.hpp"
#include "Castor3D/Scene/SceneNodeImporter.hpp"
#include "Castor3D/Scene/Light/LightImporter.hpp"

namespace c3d
{
	class CmshImporterFile
		: public ImporterFile
	{
	private:
		CmshImporterFile( Engine & engine
			, Scene * scene
			, Path const & path
			, Parameters const & parameters
			, ProgressBar * progress );

	public:
		StringArray listMaterials()override;
		Vector< MeshData > listMeshes()override;
		StringArray listSkeletons()override;
		Vector< NodeData > listSceneNodes()override;
		Vector< LightData > listLights()override;
		Vector< LightGroupData > listLightGroups()override;
		Vector< GeometryData > listGeometries()override;
		Vector< CameraData > listCameras()override;
		StringArray listMeshAnimations( Mesh const & mesh )override;
		StringArray listSkeletonAnimations( Skeleton const & skeleton )override;
		StringArray listSceneNodeAnimations( SceneNode const & node )override;
		Vector< uint32_t > listTextureAnimations( Material const & material
			, uint32_t pass )override;
		uint32_t countAllMeshAnimations()const override;
		uint32_t countAllSkeletonAnimations()const override;
		uint32_t countAllSceneNodeAnimations()const override;
		uint32_t countAllTextureAnimations()const override;

		MaterialImporterUPtr createMaterialImporter()override;
		AnimationImporterUPtr createAnimationImporter()override;
		SkeletonImporterUPtr createSkeletonImporter()override;
		MeshImporterUPtr createMeshImporter()override;
		SceneNodeImporterUPtr createSceneNodeImporter()override;
		LightImporterUPtr createLightImporter()override;
		CameraImporterUPtr createCameraImporter()override;

		static ImporterFileUPtr create( Engine & engine
			, Scene * scene
			, Path const & path
			, Parameters const & parameters
			, ProgressBar * progress );
	};

	class CmshMeshImporter
		: public MeshImporter
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The core engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit CmshMeshImporter( Engine & engine );
		/**
		 *\~english
		 *\brief		Creator function.
		 *\param[in]	engine	The core engine.
		 *\~french
		 *\brief		Fonction de création.
		 *\param[in]	engine	Le moteur.
		 */
		static MeshImporterUPtr create( Engine & engine );

	protected:
		/**
		 *\copydoc		MeshImporter::doImportMesh
		 */
		C3D_API bool doImportMesh( Mesh & mesh, uint32_t submeshIndex )override;

	public:
		static String const Type;
	};

	class CmshSkeletonImporter
		: public SkeletonImporter
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The core engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit CmshSkeletonImporter( Engine & engine );
		/**
		 *\~english
		 *\brief		Creator function.
		 *\param[in]	engine	The core engine.
		 *\~french
		 *\brief		Fonction de création.
		 *\param[in]	engine	Le moteur.
		 */
		static SkeletonImporterUPtr create( Engine & engine );

	protected:
		/**
		 *\copydoc		SkeletonImporter::doImportSkeleton
		 */
		C3D_API bool doImportSkeleton( Skeleton & skeleton )override;

	public:
		static String const Type;
	};

	class CmshAnimationImporter
		: public AnimationImporter
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The core engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit CmshAnimationImporter( Engine & engine );
		/**
		 *\~english
		 *\brief		Creator function.
		 *\param[in]	engine	The core engine.
		 *\~french
		 *\brief		Fonction de création.
		 *\param[in]	engine	Le moteur.
		 */
		static AnimationImporterUPtr create( Engine & engine );

	private:
		/**
		 *\copydoc		AnimationImporter::doImportSkeleton
		 */
		bool doImportSkeleton( SkeletonAnimation & skeleton )override;
		/**
		 *\copydoc		AnimationImporter::doImportMesh
		 */
		bool doImportMesh( MeshAnimation & mesh )override;
		/**
		 *\copydoc		AnimationImporter::doImportNode
		 */
		bool doImportNode( SceneNodeAnimation & node )override;
		/**
		 *\copydoc		AnimationImporter::doImportTexture
		 */
		bool doImportTexture( TextureAnimation & texture )override;

	public:
		static String const MeshAnimType;
		static String const SkeletonAnimType;
		static String const NodeAnimType;
	};
}

#endif
