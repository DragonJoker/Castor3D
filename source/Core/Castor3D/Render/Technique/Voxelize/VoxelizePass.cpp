#include "Castor3D/Render/Technique/Voxelize/VoxelizePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/Node/SceneCulledRenderNodes.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	VoxelizePass::VoxelizePass( Engine & engine
		, MatrixUbo & matrixUbo
		, SceneCuller & culler
		, TextureLayoutSPtr result
		, ashes::ImageView colourView )
		: RenderPass{ "Voxelize"
			, "Voxelization"
			, engine
			, matrixUbo
			, culler }
		, m_scene{ culler.getScene() }
		, m_camera{ culler.getCamera() }
		, m_commands{ nullptr, nullptr }
		, m_result{ result }
		, m_colourView{ colourView }
	{
	}

	VoxelizePass::~VoxelizePass()
	{
	}

	void VoxelizePass::accept( RenderTechniqueVisitor & visitor )
	{
		auto shaderProgram = getEngine()->getShaderProgramCache().getAutomaticProgram( *this
			, visitor.getFlags() );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_VERTEX_BIT ) );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_GEOMETRY_BIT ) );
		visitor.visit( shaderProgram->getSource( VK_SHADER_STAGE_FRAGMENT_BIT ) );
	}

	void VoxelizePass::update( GpuUpdater & updater )
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

			RenderPass::doUpdate( nodes.instancedStaticNodes.backCulled, updater.info );
			RenderPass::doUpdate( nodes.staticNodes.backCulled, updater.info );
			RenderPass::doUpdate( nodes.skinnedNodes.backCulled, updater.info );
			RenderPass::doUpdate( nodes.instancedSkinnedNodes.backCulled, updater.info );
			RenderPass::doUpdate( nodes.morphingNodes.backCulled, updater.info );
			RenderPass::doUpdate( nodes.billboardNodes.backCulled, updater.info );
		}

		static const Matrix4x4f identity
		{
			[]()
			{
				Matrix4x4f res;
				res.setIdentity();
				return res;
			}()
		};
		//Orthograhic projection
		auto sceneBoundingBox = m_scene.getBoundingBox();
		auto ortho = matrix::ortho( sceneBoundingBox.getMin()->x
			, sceneBoundingBox.getMax()->x
			, sceneBoundingBox.getMin()->y
			, sceneBoundingBox.getMax()->y
			, -1.0f * sceneBoundingBox.getMin()->z
			, -1.0f * sceneBoundingBox.getMax()->z );
		auto jitterProjSpace = updater.jitter * 2.0f;
		jitterProjSpace[0] /= m_camera.getWidth();
		jitterProjSpace[1] /= m_camera.getHeight();
		m_matrixUbo.cpuUpdate( identity
			, ortho
			, jitterProjSpace );

	}

	ashes::Semaphore const & VoxelizePass::render( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;

		if ( hasNodes() )
		{
			RenderPassTimerBlock timerBlock{ getTimer().start() };

			auto & cmd = *m_commands.commandBuffer;
			cmd.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
			cmd.memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, m_result->getDefaultView().getSampledView().makeGeneralLayout( VK_IMAGE_LAYOUT_UNDEFINED
					, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT ) );
			cmd.beginDebugBlock(
				{
					"Voxelization Pass",
					makeFloatArray( getEngine()->getNextRainbowColour() ),
				} );
			timerBlock->beginPass( cmd );
			timerBlock->notifyPassRender();
			cmd.beginRenderPass( getRenderPass()
				, *m_frameBuffer
				, { opaqueBlackClearColor }
				, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
			cmd.executeCommands( { getCommandBuffer() } );
			cmd.endRenderPass();
			timerBlock->endPass( cmd );
			cmd.endDebugBlock();
			cmd.end();

			device.graphicsQueue->submit( { cmd }
				, { *result }
				, { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }
				, { getSemaphore() }
				, nullptr );
			result = &getSemaphore();
		}

		return *result;
	}

	bool VoxelizePass::doInitialise( RenderDevice const & device
		, Size const & CU_UnusedParam( size ) )
	{
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				m_colourView->format,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_LOAD,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			},
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				ashes::nullopt,
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
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
		fbAttaches.emplace_back( m_colourView );

		m_frameBuffer = m_renderPass->createFrameBuffer( getName()
			, { m_result->getWidth(), m_result->getHeight() }
			, std::move( fbAttaches ) );

		m_commands =
		{
			device.graphicsCommandPool->createCommandBuffer( getName() ),
			device->createSemaphore( getName() ),
		};
		return true;
	}

	void VoxelizePass::doCleanup( RenderDevice const & device )
	{
		m_renderQueue.cleanup();
		m_commands = { nullptr, nullptr };
		m_frameBuffer.reset();
	}

	void VoxelizePass::doUpdate( RenderQueueArray & queues )
	{
		queues.emplace_back( m_renderQueue );
	}

	void VoxelizePass::doUpdateFlags( PipelineFlags & flags )const
	{
		flags.textures.clear();
		addFlag( flags.programFlags, ProgramFlag::eHasGeometry );
	}

	void VoxelizePass::doUpdatePipeline( RenderPipeline & pipeline )
	{
		m_sceneUbo.cpuUpdate( m_scene, &m_camera );
	}

	ashes::VkDescriptorSetLayoutBindingArray VoxelizePass::doCreateTextureBindings( PipelineFlags const & flags )const
	{
		ashes::VkDescriptorSetLayoutBindingArray textureBindings;
		textureBindings.emplace_back( makeDescriptorSetLayoutBinding( 0u
			, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
			, VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
			, 1u ) );
		return textureBindings;
	}

	ashes::PipelineDepthStencilStateCreateInfo VoxelizePass::doCreateDepthStencilState( PipelineFlags const & flags )const
	{
		return ashes::PipelineDepthStencilStateCreateInfo
		{
			0u,
			VK_FALSE,
			VK_FALSE,
		};
	}

	ashes::PipelineColorBlendStateCreateInfo VoxelizePass::doCreateBlendState( PipelineFlags const & flags )const
	{
		return RenderPass::createBlendState( flags.colourBlendMode, flags.alphaBlendMode, 1u );
	}

	void VoxelizePass::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, BillboardListRenderNode & node )
	{
	}

	void VoxelizePass::doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
		, SubmeshRenderNode & node )
	{
	}

	void VoxelizePass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, BillboardListRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		ashes::WriteDescriptorSetArray writes;
		writes.push_back( ashes::WriteDescriptorSet
			{
				0u,
				0u,
				VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
				{ {
					nullptr,
					m_result->getDefaultView().getSampledView(),
					VK_IMAGE_LAYOUT_GENERAL,
				} }
			} );
		node.texDescriptorSet->setBindings( writes );
	}

	void VoxelizePass::doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
		, uint32_t & index
		, SubmeshRenderNode & node
		, ShadowMapLightTypeArray const & shadowMaps )
	{
		ashes::WriteDescriptorSetArray writes;
		writes.push_back( ashes::WriteDescriptorSet
			{
				0u,
				0u,
				VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
				{ {
					nullptr,
					m_result->getDefaultView().getSampledView(),
					VK_IMAGE_LAYOUT_GENERAL,
				} }
			} );
		node.texDescriptorSet->setBindings( writes );
	}

	ShaderPtr VoxelizePass::doGetVertexShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;

		// Inputs
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0u );
		auto skinningData = SkinningUbo::declare( writer, SkinningUbo::BindingPoint, 0, flags.programFlags );
		UBO_MORPHING( writer, MorphingUbo::BindingPoint, 0, flags.programFlags );

		auto inPosition = writer.declInput< Vec4 >( "inPosition"
			, RenderPass::VertexInputs::PositionLocation );
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
		auto inPosition2 = writer.declInput< Vec4 >( "inPosition2"
			, RenderPass::VertexInputs::Position2Location
			, checkFlag( flags.programFlags, ProgramFlag::eMorphing ) );
		auto in = writer.getIn();

		// Outputs
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto curPosition = writer.declLocale( "curPosition"
					, vec4( inPosition.xyz(), 1.0_f ) );

				if ( checkFlag( flags.programFlags, ProgramFlag::eSkinning ) )
				{
					auto curMtxModel = writer.declLocale( "curMtxModel"
						, SkinningUbo::computeTransform( skinningData, writer, flags.programFlags ) );
				}
				else if ( checkFlag( flags.programFlags, ProgramFlag::eInstantiation ) )
				{
					auto curMtxModel = writer.declLocale( "curMtxModel"
						, inTransform );
				}
				else
				{
					auto curMtxModel = writer.declLocale( "curMtxModel"
						, c3d_curMtxModel );
				}

				auto curMtxModel = writer.getVariable< Mat4 >( "curMtxModel" );

				if ( checkFlag( flags.programFlags, ProgramFlag::eMorphing ) )
				{
					curPosition = vec4( sdw::mix( curPosition.xyz(), inPosition2.xyz(), vec3( c3d_time ) ), 1.0_f );
				}

				out.vtx.position = c3d_projection * curMtxModel * curPosition;
			} );
		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr VoxelizePass::doGetBillboardShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		VertexWriter writer;

		// Shader inputs
		auto inPosition = writer.declInput< Vec4 >( "inPosition", 0u );
		auto center = writer.declInput< Vec3 >( "center", 2u );
		UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0u );
		UBO_SCENE( writer, SceneUbo::BindingPoint, 0u );
		UBO_MODEL_MATRIX( writer, ModelMatrixUbo::BindingPoint, 0u );
		UBO_MODEL( writer, ModelUbo::BindingPoint, 0 );
		UBO_BILLBOARD( writer, BillboardUbo::BindingPoint, 0 );
		auto in = writer.getIn();

		// Outputs
		auto out = writer.getOut();

		writer.implementFunction< Void >( "main"
			, [&]()
			{
				auto curBbcenter = writer.declLocale( "curBbcenter"
					, ( c3d_curMtxModel * vec4( center, 1.0_f ) ).xyz() );
				auto curToCamera = writer.declLocale( "curToCamera"
					, c3d_cameraPosition.xyz() - curBbcenter );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );
				auto right = writer.declLocale( "right"
					, vec3( c3d_curView[0][0], c3d_curView[1][0], c3d_curView[2][0] ) );

				if ( checkFlag( flags.programFlags, ProgramFlag::eSpherical ) )
				{
					writer.declLocale( "up"
						, vec3( c3d_curView[0][1], c3d_curView[1][1], c3d_curView[2][1] ) );
				}
				else
				{
					right = normalize( vec3( right.x(), 0.0_f, right.z() ) );
					writer.declLocale( "up"
						, vec3( 0.0_f, 1.0f, 0.0f ) );
				}

				auto up = writer.getVariable< Vec3 >( "up" );

				auto width = writer.declLocale( "width"
					, c3d_dimensions.x() );
				auto height = writer.declLocale( "height"
					, c3d_dimensions.y() );

				if ( checkFlag( flags.programFlags, ProgramFlag::eFixedSize ) )
				{
					width = c3d_dimensions.x() / c3d_clipInfo.x();
					height = c3d_dimensions.y() / c3d_clipInfo.y();
				}

				auto outPosition = writer.declLocale( "outPosition"
					, vec4( curBbcenter
						+ right * inPosition.x() * width
						+ up * inPosition.y() * height
						, 1.0_f ) );
				outPosition = c3d_projection * outPosition;
				out.vtx.position = outPosition;
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr VoxelizePass::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		GeometryWriter writer;
		writer.inputLayout( ast::stmt::InputLayout::eTriangleList );
		writer.outputLayout( ast::stmt::OutputLayout::eTriangleStrip, 3u );

		auto pxl_voxelVisibility = writer.declImage< WFImg3DR32 >( "pxl_voxelVisibility", 0u, 1u );

		// Shader inputs
		auto in = writer.getIn();

		// Outputs
		uint32_t index = 0u;
		auto geo_position = writer.declOutput< Vec4 >( "geo_position", index++ );
		auto geo_minAabb = writer.declOutput< Vec3 >( "geo_minAabb", index++, uint32_t( sdw::var::Flag::eFlat ) );
		auto geo_maxAabb = writer.declOutput< Vec3 >( "geo_maxAabb", index++, uint32_t( sdw::var::Flag::eFlat ) );
		auto out = writer.getOut();

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto faceNormal = writer.declLocale( "faceNormal"
					, normalize( cross( in.vtx[1].position.xyz() - in.vtx[0].position.xyz()
						, in.vtx[2].position.xyz() - in.vtx[0].position.xyz() ) ) );
				auto faceN = writer.declLocale( "faceN"
					, abs( faceNormal ) );

				auto imgSize = writer.declLocale( "imgSize"
					, vec3( imageSize( pxl_voxelVisibility ) ) );
				auto pixelSize = writer.declLocale( "pixelSize"
					, vec3( 1.0_f ) / imgSize );
				auto pixelDiagonal = writer.declLocale< Float >( "pixelDiagonal"
					, Float{ sqrt( 3.0f ) } * pixelSize.x() );

				auto vertPosition = writer.declLocaleArray< Vec4 >( "vertPosition", 3u );
				auto edges = writer.declLocaleArray< Vec3 >( "edges", 3u );
				auto edgeNormals = writer.declLocaleArray< Vec3 >( "edgeNormals", 3u );
				auto minAABB = writer.declLocale< Vec3 >( "minAABB"
					, vec3( 2.0_f, 2.0f, 2.0f ) );
				auto maxAABB = writer.declLocale< Vec3 >( "maxAABB"
					, vec3( -2.0_f, -2.0f, -2.0f ) );

				FOR( writer, UInt, i, 0_u, i < 3_u, ++i )
				{
					vertPosition[i] = in.vtx[i].position;
					edges[i] = normalize( ( in.vtx[( i + 1 ) % 3].position.xyz() / vec3( in.vtx[( i + 1 ) % 3].position.w() ) )
						- ( in.vtx[i].position.xyz() / vec3( in.vtx[i].position.w() ) ) );
					edgeNormals[i] = normalize( cross( edges[i], faceNormal ) );
					minAABB = min( minAABB, vertPosition[i].xyz() );
					maxAABB = max( maxAABB, vertPosition[i].xyz() );
				}
				ROF;

				// calculating on which plane this triangle will be projected. Which value is maximum ? x=0, y=1, z=2
				auto maxIndex = writer.declLocale( "maxIndex"
					, writer.ternary( faceN.y() > faceN.x()
						, writer.ternary( faceN.z() > faceN.y(), 2_u, 1_u )
						, writer.ternary( faceN.z() > faceN.x(), 2_u, 0_u ) ) );

				minAABB -= pixelSize;
				maxAABB += pixelSize;

				auto biSector = writer.declLocale< Vec3 >( "biSector" );
				// project triangle on xy, yz or yz plane where it's visible most
				// also - calculate data for conservative rasterization
				FOR( writer, UInt, i, 0_u, i < 3_u, ++i )
				{
				  // calculate bisector for conservative rasterization
					biSector = pixelDiagonal * ( ( edges[( i + 2 ) % 3] / dot( edges[( i + 2 ) % 3], edgeNormals[i] ) ) + ( edges[i] / dot( edges[i], edgeNormals[( i + 2 ) % 3] ) ) );
					geo_position = vec4( vertPosition[i].xyz() / vertPosition[i].w() + biSector, 1 );

					SWITCH( writer, maxIndex )
					{
						CASE( 0u )
						{
							out.vtx.position = vec4( vertPosition[i].yz() + biSector.yz(), 0, vertPosition[i].w() );
							writer.caseBreakStmt();
						}
						ESAC;
						CASE( 1u )
						{
							out.vtx.position = vec4( vertPosition[i].xz() + biSector.xz(), 0, vertPosition[i].w() );
							writer.caseBreakStmt();
						}
						ESAC;
						CASE( 2u )
						{
							out.vtx.position = vec4( vertPosition[i].xy() + biSector.xy(), 0, vertPosition[i].w() );
							writer.caseBreakStmt();
						}
						ESAC;
					}
					HCTIWS;

					geo_minAabb = minAABB;
					geo_maxAabb = maxAABB;
					EmitVertex( writer );
				}
				ROF;

				EndPrimitive( writer );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr VoxelizePass::doGetPixelShaderSource( PipelineFlags const & flags )const
	{
		using namespace sdw;
		FragmentWriter writer;
		auto & renderSystem = *getEngine()->getRenderSystem();

		auto pxl_voxelVisibility = writer.declImage< WFImg3DR32 >( "pxl_voxelVisibility", 0u, 1u );

		uint32_t index = 0u;
		auto geo_position = writer.declInput< Vec3 >( "geo_position", index++ );
		auto geo_minAabb = writer.declInput< Vec3 >( "geo_minAabb", index++, uint32_t( sdw::var::Flag::eFlat ) );
		auto geo_maxAabb = writer.declInput< Vec3 >( "geo_maxAabb", index++, uint32_t( sdw::var::Flag::eFlat ) );
		auto in = writer.getIn();

		auto pxl_fragColor( writer.declOutput< Vec4 >( "pxl_fragColor", 0 ) );

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				IF( writer
					, any( lessThan( geo_position, geo_minAabb ) )
					|| any( lessThan( geo_maxAabb, geo_position ) ) )
				{
					writer.discard();
				}
				FI;

				auto texcoord = writer.declLocale( "texcoord"
					, geo_position * 0.5 + vec3( 0.5_f ) );
				imageStore( pxl_voxelVisibility
					, imageSize( pxl_voxelVisibility ) * ivec3( texcoord )
					, 1.0_f );
				pxl_fragColor = vec4( 1.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	ShaderPtr VoxelizePass::doGetPhongPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	ShaderPtr VoxelizePass::doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	ShaderPtr VoxelizePass::doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	//*********************************************************************************************
}
