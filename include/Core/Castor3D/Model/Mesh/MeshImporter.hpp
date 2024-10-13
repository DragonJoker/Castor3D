/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshImporter_H___
#define ___C3D_MeshImporter_H___

#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Model/Mesh/MeshModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Data/Path.hpp>
#include <CastorUtils/Graphics/ImageCache.hpp>

namespace castor3d
{
	class MeshImporter
		: public castor::OwnedBy< Engine >
	{
	public:
		virtual ~MeshImporter() = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit MeshImporter( Engine & engine
			, castor::String const & prefix );
		/**
		 *\~english
		 *\brief		Mesh import Function.
		 *\param[out]	mesh		Receives the imported data.
		 *\param[in]	file		The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\param[in]	forceImport	Tells if the mesh needs to be imported even if it's already filled.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Mesh.
		 *\param[out]	mesh		Reçoit les données importées.
		 *\param[in]	file		Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\param[in]	forceImport	Dit si le mesh importé doit être importé même s'il est déjà rempli.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool import( Mesh & mesh
			, ImporterFile * file
			, Parameters const & parameters
			, bool forceImport );
		/**
		 *\~english
		 *\brief		Mesh import Function.
		 *\param[out]	mesh		Receives the imported data.
		 *\param[in]	pathFile	The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\param[in]	forceImport	Tells if the mesh needs to be imported even if it's already filled.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Mesh.
		 *\param[out]	mesh		Reçoit les données importées.
		 *\param[in]	pathFile	Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\param[in]	forceImport	Dit si le mesh importé doit être importé même s'il est déjà rempli.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool import( Mesh & mesh
			, castor::Path const & pathFile
			, Parameters const & parameters
			, bool forceImport );

		castor::StringView getPrefix()const noexcept
		{
			return m_prefix;
		}

	protected:
		/**
		 *\~english
		 *\brief		Mesh import Function.
		 *\param[out]	mesh	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Mesh.
		 *\param[out]	mesh	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool doImportMesh( Mesh & mesh ) = 0;

	protected:
		castor::String m_prefix;
		ImporterFile * m_file{};
		//!\~english The loaded meshes.
		//!\~french Les maillages chargés.
		MeshPtrStrMap m_meshes;
		//!\~english Import configuration parameters.
		//!\~french Paramètres de configuration de l'import.
		Parameters m_parameters;
	};
}

#endif
