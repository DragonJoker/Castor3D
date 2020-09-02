/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PoolUniformBuffer_H___
#define ___C3D_PoolUniformBuffer_H___

#include "UniformBufferBase.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

namespace castor3d
{
	template< typename DataT >
	class PoolUniformBufferT
		: public UniformBufferBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	count			The elements count.
		 *\param[in]	usage			The buffer usage flags.
		 *\param[in]	flags			The buffer memory flags.
		 *\param[in]	debugName		The buffer debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	count			Le nombre d'éléments.
		 *\param[in]	usage			Les indicateurs d'utilisation du tampon.
		 *\param[in]	flags			Les indicateurs de mémoire du tampon.
		 *\param[in]	debugName		Le nom debug du tampon.
		 */
		inline PoolUniformBufferT( RenderSystem const & renderSystem
			, castor::ArrayView< uint8_t > data
			, uint32_t count
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags flags
			, castor::String debugName
			, ashes::QueueShare sharingMode = {} )
			: UniformBufferBase
			{
				renderSystem,
				count,
				sizeof( DataT ),
				usage,
				flags,
				std::move( debugName ),
				std::move( sharingMode ),
			}
			, m_data{ std::move( data ) }
		{
		}
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\~french
		*\return
		*	La n-ème instance des données.
		*/
		inline DataT const & getData( VkDeviceSize index = 0 )const
		{
			return *reinterpret_cast< DataT const * >( m_data.data() + ( index * getAlignedSize() ) );
		}
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\~french
		*\return
		*	La n-ème instance des données.
		*/
		inline DataT & getData( VkDeviceSize index = 0 )
		{
			return *reinterpret_cast< DataT * >( m_data.data() + ( index * getAlignedSize() ) );
		}
		/**
		*\~english
		*\return
		*	The data.
		*\~french
		*\return
		*	Les données.
		*/
		inline castor::ArrayView< uint8_t > const & getDatas()const
		{
			return m_data;
		}
		/**
		*\~english
		*\return
		*	The data.
		*\~french
		*\return
		*	Les données.
		*/
		inline castor::ArrayView< uint8_t > & getDatas()
		{
			return m_data;
		}

	private:
		castor::ArrayView< uint8_t > m_data;
	};

	template< typename DataT >
	inline PoolUniformBufferUPtrT< DataT > makePoolUniformBuffer( RenderSystem const & renderSystem
		, castor::ArrayView< uint8_t > data
		, uint32_t count
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, std::string name
		, ashes::QueueShare sharingMode = {} )
	{
		return std::make_unique< PoolUniformBufferT< DataT > >( renderSystem
			, std::move( data )
			, count
			, usage
			, flags
			, std::move( name )
			, std::move( sharingMode ) );
	}
}

#endif
