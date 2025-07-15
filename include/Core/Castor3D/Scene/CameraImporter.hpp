/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CameraImporter_H___
#define ___C3D_CameraImporter_H___

#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Data/Path.hpp>

namespace c3d
{
	class CameraImporter
		: public OwnedBy< Engine >
	{
	public:
		virtual ~CameraImporter() = default;
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
		C3D_API explicit CameraImporter( Engine & engine
			, String const & prefix );
		/**
		 *\~english
		 *\brief		Camera import Function.
		 *\param[out]	name		The Camera name.
		 *\param[out]	createInfo	The camera creation informations.
		 *\param[in]	file		The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		The imported Camera, \p nullptr if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Camera.
		 *\param[out]	name		Le nom du Camera.
		 *\param[out]	createInfo	Les informations de création de la caméra.
		 *\param[in]	file		Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		La Camera importée, \p nullptr si un problème quelconque est survenu.
		 */
		C3D_API CameraRes importData( String const & name
			, CameraCreateInfo const & createInfo
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Camera import Function.
		 *\param[out]	camera			Receives the imported data.
		 *\param[in]	file			The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Camera.
		 *\param[out]	camera			Reçoit les données importées.
		 *\param[in]	file			Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importData( Camera & camera
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Camera import function.
		 *\param[out]	camera			Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Camera.
		 *\param[out]	camera			Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool importData( Camera & camera
			, Path const & pathFile
			, Parameters const & parameters );

		String getPrefix()const noexcept
		{
			return m_prefix;
		}

	private:
		C3D_API virtual CameraRes doCreateCamera( String const & name
			, CameraCreateInfo const & createInfo );
		virtual bool doImportCamera( Camera & camera ) = 0;

	protected:
		String m_prefix;
		ImporterFile * m_file{};
		Parameters m_parameters;
	};
}

#endif
