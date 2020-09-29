/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GBuffer_H___
#define ___C3D_GBuffer_H___

#include "RenderModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	class GBufferBase
		: public castor::Named
	{
	public:
		using Textures = std::vector< TextureUnit const * >;

	public:
		C3D_API explicit GBufferBase( castor::String name )
			: castor::Named{ std::move( name ) }
		{
		}

	protected:
		static C3D_API TextureUnit doCreateTexture( Engine & engine
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, uint32_t mipLevels
			, VkFormat format
			, VkImageUsageFlags usageFlags
			, VkBorderColor const & borderColor );
		/**
		*	2D or 2D array textures
		*/
		template< typename TextureEnumT >
		static TextureUnitArray doCreateTextures( Engine & engine
			, std::array< TextureUnit const *, size_t( TextureEnumT::eCount ) > const & inputs
			, castor::String const & prefix
			, VkImageCreateFlags createFlags
			, castor::Size const & size
			, uint32_t layerCount = 1u )
		{
			TextureUnitArray result;

			for ( uint32_t i = 0u; i < inputs.size(); ++i )
			{
				if ( !inputs[i] )
				{
					auto texture = TextureEnumT( i );
					result.emplace_back( doCreateTexture( engine
						, prefix + castor3d::getName( texture )
						, createFlags
						, { size.getWidth(), size.getHeight(), 1u }
						, layerCount
						, getMipLevels( texture, size )
						, getFormat( texture )
						, getUsageFlags( texture )
						, getBorderColor( texture ) ) );
				}
			}

			return result;
		}
		/**
		*	3D textures
		*/
		template< typename TextureEnumT >
		static TextureUnitArray doCreateTextures( Engine & engine
			, std::array< TextureUnit const *, size_t( TextureEnumT::eCount ) > const & inputs
			, castor::String const & prefix
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size )
		{
			TextureUnitArray result;

			for ( uint32_t i = 0u; i < inputs.size(); ++i )
			{
				if ( !inputs[i] )
				{
					auto texture = TextureEnumT( i );
					result.emplace_back( doCreateTexture( engine
						, prefix + castor3d::getName( texture )
						, createFlags
						, size
						, 1u
						, getMipLevels( texture, size )
						, getFormat( texture )
						, getUsageFlags( texture )
						, getBorderColor( texture ) ) );
				}
			}

			return result;
		}
	};

	template< typename TextureEnumT >
	class GBufferT
		: private GBufferBase
	{
	public:
		/**
		*\~english
		*\brief
		*	Initialises 2D g-buffer related stuff.
		*\param[in] engine
		*	The engine.
		*\param[in] textures
		*	The gbuffer's images.
		*\~french
		*\brief
		*	Initialise les données liées au g-buffer 2D.
		*\param[in] engine
		*	Le moteur.
		*\param[in] textures
		*	Les images du gbuffer.
		*/
		GBufferT( Engine & engine
			, castor::String name
			, std::array< TextureUnit const *, size_t( TextureEnumT::eCount ) > const & inputs
			, VkImageCreateFlags createFlags
			, castor::Size const & size
			, uint32_t layerCount = 1u )
			: GBufferBase{ std::move( name ) }
			, m_engine{ engine }
			, m_owned{ GBufferBase::doCreateTextures< TextureEnumT >( m_engine
				, inputs
				, getName()
				, createFlags
				, size
				, layerCount ) }
		{
			auto itOwned = m_owned.begin();

			for ( uint32_t i = 0; i < inputs.size(); ++i )
			{
				if ( inputs[i] )
				{
					m_result.push_back( inputs[i] );
				}
				else
				{
					m_result.push_back( &( *itOwned ) );
					++itOwned;
				}
			}
		}
		/**
		*\~english
		*\brief
		*	Initialises 3D g-buffer related stuff.
		*\param[in] engine
		*	The engine.
		*\param[in] textures
		*	The gbuffer's images.
		*\~french
		*\brief
		*	Initialise les données liées au g-buffer 3D.
		*\param[in] engine
		*	Le moteur.
		*\param[in] textures
		*	Les images du gbuffer.
		*/
		GBufferT( Engine & engine
			, castor::String name
			, std::array< TextureUnit const *, size_t( TextureEnumT::eCount ) > const & inputs
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size )
			: GBufferBase{ std::move( name ) }
			, m_engine{ engine }
			, m_owned{ GBufferBase::doCreateTextures< TextureEnumT >( m_engine
				, inputs
				, getName()
				, createFlags
				, size ) }
		{
			auto itOwned = m_owned.begin();

			for ( uint32_t i = 0; i < inputs.size(); ++i )
			{
				if ( inputs[i] )
				{
					m_result.push_back( inputs[i] );
				}
				else
				{
					m_result.push_back( &( *itOwned ) );
					++itOwned;
				}
			}
		}

		void initialise( RenderDevice const & device )
		{
			for ( auto & unit : m_owned )
			{
				unit.initialise( device );
			}
		}

		void cleanup()
		{
			for ( auto & unit : m_owned )
			{
				unit.cleanup();
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
		inline TextureUnit const & operator[]( TextureEnumT texture )const
		{
			return *m_result[size_t( texture )];
		}

		inline Textures::const_iterator cbegin()const
		{
			return m_result.begin();
		}

		inline Textures::const_iterator cend()const
		{
			return m_result.end();
		}

		inline Textures::const_iterator begin()const
		{
			return m_result.begin();
		}

		inline Textures::const_iterator end()const
		{
			return m_result.end();
		}

		inline Textures::iterator begin()
		{
			return m_result.begin();
		}

		inline Textures::iterator end()
		{
			return m_result.end();
		}
		/**@}*/

	protected:
		Engine & m_engine;
		TextureUnitArray m_owned;
		Textures m_result;
	};
}

#endif
