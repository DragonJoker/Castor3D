/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureLayout_H___
#define ___C3D_TextureLayout_H___

#include "TextureModule.hpp"

#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageCreateInfo.hpp>

namespace castor3d
{
	C3D_API TextureLayoutSPtr createTextureLayout( Engine const & engine
		, castor::Path const & relative
		, castor::Path const & folder );
	C3D_API TextureLayoutSPtr createTextureLayout( Engine const & engine
		, castor::String const & name
		, castor::PxBufferBaseSPtr buffer );
	C3D_API uint32_t getMipLevels( VkExtent3D const & extent );

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
		 *\param[in]	debugName			The debug name for this layout.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem		Le render system.
		 *\param[in]	info				Les informations de l'image.
		 *\param[in]	memoryProperties	Les propriétés requise pour la mémoire.
		 *\param[in]	debugName			Le nom de debug pour ce layout.
		 */
		C3D_API TextureLayout( RenderSystem & renderSystem
			, ashes::ImageCreateInfo info
			, VkMemoryPropertyFlags memoryProperties
			, castor::String debugName );
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
		 *\~french
		 *\brief		Définit le tampon de la texture depuis un fichier image.
		 *\param[in]	folder		Le dossier contenant l'image.
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
		C3D_API castor::String getName()const;

		inline bool isInitialised()const
		{
			return m_initialised;
		}

		inline VkImageType getType()const
		{
			return m_info->imageType;
		}

		inline ashes::Image const & getTexture()const
		{
			CU_Require( m_texture );
			return *m_texture;
		}

		inline bool needsYInversion()const
		{
			CU_Require( m_defaultView );
			return m_defaultView->needsYInversion();
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

		inline ashes::ImageView const & getDefaultView()const
		{
			return m_defaultView->getView();
		}

		inline uint32_t getWidth()const
		{
			return m_info->extent.width;
		}

		inline uint32_t getHeight()const
		{
			return m_info->extent.height;
		}

		inline uint32_t getDepth()const
		{
			return m_info->extent.depth;
		}

		inline uint32_t getMipmapCount()const
		{
			return m_info->mipLevels;
		}

		inline VkExtent3D const & getDimensions()const
		{
			return m_info->extent;
		}

		inline VkFormat getPixelFormat()const
		{
			return m_info->format;
		}

		inline uint32_t getLayersCount()const
		{
			return m_info->arrayLayers;
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
		void doUpdateFromFirstImage( castor::Size const & size, VkFormat format );

	private:
		bool m_initialised{ false };
		ashes::ImageCreateInfo m_info;
		VkMemoryPropertyFlags m_properties;
		std::vector< TextureViewUPtr > m_views;
		TextureViewUPtr m_defaultView;
		ashes::ImagePtr m_texture;
		castor::String m_debugName;
	};

	inline ashes::ImagePtr makeImage( RenderDevice const & device
		, ashes::ImageCreateInfo createInfo
		, VkMemoryPropertyFlags flags
		, std::string const & name )
	{
		auto result = device->createImage( std::move( createInfo ) );
		setDebugObjectName( device, *result, name + "Map" );
		auto requirements = result->getMemoryRequirements();
		uint32_t deduced = device->deduceMemoryType( requirements.memoryTypeBits
			, flags );
		auto memory = device->allocateMemory( VkMemoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, requirements.size, deduced } );
		setDebugObjectName( device, *memory, name + "MapMem" );
		result->bindMemory( std::move( memory ) );
		return result;
	}
}

#endif
