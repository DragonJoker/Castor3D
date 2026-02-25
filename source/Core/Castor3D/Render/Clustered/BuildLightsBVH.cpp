#include "Castor3D/Render/Clustered/BuildLightsBVH.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <limits>

namespace c3d
{
	//*********************************************************************************************

	namespace lgtbvh
	{
		enum class Bindings
		{
			eClusters,
			eAllLightsAABB,
			eLightIndices,
			eLightBVH,
		};

		static uint32_t constexpr NumThreads = 32u * 16u;
		static float constexpr FltMax = std::numeric_limits< float >::max();

		static ShaderPtr createShader( RenderDevice const & device
			, LightType lightType
			, bool bottomLevel )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			auto c3d_numLevelNodes = writer.declConstantArray< sdw::UInt >( "c3d_numLevelNodes"
				, { 1_u			/* Level 0 ( 32^0 ) */
				, 32_u			/* Level 1 ( 32^1 ) */
				, 1024_u		/* Level 2 ( 32^2 ) */
				, 32768_u		/* Level 3 ( 32^3 ) */
				, 1048576_u		/* Level 4 ( 32^4 ) */
				, 33554432_u	/* Level 5 ( 32^5 ) */
				, 1073741824_u	/* Level 6 ( 32^6 ) */ } );
			auto c3d_firstNodeIndex = writer.declConstantArray< sdw::UInt >( "c3d_firstNodeIndex"
				, { 0_u			/* Level 0 */
				, 1_u			/* Level 1 */
				, 33_u			/* Level 2 */
				, 1057_u		/* Level 3 */
				, 33825_u		/* Level 4 */
				, 1082401_u		/* Level 5 */
				, 34636833_u	/* Level 6 */ } );

			// Inputs
			C3D_AllLightsAABBEx( writer
				, Bindings::eAllLightsAABB
				, 0u
				, bottomLevel );
			C3D_Clusters( writer
				, Bindings::eClusters
				, 0u );
			C3D_LightIndices( writer
				, Bindings::eLightIndices
				, 0u );
			C3D_LightBVH( writer
				, Bindings::eLightBVH
				, 0u );

			sdw::PushConstantBuffer pcb{ writer
				, "C3D_DrawData"
				, "c3d_drawData"
				, sdw::type::MemoryLayout::eC
				, !bottomLevel };
			auto c3d_childLevel = pcb.declMember< sdw::UInt >( "childLevel", !bottomLevel );
			pcb.end();

			auto gsAABBMin = writer.declSharedVariable< sdw::Vec4 >( "gsAABBMin", NumThreads );
			auto gsAABBMax = writer.declSharedVariable< sdw::Vec4 >( "gsAABBMax", NumThreads );

