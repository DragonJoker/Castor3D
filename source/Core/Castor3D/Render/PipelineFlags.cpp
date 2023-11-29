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
		namespace hi
		{
			static constexpr uint64_t maxSubmeshSize = castor::getBitSize( MaxSubmeshCombines );
			static constexpr uint64_t maxSubmeshMask = ( 0x1ull << uint64_t( maxSubmeshSize ) ) - 1u;
			static constexpr uint64_t maxPassSize = castor::getBitSize( MaxPassCombines );
			static constexpr uint64_t maxPassMask = ( 0x1ull << uint64_t( maxPassSize ) ) - 1u;
			static constexpr uint64_t maxTexturesSize = castor::getBitSize( MaxTextureCombines );
			static constexpr uint64_t maxTexturesMask = ( 0x1ull << uint64_t( maxTexturesSize ) ) - 1u;
			static constexpr uint64_t maxProgramSize = castor::getBitSize( uint32_t( ProgramFlag::eAllBase ) );
			static constexpr uint64_t maxProgramMask = ( 0x1ull << uint64_t( maxProgramSize ) ) - 1u;
			static constexpr uint64_t maxLightingModelIDSize = castor::getBitSize( MaxLightingModels );
			static constexpr uint64_t maxLightingModelIDMask = ( 0x1ull << uint64_t( maxLightingModelIDSize ) ) - 1u;
			static constexpr uint64_t maxCompareOpSize = castor::getBitSize( uint32_t( VK_COMPARE_OP_ALWAYS + 1 ) );
			static constexpr uint64_t maxCompareOpMask = ( 0x1ull << uint64_t( maxCompareOpSize ) ) - 1u;
			static constexpr uint64_t maxSubmeshDataSize = castor::getBitSize( MaxSubmeshDataBindings );
			static constexpr uint64_t maxSubmeshDataMask = ( 0x1ull << uint64_t( maxSubmeshDataSize ) ) - 1u;
			static constexpr uint64_t maxBackgroundModelIDSize = castor::getBitSize( MaxBackgroundModels );
			static constexpr uint64_t maxBackgroundModelIDMask = ( 0x1ull << uint64_t( maxBackgroundModelIDSize ) ) - 1u;
			static constexpr uint64_t maxPassLayerSize = castor::getBitSize( MaxPassLayers );
			static constexpr uint64_t maxPassLayerMask = ( 0x1ull << uint64_t( maxPassLayerSize ) ) - 1u;
			static constexpr uint64_t maxSize = maxSubmeshSize + maxProgramSize + maxLightingModelIDSize + maxPassSize + maxTexturesSize + maxCompareOpSize + maxSubmeshDataSize + maxBackgroundModelIDSize + maxPassLayerSize + 1u;
			static_assert( 64 >= maxSize );
		}

		namespace lo
		{
			static constexpr uint64_t maxMorphTargetOffsetSize = 32u;
			static constexpr uint64_t maxMorphTargetOffsetMask = ( 0x1ull << uint64_t( maxMorphTargetOffsetSize ) ) - 1u;
			static constexpr uint64_t maxSubmeshDataSize = 16u;
			static constexpr uint64_t maxSubmeshDataMask = ( 0x1ull << uint64_t( maxSubmeshDataSize ) ) - 1u;
			static constexpr uint64_t maxSize = maxMorphTargetOffsetSize + maxSubmeshDataSize;
			static_assert( 64 >= maxSize );
		}

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

			PipelineHiHashDetails result{ {}, {}, 0u, 0u };
			result.m_shaderFlags = shaderFlags;

			uint32_t offset = 0u;
			result.pass = passComponents.getPassComponentCombine( PassComponentCombineID( ( hiHash >> offset ) & hi::maxPassMask ) );
			offset += hi::maxPassSize;
			result.submesh = submeshComponents.getSubmeshComponentCombine(SubmeshComponentCombineID( ( hiHash >> offset ) & hi::maxSubmeshMask ) );
			offset += hi::maxSubmeshSize;
			result.textures = passComponents.getTextureCombine( TextureCombineID( ( hiHash >> offset ) & hi::maxTexturesMask ) );
			offset += hi::maxTexturesSize;
			result.m_programFlags = ProgramFlags( ( hiHash >> offset ) & hi::maxProgramMask );
			offset += hi::maxProgramSize;
			result.lightingModelId = ( LightingModelID( ( hiHash >> offset ) & hi::maxLightingModelIDMask ) + 1u );
			offset += hi::maxLightingModelIDSize;
			result.alphaFunc = VkCompareOp( ( hiHash >> offset ) & hi::maxCompareOpMask );
			offset += hi::maxCompareOpSize;
			result.backgroundModelId = BackgroundModelID( ( ( hiHash >> offset ) & hi::maxBackgroundModelIDMask ) + 1u );
			offset += hi::maxBackgroundModelIDSize;
			result.passLayerIndex = uint32_t( ( hiHash >> offset ) & hi::maxPassLayerMask );
			offset += hi::maxPassLayerSize;
			result.submeshDataBindings = uint32_t( ( hiHash >> offset ) & hi::maxSubmeshDataMask );
			offset += hi::maxSubmeshDataSize;
			result.isStatic = uint32_t( ( hiHash >> offset ) & 0x1u );

			CU_Require( result.lightingModelId != 0 );
			return result;
		}

		static uint64_t getHiHash( PassComponentRegister const & passComponents
			, SubmeshComponentRegister const & submeshComponents
			, PipelineHiHashDetails const & flags )
		{
			CU_Require( flags.lightingModelId != 0 );
			CU_Require( flags.passLayerIndex < MaxPassLayers );

			auto programFlags = flags.m_programFlags;
			remFlag( programFlags, ProgramFlag::eAllOptional );

			uint32_t offset = 0u;
			uint64_t result{};
			auto passComponentsId = flags.pass.baseId
				? flags.pass.baseId
				: passComponents.getPassComponentCombineID( flags.pass );
			auto submeshComponentsId = flags.submesh.baseId
				? flags.submesh.baseId
				: submeshComponents.getSubmeshComponentCombineID( flags.submesh );
			result |= uint64_t( passComponentsId & hi::maxPassMask ) << offset;
			offset += hi::maxPassSize;
			result |= ( uint64_t( submeshComponentsId ) & hi::maxSubmeshMask ) << offset;
			offset += hi::maxSubmeshSize;
			result |= uint64_t( passComponents.getTextureCombineID( flags.textures ) & hi::maxTexturesMask) << offset;
			offset += hi::maxTexturesSize;
			result |= ( uint64_t( programFlags ) & hi::maxProgramMask ) << offset;
			offset += hi::maxProgramSize;
			result |= ( uint64_t( flags.lightingModelId - 1u ) & hi::maxLightingModelIDMask ) << offset;
			offset += hi::maxLightingModelIDSize;
			result |= uint64_t( flags.alphaFunc & hi::maxCompareOpMask) << offset;
			offset += hi::maxCompareOpSize;
			result |= uint64_t( ( flags.backgroundModelId - 1u ) & hi::maxBackgroundModelIDMask ) << offset;
			offset += hi::maxBackgroundModelIDSize;
			result |= uint64_t( flags.passLayerIndex & hi::maxPassLayerMask ) << offset;
			offset += hi::maxPassLayerSize;
			result |= uint64_t( flags.submeshDataBindings & hi::maxSubmeshDataMask ) << offset;
			offset += hi::maxSubmeshDataSize;
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
			CU_Require( flags.backgroundModelId == details.backgroundModelId );
			CU_Require( flags.passLayerIndex == details.passLayerIndex );
			CU_Require( flags.submeshDataBindings == details.submeshDataBindings );
#endif
			return result;
		}

