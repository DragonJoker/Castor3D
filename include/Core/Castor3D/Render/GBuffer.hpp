/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GBuffer_H___
#define ___C3D_GBuffer_H___

#include "RenderModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <RenderGraph/ImageData.hpp>
#include <RenderGraph/ImageViewData.hpp>

namespace c3d
{
	C3D_API VkFormatFeatureFlags getFeatureFlags( ImageUsageFlags flags );

	class GBufferBase
		: public Named
	{
	public:
		C3D_API explicit GBufferBase( RenderDevice const & device
			, String name );

	protected:
		C3D_API TextureUPtr doCreateTexture( crg::ResourcesCache & resources
			, String const & name
			, ImageCreateFlags createFlags
			, Extent3D const & size
			, uint32_t layerCount
			, SampleCount sampleCount
			, uint32_t mipLevels
			, PixelFormat format
			, ImageUsageFlags usageFlags
			, BorderColour borderColor
			, ComparisonFunc compareOp )const;
		/**
		*\~english
		*\brief
		*	Initialises 2D or 2D array g-buffer textures.
		*\param[in] resources
		*	The render graph resource handler.
		*\param[in] inputs
		*	The already existing images.
		*\param[in] prefix
		*	The buffer's textures name's prefix.
		*\param[in] createFlags
		*	The creation flags for images remaining to create.
		*\param[in] size
		*	The g-buffer's dimensions.
		*\param[in] layerCount
		*	The layers count for the g-buffer.
		*\param[in] sampleCount
		*	The images samples count.
		*\param[out] owned
		*	The images that are owned by the g-buffer.
		*\~french
		*\brief
		*	Initialise les textures du g-buffer 2D ou 2D array.
		*\param[in] resources
		*	Le gestionnaire de ressources du graphe.
		*\param[in] inputs
		*	Les images déjà existantes.
		*\param[in] prefix
		*	Le préfixe du nom des textures du g-buffer.
		*\param[in] createFlags
		*	Les flags de création pour les images restant à créer.
		*\param[in] size
		*	Les dimensions du g-buffer.
		*\param[in] layerCount
		*	Le nombre de layers du g-buffer.
		*\param[in] sampleCount
		*	Le nombre de samples des images.
		*\param[out] owned
		*	Les images dont le g-buffer a la responsabilité.
		*/
		template< typename TextureEnumT >
		Vector< Texture * > doCreateTextures( crg::ResourcesCache & resources
			, Array< Texture *, size_t( TextureEnumT::eCount ) > const & inputs
			, String const & prefix
			, ImageCreateFlags createFlags
			, Size const & size
			, uint32_t layerCount
			, SampleCount sampleCount
			, Vector< TextureUPtr > & owned )const
		{
			Vector< Texture * > result;

			for ( uint32_t i = 0u; i < inputs.size(); ++i )
			{
				if ( !inputs[i] )
				{
					auto texture = TextureEnumT( i );
					auto & back = *owned.emplace_back( doCreateTexture( resources
						, prefix + getTexName( texture )
						, createFlags
						, { size.getWidth(), size.getHeight(), 1u }
						, layerCount
						, sampleCount
						, getMipLevels( m_device, texture, size )
						, getFormat( m_device, texture )
						, getUsageFlags( texture )
						, getBorderColor( texture )
						, getCompareOp( texture ) ) );
					result.push_back( &back );
				}
				else
				{
					result.push_back( inputs[i] );
				}
			}

			return result;
		}
		/**
		*\~english
		*\brief
		*	Initialises 3D g-buffer textures.
		*\param[in] resources
		*	The render graph resource handler.
		*\param[in] inputs
		*	The already existing images.
		*\param[in] prefix
		*	The buffer's textures name's prefix.
		*\param[in] createFlags
		*	The creation flags for images remaining to create.
		*\param[in] size
		*	The g-buffer's dimensions.
		*\param[in] sampleCount
		*	The images samples count.
		*\param[out] owned
		*	The images that are owned by the g-buffer.
		*\~french
		*\brief
		*	Initialise les textures du g-buffer 3D.
		*\param[in] resources
		*	Le gestionnaire de ressources du graphe.
		*\param[in] inputs
		*	Les images déjà existantes.
		*\param[in] prefix
		*	Le préfixe du nom des textures du g-buffer.
		*\param[in] createFlags
		*	Les flags de création pour les images restant à créer.
		*\param[in] size
		*	Les dimensions du g-buffer.
		*\param[in] sampleCount
		*	Le nombre de samples des images.
		*\param[out] owned
		*	Les images dont le g-buffer a la responsabilité.
		*/
		template< typename TextureEnumT >
		Vector< Texture * > doCreateTextures( crg::ResourcesCache & resources
			, Array< Texture *, size_t( TextureEnumT::eCount ) > const & inputs
			, String const & prefix
			, ImageCreateFlags createFlags
			, Extent3D const & size
			, SampleCount sampleCount
			, Vector< TextureUPtr > & owned )const
		{
			Vector< Texture * > result;

			for ( uint32_t i = 0u; i < inputs.size(); ++i )
			{
				if ( !inputs[i] )
				{
					auto texture = TextureEnumT( i );
					auto & back = *owned.emplace_back( doCreateTexture( resources
						, prefix + getTexName( texture )
						, createFlags
						, size
						, 1u
						, sampleCount
						, getMipLevels( m_device, texture, size )
						, getFormat( m_device, texture )
						, getUsageFlags( texture )
						, getBorderColor( texture )
						, getCompareOp( texture ) ) );
					result.push_back( &back );
				}
				else
				{
					result.push_back( inputs[i] );
				}
			}

			return result;
		}

	protected:
		RenderDevice const & m_device;
	};

