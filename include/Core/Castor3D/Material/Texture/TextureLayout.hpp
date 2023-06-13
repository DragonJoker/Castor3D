/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureLayout_H___
#define ___C3D_TextureLayout_H___

#include "TextureModule.hpp"

#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageCreateInfo.hpp>

namespace castor3d
{
	struct MipView
	{
		TextureViewUPtr view;
		std::vector< TextureViewUPtr > levels;

		template< typename FuncT >
		void forEachView( FuncT function )const
		{
			function( view );

			if ( !levels.empty() )
			{
				for ( auto & level : levels )
				{
					function( level );
				}
			}
		}
	};

	struct CubeView
	{
		MipView view;
		std::vector< MipView > faces;

		template< typename FuncT >
		void forEachView( FuncT function )const
		{
			view.forEachView( function );

			for ( auto & face : faces )
			{
				face.forEachView( function );
			}
		}
	};

	template< typename ViewT >
	struct ArrayView
	{
		MipView * view;
		std::vector< ViewT > layers{};

		template< typename FuncT >
		void forEachView( FuncT function )const
		{
			for ( auto & layer : layers )
			{
				layer.forEachView( function );
			}
		}
	};

	template< typename ViewT >
	struct SliceView
	{
		MipView * view;
		std::vector< ViewT > slices{};

		template< typename FuncT >
		void forEachView( FuncT function )const
		{
			for ( auto & slice : slices )
			{
				slice.forEachView( function );
			}
		}
	};

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
		 *\param[in]	isStatic			Tells if this layout is static.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem		Le render system.
		 *\param[in]	info				Les informations de l'image.
		 *\param[in]	memoryProperties	Les propriétés requise pour la mémoire.
		 *\param[in]	debugName			Le nom de debug pour ce layout.
		 *\param[in]	isStatic			Dit si ce layout est statique.
		 */
		C3D_API TextureLayout( RenderSystem & renderSystem
			, ashes::ImageCreateInfo info
			, VkMemoryPropertyFlags memoryProperties
			, castor::String debugName
			, bool isStatic = false );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	image			The image.
		 *\param[in]	imageView		The image view.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	image			L'image.
		 *\param[in]	imageView		La vue sur l'image.
		 */
		C3D_API TextureLayout( RenderSystem & renderSystem
			, castor::String const & name
			, VkImage image
			, crg::ImageViewId imageView );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	image			The image.
		 *\param[in]	createInfo		The image informations.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	image			L'image.
		 *\param[in]	createInfo		Les informations de l'image.
		 */
		C3D_API TextureLayout( RenderSystem & renderSystem
			, ashes::ImagePtr image
			, VkImageCreateInfo const & createInfo );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~TextureLayout();
		/**
		 *\~english
		 *\brief		Initialises the texture and all its views.
		 *\param[in]	device		The GPU device.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise la texture et toutes ses vues.
		 *\param[in]	device		Le device GPU.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Uploads image data to the texture buffer.
		 *\~french
		 *\brief		Upload les données d'image dans le buffer de la texture.
		 */
		C3D_API void upload( UploadData & uploader );
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
		 *\param[in]	device		The GPU device.
		 *\param[in]	srcLayout	The current layout of the image.
		 *\~french
		 *\brief		Génère les mipmaps de la texture
		 *\param[in]	device		Le device GPU.
		 *\param[in]	srcLayout	Le layout actuel de l'image.
		 */
		C3D_API void generateMipmaps( RenderDevice const & device
			, VkImageLayout srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )const;
		/**
		 *\~english
		 *\brief		Generate texture mipmaps
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\param[in]	srcLayout	The current layout of the image.
		 *\~french
		 *\brief		Génère les mipmaps de la texture
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 *\param[in]	srcLayout	Le layout actuel de l'image.
		 */
		C3D_API void generateMipmaps( QueueData const & queueData
			, VkImageLayout srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )const;
		/**
		 *\~english
		 *\brief		Generate texture mipmaps
		 *\param[in]	cmd			The command buffer recording the commands.
		 *\param[in]	srcLayout	The current layout of the image.
		 *\~french
		 *\brief		Génère les mipmaps de la texture
		 *\param[in]	cmd			Le command buffer recevant les commandes.
		 *\param[in]	srcLayout	Le layout actuel de l'image.
		 */
		C3D_API void generateMipmaps( ashes::CommandBuffer & cmd
			, VkImageLayout srcLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )const;
		/**
		 *\name Texture source setup.
		 **/
		/**@{*/
		/**
		 *\~english
		 *\brief		Sets the whole layout source.
		 *\~french
		 *\brief		Définit la source de tout le layout.
		 */
		C3D_API void setSource( castor::PxBufferBaseUPtr buffer
			, uint32_t bufferOrigLevels
			, bool isStatic = false );
		C3D_API void setSource( castor::PxBufferBaseUPtr buffer
			, bool isStatic = false );
		C3D_API void setSource( castor::Path const & folder
			, castor::Path const & relative
			, castor::ImageLoaderConfig config = { true, true, true } );
		C3D_API void setSource( VkExtent3D const & extent
			, VkFormat format );

