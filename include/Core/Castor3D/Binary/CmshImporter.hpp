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

namespace castor3d
{
	class CmshImporterFile
		: public ImporterFile
	{
	private:
		CmshImporterFile( Engine & engine
			, Scene * scene
			, castor::Path const & path
			, Parameters const & parameters
			, ProgressBar * progress );

	public:
		castor::StringArray listMaterials()override;
		castor::Vector< MeshData > listMeshes()override;
		castor::StringArray listSkeletons()override;
		castor::Vector< NodeData > listSceneNodes()override;
		castor::Vector< LightData > listLights()override;
		castor::Vector< GeometryData > listGeometries()override;
		castor::Vector< CameraData > listCameras()override;
		castor::StringArray listMeshAnimations( Mesh const & mesh )override;
		castor::StringArray listSkeletonAnimations( Skeleton const & skeleton )override;
		castor::StringArray listSceneNodeAnimations( SceneNode const & node )override;
		castor::StringArray listAllMeshAnimations()override;
		castor::StringArray listAllSkeletonAnimations()override;
		castor::StringArray listAllSceneNodeAnimations()override;

		MaterialImporterUPtr createMaterialImporter()override;
		AnimationImporterUPtr createAnimationImporter()override;
		SkeletonImporterUPtr createSkeletonImporter()override;
		MeshImporterUPtr createMeshImporter()override;
		SceneNodeImporterUPtr createSceneNodeImporter()override;
		LightImporterUPtr createLightImporter()override;
		CameraImporterUPtr createCameraImporter()override;

		static ImporterFileUPtr create( Engine & engine
			, Scene * scene
			, castor::Path const & path
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
		 *\copydoc		castor3d::MeshImporter::doImportMesh
		 */
		C3D_API bool doImportMesh( Mesh & mesh )override;

	public:
		static castor::String const Type;
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
		 *\copydoc		castor3d::SkeletonImporter::doImportSkeleton
		 */
		C3D_API bool doImportSkeleton( Skeleton & skeleton )override;

	public:
		static castor::String const Type;
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

	protected:
		/**
		 *\copydoc		castor3d::AnimationImporter::doImportSkeleton
		 */
		C3D_API bool doImportSkeleton( SkeletonAnimation & skeleton )override;
		/**
		 *\copydoc		castor3d::AnimationImporter::doImportMesh
		 */
		C3D_API bool doImportMesh( MeshAnimation & mesh )override;
		/**
		 *\copydoc		castor3d::AnimationImporter::doImportNode
		 */
		C3D_API bool doImportNode( SceneNodeAnimation & node )override;

	public:
		static castor::String const MeshAnimType;
		static castor::String const SkeletonAnimType;
		static castor::String const NodeAnimType;
	};
}

#endif
