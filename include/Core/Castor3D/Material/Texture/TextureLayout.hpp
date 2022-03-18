/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureLayout_H___
#define ___C3D_TextureLayout_H___

#include "TextureModule.hpp"

#include "Castor3D/Material/Texture/TextureView.hpp"
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
		void forMipView( FuncT function )const
		{
			function( view );
		}

		template< typename FuncT >
		void forEachLeafView( FuncT function )const
		{
			if ( !levels.empty() )
			{
				for ( auto & level : levels )
				{
					function( level );
				}
			}
			else
			{
				forMipView( function );
			}
		}

		template< typename FuncT >
		void forEachView( FuncT function )const
		{
			forMipView( function );
			forEachLeafView( function );
		}

		template< typename FuncT >
		void forEachFirstMipView( FuncT function )const
		{
			if ( !levels.empty() )
			{
				function( *levels.begin() );
			}
			else
			{
				forMipView( function );
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
		
		template< typename FuncT >
		void forEachFirstMipView( FuncT function )const
		{
			view.forEachFirstMipView( function );
		}

		template< typename FuncT >
		void forEachLeafView( FuncT function )const
		{
			for ( auto & face : faces )
			{
				face.forEachLeafView( function );
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

		template< typename FuncT >
		void forEachFirstMipView( FuncT function )const
		{
			for ( auto & layer : layers )
			{
				layer.forEachFirstMipView( function );
			}
		}

		template< typename FuncT >
		void forEachLeafView( FuncT function )const
		{
			for ( auto & layer : layers )
			{
				layer.forEachLeafView( function );
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

		template< typename FuncT >
		void forEachFirstMipView( FuncT function )const
		{
			for ( auto & slice : slices )
			{
				slice.forEachFirstMipView( function );
			}
		}

		template< typename FuncT >
		void forEachLeafView( FuncT function )const
		{
			for ( auto & slice : slices )
			{
				slice.forEachLeafView( function );
			}
		}
	};

	C3D_API TextureLayoutSPtr createTextureLayout( Engine const & engine
		, castor::Path const & relative
		, castor::Path const & folder
		, castor::ImageLoaderConfig config = { true, true, true } );
	C3D_API TextureLayoutSPtr createTextureLayout( Engine const & engine
		, castor::String const & name
		, castor::PxBufferBaseUPtr buffer
		, bool isStatic = false );
	C3D_API uint32_t getMipLevels( VkExtent3D const & extent
		, VkFormat format );

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
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise la texture et toutes ses vues.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( RenderDevice const & device
			, QueueData const & queueData );
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
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Génère les mipmaps de la texture
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void generateMipmaps( RenderDevice const & device )const;
		/**
		 *\~english
		 *\brief		Generate texture mipmaps
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\~french
		 *\brief		Génère les mipmaps de la texture
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 */
		C3D_API void generateMipmaps( QueueData const & queueData )const;
		/**
		 *\~english
		 *\brief		Generate texture mipmaps
		 *\param[in]	cmd	The command buffer recording the commands.
		 *\~french
		 *\brief		Génère les mipmaps de la texture
		 *\param[in]	cmd	Le command buffer recevant les commandes.
		 */
		C3D_API void generateMipmaps( ashes::CommandBuffer & cmd )const;
		/**
		 *\name Whole texture access.
		 **/
		/**@{*/
		/**
		 *\~english
		 *\brief		Sets the whole layout source.
		 *\~french
		 *\brief		Définit la source de tout le layout.
		 */
		C3D_API void setSource( castor::PxBufferBaseSPtr buffer
			, uint32_t bufferOrigLevels
			, bool isStatic = false );
		C3D_API void setSource( castor::PxBufferBaseSPtr buffer
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
		/**
		 *\name Getters.
		 **/
		/**@{*/
		TextureView const & getDefaultView()const
		{
			CU_Require( m_defaultView.view );
			return *m_defaultView.view;
		}

		TextureView & getDefaultView()
		{
			CU_Require( m_defaultView.view );
			return *m_defaultView.view;
		}

		MipView const & getDefault()const
		{
			return m_defaultView;
		}
		/**@}*/
		/**@}*/
		/**
		*\name
		*	2D texture's mip level access.
		**/
		/**@{*/
		TextureView const & getMipView( size_t level )const
		{
			CU_Require( getDefault().levels.size() > level );
			return *getDefault().levels[level];
		}

		TextureView & getMipView( size_t level )
		{
			CU_Require( getDefault().levels.size() > level );
			return *getDefault().levels[level];
		}
		/**@}*/
		/**
		 *\name Texture array's layer access.
		 **/
		/**@{*/
		/**
		 *\~english
		 *\brief		Sets the source for all mip lever of one layer in the layout.
		 *\~french
		 *\brief		Définit la source pour tous les mip levels une couche du layout.
		 */
		C3D_API void setLayerSource( uint32_t index
			, castor::PxBufferBaseSPtr buffer
			, uint32_t bufferOrigLevels );
		C3D_API void setLayerSource( uint32_t index
			, castor::PxBufferBaseSPtr buffer );
		C3D_API void setLayerSource( uint32_t index
			, castor::Path const & folder
			, castor::Path const & relative
			, castor::ImageLoaderConfig config = { true, true, false } );
		C3D_API void setLayerSource( uint32_t index
			, VkExtent3D const & extent
			, VkFormat format );
		void setLayerSource( uint32_t index
			, VkExtent2D const & extent
			, VkFormat format )
		{
			return setLayerSource( index
				, { extent.width, extent.height, 1u }
			, format );
		}
		/**
		 *\name Getters.
		 **/
		/**@{*/
		uint32_t getLayersCount()const
		{
			return m_info->arrayLayers;
		}

		ArrayView< MipView > const & getArray2D()const
		{
			return m_arrayView;
		}

		MipView const & getLayer2D( size_t layer )const
		{
			CU_Require( m_cubeView.layers.empty() );
			CU_Require( m_sliceView.slices.empty() );
			CU_Require( getLayersCount() > 1u );
			CU_Require( layer < m_arrayView.layers.size() );
			return m_arrayView.layers[layer];
		}

		MipView & getLayer2D( size_t layer )
		{
			CU_Require( m_cubeView.layers.empty() );
			CU_Require( m_sliceView.slices.empty() );
			CU_Require( getLayersCount() > 1u );
			CU_Require( layer < m_arrayView.layers.size() );
			return m_arrayView.layers[layer];
		}

		TextureView const & getLayer2DView( size_t layer )const
		{
			CU_Require( getLayer2D( layer ).view );
			return *getLayer2D( layer ).view;
		}

		TextureView & getLayer2DView( size_t layer )
		{
			CU_Require( getLayer2D( layer ).view );
			return *getLayer2D( layer ).view;
		}
		/**@}*/
		/**
		 *\name Texture array layer's mip level access.
		 **/
		/**@{*/
		/**
		 *\~english
		 *\brief		Sets the source for one mip level of a layer in the layout.
		 *\~french
		 *\brief		Définit la source pour un mip level d'une couche du layout.
		 */
		C3D_API void setLayerMipSource( uint32_t index
			, uint32_t level
			, castor::PxBufferBaseSPtr buffer );
		C3D_API void setLayerMipSource( uint32_t index
			, uint32_t level
			, castor::Path const & folder
			, castor::Path const & relative
			, castor::ImageLoaderConfig config = { true, false, false } );
		C3D_API void setLayerMipSource( uint32_t index
			, uint32_t level
			, VkExtent3D const & extent
			, VkFormat format );
		void setLayerMipSource( uint32_t index
			, uint32_t level
			, VkExtent2D const & extent
			, VkFormat format )
		{
			return setLayerMipSource( index
				, level
				, { extent.width, extent.height, 1u }
				, format );
		}
		/**@}*/
		/**
		 *\name 3D texture's slice access.
		 **/
		/**@{*/
		SliceView< MipView > const & getSlices3D()const
		{
			return m_sliceView;
		}

		MipView const & getSlice( size_t slice )const
		{
			CU_Require( m_cubeView.layers.empty() );
			CU_Require( m_arrayView.layers.empty() );
			CU_Require( getDepth() > 1u );
			CU_Require( slice < m_sliceView.slices.size() );
			return m_sliceView.slices[slice];
		}

		MipView & getSlice( size_t slice )
		{
			CU_Require( m_cubeView.layers.empty() );
			CU_Require( m_arrayView.layers.empty() );
			CU_Require( getDepth() > 1u );
			CU_Require( slice < m_sliceView.slices.size() );
			return m_sliceView.slices[slice];
		}

		TextureView const & getSliceView( size_t slice )const
		{
			CU_Require( getSlice( slice ).view );
			return *getSlice( slice ).view;
		}

		TextureView & getSliceView( size_t slice )
		{
			CU_Require( getSlice( slice ).view );
			return *getSlice( slice ).view;
		}
		/**@}*/
		/**
		*\name
		*	Cube array's texture access.
		*\remarks
		*	A simple cube texture is a cube array of size 1.
		**/
		/**@{*/
		uint32_t isCube()const
		{
			return getLayersCount() >= 6u
				&& ( getLayersCount() % 6u ) == 0u
				&& ashes::checkFlag( m_info->flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT );
		}

		ArrayView< CubeView > const & getArrayCube()const
		{
			return m_cubeView;
		}

		CubeView const & getLayerCube( size_t layer )const
		{
			CU_Require( m_arrayView.layers.empty() );
			CU_Require( m_sliceView.slices.empty() );
			CU_Require( isCube() );
			CU_Require( layer < m_cubeView.layers.size() );
			return m_cubeView.layers[layer];
		}

		CubeView & getLayerCube( size_t layer )
		{
			CU_Require( m_arrayView.layers.empty() );
			CU_Require( m_sliceView.slices.empty() );
			CU_Require( isCube() );
			CU_Require( layer < m_cubeView.layers.size() );
			return m_cubeView.layers[layer];
		}

		TextureView const & getLayerCubeView( size_t layer )const
		{
			CU_Require( getLayerCube( layer ).view.view );
			return *getLayerCube( layer ).view.view;
		}

		TextureView & getLayerCubeView( size_t layer )
		{
			CU_Require( getLayerCube( layer ).view.view );
			return *getLayerCube( layer ).view.view;
		}
		/**@}*/
		/**
		 *\name Cube array texture layer's face access.
		 **/
		/**@{*/
		MipView const & getLayerCubeFace( size_t layer
			, CubeMapFace face )const
		{
			return getLayerCube( layer ).faces[size_t( face )];
		}

		MipView & getLayerCubeFace( size_t layer
			, CubeMapFace face )
		{
			return getLayerCube( layer ).faces[size_t( face )];
		}

		TextureView const & getLayerCubeFaceView( size_t layer
			, CubeMapFace face )const
		{
			CU_Require( getLayerCubeFace( layer, face ).view );
			return *getLayerCubeFace( layer, face ).view;
		}

		TextureView & getLayerCubeFaceView( size_t layer
			, CubeMapFace face )
		{
			CU_Require( getLayerCubeFace( layer, face ).view );
			return *getLayerCubeFace( layer, face ).view;
		}
		/**@{*/
		/**
		 *\~english
		 *\brief		Sets the source for a layer cube's face in the layout.
		 *\~french
		 *\brief		Définit la source pour une face d'un cube d'un tableau dans le layout.
		 */
		C3D_API void setLayerCubeFaceSource( uint32_t layer
			, CubeMapFace face
			, castor::PxBufferBaseSPtr buffer );
		C3D_API void setLayerCubeFaceSource( uint32_t layer
			, CubeMapFace face
			, castor::Path const & folder
			, castor::Path const & relative
			, castor::ImageLoaderConfig config = { true, true, false } );
		void setLayerCubeFaceSource( uint32_t layer
			, CubeMapFace face
			, VkExtent2D const & extent
			, VkFormat format );
		/**@}*/
		/**@}*/
		/**
		 *\name Cube array texture layer face's mip level access.
		 **/
		/**@{*/
		TextureView const & getLayerCubeFaceMipView( size_t layer
			, CubeMapFace face
			, uint32_t level )const
		{
			CU_Require( getLayerCubeFace( layer, face ).levels.size() > level );
			CU_Require( getLayerCubeFace( layer, face ).levels[level] );
			return *getLayerCubeFace( layer, face ).levels[level];
		}

		TextureView & getLayerCubeFaceMipView( size_t layer
			, CubeMapFace face
			, uint32_t level )
		{
			CU_Require( getLayerCubeFace( layer, face ).levels.size() > level );
			CU_Require( getLayerCubeFace( layer, face ).levels[level] );
			return *getLayerCubeFace( layer, face ).levels[level];
		}
		/**@{*/
		/**
		 *\~english
		 *\brief		Sets the source for a face of layer in the layout.
		 *\~french
		 *\brief		Définit la source pour un mip level d'une couche du layout.
		 */
		C3D_API void setLayerCubeFaceMipSource( uint32_t layer
			, CubeMapFace face
			, uint32_t level
			, castor::PxBufferBaseSPtr buffer );
		C3D_API void setLayerCubeFaceMipSource( uint32_t layer
			, CubeMapFace face
			, uint32_t level
			, castor::Path const & folder
			, castor::Path const & relative
			, castor::ImageLoaderConfig config = { true, false, false } );
		void setLayerCubeFaceMipSource( uint32_t layer
			, CubeMapFace face
			, uint32_t level
			, VkExtent2D const & extent
			, VkFormat format );
		/**@}*/
		/**@}*/
		/**
		*name
		*	Getters.
		**/
		/**@{*/
		C3D_API castor::String getName()const;
		C3D_API castor::Path getPath()const;
		C3D_API bool needsYInversion()const;

		bool isInitialised()const
		{
			return m_initialised;
		}

		bool isStatic()const
		{
			return m_static;
		}

		VkImageType getType()const
		{
			return m_info->imageType;
		}

		castor::Image const & getImage()const
		{
			return m_image;
		}

		castor::Image & getImage()
		{
			return m_image;
		}

		ashes::Image const & getTexture()const
		{
			CU_Require( m_texture );
			return *m_texture;
		}

		ashes::ImageCreateInfo const & getCreateInfo()const
		{
			return m_info;
		}

		uint32_t getWidth()const
		{
			return m_info->extent.width;
		}

		uint32_t getHeight()const
		{
			return m_info->extent.height;
		}

		uint32_t getDepth()const
		{
			return m_info->extent.depth;
		}

		uint32_t getMipmapCount()const
		{
			return m_info->mipLevels;
		}

		VkExtent3D const & getDimensions()const
		{
			return m_info->extent;
		}

		VkFormat getPixelFormat()const
		{
			return m_info->format;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Views parsing.
		*\~french
		*name
		*	Parsing des vues.
		**/
		/**@{*/
		template< typename FuncT >
		void forEachView( FuncT function )const
		{
			m_defaultView.forEachView( function );

			if ( !m_cubeView.layers.empty() )
			{
				m_cubeView.forEachView( function );
			}
			else if ( !m_arrayView.layers.empty() )
			{
				m_arrayView.forEachView( function );
			}
			else if ( !m_sliceView.slices.empty() )
			{
				m_sliceView.forEachView( function );
			}
		}
		
		template< typename FuncT >
		void forEachFirstMipView( FuncT function )const
		{
			if ( !m_cubeView.layers.empty() )
			{
				m_cubeView.forEachFirstMipView( function );
			}
			else if ( !m_arrayView.layers.empty() )
			{
				m_arrayView.forEachFirstMipView( function );
			}
			else if ( !m_sliceView.slices.empty() )
			{
				m_sliceView.forEachFirstMipView( function );
			}
			else
			{
				m_defaultView.forEachFirstMipView( function );
			}
		}

		template< typename FuncT >
		void forEachLeafView( FuncT function )const
		{
			if ( !m_cubeView.layers.empty() )
			{
				m_cubeView.forEachLeafView( function );
			}
			else if ( !m_arrayView.layers.empty() )
			{
				m_arrayView.forEachLeafView( function );
			}
			else if ( !m_sliceView.slices.empty() )
			{
				m_sliceView.forEachLeafView( function );
			}
		}
		/**@}*/

	private:
		uint32_t doUpdateViews();
		void doUpdateCreateInfo( castor::ImageLayout const & layout );
		void doUpdateFromFirstImage( uint32_t mipLevel, castor::ImageLayout layout );
		void doUpdateMips( bool genNeeded, uint32_t mipLevels );
		void doUpdateLayerMip( bool genNeeded, uint32_t layer, uint32_t level );
		void doUpdateLayerMips( bool genNeeded, uint32_t layer, uint32_t mipLevels );

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