		void setSource( VkExtent2D const & extent
			, VkFormat format )
		{
			return setSource( { extent.width, extent.height, 1u }
				, format );
		}
		/**@}*/
		/**
		 *\name Getters.
		 **/
		/**@{*/
		C3D_API castor::String getDefaultSourceString()const;
		C3D_API ashes::ImageView const & getDefaultSampledView()const noexcept;
		C3D_API ashes::ImageView const & getDefaultTargetView()const noexcept;
		C3D_API castor::String getLayerCubeSourceString( size_t layer )const noexcept;
		C3D_API ashes::ImageView const & getLayerCubeTargetView( size_t layer )const noexcept;
		C3D_API castor::String getLayerCubeFaceSourceString( size_t layer
			, CubeMapFace face )const noexcept;
		C3D_API ashes::ImageView const & getLayerCubeFaceTargetView( size_t layer
			, CubeMapFace face )const noexcept;
		C3D_API castor::String getName()const;
		C3D_API castor::Path getPath()const;
		C3D_API bool needsYInversion()const;
		C3D_API bool hasBuffer()const;

		uint32_t getLayersCount()const noexcept
		{
			return m_info->arrayLayers;
		}

		uint32_t isCube()const noexcept
		{
			return getLayersCount() >= 6u
				&& ( getLayersCount() % 6u ) == 0u
				&& ashes::checkFlag( m_info->flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT );
		}

		bool isInitialised()const noexcept
		{
			return m_initialised;
		}

		bool isStatic()const noexcept
		{
			return m_static;
		}

		VkImageType getType()const noexcept
		{
			return m_info->imageType;
		}

		castor::Image const & getImage()const noexcept
		{
			return m_image;
		}

		castor::Image & getImage()noexcept
		{
			return m_image;
		}

		ashes::Image const & getTexture()const noexcept
		{
			CU_Require( m_texture );
			return *m_texture;
		}

		ashes::ImageCreateInfo const & getCreateInfo()const noexcept
		{
			return m_info;
		}

		uint32_t getWidth()const noexcept
		{
			return m_info->extent.width;
		}

		uint32_t getHeight()const noexcept
		{
			return m_info->extent.height;
		}

		uint32_t getDepth()const noexcept
		{
			return m_info->extent.depth;
		}

		uint32_t getMipLevels()const noexcept
		{
			return m_info->mipLevels;
		}

		VkExtent3D const & getDimensions()const noexcept
		{
			return m_info->extent;
		}

		VkFormat getPixelFormat()const noexcept
		{
			return m_info->format;
		}
		/**@}*/

	private:
		uint32_t doUpdateViews();
		void doUpdateCreateInfo( castor::ImageLayout const & layout );
		void doUpdateMips( bool genNeeded, uint32_t mipLevels );

		TextureView & getDefaultView()const noexcept
		{
			CU_Require( m_defaultView.view );
			return *m_defaultView.view;
		}

		CubeView const & getLayerCube( size_t layer )const noexcept
		{
			CU_Require( m_arrayView.layers.empty() );
			CU_Require( m_sliceView.slices.empty() );
			CU_Require( isCube() );
			CU_Require( layer < m_cubeView.layers.size() );
			return m_cubeView.layers[layer];
		}

		MipView const & getLayerCubeFace( size_t layer
			, CubeMapFace face )const noexcept
		{
			return getLayerCube( layer ).faces[size_t( face )];
		}

	private:
		bool m_initialised{ false };
		bool m_static{ false };
		ashes::ImageCreateInfo m_info;
		VkMemoryPropertyFlags m_properties;
		castor::Image m_image;
		MipView m_defaultView;
		ArrayView< MipView > m_arrayView;
		ArrayView< CubeView > m_cubeView;
		SliceView< MipView > m_sliceView;
		ashes::ImagePtr m_ownTexture;
		ashes::Image * m_texture{};
	};

	inline ashes::ImagePtr makeImage( RenderDevice const & device
		, ashes::ImageCreateInfo createInfo
		, VkMemoryPropertyFlags flags
		, std::string const & name )
	{
		auto result = device->createImage( name + "Map", std::move( createInfo ) );
		auto requirements = result->getMemoryRequirements();
		uint32_t deduced = device->deduceMemoryType( requirements.memoryTypeBits
			, flags );
		auto memory = device->allocateMemory( name + "MapMem"
			, VkMemoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, requirements.size, deduced } );
		result->bindMemory( std::move( memory ) );
		return result;
	}
}

#endif
