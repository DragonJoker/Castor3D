/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshImporter_H___
#define ___C3D_ModelMeshImporter_H___

#include "MeshModule.hpp"

#include <CastorUtils/Data/Path.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	class MeshImporter
		: public castor::OwnedBy< Engine >
	{
	public:
		virtual ~MeshImporter() = default;
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine		The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine		Le moteur
		 */
		C3D_API explicit MeshImporter( Engine & engine );
		/**
		 *\~english
		 *\brief		Mesh import Function.
		 *\param[out]	mesh		Receives the imported data.
		 *\param[in]	pathFile	The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\param[in]	initialise	Tells if the imported mesh must be initialised.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Mesh.
		 *\param[out]	mesh		Reçoit les données importées.
		 *\param[in]	pathFile	Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\param[in]	initialise	Dit si le mesh importé doit être initialisé.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool import( Mesh & mesh
			, castor::Path const & pathFile
			, Parameters const & parameters
			, bool initialise );
		/**
		 *\~english
		 *\brief		Loads a texture and adds it to the given pass.
		 *\param[in]	path	The image file path (can be relative or absolute).
		 *\param[in]	config	The texture unit configuration.
		 *\return		The texture unit.
		 *\~french
		 *\brief		Charge une texture et l'ajoute à la passe donnée.
		 *\param[in]	path	Le chemin vers l'image (peut être relatif ou absolu).
		 *\param[in]	config	La configuration de la texture.
		 *\return		L'unité de texture.
		 */
		C3D_API TextureUnitSPtr loadTexture( castor::Path const & path
			, TextureConfiguration const & config )const;
		/**
		 *\~english
		 *\brief		Loads a texture and adds it to the given pass.
		 *\param[in]	path	The image file path (can be relative or absolute).
		 *\param[in]	config	The texture unit configuration.
		 *\param[in]	pass	The pass.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Charge une texture et l'ajoute à la passe donnée.
		 *\param[in]	path	Le chemin vers l'image (peut être relatif ou absolu).
		 *\param[in]	config	La configuration de la texture.
		 *\param[in]	pass	La passe.
		 *\return		\p false en cas d'erreur.
		 */
		C3D_API TextureUnitSPtr loadTexture( castor::Path const & path
			, TextureConfiguration const & config
			, Pass & pass )const;

	protected:
		/**
		 *\~english
		 *\brief		Mesh import Function.
		 *\param[out]	mesh	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de maillage.
		 *\param[out]	mesh	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool doImportMesh( Mesh & mesh ) = 0;

	protected:
		//!\~english The file name	\~french Le nom du fichier
		castor::Path m_fileName;
		//!\~english The file full path	\~french Le chemin complet du fichier
		castor::Path m_filePath;
		//!\~english The loaded scene nodes	\~french Les noeuds chargés
		SceneNodePtrArray m_nodes;
		//!\~english The loaded geometries	\~french Les géométries chargées
		GeometryPtrStrMap m_geometries;
		//!\~english Import configuration parameters	\~french Paramètres de configuration de l'import
		Parameters m_parameters;
	};
}

#endif
