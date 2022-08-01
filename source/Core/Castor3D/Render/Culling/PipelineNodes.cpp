#include "Castor3D/Render/Culling/PipelineNodes.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Scene/BillboardList.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace pipnodes
	{
		template< typename DataT >
		static PipelineBaseHash getPipelineBaseHash( SubmeshFlags submeshFlags
			, ProgramFlags programFlags
			, PassFlags passFlags
			, uint32_t maxTexcoordSet
			, uint32_t texturesCount
			, TextureFlags texturesFlags
			, uint32_t passLayerIndex
			, VkDeviceSize morphTargetsOffset
			, DataT const * dataPtr )
		{
			remFlag( programFlags, ProgramFlag::eAllOptional );
			remFlag( passFlags, PassFlag::eAllOptional );
			constexpr auto maxSubmeshSize = castor::getBitSize( uint32_t( SubmeshFlag::eAllBase ) );
			constexpr auto maxProgramSize = castor::getBitSize( uint32_t( ProgramFlag::eAllBase ) );
			constexpr auto maxPassSize = castor::getBitSize( uint32_t( PassFlag::eAllBase ) );
			constexpr auto maxTexcoorSetSize = castor::getBitSize( MaxTextureCoordinatesSets );
			constexpr auto maxTextureSize = castor::getBitSize( uint32_t( TextureFlag::eAll ) );
			constexpr auto maxTexturesSize = castor::getBitSize( uint32_t( maxTextureSize ) );
			constexpr auto maxPassLayerSize = castor::getBitSize( MaxPassLayers );
			constexpr auto maxTargetOffsetSize = 64 - maxPassLayerSize;
			static_assert( 64 >= maxSubmeshSize + maxProgramSize + maxPassSize + maxTexcoorSetSize + maxTextureSize + maxTexturesSize );
			auto offset = 0u;
			PipelineBaseHash result{};
			result.lo = uint64_t( submeshFlags ) << offset;
			offset += maxSubmeshSize;
			result.lo |= uint64_t( programFlags ) << offset;
			offset += maxProgramSize;
			result.lo |= uint64_t( passFlags ) << offset;
			offset += maxPassSize;
			result.lo |= uint64_t( maxTexcoordSet ) << offset;
			offset += maxTexcoorSetSize;
			result.lo |= uint64_t( texturesCount ) << offset;
			offset += maxTexturesSize;
			result.lo |= uint64_t( texturesFlags ) << offset;
			CU_Require( passLayerIndex < MaxPassLayers );
			CU_Require( ( morphTargetsOffset >> maxTargetOffsetSize ) == 0 );
			auto hash = size_t( morphTargetsOffset );

			if constexpr ( std::is_same_v< DataT, Submesh > )
			{
				castor::hashCombinePtr( hash, *dataPtr );
				hash >>= maxPassLayerSize;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) )
			{
				// When pass masks component is present, only consider first pass,
				// since blending will occur in shader.
				passLayerIndex = 0u;
			}

			result.hi = uint64_t( hash )
				| ( uint64_t( passLayerIndex ) << maxTargetOffsetSize );
			return result;
		}
	}

	//*********************************************************************************************

	PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, Submesh const & data
		, Pass const & pass
		, bool isFrontCulled )
	{
		auto submeshFlags = data.getSubmeshFlags( &pass );
		auto programFlags = data.getProgramFlags( *pass.getOwner() );

		if ( isFrontCulled )
		{
			addFlag( programFlags, ProgramFlag::eInvertNormals );
		}

		return pipnodes::getPipelineBaseHash( renderPass.adjustFlags( submeshFlags )
			, renderPass.adjustFlags( programFlags )
			, renderPass.adjustFlags( pass.getPassFlags() )
			, pass.getMaxTexCoordSet()
			, pass.getTextureUnitsCount()
			, pass.getTextures()
			, pass.getIndex()
			, data.getMorphTargets().getOffset()
			, &data );
	}

	PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, BillboardBase const & data
		, Pass const & pass
		, bool isFrontCulled )
	{
		auto submeshFlags = data.getSubmeshFlags();
		auto programFlags = data.getProgramFlags();

		if ( isFrontCulled )
		{
			addFlag( programFlags, ProgramFlag::eInvertNormals );
		}

		return pipnodes::getPipelineBaseHash( renderPass.adjustFlags( submeshFlags )
			, renderPass.adjustFlags( programFlags )
			, renderPass.adjustFlags( pass.getPassFlags() )
			, pass.getMaxTexCoordSet()
			, pass.getTextureUnitsCount()
			, pass.getTextures()
			, pass.getIndex()
			, 0u
			, &data );
	}

	//*********************************************************************************************

	void registerPipelineNodes( PipelineBaseHash hash
		, ashes::BufferBase const & buffer
		, std::vector< PipelineBuffer > & nodesIds )
	{
		auto it = std::find_if( nodesIds.begin()
			, nodesIds.end()
			, [&hash, &buffer]( PipelineBuffer const & lookup )
			{
				return lookup.first == hash
					&& lookup.second == &buffer;
			} );

		if ( it == nodesIds.end() )
		{
			nodesIds.emplace_back( hash, &buffer );
		}
	}

	uint32_t getPipelineNodeIndex( PipelineBaseHash hash
		, ashes::BufferBase const & buffer
		, std::vector< PipelineBuffer > const & cont )
	{
		auto it = std::find_if( cont.begin()
			, cont.end()
			, [&hash, &buffer]( PipelineBuffer const & lookup )
			{
				return lookup.first == hash
					&& lookup.second == &buffer;
			} );
		CU_Require( it != cont.end() );
		return uint32_t( std::distance( cont.begin(), it ) );
	}

	PipelineNodes & getPipelineNodes( PipelineBaseHash hash
		, ashes::BufferBase const & buffer
		, std::vector< PipelineBuffer > const & cont
		, PipelineNodes * nodes
		, VkDeviceSize maxNodesCount )
	{
		auto index = getPipelineNodeIndex( hash, buffer, cont );
		CU_Require( index < maxNodesCount );
		return nodes[index];
	}

	//*********************************************************************************************
}
