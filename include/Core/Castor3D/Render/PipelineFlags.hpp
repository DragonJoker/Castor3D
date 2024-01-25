/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PipelineFlags_H___
#define ___C3D_PipelineFlags_H___

#include "RenderModule.hpp"

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/ComponentModule.hpp"

#include <CastorUtils/Design/DynamicBitset.hpp>

namespace castor3d
{
	struct PipelineBaseHash
	{
		uint64_t hi;
		uint64_t lo;
	};

	C3D_API bool operator<( PipelineBaseHash const & lhs
		, PipelineBaseHash const & rhs )noexcept;

	inline bool operator==( PipelineBaseHash const & lhs
		, PipelineBaseHash const & rhs )noexcept
	{
		return lhs.hi == rhs.hi
			&& lhs.lo == rhs.lo;
	}

	struct PipelineHiHashDetails
	{
		explicit PipelineHiHashDetails( PassComponentCombine ppassComponents
			, SubmeshComponentCombine psubmeshComponents
			, LightingModelID plightingModelId
			, BackgroundModelID pbackgroundModelId
			, ProgramFlags programFlags = ProgramFlag::eNone
			, TextureCombine ptextures = TextureCombine{}
			, ShaderFlags shaderFlags = ShaderFlag::eNone
			, VkCompareOp palphaFunc = VkCompareOp::VK_COMPARE_OP_ALWAYS
			, uint32_t ppassLayerIndex = 0u
			, uint32_t psubmeshDataBindings = 0u
			, bool pisStatic = false )noexcept
			: pass{ castor::move( ppassComponents ) }
			, submesh{ castor::move( psubmeshComponents ) }
			, textures{ castor::move( ptextures ) }
			, lightingModelId{ plightingModelId }
			, backgroundModelId{ pbackgroundModelId }
			, alphaFunc{ palphaFunc }
			, submeshDataBindings{ psubmeshDataBindings }
			, passLayerIndex{ ppassLayerIndex }
			, isStatic{ pisStatic }
			, m_programFlags{ programFlags }
			, m_shaderFlags{ shaderFlags }
		{
		}

		PassComponentCombine pass{};
		SubmeshComponentCombine submesh{};
		TextureCombine textures{};
		LightingModelID lightingModelId{};
		BackgroundModelID backgroundModelId{};
		VkCompareOp alphaFunc;
		uint32_t submeshDataBindings{};
		uint32_t passLayerIndex{};
		bool isStatic{};
		ProgramFlags m_programFlags{};
		ShaderFlags m_shaderFlags{};
	};

	C3D_API bool operator==( PipelineHiHashDetails const & lhs, PipelineHiHashDetails const & rhs )noexcept;

	struct PipelineLoHashDetails
	{
		explicit PipelineLoHashDetails( VkDeviceSize pmorphTargetsOffset = 0u
			, SubmeshRenderData const * psubmeshData = nullptr )noexcept
			: morphTargetsOffset{ pmorphTargetsOffset }
			, submeshData{ psubmeshData }
		{
		}

		VkDeviceSize morphTargetsOffset{};
		SubmeshRenderData const * submeshData{};
	};

