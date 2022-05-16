/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightImporter_H___
#define ___C3D_LightImporter_H___

#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Data/Path.hpp>

namespace castor3d
{
	class LightImporter
		: public castor::OwnedBy< Engine >
	{
	public:
		virtual ~LightImporter() = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit LightImporter( Engine & engine );
		/**
		 *\~english
		 *\brief		Light import function.
		 *\param[out]	light			Receives the imported data.
		 *\param[in]	file			The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Light.
		 *\param[out]	light			Reçoit les données importées.
		 *\param[in]	file			Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool import( Light & light
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Light import Function.
		 *\param[out]	light			Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\param[in]	textureRemaps	The imported textures remapping parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Light.
		 *\param[out]	light			Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\param[in]	textureRemaps	Les paramètres de reaffectation des textures importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool import( Light & light
			, castor::Path const & pathFile
			, Parameters const & parameters );

	private:
		virtual bool doImportLight( Light & light ) = 0;

	protected:
		ImporterFile * m_file{};
		Parameters m_parameters;
	};
}

#endif
