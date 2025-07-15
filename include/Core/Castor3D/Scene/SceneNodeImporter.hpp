/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneNodeImporter_H___
#define ___C3D_SceneNodeImporter_H___

#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Data/Path.hpp>

namespace c3d
{
	class SceneNodeImporter
		: public OwnedBy< Engine >
	{
	public:
		virtual ~SceneNodeImporter() = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	prefix	The prefix used for logging.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	prefix	Le préfixe utilisé pour le logging.
		 */
		C3D_API explicit SceneNodeImporter( Engine & engine
			, String const & prefix );
		/**
		 *\~english
		 *\brief		SceneNode import Function.
		 *\param[out]	name		The SceneNode name.
		 *\param[out]	createInfo	The scene node creation informations.
		 *\param[in]	file		The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		The imported SceneNode, \p nullptr if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SceneNode.
		 *\param[out]	name		Le nom du SceneNode.
		 *\param[out]	createInfo	Les informations de création du noeud de scène.
		 *\param[in]	file		Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		Le SceneNode importé, \p nullptr si un problème quelconque est survenu.
		 */
		C3D_API SceneNodeUPtr importData( String const & name
			, SceneNodeCreateInfo const & createInfo
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		SceneNode import Function.
		 *\param[out]	node			Receives the imported data.
		 *\param[in]	file			The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SceneNode.
		 *\param[out]	node			Reçoit les données importées.
		 *\param[in]	file			Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importData( SceneNode & node
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		SceneNode import function.
		 *\param[out]	node			Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SceneNode.
		 *\param[out]	node			Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool importData( SceneNode & node
			, Path const & pathFile
			, Parameters const & parameters );

		String getPrefix()const noexcept
		{
			return m_prefix;
		}

	private:
		C3D_API virtual SceneNodeUPtr doCreateSceneNode( String const & name
			, SceneNodeCreateInfo const & createInfo );
		virtual bool doImportSceneNode( SceneNode & node ) = 0;

	protected:
		String m_prefix;
		ImporterFile * m_file{};
		Parameters m_parameters;
	};
}

#endif
