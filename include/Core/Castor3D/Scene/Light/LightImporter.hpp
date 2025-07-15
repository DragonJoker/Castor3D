/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightImporter_H___
#define ___C3D_LightImporter_H___

#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Data/Path.hpp>

namespace c3d
{
	class LightImporter
		: public OwnedBy< Engine >
	{
	public:
		virtual ~LightImporter() = default;
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
		C3D_API explicit LightImporter( Engine & engine
			, String const & prefix );
		/**
		 *\~english
		 *\brief		Light import function.
		 *\param[in]	name		The light name.
		 *\param[in]	createInfo	The light creation informations.
		 *\param[in]	file		The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Light.
		 *\param[in]	name		Le nom de la lumière.
		 *\param[in]	createInfo	Les informations de création de la lumière.
		 *\param[in]	file		Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API LightUPtr importData( String const & name
			, LightCreateInfo const & createInfo
			, ImporterFile * file
			, Parameters const & parameters );
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
		C3D_API bool importData( Light & light
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Light import function.
		 *\param[in]	name		The light name.
		 *\param[in]	createInfo	The light creation informations.
		 *\param[in]	file		The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Light.
		 *\param[in]	name		Le nom de la lumière.
		 *\param[in]	createInfo	Les informations de création de la lumière.
		 *\param[in]	file		Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API LightGroupUPtr importData( String const & name
			, LightGroupCreateInfo const & createInfo
			, ImporterFile * file
			, Parameters const & parameters );
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
		C3D_API bool importData( LightGroup & light
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Light import Function.
		 *\param[out]	light			Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Light.
		 *\param[out]	light			Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool importData( Light & light
			, Path const & pathFile
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Light import Function.
		 *\param[out]	light			Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Light.
		 *\param[out]	light			Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool importData( LightGroup & light
			, Path const & pathFile
			, Parameters const & parameters );

		String getPrefix()const noexcept
		{
			return m_prefix;
		}

	private:
		C3D_API virtual LightUPtr doCreateLight( String const & name
			, LightCreateInfo const & createInfo );
		C3D_API virtual LightGroupUPtr doCreateLightGroup( String const & name
			, LightGroupCreateInfo const & createInfo );
		virtual bool doImportLight( Light & light ) = 0;
		virtual bool doImportLightGroup( LightGroup & light ) = 0;

	protected:
		String m_prefix;
		ImporterFile * m_file{};
		Parameters m_parameters;
	};
}

#endif
