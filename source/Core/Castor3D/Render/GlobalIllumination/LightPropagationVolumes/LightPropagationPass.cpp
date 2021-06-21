#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/GraphContext.hpp>

#include <numeric>
#include <random>

CU_ImplementCUSmartPtr( castor3d, LightPropagationPass )

using namespace castor;

namespace castor3d
{
	namespace
	{
		std::unique_ptr< ast::Shader > getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			auto inPosition = writer.declInput< Vec3 >( "inPosition", 0u );

			auto outCellIndex = writer.declOutput< IVec3 >( "outCellIndex", 0u );
			auto out = writer.getOut();

			UBO_LPVGRIDCONFIG( writer, LightPropagationPass::LpvGridUboIdx, 0u, true );

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					outCellIndex = ivec3( inPosition );
					auto screenPos = writer.declLocale( "screenPos"
						, ( inPosition.xy() + 0.5_f ) / c3d_lpvGridData.gridSize.xy() * 2.0_f - 1.0_f );
					out.vtx.position = vec4( screenPos, 0.0, 1.0 );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getGeometryProgram()
		{
			using namespace sdw;
			GeometryWriter writer;
			writer.inputLayout( ast::stmt::InputLayout::ePointList );
			writer.outputLayout( ast::stmt::OutputLayout::ePointList, 1u );

			auto inCellIndex = writer.declInputArray< IVec3 >( "inCellIndex", 0u, 1u );
			auto in = writer.getIn();

			auto outCellIndex = writer.declOutput< IVec3 >( "outCellIndex", 0u );
			auto out = writer.getOut();

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					out.vtx.position = in.vtx[0].position;
					out.vtx.pointSize = 1.0f;
					out.layer = inCellIndex[0].z();

					outCellIndex = inCellIndex[0];

					EmitVertex( writer );
					EndPrimitive( writer );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getPixelProgram( bool occlusion )
		{
			using namespace sdw;
			FragmentWriter writer;

			/*Spherical harmonics coefficients - precomputed*/
			auto SH_C0 = writer.declConstant( "SH_C0"
				, Float{ 1.0f / ( 2.0f * sqrt( castor::Pi< float > ) ) } );
			auto SH_C1 = writer.declConstant( "SH_C1"
				, Float{ sqrt( 3.0f / castor::Pi< float > ) / 2.0f } );

			/*Cosine lobe coeff*/
			auto SH_cosLobe_C0 = writer.declConstant( "SH_cosLobe_C0"
				, Float{ sqrt( castor::Pi< float > ) / 2.0f } );
			auto SH_cosLobe_C1 = writer.declConstant( "SH_cosLobe_C1"
				, Float{ sqrt( castor::Pi< float > ) / 3.0f } );

			auto directFaceSubtendedSolidAngle = writer.declConstant( "directFaceSubtendedSolidAngle"
				, 0.12753712_f ); // 0.4006696846f / Pi
			auto sideFaceSubtendedSolidAngle = writer.declConstant( "sideFaceSubtendedSolidAngle"
				, 0.13478556_f ); // 0.4234413544f / Pi
			auto propDirections = writer.declConstantArray( "propDirections"
				, std::vector< IVec3 >
				{
					//+Z
					ivec3( 0_i, 0_i, 1_i ),
					//-Z
					ivec3( 0_i, 0_i, -1_i ),
					//+X
					ivec3( 1_i, 0_i, 0_i ),
					//-X
					ivec3( -1_i, 0_i, 0_i ),
					//+Y
					ivec3( 0_i, 1_i, 0_i ),
					//-Y
					ivec3( 0_i, -1_i, 0_i ),
				} );

			//Sides of the cell - right, top, left, bottom
			auto cellSides = writer.declConstantArray( "cellSides"
				, std::vector< IVec2 >
				{
					ivec2( 1_i, 0_i ),
					ivec2( 0_i, 1_i ),
					ivec2( -1_i, 0_i ),
					ivec2( 0_i, -1_i ),
				} );

			UBO_LPVGRIDCONFIG( writer, LightPropagationPass::LpvGridUboIdx, 0u, true );
			auto c3d_lpvGridR = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Grid" ), LightPropagationPass::RLpvGridIdx, 0u );
			auto c3d_lpvGridG = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Grid" ), LightPropagationPass::GLpvGridIdx, 0u );
			auto c3d_lpvGridB = writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Grid" ), LightPropagationPass::BLpvGridIdx, 0u );
			auto c3d_geometryVolume = writer.declSampledImage< FImg3DRgba16 >( "c3d_geometryVolume", LightPropagationPass::GpGridIdx, 0u, occlusion );

			auto outLpvAccumulatorR = writer.declOutput< Vec4 >( "outLpvAccumulatorR", LightPropagationPass::RLpvAccumulatorIdx );
			auto outLpvAccumulatorG = writer.declOutput< Vec4 >( "outLpvAccumulatorG", LightPropagationPass::GLpvAccumulatorIdx );
			auto outLpvAccumulatorB = writer.declOutput< Vec4 >( "outLpvAccumulatorB", LightPropagationPass::BLpvAccumulatorIdx );
			auto outLpvNextStepR = writer.declOutput< Vec4 >( "outLpvNextStepR", LightPropagationPass::RLpvNextStepIdx );
			auto outLpvNextStepG = writer.declOutput< Vec4 >( "outLpvNextStepG", LightPropagationPass::GLpvNextStepIdx );
			auto outLpvNextStepB = writer.declOutput< Vec4 >( "outLpvNextStepB", LightPropagationPass::BLpvNextStepIdx );

			auto inCellIndex = writer.declInput< IVec3 >( "inCellIndex", 0u );
			auto in = writer.getIn();

			// no normalization
			auto evalSH_direct = writer.implementFunction< Vec4 >( "evalSH_direct"
				, [&]( Vec3 direction )
				{
					writer.returnStmt( vec4( SH_C0
						, -SH_C1 * direction.y()
						, SH_C1 * direction.z()
						, -SH_C1 * direction.x() ) );
				}
				, InVec3{ writer, "direction" } );

			// no normalization
			auto evalCosineLobeToDir_direct = writer.implementFunction< Vec4 >( "evalCosineLobeToDir_direct"
				, [&]( Vec3 direction )
				{
					writer.returnStmt( vec4( SH_cosLobe_C0
						, -SH_cosLobe_C1 * direction.y()
						, SH_cosLobe_C1 * direction.z()
						, -SH_cosLobe_C1 * direction.x() ) );
				}
				, InVec3{ writer, "direction" } );

			//Get side direction
			auto getEvalSideDirection = writer.implementFunction< Vec3 >( "getEvalSideDirection"
				, [&]( Int index
					, IVec3 orientation )
				{
					const float smallComponent = 1.0f / sqrt( 5.0f );
					const float bigComponent = 2.0f / sqrt( 5.0f );

					auto tmp = writer.declLocale( "tmp"
						, vec3( writer.cast< Float >( cellSides[index].x() ) * smallComponent
							, writer.cast< Float >( cellSides[index].y() ) * smallComponent
							, bigComponent ) );
					writer.returnStmt( vec3( orientation ) * tmp );
				}
				, InInt{ writer, "index" }
				, InIVec3{ writer, "orientation" } );

			auto getReprojSideDirection = writer.implementFunction< Vec3 >( "getReprojSideDirection"
				, [&]( Int index
					, IVec3 orientation )
				{
					writer.returnStmt( vec3( orientation.x() * cellSides[index].x()
						, orientation.y() * cellSides[index].y()
						, 0 ) );
				}
				, InInt{ writer, "index" }
				, InIVec3{ writer, "orientation" } );

			float occlusionAmplifier = 1.0f;

			auto propagate = writer.implementFunction< Void >( "propagate"
				, [&]( Vec4 shR
					, Vec4 shG
					, Vec4 shB )
				{
					FOR( writer, Int, neighbour, 0, neighbour < 6, neighbour++ )
					{
						auto RSHcoeffsNeighbour = writer.declLocale( "RSHcoeffsNeighbour"
							, vec4( 0.0_f ) );
						auto GSHcoeffsNeighbour = writer.declLocale( "GSHcoeffsNeighbour"
							, vec4( 0.0_f ) );
						auto BSHcoeffsNeighbour = writer.declLocale( "BSHcoeffsNeighbour"
							, vec4( 0.0_f ) );
						//Get main direction
						auto mainDirection = writer.declLocale( "mainDirection"
							, propDirections[neighbour] );
						//get neighbour cell indexindex
						auto neighbourGScellIndex = writer.declLocale( "neighbourGScellIndex"
							, inCellIndex - mainDirection );
						//Load sh coeffs
						RSHcoeffsNeighbour = c3d_lpvGridR.fetch( neighbourGScellIndex, 0_i );
						GSHcoeffsNeighbour = c3d_lpvGridG.fetch( neighbourGScellIndex, 0_i );
						BSHcoeffsNeighbour = c3d_lpvGridB.fetch( neighbourGScellIndex, 0_i );

						auto occlusionValue = writer.declLocale( "occlusionValue"
							, 1.0_f ); // no occlusion

						if ( occlusion )
						{
							auto occCoord = writer.declLocale( "occCoord"
								, ( vec3( neighbourGScellIndex.xyz() ) + 0.5_f * vec3( mainDirection ) ) / c3d_lpvGridData.gridSize );
							auto occCoeffs = writer.declLocale( "occCoeffs"
								, c3d_geometryVolume.sample( occCoord ) );
							occlusionValue = 1.0 - clamp( occlusionAmplifier * dot( occCoeffs, evalSH_direct( vec3( -mainDirection ) ) ), 0.0_f, 1.0_f );
						}

						auto occludedDirectFaceContribution = writer.declLocale( "occludedDirectFaceContribution"
							, occlusionValue * directFaceSubtendedSolidAngle );

						auto mainDirectionCosineLobeSH = writer.declLocale( "mainDirectionCosineLobeSH"
							, evalCosineLobeToDir_direct( vec3( mainDirection ) ) );
						auto mainDirectionSH = writer.declLocale( "mainDirectionSH"
							, evalSH_direct( vec3( mainDirection ) ) );
						shR += occludedDirectFaceContribution * max( 0.0_f, dot( RSHcoeffsNeighbour, mainDirectionSH ) ) * mainDirectionCosineLobeSH;
						shG += occludedDirectFaceContribution * max( 0.0_f, dot( GSHcoeffsNeighbour, mainDirectionSH ) ) * mainDirectionCosineLobeSH;
						shB += occludedDirectFaceContribution * max( 0.0_f, dot( BSHcoeffsNeighbour, mainDirectionSH ) ) * mainDirectionCosineLobeSH;

						//Now we have contribution for the neighbour's cell in the main direction -> need to do reprojection 
						//Reprojection will be made only onto 4 faces (acctually we need to take into account 5 faces but we already have the one in the main direction)
						FOR( writer, Int, face, 0, face < 4, face++ )
						{
							//Get the direction to the face
							auto evalDirection = writer.declLocale( "evalDirection"
								, getEvalSideDirection( face, mainDirection ) );
							//Reprojected direction
							auto reprojDirection = writer.declLocale( "reprojDirection"
								, getReprojSideDirection( face, mainDirection ) );

							if ( occlusion )
							{
								auto occCoord = writer.declLocale( "occCoord"
									, ( vec3( neighbourGScellIndex.xyz() ) + 0.5_f * evalDirection ) / c3d_lpvGridData.gridSize );
								auto occCoeffs = writer.declLocale( "occCoeffs"
									, c3d_geometryVolume.sample( occCoord ) );
								occlusionValue = 1.0 - clamp( occlusionAmplifier * dot( occCoeffs, evalSH_direct( -evalDirection ) ), 0.0_f, 1.0_f );
							}

							auto occludedSideFaceContribution = writer.declLocale( "occludedSideFaceContribution"
								, occlusionValue * sideFaceSubtendedSolidAngle );

							//Get sh coeff
							auto reprojDirectionCosineLobeSH = writer.declLocale( "reprojDirectionCosineLobeSH"
								, evalCosineLobeToDir_direct( reprojDirection ) );
							auto evalDirectionSH = writer.declLocale( "evalDirectionSH"
								, evalSH_direct( evalDirection ) );

							shR += occludedSideFaceContribution * max( 0.0_f, dot( RSHcoeffsNeighbour, evalDirectionSH ) ) * reprojDirectionCosineLobeSH;
							shG += occludedSideFaceContribution * max( 0.0_f, dot( GSHcoeffsNeighbour, evalDirectionSH ) ) * reprojDirectionCosineLobeSH;
							shB += occludedSideFaceContribution * max( 0.0_f, dot( BSHcoeffsNeighbour, evalDirectionSH ) ) * reprojDirectionCosineLobeSH;
						}
						ROF;
					}
					ROF;
				}
				, OutVec4{ writer, "shR" }
				, OutVec4{ writer, "shG" }
				, OutVec4{ writer, "shB" } );

			writer.implementMain( [&]()
				{
					auto shR = writer.declLocale( "shR"
						, vec4( 0.0_f ) );
					auto shG = writer.declLocale( "shG"
						, vec4( 0.0_f ) );
					auto shB = writer.declLocale( "shB"
						, vec4( 0.0_f ) );

					propagate( shR, shG, shB );

					outLpvAccumulatorR = shR;
					outLpvAccumulatorG = shG;
					outLpvAccumulatorB = shB;

					outLpvNextStepR = shR;
					outLpvNextStepG = shG;
					outLpvNextStepB = shB;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ashes::VertexBufferPtr< castor::Point3f > createVertexBuffer( RenderDevice const & device
			, castor::String const & name
			, uint32_t gridSize )
		{
			auto bufferSize = gridSize * gridSize * gridSize;
			auto vertexBuffer = makeVertexBuffer< castor::Point3f >( device
				, bufferSize
				, 0u
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, name );

			if ( auto buffer = vertexBuffer->lock( 0u, bufferSize, 0u ) )
			{
				for ( uint32_t d = 0; d < gridSize; d++ )
				{
					for ( uint32_t c = 0; c < gridSize; c++ )
					{
						for ( uint32_t r = 0; r < gridSize; r++ )
						{
							*buffer = castor::Point3f{ float( r ), float( c ), float( d ) };
							++buffer;
						}
					}
				}

				vertexBuffer->flush( 0u, 4u );
				vertexBuffer->unlock();
			}

			return vertexBuffer;
		}

		crg::pp::Config getConfig( ashes::PipelineShaderStageCreateInfoArray const & stages )
		{
			crg::pp::Config result;
			result.programs = { crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) };
			return result;
		}
	}

	//*********************************************************************************************

	LightPropagationPass::PipelineHolder::PipelineHolder( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, crg::pp::Config config
		, uint32_t gridSize
		, BlendMode blendMode )
		: m_holder{ pass, context, graph, std::move( config ), VK_PIPELINE_BIND_POINT_GRAPHICS, 1u }
		, m_gridSize{ gridSize }
		, m_blendMode{ blendMode }
	{
	}

	void LightPropagationPass::PipelineHolder::initialise( VkRenderPass renderPass )
	{
		m_renderPass = renderPass;
		m_holder.initialise();
		doCreatePipeline( 0u );
	}

	void LightPropagationPass::PipelineHolder::recordInto( VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		m_holder.recordInto( commandBuffer, index );
	}

	void LightPropagationPass::PipelineHolder::doCreatePipeline( uint32_t index )
	{
		ashes::PipelineVertexInputStateCreateInfo vertexState{ 0u
			, { { 0u
				, sizeof( castor::Point3f )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u
				, 0u
				, VK_FORMAT_R32G32B32_SFLOAT
				, 0u } } };
		VkViewport viewport{ 0.0f, 0.0f, float( m_gridSize ), float( m_gridSize ) };
		VkRect2D scissor{ 0, 0, m_gridSize, m_gridSize };
		ashes::PipelineViewportStateCreateInfo viewportState{ 0u
			, 1u
			, ashes::VkViewportArray{ viewport }
			, 1u
			, ashes::VkScissorArray{ scissor } };
		auto blendState = SceneRenderPass::createBlendState( m_blendMode, m_blendMode, 6u );
		VkPipelineInputAssemblyStateCreateInfo iaState{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
			, nullptr
			, 0u
			, VK_PRIMITIVE_TOPOLOGY_POINT_LIST
			, VK_FALSE };
		VkPipelineMultisampleStateCreateInfo msState{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO
			, nullptr
			, 0u
			, VK_SAMPLE_COUNT_1_BIT
			, VK_FALSE
			, 0.0f
			, nullptr
			, VK_FALSE
			, VK_FALSE };
		VkPipelineRasterizationStateCreateInfo rsState{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
			, nullptr
			, 0u
			, VK_FALSE
			, VK_FALSE
			, VK_POLYGON_MODE_FILL
			, VK_CULL_MODE_NONE
			, VK_FRONT_FACE_COUNTER_CLOCKWISE
			, VK_FALSE
			, 0.0f
			, 0.0f
			, 0.0f
			, 0.0f };
		VkPipelineDepthStencilStateCreateInfo dsState{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO
			, nullptr
			, 0u
			, VK_FALSE
			, VK_FALSE };
		VkPipelineViewportStateCreateInfo vpState = viewportState;
		VkPipelineVertexInputStateCreateInfo viState = vertexState;
		VkPipelineColorBlendStateCreateInfo cbState = blendState;
		auto & program = m_holder.getProgram( index );
		auto & pipeline = m_holder.getPipeline( index );
		VkGraphicsPipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO
			, nullptr
			, 0u
			, uint32_t( program.size() )
			, program.data()
			, &viState
			, &iaState
			, nullptr
			, &vpState
			, &rsState
			, &msState
			, &dsState
			, &cbState
			, nullptr
			, m_holder.getPipelineLayout()
			, m_renderPass
			, 0u
			, VK_NULL_HANDLE
			, 0u };
		auto res = m_holder.getContext().vkCreateGraphicsPipelines( m_holder.getContext().device
			, m_holder.getContext().cache
			, 1u
			, &createInfo
			, m_holder.getContext().allocator
			, &pipeline );
		crg::checkVkResult( res, m_holder.getPass().name + " - Pipeline creation" );
		crgRegisterObject( m_holder.getContext(), m_holder.getPass().name, pipeline );
	}

	//*********************************************************************************************

	LightPropagationPass::LightPropagationPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, bool occlusion
		, uint32_t gridSize
		, BlendMode blendMode )
		: castor::Named{ pass.name }
		, crg::RenderPass{ pass
			, context
			, graph
			, { gridSize, gridSize }
			, 1u }
		, m_device{ device }
		, m_gridSize{ gridSize }
		, m_vertexBuffer{ createVertexBuffer( device, getName(), m_gridSize ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, getName()
			, getVertexProgram() }
		, m_geometryShader{ VK_SHADER_STAGE_GEOMETRY_BIT
			, getName()
			, getGeometryProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT
			, getName()
			, getPixelProgram( occlusion ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_geometryShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_holder{ pass
			, context
			, graph
			, getConfig( m_stages )
			, gridSize
			, blendMode }
	{
	}

	void LightPropagationPass::doSubInitialise()
	{
		m_holder.initialise( getRenderPass() );
	}

	void LightPropagationPass::doSubRecordInto( VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		m_holder.recordInto( commandBuffer, index );
		auto vplCount = m_gridSize * m_gridSize * m_gridSize;
		VkDeviceSize offset{};
		VkBuffer vertexBuffer = m_vertexBuffer->getBuffer();
		m_context.vkCmdBindVertexBuffers( commandBuffer, 0u, 1u, &vertexBuffer, &offset );
		m_context.vkCmdDraw( commandBuffer, vplCount, 1u, 0u, 0u );
	}

	void LightPropagationPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_geometryShader );
		visitor.visit( m_pixelShader );
	}
}
