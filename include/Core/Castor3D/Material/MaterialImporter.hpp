/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialImporter_H___
#define ___C3D_MaterialImporter_H___

#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Data/Path.hpp>
#include <CastorUtils/Graphics/ImageCache.hpp>

namespace castor3d
{
	class MaterialImporter
		: public castor::OwnedBy< Engine >
	{
	public:
		virtual ~MaterialImporter() = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit MaterialImporter( Engine & engine );
		/**
		 *\~english
		 *\brief		Scene import Function.
		 *\param[out]	material		Receives the imported data.
		 *\param[in]	file			The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\param[in]	textureRemaps	The imported textures remapping parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Scene.
		 *\param[out]	material		Reçoit les données importées.
		 *\param[in]	file			Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\param[in]	textureRemaps	Les paramètres de reaffectation des textures importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool import( Material & material
			, ImporterFile * file
			, Parameters const & parameters
			, std::map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps );
		/**
		 *\~english
		 *\brief		Scene import Function.
		 *\param[out]	material		Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\param[in]	textureRemaps	The imported textures remapping parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Scene.
		 *\param[out]	material		Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\param[in]	textureRemaps	Les paramètres de reaffectation des textures importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool import( Material & material
			, castor::Path const & pathFile
			, Parameters const & parameters
			, std::map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps );
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
		 *\brief			Loads a texture and adds it to the given pass.
		 *\param[in]		sampler	The sampler used by the texture.
		 *\param[in]		path	The image file path (can be relative or absolute).
		 *\param[in]		config	The texture unit configuration.
		 *\param[in,out]	pass	Receives the loaded image.
		 *\~french
		 *\brief			Charge une texture et l'ajoute à la passe donnée.
		 *\param[in]		sampler	Le sampler utilisé par la texture.
		 *\param[in]		path	Le chemin vers l'image (peut être relatif ou absolu).
		 *\param[in]		config	La configuration de la texture.
		 *\param[in,out]	pass	Reçoit l'image chargée.
		 */
		C3D_API void loadTexture( castor3d::SamplerRes sampler
			, castor::Path const & path
			, PassTextureConfig const & config
			, Pass & pass )const;
		/**
		 *\~english
		 *\brief			Loads a texture and adds it to the given pass.
		 *\param[in]		sampler	The sampler used by the texture.
		 *\param[in]		name	The image name.
		 *\param[in]		type	The image data type.
		 *\param[in]		data	The image data.
		 *\param[in]		config	The texture unit configuration.
		 *\param[in,out]	pass	Receives the loaded image.
		 *\~french
		 *\brief			Charge une texture et l'ajoute à la passe donnée.
		 *\param[in]		sampler	Le sampler utilisé par la texture.
		 *\param[in]		name	Le nom de l'image.
		 *\param[in]		type	Le type des données de l'image.
		 *\param[in]		data	Les données de l'image.
		 *\param[in]		config	La configuration de la texture.
		 *\param[in,out]	pass	Reçoit l'image chargée.
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

	private:
		virtual bool doImportMaterial( Material & material ) = 0;

	protected:
		ImporterFile * m_file{};
		Parameters m_parameters;
		std::map< PassComponentTextureFlag, TextureConfiguration > m_textureRemaps;
	};
}

#endif
