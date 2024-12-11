/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonImporter_H___
#define ___C3D_SkeletonImporter_H___

#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Data/Path.hpp>

namespace castor3d
{
	class Scene;

	class SkeletonImporter
		: public castor::OwnedBy< Engine >
	{
	public:
		virtual ~SkeletonImporter() = default;
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
		C3D_API explicit SkeletonImporter( Engine & engine
			, castor::String const & prefix );
		/**
		 *\~english
		 *\brief		Skeleton import Function.
		 *\param[out]	name		The Skeleton name.
		 *\param[out]	scene		The parent scene.
		 *\param[in]	file		The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		The imported Skeleton, \p nullptr if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Skeleton.
		 *\param[out]	name		Le nom du Skeleton.
		 *\param[out]	scene		La scène parente.
		 *\param[in]	file		Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		Le Skeleton importé, \p nullptr si un problème quelconque est survenu.
		 */
		C3D_API SkeletonRes importData( castor::String const & name
			, Scene & scene
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Skeleton import Function.
		 *\param[out]	skeleton	Receives the imported data.
		 *\param[in]	file		The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Skeleton.
		 *\param[out]	skeleton	Reçoit les données importées.
		 *\param[in]	file		Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importData( Skeleton & skeleton
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Skeleton import Function.
		 *\param[out]	skeleton	Receives the imported data.
		 *\param[in]	pathFile	The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Skeleton.
		 *\param[out]	skeleton	Reçoit les données importées.
		 *\param[in]	pathFile	Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool importData( Skeleton & skeleton
			, castor::Path const & pathFile
			, Parameters const & parameters );

		castor::StringView getPrefix()const noexcept
		{
			return m_prefix;
		}

	private:
		C3D_API virtual SkeletonRes doCreateSkeleton( castor::String const & name
			, Scene & scene );
		virtual bool doImportSkeleton( Skeleton & skeleton ) = 0;

	protected:
		castor::String m_prefix;
		ImporterFile * m_file{};
		//!\~english Import configuration parameters.
		//!\~french Paramètres de configuration de l'import.
		Parameters m_parameters;
	};
}

#endif
