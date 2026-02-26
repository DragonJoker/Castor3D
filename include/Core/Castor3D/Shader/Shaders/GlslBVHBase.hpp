/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslBVHBase_H___
#define ___C3D_GlslBVHBase_H___

#include "GlslAABB.hpp"

#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace c3d::shader
{
	template< typename BVHContT >
	struct BVHBaseT
	{
		explicit BVHBaseT( sdw::ShaderWriter & writer
			, uint32_t nodeStackSize = 1024u )
			: m_writer{ writer }
			, m_nodeStackSize{ nodeStackSize }
			, m_numChildNodes{ writer.declConstantArray< sdw::UInt >( "c3d_numChildNodes"
				, { 1_u			/* 1 level   =32^0 */
				, 33_u			/* 2 levels  +32^1 */
				, 1057_u		/* 3 levels  +32^2 */
				, 33825_u		/* 4 levels  +32^3 */
				, 1082401_u		/* 5 levels  +32^4 */
				, 34636833_u	/* 6 levels  +32^5 */ } ) }
			, m_sharedNodeStack{ m_writer.declSharedVariable< sdw::UInt >( "gsNodeStack", m_nodeStackSize ) } // This should be enough to push 32 layers of nodes (32 nodes per layer).
			, m_sharedStackPtr{ m_writer.declSharedVariable< sdw::Int >( "gsStackPtr" ) } // The current index in the node stack.
			, m_sharedParentIndex{ m_writer.declSharedVariable< sdw::UInt >( "gsParentIndex" ) } // The index of the parent node in the BVH that is currently being processed.
		{
		}

		void resetTraversal()
		{
			m_sharedStackPtr = 0_u;
			m_sharedParentIndex = 0_u;
			// push the root node (at index 0) on the node stack.
			pushNode( 0_u );
		}

		sdw::Boolean hasParentNode()const
		{
			return m_sharedParentIndex > 0_u;
		}

		void popChild( sdw::UInt const & childOffset )
		{
			sdwIF( m_writer, childOffset == 0_u )
			{
				m_sharedParentIndex = doPopNode();
			}
			sdwFI
		}

		void pushNode( sdw::UInt const & pnodeIndex )const
		{
			if ( !m_pushNode )
				m_pushNode = m_writer.implementFunction< sdw::Void >( "c3d_pushNode"
					, [this]( sdw::UInt const & nodeIndex )
					{
						auto stackPtr = m_writer.declLocale( "stackPtr"
							, sdw::atomicAdd( m_sharedStackPtr, 1_i ) );

						sdwIF( m_writer, stackPtr < s32( m_nodeStackSize ) )
						{
							m_sharedNodeStack[stackPtr] = nodeIndex;
						}
						sdwFI
					}
					, sdw::InUInt{ m_writer, "nodeIndex" } );
			m_pushNode( pnodeIndex );
		}

		// Get the index of the first child node in the BVH.
		sdw::UInt getFirstChild( sdw::UInt const & numLevels )const
		{
			return m_writer.ternary( numLevels > 0_u
				, m_sharedParentIndex * 32_u + 1_u
				, 0_u );
		}

		// Check to see if an index of the BVH is a leaf.
		sdw::Boolean isLeafNode( sdw::UInt const & childIndex, sdw::UInt const & numLevels )const
		{
			return m_writer.ternary( numLevels > 0_u
				, childIndex > ( m_numChildNodes[numLevels - 1_u] - 1_u )
				, 1_b );
		}

		// Get the index of a leaf node given the node ID in the BVH.
		sdw::UInt getLeafIndex( sdw::UInt const & nodeIndex, sdw::UInt const & numLevels )const
		{
			return m_writer.ternary( numLevels > 0_u
				, nodeIndex - m_numChildNodes[numLevels - 1_u]
				, nodeIndex );
		}

	private:
		sdw::RetUInt doPopNode()const
		{
			if ( !m_popNode )
				m_popNode = m_writer.implementFunction< sdw::UInt >( "c3d_popNode"
					, [this]()
					{
						auto nodeIndex = m_writer.declLocale( "nodeIndex"
							, 0_u );
						auto stackPtr = m_writer.declLocale( "stackPtr"
							, sdw::atomicAdd( m_sharedStackPtr, -1_i ) );

						sdwIF( m_writer, stackPtr > 0 && stackPtr < s32( m_nodeStackSize ) )
						{
							nodeIndex = m_sharedNodeStack[stackPtr - 1];
						}
						sdwFI

						m_writer.returnStmt( nodeIndex );
					} );
			return m_popNode();
		}

	private:
		sdw::ShaderWriter & m_writer;
		u32 m_nodeStackSize;
		sdw::UIntArray m_numChildNodes;
		// Using a stack of node IDs to traverse the BVH was inspired by:
		// Source: https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-ii-tree-traversal-gpu/
		sdw::UIntArray m_sharedNodeStack;
		sdw::Int m_sharedStackPtr;
		sdw::UInt m_sharedParentIndex;
		mutable sdw::Function< sdw::Void, sdw::InUInt > m_pushNode;
		mutable sdw::Function< sdw::UInt > m_popNode;
	};
}

#endif
