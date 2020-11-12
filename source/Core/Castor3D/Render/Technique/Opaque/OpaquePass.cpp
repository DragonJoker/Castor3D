#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"

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
		m_nodesCommands->executeCommands( { getCommandBuffer() } );
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

	ashes::VkDescriptorSetLayoutBindingArray OpaquePass::doCreateUboBindings( PipelineFlags const & flags )const
	{
		return RenderPass::doCreateUboBindings( flags );
	}

	void OpaquePass::doUpdateFlags( PipelineFlags & flags )const
	{
		remFlag( flags.programFlags, ProgramFlag::eLighting );
	}

	void OpaquePass::doUpdatePipeline( RenderPipeline & pipeline )
	{
	}

	ashes::PipelineDepthStencilStateCreateInfo OpaquePass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
#if C3D_UseDepthPrepass
		return ashes::PipelineDepthStencilStateCreateInfo
		{
			0u
			, VK_TRUE
			, VK_FALSE
			, VK_COMPARE_OP_EQUAL
		};
#else
		return ashes::PipelineDepthStencilStateCreateInfo
		{
			0u
			, VK_TRUE
			, VK_TRUE
		};
#endif
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
		auto inPosition = writer.declInput< Vec4 >( "inPosition"
			, RenderPass::VertexInputs::PositionLocation );
		auto inNormal = writer.declInput< Vec3 >( "inNormal"
			, RenderPass::VertexInputs::NormalLocation );
		auto inTangent = writer.declInput< Vec3 >( "inTangent"
			, RenderPass::VertexInputs::TangentLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, RenderPass::VertexInputs::TextureLocation );
		auto inBoneIds0 = writer.declInput< IVec4 >( "inBoneIds0"
			, RenderPass::VertexInputs::BoneIds0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inBoneIds1 = writer.declInput< IVec4 >( "inBoneIds1"
			, RenderPass::VertexInputs::BoneIds1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inWeights0 = writer.declInput< Vec4 >( "inWeights0"
			, RenderPass::VertexInputs::Weights0Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inWeights1 = writer.declInput< Vec4 >( "inWeights1"
			, RenderPass::VertexInputs::Weights1Location
			, checkFlag( flags.programFlags, ProgramFlag::eSkinning ) );
		auto inTransform = writer.declInput< Mat4 >( "inTransform"
			, RenderPass::VertexInputs::TransformLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto inMaterial = writer.declInput< Int >( "inMaterial"
			, RenderPass::VertexInputs::MaterialLocation
			, checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) );
		auto inPosition2 = writer.declInput< Vec4 >( "inPosition2"
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inNormal2 = writer.declInput< Vec3 >( "inNormal2"
			, RenderPass::VertexInputs::Normal2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inTangent2 = writer.declInput< Vec3 >( "inTangent2"
			, RenderPass::VertexInputs::Tangent2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto inTexture2 = writer.declInput< Vec3 >( "inTexture2"
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
		auto outWorldPosition = writer.declOutput< Vec3 >( "outWorldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto outCurPosition = writer.declOutput< Vec3 >( "outCurPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto outPrvPosition = writer.declOutput< Vec3 >( "outPrvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto outTangentSpaceFragPosition = writer.declOutput< Vec3 >( "outTangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto outTangentSpaceViewPosition = writer.declOutput< Vec3 >( "outTangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto outNormal = writer.declOutput< Vec3 >( "outNormal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto outTangent = writer.declOutput< Vec3 >( "outTangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto outBitangent = writer.declOutput< Vec3 >( "outBitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto outTexture = writer.declOutput< Vec3 >( "outTexture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto outInstance = writer.declOutput< UInt >( "outInstance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto outMaterial = writer.declOutput< UInt >( "outMaterial"
			, RenderPass::VertexOutputs::MaterialLocation );
		auto out = writer.getOut();

		std::function< void() > main = [&]()
		{
			auto curPosition = writer.declLocale( "curPosition"
				, vec4( inPosition.xyz(), 1.0_f ) );
			auto v4Normal = writer.declLocale( "v4Normal"
				, vec4( inNormal, 0.0_f ) );
			auto v4Tangent = writer.declLocale( "v4Tangent"
				, vec4( inTangent, 0.0_f ) );
			auto v3Texture = writer.declLocale( "v3Texture"
				, inTexture );

			if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
			{
				auto curMtxModel = writer.declLocale( "curMtxModel"
					, SkinningUbo::computeTransform( skinningData, writer, flags.programFlags ) );
				auto prvMtxModel = writer.declLocale( "prvMtxModel"
					, curMtxModel );
			}
			else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				auto curMtxModel = writer.declLocale( "curMtxModel"
					, inTransform );
				auto prvMtxModel = writer.declLocale( "prvMtxModel"
					, curMtxModel );
			}
			else
			{
				auto curMtxModel = writer.declLocale( "curMtxModel"
					, c3d_curMtxModel );
				auto prvMtxModel = writer.declLocale( "prvMtxModel"
					, c3d_prvMtxModel );
			}

			auto curMtxModel = writer.getVariable< Mat4 >( "curMtxModel" );
			auto prvMtxModel = writer.getVariable< Mat4 >( "prvMtxModel" );
			auto mtxNormal = writer.declLocale( "mtxNormal"
				, transpose( inverse( mat3( curMtxModel ) ) ) );

			if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
			{
				outMaterial = writer.cast< UInt >( inMaterial );
			}
			else
			{
				outMaterial = writer.cast< UInt >( c3d_materialIndex );
			}

			if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
			{
				curPosition = vec4( sdw::mix( curPosition.xyz(), inPosition2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v4Normal = vec4( sdw::mix( v4Normal.xyz(), inNormal2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v4Tangent = vec4( sdw::mix( v4Tangent.xyz(), inTangent2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v3Texture = sdw::mix( v3Texture, inTexture2, vec3( c3d_time ) );
			}

			outTexture = v3Texture;
			auto prvPosition = writer.declLocale( "prvPosition"
				, prvMtxModel * curPosition );
			curPosition = curMtxModel * curPosition;
			outWorldPosition = curPosition.xyz();
			prvPosition = c3d_prvViewProj * prvPosition;
			curPosition = c3d_curViewProj * curPosition;

			outNormal = normalize( mtxNormal * v4Normal.xyz() );
			outTangent = normalize( mtxNormal * v4Tangent.xyz() );
			outTangent = normalize( sdw::fma( -outNormal, vec3( dot( outTangent, outNormal ) ), outTangent ) );
			outBitangent = cross( outNormal, outTangent );

			if ( checkFlag( flags.programFlags, ProgramFlag::eInvertNormals ) )
			{
				outNormal = -outNormal;
				outTangent = -outTangent;
				outBitangent = -outBitangent;
			}

			outInstance = writer.cast< UInt >( in.instanceIndex );

			auto tbn = writer.declLocale( "tbn"
				, transpose( mat3( outTangent, outBitangent, outNormal ) ) );
			outTangentSpaceFragPosition = tbn * outWorldPosition;
			outTangentSpaceViewPosition = tbn * c3d_cameraPosition.xyz();

			// Convert the jitter from non-homogeneous coordiantes to homogeneous
			// coordinates and add it:
			// (note that for providing the jitter in non-homogeneous projection space,
			//  pixel coordinates (screen space) need to multiplied by two in the C++
			//  code)
			curPosition.xy() -= c3d_jitter * curPosition.w();
			prvPosition.xy() -= c3d_jitter * prvPosition.w();
			out.vtx.position = curPosition;

			outCurPosition = curPosition.xyw();
			outPrvPosition = prvPosition.xyw();
			// Positions in projection space are in [-1, 1] range, while texture
			// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
			// the scale (and flip the y axis):
			outCurPosition.xy() *= vec2( 0.5_f, -0.5f );
			outPrvPosition.xy() *= vec2( 0.5_f, -0.5f );
		};

		writer.implementFunction< Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr OpaquePass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
#if C3D_DebugGeometryShaders
		using namespace sdw;
		GeometryWriter writer;
		writer.inputLayout( ast::stmt::InputLayout::eTriangleList );
		writer.outputLayout( ast::stmt::OutputLayout::eTriangleStrip, 3u );

		auto in = writer.getIn();
		auto out = writer.getOut();

		// Geometry Inputs
		auto inWorldPosition = writer.declInputArray< Vec3 >( "inWorldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation, 3u );
		auto inCurPosition = writer.declInputArray< Vec3 >( "inCurPosition"
			, RenderPass::VertexOutputs::CurPositionLocation, 3u );
		auto inPrvPosition = writer.declInputArray< Vec3 >( "inPrvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation, 3u );
		auto inTangentSpaceFragPosition = writer.declInputArray< Vec3 >( "inTangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation, 3u );
		auto inTangentSpaceViewPosition = writer.declInputArray< Vec3 >( "inTangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation, 3u );
		auto inNormal = writer.declInputArray< Vec3 >( "inNormal"
			, RenderPass::VertexOutputs::NormalLocation, 3u );
		auto inTangent = writer.declInputArray< Vec3 >( "inTangent"
			, RenderPass::VertexOutputs::TangentLocation, 3u );
		auto inBitangent = writer.declInputArray< Vec3 >( "inBitangent"
			, RenderPass::VertexOutputs::BitangentLocation, 3u );
		auto inTexture = writer.declInputArray< Vec3 >( "inTexture"
			, RenderPass::VertexOutputs::TextureLocation, 3u );
		auto inInstance = writer.declInputArray< UInt >( "inInstance"
			, RenderPass::VertexOutputs::InstanceLocation, 3u );
		auto inMaterial = writer.declInputArray< UInt >( "inMaterial"
			, RenderPass::VertexOutputs::MaterialLocation, 3u );

		// Geometry Outputs
		auto outWorldPosition = writer.declOutput< Vec3 >( "outWorldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto outCurPosition = writer.declOutput< Vec3 >( "outCurPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto outPrvPosition = writer.declOutput< Vec3 >( "outPrvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto outTangentSpaceFragPosition = writer.declOutput< Vec3 >( "outTangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto outTangentSpaceViewPosition = writer.declOutput< Vec3 >( "outTangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto outNormal = writer.declOutput< Vec3 >( "outNormal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto outTangent = writer.declOutput< Vec3 >( "outTangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto outBitangent = writer.declOutput< Vec3 >( "outBitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto outTexture = writer.declOutput< Vec3 >( "outTexture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto outInstance = writer.declOutput< UInt >( "outInstance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto Material = writer.declOutput< UInt >( "Material"
			, RenderPass::VertexOutputs::MaterialLocation );

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

		auto & renderSystem = *getEngine()->getRenderSystem();
		shader::LegacyMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		// UBOs
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		// Fragment Inputs
		auto inWorldPosition = writer.declInput< Vec3 >( "inWorldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto inCurPosition = writer.declInput< Vec3 >( "inCurPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto inPrvPosition = writer.declInput< Vec3 >( "inPrvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto inTangentSpaceFragPosition = writer.declInput< Vec3 >( "inTangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto inTangentSpaceViewPosition = writer.declInput< Vec3 >( "inTangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto inNormal = writer.declInput< Vec3 >( "inNormal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto inTangent = writer.declInput< Vec3 >( "inTangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto inBitangent = writer.declInput< Vec3 >( "inBitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto inInstance = writer.declInput< UInt >( "inInstance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto inMaterial = writer.declInput< UInt >( "inMaterial"
			, RenderPass::VertexOutputs::MaterialLocation );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

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
		utils.declareParallaxMappingFunc( flags );

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

				lightingModel.computeMapContributions( flags
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
				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, material.m_alphaRef );
				emissive *= diffuse;
				auto matFlags = writer.declLocale( "flags"
					, 0.0_f );
				utils.encodeMaterial( c3d_shadowReceiver
					, ( checkFlag( flags.passFlags, PassFlag::eReflection ) ) ? 1_i : 0_i
					, ( checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? 1_i : 0_i
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

		auto & renderSystem = *getEngine()->getRenderSystem();
		shader::PbrMRMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		// UBOs
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		// Fragment Inputs
		auto inWorldPosition = writer.declInput< Vec3 >( "inWorldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto inCurPosition = writer.declInput< Vec3 >( "inCurPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto inPrvPosition = writer.declInput< Vec3 >( "inPrvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto inTangentSpaceFragPosition = writer.declInput< Vec3 >( "inTangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto inTangentSpaceViewPosition = writer.declInput< Vec3 >( "inTangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto inNormal = writer.declInput< Vec3 >( "inNormal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto inTangent = writer.declInput< Vec3 >( "inTangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto inBitangent = writer.declInput< Vec3 >( "inBitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto inInstance = writer.declInput< UInt >( "inInstance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto inMaterial = writer.declInput< UInt >( "inMaterial"
			, RenderPass::VertexOutputs::MaterialLocation );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

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
		utils.declareParallaxMappingFunc( flags );

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

				lightingModel.computeMapContributions( flags
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
				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, material.m_alphaRef );
				emissive *= albedo;
				auto matFlags = writer.declLocale( "flags"
					, 0.0_f );
				utils.encodeMaterial( c3d_shadowReceiver
					, ( checkFlag( flags.passFlags, PassFlag::eReflection ) ) ? 1_i : 0_i
					, ( checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? 1_i : 0_i
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

		shader::PbrSGMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		// UBOs
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );

		// Fragment Inputs
		auto inWorldPosition = writer.declInput< Vec3 >( "inWorldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto inCurPosition = writer.declInput< Vec3 >( "inCurPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto inPrvPosition = writer.declInput< Vec3 >( "inPrvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto inTangentSpaceFragPosition = writer.declInput< Vec3 >( "inTangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto inTangentSpaceViewPosition = writer.declInput< Vec3 >( "inTangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto inNormal = writer.declInput< Vec3 >( "inNormal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto inTangent = writer.declInput< Vec3 >( "inTangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto inBitangent = writer.declInput< Vec3 >( "inBitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto inTexture = writer.declInput< Vec3 >( "inTexture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto inInstance = writer.declInput< UInt >( "inInstance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto inMaterial = writer.declInput< UInt >( "inMaterial"
			, RenderPass::VertexOutputs::MaterialLocation );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );

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
		utils.declareParallaxMappingFunc( flags );

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

				lightingModel.computeMapContributions( flags
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
				utils.applyAlphaFunc( flags.alphaFunc
					, alpha
					, material.m_alphaRef );
				emissive *= albedo;
				auto matFlags = writer.declLocale( "flags"
					, 0.0_f );
				utils.encodeMaterial( c3d_shadowReceiver
					, ( checkFlag( flags.passFlags, PassFlag::eReflection ) ) ? 1_i : 0_i
					, ( checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? 1_i : 0_i
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