	template< typename TextureEnumT >
	class GBufferT
		: private GBufferBase
	{
	protected:
		~GBufferT()noexcept
		{
			for ( auto const & texture : m_owned )
			{
				texture->destroy();
			}
		}

	public:
		GBufferT( GBufferT const & rhs ) = delete;
		GBufferT & operator=( GBufferT const & rhs ) = delete;
		GBufferT( GBufferT && rhs )noexcept = default;
		GBufferT & operator=( GBufferT && rhs )noexcept = default;
		/**
		*\~english
		*\brief
		*	Initialises 2D or 2D-Array g-buffer related stuff.
		*\param[in] resources
		*	The render graph resource handler.
		*\param[in] device
		*	The GPU device.
		*\param[in] name
		*	The buffer name.
		*\param[in] inputs
		*	The already existing images.
		*\param[in] createFlags
		*	The creation flags for images remaining to create.
		*\param[in] size
		*	The g-buffer's dimensions.
		*\param[in] layerCount
		*	The layers count for the g-buffer.
		*\param[in] sampleCount
		*	The images samples count.
		*\~french
		*\brief
		*	Initialise les données liées au g-buffer 2D ou 2D-Array.
		*\param[in] resources
		*	Le gestionnaire de ressources du graphe.
		*\param[in] device
		*	Le device GPU.
		*\param[in] name
		*	Le nom du buffer.
		*\param[in] inputs
		*	Les images déjà existantes.
		*\param[in] createFlags
		*	Les flags de création pour les images restant à créer.
		*\param[in] size
		*	Les dimensions du g-buffer.
		*\param[in] layerCount
		*	Le nombre de layers du g-buffer.
		*\param[in] sampleCount
		*	Le nombre de samples des images.
		*/
		GBufferT( crg::ResourcesCache & resources
			, RenderDevice const & device
			, String name
			, Array< Texture *, size_t( TextureEnumT::eCount ) > const & inputs
			, ImageCreateFlags createFlags
			, Size const & size
			, uint32_t layerCount = 1u
			, SampleCount sampleCount = SampleCount::e1 )
			: GBufferBase{ device, c3d::move( name ) }
			, m_result{ doCreateTextures< TextureEnumT >( resources
				, inputs
				, getName()
				, createFlags
				, size
				, layerCount
				, sampleCount
				, m_owned ) }
		{
		}
		/**
		*\~english
		*\brief
		*	Initialises 3D g-buffer related stuff.
		*\param[in] resources
		*	The render graph resource handler.
		*\param[in] device
		*	The GPU device.
		*\param[in] name
		*	The buffer name.
		*\param[in] inputs
		*	The already existing images.
		*\param[in] createFlags
		*	The creation flags for images remaining to create.
		*\param[in] size
		*	The g-buffer's dimensions.
		*\param[in] sampleCount
		*	The images samples count.
		*\~french
		*\brief
		*	Initialise les données liées au g-buffer 3D.
		*\param[in] resources
		*	Le gestionnaire de ressources du graphe.
		*\param[in] device
		*	Le device GPU.
		*\param[in] name
		*	Le nom du buffer.
		*\param[in] inputs
		*	Les images déjà existantes.
		*\param[in] createFlags
		*	Les flags de création pour les images restant à créer.
		*\param[in] size
		*	Les dimensions du g-buffer.
		*\param[in] sampleCount
		*	Le nombre de samples des images.
		*/
		GBufferT( crg::ResourcesCache & resources
			, RenderDevice const & device
			, String name
			, Array< Texture *, size_t( TextureEnumT::eCount ) > const & inputs
			, ImageCreateFlags createFlags
			, Extent3D const & size
			, SampleCount sampleCount = SampleCount::e1 )
			: GBufferBase{ device, c3d::move( name ) }
			, m_result{ doCreateTextures< TextureEnumT >( resources
				, inputs
				, getName()
				, createFlags
				, size
				, sampleCount
				, m_owned ) }
		{
		}

		void create()const
		{
			for ( auto const & texture : m_owned )
			{
				texture->create();
			}
		}
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		auto cbegin()const noexcept
		{
			return m_result.begin();
		}

		auto cend()const noexcept
		{
			return m_result.end();
		}

		auto begin()const noexcept
		{
			return m_result.begin();
		}

		auto end()const noexcept
		{
			return m_result.end();
		}

		auto begin()noexcept
		{
			return m_result.begin();
		}

		auto end()noexcept
		{
			return m_result.end();
		}

		auto size()noexcept
		{
			return m_result.size();
		}

		Texture const & getTexture( TextureEnumT texture )const noexcept
		{
			return *m_result[size_t( texture )];
		}

		Extent3D const & getExtent()const noexcept
		{
			return m_result[0]->getExtent();
		}

		uint32_t getArrayLayers()const noexcept
		{
			return m_result[0]->imageId.data->info.arrayLayers;
		}

		crg::ImageId const & getImageId( TextureEnumT texture )const noexcept
		{
			return m_result[size_t( texture )]->imageId;
		}

		ashes::Sampler const & getSampler( TextureEnumT texture )const noexcept
		{
			return *m_result[size_t( texture )]->sampler;
		}

		crg::Attachment const * getSampledLastAttach( TextureEnumT texture, uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_result[size_t( texture )]->getSampledLastAttach( layerIndex, mipLevel );
		}

		crg::Attachment const * getSampledLastAttach( TextureEnumT texture, uint32_t layerIndex )const noexcept
		{
			return m_result[size_t( texture )]->getSampledLastAttach( layerIndex );
		}

		crg::Attachment const * getSampledLastAttach( TextureEnumT texture )const noexcept
		{
			return m_result[size_t( texture )]->getSampledLastAttach();
		}

		crg::ImageViewId const & getTargetViewId( TextureEnumT texture, uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_result[size_t( texture )]->getTargetViewId( layerIndex, mipLevel );
		}

		crg::ImageViewId const & getTargetViewId( TextureEnumT texture, uint32_t layerIndex )const noexcept
		{
			return m_result[size_t( texture )]->getTargetViewId( layerIndex );
		}

		crg::ImageViewId const & getTargetViewId( TextureEnumT texture )const noexcept
		{
			return m_result[size_t( texture )]->getTargetViewId();
		}

		crg::ImageViewId const & getWholeViewId( TextureEnumT texture, uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_result[size_t( texture )]->getWholeViewId( layerIndex, mipLevel );
		}

		crg::ImageViewId const & getWholeViewId( TextureEnumT texture, uint32_t layerIndex )const noexcept
		{
			return m_result[size_t( texture )]->getWholeViewId( layerIndex );
		}

		crg::ImageViewId const & getWholeViewId( TextureEnumT texture )const noexcept
		{
			return m_result[size_t( texture )]->getWholeViewId();
		}

		crg::ImageViewId const & getSampledViewId( TextureEnumT texture, uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_result[size_t( texture )]->getSampledViewId( layerIndex, mipLevel );
		}

		crg::ImageViewId const & getSampledViewId( TextureEnumT texture, uint32_t layerIndex )const noexcept
		{
			return m_result[size_t( texture )]->getSampledViewId( layerIndex );
		}

		crg::ImageViewId const & getSampledViewId( TextureEnumT texture )const noexcept
		{
			return m_result[size_t( texture )]->getSampledViewId();
		}

		VkImageView getTargetView( TextureEnumT texture, uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_result[size_t( texture )]->getTargetView( layerIndex, mipLevel );
		}

		VkImageView getTargetView( TextureEnumT texture, uint32_t layerIndex )const noexcept
		{
			return m_result[size_t( texture )]->getTargetView( layerIndex );
		}

		VkImageView getTargetView( TextureEnumT texture )const noexcept
		{
			return m_result[size_t( texture )]->getTargetView();
		}

		VkImageView getWholeView( TextureEnumT texture, uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_result[size_t( texture )]->getWholeView( layerIndex, mipLevel );
		}

		VkImageView getWholeView( TextureEnumT texture, uint32_t layerIndex )const noexcept
		{
			return m_result[size_t( texture )]->getWholeView( layerIndex );
		}

		VkImageView getWholeView( TextureEnumT texture )const noexcept
		{
			return m_result[size_t( texture )]->getWholeView();
		}

		VkImageView getSampledView( TextureEnumT texture, uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_result[size_t( texture )]->getSampledView( layerIndex, mipLevel );
		}

		VkImageView getSampledView( TextureEnumT texture, uint32_t layerIndex )const noexcept
		{
			return m_result[size_t( texture )]->getSampledView( layerIndex );
		}

		VkImageView getSampledView( TextureEnumT texture )const noexcept
		{
			return m_result[size_t( texture )]->getSampledView();
		}

		crg::Attachment const * getLastAttach( TextureEnumT texture, uint32_t layerIndex, uint32_t mipLevel )const noexcept
		{
			return m_result[size_t( texture )]->getLastAttach( layerIndex, mipLevel );
		}

		crg::Attachment const * getLastAttach( TextureEnumT texture, uint32_t layerIndex )const noexcept
		{
			return m_result[size_t( texture )]->getLastAttach( layerIndex );
		}

		crg::Attachment const * getLastAttach( TextureEnumT texture )const noexcept
		{
			return m_result[size_t( texture )]->getLastAttach();
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		crg::Attachment const * setLastAttach( TextureEnumT texture, uint32_t layerIndex, crg::Attachment const * attach )noexcept
		{
			return m_result[size_t( texture )]->setLastAttach( layerIndex, attach );
		}

		crg::Attachment const * setLastAttach( TextureEnumT texture, crg::Attachment const * attach )noexcept
		{
			return m_result[size_t( texture )]->setLastAttach( attach );
		}
		/**@}*/

	protected:
		Vector< TextureUPtr > m_owned;
		Vector< Texture * > m_result;
	};
}

#endif
