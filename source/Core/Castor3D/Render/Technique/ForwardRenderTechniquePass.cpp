#include "Castor3D/Render/Technique/ForwardRenderTechniquePass.hpp"

#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/Node/RenderNode_Render.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslLpvGI.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	ForwardRenderTechniquePass::ForwardRenderTechniquePass( String const & name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config
		, LpvGridConfigUbo const * lpvConfigUbo
		, LayeredLpvGridConfigUbo const * llpvConfigUbo )
		: RenderTechniquePass{ name
			, name
			, matrixUbo
			, culler
			, environment
			, ignored
			, config
			, lpvConfigUbo
			, llpvConfigUbo }
	{
	}

	ForwardRenderTechniquePass::ForwardRenderTechniquePass( String const & name
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, bool oit
		, bool environment
		, SceneNode const * ignored
		, SsaoConfig const & config
		, LpvGridConfigUbo const * lpvConfigUbo
		, LayeredLpvGridConfigUbo const * llpvConfigUbo )
		: RenderTechniquePass{ name
			, name
			, matrixUbo
			, culler
			, oit
			, environment
			, ignored
			, config
			, lpvConfigUbo
			, llpvConfigUbo }
	{
	}

	ForwardRenderTechniquePass::~ForwardRenderTechniquePass()
	{
	}

	void ForwardRenderTechniquePass::initialiseRenderPass( RenderDevice const & device
		, ashes::ImageView const & colourView
		, ashes::ImageView const & depthView
		, castor::Size const & size
		, bool clear )
	{
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				depthView.getFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				( clear
					? VK_ATTACHMENT_LOAD_OP_CLEAR
					: VK_ATTACHMENT_LOAD_OP_LOAD ),
				VK_ATTACHMENT_STORE_OP_STORE,
				( ( clear && ashes::isDepthStencilFormat( depthView.getFormat() ) )
					? VK_ATTACHMENT_LOAD_OP_CLEAR
					: VK_ATTACHMENT_LOAD_OP_DONT_CARE ),
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				( clear
					? VK_IMAGE_LAYOUT_UNDEFINED
					: VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ),
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			},
			{
				0u,
				colourView.getFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				( clear
					? VK_ATTACHMENT_LOAD_OP_CLEAR
					: VK_ATTACHMENT_LOAD_OP_LOAD ),
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				( clear
					? VK_IMAGE_LAYOUT_UNDEFINED
					: VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VkPipelineStageFlags( clear
					? VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					: VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT ),
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VkAccessFlags( clear
					? VK_ACCESS_SHADER_READ_BIT
					: VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT ),
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VkPipelineStageFlags( clear
					? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					: VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT ),
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VkAccessFlags( clear
					? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					: VK_ACCESS_SHADER_READ_BIT ),
				VK_DEPENDENCY_BY_REGION_BIT,
			}
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = device->createRenderPass( getName()
			, std::move( createInfo ) );
		ashes::ImageViewCRefArray fbAttaches;
		fbAttaches.emplace_back( depthView );
		fbAttaches.emplace_back( colourView );

		m_frameBuffer = m_renderPass->createFrameBuffer( getName()
			, { colourView.image->getDimensions().width, colourView.image->getDimensions().height }
			, std::move( fbAttaches ) );

		m_nodesCommands = device.graphicsCommandPool->createCommandBuffer( getName() );
	}

	void ForwardRenderTechniquePass::accept( RenderTechniqueVisitor & visitor )
	{
		auto shaderProgram = getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, visitor.getFlags() );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ) );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ) );
	}

	ashes::Semaphore const & ForwardRenderTechniquePass::render( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;

		if ( hasNodes() )
		{
			static ashes::VkClearValueArray const clearValues
			{
				defaultClearDepthStencil,
				opaqueBlackClearColor,
			};

			RenderPassTimerBlock timerBlock{ getTimer().start() };

			m_nodesCommands->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
			m_nodesCommands->beginDebugBlock(
				{
					"Forward Pass",
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

			device.graphicsQueue->submit( { *m_nodesCommands }
				, { *result }
				, { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }
				, { getSemaphore() }
				, nullptr );
			result = &getSemaphore();
		}

		return *result;
	}

	void ForwardRenderTechniquePass::doCleanup( RenderDevice const & device )
	{
		m_nodesCommands.reset();
		m_frameBuffer.reset();
		RenderTechniquePass::doCleanup( device );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		// Since their vertex attribute locations overlap, we must not have both set at the same time.
		CU_Require( ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) ? 1 : 0 )
			+ ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) ? 1 : 0 ) < 2
			&& "Can't have both instantiation and morphing yet." );
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

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0 );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0 );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, flags.programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, flags.programFlags );

		// Outputs
		auto vtx_worldPosition = writer.declOutput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declOutput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declOutput< Vec3 >( "vtx_curPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declOutput< Vec3 >( "vtx_prvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declOutput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declOutput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declOutput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declOutput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declOutput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declOutput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declOutput< UInt >( "vtx_instance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declOutput< UInt >( "vtx_material"
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
				vtx_material = writer.cast< UInt >( inMaterial );
			}
			else
			{
				vtx_material = writer.cast< UInt >( c3d_materialIndex );
			}

			if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
			{
				curPosition = vec4( sdw::mix( curPosition.xyz(), inPosition2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v4Normal = vec4( sdw::mix( v4Normal.xyz(), inNormal2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v4Tangent = vec4( sdw::mix( v4Tangent.xyz(), inTangent2.xyz(), vec3( c3d_time ) ), 1.0_f );
				v3Texture = sdw::mix( v3Texture, inTexture2, vec3( c3d_time ) );
			}

			vtx_texture = v3Texture;
			auto prvPosition = writer.declLocale( "prvPosition"
				, prvMtxModel * curPosition );
			curPosition = curMtxModel * curPosition;
			vtx_worldPosition = curPosition.xyz();
			prvPosition = c3d_prvView * curPosition;
			curPosition = c3d_curView * curPosition;
			vtx_viewPosition = curPosition.xyz();

			vtx_normal = normalize( mtxNormal * v4Normal.xyz() );
			vtx_tangent = normalize( mtxNormal * v4Tangent.xyz() );
			vtx_tangent = normalize( sdw::fma( -vtx_normal, vec3( dot( vtx_tangent, vtx_normal ) ), vtx_tangent ) );
			vtx_bitangent = cross( vtx_normal, vtx_tangent );

			if ( checkFlag( flags.programFlags, ProgramFlag::eInvertNormals ) )
			{
				vtx_normal = -vtx_normal;
				vtx_tangent = -vtx_tangent;
				vtx_bitangent = -vtx_bitangent;
			}

			vtx_instance = writer.cast< UInt >( in.instanceIndex );
			prvPosition = c3d_projection * prvPosition;
			curPosition = c3d_projection * curPosition;

			auto tbn = writer.declLocale( "tbn"
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
			out.vtx.position = curPosition;

			vtx_curPosition = curPosition.xyw();
			vtx_prvPosition = prvPosition.xyw();
			// Positions in projection space are in [-1, 1] range, while texture
			// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
			// the scale (and flip the y axis):
			vtx_curPosition.xy() *= vec2( 0.5_f, -0.5_f );
			vtx_prvPosition.xy() *= vec2( 0.5_f, -0.5_f );
		};

		writer.implementFunction< sdw::Void >( "main", main );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declInput< Vec3 >( "vtx_curPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( "vtx_prvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( "vtx_instance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::LegacyMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", getLightBufferIndex(), 0u );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );
		UBO_LPVGRIDCONFIG( writer, LpvGridConfigUbo::BindingPoint, 0u, checkFlag( flags.sceneFlags, SceneFlag::eLpvGI ) );
		UBO_LAYERED_LPVGRIDCONFIG( writer, LayeredLpvGridConfigUbo::BindingPoint, 0u, checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI ) );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );
		index += uint32_t( flags.textures.size() );
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment"
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? index++ : 0u
			, 1u
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ) );
		shader::LpvGI lpvGI{ writer };
		lpvGI.declare( 1u, index, flags.sceneFlags );

		auto in = writer.getIn();

		shader::Fog fog{ getFogType( flags.sceneFlags ), writer };
		shader::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareParallaxMappingFunc( flags );
		auto lighting = shader::PhongLightingModel::createModel( writer
			, utils
			, flags.sceneFlags
			, false // rsm
			, index
			, m_mode != RenderMode::eTransparentOnly );
		shader::PhongReflectionModel reflections{ writer, utils };

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = writer.declLocale( "material"
					, materials.getMaterial( vtx_material ) );
				auto texCoord = writer.declLocale( "texCoord"
					, vtx_texture );
				auto alpha = writer.declLocale( "alpha"
					, material.m_opacity );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto normal = writer.declLocale( "normal"
					, normalize( vtx_normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( vtx_tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( vtx_bitangent ) );
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
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, vtx_tangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, vtx_tangentSpaceFragPosition );
				lighting->computeMapContributions( flags
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
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_cameraPosition.xyz() );
				auto lightDiffuse = writer.declLocale( "lightDiffuse"
					, vec3( 0.0_f ) );
				auto lightSpecular = writer.declLocale( "lightSpecular"
					, vec3( 0.0_f ) );
				shader::OutputComponents output{ lightDiffuse, lightSpecular };
				lighting->computeCombined( worldEye
					, shininess
					, c3d_shadowReceiver
					, shader::FragmentInput( in.fragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
					, output );

				auto ambient = writer.declLocale( "ambient"
					, clamp( c3d_ambientLight.xyz() * material.m_ambient * diffuse
						, vec3( 0.0_f )
						, vec3( 1.0_f ) ) );
				
				if ( checkFlag( flags.passFlags, PassFlag::eReflection )
					|| checkFlag( flags.passFlags, PassFlag::eRefraction ) )
				{
					auto incident = writer.declLocale( "incident"
						, reflections.computeIncident( vtx_worldPosition, c3d_cameraPosition.xyz() ) );

					if ( checkFlag( flags.passFlags, PassFlag::eReflection )
						&& checkFlag( flags.passFlags, PassFlag::eRefraction ) )
					{
						reflections.computeReflRefr( incident
							, normal
							, occlusion
							, c3d_mapEnvironment
							, material.m_refractionRatio
							, shininess
							, ambient
							, diffuse );
					}
					else if ( checkFlag( flags.passFlags, PassFlag::eReflection ) )
					{
						reflections.computeRefl( incident
							, normal
							, occlusion
							, c3d_mapEnvironment
							, shininess
							, ambient
							, diffuse );
					}
					else
					{
						reflections.computeRefr( incident
							, normal
							, occlusion
							, c3d_mapEnvironment
							, material.m_refractionRatio
							, shininess
							, ambient
							, diffuse );
					}
				}
				else
				{
					ambient *= occlusion * diffuse;
					diffuse *= lightDiffuse;
				}

				pxl_fragColor = vec4( lpvGI.computeResult( flags.sceneFlags
						, vtx_worldPosition
						, normal
						, c3d_minVolumeCorner
						, c3d_cellSize
						, c3d_gridSize
						, c3d_allMinVolumeCorners
						, c3d_allCellSizes
						, c3d_gridSizes
						, diffuse + lightSpecular + emissive
						, ambient )
					, alpha );

				if ( getFogType( flags.sceneFlags ) != FogType::eDisabled )
				{
					pxl_fragColor = fog.apply( vec4( utils.removeGamma( gamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
						, pxl_fragColor
						, length( vtx_viewPosition )
						, vtx_viewPosition.y()
						, c3d_fogInfo
						, c3d_cameraPosition );
				}
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declInput< Vec3 >( "vtx_curPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( "vtx_prvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( "vtx_instance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::PbrMRMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", getLightBufferIndex(), 0u );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );
		UBO_LPVGRIDCONFIG( writer, LpvGridConfigUbo::BindingPoint, 0u, checkFlag( flags.sceneFlags, SceneFlag::eLpvGI ) );
		UBO_LAYERED_LPVGRIDCONFIG( writer, LayeredLpvGridConfigUbo::BindingPoint, 0u, checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI ) );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );
		index += uint32_t( flags.textures.size() );
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment"
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? index++ : 0u
			, 1u
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ) );
		auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance"
			, index++
			, 1u );
		auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered"
			, index++
			, 1u );
		auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf"
			, index++
			, 1u );
		shader::LpvGI lpvGI{ writer };
		lpvGI.declare( 1u, index, flags.sceneFlags );

		auto in = writer.getIn();

		shader::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareFresnelSchlick();
		utils.declareComputeIBL();
		utils.declareInvertVec3Y();
		utils.declareParallaxMappingFunc( flags );
		auto lighting = shader::MetallicBrdfLightingModel::createModel( writer
			, utils
			, flags.sceneFlags
			, false // rsm
			, index
			, m_mode != RenderMode::eTransparentOnly );
		shader::MetallicPbrReflectionModel reflections{ writer, utils };
		shader::Fog fog{ getFogType( flags.sceneFlags ), writer };

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = writer.declLocale( "material"
					, materials.getMaterial( vtx_material ) );
				auto texCoord = writer.declLocale( "texCoord"
					, vtx_texture );
				auto alpha = writer.declLocale( "alpha"
					, material.m_opacity );
				auto normal = writer.declLocale( "normal"
					, normalize( vtx_normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( vtx_tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( vtx_bitangent ) );
				auto ambient = writer.declLocale( "ambient"
					, c3d_ambientLight.xyz() );
				auto metallic = writer.declLocale( "metallic"
					, material.m_metallic );
				auto roughness = writer.declLocale( "roughness"
					, material.m_roughness );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto albedo = writer.declLocale( "albedo"
					, utils.removeGamma( gamma, material.m_albedo ) );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.m_emissive ) );
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_cameraPosition.xyz() );
				auto envAmbient = writer.declLocale( "envAmbient"
					, vec3( 1.0_f ) );
				auto envDiffuse = writer.declLocale( "envDiffuse"
					, vec3( 1.0_f ) );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, vtx_tangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, vtx_tangentSpaceFragPosition );
				lighting->computeMapContributions( flags
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
				auto lightDiffuse = writer.declLocale( "lightDiffuse"
					, vec3( 0.0_f ) );
				auto lightSpecular = writer.declLocale( "lightSpecular"
					, vec3( 0.0_f ) );
				shader::OutputComponents output{ lightDiffuse, lightSpecular };
				lighting->computeCombined( worldEye
					, albedo
					, metallic
					, roughness
					, c3d_shadowReceiver
					, shader::FragmentInput( in.fragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
					, output );

				if ( checkFlag( flags.passFlags, PassFlag::eReflection )
					|| checkFlag( flags.passFlags, PassFlag::eRefraction ) )
				{
					auto incident = writer.declLocale( "incident"
						, reflections.computeIncident( vtx_worldPosition, worldEye ) );
					auto ratio = writer.declLocale( "ratio"
						, material.m_refractionRatio );

					if ( checkFlag( flags.passFlags, PassFlag::eReflection ) )
					{
						// Reflection from environment map.
						ambient = reflections.computeRefl( incident
							, normal
							, occlusion
							, c3d_mapEnvironment
							, c3d_ambientLight.xyz()
							, albedo );
					}
					else
					{
						// Reflection from background skybox.
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeMetallicIBL( normal
								, vtx_worldPosition
								, albedo
								, metallic
								, roughness
								, c3d_cameraPosition.xyz()
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
					}

					if ( checkFlag( flags.passFlags, PassFlag::eRefraction ) )
					{
						// Refraction from environment map.
						ambient = reflections.computeRefrEnvMap( incident
							, normal
							, occlusion
							, c3d_mapEnvironment
							, ratio
							, ambient
							, albedo
							, roughness );
					}
					else
					{
						IF( writer, ratio != 0.0_f )
						{
							// Refraction from background skybox.
							ambient = reflections.computeRefrSkybox( incident
								, normal
								, occlusion
								, c3d_mapPrefiltered
								, material.m_refractionRatio
								, ambient
								, albedo
								, roughness );
						}
						FI;
					}
				}
				else
				{
					// Reflection from background skybox.
					ambient *= occlusion * utils.computeMetallicIBL( normal
						, vtx_worldPosition
						, albedo
						, metallic
						, roughness
						, c3d_cameraPosition.xyz()
						, c3d_mapIrradiance
						, c3d_mapPrefiltered
						, c3d_mapBrdf );
					auto ratio = writer.declLocale( "ratio"
						, material.m_refractionRatio );

					IF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						auto incident = writer.declLocale( "incident"
							, reflections.computeIncident( vtx_worldPosition, worldEye ) );
						ambient = reflections.computeRefrSkybox( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio
							, ambient
							, albedo
							, roughness );
					}
					FI;
				}

				pxl_fragColor = vec4( lpvGI.computeResult( flags.sceneFlags
						, vtx_worldPosition
						, normal
						, c3d_minVolumeCorner
						, c3d_cellSize
						, c3d_gridSize
						, c3d_allMinVolumeCorners
						, c3d_allCellSizes
						, c3d_gridSizes
						, sdw::fma( lightDiffuse
							, albedo
							, lightSpecular + emissive )
						, ambient )
					, alpha );

				if ( getFogType( flags.sceneFlags ) != FogType::eDisabled )
				{
					pxl_fragColor = fog.apply( vec4( utils.removeGamma( gamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
						, pxl_fragColor
						, length( vtx_viewPosition )
						, vtx_viewPosition.y()
						, c3d_fogInfo
						, c3d_cameraPosition );
				}
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr ForwardRenderTechniquePass::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		// Fragment Intputs
		auto vtx_worldPosition = writer.declInput< Vec3 >( "vtx_worldPosition"
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_viewPosition = writer.declInput< Vec3 >( "vtx_viewPosition"
			, RenderPass::VertexOutputs::ViewPositionLocation );
		auto vtx_curPosition = writer.declInput< Vec3 >( "vtx_curPosition"
			, RenderPass::VertexOutputs::CurPositionLocation );
		auto vtx_prvPosition = writer.declInput< Vec3 >( "vtx_prvPosition"
			, RenderPass::VertexOutputs::PrvPositionLocation );
		auto vtx_tangentSpaceFragPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceFragPosition"
			, RenderPass::VertexOutputs::TangentSpaceFragPositionLocation );
		auto vtx_tangentSpaceViewPosition = writer.declInput< Vec3 >( "vtx_tangentSpaceViewPosition"
			, RenderPass::VertexOutputs::TangentSpaceViewPositionLocation );
		auto vtx_normal = writer.declInput< Vec3 >( "vtx_normal"
			, RenderPass::VertexOutputs::NormalLocation );
		auto vtx_tangent = writer.declInput< Vec3 >( "vtx_tangent"
			, RenderPass::VertexOutputs::TangentLocation );
		auto vtx_bitangent = writer.declInput< Vec3 >( "vtx_bitangent"
			, RenderPass::VertexOutputs::BitangentLocation );
		auto vtx_texture = writer.declInput< Vec3 >( "vtx_texture"
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_instance = writer.declInput< UInt >( "vtx_instance"
			, RenderPass::VertexOutputs::InstanceLocation );
		auto vtx_material = writer.declInput< UInt >( "vtx_material"
			, RenderPass::VertexOutputs::MaterialLocation );

		shader::PbrSGMaterials materials{ writer };
		materials.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", getLightBufferIndex(), 0u );
		shader::TextureConfigurations textureConfigs{ writer };
		bool hasTextures = !flags.textures.empty();

		if ( hasTextures )
		{
			textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers() );
		}

		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0u );
		UBO_TEXTURES( writer, TexturesUbo::BindingPoint, 0u, hasTextures );
		UBO_LPVGRIDCONFIG( writer, LpvGridConfigUbo::BindingPoint, 0u, checkFlag( flags.sceneFlags, SceneFlag::eLpvGI ) );
		UBO_LAYERED_LPVGRIDCONFIG( writer, LayeredLpvGridConfigUbo::BindingPoint, 0u, checkFlag( flags.sceneFlags, SceneFlag::eLayeredLpvGI ) );

		auto index = getMinTextureIndex();
		auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
			, index
			, 1u
			, std::max( 1u, uint32_t( flags.textures.size() ) )
			, hasTextures ) );
		index += uint32_t( flags.textures.size() );
		auto c3d_mapEnvironment( writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapEnvironment"
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ? index++ : 0u
			, 1u
			, ( checkFlag( flags.passFlags, PassFlag::eReflection )
				|| checkFlag( flags.passFlags, PassFlag::eRefraction ) ) ) );
		auto c3d_mapIrradiance = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapIrradiance"
			, index++
			, 1u );
		auto c3d_mapPrefiltered = writer.declSampledImage< FImgCubeRgba32 >( "c3d_mapPrefiltered"
			, index++
			, 1u );
		auto c3d_mapBrdf = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapBrdf"
			, index++
			, 1u );
		shader::LpvGI lpvGI{ writer };
		lpvGI.declare( 1u, index, flags.sceneFlags );

		auto in = writer.getIn();

		shader::Fog fog{ getFogType( flags.sceneFlags ), writer };
		shader::Utils utils{ writer };
		utils.declareApplyGamma();
		utils.declareRemoveGamma();
		utils.declareFresnelSchlick();
		utils.declareComputeIBL();
		utils.declareParallaxMappingFunc( flags );
		auto lighting = shader::SpecularBrdfLightingModel::createModel( writer
			, utils
			, flags.sceneFlags
			, false // rsm
			, index
			, m_mode != RenderMode::eTransparentOnly );
		shader::SpecularPbrReflectionModel reflections{ writer, utils };

		// Fragment Outputs
		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto material = writer.declLocale( "material"
					, materials.getMaterial( vtx_material ) );
				auto texCoord = writer.declLocale( "texCoord"
					, vtx_texture );
				auto alpha = writer.declLocale( "alpha"
					, material.m_opacity );
				auto normal = writer.declLocale( "normal"
					, normalize( vtx_normal ) );
				auto tangent = writer.declLocale( "tangent"
					, normalize( vtx_tangent ) );
				auto bitangent = writer.declLocale( "bitangent"
					, normalize( vtx_bitangent ) );
				auto ambient = writer.declLocale( "ambient"
					, c3d_ambientLight.xyz() );
				auto specular = writer.declLocale( "specular"
					, material.m_specular );
				auto glossiness = writer.declLocale( "glossiness"
					, material.m_glossiness );
				auto gamma = writer.declLocale( "gamma"
					, material.m_gamma );
				auto albedo = writer.declLocale( "albedo"
					, utils.removeGamma( gamma, material.m_diffuse() ) );
				auto emissive = writer.declLocale( "emissive"
					, vec3( material.m_emissive ) );
				auto worldEye = writer.declLocale( "worldEye"
					, c3d_cameraPosition.xyz() );
				auto envAmbient = writer.declLocale( "envAmbient"
					, vec3( 1.0_f ) );
				auto envDiffuse = writer.declLocale( "envDiffuse"
					, vec3( 1.0_f ) );
				auto occlusion = writer.declLocale( "occlusion"
					, 1.0_f );
				auto transmittance = writer.declLocale( "transmittance"
					, 0.0_f );
				auto tangentSpaceViewPosition = writer.declLocale( "tangentSpaceViewPosition"
					, vtx_tangentSpaceViewPosition );
				auto tangentSpaceFragPosition = writer.declLocale( "tangentSpaceFragPosition"
					, vtx_tangentSpaceFragPosition );
				lighting->computeMapContributions( flags
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
				auto lightDiffuse = writer.declLocale( "lightDiffuse"
					, vec3( 0.0_f ) );
				auto lightSpecular = writer.declLocale( "lightSpecular"
					, vec3( 0.0_f ) );
				shader::OutputComponents output{ lightDiffuse, lightSpecular };
				lighting->computeCombined( worldEye
					, specular
					, glossiness
					, c3d_shadowReceiver
					, shader::FragmentInput( in.fragCoord.xy(), vtx_viewPosition, vtx_worldPosition, normal )
					, output );

				if ( checkFlag( flags.passFlags, PassFlag::eReflection )
					|| checkFlag( flags.passFlags, PassFlag::eRefraction ) )
				{
					auto incident = writer.declLocale( "incident"
						, reflections.computeIncident( vtx_worldPosition, worldEye ) );
					auto ratio = writer.declLocale( "ratio"
						, material.m_refractionRatio );

					if ( checkFlag( flags.passFlags, PassFlag::eReflection ) )
					{
						// Reflection from environment map.
						ambient = reflections.computeRefl( incident
							, normal
							, occlusion
							, c3d_mapEnvironment
							, c3d_ambientLight.xyz()
							, albedo );
					}
					else
					{
						// Reflection from background skybox.
						ambient = c3d_ambientLight.xyz()
							* occlusion
							* utils.computeSpecularIBL( normal
								, vtx_worldPosition
								, albedo
								, specular
								, glossiness
								, c3d_cameraPosition.xyz()
								, c3d_mapIrradiance
								, c3d_mapPrefiltered
								, c3d_mapBrdf );
					}

					if ( checkFlag( flags.passFlags, PassFlag::eRefraction ) )
					{
						// Refraction from environment map.
						ambient = reflections.computeRefrEnvMap( incident
							, normal
							, occlusion
							, c3d_mapEnvironment
							, ratio
							, ambient
							, albedo
							, glossiness );
					}
					else
					{
						IF( writer, ratio != 0.0_f )
						{
							// Refraction from background skybox.
							ambient = reflections.computeRefrSkybox( incident
								, normal
								, occlusion
								, c3d_mapPrefiltered
								, material.m_refractionRatio
								, ambient
								, albedo
								, glossiness );
						}
						FI;
					}
				}
				else
				{
					// Reflection from background skybox.
					ambient *= occlusion * utils.computeSpecularIBL( normal
						, vtx_worldPosition
						, albedo
						, specular
						, glossiness
						, c3d_cameraPosition.xyz()
						, c3d_mapIrradiance
						, c3d_mapPrefiltered
						, c3d_mapBrdf );
					auto ratio = writer.declLocale( "ratio"
						, material.m_refractionRatio );

					IF( writer, ratio != 0.0_f )
					{
						// Refraction from background skybox.
						auto incident = writer.declLocale( "incident"
							, reflections.computeIncident( vtx_worldPosition, worldEye ) );
						ambient = reflections.computeRefrSkybox( incident
							, normal
							, occlusion
							, c3d_mapPrefiltered
							, material.m_refractionRatio
							, ambient
							, albedo
							, glossiness );
					}
					FI;
				}

				pxl_fragColor = vec4( lpvGI.computeResult( flags.sceneFlags
						, vtx_worldPosition
						, normal
						, c3d_minVolumeCorner
						, c3d_cellSize
						, c3d_gridSize
						, c3d_allMinVolumeCorners
						, c3d_allCellSizes
						, c3d_gridSizes
						, sdw::fma( lightDiffuse
							, albedo
							, lightSpecular + emissive )
						, ambient )
					, alpha );

				if ( getFogType( flags.sceneFlags ) != FogType::eDisabled )
				{
					pxl_fragColor = fog.apply( vec4( utils.removeGamma( gamma, c3d_backgroundColour.rgb() ), c3d_backgroundColour.a() )
						, pxl_fragColor
						, length( vtx_viewPosition )
						, vtx_viewPosition.y()
						, c3d_fogInfo
						, c3d_cameraPosition );
				}
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	//*********************************************************************************************
}
