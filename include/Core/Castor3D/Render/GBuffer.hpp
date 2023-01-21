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
		C3D_API TexturePtr doCreateTexture( crg::ResourcesCache & resources
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, uint32_t mipLevels
			, VkFormat format
			, VkImageUsageFlags usageFlags
			, VkBorderColor const & borderColor )const;
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
		*/
		template< typename TextureEnumT >
		TextureArray doCreateTextures( crg::ResourcesCache & resources
			, std::array< TexturePtr, size_t( TextureEnumT::eCount ) > const & inputs
			, castor::String const & prefix
			, VkImageCreateFlags createFlags
			, castor::Size const & size
			, uint32_t layerCount = 1u )const
		{
			TextureArray result;

			for ( uint32_t i = 0u; i < inputs.size(); ++i )
			{
				if ( !inputs[i] )
				{
					auto texture = TextureEnumT( i );
					result.emplace_back( doCreateTexture( resources
						, prefix + getTexName( texture )
						, createFlags
						, { size.getWidth(), size.getHeight(), 1u }
						, layerCount
						, getMipLevels( m_device, texture, size )
						, getFormat( m_device, texture )
						, getUsageFlags( texture )
						, getBorderColor( texture ) ) );
				}
				else
				{
					result.emplace_back( inputs[i] );
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
		*/
		template< typename TextureEnumT >
		TextureArray doCreateTextures( crg::ResourcesCache & resources
			, std::array< TexturePtr, size_t( TextureEnumT::eCount ) > const & inputs
			, castor::String const & prefix
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size )const
		{
			TextureArray result;

			for ( uint32_t i = 0u; i < inputs.size(); ++i )
			{
				if ( !inputs[i] )
				{
					auto texture = TextureEnumT( i );
					result.emplace_back( doCreateTexture( resources
						, prefix + getTexName( texture )
						, createFlags
						, size
						, 1u
						, getMipLevels( m_device, texture, size )
						, getFormat( m_device, texture )
						, getUsageFlags( texture )
						, getBorderColor( texture ) ) );
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
		GBufferT( GBufferT && rhs ) = default;
		GBufferT & operator=( GBufferT && rhs ) = default;
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
		*/
		GBufferT( crg::ResourcesCache & resources
			, RenderDevice const & device
			, castor::String name
			, std::array< TexturePtr, size_t( TextureEnumT::eCount ) > const & inputs
			, VkImageCreateFlags createFlags
			, castor::Size const & size
			, uint32_t layerCount = 1u )
			: GBufferBase{ device, std::move( name ) }
			, m_result{ doCreateTextures< TextureEnumT >( resources
				, inputs
				, getName()
				, createFlags
				, size
				, layerCount ) }
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
		*/
		GBufferT( crg::ResourcesCache & resources
			, RenderDevice const & device
			, castor::String name
			, std::array< TexturePtr, size_t( TextureEnumT::eCount ) > const & inputs
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size )
			: GBufferBase{ device, std::move( name ) }
			, m_result{ doCreateTextures< TextureEnumT >( resources
				, inputs
				, getName()
				, createFlags
				, size ) }
		{
		}

		~GBufferT()
		{
			for ( auto & texture : m_result )
			{
				texture->destroy();
			}
		}

		void create()
		{
			for ( auto & texture : m_result )
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
		Texture const & operator[]( TextureEnumT texture )const
		{
			return *m_result[size_t( texture )];
		}

		TextureArray::const_iterator cbegin()const
		{
			return m_result.begin();
		}

		TextureArray::const_iterator cend()const
		{
			return m_result.end();
		}

		TextureArray::const_iterator begin()const
		{
			return m_result.begin();
		}

		TextureArray::const_iterator end()const
		{
			return m_result.end();
		}

		TextureArray::iterator begin()
		{
			return m_result.begin();
		}

		TextureArray::iterator end()
		{
			return m_result.end();
		}
		/**@}*/

	protected:
		TextureArray m_result;
	};
}

#endif
