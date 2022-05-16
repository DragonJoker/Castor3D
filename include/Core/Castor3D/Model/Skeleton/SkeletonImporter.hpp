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
	class SkeletonImporter
		: public castor::OwnedBy< Engine >
	{
	public:
		virtual ~SkeletonImporter() = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit SkeletonImporter( Engine & engine );
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
		C3D_API bool import( Skeleton & skeleton
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
		C3D_API static bool import( Skeleton & skeleton
			, castor::Path const & pathFile
			, Parameters const & parameters );

	protected:
		/**
		 *\~english
		 *\brief		Skeleton import Function.
		 *\param[out]	skeleton	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Skeleton.
		 *\param[out]	skeleton	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool doImportSkeleton( Skeleton & skeleton ) = 0;

	protected:
		ImporterFile * m_file{};
		//!\~english Import configuration parameters.
		//!\~french Paramètres de configuration de l'import.
		Parameters m_parameters;
	};
}

#endif
