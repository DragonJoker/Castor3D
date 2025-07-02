#include "Castor3D/Render/ShadowMap/ShadowMapPassDirectional.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"
#include "Castor3D/Render/RenderTechniquePass.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
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
	castor::String const ShadowMapPassDirectional::Type = cuT( "c3d.shadows.directional" );

	ShadowMapPassDirectional::ShadowMapPassDirectional( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, SceneCuller & culler
		, Camera & camera
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
		, m_camera{ camera }
	{
		log::trace << "Created " << getName() << std::endl;
	}

	ShadowMapPassDirectional::~ShadowMapPassDirectional()noexcept
	{
		m_camera.detach();
	}

	void ShadowMapPassDirectional::doUpdate( CpuUpdater & updater )
	{
		if ( auto sceneIt = updater.dirtyScenes.find( &getScene() );
			sceneIt != updater.dirtyScenes.end() )
		{
			auto & sceneObjs = sceneIt->second;
			auto it = std::find( sceneObjs.dirtyCameras.begin()
				, sceneObjs.dirtyCameras.end()
				, &m_camera );

			if ( it == sceneObjs.dirtyCameras.end() )
			{
				it = std::find( sceneObjs.dirtyCameras.begin()
					, sceneObjs.dirtyCameras.end()
					, updater.camera );
			}

			m_outOfDate = m_outOfDate
				|| it != sceneObjs.dirtyCameras.end();
		}

		getCuller().update( updater );
		m_outOfDate = m_outOfDate
			|| getCuller().areAnyChanged();
		RenderNodesPass::doUpdate( updater );
	}

	void ShadowMapPassDirectional::doUpdateUbos( CpuUpdater & updater )
	{
		RenderNodesPass::doUpdateUbos( updater );
		m_shadowMapUbo.update( *updater.light, updater.index );
	}

	ashes::PipelineDepthStencilStateCreateInfo ShadowMapPassDirectional::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_TRUE, VK_COMPARE_OP_GREATER };
	}

	ashes::PipelineColorBlendStateCreateInfo ShadowMapPassDirectional::doCreateBlendState( PipelineFlags const & flags )const
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

	ProgramFlags ShadowMapPassDirectional::doAdjustProgramFlags( ProgramFlags flags )const
	{
		return flags;
	}

	void ShadowMapPassDirectional::doGetSubmeshShaderSource( PipelineFlags const & flags
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
		auto index = uint32_t( castor3d::GlobalBuffersIdx::eCount ) + flags.submeshDataBindings + 1u;
		C3D_ShadowMap( writer
			, index++
			, RenderPipeline::eBuffers );
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
			, [&engine, &writer, &meshBuffers, &materials, &c3d_modelsData, c3d_shadowMapData, &c3d_objectIdsData, &pipelineID, &drawID, &flags]( sdw::VertexIn const & in
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
						, vec3( 0.0_f )
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
						, vec3( 0.0_f )
						, worldPos.xyz()
						, mtxNormal
						, curNormal
						, curTangent
						, curBitangent );
				}

				auto worldPos = writer.getVariable< sdw::Vec4 >( "worldPos" );
				out.worldPosition = worldPos;
				curPosition = c3d_shadowMapData.worldToView( worldPos );
				out.vtx.position = c3d_shadowMapData.viewToProj( curPosition );
			} );
	}

	void ShadowMapPassDirectional::doGetPixelShaderSource( PipelineFlags const & flags
		, ast::ShaderBuilder & builder )const
	{
		sdw::FragmentWriter writer{ builder };
		auto enableTextures = flags.enableTextures();
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
		auto index = uint32_t( castor3d::GlobalBuffersIdx::eCount ) + flags.submeshDataBindings;
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
			, LightType::eDirectional
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
				auto depth = writer.declLocale( "depth"
					, in.fragCoord.z() );
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
						, c3d_shadowMapData.getDirectionalLight( lights ) );
					components.baseColour *= in.colour;
					out.flux.rgb() = components.baseColour
						* light.base().colour()
						* light.base().intensity();
					out.normal.xyz() = components.getRawNormal();
					out.position.xyz() = in.worldPosition.xyz();
				}
			} );
	}
}
