#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Node/SceneCulledRenderNodes.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;
using namespace castor3d;

#define C3D_DebugGeometryShaders 0

namespace castor3d
{
	String const OpaquePass::Output1 = "outData1";
	String const OpaquePass::Output2 = "outData2";
	String const OpaquePass::Output3 = "outData3";
	String const OpaquePass::Output4 = "outData4";
	String const OpaquePass::Output5 = "outData5";

	OpaquePass::OpaquePass( MatrixUbo & matrixUbo
		, SceneCuller & culler
		, SsaoConfig const & config )
		: castor3d::RenderTechniquePass{ cuT( "Opaque" )
			, cuT( "Geometry pass" )
			, matrixUbo
			, culler
			, false
			, nullptr
			, config }
	{
	}

	OpaquePass::~OpaquePass()
	{
	}

	void OpaquePass::initialiseRenderPass( RenderDevice const & device
		, OpaquePassResult const & gpResult )
	{
		ashes::VkAttachmentDescriptionArray attachments
		{
			{
				0u,
				gpResult[DsTexture::eDepth].getTexture()->getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_LOAD,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			}
		};
		ashes::VkAttachmentReferenceArray colorAttachments;

		// Colour attachments.
		for ( auto index = 1u; index < uint32_t( DsTexture::eCount ); ++index )
		{
			attachments.push_back(
				{
					0u,
					gpResult[DsTexture( index )].getTexture()->getPixelFormat(),
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				} );
			colorAttachments.push_back( { uint32_t( index ), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
		}
		
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				std::move( colorAttachments ),
				{},
				VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			}
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attachments ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = device->createRenderPass( "OpaquePass"
			, std::move( createInfo ) );

		ashes::ImageViewCRefArray attaches;

		for ( auto & unit : gpResult )
		{
			attaches.emplace_back( unit->getTexture()->getDefaultView().getTargetView() );
		}

		m_frameBuffer = m_renderPass->createFrameBuffer( "OpaquePass"
			, {
				gpResult[DsTexture::eDepth].getTexture()->getWidth(),
				gpResult[DsTexture::eDepth].getTexture()->getHeight(),
			}
			, std::move( attaches ) );
		m_nodesCommands = device.graphicsCommandPool->createCommandBuffer( "OpaquePass" );
	}

	void OpaquePass::accept( RenderTechniqueVisitor & visitor )
	{
		auto & flags = visitor.getFlags();
		auto shaderProgram = getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, flags );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ) );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ) );
	}

	ashes::Semaphore const & OpaquePass::render( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		static ashes::VkClearValueArray const clearValues
		{
			defaultClearDepthStencil,
			transparentBlackClearColor,
			transparentBlackClearColor,
			transparentBlackClearColor,
			transparentBlackClearColor,
			transparentBlackClearColor,
		};

		auto * result = &toWait;
		auto timerBlock = getTimer().start();

		m_nodesCommands->begin();
		m_nodesCommands->beginDebugBlock(
			{
				"Deferred - Geometry",
				makeFloatArray( getEngine()->getNextRainbowColour() ),
			} );
		timerBlock->beginPass( *m_nodesCommands );
		timerBlock->notifyPassRender();
		m_nodesCommands->beginRenderPass( getRenderPass()
			, *m_frameBuffer
			, clearValues
			, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );

		if ( m_renderQueue.hasNodes() )
		{
			m_nodesCommands->executeCommands( { getCommandBuffer() } );
		}

		m_nodesCommands->endRenderPass();
		timerBlock->endPass( *m_nodesCommands );
		m_nodesCommands->endDebugBlock();
		m_nodesCommands->end();

		device.graphicsQueue->submit( *m_nodesCommands
			, *result
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, getSemaphore()
			, nullptr );
		result = &getSemaphore();

		return *result;
	}

	TextureFlags OpaquePass::getTexturesMask()const
	{
		return TextureFlags{ TextureFlag::eAll };
	}

	void OpaquePass::doUpdateFlags( PipelineFlags & flags )const
	{
		remFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.sceneFlags, SceneFlag::eLpvGI );
		remFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI );
		remFlag( flags.sceneFlags, SceneFlag::eVoxelConeTracing );
	}

	void OpaquePass::doUpdatePipeline( RenderPipeline & pipeline )
	{
	}

	ashes::PipelineColorBlendStateCreateInfo OpaquePass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return SceneRenderPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 5u );
	}

	void OpaquePass::doFillTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		node.passNode.fillDescriptor( layout
			, index
			, *node.texDescriptorSet
			, pipeline.getFlags().textures );
	}

	void OpaquePass::doFillTextureDescriptor( RenderPipeline const & pipeline
		, ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		ashes::WriteDescriptorSetArray writes;
		node.passNode.fillDescriptor( layout
			, index
			, writes
			, pipeline.getFlags().textures );
		node.texDescriptorSet->setBindings( writes );
	}

	ashes::VkDescriptorSetLayoutBindingArray OpaquePass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = 0u;
		ashes::VkDescriptorSetLayoutBindingArray textureBindings;

		if ( !flags.textures.empty() )
		{
			textureBindings.emplace_back( makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, uint32_t( flags.textures.size() ) ) );
			index += uint32_t( flags.textures.size() );
		}

		return textureBindings;
	}

	ShaderPtr OpaquePass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
