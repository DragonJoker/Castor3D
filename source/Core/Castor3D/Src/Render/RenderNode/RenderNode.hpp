/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderNode_H___
#define ___C3D_RenderNode_H___

#include "BillboardRenderNode.hpp"
#include "DistanceRenderNode.hpp"
#include "MorphingRenderNode.hpp"
#include "ObjectRenderNode.hpp"
#include "PassRenderNode.hpp"
#include "SceneRenderNode.hpp"
#include "SkinningRenderNode.hpp"
#include "StaticRenderNode.hpp"

namespace castor3d
{
	template< typename T >
	struct TypeRenderNodesByPassMap
	{
	public:
		using pass_ptr = PassRPtr;
		using key_type = typename std::map< pass_ptr, T >::key_type;
		using mapped_type = typename std::map< pass_ptr, T >::mapped_type;
		using value_type = typename std::map< pass_ptr, T >::value_type;

		inline auto begin()const
		{
			return m_map.begin();
		}

		inline auto begin()
		{
			return m_map.begin();
		}

		inline auto end()const
		{
			return m_map.end();
		}

		inline auto end()
		{
			return m_map.end();
		}

		inline auto size()const
		{
			return m_map.size();
		}

		inline auto find( key_type p_pass )const
		{
			return m_map.find( p_pass );
		}

		inline auto insert( std::pair< key_type, mapped_type > p_pair )
		{
			return m_map.insert( p_pair );
		}

		inline auto emplace( key_type && p_key, mapped_type && p_value )
		{
			return m_map.emplace( std::move( p_key ), std::move( p_value ) );
		}

	private:
		std::map< key_type, mapped_type > m_map;
	};

	DECLARE_MULTIMAP( double, StaticRenderNode, StaticRenderNodeByDistance );
	DECLARE_MULTIMAP( double, SkinningRenderNode, SkinningRenderNodeByDistance );
	DECLARE_MULTIMAP( double, MorphingRenderNode, MorphingRenderNodeByDistance );
	DECLARE_MULTIMAP( double, BillboardRenderNode, BillboardRenderNodeByDistance );
	DECLARE_VECTOR( StaticRenderNode, StaticRenderNode );
	DECLARE_VECTOR( SkinningRenderNode, SkinningRenderNode );
	DECLARE_VECTOR( MorphingRenderNode, MorphingRenderNode );
	DECLARE_VECTOR( BillboardRenderNode, BillboardRenderNode );
	DECLARE_MAP( RenderPipelineRPtr, StaticRenderNodeArray, StaticRenderNodesByPipeline );
	DECLARE_MAP( RenderPipelineRPtr, SkinningRenderNodeArray, SkinningRenderNodesByPipeline );
	DECLARE_MAP( RenderPipelineRPtr, MorphingRenderNodeArray, MorphingRenderNodesByPipeline );
	DECLARE_MAP( RenderPipelineRPtr, BillboardRenderNodeArray, BillboardRenderNodesByPipeline );
	DECLARE_MAP( SubmeshRPtr, StaticRenderNodeArray, SubmeshStaticRenderNodes );
	DECLARE_MAP( SubmeshRPtr, SkinningRenderNodeArray, SubmeshSkinningRenderNodes );

	using SubmeshStaticRenderNodesByPassMap = TypeRenderNodesByPassMap< SubmeshStaticRenderNodesMap >;
	DECLARE_MAP( RenderPipelineRPtr, SubmeshStaticRenderNodesByPassMap, SubmeshStaticRenderNodesByPipeline );
	using SubmeshSkinninRenderNodesByPassMap = TypeRenderNodesByPassMap< SubmeshSkinningRenderNodesMap >;
	DECLARE_MAP( RenderPipelineRPtr, SubmeshSkinninRenderNodesByPassMap, SubmeshSkinningRenderNodesByPipeline );
}

#endif