			auto logStepReduction = writer.implementFunction< sdw::Void >( "logStepReduction"
				, [&device, &writer, &gsAABBMin, &gsAABBMax]( sdw::UInt const & groupIndex )
				{
					auto mod32GroupIndex = writer.declLocale( "mod32GroupIndex"
						, groupIndex % 32_u );

					if ( device.renderSystem.getGpuInformations().isNVIDIA() )
					{
						auto reduceIndex = writer.declLocale( "reduceIndex"
							, 32_u >> 1_u );

						sdwWHILE( writer, mod32GroupIndex < reduceIndex )
						{
							gsAABBMin[groupIndex] = min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
							gsAABBMax[groupIndex] = max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );

							reduceIndex >>= 1_u;
						}
						sdwELIHW
					}
					else
					{
						shader::groupMemoryBarrierWithGroupSync( writer );

						sdwIF( writer, mod32GroupIndex == 0_u )
						{
							for ( uint32_t i = 1u; i < 32u; ++i )
							{
								gsAABBMin[groupIndex] = min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + i] );
								gsAABBMax[groupIndex] = max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + i] );
							}
						}
						sdwFI

						shader::groupMemoryBarrierWithGroupSync( writer );
					}
				}
				, sdw::InUInt{ writer, "groupIndex" } );

			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&c3d_clustersData, &c3d_numLevelNodes, &c3d_firstNodeIndex, &c3d_lightBVH, &c3d_lightIndices
					, &c3d_childLevel, &c3d_allLightsAABB
					, &writer, &logStepReduction, &gsAABBMin, &gsAABBMax
					, &lightType, &bottomLevel]( sdw::ComputeIn const & in )
				{
					auto const & groupIndex = in.localInvocationIndex;
					auto threadIndex = in.globalInvocationID.x();
					auto aabbMin = writer.declLocale( "aabbMin"
						, vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f ) );
					auto aabbMax = writer.declLocale( "aabbMax"
						, vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f ) );

					sdwIF( writer, groupIndex == 0_u )
					{
						sdwFOR( writer, sdw::UInt, n, 0_u, n < NumThreads, ++n )
						{
							gsAABBMin[n] = aabbMin;
							gsAABBMax[n] = aabbMax;
						}
						sdwROF
					}
					sdwFI

					shader::groupMemoryBarrierWithGroupSync( writer );

					// Number of levels in the BVH
					auto numLevels = writer.declLocale( "numLevels"
						, ( lightType == LightType::ePoint
							? c3d_clustersData.pointLightLevels()
							: c3d_clustersData.spotLightLevels() ) );

					auto writeToGlobalMemory = [&]( sdw::UInt const & childLevel
						, sdw::UInt const & currentLevel )
					{
						// The first thread of each warp will write the AABB to global memory.
						sdwIF( writer, threadIndex % 32_u == 0_u )
						{
							// Offset of the node in the BVH at the last level of child nodes.
							auto nodeOffset = writer.declLocale( "nodeOffset"
								, threadIndex / 32_u );

							sdwIF( writer, childLevel < numLevels && nodeOffset < c3d_numLevelNodes[currentLevel - 1_u] )
							{
								auto nodeIndex = writer.declLocale( "nodeIndex"
									, c3d_firstNodeIndex[currentLevel - 1_u] + nodeOffset );
								c3d_lightBVH[nodeIndex] = shader::AABB{ gsAABBMin[groupIndex], gsAABBMax[groupIndex] };
							}
							sdwFI
						}
						sdwFI
					};

					if ( bottomLevel )
					{
						// Compute BVH AABB for lights.
						auto leafIndex = writer.declLocale( "leafIndex"
							, threadIndex );
						auto lightCount = lightType == LightType::ePoint
							? c3d_clustersData.pointLightCount()
							: c3d_clustersData.spotLightCount();

						sdwIF( writer, lightCount > 0_u )
						{
							sdwIF( writer, leafIndex < lightCount )
							{
								auto lightIndex = writer.declLocale( "lightIndex"
									, ( lightType == LightType::ePoint
										? c3d_lightIndices[leafIndex]
										: c3d_clustersData.pointLightCount() + c3d_lightIndices[leafIndex] ) );
								auto aabb = writer.declLocale( "aabb"
									, c3d_allLightsAABB[lightIndex] );

								aabbMin = aabb.min();
								aabbMax = aabb.max();
							}
							sdwELSE
							{
								aabbMin = vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f );
								aabbMax = vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f );
							}
							sdwFI

							gsAABBMin[groupIndex] = aabbMin;
							gsAABBMax[groupIndex] = aabbMax;

							// Log-step reduction is performed warp-syncronous and thus does not require
							// a group sync barrier.
							logStepReduction( groupIndex );

							writeToGlobalMemory( 0_u, numLevels );
						}
						sdwFI
					}
					else
					{
						// Build upper BVH for light BVH.
						auto childOffset = writer.declLocale( "childOffset"
							, threadIndex );

						sdwIF( writer, c3d_childLevel < numLevels && childOffset < c3d_numLevelNodes[c3d_childLevel] )
						{
							auto childIndex = writer.declLocale( "childIndex"
								, c3d_firstNodeIndex[c3d_childLevel] + childOffset );

							aabbMin = c3d_lightBVH[childIndex].min();
							aabbMax = c3d_lightBVH[childIndex].max();

							sdwIF( writer, aabbMin.x() == aabbMax.x()
								&& aabbMin.y() == aabbMax.y()
								&& aabbMin.z() == aabbMax.z() )
							{
								aabbMin = vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f );
								aabbMax = vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f );
							}
							sdwFI
						}
						sdwELSE
						{
							aabbMin = vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f );
							aabbMax = vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f );
						}
						sdwFI

						gsAABBMin[groupIndex] = aabbMin;
						gsAABBMax[groupIndex] = aabbMax;

						// Log-step reduction is performed warp-syncronous and thus does not require
						// a group sync barrier.
						logStepReduction( groupIndex );

						writeToGlobalMemory( c3d_childLevel, c3d_childLevel );
					}
				} );
			return writer.getBuilder().releaseShader();
		}

		class FramePass
			: public crg::RunnablePass
		{
		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, FrustumClusters const & clusters
				, LightType lightType )
				: crg::RunnablePass{ framePass
					, context
					, graph
					, { [this]( uint32_t index ){ doInitialise( index ); }
						, GetPipelineStateCallback( [](){ return crg::getPipelineState( PipelineStageFlags::eComputeShader ); } )
						, [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); }
						, GetPassIndexCallback( [this](){ return doGetPassIndex(); } )
						, IsEnabledCallback( [this](){ return doIsEnabled(); } )
						, IsComputePassCallback( [](){ return true; } ) }
					, crg::ru::Config{ 2u, true /* resettable */ } }
				, m_clusters{ clusters }
				, m_lightCache{ clusters.getCamera().getScene()->getLightCache() }
				, m_lightType{ lightType }
				, m_bottom{ framePass, context, graph, device, true, this, m_lightType }
				, m_top{ framePass, context, graph, device, false, this, m_lightType }
			{
			}

			CRG_API void resetPipeline( uint32_t index )
			{
				resetCommandBuffer( index );
				m_bottom.pipeline.resetPipeline( m_bottom.pipeline.getProgram( index ), index );
				m_top.pipeline.resetPipeline( m_top.pipeline.getProgram( index ), index );
				doCreatePipeline( index, m_bottom );
				doCreatePipeline( index, m_top );
				reRecordCurrent();
			}

		private:
			struct Pipeline
			{
				struct ProgramData
				{
					ProgramData() = default;
					ShaderModule shaderModule{};
					ashes::PipelineShaderStageCreateInfoArray stages{};
				};
				crg::cp::ConfigData cpConfig;
				crg::PipelineHolder pipeline;
				Map< uint32_t, ProgramData > programs;

				Pipeline( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph
					, RenderDevice const & device
					, bool bottomLevel
					, FramePass const * parent
					, LightType lightType )
					: cpConfig{ crg::getDefaultV< InitialiseCallback >()
						, nullptr
						, IsEnabledCallback( [parent]() { return parent->m_clusters.needsLightsUpdate(); } )
						, GetPassIndexCallback( [parent]() { return parent->doGetPassIndex(); } )
						, crg::getDefaultV< RecordCallback >()
						, crg::getDefaultV< RecordCallback >()
						, 1u
						, 1u
						, 1u }
					, pipeline{ framePass
						, context
						, graph
						, crg::pp::Config{}
							.programCreator( { 2u, [this, &device, bottomLevel, lightType]( uint32_t passIndex ){ return doCreateProgram( device, passIndex, bottomLevel, lightType ); } } )
							.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, 4u } )
						, VK_PIPELINE_BIND_POINT_COMPUTE
						, 6u }
				{
				}

			private:
				crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device
					, uint32_t passIndex
					, bool bottomLevel
					, LightType lightType )
				{
					auto [it, res] = programs.try_emplace( passIndex );

					if ( res )
					{
						auto & program = it->second;
						program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT
							, cuT( "BuildLightsBVH/" ) + ( bottomLevel ? String{ cuT( "Bottom/" ) } : String{ cuT( "Top/" ) } ) + getName( lightType )
							, createShader( device, lightType, bottomLevel ) };
						program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, program.shaderModule ) };
					}

					return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
				}
			};

		private:
			FrustumClusters const & m_clusters;
			LightCache const & m_lightCache;
			LightType m_lightType;
			Pipeline m_bottom;
			Pipeline m_top;

		private:
			void doInitialise( uint32_t index )
			{
				m_bottom.pipeline.initialise();
				m_top.pipeline.initialise();
				doCreatePipeline( index, m_bottom );
				doCreatePipeline( index, m_top );
			}

			uint32_t doGetPassIndex()const
			{
				u32 result = {};

				auto totalValues = m_lightCache.getLightsBufferCount( m_lightType );
				if ( auto numChunks = getLightsMortonCodeChunkCount( totalValues );
					numChunks > 1u )
					result += ( ( numChunks - 1u ) % 2u );

				return result;
			}

			bool doIsEnabled()const
			{
				return m_lightCache.getLightsCount( m_lightType ) > 0u
					&& ( ( m_bottom.cpConfig.isEnabled ? ( *m_bottom.cpConfig.isEnabled )() : false )
						|| ( m_top.cpConfig.isEnabled ? ( *m_top.cpConfig.isEnabled )() : false ) );
			}

			void doRecordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				// Build bottom level of the BVH.
				auto maxLeaves = m_lightCache.getLightsBufferCount( m_lightType );
				auto numThreadGroups = divRoundUp( maxLeaves, NumThreads );
				m_bottom.pipeline.recordInto( context, commandBuffer, index );
				context->vkCmdDispatch( commandBuffer, numThreadGroups, 1u, 1u );
				uint32_t maxLevels = FrustumClusters::getNumLevels( maxLeaves );
				doBarriers( context, commandBuffer, index, 0 );

				// Now build upper levels of the BVH.
				if ( maxLevels > 1u )
				{
					m_top.pipeline.recordInto( context, commandBuffer, index );

					for ( uint32_t level = maxLevels - 1u; level > 0; --level )
					{
						doBarriers( context, commandBuffer, index, 1 );
						context->vkCmdPushConstants( commandBuffer, m_top.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 4u, &level );
						uint32_t numChildNodes = FrustumClusters::getNumLevelNodes( level );
						numThreadGroups = divRoundUp( numChildNodes, NumThreads );
						context->vkCmdDispatch( commandBuffer, numThreadGroups, 1u, 1u );
					}
				}

				doBarriers( context, commandBuffer, index, 2 );
			}

			void doBarriers( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t passIndex
				, int idx )const
			{
				for ( auto & [binding, attach] : getPass().getOutputs() )
				{
					auto currentState = context.getAccessState( attach->buffer( passIndex ) );
					context.memoryBarrier( commandBuffer
						, attach->buffer( passIndex )
						, currentState
						, ( ( idx == 2 ) ? ComputeShaderReadState : ComputeShaderReadWriteState )
						, true );
				}
			}

			void doCreatePipeline( uint32_t index
				, Pipeline & pipeline )const
			{
				auto & program = pipeline.pipeline.getProgram( index );
				VkComputePipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO
					, nullptr
					, 0u
					, program.front()
					, pipeline.pipeline.getPipelineLayout()
					, VkPipeline{}
					, 0u };
				pipeline.pipeline.createPipeline( index, createInfo );
			}
		};
	}

	namespace dspbvh
	{
		enum class Bindings
		{
			eMainCamera,
			eClustersCamera,
			eLightBVH,
		};

		static ShaderPtr createDebugDisplayShader( RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			C3D_CameraNamed( writer
				, Main
				, Bindings::eMainCamera
				, 0u );
			C3D_CameraNamed( writer
				, Clusters
				, Bindings::eClustersCamera
				, 0u );
			C3D_LightBVH( writer
				, Bindings::eLightBVH
				, 0u );

			auto numBVHNodes = writer.declConstantArray< sdw::UInt >( "numBVHNodes"
				, std::vector< sdw::UInt >{ 0_u /* Level 0 */
					, 1_u /* Level 1 */
					, 33_u /* Level 2 */
					, 1057_u /* Level 3 */
					, 33825_u /* Level 4 */
					, 1082401_u /* Level 5 */
					, 34636833_u /* Level 6 */ } );
			auto colorPalette = writer.declConstantArray( "colorPalette"
				, std::vector< sdw::Vec4 >{ vec4( 0.25_f, 0.25_f, 1.00_f, 1.0_f )
					, vec4( 0.25_f, 1.00_f, 0.25_f, 1.0_f )
					, vec4( 0.25_f, 1.00_f, 1.00_f, 1.0_f )
					, vec4( 1.00_f, 0.25_f, 0.25_f, 1.0_f )
					, vec4( 1.00_f, 0.25_f, 1.00_f, 1.0_f )
					, vec4( 1.00_f, 1.00_f, 0.25_f, 1.0_f ) } );

			auto getNodeLevel = writer.implementFunction< sdw::UInt >( "getNodeLevel"
				, [&writer, &numBVHNodes]( sdw::UInt const & nodeIndex )
				{
					auto i = writer.declLocale( "i", 0_u );

					sdwWHILE( writer, nodeIndex > numBVHNodes[i] && i < 5_u )
					{
						++i;
					}
					sdwELIHW

					writer.returnStmt( i );
				}
				, sdw::InUInt{ writer, "nodeIndex" } );

			writer.implementEntryPointT< shader::Position4FT, shader::Colour4FT >( [&writer, &c3d_cameraDataMain, &c3d_cameraDataClusters, &c3d_lightBVH, &colorPalette, &getNodeLevel]( sdw::VertexInT< shader::Position4FT > const & in
				, sdw::VertexOutT< shader::Colour4FT > out )
				{
					auto aabb = writer.declLocale( "aabb"
						, c3d_lightBVH[in.instanceIndex] );

					sdwIF( writer, aabb.min().x() == aabb.max().x()
						&& aabb.min().y() == aabb.max().y()
						&& aabb.min().z() == aabb.max().z() )
					{
						out.vtx.position = vec4( -100.0_f );
					}
					sdwELSE
					{
						auto position = writer.declLocale( "position"
							, in.position() );
						position.x() = mix( aabb.min().x(), aabb.max().x(), position.x() );
						position.y() = mix( aabb.min().y(), aabb.max().y(), position.y() );
						position.z() = mix( aabb.min().z(), aabb.max().z(), position.z() );
						// Convert from clusters view position to world position
						position = c3d_cameraDataClusters.curViewToWorld( position );
						position.w() = 1.0_f;
						// Then from world to main camera proj.
						out.vtx.position = c3d_cameraDataMain.worldToCurProj( position );

						out.colour() = colorPalette[getNodeLevel( writer.cast< sdw::UInt >( in.instanceIndex ) )];
					}
					sdwFI
				} );

			writer.implementEntryPointT< shader::Colour4FT, shader::Colour4FT >( []( sdw::FragmentInT< shader::Colour4FT > const & in
				, sdw::FragmentOutT< shader::Colour4FT > const & out )
				{
					out.colour() = in.colour();
				} );

			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	void createBuildLightsBVHPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, BufferBase const & allLightsAABB
		, ClustersLightSortAttachs const & outputSortAttachs
		, BufferBase const & mergePathPartitions
		, BufferBase & pointLightBVH
		, BufferBase & spotLightBVH )
	{
		{
			// Point lights
			auto & point = graph.createPass( "BuildLightsBVH/Point"
				, [&clusters, &device]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto runPass = makeRawUnique< lgtbvh::FramePass >( framePass
						, context
						, runGraph
						, device
						, clusters
						, LightType::ePoint );
					c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
						, runPass->getTimer() );
					return runPass;
				} );
			clusters.getClustersUbo().createPassBinding( point, lgtbvh::Bindings::eClusters );
			point.addImplicit( *mergePathPartitions.getLastAttach(), AccessState{} );
			point.addImplicit( *outputSortAttachs.pointLightMortonCodes, AccessState{} );
			point.addInputStorageT( *allLightsAABB.getLastAttach(), lgtbvh::Bindings::eAllLightsAABB );
			point.addInputStorageT( *outputSortAttachs.pointLightIndices, lgtbvh::Bindings::eLightIndices );
			pointLightBVH.setLastAttach( point.addClearableOutputStorageBufferT( pointLightBVH.bufferViewId, lgtbvh::Bindings::eLightBVH ) );
		}
		{
			// Spot lights
			auto & spot = graph.createPass( "BuildLightsBVH/Spot"
				, [&clusters, &device]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto runPass = makeRawUnique< lgtbvh::FramePass >( framePass
						, context
						, runGraph
						, device
						, clusters
						, LightType::eSpot );
					c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
						, runPass->getTimer() );
					return runPass;
				} );
			clusters.getClustersUbo().createPassBinding( spot, lgtbvh::Bindings::eClusters );
			spot.addImplicit( *outputSortAttachs.spotLightMortonCodes, AccessState{} );
			spot.addInputStorageT( *allLightsAABB.getLastAttach(), lgtbvh::Bindings::eAllLightsAABB );
			spot.addInputStorageT( *outputSortAttachs.spotLightIndices, lgtbvh::Bindings::eLightIndices );
			spotLightBVH.setLastAttach( spot.addClearableOutputStorageBufferT( spotLightBVH.bufferViewId, lgtbvh::Bindings::eLightBVH ) );
		}
	}

	void createDisplayPointLightsBVHProgram( RenderDevice const & device
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, ashes::WriteDescriptorSetArray & writes
		, BufferBase const & pointLightBVH )
	{
		ProgramModule programModule{ "PointLightsBVH", dspbvh::createDebugDisplayShader( device ) };
		program = makeProgramStates( device, programModule );

		mainCameraUbo.addLayoutBindingT( bindings, dspbvh::Bindings::eMainCamera, VK_SHADER_STAGE_VERTEX_BIT );
		clustersCameraUbo.addLayoutBindingT( bindings, dspbvh::Bindings::eClustersCamera, VK_SHADER_STAGE_VERTEX_BIT );
		c3d::addDescriptorSetLayoutBindingT( bindings, dspbvh::Bindings::eLightBVH, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );

		mainCameraUbo.addDescriptorWriteT( writes, dspbvh::Bindings::eMainCamera );
		clustersCameraUbo.addDescriptorWriteT( writes, dspbvh::Bindings::eClustersCamera );
		writes.emplace_back( uint32_t( dspbvh::Bindings::eLightBVH ), 0u, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ pointLightBVH.getBuffer(), 0u, pointLightBVH.getSize() } } );
	}

	void createDisplaySpotLightsBVHProgram( RenderDevice const & device
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, ashes::WriteDescriptorSetArray & writes
		, BufferBase const & spotLightBVH )
	{
		ProgramModule programModule{ "SpotLightsBVH", dspbvh::createDebugDisplayShader( device ) };
		program = makeProgramStates( device, programModule );

		mainCameraUbo.addLayoutBindingT( bindings, dspbvh::Bindings::eMainCamera, VK_SHADER_STAGE_VERTEX_BIT );
		clustersCameraUbo.addLayoutBindingT( bindings, dspbvh::Bindings::eClustersCamera, VK_SHADER_STAGE_VERTEX_BIT );
		c3d::addDescriptorSetLayoutBindingT( bindings, dspbvh::Bindings::eLightBVH, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );

		mainCameraUbo.addDescriptorWriteT( writes, dspbvh::Bindings::eMainCamera );
		clustersCameraUbo.addDescriptorWriteT( writes, dspbvh::Bindings::eClustersCamera );
		writes.emplace_back( uint32_t( dspbvh::Bindings::eLightBVH ), 0u, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ spotLightBVH.getBuffer(), 0u, spotLightBVH.getSize() } } );
	}

	//*********************************************************************************************
}
