#include "Castor3D/Render/Prepass/VisibilityPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputs.hpp"
#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( castor3d, VisibilityPass )

namespace castor3d
{
	//*********************************************************************************************

	castor::String const VisibilityPass::Type = "c3d.visibility";

	VisibilityPass::VisibilityPass( RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, crg::ImageViewIdArray targetDepth
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: RenderTechniqueNodesPass{ parent
			, pass
			, context
			, graph
			, device
			, Type
			, {}
			, std::move( targetDepth )
			, renderPassDesc
			, techniquePassDesc }
	{
	}

	void VisibilityPass::accept( RenderTechniqueVisitor & visitor )
	{
		doAccept( visitor );
	}

	ShaderFlags VisibilityPass::getShaderFlags()const
	{
		return ShaderFlag::eWorldSpace
			| ShaderFlag::eTangentSpace
			| ShaderFlag::eOpacity
			| ShaderFlag::eVelocity
			| ShaderFlag::eVisibility
			| ShaderFlag::eNormal
			| ShaderFlag::eColour;
	}

	void VisibilityPass::doFillAdditionalBindings( PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings )const
	{
	}

	void VisibilityPass::doFillAdditionalDescriptor( PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, ShadowMapLightTypeArray const & shadowMaps
		, ShadowBuffer const * shadowBuffer )
	{
	}

	ashes::PipelineDepthStencilStateCreateInfo VisibilityPass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u
			, VK_TRUE
			, VK_TRUE
			, ( flags.passLayerIndex > 0
				? VK_COMPARE_OP_GREATER_OR_EQUAL
				: VK_COMPARE_OP_GREATER ) };
	}

	ashes::PipelineColorBlendStateCreateInfo VisibilityPass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderNodesPass::createBlendState( BlendMode::eNoBlend
			, BlendMode::eNoBlend
			, 4u );
	}

	void VisibilityPass::doGetPixelShaderSource( PipelineFlags const & flags
		, ast::ShaderBuilder & builder )const
	{
		sdw::FragmentWriter writer{ builder };
		bool enableTextures = flags.enableTextures();

		shader::Utils utils{ writer };
		shader::BRDFHelpers brdf{ writer };
		shader::PassShaders passShaders{ getEngine()->getPassComponentsRegister()
			, flags
			, ( ComponentModeFlag::eOpacity
				| ComponentModeFlag::eNormals
				| ComponentModeFlag::eGeometry )
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
			, RenderPipeline::eBuffers };
		shader::TextureConfigurations textureConfigs{ writer
			, uint32_t( GlobalBuffersIdx::eTexConfigs )
			, RenderPipeline::eBuffers
			, enableTextures };
		shader::TextureAnimations textureAnims{ writer
			, uint32_t( GlobalBuffersIdx::eTexAnims )
			, RenderPipeline::eBuffers
			, enableTextures };

		auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
			, 0u
			, RenderPipeline::eTextures
			, enableTextures ) );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();
		auto constexpr maxPipelinesSize = uint32_t( castor::getBitSize( MaxPipelines ) );

		shader::Lights lights{ *getEngine()
			, flags.lightingModelId
			, flags.backgroundModelId
			, materials
			, brdf
			, utils
			, {}
			, nullptr };

		writer.implementMainT< shader::FragmentSurfaceT, shader::PrepassOutputT >( sdw::FragmentInT< shader::FragmentSurfaceT >{ writer, submeshShaders, passShaders, flags }
			, sdw::FragmentOutT< shader::PrepassOutputT >{ writer, flags }
			, [&]( sdw::FragmentInT< shader::FragmentSurfaceT > in
				, sdw::FragmentOutT< shader::PrepassOutputT > out )
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

				if ( components.transmission )
				{
					auto incident = writer.declLocale( "incident"
						, normalize( in.worldPosition.xyz() - c3d_cameraData.position() ) );

					IF( writer, components.transmission >= 0.1_f )
					{
						writer.demote();
					}
					FI;
				}

				out.depthObj = vec4( in.fragCoord.z()
					, length( in.worldPosition.xyz() - c3d_cameraData.position() )
					, writer.cast< sdw::Float >( in.nodeId )
					, writer.cast< sdw::Float >( material.lightingModel ) );

				if ( flags.isBillboard() )
				{
					out.visibility = uvec2( ( in.nodeId << maxPipelinesSize ) | ( pipelineID )
						, in.vertexId * 2_u + writer.cast< sdw::UInt >( in.primitiveID ) );
				}
				else if ( isMeshShading() )
				{
					auto constexpr maxPrimitiveIDSize = uint32_t( castor::getBitSize( MaxMeshletTriangleCount ) );
					out.visibility = uvec2( ( in.nodeId << maxPipelinesSize ) | ( pipelineID )
						, ( in.meshletId << maxPrimitiveIDSize ) | writer.cast< sdw::UInt >( in.primitiveID ) );
				}
				else
				{
					out.visibility = uvec2( ( in.nodeId << maxPipelinesSize ) | ( pipelineID )
						, writer.cast< sdw::UInt >( in.primitiveID ) );
				}

				out.velocity = in.getVelocity();
				out.nmlOcc = vec4( components.normal, components.occlusion );
			} );
	}
}
