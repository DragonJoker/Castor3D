/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimationImporter_H___
#define ___C3D_AnimationImporter_H___

#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Animation/AnimationModule.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimationModule.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include <CastorUtils/Data/Path.hpp>

namespace castor3d
{
	class AnimationImporter
		: public castor::OwnedBy< Engine >
	{
	public:
		virtual ~AnimationImporter() = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit AnimationImporter( Engine & engine );
		/**
		 *\~english
		 *\brief		SkeletonAnimation import function.
		 *\param[out]	animation	Receives the imported data.
		 *\param[in]	file		The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SkeletonAnimation.
		 *\param[out]	animation	Reçoit les données importées.
		 *\param[in]	file		Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool import( SkeletonAnimation & animation
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		SkeletonAnimation import function.
		 *\param[out]	animation	Receives the imported data.
		 *\param[in]	pathFile	The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SkeletonAnimation.
		 *\param[out]	animation	Reçoit les données importées.
		 *\param[in]	pathFile	Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool import( SkeletonAnimation & animation
			, castor::Path const & pathFile
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		MeshAnimation import function.
		 *\param[out]	animation	Receives the imported data.
		 *\param[in]	file		The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de MeshAnimation.
		 *\param[out]	animation	Reçoit les données importées.
		 *\param[in]	file		Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool import( MeshAnimation & animation
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		MeshAnimation import function.
		 *\param[out]	animation	Receives the imported data.
		 *\param[in]	pathFile	The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de MeshAnimation.
		 *\param[out]	animation	Reçoit les données importées.
		 *\param[in]	pathFile	Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool import( MeshAnimation & animation
			, castor::Path const & pathFile
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		SceneNodeAnimation import function.
		 *\param[out]	animation		Receives the imported data.
		 *\param[in]	file			The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SceneNodeAnimation.
		 *\param[out]	animation		Reçoit les données importées.
		 *\param[in]	file			Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool import( SceneNodeAnimation & animation
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		SceneNodeAnimation import function.
		 *\param[out]	animation		Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SceneNodeAnimation.
		 *\param[out]	animation		Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool import( SceneNodeAnimation & animation
			, castor::Path const & pathFile
			, Parameters const & parameters );

	protected:
		/**
		 *\~english
		 *\brief		SkeletonAnimation import Function.
		 *\param[out]	animation	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SkeletonAnimation.
		 *\param[out]	animation	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool doImportSkeleton( SkeletonAnimation & animation ) = 0;
		/**
		 *\~english
		 *\brief		MeshAnimation import function.
		 *\param[out]	animation	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de MeshAnimation.
		 *\param[out]	animation	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool doImportMesh( MeshAnimation & animation ) = 0;
		/**
		 *\~english
		 *\brief		SceneNodeAnimation import Function.
		 *\param[out]	animation	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SceneNodeAnimation.
		 *\param[out]	animation	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool doImportNode( SceneNodeAnimation & animation ) = 0;

	protected:
		ImporterFile * m_file{};
		//!\~english Import configuration parameters.
		//!\~french Paramètres de configuration de l'import.
		Parameters m_parameters;
	};
}

#endif
