/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TEXTURE_LAYOUT_H___
#define ___C3D_TEXTURE_LAYOUT_H___

#include "Castor3D/Texture/TextureView.hpp"

#include <Ashes/Image/ImageCreateInfo.hpp>
#include <Ashes/Image/Texture.hpp>

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
		 *\param[in]	renderSystem		The render system.
		 *\param[in]	info				The image informations.
		 *\param[in]	memoryProperties	The required memory properties.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem		Le render system.
		 *\param[in]	info				Les informations de l'image.
		 *\param[in]	memoryProperties	Les propriétés requise pour la mémoire.
		 */
		C3D_API TextureLayout( RenderSystem & renderSystem
			, ashes::ImageCreateInfo info
			, ashes::MemoryPropertyFlags memoryProperties );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~TextureLayout();
		/**
		 *\~english
		 *\brief		Defines the texture buffer from an image file.
		 *\param[in]	folder		The folder containing the image.
		 *\param[in]	relative	The image file path, relative to folder.
		 *\param[in]	components	The components to keep in the loaded image.
		 *\~french
		 *\brief		Définit le tampon de la texture depuis un fichier image.
		 *\param[in]	folder		Le dossier contenant l'image.
		 *\param[in]	relative	Le chemin d'accès à l'image, relatif à folder.
		 *\param[in]	components	Les composantes à garder dans l'image chargée.
		 */
		C3D_API void setSource( castor::Path const & folder
			, castor::Path const & relative
			, ImageComponents components );
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
		 *\brief		Initialises the texture and all its views.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise la texture et toutes ses vues.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleans up the texture and all its views.
		 *\~french
		 *\brief		Nettoie la texture et toutes ses vues.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Generate texture mipmaps
		 *\~french
		 *\brief		Génère les mipmaps de la texture
		 */
		C3D_API void generateMipmaps()const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		inline bool isInitialised()const
		{
			return m_initialised;
		}

		inline ashes::TextureType getType()const
		{
			return m_info.imageType;
		}

		inline ashes::Texture const & getTexture()const
		{
			CU_Require( m_texture );
			return *m_texture;
		}

		inline TextureView const & getView( size_t index = 0u )const
		{
			return getImage( index );
		}

		inline TextureView const & getImage( size_t index = 0u )const
		{
			CU_Require( index < m_views.size() && m_views[index] );
			return *m_views[index];
		}

		inline TextureView & getImage( size_t index = 0u )
		{
			CU_Require( index < m_views.size() && m_views[index] );
			return *m_views[index];
		}

		inline TextureView const & getImage( CubeMapFace index )const
		{
			return getImage( size_t( index ) );
		}

		inline TextureView & getImage( CubeMapFace index )
		{
			return getImage( size_t( index ) );
		}

		inline TextureView const & getDefaultImage()const
		{
			CU_Require( m_defaultView );
			return *m_defaultView;
		}

		inline TextureView & getDefaultImage()
		{
			CU_Require( m_defaultView );
			return *m_defaultView;
		}

		inline ashes::TextureView const & getDefaultView()const
		{
			return m_defaultView->getView();
		}

		inline uint32_t getWidth()const
		{
			return m_info.extent.width;
		}

		inline uint32_t getHeight()const
		{
			return m_info.extent.height;
		}

		inline uint32_t getDepth()const
		{
			return m_info.extent.depth;
		}

		inline uint32_t getMipmapCount()const
		{
			return m_info.mipLevels;
		}

		inline ashes::Extent3D const & getDimensions()const
		{
			return m_info.extent;
		}

		inline ashes::Format getPixelFormat()const
		{
			return m_info.format;
		}

		inline uint32_t getLayersCount()const
		{
			return m_info.arrayLayers;
		}

		inline auto begin()
		{
			return m_views.begin();
		}

		inline auto begin()const
		{
			return m_views.begin();
		}

		inline auto end()
		{
			return m_views.end();
		}

		inline auto end()const
		{
			return m_views.end();
		}
		/**@}*/

	private:
		void doUpdateFromFirstImage( castor::Size const & size, ashes::Format format );

	private:
		bool m_initialised{ false };
		ashes::ImageCreateInfo m_info;
		ashes::MemoryPropertyFlags m_properties;
		std::vector< TextureViewUPtr > m_views;
		TextureViewUPtr m_defaultView;
		ashes::TexturePtr m_texture;
	};
}

#endif
