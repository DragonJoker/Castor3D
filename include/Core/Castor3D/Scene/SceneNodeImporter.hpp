/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneNodeImporter_H___
#define ___C3D_SceneNodeImporter_H___

#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Data/Path.hpp>

namespace castor3d
{
	class SceneNodeImporter
		: public castor::OwnedBy< Engine >
	{
	public:
		virtual ~SceneNodeImporter() = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit SceneNodeImporter( Engine & engine
			, castor::String const & prefix );
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
		C3D_API bool import( SceneNode & node
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
		C3D_API static bool import( SceneNode & node
			, castor::Path const & pathFile
			, Parameters const & parameters );

		castor::StringView getPrefix()const noexcept
		{
			return m_prefix;
		}

	private:
		/**
		 *\~english
		 *\brief		SceneNode import function.
		 *\param[out]	node	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SceneNode.
		 *\param[out]	node	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		virtual bool doImportSceneNode( SceneNode & node ) = 0;

	protected:
		castor::String m_prefix;
		ImporterFile * m_file{};
		Parameters m_parameters;
	};
}

#endif
