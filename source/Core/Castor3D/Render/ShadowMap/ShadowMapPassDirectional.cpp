#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	namespace shdpassdir
	{
		static castor::String getPassName( uint32_t cascadeIndex
			, bool needsVsm
			, bool needsRsm )
		{
			auto result = cuT( "DirectionalSMC" ) + castor::string::toString( cascadeIndex );

			if ( needsVsm )
			{
				result += "_VSM";
			}

			if ( needsRsm )
			{
				result += "_RSM";
			}

			return result;
		}
	}

	castor::String const ShadowMapPassDirectional::Type = "c3d.shadows.directional";

	ShadowMapPassDirectional::ShadowMapPassDirectional( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap
		, bool needsVsm
		, bool needsRsm
		, uint32_t cascadeCount )
		: ShadowMapPass{ pass
			, context
			, graph
			, device
			, Type
			, shdpassdir::getPassName( cascadeCount, needsVsm, needsRsm )
			, matrixUbo
			, culler
			, shadowMap
			, needsVsm
			, needsRsm }
	{
		log::trace << "Created " << m_name << std::endl;
	}

	ShadowMapPassDirectional::~ShadowMapPassDirectional()
	{
		getCuller().getCamera().detach();
	}

	void ShadowMapPassDirectional::update( CpuUpdater & updater )
	{
		getCuller().update( updater );
		m_outOfDate = m_outOfDate
			|| getCuller().areAnyChanged();
		RenderNodesPass::update( updater );
	}

	void ShadowMapPassDirectional::doUpdateUbos( CpuUpdater & updater )
	{
		RenderNodesPass::doUpdateUbos( updater );
		m_shadowMapUbo.update( *updater.light, updater.index );
	}

	void ShadowMapPassDirectional::doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		bindings.emplace_back( m_shadowMap.getScene().getLightCache().createLayoutBinding( index++ ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );
		m_initialised = true;
	}

	void ShadowMapPassDirectional::doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		descriptorWrites.push_back( getCuller().getScene().getLightCache().getBinding( index++ ) );
		descriptorWrites.push_back( m_shadowMapUbo.getDescriptorWrite( index++ ) );
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassDirectional::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassDirectional::doCreateBlendState( PipelineFlags const & flags )const
	{
		uint32_t result = 1u;

		if ( m_needsVsm )
		{
			++result;
		}

		if ( m_needsRsm )
		{
			result += 3;
		}

		return RenderNodesPass::createBlendState( BlendMode::eNoBlend
			, BlendMode::eNoBlend
			, result );
	}

	PassFlags ShadowMapPassDirectional::doAdjustPassFlags( PassFlags flags )const
	{
		remFlag( flags, PassFlag::eAlphaBlending );
		return flags;
	}

	ProgramFlags ShadowMapPassDirectional::doAdjustProgramFlags( ProgramFlags flags )const
	{
		addFlag( flags, ProgramFlag::eLighting );
		addFlag( flags, ProgramFlag::eShadowMapDirectional );

		if ( m_needsVsm )
		{
			addFlag( flags, ProgramFlag::eVsmShadowMap );
		}

		if ( m_needsRsm )
		{
			addFlag( flags, ProgramFlag::eRsmShadowMap );
		}

		return flags;
	}

	ShaderPtr ShadowMapPassDirectional::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();

		C3D_ObjectIdsData( writer
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		auto index = uint32_t( GlobalBuffersIdx::eCount ) + 1u;
		C3D_ShadowMap( writer
			, index++
			, RenderPipeline::eBuffers );

		sdw::Pcb pcb{ writer, "DrawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< shader::VertexSurfaceT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::VertexSurfaceT >{ writer
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, [&]( VertexInT< shader::VertexSurfaceT > in
			, VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, in
						, pipelineID
						, writer.cast< sdw::UInt >( in.drawID )
						, flags.programFlags ) );
				auto curPosition = writer.declLocale( "curPosition"
					, in.position );
				auto curNormal = writer.declLocale( "curNormal"
					, in.normal );
				auto curTangent = writer.declLocale( "curTangent"
					, in.tangent );
				out.texture0 = in.texture0;
				out.texture1 = in.texture1;
				out.texture2 = in.texture2;
				out.texture3 = in.texture3;
				out.colour = in.colour;
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				out.nodeId = writer.cast< sdw::Int >( nodeId );
				out.instanceId = writer.cast< UInt >( in.instanceIndex );

				auto mtxModel = writer.declLocale< Mat4 >( "mtxModel"
					, modelData.getModelMtx() );

				if ( checkFlag( flags.submeshFlags, SubmeshFlag::eVelocity ) )
				{
					auto worldPos = writer.declLocale( "worldPos"
						, curPosition );
					out.computeTangentSpace( flags.submeshFlags
						, flags.programFlags
						, vec3( 0.0_f )
						, worldPos.xyz()
						, curNormal
						, curTangent );
				}
				else
				{
					auto worldPos = writer.declLocale( "worldPos"
						, mtxModel * curPosition );
					auto mtxNormal = writer.declLocale< Mat3 >( "mtxNormal"
						, modelData.getNormalMtx( flags.programFlags, mtxModel ) );
					out.computeTangentSpace( flags.submeshFlags
						, flags.programFlags
						, vec3( 0.0_f )
						, worldPos.xyz()
						, mtxNormal
						, curNormal
						, curTangent );
				}

				auto worldPos = writer.getVariable< sdw::Vec4 >( "worldPos" );
				out.worldPosition = worldPos;
				curPosition = c3d_shadowMapData.worldToView( worldPos );
				out.vtx.position = c3d_shadowMapData.viewToProj( curPosition );
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ShadowMapPassDirectional::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		auto & renderSystem = *getEngine()->getRenderSystem();

		using namespace sdw;
		FragmentWriter writer;
		auto textureFlags = filterTexturesFlags( flags.textures );
		bool hasTextures = flags.hasTextures() && !textureFlags.empty();

		shader::Utils utils{ writer, *renderSystem.getEngine() };

		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers
			, ( m_needsRsm
				|| ( checkFlag( flags.texturesFlags, TextureFlag::eOpacity )
					&& flags.alphaFunc != VK_COMPARE_OP_ALWAYS ) ) };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::eBuffers
			, hasTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, hasTextures };
		auto index = uint32_t( GlobalBuffersIdx::eCount );
		auto lightsIndex = index++;
		C3D_ShadowMap( writer
			, index++
			, RenderPipeline::eBuffers );
		auto lightingModel = shader::LightingModel::createModel( utils
			, shader::getLightingModelName( *getEngine(), flags.passType )
			, LightType::eDirectional
			, lightsIndex
			, RenderPipeline::eBuffers
			, false
			, shader::ShadowOptions{ SceneFlag::eNone, true, false }
			, nullptr
			, index
			, RenderPipeline::eBuffers );

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, hasTextures ) );

		// Fragment Outputs
		uint32_t outIndex{};
		auto pxl_linear( writer.declOutput< Float >( "pxl_linear", outIndex++ ) );
		auto pxl_variance( writer.declOutput< Vec2 >( "pxl_variance", m_needsVsm ? outIndex++ : 0u, m_needsVsm ) );
		auto pxl_normal( writer.declOutput< Vec4 >( "pxl_normal", m_needsRsm ? outIndex++ : 0u, m_needsRsm ) );
		auto pxl_position( writer.declOutput< Vec4 >( "pxl_position", m_needsRsm ? outIndex++ : 0u, m_needsRsm ) );
		auto pxl_flux( writer.declOutput< Vec4 >( "pxl_flux", m_needsRsm ? outIndex++ : 0u, m_needsRsm ) );

		writer.implementMainT< shader::FragmentSurfaceT, VoidT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer
				, flags.submeshFlags
				, flags.programFlags
				, getShaderFlags()
				, textureFlags
				, flags.passFlags
				, hasTextures }
			, FragmentOut{ writer }
			, [&]( FragmentInT< shader::FragmentSurfaceT > in
				, FragmentOut out )
			{
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[in.nodeId - 1] );
				pxl_linear = 0.0_f;
				pxl_variance = vec2( 0.0_f );
				pxl_normal = vec4( 0.0_f );
				pxl_position = vec4( 0.0_f );
				pxl_flux = vec4( 0.0_f );
				auto normal = writer.declLocale( "normal"
					, normalize( in.normal )
					, m_needsRsm );
				auto tangent = writer.declLocale( "tangent"
					, normalize( in.tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( in.bitangent ) );
				auto material = materials.getMaterial( modelData.getMaterialId() );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.emissive )
					, m_needsRsm );
				auto alpha = writer.declLocale( "alpha"
					, material.opacity
					, ( checkFlag( flags.texturesFlags, TextureFlag::eOpacity )
						&& flags.alphaFunc != VK_COMPARE_OP_ALWAYS ) );
				auto alphaRef = writer.declLocale( "alphaRef"
					, material.alphaRef
					, flags.alphaFunc != VK_COMPARE_OP_ALWAYS );
				auto lightMat = lightingModel->declMaterial( "lightMat"
					, m_needsRsm );
				lightMat->create( in.colour
					, material );

				if ( hasTextures )
				{
					auto texCoord0 = writer.declLocale( "texCoord0"
						, in.texture0 );
					auto texCoord1 = writer.declLocale( "texCoord1"
						, in.texture1 );
					auto texCoord2 = writer.declLocale( "texCoord2"
						, in.texture2 );
					auto texCoord3 = writer.declLocale( "texCoord3"
						, in.texture3 );

					if ( m_needsRsm )
					{
						auto occlusion = writer.declLocale( "occlusion"
							, 1.0_f );
						auto transmittance = writer.declLocale( "transmittance"
							, 0.0_f );
						lightingModel->computeMapContributions( flags.passFlags
							, flags.textures
							, textureConfigs
							, textureAnims
							, c3d_maps
							, modelData.getTextures0()
							, modelData.getTextures1()
							, texCoord0
							, texCoord1
							, texCoord2
							, texCoord3
							, normal
							, tangent
							, bitangent
							, emissive
							, alpha
							, occlusion
							, transmittance
							, *lightMat
							, in.tangentSpaceViewPosition
							, in.tangentSpaceFragPosition );
					}
					else
					{
						textureConfigs.computeGeometryMapContributions( utils
							, flags.passFlags
							, flags.textures
							, textureAnims
							, c3d_maps
							, modelData.getTextures0()
							, modelData.getTextures1()
							, texCoord0
							, texCoord1
							, texCoord2
							, texCoord3
							, alpha
							, in.tangentSpaceViewPosition
							, in.tangentSpaceFragPosition );
					}
				}

				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, alphaRef );

				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
				pxl_linear = depth;

				if ( m_needsVsm )
				{
					pxl_variance.x() = depth;
					pxl_variance.y() = depth * depth;

					auto dx = writer.declLocale( "dx"
						, dFdx( depth ) );
					auto dy = writer.declLocale( "dy"
						, dFdy( depth ) );
					pxl_variance.y() += 0.25_f * ( dx * dx + dy * dy );
				}

				if ( m_needsRsm )
				{
					auto lightDiffuse = writer.declLocale( "lightDiffuse"
						, vec3( 0.0_f ) );
					auto lightSpecular = writer.declLocale( "lightSpecular"
						, vec3( 0.0_f ) );
					shader::OutputComponents output{ lightDiffuse, lightSpecular };
					auto light = writer.declLocale( "light"
						, c3d_shadowMapData.getDirectionalLight( *lightingModel ) );
					pxl_flux.rgb() = lightMat->albedo
						* light.base.colour
						* light.base.intensity.x();
					pxl_normal.xyz() = normal;
					pxl_position.xyz() = in.worldPosition.xyz();
				}
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
