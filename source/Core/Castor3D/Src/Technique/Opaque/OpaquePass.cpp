#include "OpaquePass.hpp"

#include "LightPass.hpp"

#include "Engine.hpp"
#include "Cache/MaterialCache.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Shader/Program.hpp"
#include "Technique/Opaque/GeometryPassResult.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureView.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslMaterial.hpp"

#include <RenderPass/RenderPassCreateInfo.hpp>

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
		auto & device = getCurrentDevice( *this );
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].depthStencilAttachment = { 0u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };

		renderPass.attachments.resize( gpResult.getViews().size() );
		renderPass.attachments[0].format = gpResult.getViews()[0]->getFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		for ( size_t i = 1u; i < gpResult.getViews().size(); ++i )
		{
			renderPass.attachments[i].format = gpResult.getViews()[i]->getFormat();
			renderPass.attachments[i].loadOp = renderer::AttachmentLoadOp::eClear;
			renderPass.attachments[i].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[i].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[i].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			renderPass.attachments[i].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[i].initialLayout = renderer::ImageLayout::eUndefined;
			renderPass.attachments[i].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

			renderPass.subpasses[0].colorAttachments.push_back( { uint32_t( i ), renderer::ImageLayout::eColourAttachmentOptimal } );
		}

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );
		renderer::FrameBufferAttachmentArray attaches;

		for ( size_t i = 0u; i < gpResult.getViews().size(); ++i )
		{
			attaches.emplace_back( *( m_renderPass->getAttachments().begin() + i ), *gpResult.getViews()[i] );
		}

		m_frameBuffer = m_renderPass->createFrameBuffer( { gpResult[0].getDimensions().width, gpResult[0].getDimensions().height }
			, std::move( attaches ) );
		m_nodesCommands = device.getGraphicsCommandPool().createCommandBuffer();
	}

	void OpaquePass::accept( RenderTechniqueVisitor & visitor )
	{
		auto shaderProgram = getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, visitor.getPassFlags()
			, visitor.getTextureFlags()
			, ProgramFlags{}
			, visitor.getSceneFlags()
			, visitor.getAlphaFunc()
			, false );
		visitor.visit( cuT( "Object" )
			, renderer::ShaderStageFlag::eVertex
			, shaderProgram->getSource( renderer::ShaderStageFlag::eVertex ) );
		visitor.visit( cuT( "Object" )
			, renderer::ShaderStageFlag::eFragment
			, shaderProgram->getSource( renderer::ShaderStageFlag::eFragment ) );
	}

	void OpaquePass::update( RenderInfo & info
		, Point2r const & jitter )
	{
		auto & nodes = m_renderQueue.getCulledRenderNodes();

		if ( nodes.hasNodes() )
		{
			auto & camera = *m_camera;

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

	renderer::Semaphore const & OpaquePass::render( renderer::Semaphore const & toWait )
	{
		static renderer::ClearValueArray const clearValues
		{
			renderer::DepthStencilClearValue{ 1.0, 0 },
			renderer::ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f },
			renderer::ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f },
			renderer::ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f },
			renderer::ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f },
			renderer::ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f },
		};

		auto * result = &toWait;
		auto & timer = getTimer();
		auto & device = getCurrentDevice( *this );
		auto timerBlock = timer.start();

		m_nodesCommands->begin();
		timer.beginPass( *m_nodesCommands );
		timer.notifyPassRender();
		m_nodesCommands->beginRenderPass( getRenderPass()
			, *m_frameBuffer
			, clearValues
			, renderer::SubpassContents::eSecondaryCommandBuffers );
		m_nodesCommands->executeCommands( { getCommandBuffer() } );
		m_nodesCommands->endRenderPass();
		timer.endPass( *m_nodesCommands );
		m_nodesCommands->end();

		device.getGraphicsQueue().submit( *m_nodesCommands
			, *result
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, getSemaphore()
			, nullptr );
		result = &getSemaphore();

		return *result;
	}

	renderer::DescriptorSetLayoutBindingArray OpaquePass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		renderer::DescriptorSetLayoutBindingArray uboBindings;

		if ( !checkFlag( flags.programFlags, ProgramFlag::eDepthPass ) )
		{
			uboBindings.emplace_back( getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding() );
		}

		uboBindings.emplace_back( MatrixUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment );
		uboBindings.emplace_back( SceneUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment );

		if ( !checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
		{
			uboBindings.emplace_back( ModelMatrixUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex );
		}

		uboBindings.emplace_back( ModelUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment );

		if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
		{
			uboBindings.push_back( SkinningUbo::createLayoutBinding( SkinningUbo::BindingPoint, flags.programFlags ) );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
		{
			uboBindings.emplace_back( MorphingUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex );
		}

		if ( checkFlag( flags.programFlags, ProgramFlag::eBillboards ) )
		{
			uboBindings.emplace_back( BillboardUbo::BindingPoint, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex );
		}

		return uboBindings;
	}

	renderer::DescriptorSetLayoutBindingArray OpaquePass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		auto index = MinBufferIndex;
		renderer::DescriptorSetLayoutBindingArray textureBindings;

		if ( checkFlag( flags.textureFlags, TextureChannel::eDiffuse ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eSpecular ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eGloss ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eNormal ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		bool opacityMap = checkFlag( flags.textureFlags, TextureChannel::eOpacity )
			&& flags.alphaFunc != renderer::CompareOp::eAlways;

		if ( opacityMap )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eHeight ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eAmbientOcclusion ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eEmissive ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		if ( checkFlag( flags.textureFlags, TextureChannel::eTransmittance ) )
		{
			textureBindings.emplace_back( index++, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		return textureBindings;
	}

	void OpaquePass::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		remFlag( programFlags, ProgramFlag::eLighting );
	}

	glsl::Shader OpaquePass::doGetVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		REQUIRE( ( checkFlag( programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2 );
		using namespace glsl;
		auto writer = getEngine()->getRenderSystem()->createGlslWriter();
		// Vertex inputs
		auto position = writer.declAttribute< Vec4 >( cuT( "position" )
			, RenderPass::VertexInputs::PositionLocation );
		auto normal = writer.declAttribute< Vec3 >( cuT( "normal" )
			, RenderPass::VertexInputs::NormalLocation );
		auto tangent = writer.declAttribute< Vec3 >( cuT( "tangent" )
			, RenderPass::VertexInputs::TangentLocation );
		auto texture = writer.declAttribute< Vec3 >( cuT( "texcoord" )
			, RenderPass::VertexInputs::TextureLocation );
		auto bone_ids0 = writer.declAttribute< IVec4 >( cuT( "bone_ids0" )
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto bone_ids1 = writer.declAttribute< IVec4 >( cuT( "bone_ids1" )
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights0 = writer.declAttribute< Vec4 >( cuT( "weights0" )
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto weights1 = writer.declAttribute< Vec4 >( cuT( "weights1" )
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( programFlags, ProgramFlag::eSkinning ) );
		auto transform = writer.declAttribute< Mat4 >( cuT( "transform" )
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto material = writer.declAttribute< Int >( cuT( "material" )
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( programFlags, ProgramFlag::eInstantiation ) );
		auto position2 = writer.declAttribute< Vec4 >( cuT( "position2" )
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto normal2 = writer.declAttribute< Vec3 >( cuT( "normal2" )
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto tangent2 = writer.declAttribute< Vec3 >( cuT( "tangent2" )
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto texture2 = writer.declAttribute< Vec3 >( cuT( "texture2" )
			, RenderPass::VertexInputs::Texture2Location
			, checkFlag( programFlags, ProgramFlag::eMorphing ) );
		auto gl_InstanceID( writer.declBuiltin< Int >( writer.getInstanceID() ) );

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0u, programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0u, programFlags );

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
		auto vtx_instance = writer.declOutput< Int >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declOutput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = gl_PerVertex{ writer };

		std::function< void() > main = [&]()
		{
			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vec4( position.xyz(), 1.0 ) );
			auto v4Normal = writer.declLocale( cuT( "v4Normal" )
				, vec4( normal, 0.0 ) );
			auto v4Tangent = writer.declLocale( cuT( "v4Tangent" )
				, vec4( tangent, 0.0 ) );
			auto v3Texture = writer.declLocale( cuT( "v3Texture" )
				, texture );

			if ( checkFlag( programFlags, ProgramFlag::eSkinning ) )
			{
				auto curMtxModel = writer.declLocale( cuT( "curMtxModel" )
					, SkinningUbo::computeTransform( writer, programFlags ) );
				auto prvMtxModel = writer.declLocale( cuT( "prvMtxModel" )
					, curMtxModel );
			}
			else if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
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

			auto curMtxModel = writer.declBuiltin< Mat4 >( cuT( "curMtxModel" ) );
			auto prvMtxModel = writer.declBuiltin< Mat4 >( cuT( "prvMtxModel" ) );
			auto mtxNormal = writer.declLocale( cuT( "mtxNormal" )
				, transpose( inverse( mat3( curMtxModel ) ) ) );

			if ( checkFlag( programFlags, ProgramFlag::eInstantiation ) )
			{
				vtx_material = material;
			}
			else
			{
				vtx_material = c3d_materialIndex;
			}

			if ( checkFlag( programFlags, ProgramFlag::eMorphing ) )
			{
				curPosition = vec4( glsl::mix( curPosition.xyz(), position2.xyz(), c3d_time ), 1.0 );
				v4Normal = vec4( glsl::mix( v4Normal.xyz(), normal2.xyz(), c3d_time ), 1.0 );
				v4Tangent = vec4( glsl::mix( v4Tangent.xyz(), tangent2.xyz(), c3d_time ), 1.0 );
				v3Texture = glsl::mix( v3Texture, texture2, c3d_time );
			}

			vtx_texture = v3Texture;
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, prvMtxModel * curPosition );
			curPosition = curMtxModel * curPosition;
			vtx_worldPosition = curPosition.xyz();
			prvPosition = c3d_prvViewProj * prvPosition;
			curPosition = c3d_curViewProj * curPosition;

			if ( invertNormals )
			{
				vtx_normal = normalize( mtxNormal * -v4Normal.xyz() );
			}
			else
			{
				vtx_normal = normalize( mtxNormal * v4Normal.xyz() );
			}

			vtx_tangent = normalize( mtxNormal * v4Tangent.xyz() );
			vtx_tangent = normalize( glsl::fma( -vtx_normal, vec3( dot( vtx_tangent, vtx_normal ) ), vtx_tangent ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );
			vtx_instance = gl_InstanceID;

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
			out.gl_Position() = curPosition;

			vtx_curPosition = curPosition.xyw();
			vtx_prvPosition = prvPosition.xyw();
			// Positions in projection space are in [-1, 1] range, while texture
			// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
			// the scale (and flip the y axis):
			vtx_curPosition.xy() *= vec2( 0.5_f, -0.5 );
			vtx_prvPosition.xy() *= vec2( 0.5_f, -0.5 );
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}

	glsl::Shader OpaquePass::doGetLegacyPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		shader::LegacyMaterials materials{ writer };
		materials.declare();

		// UBOs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );

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
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );

		auto index = MinBufferIndex;
		auto c3d_mapDiffuse( writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" )
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapSpecular( writer.declSampler< Sampler2D >( cuT( "c3d_mapSpecular" )
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapGloss( writer.declSampler< Sampler2D >( cuT( "c3d_mapGloss" )
			, checkFlag( textureFlags, TextureChannel::eGloss ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapNormal( writer.declSampler< Sampler2D >( cuT( "c3d_mapNormal" )
			, checkFlag( textureFlags, TextureChannel::eNormal ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		bool opacityMap = checkFlag( textureFlags, TextureChannel::eOpacity )
			&& alphaFunc != renderer::CompareOp::eAlways;
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, opacityMap ? index++ : 0u
			, 1u
			, opacityMap ) );
		auto c3d_mapHeight( writer.declSampler< Sampler2D >( cuT( "c3d_mapHeight" )
			, checkFlag( textureFlags, TextureChannel::eHeight ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declSampler< Sampler2D >( cuT( "c3d_mapAmbientOcclusion" )
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declSampler< Sampler2D >( cuT( "c3d_mapEmissive" )
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapTransmittance( writer.declSampler< Sampler2D >( cuT( "c3d_mapTransmittance" )
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ) );
		auto c3d_heightScale( writer.declConstant< Float >( cuT( "c3d_heightScale" )
			, 0.1_f
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		index = 0u;
		auto out_c3dOutput1 = writer.declFragData< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.declFragData< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.declFragData< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.declFragData< Vec4 >( OpaquePass::Output4, index++ );
		auto out_c3dOutput5 = writer.declFragData< Vec4 >( OpaquePass::Output5, index++ );

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );
		declareEncodeMaterial( writer );
		glsl::Utils utils{ writer };
		utils.declareRemoveGamma();

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceViewPosition - vtx_tangentSpaceFragPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			if ( alphaFunc != renderer::CompareOp::eAlways )
			{
				auto alpha = writer.declLocale( cuT( "alpha" )
					, material.m_opacity() );

				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material.m_alphaRef() );
			}

			auto gamma = writer.declLocale( cuT( "gamma" )
				, material.m_gamma() );
			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto diffuse = writer.declLocale( cuT( "diffuse" )
				, utils.removeGamma( gamma, material.m_diffuse() ) );
			auto specular = writer.declLocale( cuT( "specular" )
				, material.m_specular() );
			auto matShininess = writer.declLocale( cuT( "matShininess" )
				, material.m_shininess() );
			auto emissive = writer.declLocale( cuT( "emissive" )
				, vec3( material.m_emissive() ) );
			shader::legacy::computePreLightingMapContributions( writer
				, normal
				, matShininess
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
			shader::legacy::computePostLightingMapContributions( writer
				, diffuse
				, specular
				, emissive
				, gamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto flags = writer.declLocale( cuT( "flags" ), 0.0_f );
			encodeMaterial( writer
				, c3d_shadowReceiver
				, checkFlag( textureFlags, TextureChannel::eReflection ) ? 1_i : 0_i
				, checkFlag( textureFlags, TextureChannel::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, flags );

			auto ambientOcclusion = writer.declLocale( cuT( "ambientOcclusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				ambientOcclusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, 0.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eTransmittance ) )
			{
				transmittance = texture( c3d_mapTransmittance, texCoord.xy() ).r();
			}

			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			out_c3dOutput1 = vec4( normal, flags );
			out_c3dOutput2 = vec4( diffuse, matShininess );
			out_c3dOutput3 = vec4( specular, ambientOcclusion );
			out_c3dOutput4 = vec4( emissive, transmittance );
			out_c3dOutput5 = vec4( curPosition - prvPosition, writer.cast< Float >( vtx_material ), 0.0_f );
		} );

		return writer.finalise();
	}

	glsl::Shader OpaquePass::doGetPbrMRPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		shader::PbrMRMaterials materials{ writer };
		materials.declare();

		// UBOs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );

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
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );

		auto index = MinBufferIndex;
		auto c3d_mapAlbedo( writer.declSampler< Sampler2D >( cuT( "c3d_mapAlbedo" )
			, checkFlag( textureFlags, TextureChannel::eAlbedo ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eAlbedo ) ) );
		auto c3d_mapRoughness( writer.declSampler< Sampler2D >( cuT( "c3d_mapRoughness" )
			, checkFlag( textureFlags, TextureChannel::eRoughness ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eRoughness ) ) );
		auto c3d_mapMetallic( writer.declSampler< Sampler2D >( cuT( "c3d_mapMetallic" )
			, checkFlag( textureFlags, TextureChannel::eMetallic ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eMetallic ) ) );
		auto c3d_mapNormal( writer.declSampler< Sampler2D >( cuT( "c3d_mapNormal" )
			, checkFlag( textureFlags, TextureChannel::eNormal ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		bool opacityMap = checkFlag( textureFlags, TextureChannel::eOpacity )
			&& alphaFunc != renderer::CompareOp::eAlways;
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, opacityMap ? index++ : 0u
			, 1u
			, opacityMap ) );
		auto c3d_mapHeight( writer.declSampler< Sampler2D >( cuT( "c3d_mapHeight" )
			, checkFlag( textureFlags, TextureChannel::eHeight ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declSampler< Sampler2D >( cuT( "c3d_mapAmbientOcclusion" )
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declSampler< Sampler2D >( cuT( "c3d_mapEmissive" )
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapTransmittance( writer.declSampler< Sampler2D >( cuT( "c3d_mapTransmittance" )
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ) );
		auto c3d_heightScale( writer.declConstant< Float >( cuT( "c3d_heightScale" )
			, 0.1_f
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		index = 0u;
		auto out_c3dOutput1 = writer.declFragData< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.declFragData< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.declFragData< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.declFragData< Vec4 >( OpaquePass::Output4, index++ );
		auto out_c3dOutput5 = writer.declFragData< Vec4 >( OpaquePass::Output5, index++ );

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );
		declareEncodeMaterial( writer );
		glsl::Utils utils{ writer };
		utils.declareRemoveGamma();

		if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			utils.declareGetMapNormal();
		}

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceViewPosition - vtx_tangentSpaceFragPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			if ( alphaFunc != renderer::CompareOp::eAlways )
			{
				auto alpha = writer.declLocale( cuT( "alpha" )
					, material.m_opacity() );

				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material.m_alphaRef() );
			}

			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto matAlbedo = writer.declLocale( cuT( "matAlbedo" )
				, material.m_albedo() );
			auto matRoughness = writer.declLocale( cuT( "matRoughness" )
				, material.m_roughness() );
			auto matMetallic = writer.declLocale( cuT( "matMetallic" )
				, material.m_metallic() );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" )
				, vec3( material.m_emissive() ) );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );
			shader::pbr::mr::computePreLightingMapContributions( writer
				, normal
				, matMetallic
				, matRoughness
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
			shader::pbr::mr::computePostLightingMapContributions( writer
				, matAlbedo
				, matEmissive
				, matGamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto flags = writer.declLocale( cuT( "flags" ), 0.0_f );
			encodeMaterial( writer
				, c3d_shadowReceiver
				, checkFlag( textureFlags, TextureChannel::eReflection ) ? 1_i : 0_i
				, checkFlag( textureFlags, TextureChannel::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, flags );
			
			auto ambientOcclusion = writer.declLocale( cuT( "ambientOcclusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				ambientOcclusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, 0.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eTransmittance ) )
			{
				transmittance = texture( c3d_mapTransmittance, texCoord.xy() ).r();
			}

			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			out_c3dOutput1 = vec4( normal, flags );
			out_c3dOutput2 = vec4( matAlbedo, 0.0_f );
			out_c3dOutput3 = vec4( matMetallic, matRoughness, 0.0_f, ambientOcclusion );
			out_c3dOutput4 = vec4( matEmissive, transmittance );
			out_c3dOutput5 = vec4( curPosition - prvPosition, writer.cast< Float >( vtx_material ), 0.0_f );
		} );

		return writer.finalise();
	}

	glsl::Shader OpaquePass::doGetPbrSGPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// UBOs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );

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
		auto vtx_instance = writer.declInput< Int >( cuT( "vtx_instance" )
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::PbrSGMaterials materials{ writer };
		materials.declare();

		auto index = MinBufferIndex;
		auto c3d_mapDiffuse( writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" )
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapSpecular( writer.declSampler< Sampler2D >( cuT( "c3d_mapSpecular" )
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapGlossiness( writer.declSampler< Sampler2D >( cuT( "c3d_mapGloss" )
			, checkFlag( textureFlags, TextureChannel::eGloss ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eGloss ) ) );
		auto c3d_mapNormal( writer.declSampler< Sampler2D >( cuT( "c3d_mapNormal" )
			, checkFlag( textureFlags, TextureChannel::eNormal ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eNormal ) ) );
		bool opacityMap = checkFlag( textureFlags, TextureChannel::eOpacity )
			&& alphaFunc != renderer::CompareOp::eAlways;
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, opacityMap ? index++ : 0u
			, 1u
			, opacityMap ) );
		auto c3d_mapHeight( writer.declSampler< Sampler2D >( cuT( "c3d_mapHeight" )
			, checkFlag( textureFlags, TextureChannel::eHeight ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapAmbientOcclusion( writer.declSampler< Sampler2D >( cuT( "c3d_mapAmbientOcclusion" )
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) ) );
		auto c3d_mapEmissive( writer.declSampler< Sampler2D >( cuT( "c3d_mapEmissive" )
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapTransmittance( writer.declSampler< Sampler2D >( cuT( "c3d_mapTransmittance" )
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ? index++ : 0u
			, 1u
			, checkFlag( textureFlags, TextureChannel::eTransmittance ) ) );
		auto c3d_heightScale( writer.declConstant< Float >( cuT( "c3d_heightScale" )
			, 0.1_f
			, checkFlag( textureFlags, TextureChannel::eHeight ) ) );

		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		index = 0u;
		auto out_c3dOutput1 = writer.declFragData< Vec4 >( OpaquePass::Output1, index++ );
		auto out_c3dOutput2 = writer.declFragData< Vec4 >( OpaquePass::Output2, index++ );
		auto out_c3dOutput3 = writer.declFragData< Vec4 >( OpaquePass::Output3, index++ );
		auto out_c3dOutput4 = writer.declFragData< Vec4 >( OpaquePass::Output4, index++ );
		auto out_c3dOutput5 = writer.declFragData< Vec4 >( OpaquePass::Output5, index++ );

		auto parallaxMapping = shader::declareParallaxMappingFunc( writer, textureFlags, programFlags );
		declareEncodeMaterial( writer );
		glsl::Utils utils{ writer };
		utils.declareRemoveGamma();

		if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
		{
			utils.declareGetMapNormal();
		}

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			auto material = writer.declLocale( cuT( "material" )
				, materials.getMaterial( vtx_material ) );
			auto texCoord = writer.declLocale( cuT( "texCoord" )
				, vtx_texture );

			if ( checkFlag( textureFlags, TextureChannel::eHeight )
				&& checkFlag( textureFlags, TextureChannel::eNormal )
				&& checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
			{
				auto viewDir = writer.declLocale( cuT( "viewDir" )
					, normalize( vtx_tangentSpaceViewPosition - vtx_tangentSpaceFragPosition ) );
				texCoord.xy() = parallaxMapping( texCoord.xy(), viewDir );
			}

			if ( alphaFunc != renderer::CompareOp::eAlways )
			{
				auto alpha = writer.declLocale( cuT( "alpha" )
					, material.m_opacity() );

				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, texCoord.xy() ).r();
				}

				shader::applyAlphaFunc( writer
					, alphaFunc
					, alpha
					, material.m_alphaRef() );
			}

			auto normal = writer.declLocale( cuT( "normal" )
				, normalize( vtx_normal ) );
			auto matDiffuse = writer.declLocale( cuT( "matDiffuse" )
				, material.m_diffuse() );
			auto matGlossiness = writer.declLocale( cuT( "matGlossiness" )
				, material.m_glossiness() );
			auto matSpecular = writer.declLocale( cuT( "matSpecular" )
				, material.m_specular() );
			auto matEmissive = writer.declLocale( cuT( "matEmissive" )
				, vec3( material.m_emissive() ) );
			auto matGamma = writer.declLocale( cuT( "matGamma" )
				, material.m_gamma() );

			shader::pbr::sg::computePreLightingMapContributions( writer
				, normal
				, matSpecular
				, matGlossiness
				, textureFlags
				, programFlags
				, sceneFlags
				, passFlags );
			shader::pbr::sg::computePostLightingMapContributions( writer
				, matDiffuse
				, matEmissive
				, matGamma
				, textureFlags
				, programFlags
				, sceneFlags );
			auto flags = writer.declLocale( cuT( "flags" ), 0.0_f );
			encodeMaterial( writer
				, c3d_shadowReceiver
				, checkFlag( textureFlags, TextureChannel::eReflection ) ? 1_i : 0_i
				, checkFlag( textureFlags, TextureChannel::eRefraction ) ? 1_i : 0_i
				, c3d_envMapIndex
				, flags );

			auto ambientOcclusion = writer.declLocale( cuT( "ambientOcclusion" )
				, 1.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eAmbientOcclusion ) )
			{
				ambientOcclusion = texture( c3d_mapAmbientOcclusion, texCoord.xy() ).r();
			}

			auto transmittance = writer.declLocale( cuT( "transmittance" )
				, 0.0_f );

			if ( checkFlag( textureFlags, TextureChannel::eTransmittance ) )
			{
				transmittance = texture( c3d_mapTransmittance, texCoord.xy() ).r();
			}

			auto curPosition = writer.declLocale( cuT( "curPosition" )
				, vtx_curPosition.xy() / vtx_curPosition.z() ); // w is stored in z
			auto prvPosition = writer.declLocale( cuT( "prvPosition" )
				, vtx_prvPosition.xy() / vtx_prvPosition.z() );
			out_c3dOutput1 = vec4( normal, flags );
			out_c3dOutput2 = vec4( matDiffuse, matGlossiness );
			out_c3dOutput3 = vec4( matSpecular, ambientOcclusion );
			out_c3dOutput4 = vec4( matEmissive, transmittance );
			out_c3dOutput5 = vec4( curPosition - prvPosition, writer.cast< Float >( vtx_material ), 0.0_f );
		} );

		return writer.finalise();
	}

	void OpaquePass::doUpdatePipeline( RenderPipeline & pipeline )const
	{
	}

	renderer::DepthStencilState OpaquePass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return renderer::DepthStencilState{ 0u, true, true };
	}

	renderer::ColourBlendState OpaquePass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 5u );
	}
}
