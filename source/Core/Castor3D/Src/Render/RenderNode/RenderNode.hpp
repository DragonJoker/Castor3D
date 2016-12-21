/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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

namespace Castor3D
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

	using SubmeshStaticRenderNodesByPassMap = TypeRenderNodesByPassMap< SubmeshStaticRenderNodesMap >;
	DECLARE_MAP( RenderPipelineRPtr, SubmeshStaticRenderNodesByPassMap, SubmeshStaticRenderNodesByPipeline );
}

#endif
