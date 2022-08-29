/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PipelineFlags_H___
#define ___C3D_PipelineFlags_H___

#include "RenderModule.hpp"

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

namespace castor3d
{
	struct PipelineBaseHash
	{
		uint64_t hi;
		uint64_t lo;
	};

	inline bool operator<( PipelineBaseHash const & lhs
		, PipelineBaseHash const & rhs )
	{
		return lhs.hi < rhs.hi
			|| ( ( lhs.hi == rhs.hi ) && ( lhs.lo < rhs.lo ) );
	}

	inline bool operator==( PipelineBaseHash const & lhs
		, PipelineBaseHash const & rhs )
	{
		return lhs.hi == rhs.hi
			&& lhs.lo == rhs.lo;
	}

	struct PipelineHashDetails
	{
		explicit PipelineHashDetails( PassTypeID passType
			, PassFlags passFlags = PassFlag::eNone
			, SubmeshFlags submeshFlags = SubmeshFlag::eNone
			, ProgramFlags programFlags = ProgramFlag::eNone
			, TextureFlags texturesFlags = TextureFlag::eNone
			, ShaderFlags shaderFlags = ShaderFlag::eNone
			, VkCompareOp alphaFunc = VkCompareOp::VK_COMPARE_OP_ALWAYS
			, uint32_t passLayerIndex = 0u )
			: passType{ passType }
			, alphaFunc{ alphaFunc }
			, passLayerIndex{ passLayerIndex }
			, m_passFlags{ passFlags }
			, m_submeshFlags{ submeshFlags }
			, m_programFlags{ programFlags }
			, m_texturesFlags{ texturesFlags }
			, m_shaderFlags{ shaderFlags }
		{
		}

		PassTypeID passType;
		VkCompareOp alphaFunc;
		uint32_t passLayerIndex;
		//uint32_t maxTexcoordSet;
		PassFlags m_passFlags;
		SubmeshFlags m_submeshFlags;
		ProgramFlags m_programFlags;
		TextureFlags m_texturesFlags;
		ShaderFlags m_shaderFlags;
	};

	C3D_API bool operator==( PipelineHashDetails const & lhs, PipelineHashDetails const & rhs );

	struct PipelineHiHashDetails
	{
		explicit PipelineHiHashDetails( VkDeviceSize morphTargetsOffset = 0u )
			: morphTargetsOffset{ morphTargetsOffset }
		{
		}

		VkDeviceSize morphTargetsOffset{};
	};

