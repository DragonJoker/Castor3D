/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TEXTURE_LAYOUT_H___
#define ___C3D_TEXTURE_LAYOUT_H___

#include "TextureView.hpp"

namespace castor3d
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
		: public castor::OwnedBy< RenderSystem >
	{
		friend class TextureView;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type			The texture type.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	gpuAccess		The required GPU access (combination of AccessType).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type			Le type de texture.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	cpuAccess		Les accès requis pour le CPU (combinaison de AccessType).
		 *\param[in]	gpuAccess		Les accès requis pour le GPU (combinaison de AccessType).
		 */
		C3D_API TextureLayout( RenderSystem & renderSystem
			, TextureType type
			, AccessTypes const & cpuAccess
			, AccessTypes const & gpuAccess );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type			The texture type.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	gpuAccess		The required GPU access (combination of AccessType).
		 *\param[in]	mipmapCount		The mipmap count for the texture.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type			Le type de texture.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	cpuAccess		Les accès requis pour le CPU (combinaison de AccessType).
		 *\param[in]	gpuAccess		Les accès requis pour le GPU (combinaison de AccessType).
		 *\param[in]	mipmapCount		Le nombre de mipmaps de la texture.
		 */
		C3D_API TextureLayout( RenderSystem & renderSystem
			, TextureType type
			, AccessTypes const & cpuAccess
			, AccessTypes const & gpuAccess
			, uint32_t mipmapCount );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type			The texture type.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	gpuAccess		The required GPU access (combination of AccessType).
		 *\param[in]	format			The texture format.
		 *\param[in]	size			The texture dimensions.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type			Le type de texture.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	cpuAccess		Les accès requis pour le CPU (combinaison de AccessType).
		 *\param[in]	gpuAccess		Les accès requis pour le GPU (combinaison de AccessType).
		 *\param[in]	format			Le format de la texture.
		 *\param[in]	size			Les dimensions de la texture.
		 */
		C3D_API TextureLayout( RenderSystem & renderSystem
			, TextureType type
			, AccessTypes const & cpuAccess
			, AccessTypes const & gpuAccess
			, castor::PixelFormat format
			, castor::Size const & size );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type			The texture type.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	gpuAccess		The required GPU access (combination of AccessType).
		 *\param[in]	format			The texture format.
		 *\param[in]	size			The texture dimensions.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type			Le type de texture.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	cpuAccess		Les accès requis pour le CPU (combinaison de AccessType).
		 *\param[in]	gpuAccess		Les accès requis pour le GPU (combinaison de AccessType).
		 *\param[in]	format			Le format de la texture.
		 *\param[in]	size			Les dimensions de la texture.
		 */
		C3D_API TextureLayout( RenderSystem & renderSystem
			, TextureType type
			, AccessTypes const & cpuAccess
			, AccessTypes const & gpuAccess
			, castor::PixelFormat format
			, castor::Point3ui const & size );
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
		 *\param[in]	index	The texture index
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	index	L'index de texture
		 */
		C3D_API void bind( uint32_t index )const;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\param[in]	index	The texture index
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\param[in]	index	L'index de texture
		 */
		C3D_API void unbind( uint32_t index )const;
		/**
		 *\~english
		 *\brief		Resizes the texture buffer
		 *\param[in]	size	The new size
		 *\~french
		 *\brief		Redimensionne le buffer de la texture
		 *\param[in]	size	La nouvelle taille
		 */
		C3D_API void resize( castor::Size const & size );
		/**
		 *\~english
		 *\brief		Resizes the 3D texture buffer
		 *\param[in]	size	The new size
		 *\~french
		 *\brief		Redimensionne le buffer de la texture 3D
		 *\param[in]	size	La nouvelle taille
		 */
		C3D_API void resize( castor::Point3ui const & size );
		/**
		 *\~english
		 *\brief		Locks layout buffer from GPU, allowing modifications into it.
		 *\remarks		Binds the texture.
		 *\param[in]	lock	Defines the lock mode (r, w, rw), combination of AccessType.
		 *\return		The image buffer.
		 *\~french
		 *\brief		Locke le buffer du layout à partir du GPU, permettant des modification dessus.
		 *\remarks		Active la texture.
		 *\param[in]	lock	Définit le mode de lock (lecture, écriture, les 2), combinaison de AccessType.
		 *\return		Le buffer de l'image.
		 */
		C3D_API uint8_t * lock( AccessTypes const & lock );
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU.
		 *\remarks		Unbinds the texture.
		 *\param[in]	modified	Tells if the buffer has been modified, so modifications are uploaded to GPU.
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU.
		 *\remarks		Désactive la texture.
		 *\param[in]	modified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU.
		 */
		C3D_API void unlock( bool modified );
		/**
		 *\~english
		 *\brief		Locks image buffer from GPU, allowing modifications into it.
		 *\remarks		The texture must be bound.
		 *\param[in]	lock	Defines the lock mode (r, w, rw), combination of AccessType.
		 *\param[in]	index	The image index.
		 *\return		The image buffer.
		 *\~french
		 *\brief		Locke le buffer de l'image à partir du GPU, permettant des modification dessus.
		 *\remarks		La texture doit être activée.
		 *\param[in]	lock	Définit le mode de lock (lecture, écriture, les 2), combinaison de AccessType.
		 *\param[in]	index	L'index de l'image.
		 *\return		Le buffer de l'image.
		 */
		C3D_API uint8_t * lock( AccessTypes const & lock, uint32_t index );
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU.
		 *\remarks		The texture must be bound.
		 *\param[in]	modified	Tells if the buffer has been modified, so modifications are uploaded to GPU.
		 *\param[in]	index		The image index.
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU.
		 *\remarks		La texture doit être activée.
		 *\param[in]	modified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU.
		 *\param[in]	index		L'index de l'image.
		 */
		C3D_API void unlock( bool modified, uint32_t index );
		/**
		 *\~english
		 *\brief		Defines the texture buffer from an image file.
		 *\param[in]	folder	The folder containing the image.
		 *\param[in]	relative	The image file path, relative to folder.
		 *\~french
		 *\brief		Définit le tampon de la texture depuis un fichier image.
		 *\param[in]	folder	Le dossier contenant l'image.
		 *\param[in]	relative	Le chemin d'accès à l'image, relatif à folder.
		 */
		C3D_API void setSource( castor::Path const & folder
			, castor::Path const & relative );
		/**
		 *\~english
		 *\brief		Initialises the texture buffer.
		 *\param[in]	buffer	The buffer.
		 *\~french
		 *\brief		Initialise le tampon de la texture.
		 *\param[in]	buffer	Le tampon.
		 */
		C3D_API void setSource( castor::PxBufferBaseSPtr buffer );
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Generate texture mipmaps
		 *\~french
		 *\brief		Génère les mipmaps de la texture
		 */
		C3D_API virtual void generateMipmaps()const = 0;
		/**
		 *\~english
		 *\return		The initialisation status.
		 *\~french
		 *\return		Le statut d'initialisation.
		 */
		inline bool isInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\return		The texture type.
		 *\~french
		 *\return		Le type de texture.
		 */
		inline TextureType getType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\param[in]	index	The image index.
		 *\return		The texture image.
		 *\~french
		 *\param[in]	index	L'index de l'image.
		 *\return		L'image de la texture.
		 */
		inline TextureView const & getImage( size_t index = 0 )const
		{
			REQUIRE( index < m_images.size() && m_images[index] );
			return *m_images[index];
		}
		/**
		 *\~english
		 *\param[in]	index	The image index.
		 *\return		The texture image.
		 *\~french
		 *\param[in]	index	L'index de l'image.
		 *\return		L'image de la texture.
		 */
		inline TextureView & getImage( size_t index = 0 )
		{
			REQUIRE( index < m_images.size() && m_images[index] );
			return *m_images[index];
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture image width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur de l'image de la texture
		 *\return		La largeur
		 */
		inline uint32_t getWidth()const
		{
			return m_size.getWidth();
		}
		/**
		 *\~english
		 *\return		The texture image height.
		 *\~french
		 *\return		La hauteur de l'image de la texture.
		 */
		inline uint32_t getHeight()const
		{
			return m_size.getHeight();
		}
		/**
		 *\~english
		 *\return		The mimap count.
		 *\~french
		 *\return		Le nombre de mipmaps.
		 */
		inline uint32_t getMipmapCount()const
		{
			return m_mipmapCount;
		}
		/**
		 *\~english
		 *\return		The image image dimensions.
		 *\~french
		 *\return		Les dimensions de l'image de la texture.
		 */
		inline castor::Size const & getDimensions()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The texture pixel format.
		 *\~french
		 *\return		Le format des pixels de la texture.
		 */
		inline castor::PixelFormat getPixelFormat()const
		{
			return m_format;
		}
		/**
		 *\~english
		 *\return		The texture depth, 1 if not 3D.
		 *\~french
		 *\return		La profondeur de la texture, 1 si pas 3D.
		 */
		inline uint32_t getDepth()const
		{
			return m_type == TextureType::eThreeDimensions ? m_depth : 1;
		}
		/**
		 *\~english
		 *\return		The texture layer count, 1 if not array.
		 *\~french
		 *\return		Le nombre de couches de la texture, 1 si pas tableau.
		 */
		inline uint32_t getLayersCount()const
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
		 *\param[in]	size	The image size.
		 *\param[in]	format	The image format.
		 *\~french
		 *\brief		Met à jour les dimensions et le format, depuis la première image chargée.
		 *\param[in]	size	Les dimensions de l'image.
		 *\param[in]	format	Le format des pixels de l'image.
		 */
		void doUpdateFromFirstImage( castor::Size const & size, castor::PixelFormat format );
		/**
		 *\~english
		 *\brief		Reinitialises the storage.
		 *\remarks		If the storage was not created, does nothing.
		 *\~french
		 *\brief		Réinitialise le stockage.
		 *\remarks		Ne fait rien si le stockage n'avait pas été créé.
		 */
		bool doResetStorage();
		/**
		 *\~english
		 *\brief		Initialises the GPU storage.
		 *\param[in]	type	The storage type.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise le stockage GPU.
		 *\param[in]	type	Le type de stockage.
		 *\return		\p true si tout s'est bien passé.
		 */
		bool doCreateStorage( TextureStorageType type );

	private:
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool doInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API virtual void doCleanup() = 0;
		/**
		 *\~english
		 *\brief		API specific binding function
		 *\~french
		 *\brief		Activation spécifique selon l'API
		 */
		C3D_API virtual void doBind( uint32_t index )const = 0;
		/**
		 *\~english
		 *\brief		API specific unbinding function
		 *\~french
		 *\brief		Désactivation spécifique selon l'API
		 */
		C3D_API virtual void doUnbind( uint32_t index )const = 0;

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
		castor::Size m_size;
		//!\~english	The texture pixels' format.
		//!\~french		Le format des pixels de la texture.
		castor::PixelFormat m_format;
		//!\~english	The texture's depth or layers count.
		//!\~french		La profondeur ou nombre de couches de la texture.
		uint32_t m_depth{ 1 };
		//!\~english	The texture's mipmap count.
		//!\~french		Le nombre de mipmaps de la texture.
		uint32_t m_mipmapCount{ ~( 0u ) };
	};
}

#endif
