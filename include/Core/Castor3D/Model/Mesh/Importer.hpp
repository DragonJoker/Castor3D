/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshImporter_H___
#define ___C3D_ModelMeshImporter_H___

#include "MeshModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
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
		C3D_API explicit MeshImporter( Engine & engine );
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
		C3D_API bool import( Skeleton & skeleton
			, castor::Path const & pathFile
			, Parameters const & parameters );
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
		C3D_API bool import( Mesh & mesh
			, castor::Path const & pathFile
			, Parameters const & parameters
			, bool forceImport );
		/**
		 *\~english
		 *\brief		Scene import Function.
		 *\param[out]	scene			Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\param[in]	textureRemaps	The imported textures remapping parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Scene.
		 *\param[out]	scene			Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\param[in]	textureRemaps	Les paramètres de reaffectation des textures importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool import( Scene & scene
			, castor::Path const & pathFile
			, Parameters const & parameters
			, std::map< TextureFlag, TextureConfiguration > const & textureRemaps );
		/**
		 *\~english
		 *\brief		Skeleton animations import Function.
		 *\param[out]	skeleton	Receives the imported data.
		 *\param[in]	pathFile	The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import des animations de Skeleton.
		 *\param[out]	skeleton	Reçoit les données importées.
		 *\param[in]	pathFile	Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importAnimations( Skeleton & skeleton
			, castor::Path const & pathFile
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Mesh animations import Function.
		 *\param[out]	mesh		Receives the imported data.
		 *\param[in]	pathFile	The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import des animations de Mesh.
		 *\param[out]	mesh		Reçoit les données importées.
		 *\param[in]	pathFile	Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importAnimations( Mesh & mesh
			, castor::Path const & pathFile
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Scene animations import Function.
		 *\param[out]	scene			Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import des animations de Scene.
		 *\param[out]	scene			Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importAnimations( Scene & scene
			, castor::Path const & pathFile
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Loads an image.
		 *\param[in]	name	The image name.
		 *\param[in]	params	The image creation parameters.
		 *\return		The image.
		 *\~french
		 *\brief		Charge une image.
		 *\param[in]	name	Le nom de l'image.
		 *\param[in]	params	Les paramètres de création de l'image.
		 *\return		L'image.
		 */
		C3D_API castor::ImageSPtr loadImage( castor::String const & name
			, castor::ImageCreateParams const & params )const;
		/**
		 *\~english
		 *\brief		Loads an image from a file.
		 *\param[in]	path	The image file path (can be relative or absolute).
		 *\return		The image.
		 *\~french
		 *\brief		Charge une image depuis un fichier.
		 *\param[in]	path	Le chemin vers l'image (peut être relatif ou absolu).
		 *\return		L'image.
		 */
		C3D_API castor::ImageSPtr loadImage( castor::Path const & path )const;
		/**
		 *\~english
		 *\brief		Loads a texture.
		 *\param[in]	name	The image name.
		 *\param[in]	type	The image data type.
		 *\param[in]	data	The image data.
		 *\return		The texture unit.
		 *\~french
		 *\brief		Charge une texture.
		 *\param[in]	name	Le nom de l'image.
		 *\param[in]	type	Le type des données de l'image.
		 *\param[in]	data	Les données de l'image.
		 *\return		L'unité de texture.
		 */
		C3D_API castor::ImageSPtr loadImage( castor::String name
			, castor::String type
			, castor::ByteArray data )const;
		/**
		 *\~english
		 *\brief		Loads a texture.
		 *\param[in]	sampler	The sampler used by the texture.
		 *\param[in]	path	The image file path (can be relative or absolute).
		 *\param[in]	config	The texture unit configuration.
		 *\return		The texture unit.
		 *\~french
		 *\brief		Charge une texture.
		 *\param[in]	sampler	Le sampler utilisé par la texture.
		 *\param[in]	path	Le chemin vers l'image (peut être relatif ou absolu).
		 *\param[in]	config	La configuration de la texture.
		 *\return		L'unité de texture.
		 */
		C3D_API TextureSourceInfo loadTexture( castor3d::SamplerRes sampler
			, castor::Path const & path
			, TextureConfiguration const & config )const;
		/**
		 *\~english
		 *\brief		Loads a texture.
		 *\param[in]	sampler	The sampler used by the texture.
		 *\param[in]	name	The image name.
		 *\param[in]	type	The image data type.
		 *\param[in]	data	The image data.
		 *\param[in]	config	The texture unit configuration.
		 *\return		The texture unit.
		 *\~french
		 *\brief		Charge une texture.
		 *\param[in]	sampler	Le sampler utilisé par la texture.
		 *\param[in]	name	Le nom de l'image.
		 *\param[in]	type	Le type des données de l'image.
		 *\param[in]	data	Les données de l'image.
		 *\param[in]	config	La configuration de la texture.
		 *\return		L'unité de texture.
		 */
		C3D_API TextureSourceInfo loadTexture( castor3d::SamplerRes sampler
			, castor::String name
			, castor::String type
			, castor::ByteArray data
			, TextureConfiguration const & config )const;
		/**
		 *\~english
		 *\brief		Loads a texture and adds it to the given pass.
		 *\param[in]	sampler	The sampler used by the texture.
		 *\param[in]	path	The image file path (can be relative or absolute).
		 *\param[in]	config	The texture unit configuration.
		 *\param[in]	pass	The pass.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Charge une texture et l'ajoute à la passe donnée.
		 *\param[in]	sampler	Le sampler utilisé par la texture.
		 *\param[in]	path	Le chemin vers l'image (peut être relatif ou absolu).
		 *\param[in]	config	La configuration de la texture.
		 *\param[in]	pass	La passe.
		 *\return		\p false en cas d'erreur.
		 */
		C3D_API void loadTexture( castor3d::SamplerRes sampler
			, castor::Path const & path
			, PassTextureConfig const & config
			, Pass & pass )const;
		/**
		 *\~english
		 *\brief		Loads a texture and adds it to the given pass.
		 *\param[in]	sampler	The sampler used by the texture.
		 *\param[in]	name	The image name.
		 *\param[in]	type	The image data type.
		 *\param[in]	data	The image data.
		 *\param[in]	config	The texture unit configuration.
		 *\param[in]	pass	The pass.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Charge une texture et l'ajoute à la passe donnée.
		 *\param[in]	sampler	Le sampler utilisé par la texture.
		 *\param[in]	name	Le nom de l'image.
		 *\param[in]	type	Le type des données de l'image.
		 *\param[in]	data	Les données de l'image.
		 *\param[in]	config	La configuration de la texture.
		 *\param[in]	pass	La passe.
		 *\return		\p false en cas d'erreur.
		 */
		C3D_API void loadTexture( castor3d::SamplerRes sampler
			, castor::String name
			, castor::String type
			, castor::ByteArray data
			, PassTextureConfig const & config
			, Pass & pass )const;
		/**
		 *\~english
		 *\brief			Converts given height image file to a normal+height image.
		 *\param[in,out]	path	The image file path (can be relative or absolute), receives the resulting normal+height image file path.
		 *\param[in,out]	config	The texture unit configuration.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Convertit le fichier d'image de hauteur en une image normale+hauteur.
		 *\param[in,out]	path	Le chemin vers l'image (peut être relatif ou absolu), reçoit le chemin de l'image normale+hauteur.
		 *\param[in,out]	config	La configuration de la texture.
		 *\return			\p false en cas d'erreur.
		 */
		C3D_API bool convertToNormalMap( castor::Path & path
			, castor3d::TextureConfiguration & config )const;

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
		/**
		 *\~english
		 *\brief		Scene import Function.
		 *\param[out]	scene	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Scene.
		 *\param[out]	scene	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool doImportScene( Scene & scene ) = 0;

	protected:
		//!\~english The file name.
		//!\~french Le nom du fichier.
		castor::Path m_fileName;
		//!\~english The file full path.
		//!\~french Le chemin complet du fichier.
		castor::Path m_filePath;
		//!\~english The loaded scene nodes.
		//!\~french Les noeuds chargés.
		SceneNodePtrArray m_nodes;
		//!\~english The loaded geometries.
		//!\~french Les géométries chargées.
		GeometryPtrStrMap m_geometries;
		//!\~english The loaded meshes.
		//!\~french Les maillages chargés.
		MeshPtrStrMap m_meshes;
		//!\~english Import configuration parameters.
		//!\~french Paramètres de configuration de l'import.
		Parameters m_parameters;
		//!\~english Import texture remapping parameters.
		//!\~french Paramètres de reaffectation des textures importées.
		std::map< TextureFlag, TextureConfiguration > m_textureRemaps;
		//!\~english Tells if only the animations need to be imported.
		//!\~french Dit si seules les animations doivent être importées+.
		bool m_animsOnly{ false };
	};
}

#endif
