/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PoolUniformBuffer_H___
#define ___C3D_PoolUniformBuffer_H___

#include "UniformBufferBase.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

namespace castor3d
{
	template< typename T >
	class PoolUniformBuffer
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
		inline PoolUniformBuffer( RenderSystem const & renderSystem
			, castor::ArrayView< T > data
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags flags
			, castor::String debugName
			, ashes::QueueShare sharingMode = {} )
			: UniformBufferBase
			{
				renderSystem,
				data.size(),
				sizeof( T ),
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
		inline T const & getData( VkDeviceSize index = 0 )const
		{
			return m_data[index];
		}
		/**
		*\~english
		*\return
		*	The N-th instance of the data.
		*\~french
		*\return
		*	La n-ème instance des données.
		*/
		inline T & getData( VkDeviceSize index = 0 )
		{
			return m_data[index];
		}
		/**
		*\~english
		*\return
		*	The data.
		*\~french
		*\return
		*	Les données.
		*/
		inline castor::ArrayView< T > const & getDatas()const
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
		inline castor::ArrayView< T > & getDatas()
		{
			return m_data;
		}

	private:
		castor::ArrayView< T > m_data;
	};

	template< typename T >
	inline PoolUniformBufferUPtr< T > makePoolUniformBuffer( RenderSystem const & renderSystem
		, castor::ArrayView< T > data
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, std::string name
		, ashes::QueueShare sharingMode = {} )
	{
		return std::make_unique< PoolUniformBuffer< T > >( renderSystem
			, std::move( data )
			, usage
			, flags
			, std::move( name )
			, std::move( sharingMode ) );
	}
}

#endif
