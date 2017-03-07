/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_TEXTURE_LAYOUT_H___
#define ___C3D_TEXTURE_LAYOUT_H___

#include "TextureImage.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Texture base class
	\~french
	\brief		Class de base d'une texture
	*/
	class TextureLayout
		: public Castor::OwnedBy< RenderSystem >
	{
		friend class TextureImage;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of AccessType).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type			Le type de texture.
		 *\param[in]	p_renderSystem	Le render system.
		 *\param[in]	p_cpuAccess		Les accès requis pour le CPU (combinaison de AccessType).
		 *\param[in]	p_gpuAccess		Les accès requis pour le GPU (combinaison de AccessType).
		 */
		C3D_API TextureLayout(
			RenderSystem & p_renderSystem,
			TextureType p_type,
			AccessTypes const & p_cpuAccess,
			AccessTypes const & p_gpuAccess );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of AccessType).
		 *\param[in]	p_format		The texture format.
		 *\param[in]	p_size			The texture dimensions.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type			Le type de texture.
		 *\param[in]	p_renderSystem	Le render system.
		 *\param[in]	p_cpuAccess		Les accès requis pour le CPU (combinaison de AccessType).
		 *\param[in]	p_gpuAccess		Les accès requis pour le GPU (combinaison de AccessType).
		 *\param[in]	p_format		Le format de la texture.
		 *\param[in]	p_size			Les dimensions de la texture.
		 */
		C3D_API TextureLayout(
			RenderSystem & p_renderSystem,
			TextureType p_type,
			AccessTypes const & p_cpuAccess,
			AccessTypes const & p_gpuAccess,
			Castor::PixelFormat p_format,
			Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of AccessType).
		 *\param[in]	p_format		The texture format.
		 *\param[in]	p_size			The texture dimensions.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type			Le type de texture.
		 *\param[in]	p_renderSystem	Le render system.
		 *\param[in]	p_cpuAccess		Les accès requis pour le CPU (combinaison de AccessType).
		 *\param[in]	p_gpuAccess		Les accès requis pour le GPU (combinaison de AccessType).
		 *\param[in]	p_format		Le format de la texture.
		 *\param[in]	p_size			Les dimensions de la texture.
		 */
		C3D_API TextureLayout(
			RenderSystem & p_renderSystem,
			TextureType p_type,
			AccessTypes const & p_cpuAccess,
			AccessTypes const & p_gpuAccess,
			Castor::PixelFormat p_format,
			Castor::Point3ui const & p_size );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~TextureLayout();
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_index	The texture index
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_index	L'index de texture
		 */
		C3D_API void Bind( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\param[in]	p_index	The texture index
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\param[in]	p_index	L'index de texture
		 */
		C3D_API void Unbind( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Resizes the texture buffer
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne le buffer de la texture
		 *\param[in]	p_size	La nouvelle taille
		 */
		C3D_API void Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Resizes the 3D texture buffer
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne le buffer de la texture 3D
		 *\param[in]	p_size	La nouvelle taille
		 */
		C3D_API void Resize( Castor::Point3ui const & p_size );
		/**
		 *\~english
		 *\brief		Locks layout buffer from GPU, allowing modifications into it.
		 *\remarks		Binds the texture.
		 *\param[in]	p_lock	Defines the lock mode (r, w, rw), combination of AccessType.
		 *\return		The image buffer.
		 *\~french
		 *\brief		Locke le buffer du layout à partir du GPU, permettant des modification dessus.
		 *\remarks		Active la texture.
		 *\param[in]	p_lock	Définit le mode de lock (lecture, écriture, les 2), combinaison de AccessType.
		 *\return		Le buffer de l'image.
		 */
		C3D_API uint8_t * Lock( AccessTypes const & p_lock );
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU.
		 *\remarks		Unbinds the texture.
		 *\param[in]	p_index		The texture index.
		 *\param[in]	p_modified	Tells if the buffer has been modified, so modifications are uploaded to GPU.
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU.
		 *\remarks		Désactive la texture.
		 *\param[in]	p_index		L'index de texture.
		 *\param[in]	p_modified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU.
		 */
		C3D_API void Unlock( bool p_modified );
		/**
		 *\~english
		 *\brief		Locks image buffer from GPU, allowing modifications into it.
		 *\remarks		The texture must be bound.
		 *\param[in]	p_lock	Defines the lock mode (r, w, rw), combination of AccessType.
		 *\param[in]	p_index	The image index.
		 *\return		The image buffer.
		 *\~french
		 *\brief		Locke le buffer de l'image à partir du GPU, permettant des modification dessus.
		 *\remarks		La texture doit être activée.
		 *\param[in]	p_lock	Définit le mode de lock (lecture, écriture, les 2), combinaison de AccessType.
		 *\param[in]	p_index	L'index de l'image.
		 *\return		Le buffer de l'image.
		 */
		C3D_API uint8_t * Lock( AccessTypes const & p_lock, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU.
		 *\remarks		The texture must be bound.
		 *\param[in]	p_modified	Tells if the buffer has been modified, so modifications are uploaded to GPU.
		 *\param[in]	p_index		The image index.
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU.
		 *\remarks		La texture doit être activée.
		 *\param[in]	p_modified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU.
		 *\param[in]	p_index		L'index de l'image.
		 */
		C3D_API void Unlock( bool p_modified, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Defines the texture buffer from an image file.
		 *\param[in]	p_folder	The folder containing the image.
		 *\param[in]	p_relative	The image file path, relative to p_folder.
		 *\~french
		 *\brief		Définit le tampon de la texture depuis un fichier image.
		 *\param[in]	p_folder	Le dossier contenant l'image.
		 *\param[in]	p_relative	Le chemin d'accès à l'image, relatif à p_folder.
		 */
		C3D_API void SetSource( Castor::Path const & p_folder, Castor::Path const & p_relative );
		/**
		 *\~english
		 *\brief		Initialises the texture buffer.
		 *\param[in]	p_buffer	The buffer.
		 *\~french
		 *\brief		Initialise le tampon de la texture.
		 *\param[in]	p_buffer	Le tampon.
		 */
		C3D_API void SetSource( Castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Initialise();
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Generate texture mipmaps
		 *\~french
		 *\brief		Génère les mipmaps de la texture
		 */
		C3D_API virtual void GenerateMipmaps()const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the initialisation status
		 *\return		The initialisation status
		 *\~french
		 *\brief		Récupère le statut d'initialisation
		 *\return		Le statut d'initialisation
		 */
		inline bool IsInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture dimension
		 *\return		The texture dimension
		 *\~french
		 *\brief		Récupère la dimension de la texture
		 *\return		La dimension de la texture
		 */
		inline TextureType GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\param[in]	p_index	The image index.
		 *\return		The texture image.
		 *\~french
		 *\param[in]	p_index	L'index de l'image.
		 *\return		L'image de la texture.
		 */
		inline TextureImage const & GetImage( size_t p_index = 0 )const
		{
			REQUIRE( p_index < m_images.size() && m_images[p_index] );
			return *m_images[p_index];
		}
		/**
		 *\~english
		 *\param[in]	p_index	The image index.
		 *\return		The texture image.
		 *\~french
		 *\param[in]	p_index	L'index de l'image.
		 *\return		L'image de la texture.
		 */
		inline TextureImage & GetImage( size_t p_index = 0 )
		{
			REQUIRE( p_index < m_images.size() && m_images[p_index] );
			return *m_images[p_index];
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture image width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur de l'image de la texture
		 *\return		La largeur
		 */
		inline uint32_t GetWidth()const
		{
			return m_size.width();
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture image height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur de l'image de la texture
		 *\return		La hauteur
		 */
		inline uint32_t GetHeight()const
		{
			return m_size.height() / GetDepth();
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture image dimensions
		 *\return		The image dimensions
		 *\~french
		 *\brief		Récupère les dimensions de l'image de la texture
		 *\return		Les dimensions de l'image
		 */
		inline Castor::Size GetDimensions()const
		{
			return Castor::Size{ GetWidth(), GetHeight() };
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format des pixels de la texture
		 *\return		Le format des pixels
		 */
		inline Castor::PixelFormat GetPixelFormat()const
		{
			return m_format;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture depth
		 *\return		The depth, 1 if not 3D
		 *\~french
		 *\brief		Récupère la profondeur de la texture
		 *\return		La profondeur, 1 si pas 3D
		 */
		inline uint32_t GetDepth()const
		{
			return m_type == TextureType::eThreeDimensions ? m_depth : 1;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture depth
		 *\return		The depth, 1 if not array
		 *\~french
		 *\brief		Récupère la profondeur de la texture
		 *\return		La profondeur, 1 si pas tableau
		 */
		inline uint32_t GetLayersCount()const
		{
			return ( m_type == TextureType::eTwoDimensionsArray
					|| m_type == TextureType::eOneDimensionArray
					|| m_type == TextureType::eCubeArray )
				? m_depth
				: 1;
		}
		/**
		 *\~english
		 *\return		An iterator to the images beginning.
		 *\~french
		 *\return		Un itérateur sur le début des images.
		 */
		inline auto begin()
		{
			return m_images.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the images beginning.
		 *\~french
		 *\return		Un itérateur sur le début des images.
		 */
		inline auto begin()const
		{
			return m_images.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the images end.
		 *\~french
		 *\return		Un itérateur sur la fin des images.
		 */
		inline auto end()
		{
			return m_images.end();
		}
		/**
		 *\~english
		 *\return		An iterator to the images end.
		 *\~french
		 *\return		Un itérateur sur la fin des images.
		 */
		inline auto end()const
		{
			return m_images.end();
		}

	private:
		/**
		 *\~english
		 *\brief		Updates the size and format from first loaded image.
		 *\param[in]	p_size		The image size.
		 *\param[in]	p_format	The image format.
		 *\~french
		 *\brief		Met à jour les dimensions et le format, depuis la première image chargée.
		 *\param[in]	p_size		Les dimensions de l'image.
		 *\param[in]	p_format	Le format des pixels de l'image.
		 */
		void DoUpdateFromFirstImage( Castor::Size const & p_size, Castor::PixelFormat p_format );
		/**
		 *\~english
		 *\brief		Reinitialises the storage.
		 *\remarks		If the storage was not created, does nothing.
		 *\~french
		 *\brief		Réinitialise le stockage.
		 *\remarks		Ne fait rien si le stockage n'avait pas été créé.
		 */
		bool DoResetStorage();
		/**
		 *\~english
		 *\brief		Initialises the GPU storage.
		 *\param[in]	p_type		The storage type.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise le stockage GPU.
		 *\param[in]	p_type		Le type de stockage.
		 *\return		\p true si tout s'est bien passé.
		 */
		bool DoCreateStorage( TextureStorageType p_type );

	private:
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		API specific binding function
		 *\~french
		 *\brief		Activation spécifique selon l'API
		 */
		C3D_API virtual void DoBind( uint32_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		API specific unbinding function
		 *\~french
		 *\brief		Désactivation spécifique selon l'API
		 */
		C3D_API virtual void DoUnbind( uint32_t p_index )const = 0;

	protected:
		//!\~english	Initialisation status
		//!\~french		Statut d'initialisation
		bool m_initialised{ false };
		//!\~english	Texture type.
		//!\~french		Type de texture.
		TextureType m_type;
		//!\~english	The texture GPU storage.
		//!\~french		Le stockage GPU de la texture.
		TextureStorageUPtr m_storage;
		//!\~english	The texture images.
		//!\~french		Les images de la texture.
		std::vector< TextureImageUPtr > m_images;
		//!\~english	The required CPU access (combination of AccessType).
		//!\~french		Les accès requis pour le CPU (combinaison de AccessType).
		AccessTypes m_cpuAccess;
		//!\~english	The required GPU access (combination of AccessType).
		//!\~french		Les accès requis pour le GPU (combinaison de AccessType).
		AccessTypes m_gpuAccess;
		//!\~english	The teture dimensions.
		//!\~french		Les dimensions de la texture.
		Castor::Size m_size;
		//!\~english	The texture pixels' format.
		//!\~french		Le format des pixels de la texture.
		Castor::PixelFormat m_format;
		//!\~english	The texture's depth or layers count.
		//!\~french		La profondeur ou nombre de couches de la texture.
		uint32_t m_depth;
	};
}

#endif
