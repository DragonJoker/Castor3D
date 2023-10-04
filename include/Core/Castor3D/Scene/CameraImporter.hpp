/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CameraImporter_H___
#define ___C3D_CameraImporter_H___

#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Data/Path.hpp>

namespace castor3d
{
	class CameraImporter
		: public castor::OwnedBy< Engine >
	{
	public:
		virtual ~CameraImporter() = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit CameraImporter( Engine & engine );
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
		C3D_API bool import( Camera & camera
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
		C3D_API static bool import( Camera & camera
			, castor::Path const & pathFile
			, Parameters const & parameters );

	private:
		/**
		 *\~english
		 *\brief		Camera import function.
		 *\param[out]	camera	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Camera.
		 *\param[out]	camera	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		virtual bool doImportCamera( Camera & camera ) = 0;

	protected:
		ImporterFile * m_file{};
		Parameters m_parameters;
	};
}

#endif
