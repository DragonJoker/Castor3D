#include "Castor3D/Render/PipelineFlags.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace pipflags
	{
		static bool hasMatchingFlag( SubmeshFlag submeshFlag
			, ShaderFlags const & shaderFlags )
		{
			switch ( submeshFlag )
			{
			case castor3d::SubmeshFlag::eNormals:
				return checkFlag( shaderFlags, ShaderFlag::eNormal );
			case castor3d::SubmeshFlag::eTangents:
				return checkFlag( shaderFlags, ShaderFlag::eTangent );
			case castor3d::SubmeshFlag::eVelocity:
				return checkFlag( shaderFlags, ShaderFlag::eVelocity );
			case castor3d::SubmeshFlag::eColours:
				return checkFlag( shaderFlags, ShaderFlag::eColour );
			default:
				return true;
			}
		}

		static PipelineHiHashDetails getHiHashDetails( uint64_t hiHash
			, PassComponentsBitset components
			, ComponentsTextures textures
			, ShaderFlags shaderFlags )
		{
			constexpr auto maxSubmeshSize = castor::getBitSize( uint32_t( SubmeshFlag::eAllBase ) );
			constexpr auto maxSubmeshMask = ( 0x1u << uint64_t( maxSubmeshSize ) ) - 1u;
			constexpr auto maxProgramSize = castor::getBitSize( uint32_t( ProgramFlag::eAllBase ) );
			constexpr auto maxProgramMask = ( 0x1u << uint64_t( maxProgramSize ) ) - 1u;
			constexpr auto maxPassIDSize = castor::getBitSize( MaxPassTypes );
			constexpr auto maxPassIDMask = ( 0x1u << uint64_t( maxPassIDSize ) ) - 1u;
			constexpr auto maxPassSize = castor::getBitSize( uint32_t( PassFlag::eAllBase ) );
			constexpr auto maxPassMask = ( 0x1u << uint64_t( maxPassSize ) ) - 1u;
			constexpr auto maxCompareOpSize = castor::getBitSize( uint32_t( VK_COMPARE_OP_ALWAYS + 1 ) );
			constexpr auto maxCompareOpMask = ( 0x1u << uint64_t( maxCompareOpSize ) ) - 1u;
			constexpr auto maxPassLayerSize = castor::getBitSize( MaxPassLayers );
			constexpr auto maxPassLayerMask = ( 0x1u << uint64_t( maxPassLayerSize ) ) - 1u;
			constexpr auto maxSize = maxSubmeshSize + maxProgramSize + maxPassIDSize + maxPassSize + maxCompareOpSize + maxPassLayerSize;
			static_assert( 64 >= maxSize );

			PipelineHiHashDetails result{ {}, 0u };
			result.m_shaderFlags = shaderFlags;
			result.components = std::move( components );
			result.textures = std::move( textures );

			auto offset = 0u;
			result.m_submeshFlags = SubmeshFlags( ( hiHash >> offset ) & maxSubmeshMask );
			offset += maxSubmeshSize;
			result.m_programFlags = ProgramFlags( ( hiHash >> offset ) & maxProgramMask );
			offset += maxProgramSize;
			result.passType = PassFlags( ( hiHash >> offset ) & maxPassIDMask );
			offset += maxPassIDSize;
			result.m_passFlags = PassFlags( ( hiHash >> offset ) & maxPassMask );
			offset += maxPassSize;
			result.alphaFunc = VkCompareOp( ( hiHash >> offset ) & maxCompareOpMask );
			offset += maxCompareOpSize;
			result.passLayerIndex = uint32_t( ( hiHash >> offset ) & maxPassLayerMask );

			CU_Require( result.passType != 0 );
			return result;
		}

		static uint64_t getComponentsHash( PassComponentsBitset const & components )
		{
			uint64_t result{ std::hash< bool >{}( components[0] ) };

			for ( uint32_t i = 1u; i < components.getSize(); ++i )
			{
				castor::hashCombine64( result, components[i] );
			}

			return result;
		}

		static uint64_t getComponentsTexturesHash( ComponentsTextures const & textures )
		{
			uint64_t result{ std::hash< uint8_t >{}( uint8_t( textures[0] ) ) };

			for ( uint32_t i = 1u; i < textures.size(); ++i )
			{
				castor::hashCombine64( result, textures[i] );
			}

			return result;
		}

		static uint64_t getHiHash( PipelineHiHashDetails & flags )
		{
			constexpr auto maxSubmeshSize = castor::getBitSize( uint32_t( SubmeshFlag::eAllBase ) );
			constexpr auto maxProgramSize = castor::getBitSize( uint32_t( ProgramFlag::eAllBase ) );
			constexpr auto maxPassIDSize = castor::getBitSize( MaxPassTypes );
			constexpr auto maxPassSize = castor::getBitSize( uint32_t( PassFlag::eAllBase ) );
			constexpr auto maxCompareOpSize = castor::getBitSize( uint32_t( VK_COMPARE_OP_ALWAYS + 1 ) );
			constexpr auto maxPassLayerSize = castor::getBitSize( MaxPassLayers );
			constexpr auto maxSize = maxSubmeshSize + maxProgramSize + maxPassIDSize + maxPassSize + maxCompareOpSize + maxPassLayerSize;
			static_assert( 64 >= maxSize );
			CU_Require( flags.passType != 0 );
			CU_Require( flags.passLayerIndex < MaxPassLayers );

			auto programFlags = flags.m_programFlags;
			remFlag( programFlags, ProgramFlag::eAllOptional );

			auto offset = 0u;
			uint64_t result{};
			result = uint64_t( flags.m_submeshFlags ) << offset;
			offset += maxSubmeshSize;
			result |= uint64_t( programFlags ) << offset;
			offset += maxProgramSize;
			result |= uint64_t( flags.passType ) << offset;
			offset += maxPassIDSize;
			result |= uint64_t( flags.m_passFlags ) << offset;
			offset += maxPassSize;
			result |= uint64_t( flags.alphaFunc ) << offset;
			offset += maxCompareOpSize;
			result |= uint64_t( flags.passLayerIndex ) << offset;

#if !defined( NDEBUG )
			auto details = getHiHashDetails( result
				, flags.components
				, flags.textures
				, flags.m_shaderFlags );
			CU_Require( flags.m_shaderFlags == details.m_shaderFlags );
			CU_Require( flags.m_submeshFlags == details.m_submeshFlags );
			CU_Require( programFlags == details.m_programFlags );
			CU_Require( flags.passType == details.passType );
			CU_Require( flags.m_passFlags == details.m_passFlags );
			CU_Require( flags.alphaFunc == details.alphaFunc );
			CU_Require( flags.passLayerIndex == details.passLayerIndex );
#endif
			return result;
		}

#if !defined( NDEBUG )
		static PipelineLoHashDetails getLoHashDetails( uint64_t hash )
		{
			static constexpr auto maxTargetOffsetSize = 64;
			static constexpr auto maxSize = maxTargetOffsetSize;
			constexpr auto maxMorphTargetOffsetMask = maxTargetOffsetSize == maxSize
				? 0xFFFFFFFFFFFFFFFFull
				: ( 0x1ull << uint64_t( std::min( maxTargetOffsetSize, maxSize - 1 ) ) ) - 1ull;
			static_assert( 64 >= maxSize );

			PipelineLoHashDetails result{};
			auto offset = 0u;
			result.morphTargetsOffset = VkDeviceSize( ( hash >> offset ) & maxMorphTargetOffsetMask );

			return result;
		}
#endif

		static uint64_t getLoHash( PipelineLoHashDetails const & flags )
		{
			static constexpr auto maxTargetOffsetSize = 64;
			static constexpr auto maxSize = maxTargetOffsetSize;
			static_assert( 64 >= maxSize );
			CU_Require( maxTargetOffsetSize == maxSize
				|| ( flags.morphTargetsOffset >> std::min( maxTargetOffsetSize, maxSize - 1 ) ) == 0 );

			uint64_t result{};
			auto offset = 0u;
			result = uint64_t( flags.morphTargetsOffset ) << offset;

#if !defined( NDEBUG )
			auto details = getLoHashDetails( result );
			CU_Require( flags.morphTargetsOffset == details.morphTargetsOffset );
#endif
			return result;
		}

		static PipelineBaseHash getPipelineBaseHash( PipelineFlags flags )
		{
			PipelineBaseHash result{};
			result.hi = getHiHash( flags );
			result.lo = getLoHash( flags );
			result.components = flags.components;
			result.textures = flags.textures;
			return result;
		}

		static bool isTexcoordComponent( SubmeshFlag submeshData )
		{
			return submeshData == SubmeshFlag::eTexcoords0
				|| submeshData == SubmeshFlag::eTexcoords1
				|| submeshData == SubmeshFlag::eTexcoords2
				|| submeshData == SubmeshFlag::eTexcoords3;
		}
	}

	//*********************************************************************************************

	bool operator<( PipelineBaseHash const & lhs
		, PipelineBaseHash const & rhs )
	{
		return lhs.hi < rhs.hi
			|| ( ( lhs.hi == rhs.hi )
				&& ( ( lhs.lo < rhs.lo )
					|| ( ( lhs.lo == rhs.lo )
						&& ( ( pipflags::getComponentsHash( lhs.components ) < pipflags::getComponentsHash( rhs.components ) )
							|| ( ( pipflags::getComponentsHash( lhs.components ) == pipflags::getComponentsHash( rhs.components ) )
								&& ( pipflags::getComponentsTexturesHash( lhs.textures ) < pipflags::getComponentsTexturesHash( lhs.textures ) ) ) ) ) ) );
	}

	//*********************************************************************************************

	bool operator==( PipelineHiHashDetails const & lhs, PipelineHiHashDetails const & rhs )
	{
		return lhs.components == rhs.components
			&& lhs.textures == rhs.textures
			&& lhs.passType == rhs.passType
			&& lhs.alphaFunc == rhs.alphaFunc
			&& lhs.passLayerIndex == rhs.passLayerIndex
			&& lhs.m_passFlags == rhs.m_passFlags
			&& lhs.m_submeshFlags == rhs.m_submeshFlags
			&& lhs.m_programFlags == rhs.m_programFlags
			&& lhs.m_shaderFlags == rhs.m_shaderFlags;
	}

	//*********************************************************************************************

	bool operator==( PipelineLoHashDetails const & lhs, PipelineLoHashDetails const & rhs )
	{
		return lhs.morphTargetsOffset == rhs.morphTargetsOffset;
	}

	//*********************************************************************************************

	bool PipelineFlags::enableTexcoords()const
	{
		return forceTexCoords()
			|| ( !areFlagsEmpty( textures ) && ( hasAny( m_submeshFlags, SubmeshFlag::eTexcoords ) ) );
	}

	bool PipelineFlags::enableVertexInput( SubmeshFlag data )const
	{
		return pipflags::isTexcoordComponent( data )
			? enableTexcoord( data )
			: enableNonTexcoord( data );
	}

	bool PipelineFlags::enableIndices()const
	{
		return enableVertexInput( SubmeshFlag::eIndex );
	}

	bool PipelineFlags::enablePosition()const
	{
		return enableVertexInput( SubmeshFlag::ePositions );
	}

	bool PipelineFlags::enableNormal()const
	{
		return enableVertexInput( SubmeshFlag::eNormals );
	}

	bool PipelineFlags::enableTangentSpace()const
	{
		return enableVertexInput( SubmeshFlag::eTangents );
	}

	bool PipelineFlags::enableColours()const
	{
		return enableVertexInput( SubmeshFlag::eColours );
	}

	bool PipelineFlags::enablePassMasks()const
	{
		return enableVertexInput( SubmeshFlag::ePassMasks );
	}

	bool PipelineFlags::enableTexcoord0()const
	{
		return enableVertexInput( SubmeshFlag::eTexcoords0 );
	}

	bool PipelineFlags::enableTexcoord1()const
	{
		return enableVertexInput( SubmeshFlag::eTexcoords1 );
	}

	bool PipelineFlags::enableTexcoord2()const
	{
		return enableVertexInput( SubmeshFlag::eTexcoords2 );
	}

	bool PipelineFlags::enableTexcoord3()const
	{
		return enableVertexInput( SubmeshFlag::eTexcoords3 );
	}

	bool PipelineFlags::enableTextures()const
	{
		return enableTexcoords() && !areFlagsEmpty( textures );
	}

	bool PipelineFlags::enableVertexID()const
	{
		return writeVisibility() && isBillboard();
	}

	bool PipelineFlags::enableInstantiation()const
	{
		return checkFlag( m_programFlags, ProgramFlag::eInstantiation );
	}

	bool PipelineFlags::enableParallaxOcclusionMapping( PassComponentRegister const & passComponents )const
	{
		return enableTangentSpace()
			&& checkFlags( textures, passComponents.getHeightFlags() ) != textures.end()
			&& ( checkFlag( m_passFlags, PassFlag::eParallaxOcclusionMappingOne )
				|| checkFlag( m_passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) );
	}

	bool PipelineFlags::enableParallaxOcclusionMappingOne( PassComponentRegister const & passComponents )const
	{
		return enableTangentSpace()
			&& checkFlags( textures, passComponents.getHeightFlags() ) != textures.end()
			&& checkFlag( m_passFlags, PassFlag::eParallaxOcclusionMappingOne );
	}

	bool PipelineFlags::enableVelocity()const
	{
		return hasWorldPosInputs()
			&& writeVelocity();
	}

	bool PipelineFlags::hasFog()const
	{
		return getFogType( m_sceneFlags ) != FogType::eDisabled;
	}

	bool PipelineFlags::enableTexcoord( SubmeshFlag data )const
	{
		CU_Require( uint32_t( data ) >= uint32_t( SubmeshFlag::eTexcoords0 )
			&& uint32_t( data ) <= uint32_t( SubmeshFlag::eTexcoords3 ) );
		return hasSubmeshData( data )
			&& ( forceTexCoords()
				|| ( !areFlagsEmpty( textures ) && hasSubmeshData( data ) ) );
	}

	bool PipelineFlags::enableNonTexcoord( SubmeshFlag data )const
	{
		CU_Require( uint32_t( data ) < uint32_t( SubmeshFlag::eTexcoords0 )
			|| uint32_t( data ) > uint32_t( SubmeshFlag::eTexcoords3 ) );
		return hasSubmeshData( data )
			&& pipflags::hasMatchingFlag( data, m_shaderFlags );
	}

	bool PipelineFlags::hasMap( PassComponentTextureFlag flag )const
	{
		auto ires = splitTextureFlag( flag );
		auto passIndex = ires.first;
		auto textureFlag = ires.second;
		return textures.end() != std::find_if( textures.begin()
			, textures.end()
			, [passIndex, textureFlag]( PassComponentTextureFlag lookup )
			{
				auto [lookupPassIndex, lookupTextureFlag] = splitTextureFlag( lookup );
				return lookupPassIndex == passIndex
					&& castor::hasAny( lookupTextureFlag, textureFlag );
			} );
	}

	TextureFlagsArray PipelineFlags::makeTexturesFlags()const
	{
		TextureFlagsArray result;

		for ( auto & textureFlag : textures )
		{
			if ( textureFlag != 0u )
			{
				result.push_back( textureFlag );
			}
		}

		return result;
	}

	//*********************************************************************************************

	bool operator==( PipelineFlags const & lhs, PipelineFlags const & rhs )
	{
		return static_cast< PipelineHiHashDetails const & >( lhs ) == static_cast< PipelineHiHashDetails const & >( rhs )
			&& static_cast< PipelineLoHashDetails const & >( lhs ) == static_cast< PipelineLoHashDetails const & >( rhs )
			&& lhs.m_sceneFlags == rhs.m_sceneFlags
			&& lhs.colourBlendMode == rhs.colourBlendMode
			&& lhs.alphaBlendMode == rhs.alphaBlendMode
			&& lhs.renderPassType == rhs.renderPassType
			&& lhs.topology == rhs.topology
			&& lhs.patchVertices == rhs.patchVertices;
	}

	//*********************************************************************************************

	PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, Submesh const & data
		, Pass const & pass
		, bool isFrontCulled )
	{
		return pipflags::getPipelineBaseHash( renderPass.createPipelineFlags( pass
			, pass.getTexturesMask()
			, data.getSubmeshFlags( &pass )
			, data.getProgramFlags( *pass.getOwner() )
			, SceneFlag::eNone
			, data.getTopology()
			, isFrontCulled
			, {} ) );
	}

	PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, BillboardBase const & data
		, Pass const & pass
		, bool isFrontCulled )
	{
		return pipflags::getPipelineBaseHash( renderPass.createPipelineFlags( pass
			, pass.getTexturesMask()
			, data.getSubmeshFlags()
			, data.getProgramFlags()
			, SceneFlag::eNone
			, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
			, isFrontCulled
			, {} ) );
	}

	PipelineHiHashDetails getPipelineHiHashDetails( PipelineBaseHash const & hash
		, ShaderFlags shaderFlags )
	{
		return pipflags::getHiHashDetails( hash.hi
			, hash.components
			, hash.textures
			, shaderFlags );
	}

	//*********************************************************************************************
}
