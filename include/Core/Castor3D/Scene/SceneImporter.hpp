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

namespace castor3d
{
	class SceneImporter
		: public castor::OwnedBy< Engine >
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
		C3D_API bool import( Scene & scene
			, ImporterFile * file
			, Parameters const & parameters
			, std::map< TextureFlag, TextureConfiguration > const & textureRemaps );
		/**
		 *\~english
		 *\brief		Scene import Function.
		 *\param[out]	scene			Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\param[in]	textureRemaps	The imported textures remapping parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Scene.
		 *\param[out]	scene			Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\param[in]	textureRemaps	Les paramètres de reaffectation des textures importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool import( Scene & scene
			, castor::Path const & pathFile
			, Parameters const & parameters
			, std::map< TextureFlag, TextureConfiguration > const & textureRemaps );
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
		C3D_API bool importAnimations( Scene & scene
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Scene animations import Function.
		 *\param[out]	scene			Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import des animations de Scene.
		 *\param[out]	scene			Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importAnimations( Scene & scene
			, castor::Path const & pathFile
			, Parameters const & parameters );

	private:
		void doImportMaterials( Scene & scene
			, Parameters const & parameters
			, std::map< TextureFlag, TextureConfiguration > const & textureRemaps );
		std::map< castor::String, SkeletonRPtr > doImportSkeletons( Scene & scene );
		std::map< castor::String, MeshResPtr > doImportMeshes( Scene & scene
			, std::map< castor::String, SkeletonRPtr > const & skeletons );
		std::map< castor::String, SceneNodeSPtr > doImportNodes( Scene & scene );
		void doImportLights( Scene & scene );
		void doCreateGeometries( Scene & scene
			, std::map< castor::String, MeshResPtr > const & meshes
			, std::map< castor::String, SceneNodeSPtr > const & nodes );

		struct AnimObjects
		{
			std::vector< SkeletonRPtr > skeletons;
			std::vector< MeshRPtr > meshes;
			std::vector< SceneNodeRPtr > nodes;
		};

		void doImportSkeletonsAnims( Scene & scene
			, AnimationImporter & importer
			, std::map< castor::String, AnimObjects > & anims );
		void doImportMeshesAnims( Scene & scene
			, AnimationImporter & importer
			, std::map< castor::String, AnimObjects > & anims );
		void doImportNodesAnims( Scene & scene
			, AnimationImporter & importer
			, std::map< castor::String, AnimObjects > & anims );
		void doCreateAnimationGroups( Scene & scene
			, std::map< castor::String, AnimObjects > & anims );

		void doTransformScene( Scene & scene
			, Parameters const & parameters
			, std::map< castor::String, SceneNodeSPtr > const & nodes );
		void doAddAnimationGroup( Geometry & geometry );

	private:
		ImporterFile * m_file;
		SceneNodePtrArray m_nodes;
	};
}

#endif
