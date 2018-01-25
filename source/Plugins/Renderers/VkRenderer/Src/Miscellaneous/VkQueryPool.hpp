/*
See LICENSE file in root folder
*/
#ifndef ___VkRenderer_QueryPool_H___
#define ___VkRenderer_QueryPool_H___

#include "VkRendererPrerequisites.hpp"

#include <Miscellaneous/QueryPool.hpp>

namespace vk_renderer
{
	/**
	*\~english
	*\brief
	*	GPU query pool implementation.
	*\~french
	*\brief
	*	Implémentation d'un pool de requêtes GPU.
	*/
	class QueryPool
		: public renderer::QueryPool
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical device.
		*\param[in] type
		*	The query type.
		*\param[in] count
		*	The number of queries managed by the pool.
		*\param[in] pipelineStatistics
		*	Specifies the counters to be returned in queries.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] type
		*	Le type de requête.
		*\param[in] count
		*	Le nombre de requêtes gérées par le pool.
		*\param[in] pipelineStatistics
		*	Définit les compteurs que les requêtes retournent.
		*/
		QueryPool( Device const & device
			, renderer::QueryType type
			, uint32_t count
			, renderer::QueryPipelineStatisticFlags pipelineStatistics );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		~QueryPool();
		/**
		*\copydoc	renderer::QueryPool:getResults
		*/
		void getResults( uint32_t firstQuery
			, uint32_t queryCount
			, uint32_t stride
			, renderer::QueryResultFlags flags
			, renderer::UInt32Array & data )const override;
		/**
		*\copydoc	renderer::QueryPool:getResults
		*/
		void getResults( uint32_t firstQuery
			, uint32_t queryCount
			, uint32_t stride
			, renderer::QueryResultFlags flags
			, renderer::UInt64Array & data )const override;
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkQueryPool.
		*\~english
		*\brief
		*	VkQueryPool implicit cast operator.
		*/
		inline operator VkQueryPool const &()const
		{
			return m_pool;
		}

	protected:
		Device const & m_device;
		VkQueryPool m_pool;
	};
}

#endif
