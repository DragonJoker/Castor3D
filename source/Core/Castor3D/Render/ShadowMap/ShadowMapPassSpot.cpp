#include "Castor3D/Render/ShadowMap/ShadowMapPassSpot.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"
#include "Castor3D/Render/RenderTechniquePass.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include <Castor3D/Shader/Shaders/GlslMeshVertex.hpp>
#include "Castor3D/Shader/Shaders/GlslOutputs.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	String const ShadowMapPassSpot::Type = cuT( "c3d.shadows.spot" );

	ShadowMapPassSpot::ShadowMapPassSpot( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, SceneCuller & culler
		, ShadowMap const & shadowMap
		, bool needsVsm
		, bool needsRsm
		, bool isStatic )
		: ShadowMapPass{ pass
			, context
			, graph
			, device
			, Type
			, cameraUbo
			, culler
			, shadowMap
			, needsVsm
			, needsRsm
			, isStatic }
	{
		log::trace << "Created " << getName() << std::endl;
	}

	ShadowMapPassSpot::~ShadowMapPassSpot()noexcept
	{
		getCuller().getCamera().detach();
	}

	void ShadowMapPassSpot::doUpdate( CpuUpdater & updater )
	{
		getCuller().update( updater );
		m_outOfDate = m_outOfDate
			|| getCuller().areAnyChanged();
		RenderNodesPass::doUpdate( updater );
	}

	void ShadowMapPassSpot::doUpdateUbos( CpuUpdater & updater )
	{
		auto const & light = *updater.light;
		m_shadowType = light.getShadowType();
		m_shadowMapUbo.update( light
			, updater.index );
		auto angle = static_cast< SpotLight const & >( light.getCategory() ).getOuterCutOff().radians();

		if ( angle != m_angle )
		{
			m_outOfDate = true;
			m_angle = angle;
		}
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassSpot::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_TRUE, VK_COMPARE_OP_GREATER };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassSpot::doCreateBlendState( PipelineFlags const & flags )const
	{
		uint32_t result = 1u;
		auto needsVsm = flags.writeShadowVSM();
		auto needsRsm = flags.writeShadowRSM();

		if ( needsVsm )
		{
			++result;
		}

		if ( needsRsm )
		{
			result += 3;
		}

		return RenderNodesPass::createBlendState( BlendMode::eNoBlend
			, BlendMode::eNoBlend
			, result );
	}

	ProgramFlags ShadowMapPassSpot::doAdjustProgramFlags( ProgramFlags flags )const
	{
		return flags;
	}

	void ShadowMapPassSpot::doGetSubmeshShaderSource( PipelineFlags const & flags
		, ast::ShaderBuilder & builder )const
	{
		sdw::VertexWriter writer{ builder };

		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, ComponentModeFlag::eNone
			, utils };
		shader::SubmeshShaders submeshShaders{ getEngine()->getSubmeshComponentsRegister()
			, flags };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };
		shader::InstantiatedMeshBuffers meshBuffers{ writer
			, flags
			, uint32_t( MeshBuffersIdx::ePosition )
			, uint32_t( RenderPipeline::eMeshBuffers )
			, flags.vertexStride };

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		auto drawID = pcb.declMember< sdw::Int >( "drawID", !getEngine()->getRenderDevice()->hasDrawId() );
		pcb.end();
		
		Engine const & engine = *getEngine();
		writer.implementMainT< sdw::VoidT, shader::FragmentSurfaceT >( sdw::VertexIn{ writer }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer, submeshShaders, passShaders, flags }
			, [&engine, &writer, &meshBuffers, &materials, &c3d_modelsData, c3d_cameraData, &c3d_objectIdsData, &pipelineID, &drawID, &flags]( sdw::VertexIn const & in
				, sdw::VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto instanceId = writer.declLocale( "instanceId"
					, writer.cast< sdw::UInt >( in.instanceIndex )
						+ writer.cast< sdw::UInt >( engine.getRenderDevice()->hasDrawId() ? in.drawID : drawID ) );
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, meshBuffers.instances
						, pipelineID
						, instanceId
						, flags ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				auto vertexIndex = writer.declLocale( "vertexIndex"
					, writer.cast< sdw::UInt >( in.vertexIndex ) );

				auto curPosition = writer.declLocale( "curPosition"
					, meshBuffers.positions[vertexIndex].position );
				auto curNormal = writer.declLocale( "curNormal"
					, meshBuffers.normals[vertexIndex].xyz() );
				auto curTangent = writer.declLocale( "curTangent"
					, meshBuffers.tangents[vertexIndex] );
				auto curBitangent = writer.declLocale( "curBitangent"
					, meshBuffers.bitangents[vertexIndex].xyz() );
				out.texture0 = meshBuffers.textures0[vertexIndex].xyz();
				out.texture1 = meshBuffers.textures1[vertexIndex].xyz();
				out.texture2 = meshBuffers.textures2[vertexIndex].xyz();
				out.texture3 = meshBuffers.textures3[vertexIndex].xyz();
				out.colour = meshBuffers.colours[vertexIndex].xyz();

				out.nodeId = nodeId;
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				material.getPassMultipliers( flags
					, meshBuffers.passMasks[vertexIndex]
					, out.passMultipliers );

				auto mtxModel = writer.declLocale< sdw::Mat4 >( "mtxModel"
					, modelData.getModelMtx() );

				if ( flags.hasWorldPosInputs() )
				{
					auto worldPos = writer.declLocale( "worldPos"
						, curPosition );
					out.computeTangentSpace( flags
						, c3d_cameraData.getCurViewCenter()
						, worldPos.xyz()
						, curNormal
						, curTangent
						, curBitangent );
				}
				else
				{
					auto worldPos = writer.declLocale( "worldPos"
						, mtxModel * curPosition );
					auto mtxNormal = writer.declLocale< sdw::Mat3 >( "mtxNormal"
						, modelData.getNormalMtx( flags, mtxModel ) );
					out.computeTangentSpace( flags
						, c3d_cameraData.getCurViewCenter()
						, worldPos.xyz()
						, mtxNormal
						, curNormal
						, curTangent
						, curBitangent );
				}

				auto worldPos = writer.getVariable< sdw::Vec4 >( "worldPos" );

				if ( flags.writeShadowRSM() )
				{
					out.worldPosition = worldPos;
				}

				out.viewPosition = c3d_cameraData.worldToCurProj( worldPos );
				out.vtx.position = out.viewPosition;
			} );
	}

	void ShadowMapPassSpot::doGetPixelShaderSource( PipelineFlags const & flags
		, ast::ShaderBuilder & builder )const
	{
		sdw::FragmentWriter writer{ builder };
		bool enableTextures = flags.enableTextures();
		auto needsVsm = flags.writeShadowVSM();
		auto needsRsm = flags.writeShadowRSM();

		shader::Utils utils{ writer };
		shader::BRDFHelpers brdf{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, getComponentsMask()
			, utils };
		shader::SubmeshShaders submeshShaders{ getEngine()->getSubmeshComponentsRegister()
			, flags };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers
			, needsRsm || passShaders.enableOpacity() };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::eBuffers
			, enableTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, enableTextures };
		auto index = uint32_t( GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;
		auto lightsIndex = index;
		++index;
		C3D_ShadowMap( writer
			, index
			, RenderPipeline::eBuffers );
		++index;
		shader::Lights lights{ *getEngine()
			, flags.lightingModelId
			, flags.backgroundModelId
			, materials
			, brdf
			, utils
			, shader::ShadowOptions{}
			, nullptr /* sssProfiles */
			, nullptr /* sssDiffusionProfiles */
			, LightType::eSpot
			, lightsIndex /* lightBinding */
			, RenderPipeline::eBuffers /* lightSet */
			, index /* shadowMapBinding */
			, RenderPipeline::eBuffers /* shadowMapSet */
			, false /* enableVolumetric */ };

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, enableTextures ) );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< shader::FragmentSurfaceT, shader::ShadowsOutputT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer, submeshShaders, passShaders, flags }
			, sdw::FragmentOutT< shader::ShadowsOutputT >{ writer, needsVsm, needsRsm }
			, [&]( sdw::FragmentInT< shader::FragmentSurfaceT > const & in
				, sdw::FragmentOutT< shader::ShadowsOutputT > out )
			{
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[in.nodeId - 1u] );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				auto components = writer.declLocale( "components"
					, shader::BlendComponents{ materials
						, material
						, in } );
				materials.blendMaterials( flags
					, textureConfigs
					, textureAnims
					, c3d_maps
					, material
					, modelData.getMaterialId()
					, in.passMultipliers
					, components );

				auto csPosition = writer.declLocale( "csPosition"
					, in.viewPosition );
				csPosition.xyz() /= csPosition.w();
				auto depth = writer.declLocale( "depth"
					, csPosition.z() );
				out.linear = depth;

				if ( needsVsm )
				{
					out.variance.x() = depth;
					out.variance.y() = depth * depth;

					auto dx = writer.declLocale( "dx"
						, dFdx( depth ) );
					auto dy = writer.declLocale( "dy"
						, dFdy( depth ) );
					out.variance.y() += 0.25_f * ( dx * dx + dy * dy );
				}

				if ( needsRsm )
				{
					out.normal = vec4( 0.0_f );
					out.position = vec4( 0.0_f );
					out.flux = vec4( 0.0_f );
					auto light = writer.declLocale( "light"
						, c3d_shadowMapData.getSpotLight( lights ) );
					auto lightToVertex = writer.declLocale( "lightToVertex"
						, light.position() - in.worldPosition.xyz() );
					auto distance = writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto attenuation = writer.declLocale( "attenuation"
						, light.getAttenuationFactor( distance ) );
					auto L = writer.declLocale( "L"
						, lightToVertex / distance );
					auto spotFactor = writer.declLocale( "spotFactor"
						, dot( L, light.direction() ) );
					spotFactor = max( 0.0_f
						, sdw::fma( ( spotFactor - 1.0_f )
							, 1.0_f / ( 1.0_f - light.outerCutOffCos() )
							, 1.0_f ) );
					spotFactor = 1.0_f - step( spotFactor, 0.0_f );
					components.baseColour *= in.colour;
					out.flux.rgb() = ( components.baseColour
							* light.base().colour()
							* light.base().intensity()
							* clamp( dot( L, components.getRawNormal() ), 0.0_f, 1.0_f ) )
						* attenuation;
					out.normal.xyz() = components.getRawNormal();
					out.position.xyz() = in.worldPosition.xyz();
				}
			} );
	}
}
