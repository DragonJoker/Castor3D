/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneImporter_H___
#define ___C3D_SceneImporter_H___

#include "SceneModule.hpp"

#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Animation/AnimationImporter.hpp"
#include "Castor3D/Material/MaterialImporter.hpp"
#include "Castor3D/Model/Mesh/MeshImporter.hpp"
#include "Castor3D/Model/Skeleton/SkeletonImporter.hpp"

namespace c3d
{
	class SceneImporter
		: public OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit SceneImporter( Engine & engine );
		/**
		 *\~english
		 *\brief		Scene import Function.
		 *\param[out]	scene			Receives the imported data.
		 *\param[in]	file			The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\param[in]	textureRemaps	The imported textures remapping parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Scene.
		 *\param[out]	scene			Reçoit les données importées.
		 *\param[in]	file			Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\param[in]	textureRemaps	Les paramètres de reaffectation des textures importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importData( Scene & scene
			, ImporterFile * file
			, Parameters const & parameters
			, Map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps );
		/**
		 *\~english
		 *\brief			Scene import Function.
		 *\param[out]		scene			Receives the imported data.
		 *\param[in]		pathFile		The location of the file to import.
		 *\param[in]		parameters		Import configuration parameters.
		 *\param[in]		textureRemaps	The imported textures remapping parameters.
		 *\param[in,out]	progress		Optional progress bar.
		 *\return			\p false if any problem occured.
		 *\~french
		 *\brief			Fonction d'import de Scene.
		 *\param[out]		scene			Reçoit les données importées.
		 *\param[in]		pathFile		Le chemin vers le fichier à importer.
		 *\param[in]		parameters		Paramètres de configuration de l'import.
		 *\param[in]		textureRemaps	Les paramètres de reaffectation des textures importées.
		 *\param[in,out]	progress		Barre de progression optionnelle.
		 *\return			\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importData( Scene & scene
			, Path const & pathFile
			, Parameters const & parameters
			, Map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps
			, ProgressBar * progress = nullptr );
		/**
		 *\~english
		 *\brief		Scene animations import Function.
		 *\param[out]	scene		Receives the imported data.
		 *\param[in]	file		The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import des animations de Scene.
		 *\param[out]	scene		Reçoit les données importées.
		 *\param[in]	file		Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importAnimationsData( Scene & scene
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief			Scene animations import Function.
		 *\param[out]		scene			Receives the imported data.
		 *\param[in]		pathFile		The location of the file to import.
		 *\param[in]		parameters		Import configuration parameters.
		 *\param[in,out]	progress		Optional progress bar.
		 *\return			\p false if any problem occured.
		 *\~french
		 *\brief			Fonction d'import des animations de Scene.
		 *\param[out]		scene			Reçoit les données importées.
		 *\param[in]		pathFile		Le chemin vers le fichier à importer.
		 *\param[in]		parameters		Paramètres de configuration de l'import.
		 *\param[in,out]	progress		Barre de progression optionnelle.
		 *\return			\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importAnimationsData( Scene & scene
			, Path const & pathFile
			, Parameters const & parameters
			, ProgressBar * progress = nullptr );

	private:
		void doImportMaterials( Scene & scene
			, Parameters const & parameters
			, Map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps );
		StringMap< SkeletonRPtr > doImportSkeletons( Scene & scene );
		StringMap< MeshResPtr > doImportMeshes( Scene & scene
			, StringMap< SkeletonRPtr > const & skeletons );
		StringMap< SceneNodeRPtr > doImportNodes( Scene & scene );
		void doImportLights( Scene & scene );
		void doImportLightGroups( Scene & scene );
		void doImportCameras( Scene & scene );
		void doCreateGeometries( Scene & scene
			, StringMap< MeshResPtr > const & meshes
			, StringMap< SceneNodeRPtr > const & nodes );

		struct AnimObjects
		{
			AnimObjects() = default;
			Vector< SkeletonRPtr > skeletons;
			Vector< MeshRPtr > meshes;
			Vector< SceneNodeRPtr > nodes;
		};

		void doImportSkeletonsAnims( Scene & scene
			, AnimationImporter & importer
			, StringMap< AnimObjects > & anims );
		void doImportMeshesAnims( Scene & scene
			, AnimationImporter & importer
			, StringMap< AnimObjects > & anims );
		void doImportNodesAnims( Scene & scene
			, AnimationImporter & importer
			, StringMap< AnimObjects > & anims );
		void doImportTexturesAnims( Scene & scene
			, AnimationImporter & importer );
		void doCreateAnimationGroups( Scene & scene
			, StringMap< AnimObjects > & anims )const;

		void doTransformScene( Scene & scene
			, Parameters const & parameters
			, StringMap< SceneNodeRPtr > const & nodes )const;
		void doAddAnimationGroup( Geometry & geometry );

		void doCenterCamera( Scene & scene
			, Parameters const & parameters )const;

	private:
		ImporterFile * m_file{};
		SceneNodePtrArray m_nodes;
	};

	struct SceneContext;

	struct SceneImportContext
	{
		SceneContext * scene{};
		PathArray files{};
		PathArray animFiles{};
		String prefix{};
		Map< PassComponentTextureFlag, TextureConfiguration > textureRemaps;
		float rescale{ 1.0f };
		float pitch{ 0.0f };
		float yaw{ 0.0f };
		float roll{ 0.0f };
		bool noOptimisations{ false };
		bool noValidation{ false };
		bool ignoreVertexColour{ false };
		bool disableImageCompression{ false };
		float emissiveMult{ 1.0f };
		Map< PassComponentTextureFlag, TextureConfiguration >::iterator textureRemapIt;
		String centerCamera{};
		String preferredImporter{ cuT( "any" ) };
	};

	C3D_API String getPrefix( SceneImportContext const & context );
	C3D_API Engine * getEngine( SceneImportContext const & context );
}

#endif
