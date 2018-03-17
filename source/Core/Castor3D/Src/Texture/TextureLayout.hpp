/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TEXTURE_LAYOUT_H___
#define ___C3D_TEXTURE_LAYOUT_H___

#include "TextureView.hpp"

#include <Image/ImageCreateInfo.hpp>
#include <Image/Texture.hpp>

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
			, renderer::ImageCreateInfo info
			, renderer::MemoryPropertyFlags memoryProperties );
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

		inline renderer::TextureType getType()const
		{
			return m_info.imageType;
		}

		inline renderer::Texture const & getTexture()const
		{
			REQUIRE( m_texture );
			return *m_texture;
		}

		inline TextureView const & getImage( size_t index = 0u )const
		{
			REQUIRE( index < m_views.size() && m_views[index] );
			return *m_views[index];
		}

		inline TextureView & getImage( size_t index = 0u )
		{
			REQUIRE( index < m_views.size() && m_views[index] );
			return *m_views[index];
		}

		inline TextureView const & getDefaultImage()const
		{
			REQUIRE( m_defaultView );
			return *m_defaultView;
		}

		inline TextureView & getDefaultImage()
		{
			REQUIRE( m_defaultView );
			return *m_defaultView;
		}

		inline renderer::TextureView const & getDefaultView()const
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

		inline renderer::Extent3D const & getDimensions()const
		{
			return m_info.extent;
		}

		inline renderer::Format getPixelFormat()const
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
		void doUpdateFromFirstImage( castor::Size const & size, renderer::Format format );

	private:
		bool m_initialised{ false };
		renderer::ImageCreateInfo m_info;
		renderer::MemoryPropertyFlags m_properties;
		std::vector< TextureViewUPtr > m_views;
		TextureViewUPtr m_defaultView;
		renderer::TexturePtr m_texture;
	};
}

#endif
