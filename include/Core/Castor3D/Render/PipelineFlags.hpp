/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PipelineFlags_H___
#define ___C3D_PipelineFlags_H___

#include "RenderModule.hpp"

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

#include <CastorUtils/Design/DynamicBitset.hpp>

namespace castor3d
{
	struct PipelineBaseHash
	{
		uint64_t hi;
		uint64_t lo;
		PassComponentsBitset components;
		ComponentsTextures textures;
	};

	C3D_API bool operator<( PipelineBaseHash const & lhs
		, PipelineBaseHash const & rhs );

	inline bool operator==( PipelineBaseHash const & lhs
		, PipelineBaseHash const & rhs )
	{
		return lhs.hi == rhs.hi
			&& lhs.lo == rhs.lo
			&& lhs.components == rhs.components;
	}

	struct PipelineHiHashDetails
	{
		explicit PipelineHiHashDetails( PassComponentsBitset pcomponents
			, PassTypeID passType
			, PassFlags passFlags = PassFlag::eNone
			, SubmeshFlags submeshFlags = SubmeshFlag::eNone
			, ProgramFlags programFlags = ProgramFlag::eNone
			, TextureFlagsArray ptextures = TextureFlagsArray{}
			, ShaderFlags shaderFlags = ShaderFlag::eNone
			, VkCompareOp alphaFunc = VkCompareOp::VK_COMPARE_OP_ALWAYS
			, uint32_t passLayerIndex = 0u )
			: components{ std::move( pcomponents ) }
			, textures{ makeComponentsTextures( ptextures ) }
			, passType{ passType }
			, alphaFunc{ alphaFunc }
			, passLayerIndex{ passLayerIndex }
			, m_passFlags{ passFlags }
			, m_submeshFlags{ submeshFlags }
			, m_programFlags{ programFlags }
			, m_shaderFlags{ shaderFlags }
		{
		}

		PassComponentsBitset components;
		ComponentsTextures textures;
		PassTypeID passType;
		VkCompareOp alphaFunc;
		uint32_t passLayerIndex;
		//uint32_t maxTexcoordSet;
		PassFlags m_passFlags;
		SubmeshFlags m_submeshFlags;
		ProgramFlags m_programFlags;
		ShaderFlags m_shaderFlags;
	};

	C3D_API bool operator==( PipelineHiHashDetails const & lhs, PipelineHiHashDetails const & rhs );

	struct PipelineLoHashDetails
	{
		explicit PipelineLoHashDetails( VkDeviceSize morphTargetsOffset = 0u )
			: morphTargetsOffset{ morphTargetsOffset }
		{
		}

		VkDeviceSize morphTargetsOffset{};
	};

