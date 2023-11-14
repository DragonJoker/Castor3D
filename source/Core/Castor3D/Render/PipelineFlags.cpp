#include "Castor3D/Render/PipelineFlags.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderTechniquePass.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace pipflags
	{
		static bool hasMatchingFlag( SubmeshData submeshFlag
			, ShaderFlags const & shaderFlags )
		{
			switch ( submeshFlag )
			{
			case castor3d::SubmeshData::eNormals:
				return checkFlag( shaderFlags, ShaderFlag::eNormal );
			case castor3d::SubmeshData::eTangents:
			case castor3d::SubmeshData::eBitangents:
				return checkFlag( shaderFlags, ShaderFlag::eTangent );
			case castor3d::SubmeshData::eVelocity:
				return checkFlag( shaderFlags, ShaderFlag::eVelocity );
			case castor3d::SubmeshData::eColours:
				return checkFlag( shaderFlags, ShaderFlag::eColour );
			default:
				return true;
			}
		}

		static PipelineHiHashDetails getHiHashDetails( PassComponentRegister const & passComponents
			, SubmeshComponentRegister const & submeshComponents
			, uint64_t hiHash
			, ShaderFlags shaderFlags )
		{
			constexpr uint64_t maxSubmeshSize = sizeof( SubmeshComponentCombineID ) * 8u;
			constexpr uint64_t maxSubmeshMask = ( 0x1ull << uint64_t( maxSubmeshSize ) ) - 1u;
			constexpr uint64_t maxProgramSize = castor::getBitSize( uint32_t( ProgramFlag::eAllBase ) );
			constexpr uint64_t maxProgramMask = ( 0x1ull << uint64_t( maxProgramSize ) ) - 1u;
			constexpr uint64_t maxLightingModelIDSize = castor::getBitSize( MaxLightingModels );
			constexpr uint64_t maxLightingModelIDMask = ( 0x1ull << uint64_t( maxLightingModelIDSize ) ) - 1u;
			constexpr uint64_t maxPassSize = sizeof( PassComponentCombineID ) * 8u;
			constexpr uint64_t maxPassMask = ( 0x1ull << uint64_t( maxPassSize ) ) - 1u;
			constexpr uint64_t maxTexturesSize = sizeof( TextureCombineID ) * 8u;
			constexpr uint64_t maxTexturesMask = ( 0x1ull << uint64_t( maxTexturesSize ) ) - 1u;
			constexpr uint64_t maxCompareOpSize = castor::getBitSize( uint32_t( VK_COMPARE_OP_ALWAYS + 1 ) );
			constexpr uint64_t maxCompareOpMask = ( 0x1ull << uint64_t( maxCompareOpSize ) ) - 1u;
			constexpr uint64_t maxSize = maxSubmeshSize + maxProgramSize + maxLightingModelIDSize + maxPassSize + maxTexturesSize + maxCompareOpSize + 1u;
			static_assert( 64 >= maxSize );

			PipelineHiHashDetails result{ {}, {}, 0u };
			result.m_shaderFlags = shaderFlags;

			auto offset = 0u;
			result.pass = passComponents.getPassComponentCombine( PassComponentCombineID( ( hiHash >> offset ) & maxPassMask ) );
			offset += maxPassSize;
			result.submesh = submeshComponents.getSubmeshComponentCombine(SubmeshComponentCombineID( ( hiHash >> offset ) & maxSubmeshMask ) );
			offset += maxPassSize;
			result.textures = passComponents.getTextureCombine( TextureCombineID( ( hiHash >> offset ) & maxTexturesMask ) );
			offset += maxTexturesSize;
			result.m_programFlags = ProgramFlags( ( hiHash >> offset ) & maxProgramMask );
			offset += maxProgramSize;
			result.lightingModelId = ( LightingModelID( ( hiHash >> offset ) & maxLightingModelIDMask ) + 1u );
			offset += maxLightingModelIDSize;
			result.alphaFunc = VkCompareOp( ( hiHash >> offset ) & maxCompareOpMask );
			offset += maxCompareOpSize;
			result.isStatic = uint32_t( ( hiHash >> offset ) & 0x1u );

			CU_Require( result.lightingModelId != 0 );
			return result;
		}

		static uint64_t getHiHash( PassComponentRegister const & passComponents
			, SubmeshComponentRegister const & submeshComponents
			, PipelineHiHashDetails const & flags )
		{
			constexpr uint64_t maxSubmeshSize = sizeof( SubmeshComponentCombineID ) * 8u;
			constexpr uint64_t maxSubmeshMask = ( 0x1ull << uint64_t( maxSubmeshSize ) ) - 1u;
			constexpr uint64_t maxProgramSize = castor::getBitSize( uint32_t( ProgramFlag::eAllBase ) );
			constexpr uint64_t maxProgramMask = ( 0x1ull << uint64_t( maxProgramSize ) ) - 1u;
			constexpr uint64_t maxLightingModelIDSize = castor::getBitSize( MaxLightingModels - 1u );
			constexpr uint64_t maxLightingModelIDMask = ( 0x1ull << uint64_t( maxLightingModelIDSize ) ) - 1u;
			constexpr uint64_t maxPassSize = sizeof( PassComponentCombineID ) * 8u;
			constexpr uint64_t maxPassMask = ( 0x1ull << uint64_t( maxPassSize ) ) - 1u;
			constexpr uint64_t maxTexturesSize = sizeof( TextureCombineID ) * 8u;
			constexpr uint64_t maxTexturesMask = ( 0x1ull << uint64_t( maxTexturesSize ) ) - 1u;
			constexpr uint64_t maxCompareOpSize = castor::getBitSize( uint32_t( VK_COMPARE_OP_ALWAYS + 1 ) );
			constexpr uint64_t maxCompareOpMask = ( 0x1ull << uint64_t( maxCompareOpSize ) ) - 1u;
			constexpr uint64_t maxSize = maxSubmeshSize + maxProgramSize + maxLightingModelIDSize + maxPassSize + maxTexturesSize + maxCompareOpSize + 1u;
			static_assert( 64 >= maxSize );
			CU_Require( flags.lightingModelId != 0 );

			auto programFlags = flags.m_programFlags;
			remFlag( programFlags, ProgramFlag::eAllOptional );

			auto offset = 0u;
			uint64_t result{};
			auto passComponentsId = flags.pass.baseId
				? flags.pass.baseId
				: passComponents.getPassComponentCombineID( flags.pass );
			auto submeshComponentsId = flags.submesh.baseId
				? flags.submesh.baseId
				: submeshComponents.getSubmeshComponentCombineID( flags.submesh );
			result |= uint64_t( passComponentsId & maxPassMask ) << offset;
			offset += maxPassSize;
			result |= ( uint64_t( submeshComponentsId ) & maxSubmeshMask ) << offset;
			offset += maxSubmeshSize;
			result |= uint64_t( passComponents.getTextureCombineID( flags.textures ) & maxTexturesMask) << offset;
			offset += maxTexturesSize;
			result |= ( uint64_t( programFlags ) & maxProgramMask ) << offset;
			offset += maxProgramSize;
			result |= ( uint64_t( flags.lightingModelId - 1u ) & maxLightingModelIDMask ) << offset;
			offset += maxLightingModelIDSize;
			result |= uint64_t( flags.alphaFunc & maxCompareOpMask) << offset;
			offset += maxCompareOpSize;
			result |= ( flags.isStatic ? 1ull : 0ull ) << offset;

#if !defined( NDEBUG )
			auto passCombine = flags.pass.baseId
				? passComponents.getPassComponentCombine( flags.pass.baseId )
				: flags.pass;
			auto submeshCombine = flags.submesh.baseId
				? submeshComponents.getSubmeshComponentCombine( flags.submesh.baseId )
				: flags.submesh;
			auto details = getHiHashDetails( passComponents
				, submeshComponents
				, result
				, flags.m_shaderFlags );
			CU_Require( flags.m_shaderFlags == details.m_shaderFlags );
			CU_Require( programFlags == details.m_programFlags );
			CU_Require( flags.lightingModelId == details.lightingModelId );
			CU_Require( flags.alphaFunc == details.alphaFunc );
			CU_Require( flags.isStatic == details.isStatic );
			CU_Require( contains( passCombine, details.pass ) );
			CU_Require( contains( submeshCombine, details.submesh ) );
			CU_Require( contains( flags.textures, details.textures ) );
#endif
			return result;
		}

#if !defined( NDEBUG )
		static PipelineLoHashDetails getLoHashDetails( uint64_t hash )
		{
			constexpr uint64_t maxBackgroundModelIDSize = castor::getBitSize( MaxBackgroundModels - 1u );
			constexpr uint64_t maxBackgroundModelIDMask = ( 0x1ull << uint64_t( maxBackgroundModelIDSize ) ) - 1u;
			constexpr uint64_t maxPassLayerSize = castor::getBitSize( MaxPassLayers );
			constexpr uint64_t maxPassLayerMask = ( 0x1ull << uint64_t( maxPassLayerSize ) ) - 1u;
			constexpr uint64_t maxMorphTargetOffsetSize = 64u - maxBackgroundModelIDSize - maxPassLayerSize;
			constexpr uint64_t maxMorphTargetOffsetMask = ( 0x1ull << uint64_t( maxMorphTargetOffsetSize ) ) - 1u;
			constexpr uint64_t maxSize = maxMorphTargetOffsetSize + maxBackgroundModelIDSize + maxPassLayerSize;
			static_assert( 64 >= maxSize );

			PipelineLoHashDetails result{};
			auto offset = 0u;
			result.morphTargetsOffset = VkDeviceSize( ( hash >> offset ) & maxMorphTargetOffsetMask );
			offset += maxMorphTargetOffsetSize;
			result.backgroundModelId = BackgroundModelID( ( ( hash >> offset ) & maxBackgroundModelIDMask ) + 1u );
			offset += maxBackgroundModelIDSize;
			result.passLayerIndex = uint32_t( ( hash >> offset ) & maxPassLayerMask );

			return result;
		}
#endif

		static uint64_t getLoHash( PipelineLoHashDetails const & flags )
		{
			constexpr uint64_t maxBackgroundModelIDSize = castor::getBitSize( MaxBackgroundModels - 1u );
			constexpr uint64_t maxBackgroundModelIDMask = ( 0x1ull << uint64_t( maxBackgroundModelIDSize ) ) - 1u;
			constexpr uint64_t maxPassLayerSize = castor::getBitSize( MaxPassLayers );
			constexpr uint64_t maxPassLayerMask = ( 0x1ull << uint64_t( maxPassLayerSize ) ) - 1u;
			constexpr uint64_t maxMorphTargetOffsetSize = 64u - maxBackgroundModelIDSize - maxPassLayerSize;
			constexpr uint64_t maxMorphTargetOffsetMask = ( 0x1ull << uint64_t( maxMorphTargetOffsetSize ) ) - 1u;
			constexpr uint64_t maxSize = maxMorphTargetOffsetSize + maxBackgroundModelIDSize + maxPassLayerSize;
			static_assert( 64 >= maxSize );
			CU_Require( maxMorphTargetOffsetSize == maxSize
				|| ( flags.morphTargetsOffset >> std::min( maxMorphTargetOffsetSize, maxSize - 1 ) ) == 0 );
			CU_Require( flags.passLayerIndex < MaxPassLayers );

			uint64_t result{};
			auto offset = 0u;
			result |= uint64_t( flags.morphTargetsOffset & maxMorphTargetOffsetMask ) << offset;
			offset += maxMorphTargetOffsetSize;
			result |= uint64_t( ( flags.backgroundModelId - 1u ) & maxBackgroundModelIDMask ) << offset;
			offset += maxBackgroundModelIDSize;
			result |= uint64_t( flags.passLayerIndex & maxPassLayerMask ) << offset;

#if !defined( NDEBUG )
			auto details = getLoHashDetails( result );
			CU_Require( flags.morphTargetsOffset == details.morphTargetsOffset );
			CU_Require( flags.backgroundModelId == details.backgroundModelId );
			CU_Require( flags.passLayerIndex == details.passLayerIndex );
#endif
			return result;
		}

		static bool isTexcoordComponent( SubmeshData submeshData )
		{
			return submeshData == SubmeshData::eTexcoords0
				|| submeshData == SubmeshData::eTexcoords1
				|| submeshData == SubmeshData::eTexcoords2
				|| submeshData == SubmeshData::eTexcoords3;
		}
	}

	//*********************************************************************************************

	bool operator<( PipelineBaseHash const & lhs
		, PipelineBaseHash const & rhs )
	{
		return lhs.hi < rhs.hi
			|| ( ( lhs.hi == rhs.hi )
				&& ( lhs.lo < rhs.lo ) );
	}

	//*********************************************************************************************

	bool operator==( PipelineHiHashDetails const & lhs, PipelineHiHashDetails const & rhs )
	{
		return lhs.pass == rhs.pass
			&& lhs.submesh == rhs.submesh
			&& lhs.textures == rhs.textures
			&& lhs.lightingModelId == rhs.lightingModelId
			&& lhs.alphaFunc == rhs.alphaFunc
			&& lhs.isStatic == rhs.isStatic
			&& lhs.m_programFlags == rhs.m_programFlags
			&& lhs.m_shaderFlags == rhs.m_shaderFlags;
	}

	//*********************************************************************************************

	bool operator==( PipelineLoHashDetails const & lhs, PipelineLoHashDetails const & rhs )
	{
		return lhs.backgroundModelId == rhs.backgroundModelId
			&& lhs.morphTargetsOffset == rhs.morphTargetsOffset
			&& lhs.passLayerIndex == rhs.passLayerIndex;
	}

	//*********************************************************************************************

	bool PipelineFlags::enableTexcoords()const
	{
		return forceTexCoords()
			|| ( textures.configCount != 0u
				&& ( submesh.hasTexcoord0Flag
					|| submesh.hasTexcoord1Flag
					|| submesh.hasTexcoord2Flag
					|| submesh.hasTexcoord3Flag ) );
	}

	bool PipelineFlags::enableVertexInput( SubmeshData data )const
	{
		return pipflags::isTexcoordComponent( data )
			? enableTexcoord( data )
			: enableNonTexcoord( data );
	}

	bool PipelineFlags::enableIndices()const
	{
		return submesh.hasTriangleIndexFlag
			|| submesh.hasLineIndexFlag;
	}

	bool PipelineFlags::enablePosition()const
	{
		return submesh.hasPositionFlag;
	}

	bool PipelineFlags::enableNormal()const
	{
		return submesh.hasNormalFlag;
	}

	bool PipelineFlags::enableTangentSpace()const
	{
		return submesh.hasTangentFlag;
	}

	bool PipelineFlags::enableBitangent()const
	{
		return submesh.hasBitangentFlag;
	}

	bool PipelineFlags::enableColours()const
	{
		return submesh.hasColourFlag;
	}

	bool PipelineFlags::enablePassMasks()const
	{
		return submesh.hasPassMaskFlag;
	}

	bool PipelineFlags::enableTexcoord0()const
	{
		return submesh.hasTexcoord0Flag;
	}

	bool PipelineFlags::enableTexcoord1()const
	{
		return submesh.hasTexcoord1Flag;
	}

	bool PipelineFlags::enableTexcoord2()const
	{
		return submesh.hasTexcoord2Flag;
	}

	bool PipelineFlags::enableTexcoord3()const
	{
		return submesh.hasTexcoord3Flag;
	}

	bool PipelineFlags::enableTextures()const
	{
		return enableTexcoords() && textures.configCount != 0u;
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
			&& hasMap( passComponents.getHeightMapFlags() )
			&& ( hasAny( pass, passComponents.getParallaxOcclusionMappingOneFlag() )
				|| hasAny( pass, passComponents.getParallaxOcclusionMappingRepeatFlag() ) );
	}

	bool PipelineFlags::enableParallaxOcclusionMappingOne( PassComponentRegister const & passComponents )const
	{
		return enableTangentSpace()
			&& hasMap( passComponents.getHeightMapFlags() )
			&& hasAny( pass, passComponents.getParallaxOcclusionMappingOneFlag() );
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

	bool PipelineFlags::enableTexcoord( SubmeshData data )const
	{
		CU_Require( uint32_t( data ) >= uint32_t( SubmeshData::eTexcoords0 )
			&& uint32_t( data ) <= uint32_t( SubmeshData::eTexcoords3 ) );
		return hasSubmeshData( data )
			&& ( forceTexCoords()
				|| ( textures.configCount != 0u && hasSubmeshData( data ) ) );
	}

	bool PipelineFlags::enableNonTexcoord( SubmeshData data )const
	{
		CU_Require( uint32_t( data ) < uint32_t( SubmeshData::eTexcoords0 )
			|| uint32_t( data ) > uint32_t( SubmeshData::eTexcoords3 ) );
		return hasSubmeshData( data )
			&& pipflags::hasMatchingFlag( data, m_shaderFlags );
	}

	bool PipelineFlags::hasSubmeshData( SubmeshData data )const
	{
		switch (data)
		{
		case castor3d::SubmeshData::eIndex:
			return submesh.hasLineIndexFlag || submesh.hasTriangleIndexFlag;
		case castor3d::SubmeshData::ePositions:
			return submesh.hasPositionFlag;
		case castor3d::SubmeshData::eNormals:
			return submesh.hasNormalFlag;
		case castor3d::SubmeshData::eTangents:
			return submesh.hasTangentFlag;
		case castor3d::SubmeshData::eBitangents:
			return submesh.hasBitangentFlag;
		case castor3d::SubmeshData::eTexcoords0:
			return submesh.hasTexcoord0Flag;
		case castor3d::SubmeshData::eTexcoords1:
			return submesh.hasTexcoord1Flag;
		case castor3d::SubmeshData::eTexcoords2:
			return submesh.hasTexcoord2Flag;
		case castor3d::SubmeshData::eTexcoords3:
			return submesh.hasTexcoord3Flag;
		case castor3d::SubmeshData::eColours:
			return submesh.hasColourFlag;
		case castor3d::SubmeshData::eSkin:
			return submesh.hasSkinFlag;
		case castor3d::SubmeshData::ePassMasks:
			return submesh.hasPassMaskFlag;
		case castor3d::SubmeshData::eVelocity:
			return submesh.hasVelocityFlag;
		default:
			return false;
		}
	}

	bool PipelineFlags::hasPassFlag( PassComponentFlag flag )const
	{
		return hasAny( pass, flag );
	}

	bool PipelineFlags::hasSubmeshFlag( SubmeshComponentFlag flag )const
	{
		return hasAny( submesh, flag );
	}

	bool PipelineFlags::hasMap( PassComponentTextureFlag flag )const
	{
		return hasAny( textures, flag );
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

	PipelineBaseHash getPipelineBaseHash( PassComponentRegister const & passComponents
		, SubmeshComponentRegister const & submeshComponents
		, PipelineFlags const & flags )
	{
		PipelineBaseHash result{};
		result.hi = pipflags::getHiHash( passComponents, submeshComponents, flags );
		result.lo = pipflags::getLoHash( flags );
		return result;
	}

	PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, Submesh const & data
		, Pass const & pass
		, bool isFrontCulled )
	{
		auto & submeshComponents = renderPass.getEngine()->getSubmeshComponentsRegister();
		return getPipelineBaseHash( renderPass.getEngine()->getPassComponentsRegister()
			, submeshComponents
			, renderPass.createPipelineFlags( pass
				, pass.getTexturesMask()
				, submeshComponents.getSubmeshComponentCombine( data.getComponentCombineID() )
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
		auto & submeshComponents = renderPass.getEngine()->getSubmeshComponentsRegister();
		return getPipelineBaseHash( renderPass.getEngine()->getPassComponentsRegister()
			, submeshComponents
			, renderPass.createPipelineFlags( pass
				, pass.getTexturesMask()
				, submeshComponents.getSubmeshComponentCombine( data.getComponentCombineID() )
				, data.getProgramFlags()
				, SceneFlag::eNone
				, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
				, isFrontCulled
				, {} ) );
	}

	PipelineHiHashDetails getPipelineHiHashDetails( RenderNodesPass const & renderPass
		, PipelineBaseHash const & hash
		, ShaderFlags shaderFlags )
	{
		return pipflags::getHiHashDetails( renderPass.getEngine()->getPassComponentsRegister()
			, renderPass.getEngine()->getSubmeshComponentsRegister()
			, hash.hi
			, shaderFlags );
	}

	PipelineHiHashDetails getPipelineHiHashDetails( RenderTechniquePass const & renderPass
		, PipelineBaseHash const & hash
		, ShaderFlags shaderFlags )
	{
		return pipflags::getHiHashDetails( renderPass.getEngine()->getPassComponentsRegister()
			, renderPass.getEngine()->getSubmeshComponentsRegister()
			, hash.hi
			, shaderFlags );
	}

	//*********************************************************************************************
}
