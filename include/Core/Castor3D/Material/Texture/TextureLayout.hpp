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
		void forEachView( FuncT function )const
		{
			function( view );

			for ( auto & level : levels )
			{
				function( level );
			}
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
				function( view );
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
		std::vector< ViewT > layers;

		template< typename FuncT >
		void forEachView( FuncT function )const
		{
			for ( auto & layer : layers )
			{
				layer.forEachView( function );
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
		std::vector< ViewT > slices;

		template< typename FuncT >
		void forEachView( FuncT function )const
		{
			for ( auto & slice : slices )
			{
				slice.forEachView( function );
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
		, castor::Path const & folder );
	C3D_API TextureLayoutSPtr createTextureLayout( Engine const & engine
		, castor::String const & name
		, castor::PxBufferBaseUPtr buffer );
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
			, castor::String debugName
			, bool isStatic = false );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~TextureLayout();
		/**
		 *\~english
		 *\brief		Sets the whole layout source.
		 *\~french
		 *\brief		Définit la source de tout le layout.
		 */
		C3D_API void setSource( castor::PxBufferBaseSPtr buffer
			, bool isStatic = false );
		C3D_API void setSource( castor::Path const & folder
			, castor::Path const & relative );
		C3D_API void setSource( VkExtent3D const & extent
			, VkFormat format );
		inline void setSource( VkExtent2D const & extent
			, VkFormat format )
		{
			return setSource( { extent.width, extent.height, 1u }
				, format );
		}
		/**
		 *\~english
		 *\brief		Sets the source for all mip lever of one layer in the layout.
		 *\~french
		 *\brief		Définit la source pour tous les mip levels une couche du layout.
		 */
		C3D_API void setLayerSource( uint32_t index
			, castor::PxBufferBaseSPtr buffer );
		C3D_API void setLayerSource( uint32_t index
			, castor::Path const & folder
			, castor::Path const & relative );
		C3D_API void setLayerSource( uint32_t index
			, VkExtent3D const & extent
			, VkFormat format );
		inline void setLayerSource( uint32_t index
			, VkExtent2D const & extent
			, VkFormat format )
		{
			return setLayerSource( index
				, { extent.width, extent.height, 1u }
				, format );
		}
		/**
		 *\~english
		 *\brief		Sets the source for one mip level of layer in the layout.
		 *\~french
		 *\brief		Définit la source pour un mip level d'une couche du layout.
		 */
		C3D_API void setLayerMipSource( uint32_t index
			, uint32_t level
			, castor::PxBufferBaseSPtr buffer );
		C3D_API void setLayerMipSource( uint32_t index
			, uint32_t level
			, castor::Path const & folder
			, castor::Path const & relative );
		C3D_API void setLayerMipSource( uint32_t index
			, uint32_t level
			, VkExtent3D const & extent
			, VkFormat format );
		inline void setLayerMipSource( uint32_t index
			, uint32_t level
			, VkExtent2D const & extent
			, VkFormat format )
		{
			return setLayerMipSource( index
				, level
				, { extent.width, extent.height, 1u }
				, format );
		}
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

		inline bool isStatic()const
		{
			return m_static;
		}

		inline VkImageType getType()const
		{
			return m_info->imageType;
		}

		inline castor::Image const & getImage()const
		{
			return m_image;
		}

		inline castor::Image & getImage()
		{
			return m_image;
		}

		inline ashes::Image const & getTexture()const
		{
			CU_Require( m_texture );
			return *m_texture;
		}

		inline bool needsYInversion()const
		{
			return getDefaultView().needsYInversion();
		}

		inline TextureView const & getDefaultView()const
		{
			CU_Require( m_defaultView.view );
			return *m_defaultView.view;
		}

		inline TextureView & getDefaultView()
		{
			CU_Require( m_defaultView.view );
			return *m_defaultView.view;
		}

		inline MipView const & getDefault()const
		{
			return m_defaultView;
		}

		inline TextureView const & getMipView( size_t level )const
		{
			CU_Require( getDefault().levels.size() > level );
			return *getDefault().levels[level];
		}

		inline TextureView & getMipView( size_t level )
		{
			CU_Require( getDefault().levels.size() > level );
			return *getDefault().levels[level];
		}

		inline ArrayView< MipView > const & getArray2D()const
		{
			return m_arrayView;
		}

		inline MipView const & getLayer2D( size_t layer )const
		{
			CU_Require( m_cubeView.layers.empty() );
			CU_Require( m_sliceView.slices.empty() );
			CU_Require( getLayersCount() > 1u );
			CU_Require( layer < m_arrayView.layers.size() );
			return m_arrayView.layers[layer];
		}

		inline MipView & getLayer2D( size_t layer )
		{
			CU_Require( m_cubeView.layers.empty() );
			CU_Require( m_sliceView.slices.empty() );
			CU_Require( getLayersCount() > 1u );
			CU_Require( layer < m_arrayView.layers.size() );
			return m_arrayView.layers[layer];
		}

		inline TextureView const & getLayer2DView( size_t layer )const
		{
			CU_Require( getLayer2D( layer ).view );
			return *getLayer2D( layer ).view;
		}

		inline TextureView & getLayer2DView( size_t layer )
		{
			CU_Require( getLayer2D( layer ).view );
			return *getLayer2D( layer ).view;
		}

		inline SliceView< MipView > const & getSlices3D()const
		{
			return m_sliceView;
		}

		inline MipView const & getSlice( size_t slice )const
		{
			CU_Require( m_cubeView.layers.empty() );
			CU_Require( m_arrayView.layers.empty() );
			CU_Require( getDepth() > 1u );
			CU_Require( slice < m_sliceView.slices.size() );
			return m_sliceView.slices[slice];
		}

		inline MipView & getSlice( size_t slice )
		{
			CU_Require( m_cubeView.layers.empty() );
			CU_Require( m_arrayView.layers.empty() );
			CU_Require( getDepth() > 1u );
			CU_Require( slice < m_sliceView.slices.size() );
			return m_sliceView.slices[slice];
		}

		inline TextureView const & getSliceView( size_t slice )const
		{
			CU_Require( getSlice( slice ).view );
			return *getSlice( slice ).view;
		}

		inline TextureView & getSliceView( size_t slice )
		{
			CU_Require( getSlice( slice ).view );
			return *getSlice( slice ).view;
		}

		inline ArrayView< CubeView > const & getArrayCube()const
		{
			return m_cubeView;
		}

		inline CubeView const & getLayerCube( size_t layer )const
		{
			CU_Require( m_arrayView.layers.empty() );
			CU_Require( m_sliceView.slices.empty() );
			CU_Require( isCube() );
			CU_Require( layer < m_cubeView.layers.size() );
			return m_cubeView.layers[layer];
		}

		inline CubeView & getLayerCube( size_t layer )
		{
			CU_Require( m_arrayView.layers.empty() );
			CU_Require( m_sliceView.slices.empty() );
			CU_Require( isCube() );
			CU_Require( layer < m_cubeView.layers.size() );
			return m_cubeView.layers[layer];
		}

		inline TextureView const & getLayerCubeView( size_t layer )const
		{
			CU_Require( getLayerCube( layer ).view.view );
			return *getLayerCube( layer ).view.view;
		}

		inline TextureView & getLayerCubeView( size_t layer )
		{
			CU_Require( getLayerCube( layer ).view.view );
			return *getLayerCube( layer ).view.view;
		}

		inline MipView const & getLayerCubeFace( size_t layer
			, CubeMapFace face )const
		{
			return getLayerCube( layer ).faces[size_t( face )];
		}

		inline MipView & getLayerCubeFace( size_t layer
			, CubeMapFace face )
		{
			return getLayerCube( layer ).faces[size_t( face )];
		}

		inline TextureView const & getLayerCubeFaceView( size_t layer
			, CubeMapFace face )const
		{
			CU_Require( getLayerCubeFace( layer, face ).view );
			return *getLayerCubeFace( layer, face ).view;
		}

		inline TextureView & getLayerCubeFaceView( size_t layer
			, CubeMapFace face )
		{
			CU_Require( getLayerCubeFace( layer, face ).view );
			return *getLayerCubeFace( layer, face ).view;
		}

		inline TextureView const & getLayerCubeFaceMipView( size_t layer
			, CubeMapFace face
			, uint32_t level )const
		{
			CU_Require( getLayerCubeFace( layer, face ).levels.size() > level );
			CU_Require( getLayerCubeFace( layer, face ).levels[level] );
			return *getLayerCubeFace( layer, face ).levels[level];
		}

		inline TextureView & getLayerCubeFaceMipView( size_t layer
			, CubeMapFace face
			, uint32_t level )
		{
			CU_Require( getLayerCubeFace( layer, face ).levels.size() > level );
			CU_Require( getLayerCubeFace( layer, face ).levels[level] );
			return *getLayerCubeFace( layer, face ).levels[level];
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

		inline uint32_t isCube()const
		{
			return getLayersCount() >= 6u
				&& ( getLayersCount() % 6u ) == 0u
				&& ashes::checkFlag( m_info->flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT );
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
		void doUpdateFromFirstImage( castor::ImageLayout const & layout );

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
		ashes::ImagePtr m_texture;
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