#if C3D_DebugGeometryShaders
		using namespace sdw;
		GeometryWriter writer;
		bool hasTextures = !flags.textures.empty();
		writer.inputLayout( ast::stmt::InputLayout::eTriangleList );
		writer.outputLayout( ast::stmt::OutputLayout::eTriangleStrip, 3u );

		auto in = writer.getIn();
		auto out = writer.getOut();

		// Geometry Inputs
		auto inWorldPosition = writer.declInputArray< Vec3 >( "inWorldPosition"
			, SceneRenderPass::VertexOutputs::WorldPositionLocation, 3u );
		auto inCurPosition = writer.declInputArray< Vec3 >( "inCurPosition"
			, SceneRenderPass::VertexOutputs::CurPositionLocation, 3u );
		auto inPrvPosition = writer.declInputArray< Vec3 >( "inPrvPosition"
			, SceneRenderPass::VertexOutputs::PrvPositionLocation, 3u );
		auto inTangentSpaceFragPosition = writer.declInputArray< Vec3 >( "inTangentSpaceFragPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation, 3u );
		auto inTangentSpaceViewPosition = writer.declInputArray< Vec3 >( "inTangentSpaceViewPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation, 3u );
		auto inNormal = writer.declInputArray< Vec3 >( "inNormal"
			, SceneRenderPass::VertexOutputs::NormalLocation, 3u );
		auto inTangent = writer.declInputArray< Vec3 >( "inTangent"
			, SceneRenderPass::VertexOutputs::TangentLocation, 3u );
		auto inBitangent = writer.declInputArray< Vec3 >( "inBitangent"
			, SceneRenderPass::VertexOutputs::BitangentLocation, 3u );
		auto inTexture = writer.declInputArray< Vec3 >( "inTexture"
			, SceneRenderPass::VertexOutputs::TextureLocation, 3u
			, hasTextures );
		auto inInstance = writer.declInputArray< UInt >( "inInstance"
			, SceneRenderPass::VertexOutputs::InstanceLocation, 3u );
		auto inMaterial = writer.declInputArray< UInt >( "inMaterial"
			, SceneRenderPass::VertexOutputs::MaterialLocation, 3u );

		// Geometry Outputs
		auto outWorldPosition = writer.declOutput< Vec3 >( "outWorldPosition"
			, SceneRenderPass::VertexOutputs::WorldPositionLocation );
		auto outCurPosition = writer.declOutput< Vec3 >( "outCurPosition"
			, SceneRenderPass::VertexOutputs::CurPositionLocation );
		auto outPrvPosition = writer.declOutput< Vec3 >( "outPrvPosition"
			, SceneRenderPass::VertexOutputs::PrvPositionLocation );
		auto outTangentSpaceFragPosition = writer.declOutput< Vec3 >( "outTangentSpaceFragPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto outTangentSpaceViewPosition = writer.declOutput< Vec3 >( "outTangentSpaceViewPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto outNormal = writer.declOutput< Vec3 >( "outNormal"
			, SceneRenderPass::VertexOutputs::NormalLocation );
		auto outTangent = writer.declOutput< Vec3 >( "outTangent"
			, SceneRenderPass::VertexOutputs::TangentLocation );
		auto outBitangent = writer.declOutput< Vec3 >( "outBitangent"
			, SceneRenderPass::VertexOutputs::BitangentLocation );
		auto outTexture = writer.declOutput< Vec3 >( "outTexture"
			, SceneRenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto outInstance = writer.declOutput< UInt >( "outInstance"
			, SceneRenderPass::VertexOutputs::InstanceLocation );
		auto Material = writer.declOutput< UInt >( "Material"
			, SceneRenderPass::VertexOutputs::MaterialLocation );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				for ( uint32_t i = 0; i < 3u; ++i )
				{
					outWorldPosition = inWorldPosition[i];
					outCurPosition = inCurPosition[i];
					outPrvPosition = inPrvPosition[i];
					outTangentSpaceFragPosition = inTangentSpaceFragPosition[i];
					outTangentSpaceViewPosition = inTangentSpaceViewPosition[i];
					outNormal = inNormal[i];
					outTangent = inTangent[i];
					outBitangent = inBitangent[i];
					outTexture = inTexture[i];
					outInstance = inInstance[i];
					Material = inMaterial[i];
					out.vtx.position = in.vtx[i].position;
					EmitVertex( writer );
				}

				EndPrimitive( writer );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
#else
		return nullptr;
#endif
	}

	ShaderPtr OpaquePass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;

		auto textures = filterTexturesFlags( flags.textures );
		auto & renderSystem = *getEngine()->getRenderSystem();
		shader::PhongMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials ) );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTexturesBuffer ) );
		}

		// UBOs
		UBO_SCENE( writer, uint32_t( NodeUboIdx::eScene ), 0u );
		UBO_MODEL( writer, uint32_t( NodeUboIdx::eModel ), 0u );
		UBO_TEXTURES( writer, uint32_t( NodeUboIdx::eTexturesConfig ), 0u, hasTextures );

		auto index = 0u;
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

		// Fragment Inputs
		auto inWorldPosition = writer.declInput< Vec3 >( "inWorldPosition"
			, SceneRenderPass::VertexOutputs::WorldPositionLocation );
		auto inCurPosition = writer.declInput< Vec3 >( "inCurPosition"
			, SceneRenderPass::VertexOutputs::CurPositionLocation );
		auto inPrvPosition = writer.declInput< Vec3 >( "inPrvPosition"
			, SceneRenderPass::VertexOutputs::PrvPositionLocation );
		auto inTangentSpaceFragPosition = writer.declInput< Vec3 >( "inTangentSpaceFragPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto inTangentSpaceViewPosition = writer.declInput< Vec3 >( "inTangentSpaceViewPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto inNormal = writer.declInput< Vec3 >( "inNormal"
			, SceneRenderPass::VertexOutputs::NormalLocation );
		auto inTangent = writer.declInput< Vec3 >( "inTangent"
			, SceneRenderPass::VertexOutputs::TangentLocation );
		auto inBitangent = writer.declInput< Vec3 >( "inBitangent"
			, SceneRenderPass::VertexOutputs::BitangentLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, SceneRenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto inInstance = writer.declInput< UInt >( "inInstance"
			, SceneRenderPass::VertexOutputs::InstanceLocation );
		auto inMaterial = writer.declInput< UInt >( "inMaterial"
			, SceneRenderPass::VertexOutputs::MaterialLocation );

		// Fragment Outputs
		index = 0u;
		auto outData1 = writer.declOutput< Vec4 >( OpaquePass::Output1, index++ );
		auto outData2 = writer.declOutput< Vec4 >( OpaquePass::Output2, index++ );
		auto outData3 = writer.declOutput< Vec4 >( OpaquePass::Output3, index++ );
		auto outData4 = writer.declOutput< Vec4 >( OpaquePass::Output4, index++ );
		auto outData5 = writer.declOutput< Vec4 >( OpaquePass::Output5, index++ );
		auto out = writer.getOut();

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		utils.declareEncodeMaterial();
		utils.declareParallaxMappingFunc( flags.passFlags
			, getTexturesMask() );

		shader::PhongLightingModel lightingModel{ writer
			, utils
			, {}
			, true };

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = writer.declLocale( "material"
					, materials.getMaterial( inMaterial ) );
				auto texCoord = writer.declLocale( "texCoord"
					, inTexture );
				auto alpha = writer.declLocale( "alpha"
					, material.m_opacity );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto normal = writer.declLocale( "normal"
					, normalize( inNormal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( inTangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( inBitangent ) );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, inTangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, inTangentSpaceFragPosition );
				auto diffuse = writer.declLocale( "diffuse"
					, utils.removeGamma( gamma, material.m_diffuse() ) );
				auto specular = writer.declLocale( "specular"
					, material.m_specular );
				auto shininess = writer.declLocale( "shininess"
					, material.m_shininess );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.m_emissive ) );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );

				if ( hasTextures )
				{
					lightingModel.computeMapContributions( flags.passFlags
						, textures
						, gamma
						, textureConfigs
						, c3d_textureConfig
						, c3d_maps
						, texCoord
						, normal
						, tangent
						, bitangent
						, emissive
						, alpha
						, occlusion
						, transmittance
						, diffuse
						, specular
						, shininess
						, tangentSpaceViewPosition
						, tangentSpaceFragPosition );
				}

				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, material.m_alphaRef );
				emissive *= diffuse;
				auto matFlags = writer.declLocale( "flags"
					, 0.0_f );
				utils.encodeMaterial( c3d_shadowReceiver
					, ( checkFlag( flags.passFlags, PassFlag::eReflection ) ) ? 1_i : 0_i
					, ( checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? 1_i : 0_i
					, ( checkFlag( flags.passFlags, PassFlag::eLighting ) ) ? 1_i : 0_i
					, c3d_envMapIndex
					, matFlags );

				auto curPosition = writer.declLocale( "curPosition"
					, inCurPosition.xy() / inCurPosition.z() ); // w is stored in z
				auto prvPosition = writer.declLocale( "prvPosition"
					, inPrvPosition.xy() / inPrvPosition.z() );
				outData1 = vec4( normal, matFlags );
				outData2 = vec4( diffuse, shininess );
				outData3 = vec4( specular, occlusion );
				outData4 = vec4( emissive, transmittance );
				outData5 = vec4( curPosition - prvPosition, writer.cast< Float >( inMaterial ), 0.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr OpaquePass::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;

		auto textures = filterTexturesFlags( flags.textures );
		auto & renderSystem = *getEngine()->getRenderSystem();
		shader::PbrMRMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials ) );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTexturesBuffer ) );
		}

		// UBOs
		UBO_SCENE( writer, uint32_t( NodeUboIdx::eScene ), 0u );
		UBO_MODEL( writer, uint32_t( NodeUboIdx::eModel ), 0u );
		UBO_TEXTURES( writer, uint32_t( NodeUboIdx::eTexturesConfig ), 0u, hasTextures );

		auto index = 0u;
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

		// Fragment Inputs
		auto inWorldPosition = writer.declInput< Vec3 >( "inWorldPosition"
			, SceneRenderPass::VertexOutputs::WorldPositionLocation );
		auto inCurPosition = writer.declInput< Vec3 >( "inCurPosition"
			, SceneRenderPass::VertexOutputs::CurPositionLocation );
		auto inPrvPosition = writer.declInput< Vec3 >( "inPrvPosition"
			, SceneRenderPass::VertexOutputs::PrvPositionLocation );
		auto inTangentSpaceFragPosition = writer.declInput< Vec3 >( "inTangentSpaceFragPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto inTangentSpaceViewPosition = writer.declInput< Vec3 >( "inTangentSpaceViewPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto inNormal = writer.declInput< Vec3 >( "inNormal"
			, SceneRenderPass::VertexOutputs::NormalLocation );
		auto inTangent = writer.declInput< Vec3 >( "inTangent"
			, SceneRenderPass::VertexOutputs::TangentLocation );
		auto inBitangent = writer.declInput< Vec3 >( "inBitangent"
			, SceneRenderPass::VertexOutputs::BitangentLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, SceneRenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto inInstance = writer.declInput< UInt >( "inInstance"
			, SceneRenderPass::VertexOutputs::InstanceLocation );
		auto inMaterial = writer.declInput< UInt >( "inMaterial"
			, SceneRenderPass::VertexOutputs::MaterialLocation );

		// Fragment Outputs
		index = 0u;
		auto outData1 = writer.declOutput< Vec4 >( OpaquePass::Output1, index++ );
		auto outData2 = writer.declOutput< Vec4 >( OpaquePass::Output2, index++ );
		auto outData3 = writer.declOutput< Vec4 >( OpaquePass::Output3, index++ );
		auto outData4 = writer.declOutput< Vec4 >( OpaquePass::Output4, index++ );
		auto outData5 = writer.declOutput< Vec4 >( OpaquePass::Output5, index++ );
		auto out = writer.getOut();

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		utils.declareEncodeMaterial();
		utils.declareParallaxMappingFunc( flags.passFlags
			, getTexturesMask() );

		shader::MetallicBrdfLightingModel lightingModel{ writer
			, utils
			, {}
			, true };

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = writer.declLocale( "material"
					, materials.getMaterial( inMaterial ) );
				auto texCoord = writer.declLocale( "texCoord"
					, inTexture );
				auto alpha = writer.declLocale( "alpha"
					, material.m_opacity );
				auto normal = writer.declLocale( "normal"
					, normalize( inNormal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( inTangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( inBitangent ) );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, inTangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, inTangentSpaceFragPosition );
				auto albedo = writer.declLocale( "albedo"
					, material.m_albedo );
				auto roughness = writer.declLocale( "roughness"
					, material.m_roughness );
				auto metallic = writer.declLocale( "metallic"
					, material.m_metallic );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.m_emissive ) );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto occlusion = writer.declLocale( "ambientOcclusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );

				if ( hasTextures )
				{
					lightingModel.computeMapContributions( flags.passFlags
						, textures
						, gamma
						, textureConfigs
						, c3d_textureConfig
						, c3d_maps
						, texCoord
						, normal
						, tangent
						, bitangent
						, emissive
						, alpha
						, occlusion
						, transmittance
						, albedo
						, metallic
						, roughness
						, tangentSpaceViewPosition
						, tangentSpaceFragPosition );
				}

				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, material.m_alphaRef );
				emissive *= albedo;
				auto matFlags = writer.declLocale( "flags"
					, 0.0_f );
				utils.encodeMaterial( c3d_shadowReceiver
					, ( checkFlag( flags.passFlags, PassFlag::eReflection ) ) ? 1_i : 0_i
					, ( checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? 1_i : 0_i
					, ( checkFlag( flags.passFlags, PassFlag::eLighting ) ) ? 1_i : 0_i
					, c3d_envMapIndex
					, matFlags );

				auto curPosition = writer.declLocale( "curPosition"
					, inCurPosition.xy() / inCurPosition.z() ); // w is stored in z
				auto prvPosition = writer.declLocale( "prvPosition"
					, inPrvPosition.xy() / inPrvPosition.z() );
				outData1 = vec4( normal, matFlags );
				outData2 = vec4( albedo, roughness );
				outData3 = vec4( metallic, 0.0_f, 0.0_f, occlusion );
				outData4 = vec4( emissive, transmittance );
				outData5 = vec4( curPosition - prvPosition, writer.cast< Float >( inMaterial ), 0.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr OpaquePass::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		auto textures = filterTexturesFlags( flags.textures );
		shader::PbrSGMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
			, uint32_t( NodeUboIdx::eMaterials ) );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( NodeUboIdx::eTexturesBuffer ) );
		}

		// UBOs
		UBO_SCENE( writer, uint32_t( NodeUboIdx::eScene ), 0u );
		UBO_MODEL( writer, uint32_t( NodeUboIdx::eModel ), 0u );
		UBO_TEXTURES( writer, uint32_t( NodeUboIdx::eTexturesConfig ), 0u, hasTextures );

		auto index = 0u;
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

		// Fragment Inputs
		auto inWorldPosition = writer.declInput< Vec3 >( "inWorldPosition"
			, SceneRenderPass::VertexOutputs::WorldPositionLocation );
		auto inCurPosition = writer.declInput< Vec3 >( "inCurPosition"
			, SceneRenderPass::VertexOutputs::CurPositionLocation );
		auto inPrvPosition = writer.declInput< Vec3 >( "inPrvPosition"
			, SceneRenderPass::VertexOutputs::PrvPositionLocation );
		auto inTangentSpaceFragPosition = writer.declInput< Vec3 >( "inTangentSpaceFragPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto inTangentSpaceViewPosition = writer.declInput< Vec3 >( "inTangentSpaceViewPosition"
			, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto inNormal = writer.declInput< Vec3 >( "inNormal"
			, SceneRenderPass::VertexOutputs::NormalLocation );
		auto inTangent = writer.declInput< Vec3 >( "inTangent"
			, SceneRenderPass::VertexOutputs::TangentLocation );
		auto inBitangent = writer.declInput< Vec3 >( "inBitangent"
			, SceneRenderPass::VertexOutputs::BitangentLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, SceneRenderPass::VertexOutputs::TextureLocation
			, hasTextures );
		auto inInstance = writer.declInput< UInt >( "inInstance"
			, SceneRenderPass::VertexOutputs::InstanceLocation );
		auto inMaterial = writer.declInput< UInt >( "inMaterial"
			, SceneRenderPass::VertexOutputs::MaterialLocation );

		// Fragment Outputs
		index = 0u;
		auto outData1 = writer.declOutput< Vec4 >( OpaquePass::Output1, index++ );
		auto outData2 = writer.declOutput< Vec4 >( OpaquePass::Output2, index++ );
		auto outData3 = writer.declOutput< Vec4 >( OpaquePass::Output3, index++ );
		auto outData4 = writer.declOutput< Vec4 >( OpaquePass::Output4, index++ );
		auto outData5 = writer.declOutput< Vec4 >( OpaquePass::Output5, index++ );
		auto out = writer.getOut();

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		utils.declareEncodeMaterial();
		utils.declareParallaxMappingFunc( flags.passFlags
			, getTexturesMask() );

		shader::SpecularBrdfLightingModel lightingModel{ writer
			, utils
			, {}
			, true };

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = writer.declLocale( "material"
					, materials.getMaterial( inMaterial ) );
				auto texCoord = writer.declLocale( "texCoord"
					, inTexture );
				auto alpha = writer.declLocale( "alpha"
					, material.m_opacity );
				auto normal = writer.declLocale( "normal"
					, normalize( inNormal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( inTangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( inBitangent ) );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, inTangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, inTangentSpaceFragPosition );
				auto albedo = writer.declLocale( "albedo"
					, material.m_diffuse() );
				auto glossiness = writer.declLocale( "glossiness"
					, material.m_glossiness );
				auto specular = writer.declLocale( "specular"
					, material.m_specular );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.m_emissive ) );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto occlusion = writer.declLocale( "ambientOcclusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );

				if ( hasTextures )
				{
					lightingModel.computeMapContributions( flags.passFlags
						, textures
						, gamma
						, textureConfigs
						, c3d_textureConfig
						, c3d_maps
						, texCoord
						, normal
						, tangent
						, bitangent
						, emissive
						, alpha
						, occlusion
						, transmittance
						, albedo
						, specular
						, glossiness
						, tangentSpaceViewPosition
						, tangentSpaceFragPosition );
				}

				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, material.m_alphaRef );
				emissive *= albedo;
				auto matFlags = writer.declLocale( "flags"
					, 0.0_f );
				utils.encodeMaterial( c3d_shadowReceiver
					, ( checkFlag( flags.passFlags, PassFlag::eReflection ) ) ? 1_i : 0_i
					, ( checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? 1_i : 0_i
					, ( checkFlag( flags.passFlags, PassFlag::eLighting ) ) ? 1_i : 0_i
					, c3d_envMapIndex
					, matFlags );

				auto curPosition = writer.declLocale( "curPosition"
					, inCurPosition.xy() / inCurPosition.z() ); // w is stored in z
				auto prvPosition = writer.declLocale( "prvPosition"
					, inPrvPosition.xy() / inPrvPosition.z() );
				outData1 = vec4( normal, matFlags );
				outData2 = vec4( albedo, glossiness );
				outData3 = vec4( specular, occlusion );
				outData4 = vec4( emissive, transmittance );
				outData5 = vec4( curPosition - prvPosition, writer.cast< Float >( inMaterial ), 0.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