	C3D_API bool operator==( PipelineLoHashDetails const & lhs, PipelineLoHashDetails const & rhs )noexcept;
	/**
	*\~english
	*\brief
	*	Pipeline flags.
	*\~french
	*\brief
	*	Indicateurs de pipeline.
	*/
	struct PipelineFlags
		: PipelineHiHashDetails
		, PipelineLoHashDetails
	{
		explicit PipelineFlags( PipelineHiHashDetails hiDetails
			, PipelineLoHashDetails loDetails = PipelineLoHashDetails{}
			, SceneFlags sceneFlags = SceneFlag::eNone
			, BlendMode colourBlendMode = BlendMode::eNoBlend
			, BlendMode alphaBlendMode = BlendMode::eNoBlend
			, RenderPassTypeID renderPassType = 0u
			, VkPrimitiveTopology ptopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
			, uint32_t ppatchVertices = 3u )noexcept
			: PipelineHiHashDetails{ castor::move( hiDetails ) }
			, PipelineLoHashDetails{ castor::move( loDetails ) }
			, m_sceneFlags{ sceneFlags }
			, colourBlendMode{ colourBlendMode }
			, alphaBlendMode{ alphaBlendMode }
			, renderPassType{ renderPassType }
			, topology{ ptopology }
			, patchVertices{ ppatchVertices }
		{
			CU_Require( lightingModelId != 0 );
		}

		explicit PipelineFlags( PassComponentCombine ppassComponents
			, SubmeshComponentCombine psubmeshComponents
			, LightingModelID plightingModelId
			, BackgroundModelID pbackgroundModelId
			, BlendMode pcolourBlendMode = BlendMode::eNoBlend
			, BlendMode palphaBlendMode = BlendMode::eNoBlend
			, RenderPassTypeID prenderPassType = 0u
			, ProgramFlags pprogramFlags = ProgramFlag::eNone
			, SceneFlags psceneFlags = SceneFlag::eNone
			, ShaderFlags pshaderFlags = ShaderFlag::eNone
			, VkPrimitiveTopology ptopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
			, uint32_t ppatchVertices = 3u
			, VkCompareOp palphaFunc = VK_COMPARE_OP_ALWAYS
			, TextureCombine textures = {}
			, uint32_t ppassLayerIndex = {}
			, VkDeviceSize pmorphTargetsOffset = {}
			, SubmeshRenderData const * psubmeshData = {}
			, bool pisStatic = false )noexcept
			: PipelineFlags{ PipelineHiHashDetails{ castor::move( ppassComponents )
					, castor::move( psubmeshComponents )
					, plightingModelId
					, pbackgroundModelId
					, pprogramFlags
					, castor::move( textures )
					, pshaderFlags
					, palphaFunc
					, ppassLayerIndex
					, pisStatic }
				, PipelineLoHashDetails{ pmorphTargetsOffset, psubmeshData }
				, psceneFlags
				, pcolourBlendMode
				, palphaBlendMode
				, prenderPassType
				, ptopology
				, ppatchVertices }
		{
			CU_Require( lightingModelId != 0 );
		}

		PipelineFlags( PassComponentCombine ppassComponents
			, SubmeshComponentCombine psubmeshComponents
			, LightingModelID lightingModelId
			, BackgroundModelID pbackgroundModelId
			, ProgramFlags programFlags
			, TextureCombine textures
			, ShaderFlags shaderFlags
			, VkCompareOp alphaFunc
			, uint32_t passLayerIndex = 0u )
			: PipelineFlags{ PipelineHiHashDetails{ castor::move( ppassComponents )
					, castor::move( psubmeshComponents )
					, lightingModelId
					, pbackgroundModelId
					, programFlags
					, castor::move( textures )
					, shaderFlags
					, alphaFunc
					, passLayerIndex }
				, PipelineLoHashDetails{ 0u, nullptr }
				, SceneFlag::eNone
				, BlendMode::eNoBlend
				, BlendMode::eNoBlend
				, 0u
				, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
				, 3u }
		{
			CU_Require( lightingModelId != 0 );
		}

		/* Vertex inputs */
		C3D_API bool enableTexcoords()const noexcept;
		C3D_API bool enableVertexInput( SubmeshData data )const noexcept;
		C3D_API bool enableIndices()const noexcept;
		C3D_API bool enablePosition()const noexcept;
		C3D_API bool enableNormal()const noexcept;
		C3D_API bool enableTangentSpace()const noexcept;
		C3D_API bool enableBitangent()const noexcept;
		C3D_API bool enableColours()const noexcept;
		C3D_API bool enablePassMasks()const noexcept;
		C3D_API bool enableTexcoord0()const noexcept;
		C3D_API bool enableTexcoord1()const noexcept;
		C3D_API bool enableTexcoord2()const noexcept;
		C3D_API bool enableTexcoord3()const noexcept;
		/**/
		C3D_API bool enableTextures()const noexcept;
		C3D_API bool enableVertexID()const noexcept;
		C3D_API bool enableMeshletID()const noexcept;
		C3D_API bool enableInstantiation()const noexcept;
		C3D_API bool enableParallaxOcclusionMapping( PassComponentRegister const & passComponents )const noexcept;
		C3D_API bool enableParallaxOcclusionMappingOne( PassComponentRegister const & passComponents )const noexcept;
		C3D_API bool enableVelocity()const noexcept;

		C3D_API bool hasFog()const noexcept;

		/**@name ShaderFlags */
		//@{
		bool usesWorldSpace()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eWorldSpace );
		}

		bool usesViewSpace()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eViewSpace );
		}

		bool usesGeometry()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eGeometry );
		}

		bool usesTessellation()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eTessellation );
		}

		bool usesColour()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eColour );
		}

		bool usesOpacity()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eOpacity );
		}

		bool forceTexCoords()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eForceTexCoords );
		}

		bool writeEnvironmentMap()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eEnvironmentMapping );
		}

		bool writeShadowMap()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eShadowMapDirectional )
				|| checkFlag( m_shaderFlags, ShaderFlag::eShadowMapSpot )
				|| checkFlag( m_shaderFlags, ShaderFlag::eShadowMapPoint );
		}

		bool writeShadowVSM()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eVsmShadowMap );
		}

		bool writeShadowRSM()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eRsmShadowMap );
		}

		bool writeVelocity()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eVelocity );
		}

		bool writeVisibility()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eVisibility );
		}

		bool writeDepth()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eDepth );
		}

		bool writePicking()const noexcept
		{
			return checkFlag( m_shaderFlags, ShaderFlag::ePicking );
		}
		//@}
		/**@name SceneFlags */
		//@{
		SceneFlags getShadowFlags()const noexcept
		{
			return m_sceneFlags & SceneFlag::eShadowAny;
		}

		SceneFlags getGlobalIlluminationFlags()const noexcept
		{
			return m_sceneFlags & SceneFlag::eGIAny;
		}

		bool hasDiffuseGI()const noexcept
		{
			return hasAny( m_sceneFlags, SceneFlag::eGIAny );
		}
		//@}
		/**@name SubmeshComponents */
		//@{
		bool hasWorldPosInputs()const noexcept
		{
			return submesh.hasVelocityFlag;
		}

		bool hasSkinData()const noexcept
		{
			return submesh.hasSkinFlag;
		}
		//@}
		/**@name ProgramFlags */
		//@{
		bool isBillboard()const noexcept
		{
			return checkFlag( m_programFlags, ProgramFlag::eBillboards );
		}

		bool isFrontCulled()const noexcept
		{
			return checkFlag( m_programFlags, ProgramFlag::eFrontCulled );
		}

		bool usesMesh()const noexcept
		{
			return checkFlag( m_programFlags, ProgramFlag::eHasMesh );
		}

		bool usesTask()const noexcept
		{
			return checkFlag( m_programFlags, ProgramFlag::eHasTask );
		}
		//@}
		/**@name Components */
		//@{
		C3D_API bool hasPassFlag( PassComponentFlag flag )const noexcept;
		C3D_API bool hasSubmeshFlag( SubmeshComponentFlag flag )const noexcept;
		//@}
		/**@name Textures */
		//@{
		C3D_API bool hasMap( PassComponentTextureFlag flag )const noexcept;
		//@}

	public:
		SceneFlags m_sceneFlags;
		BlendMode colourBlendMode;
		BlendMode alphaBlendMode;
		RenderPassTypeID renderPassType;
		VkPrimitiveTopology topology;
		uint32_t patchVertices;

	private:
		bool enableTexcoord( SubmeshData data )const noexcept;
		bool enableNonTexcoord( SubmeshData data )const noexcept;
		bool hasSubmeshData( SubmeshData data )const noexcept;
	};

	C3D_API bool operator==( PipelineFlags const & lhs, PipelineFlags const & rhs )noexcept;
	C3D_API PipelineBaseHash getPipelineBaseHash( PassComponentRegister const & passComponents
		, SubmeshComponentRegister const & submeshComponents
		, PipelineFlags const & flags )noexcept;
	C3D_API PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, Submesh const & data
		, Pass const & pass
		, bool isFrontCulled )noexcept;
	C3D_API PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, BillboardBase const & data
		, Pass const & pass
		, bool isFrontCulled )noexcept;
	C3D_API PipelineHiHashDetails getPipelineHiHashDetails( RenderNodesPass const & renderPass
		, PipelineBaseHash const & hash
		, ShaderFlags shaderFlags )noexcept;
	C3D_API PipelineHiHashDetails getPipelineHiHashDetails( RenderTechniquePass const & renderPass
		, PipelineBaseHash const & hash
		, ShaderFlags shaderFlags )noexcept;
}

#endif
