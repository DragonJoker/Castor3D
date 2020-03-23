#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"

#include "Castor3D/Render/Technique/Opaque/LightPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Node/SceneCulledRenderNodes.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"
#include "Castor3D/Render/Technique/Opaque/GeometryPassResult.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	String const OpaquePass::Output1 = cuT( "c3d_output1" );
	String const OpaquePass::Output2 = cuT( "c3d_output2" );
	String const OpaquePass::Output3 = cuT( "c3d_output3" );
	String const OpaquePass::Output4 = cuT( "c3d_output4" );
	String const OpaquePass::Output5 = cuT( "c3d_output5" );

	OpaquePass::OpaquePass( MatrixUbo const & matrixUbo
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

	void OpaquePass::initialiseRenderPass( GeometryPassResult const & gpResult )
	{
		ashes::VkAttachmentDescriptionArray attachments
		{
			{
				0u,
				gpResult.getViews()[0]->format,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			}
		};
		ashes::VkAttachmentReferenceArray colorAttachments;

		// Colour attachments.
		for ( size_t i = 1u; i < gpResult.getViews().size(); ++i )
		{
			attachments.push_back(
				{
					0u,
					gpResult.getViews()[i]->format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				} );
			colorAttachments.push_back( { uint32_t( i ), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
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
		auto & device = getCurrentRenderDevice( *this );
		m_renderPass = device->createRenderPass( std::move( createInfo ) );

		ashes::ImageViewCRefArray attaches;

		for ( size_t i = 0u; i < gpResult.getViews().size(); ++i )
		{
			attaches.emplace_back( gpResult.getViews()[i] );
		}

		m_frameBuffer = m_renderPass->createFrameBuffer( { gpResult[0].getDimensions().width, gpResult[0].getDimensions().height }
			, std::move( attaches ) );
		m_nodesCommands = device.graphicsCommandPool->createCommandBuffer();
	}

	void OpaquePass::accept( RenderTechniqueVisitor & visitor )
	{
		auto & flags = visitor.getFlags();
		auto shaderProgram = getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, flags );
		visitor.visit( cuT( "Object" )
			, VK_SHADER_STAGE_VERTEX_BIT
			, *shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ).shader );
		visitor.visit( cuT( "Object" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ).shader );
	}

	void OpaquePass::update( RenderInfo & info
		, castor::Point2f const & jitter )
	{
		auto & nodes = m_renderQueue.getCulledRenderNodes();

		if ( nodes.hasNodes() )
		{
			RenderPass::doUpdate( nodes.instancedStaticNodes.frontCulled );
			RenderPass::doUpdate( nodes.staticNodes.frontCulled );
			RenderPass::doUpdate( nodes.skinnedNodes.frontCulled );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.frontCulled );
			RenderPass::doUpdate( nodes.morphingNodes.frontCulled );
			RenderPass::doUpdate( nodes.billboardNodes.frontCulled );

			RenderPass::doUpdate( nodes.instancedStaticNodes.backCulled, info );
			RenderPass::doUpdate( nodes.staticNodes.backCulled, info );
			RenderPass::doUpdate( nodes.skinnedNodes.backCulled, info );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled, info );
			RenderPass::doUpdate( nodes.morphingNodes.backCulled, info );
			RenderPass::doUpdate( nodes.billboardNodes.backCulled, info );
		}
	}

	ashes::Semaphore const & OpaquePass::render( ashes::Semaphore const & toWait )
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
		auto & timer = getTimer();
		auto & device = getCurrentRenderDevice( *this );
		auto timerBlock = timer.start();

		m_nodesCommands->begin();
		m_nodesCommands->beginDebugBlock(
			{
				"Deferred - Geometry",
				{
					opaqueBlackClearColor.color.float32[0],
					opaqueBlackClearColor.color.float32[1],
					opaqueBlackClearColor.color.float32[2],
					opaqueBlackClearColor.color.float32[3],
				},
			} );
		timer.beginPass( *m_nodesCommands );
		timer.notifyPassRender();
		m_nodesCommands->beginRenderPass( getRenderPass()
			, *m_frameBuffer
			, clearValues
			, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
		m_nodesCommands->executeCommands( { getCommandBuffer() } );
		m_nodesCommands->endRenderPass();
		timer.endPass( *m_nodesCommands );
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

	ashes::VkDescriptorSetLayoutBindingArray OpaquePass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		return RenderPass::doCreateUboBindings( flags );
	}

	void OpaquePass::doUpdateFlags( PipelineFlags & flags )const
	{
		remFlag( flags.programFlags, ProgramFlag::eLighting );
	}

	void OpaquePass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
	}

	ashes::PipelineDepthStencilStateCreateInfo OpaquePass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo{ 0u, true, true };
	}

	ashes::PipelineColorBlendStateCreateInfo OpaquePass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 5u );
	}

	ShaderPtr OpaquePass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		CU_Require( ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2 );
		using namespace sdw;
		VertexWriter writer;
		// Vertex inputs
		auto position = writer.declInput< Vec4 >( cuT( "position" )
			, RenderPass::VertexInputs::PositionLocation );
		auto normal = writer.declInput< Vec3 >( cuT( "normal" )
			, RenderPass::VertexInputs::NormalLocation );
		auto tangent = writer.declInput< Vec3 >( cuT( "tangent" )
			, RenderPass::VertexInputs::TangentLocation );
		auto texture = writer.declInput< Vec3 >( cuT( "uv" )
			, RenderPass::VertexInputs::TextureLocation );
		auto bone_ids0 = writer.declInput< IVec4 >( cuT( "bone_ids0" )
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declInput< IVec4 >( cuT( "bone_ids1" )
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declInput< Vec4 >( cuT( "weights0" )
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declInput< Vec4 >( cuT( "weights1" )
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declInput< Mat4 >( cuT( "transform" )
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declInput< Int >( cuT( "material" )
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declInput< Vec4 >( cuT( "position2" )
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declInput< Vec3 >( cuT( "normal2" )
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declInput< Vec3 >( cuT( "tangent2" )
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declInput< Vec3 >( cuT( "texture2" )
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto in = writer.getIn();

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0u, flags.programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0u, flags.programFlags );

		// Outputs
		auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" )
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_curPosition = writer.declOutput< Vec3 >( cuT( "vtx_curPosition" )
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declOutput< Vec3 >( cuT( "vtx_prvPosition" )
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declOutput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" )
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declOutput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" )
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declOutput< Vec3 >( cuT( "vtx_normal" )
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declOutput< Vec3 >( cuT( "vtx_tangent" )
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declOutput< Vec3 >( cuT( "vtx_bitangent" )
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declOutput< UInt >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declOutput< UInt >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		std::function< void() > main = [&]()
		{
			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vec4( position.xyz(), 1.0_f ) );
			auto v4Normal = writer.declLocale( cuT( "v4Normal" )
				, vec4( normal, 0.0_f ) );
			auto v4Tangent = writer.declLocale( cuT( "v4Tangent" )
				, vec4( tangent, 0.0_f ) );
			auto v3Texture = writer.declLocale( cuT( "v3Texture" )
				, texture );

			if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
			{
				auto curMtxModel = writer.declLocale( cuT( "curMtxModel" )
					, SkinningUbo::computeTransform( skinningData, writer, flags.programFlags ) );
				auto prvMtxModel = writer.declLocale( cuT( "prvMtxModel" )
					, curMtxModel );
			}
			else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				auto curMtxModel = writer.declLocale( cuT( "curMtxModel" )
					, transform );
				auto prvMtxModel = writer.declLocale( cuT( "prvMtxModel" )
					, curMtxModel );
			}
			else
			{
				auto curMtxModel = writer.declLocale( cuT( "curMtxModel" )
					, c3d_curMtxModel );
				auto prvMtxModel = writer.declLocale( cuT( "prvMtxModel" )
					, c3d_prvMtxModel );
			}

			auto curMtxModel = writer.getVariable< Mat4 >( cuT( "curMtxModel" ) );
			auto prvMtxModel = writer.getVariable< Mat4 >( cuT( "prvMtxModel" ) );
			auto mtxNormal = writer.declLocale( cuT( "mtxNormal" )
				, transpose( inverse( mat3( curMtxModel ) ) ) );

			if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				vtx_material = writer.cast< UInt >( material );
			}
			else
			{
				vtx_material = writer.cast< UInt >( c3d_materialIndex );
			}

			if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
			{
				curPosition = vec4( sdw::mix( curPosition.xyz(), position2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v4Normal = vec4( sdw::mix( v4Normal.xyz(), normal2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v4Tangent = vec4( sdw::mix( v4Tangent.xyz(), tangent2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v3Texture = sdw::mix( v3Texture, texture2, vec3( c3d_time ) );
			}

			vtx_texture = v3Texture;
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, prvMtxModel * curPosition );
			curPosition = curMtxModel * curPosition;
			vtx_worldPosition = curPosition.xyz();
			prvPosition = c3d_prvViewProj * prvPosition;
			curPosition = c3d_curViewProj * curPosition;

			if ( checkFlag( flags.programFlags, ProgramFlag::eInvertNormals ) )
			{
				vtx_normal = normalize( mtxNormal * -v4Normal.xyz() );
			}
			else
			{
				vtx_normal = normalize( mtxNormal * v4Normal.xyz() );
			}

			vtx_tangent = normalize( mtxNormal * v4Tangent.xyz() );
			vtx_tangent = normalize( sdw::fma( -vtx_normal, vec3( dot( vtx_tangent, vtx_normal ) ), vtx_tangent ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = writer.cast< UInt >( in.gl_InstanceID );

			auto tbn = writer.declLocale( cuT( "tbn" )
				, transpose( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) ) );
			vtx_tangentSpaceFragPosition = tbn * vtx_worldPosition;
			vtx_tangentSpaceViewPosition = tbn * c3d_cameraPosition.xyz();

			// Convert the jitter from non-homogeneous coordiantes to homogeneous
			// coordinates and add it:
			// (note that for providing the jitter in non-homogeneous projection space,
			//  pixel coordinates (screen space) need to multiplied by two in the C++
			//  code)
			curPosition.xy() -= c3d_jitter * curPosition.w();
			prvPosition.xy() -= c3d_jitter * prvPosition.w();
			out.gl_out.gl_Position = curPosition;

			vtx_curPosition = curPosition.xyw();
			vtx_prvPosition = prvPosition.xyw();
			// Positions in projection space are in [-1, 1] range, while texture
			// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
			// the scale (and flip the y axis):
			vtx_curPosition.xy() *= vec2( 0.5_f, -0.5f );
			vtx_prvPosition.xy() *= vec2( 0.5_f, -0.5f );
		};

		writer.implementFunction< Void >( cuT( "main" ), main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr OpaquePass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;

		auto & renderSystem = *getEngine()->getRenderSystem();
		shader::LegacyMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = flags.texturesCount > 0;

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		// UBOs
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		// Fragment Inputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" )
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_curPosition = writer.declInput< Vec3 >( cuT( "vtx_curPosition" )
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( cuT( "vtx_prvPosition" )
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" )
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" )
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( cuT( "vtx_normal" )
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( cuT( "vtx_tangent" )
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( cuT( "vtx_bitangent" )
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< UInt >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );

		auto out = writer.getOut();

		// Fragment Outputs
		index = 0u;
		auto out_c3dOutput1 = writer.declOutput< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.declOutput< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.declOutput< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.declOutput< Vec4 >( OpaquePass::Output4, index++ );
		auto out_c3dOutput5 = writer.declOutput< Vec4 >( OpaquePass::Output5, index++ );

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		utils.declareEncodeMaterial();
		utils.declareParallaxMappingFunc( flags );

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );
			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity );
			auto gamma = writer.declLocale( cuT( "gamma" )
				, material.m_gamma );
			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto diffuse = writer.declLocale( cuT( "diffuse" )
				, utils.removeGamma( gamma, material.m_diffuse() ) );
			auto specular = writer.declLocale( cuT( "specular" )
				, material.m_specular );
			auto shininess = writer.declLocale( cuT( "shininess" )
				, material.m_shininess );
			auto emissive = writer.declLocale( cuT( "emissive" )
				, vec3( material.m_emissive ) );
			auto occlusion = writer.declLocale( cuT( "occlusion" )
				, 1.0_f );
			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, 0.0_f );

			utils.computeHeightMapContribution( flags
				, textureConfigs
				, c3d_textureConfig
				, vtx_tangentSpaceViewPosition
				, vtx_tangentSpaceFragPosition
				, c3d_maps
				, texCoord );
			shader::PhongLightingModel::computeMapContributions( writer
				, utils
				, flags
				, gamma
				, textureConfigs
				, c3d_textureConfig
				, vtx_tangent
				, vtx_bitangent
				, c3d_maps
				, texCoord
				, normal
				, diffuse
				, specular
				, emissive
				, shininess
				, alpha
				, occlusion
				, transmittance );
			utils.applyAlphaFunc( flags.alphaFunc
				, alpha
				, material.m_alphaRef );
			emissive *= diffuse;
			auto matFlags = writer.declLocale( cuT( "flags" ), 0.0_f );
			utils.encodeMaterial( c3d_shadowReceiver
				, checkFlag( flags.textures, TextureFlag::eReflection ) ? 1_i : 0_i
				, checkFlag( flags.textures, TextureFlag::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, matFlags );

			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			out_c3dOutput1 = vec4( normal, matFlags );
			out_c3dOutput2 = vec4( diffuse, shininess );
			out_c3dOutput3 = vec4( specular, occlusion );
			out_c3dOutput4 = vec4( emissive, transmittance );
			out_c3dOutput5 = vec4( curPosition - prvPosition, writer.cast< Float >( vtx_material ), 0.0_f );
		} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr OpaquePass::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;

		auto & renderSystem = *getEngine()->getRenderSystem();
		shader::PbrMRMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = flags.texturesCount > 0;

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		// UBOs
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		// Fragment Inputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" )
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_curPosition = writer.declInput< Vec3 >( cuT( "vtx_curPosition" )
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( cuT( "vtx_prvPosition" )
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" )
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" )
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( cuT( "vtx_normal" )
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( cuT( "vtx_tangent" )
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( cuT( "vtx_bitangent" )
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< UInt >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );

		auto out = writer.getOut();

		// Fragment Outputs
		index = 0u;
		auto out_c3dOutput1 = writer.declOutput< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.declOutput< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.declOutput< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.declOutput< Vec4 >( OpaquePass::Output4, index++ );
		auto out_c3dOutput5 = writer.declOutput< Vec4 >( OpaquePass::Output5, index++ );

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		utils.declareEncodeMaterial();
		utils.declareParallaxMappingFunc( flags );

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );
			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity );
			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto albedo = writer.declLocale( cuT( "albedo" )
				, material.m_albedo );
			auto roughness = writer.declLocale( cuT( "roughness" )
				, material.m_roughness );
			auto metallic = writer.declLocale( cuT( "metallic" )
				, material.m_metallic );
			auto emissive = writer.declLocale( cuT( "emissive" )
				, vec3( material.m_emissive ) );
			auto gamma = writer.declLocale( cuT( "gamma" )
				, material.m_gamma );
			auto occlusion = writer.declLocale( cuT( "ambientOcclusion" )
				, 1.0_f );
			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, 0.0_f );

			utils.computeHeightMapContribution( flags
				, textureConfigs
				, c3d_textureConfig
				, vtx_tangentSpaceViewPosition
				, vtx_tangentSpaceFragPosition
				, c3d_maps
				, texCoord );
			shader::MetallicBrdfLightingModel::computeMapContributions( writer
				, utils
				, flags
				, gamma
				, textureConfigs
				, c3d_textureConfig
				, vtx_tangent
				, vtx_bitangent
				, c3d_maps
				, texCoord
				, normal
				, albedo
				, metallic
				, emissive
				, roughness
				, alpha
				, occlusion
				, transmittance );
			utils.applyAlphaFunc( flags.alphaFunc
				, alpha
				, material.m_alphaRef );
			emissive *= albedo;
			auto matFlags = writer.declLocale( cuT( "flags" ), 0.0_f );
			utils.encodeMaterial( c3d_shadowReceiver
				, checkFlag( flags.textures, TextureFlag::eReflection ) ? 1_i : 0_i
				, checkFlag( flags.textures, TextureFlag::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, matFlags );
			
			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			out_c3dOutput1 = vec4( normal, matFlags );
			out_c3dOutput2 = vec4( albedo, 0.0_f );
			out_c3dOutput3 = vec4( metallic, roughness, 0.0_f, occlusion );
			out_c3dOutput4 = vec4( emissive, transmittance );
			out_c3dOutput5 = vec4( curPosition - prvPosition, writer.cast< Float >( vtx_material ), 0.0_f );
		} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr OpaquePass::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		shader::PbrSGMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = flags.texturesCount > 0;

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		// UBOs
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		// Fragment Inputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" )
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_curPosition = writer.declInput< Vec3 >( cuT( "vtx_curPosition" )
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( cuT( "vtx_prvPosition" )
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceFragPosition" )
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( cuT( "vtx_tangentSpaceViewPosition" )
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( cuT( "vtx_normal" )
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( cuT( "vtx_tangent" )
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( cuT( "vtx_bitangent" )
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< UInt >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, flags.texturesCount )
			, flags.texturesCount > 0u ) );

		auto out = writer.getOut();

		// Fragment Outputs
		index = 0u;
		auto out_c3dOutput1 = writer.declOutput< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.declOutput< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.declOutput< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.declOutput< Vec4 >( OpaquePass::Output4, index++ );
		auto out_c3dOutput5 = writer.declOutput< Vec4 >( OpaquePass::Output5, index++ );

		shader::Utils utils{ writer };
		utils.declareRemoveGamma();
		utils.declareEncodeMaterial();
		utils.declareParallaxMappingFunc( flags );

		writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
		{
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );
			auto alpha = writer.declLocale( cuT( "alpha" )
				, material.m_opacity );
			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto albedo = writer.declLocale( cuT( "albedo" )
				, material.m_diffuse() );
			auto glossiness = writer.declLocale( cuT( "glossiness" )
				, material.m_glossiness );
			auto specular = writer.declLocale( cuT( "specular" )
				, material.m_specular );
			auto emissive = writer.declLocale( cuT( "emissive" )
				, vec3( material.m_emissive ) );
			auto gamma = writer.declLocale( cuT( "gamma" )
				, material.m_gamma );
			auto occlusion = writer.declLocale( cuT( "ambientOcclusion" )
				, 1.0_f );
			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, 0.0_f );

			utils.computeHeightMapContribution( flags
				, textureConfigs
				, c3d_textureConfig
				, vtx_tangentSpaceViewPosition
				, vtx_tangentSpaceFragPosition
				, c3d_maps
				, texCoord );
			shader::SpecularBrdfLightingModel::computeMapContributions( writer
				, utils
				, flags
				, gamma
				, textureConfigs
				, c3d_textureConfig
				, vtx_tangent
				, vtx_bitangent
				, c3d_maps
				, texCoord
				, normal
				, albedo
				, specular
				, emissive
				, glossiness
				, alpha
				, occlusion
				, transmittance );
			utils.applyAlphaFunc( flags.alphaFunc
				, alpha
				, material.m_alphaRef );
			emissive *= albedo;
			auto matFlags = writer.declLocale( cuT( "flags" ), 0.0_f );
			utils.encodeMaterial( c3d_shadowReceiver
				, checkFlag( flags.textures, TextureFlag::eReflection ) ? 1_i : 0_i
				, checkFlag( flags.textures, TextureFlag::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, matFlags );

			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			out_c3dOutput1 = vec4( normal, matFlags );
			out_c3dOutput2 = vec4( albedo, glossiness );
			out_c3dOutput3 = vec4( specular, occlusion );
			out_c3dOutput4 = vec4( emissive, transmittance );
			out_c3dOutput5 = vec4( curPosition - prvPosition, writer.cast< Float >( vtx_material ), 0.0_f );
		} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