	C3D_API bool operator==( PipelineLoHashDetails const & lhs, PipelineLoHashDetails const & rhs );
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
			, uint32_t ppatchVertices = 3u )
			: PipelineHiHashDetails{ std::move( hiDetails ) }
			, PipelineLoHashDetails{ std::move( loDetails ) }
			, m_sceneFlags{ sceneFlags }
			, colourBlendMode{ colourBlendMode }
			, alphaBlendMode{ alphaBlendMode }
			, renderPassType{ renderPassType }
			, topology{ ptopology }
			, patchVertices{ ppatchVertices }
		{
			CU_Require( passType != 0 );
		}

		explicit PipelineFlags( PassComponentsBitset pcomponents
			, PassTypeID ppassType
			, BlendMode pcolourBlendMode = BlendMode::eNoBlend
			, BlendMode palphaBlendMode = BlendMode::eNoBlend
			, PassFlags ppassFlags = PassFlag::eNone
			, RenderPassTypeID prenderPassType = 0u
			, SubmeshFlags psubmeshFlags = SubmeshFlag::eIndex
			, ProgramFlags pprogramFlags = ProgramFlag::eNone
			, SceneFlags psceneFlags = SceneFlag::eNone
			, ShaderFlags pshaderFlags = ShaderFlag::eNone
			, VkPrimitiveTopology ptopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
			, uint32_t ppatchVertices = 3u
			, VkCompareOp palphaFunc = VK_COMPARE_OP_ALWAYS
			, TextureFlagsArray textures = {}
			, uint32_t ppassLayerIndex = {}
			, VkDeviceSize pmorphTargetsOffset = {} )
			: PipelineFlags{ PipelineHiHashDetails{ std::move( pcomponents )
					, ppassType
					, ppassFlags
					, psubmeshFlags
					, pprogramFlags
					, std::move( textures )
					, pshaderFlags
					, palphaFunc
					, ppassLayerIndex }
				, PipelineLoHashDetails{ pmorphTargetsOffset }
				, psceneFlags
				, pcolourBlendMode
				, palphaBlendMode
				, prenderPassType
				, ptopology
				, ppatchVertices }
		{
			CU_Require( passType != 0 );
		}

		PipelineFlags( PassComponentsBitset pcomponents
			, PassTypeID passType
			, PassFlags passFlags
			, SubmeshFlags submeshFlags
			, ProgramFlags programFlags
			, TextureFlagsArray textures
			, ShaderFlags shaderFlags
			, VkCompareOp alphaFunc
			, uint32_t passLayerIndex = 0u )
			: PipelineFlags{ PipelineHiHashDetails{ std::move( pcomponents )
					, passType
					, passFlags
					, submeshFlags
					, programFlags
					, std::move( textures )
					, shaderFlags
					, alphaFunc
					, passLayerIndex }
				, PipelineLoHashDetails{}
				, SceneFlag::eNone
				, BlendMode::eNoBlend
				, BlendMode::eNoBlend
				, 0u
				, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
				, 3u }
		{
			CU_Require( passType != 0 );
		}

		/* Vertex inputs */
		C3D_API bool enableTexcoords()const;
		C3D_API bool enableVertexInput( SubmeshFlag data )const;
		C3D_API bool enableIndices()const;
		C3D_API bool enablePosition()const;
		C3D_API bool enableNormal()const;
		C3D_API bool enableTangentSpace()const;
		C3D_API bool enableColours()const;
		C3D_API bool enablePassMasks()const;
		C3D_API bool enableTexcoord0()const;
		C3D_API bool enableTexcoord1()const;
		C3D_API bool enableTexcoord2()const;
		C3D_API bool enableTexcoord3()const;
		/**/
		C3D_API bool enableTextures()const;
		C3D_API bool enableVertexID()const;
		C3D_API bool enableInstantiation()const;
		C3D_API bool enableParallaxOcclusionMapping( PassComponentRegister const & passComponents )const;
		C3D_API bool enableParallaxOcclusionMappingOne( PassComponentRegister const & passComponents )const;
		C3D_API bool enableVelocity()const;

		C3D_API bool hasFog()const;

		/**@name ShaderFlags */
		//@{
		bool usesWorldSpace()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eWorldSpace );
		}

		bool usesViewSpace()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eViewSpace );
		}

		bool usesGeometry()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eGeometry );
		}

		bool usesTessellation()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eTessellation );
		}

		bool usesColour()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eColour );
		}

		bool usesOpacity()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eOpacity );
		}

		bool forceTexCoords()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eForceTexCoords );
		}

		bool writeEnvironmentMap()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eEnvironmentMapping );
		}

		bool writeShadowMap()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eShadowMapDirectional )
				|| checkFlag( m_shaderFlags, ShaderFlag::eShadowMapSpot )
				|| checkFlag( m_shaderFlags, ShaderFlag::eShadowMapPoint );
		}

		bool writeShadowVSM()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eVsmShadowMap );
		}

		bool writeShadowRSM()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eRsmShadowMap );
		}

		bool writeVelocity()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eVelocity );
		}

		bool writeVisibility()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eVisibility );
		}

		bool writeDepth()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::eDepth );
		}

		bool writePicking()const
		{
			return checkFlag( m_shaderFlags, ShaderFlag::ePicking );
		}
		//@}
		/**@name SceneFlags */
		//@{
		SceneFlags getShadowFlags()const
		{
			return m_sceneFlags & SceneFlag::eShadowAny;
		}

		SceneFlags getGlobalIlluminationFlags()const
		{
			return m_sceneFlags & SceneFlag::eGIAny;
		}

		bool hasDiffuseGI()const
		{
			return hasAny( m_sceneFlags, SceneFlag::eGIAny );
		}
		//@}
		/**@name SubmeshFlags */
		//@{
		bool hasSubmeshData( SubmeshFlag flag )const
		{
			return checkFlag( m_submeshFlags, flag );
		}

		bool hasWorldPosInputs()const
		{
			return hasSubmeshData( SubmeshFlag::eVelocity );
		}

		bool hasSkinData()const
		{
			return hasSubmeshData( SubmeshFlag::eSkin );
		}
		//@}
		/**@name ProgramFlags */
		//@{
		bool isBillboard()const
		{
			return checkFlag( m_programFlags, ProgramFlag::eBillboards );
		}

		bool hasInvertNormals()const
		{
			return checkFlag( m_programFlags, ProgramFlag::eInvertNormals );
		}

		bool usesMesh()const
		{
			return checkFlag( m_programFlags, ProgramFlag::eHasMesh );
		}

		bool usesTask()const
		{
			return checkFlag( m_programFlags, ProgramFlag::eHasTask );
		}
		//@}
		/**@name PassFlags */
		//@{
		bool hasUntile()const
		{
			return checkFlag( m_passFlags, PassFlag::eUntile );
		}

		bool hasOpacity()const
		{
			return ( checkFlag( m_passFlags, PassFlag::eAlphaBlending )
				|| ( checkFlag( m_passFlags, PassFlag::eAlphaTest ) && ( alphaFunc != VK_COMPARE_OP_ALWAYS ) ) );
		}
		//@}
		/**@name Textures */
		//@{
		C3D_API bool hasMap( PassComponentTextureFlag flag )const;
		C3D_API TextureFlagsArray makeTexturesFlags()const;
		//@}

	public:
		SceneFlags m_sceneFlags;
		BlendMode colourBlendMode;
		BlendMode alphaBlendMode;
		RenderPassTypeID renderPassType;
		VkPrimitiveTopology topology;
		uint32_t patchVertices;

	private:
		bool enableTexcoord( SubmeshFlag data )const;
		bool enableNonTexcoord( SubmeshFlag data )const;
	};

	C3D_API bool operator==( PipelineFlags const & lhs, PipelineFlags const & rhs );
	C3D_API PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, Submesh const & data
		, Pass const & pass
		, bool isFrontCulled );
	C3D_API PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, BillboardBase const & data
		, Pass const & pass
		, bool isFrontCulled );
	C3D_API PipelineHiHashDetails getPipelineHiHashDetails( PipelineBaseHash const & hash
		, ShaderFlags shaderFlags );
}

#endif