#if !defined( NDEBUG )
		static PipelineLoHashDetails getLoHashDetails( SubmeshComponentRegister const & submeshComponents
			, uint64_t loHash )
		{
			PipelineLoHashDetails result{};

			uint32_t offset = 0u;
			result.morphTargetsOffset = VkDeviceSize( ( loHash >> offset ) & lo::maxMorphTargetOffsetMask );
			offset += lo::maxMorphTargetOffsetSize;
			result.submeshData = submeshComponents.getRenderData( uint16_t( ( loHash >> offset ) & lo::maxSubmeshDataMask ) );

			return result;
		}
#endif

		static uint64_t getLoHash( SubmeshComponentRegister const & submeshComponents
			, PipelineLoHashDetails const & flags )
		{
			uint32_t offset = 0u;
			uint64_t result{};
			result |= uint64_t( flags.morphTargetsOffset & lo::maxMorphTargetOffsetMask ) << offset;
			offset += lo::maxMorphTargetOffsetSize;
			result |= uint64_t( uint64_t( submeshComponents.getRenderDataId( flags.submeshData ) ) & lo::maxSubmeshDataMask ) << offset;

#if !defined( NDEBUG )
			auto details = getLoHashDetails( submeshComponents, result );
			CU_Require( flags.morphTargetsOffset == details.morphTargetsOffset );
			CU_Require( flags.submeshData == details.submeshData );
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
			&& lhs.backgroundModelId == rhs.backgroundModelId
			&& lhs.alphaFunc == rhs.alphaFunc
			&& lhs.passLayerIndex == rhs.passLayerIndex
			&& lhs.submeshDataBindings == rhs.submeshDataBindings
			&& lhs.isStatic == rhs.isStatic
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
		result.lo = pipflags::getLoHash( submeshComponents, flags );
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
				, {}
				, data.getRenderData() ) );
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
				, {}
				, nullptr ) );
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
