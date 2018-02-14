/*
See LICENSE file in root folder
*/
#ifndef ___Renderer_QueryPool_H___
#define ___Renderer_QueryPool_H___

#include "RendererPrerequisites.hpp"

namespace renderer
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
	{
	protected:
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
			, QueryType type
			, uint32_t count
			, QueryPipelineStatisticFlags pipelineStatistics );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~QueryPool() = default;
		/**
		*\~english
		*\brief
		*	Rerieves the status or result of a set of queries.
		*\param[in] firstQuery
		*	The first query index.
		*\param[in] queryCount
		*	The number of queries (starting at \p firstQuery).
		*\param[in] stride
		*	The stride in bytes between results for individual queries, in \p data.
		*\param[in] flags
		*	Specifies how and when results are returned.
		*\param[out] data
		*	Receives the results.
		*\~french
		*\brief
		*	Récupère le statut ou le résultat d'un ensemble de requêtes.
		*\param[in] firstQuery
		*	L'index de la première requête.
		*\param[in] queryCount
		*	Le nombre de requêtes (à partir de \p firstQuery).
		*\param[in] stride
		*	L'intervalle en octets entre les résultats pour chaque requête, dans \p data.
		*\param[in] flags
		*	Définit quand et comment les résultats sont récupérés.
		*\param[out] data
		*	Reçoit les résultats.
		*/
		virtual void getResults( uint32_t firstQuery
			, uint32_t queryCount
			, uint32_t stride
			, QueryResultFlags flags
			, UInt32Array & data )const = 0;
		/**
		*\~english
		*\brief
		*	Rerieves the status or result of a set of queries.
		*\param[in] firstQuery
		*	The first query index.
		*\param[in] queryCount
		*	The number of queries (starting at \p firstQuery).
		*\param[in] stride
		*	The stride in bytes between results for individual queries, in \p data.
		*\param[in] flags
		*	Specifies how and when results are returned.
		*\param[out] data
		*	Receives the results.
		*\~french
		*\brief
		*	Récupère le statut ou le résultat d'un ensemble de requêtes.
		*\param[in] firstQuery
		*	L'index de la première requête.
		*\param[in] queryCount
		*	Le nombre de requêtes (à partir de \p firstQuery).
		*\param[in] stride
		*	L'intervalle en octets entre les résultats pour chaque requête, dans \p data.
		*\param[in] flags
		*	Définit quand et comment les résultats sont récupérés.
		*\param[out] data
		*	Reçoit les résultats.
		*/
		virtual void getResults( uint32_t firstQuery
			, uint32_t queryCount
			, uint32_t stride
			, QueryResultFlags flags
			, UInt64Array & data )const = 0;
		/**
		*\~english
		*\return
		*	The query type.
		*\~french
		*\return
		*	Le type de requête.
		*/
		inline QueryType getType()const
		{
			return m_type;
		}
		/**
		*\~english
		*\return
		*	The number of queries in the pool.
		*\~french
		*\return
		*	Le nombre de requêtes dans le pool.
		*/
		inline uint32_t getCount()const
		{
			return m_count;
		}

	protected:
		Device const & m_device;
		QueryType m_type;
		uint32_t m_count;
		QueryPipelineStatisticFlags m_pipelineStatistics;
	};
}

#endif
