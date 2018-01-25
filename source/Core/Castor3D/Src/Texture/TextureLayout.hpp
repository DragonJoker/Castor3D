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
			, renderer::TextureType type
			, renderer::ImageUsageFlags usage
			, renderer::MemoryPropertyFlags memoryProperties );
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
			, renderer::TextureType type
			, renderer::ImageUsageFlags usage
			, renderer::MemoryPropertyFlags memoryProperties
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
			, renderer::TextureType type
			, renderer::ImageUsageFlags usage
			, renderer::MemoryPropertyFlags memoryProperties
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
			, renderer::TextureType type
			, renderer::ImageUsageFlags usage
			, renderer::MemoryPropertyFlags memoryProperties
			, castor::PixelFormat format
			, castor::Point3ui const & size );
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
			return m_type;
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

		inline renderer::Texture const & getTexture()const
		{
			REQUIRE( m_texture );
			return *m_texture;
		}

		inline renderer::TextureView const & getView()const
		{
			return m_defaultView.getView();
		}

		inline uint32_t getWidth()const
		{
			return m_size.getWidth();
		}

		inline uint32_t getHeight()const
		{
			return m_size.getHeight();
		}

		inline uint32_t getMipmapCount()const
		{
			return m_mipmapCount;
		}

		inline castor::Size const & getDimensions()const
		{
			return m_size;
		}

		inline castor::PixelFormat getPixelFormat()const
		{
			return m_format;
		}

		inline uint32_t getDepth()const
		{
			return m_type == renderer::TextureType::e3D ? m_depth : 1;
		}

		inline uint32_t getLayersCount()const
		{
			return ( m_type == renderer::TextureType::e2DArray
					|| m_type == renderer::TextureType::e1DArray
					|| m_type == renderer::TextureType::eCubeArray )
				? m_depth
				: 1;
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
		void doUpdateFromFirstImage( castor::Size const & size, castor::PixelFormat format );

	private:
		bool m_initialised{ false };
		renderer::TextureType m_type;
		renderer::ImageUsageFlags m_usage;
		renderer::MemoryPropertyFlags m_properties;
		castor::Size m_size;
		castor::PixelFormat m_format;
		uint32_t m_depth{ 1 };
		uint32_t m_mipmapCount{ ~( 0u ) };
		std::vector< TextureViewUPtr > m_views;
		TextureView & m_defaultView;
		renderer::TexturePtr m_texture;
	};
}

#endif
