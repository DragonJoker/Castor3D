/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneImporter_H___
#define ___C3D_SceneImporter_H___

#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Animation/AnimationModule.hpp"

#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

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
		 *\param[out]	scene	Receives the imported data.
		 *\param[in]	file	The location of the file to import.
		 *\param[in]	config	Import configuration.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Scene.
		 *\param[out]	scene	Reçoit les données importées.
		 *\param[in]	file	Le chemin vers le fichier à importer.
		 *\param[in]	config	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importData( Scene & scene
			, ImporterFile * file
			, ImportConfig const & config );
		/**
		 *\~english
		 *\brief			Scene import Function.
		 *\param[out]		scene		Receives the imported data.
		 *\param[in]		pathFile	The location of the file to import.
		 *\param[in]		config		Import configuration parameters.
		 *\param[in,out]	progress	Optional progress bar.
		 *\return			\p false if any problem occured.
		 *\~french
		 *\brief			Fonction d'import de Scene.
		 *\param[out]		scene		Reçoit les données importées.
		 *\param[in]		pathFile	Le chemin vers le fichier à importer.
		 *\param[in]		config		Paramètres de configuration de l'import.
		 *\param[in,out]	progress	Barre de progression optionnelle.
		 *\return			\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importData( Scene & scene
			, Path const & pathFile
			, ImportConfig const & config
			, ProgressBar * progress = nullptr );
		/**
		 *\~english
		 *\brief		Scene animations import Function.
		 *\param[out]	scene	Receives the imported data.
		 *\param[in]	file	The location of the file to import.
		 *\param[in]	config	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import des animations de Scene.
		 *\param[out]	scene	Reçoit les données importées.
		 *\param[in]	file	Le chemin vers le fichier à importer.
		 *\param[in]	config	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importAnimationsData( Scene & scene
			, ImporterFile * file
			, ImportConfig const & config );
		/**
		 *\~english
		 *\brief			Scene animations import Function.
		 *\param[out]		scene		Receives the imported data.
		 *\param[in]		pathFile	The location of the file to import.
		 *\param[in]		config		Import configuration parameters.
		 *\param[in,out]	progress	Optional progress bar.
		 *\return			\p false if any problem occured.
		 *\~french
		 *\brief			Fonction d'import des animations de Scene.
		 *\param[out]		scene		Reçoit les données importées.
		 *\param[in]		pathFile	Le chemin vers le fichier à importer.
		 *\param[in]		config		Paramètres de configuration de l'import.
		 *\param[in,out]	progress	Barre de progression optionnelle.
		 *\return			\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importAnimationsData( Scene & scene
			, Path const & pathFile
			, ImportConfig const & config
			, ProgressBar * progress = nullptr );

	private:
		void doImportMaterials( Scene & scene
			, ImportConfig const & config );
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
			, ImportConfig const & config
			, StringMap< SceneNodeRPtr > const & nodes )const;
		void doAddAnimationGroup( Geometry & geometry );

		void doCenterCamera( Scene & scene
			, ImportConfig const & config )const;

	private:
		ImporterFile * m_file{};
		SceneNodePtrArray m_nodes;
	};

	struct SceneContext;

	struct SceneImportContext
	{
		SceneContext * scene{};
		Vector< Pair< Path, ImportConfig > > files{};
		Vector< Pair< Path, ImportConfig > > animFiles{};
		Map< PassComponentTextureFlag, TextureConfiguration >::iterator textureRemapIt;
		ImportConfig config;
	};

	C3D_API String getPrefix( SceneImportContext const & context );
	C3D_API Engine * getEngine( SceneImportContext const & context );
}

#endif
