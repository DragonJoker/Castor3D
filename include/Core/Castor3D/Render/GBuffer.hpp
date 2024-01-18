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

namespace castor3d
{
	C3D_API VkFormatFeatureFlags getFeatureFlags( VkImageUsageFlags flags );

	class GBufferBase
		: public castor::Named
	{
	public:
		C3D_API explicit GBufferBase( RenderDevice const & device
			, castor::String name );

	protected:
		C3D_API TextureUPtr doCreateTexture( crg::ResourcesCache & resources
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, VkSampleCountFlagBits sampleCount
			, uint32_t mipLevels
			, VkFormat format
			, VkImageUsageFlags usageFlags
			, VkBorderColor const & borderColor
			, VkCompareOp compareOp )const;
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
		*/
		template< typename TextureEnumT >
		std::vector< Texture const * > doCreateTextures( crg::ResourcesCache & resources
			, std::array< Texture const *, size_t( TextureEnumT::eCount ) > const & inputs
			, castor::String const & prefix
			, VkImageCreateFlags createFlags
			, castor::Size const & size
			, uint32_t layerCount
			, VkSampleCountFlagBits sampleCount
			, std::vector< TextureUPtr > & owned )const
		{
			std::vector< Texture const * > result;

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
		*/
		template< typename TextureEnumT >
		std::vector< Texture const * > doCreateTextures( crg::ResourcesCache & resources
			, std::array< Texture const *, size_t( TextureEnumT::eCount ) > const & inputs
			, castor::String const & prefix
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, VkSampleCountFlagBits sampleCount
			, std::vector< TextureUPtr > & owned )const
		{
			std::vector< Texture const * > result;

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
			, castor::String name
			, std::array< Texture const *, size_t( TextureEnumT::eCount ) > const & inputs
			, VkImageCreateFlags createFlags
			, castor::Size const & size
			, uint32_t layerCount = 1u
			, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT )
			: GBufferBase{ device, std::move( name ) }
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
			, castor::String name
			, std::array< Texture const *, size_t( TextureEnumT::eCount ) > const & inputs
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT )
			: GBufferBase{ device, std::move( name ) }
			, m_result{ doCreateTextures< TextureEnumT >( resources
				, inputs
				, getName()
				, createFlags
				, size
				, sampleCount
				, m_owned ) }
		{
		}

		~GBufferT()noexcept
		{
			for ( auto const & texture : m_owned )
			{
				texture->destroy();
			}
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
		Texture const & operator[]( TextureEnumT texture )const noexcept
		{
			return *m_result[size_t( texture )];
		}

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
		/**@}*/

	protected:
		std::vector< TextureUPtr > m_owned;
		std::vector< Texture const * > m_result;
	};
}

#endif