	C3D_API bool operator==( PipelineHiHashDetails const & lhs, PipelineHiHashDetails const & rhs );
	/**
	*\~english
	*\brief
	*	Pipeline flags.
	*\~french
	*\brief
	*	Indicateurs de pipeline.
	*/
	struct PipelineFlags
		: PipelineHashDetails
		, PipelineHiHashDetails
	{
		explicit PipelineFlags( PassTypeID passType
			, BlendMode colourBlendMode = BlendMode::eNoBlend
			, BlendMode alphaBlendMode = BlendMode::eNoBlend
			, PassFlags passFlags = PassFlag::eNone
			, RenderPassTypeID renderPassType = 0u
			, SubmeshFlags submeshFlags = SubmeshFlag::eIndex
			, ProgramFlags programFlags = ProgramFlag::eNone
			, SceneFlags sceneFlags = SceneFlag::eNone
			, ShaderFlags shaderFlags = ShaderFlag::eNone
			, VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
			, uint32_t patchVertices = 3u
			, VkCompareOp alphaFunc = VK_COMPARE_OP_ALWAYS
			, TextureFlagsArray textures = {}
			, uint32_t passLayerIndex = {}
			, VkDeviceSize morphTargetsOffset = {} )
			: PipelineHashDetails{ passType
				, passFlags
				, submeshFlags
				, programFlags
				, merge( textures )
				, shaderFlags
				, alphaFunc
				, passLayerIndex }
			, PipelineHiHashDetails{ morphTargetsOffset }
			, m_sceneFlags{ sceneFlags }
			, colourBlendMode{ colourBlendMode }
			, alphaBlendMode{ alphaBlendMode }
			, renderPassType{ renderPassType }
			, topology{ topology }
			, patchVertices{ patchVertices }
		{
			CU_Require( passType != 0 );
		}

		PipelineFlags( PassTypeID passType
			, PassFlags passFlags
			, SubmeshFlags submeshFlags
			, ProgramFlags programFlags
			, TextureFlags texturesFlags
			, ShaderFlags shaderFlags
			, VkCompareOp alphaFunc
			, uint32_t passLayerIndex = 0u )
			: PipelineHashDetails{ passType
				, passFlags
				, submeshFlags
				, programFlags
				, texturesFlags
				, shaderFlags
				, alphaFunc
				, passLayerIndex }
			, PipelineHiHashDetails{}
			, m_sceneFlags{ SceneFlag::eNone }
			, colourBlendMode{ BlendMode::eNoBlend }
			, alphaBlendMode{ BlendMode::eNoBlend }
			, renderPassType{ 0u }
			, topology{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST }
			, patchVertices{ 3u }
		{
			CU_Require( passType != 0 );
		}

		explicit PipelineFlags( PipelineHashDetails const & details )
			: PipelineHashDetails{ details }
			, PipelineHiHashDetails{}
			, m_sceneFlags{ SceneFlag::eNone }
			, colourBlendMode{ BlendMode::eNoBlend }
			, alphaBlendMode{ BlendMode::eNoBlend }
			, renderPassType{ 0u }
			, topology{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST }
			, patchVertices{ 3u }
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
		C3D_API bool enableOpacity()const;
		C3D_API bool enableInstantiation()const;
		C3D_API bool enableParallaxOcclusionMapping()const;
		C3D_API bool enableParallaxOcclusionMappingOne()const;
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
		//@}
		/**@name TextureFlags */
		//@{
		bool hasDiffuseMap()const
		{
			return checkFlag( m_texturesFlags, TextureFlag::eDiffuse );
		}

		bool hasAlbedoMap()const
		{
			return checkFlag( m_texturesFlags, TextureFlag::eAlbedo );
		}

		bool hasEmissiveMap()const
		{
			return checkFlag( m_texturesFlags, TextureFlag::eEmissive );
		}

		bool hasSpecularMap()const
		{
			return checkFlag( m_texturesFlags, TextureFlag::eSpecular );
		}

		bool hasMetalnessMap()const
		{
			return checkFlag( m_texturesFlags, TextureFlag::eMetalness );
		}

		bool hasShininessMap()const
		{
			return checkFlag( m_texturesFlags, TextureFlag::eShininess );
		}

		bool hasRoughnessMap()const
		{
			return checkFlag( m_texturesFlags, TextureFlag::eRoughness );
		}

		bool hasGlossinessMap()const
		{
			return checkFlag( m_texturesFlags, TextureFlag::eGlossiness );
		}

		bool hasNormalMap()const
		{
			return checkFlag( m_texturesFlags, TextureFlag::eNormal );
		}

		bool hasHeightMap()const
		{
			return checkFlag( m_texturesFlags, TextureFlag::eHeight );
		}

		bool hasOcclusionMap()const
		{
			return checkFlag( m_texturesFlags, TextureFlag::eOcclusion );
		}

		bool hasTransmittanceMap()const
		{
			return checkFlag( m_texturesFlags, TextureFlag::eTransmittance );
		}

		bool hasGeometryMaps()const
		{
			return hasAny( m_texturesFlags, TextureFlag::eGeometry );
		}
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
	C3D_API PipelineHashDetails getPipelineHashDetails( PipelineBaseHash const & hash
		, ShaderFlags shaderFlags );
}

#endif
