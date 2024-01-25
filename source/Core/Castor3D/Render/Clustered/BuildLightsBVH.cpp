#include "Castor3D/Render/Clustered/BuildLightsBVH.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Shaders/GlslAppendBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <limits>

namespace castor3d
{
	//*********************************************************************************************

	namespace lgtbvh
	{
		enum BindingPoints
		{
			eClusters,
			eAllLightsAABB,
			eLightIndices,
			eLightBVH,
		};

		static uint32_t constexpr NumThreads = 32u * 16u;
		static float constexpr FltMax = std::numeric_limits< float >::max();

		static ShaderPtr createShader( RenderDevice const & device
			, ClustersConfig const & config
			, LightType lightType
			, bool bottomLevel )
		{
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

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
				, eAllLightsAABB
				, 0u
				, bottomLevel );
			C3D_Clusters( writer
				, eClusters
				, 0u
				, &config );
			C3D_LightIndicesEx( writer
				, eLightIndices
				, 0u
				, bottomLevel && config.sortLights );
			C3D_LightBVH( writer
				, eLightBVH
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
				, [&]( sdw::UInt const & groupIndex )
				{
					auto mod32GroupIndex = writer.declLocale( "mod32GroupIndex"
						, groupIndex % 32_u );

					if ( config.enableBVHWarpOptimisation )
					{
						auto reduceIndex = writer.declLocale( "reduceIndex"
							, 32_u >> 1_u );

						WHILE( writer, mod32GroupIndex < reduceIndex )
						{
							gsAABBMin[groupIndex] = min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
							gsAABBMax[groupIndex] = max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );

							reduceIndex >>= 1_u;
						}
						ELIHW
					}
					else
					{
						shader::groupMemoryBarrierWithGroupSync( writer );

						IF( writer, mod32GroupIndex == 0_u )
						{
							for ( uint32_t i = 1u; i < 32u; ++i )
							{
								gsAABBMin[groupIndex] = min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + i] );
								gsAABBMax[groupIndex] = max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + i] );
							}
						}
						FI

						shader::groupMemoryBarrierWithGroupSync( writer );
					}
				}
				, sdw::InUInt{ writer, "groupIndex" } );

			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&]( sdw::ComputeIn const & in )
				{
					auto const & groupIndex = in.localInvocationIndex;
					auto threadIndex = in.globalInvocationID.x();
					auto aabbMin = writer.declLocale( "aabbMin"
						, vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f ) );
					auto aabbMax = writer.declLocale( "aabbMax"
						, vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f ) );

					IF( writer, groupIndex == 0_u )
					{
						FOR( writer, sdw::UInt, n, 0_u, n < NumThreads, ++n )
						{
							gsAABBMin[n] = aabbMin;
							gsAABBMax[n] = aabbMax;
						}
						ROF
					}
					FI

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
						IF( writer, threadIndex % 32_u == 0_u )
						{
							// Offset of the node in the BVH at the last level of child nodes.
							auto nodeOffset = writer.declLocale( "nodeOffset"
								, threadIndex / 32_u );

							IF( writer, childLevel < numLevels && nodeOffset < c3d_numLevelNodes[currentLevel - 1_u] )
							{
								auto nodeIndex = writer.declLocale( "nodeIndex"
									, c3d_firstNodeIndex[currentLevel - 1_u] + nodeOffset );
								c3d_lightBVH[nodeIndex] = shader::AABB{ gsAABBMin[groupIndex], gsAABBMax[groupIndex] };
							}
							FI
						}
						FI
					};

					if ( bottomLevel )
					{
						// Compute BVH AABB for lights.
						auto leafIndex = writer.declLocale( "leafIndex"
							, threadIndex );
						auto lightCount = lightType == LightType::ePoint
							? c3d_clustersData.pointLightCount()
							: c3d_clustersData.spotLightCount();

						IF( writer, lightCount > 0_u )
						{
							IF( writer, leafIndex < lightCount )
							{
								auto lightIndex = writer.declLocale( "lightIndex"
									, ( lightType == LightType::ePoint
										? ( config.sortLights ? c3d_lightIndices[leafIndex] : leafIndex )
										: c3d_clustersData.pointLightCount() + ( config.sortLights ? c3d_lightIndices[leafIndex] : leafIndex ) ) );
								auto aabb = writer.declLocale( "aabb"
									, c3d_allLightsAABB[lightIndex] );

								aabbMin = aabb.min();
								aabbMax = aabb.max();
							}
							ELSE
							{
								aabbMin = vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f );
								aabbMax = vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f );
							}
							FI

							gsAABBMin[groupIndex] = aabbMin;
							gsAABBMax[groupIndex] = aabbMax;

							// Log-step reduction is performed warp-syncronous and thus does not require
							// a group sync barrier.
							logStepReduction( groupIndex );

							writeToGlobalMemory( 0_u, numLevels );
						}
						FI
					}
					else
					{
						// Build upper BVH for light BVH.
						auto childOffset = writer.declLocale( "childOffset"
							, threadIndex );

						IF( writer, c3d_childLevel < numLevels && childOffset < c3d_numLevelNodes[c3d_childLevel] )
						{
							auto childIndex = writer.declLocale( "childIndex"
								, c3d_firstNodeIndex[c3d_childLevel] + childOffset );

							aabbMin = c3d_lightBVH[childIndex].min();
							aabbMax = c3d_lightBVH[childIndex].max();

							IF( writer, aabbMin.x() == aabbMax.x()
								&& aabbMin.y() == aabbMax.y()
								&& aabbMin.z() == aabbMax.z() )
							{
								aabbMin = vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f );
								aabbMax = vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f );
							}
							FI
						}
						ELSE
						{
							aabbMin = vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f );
							aabbMax = vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f );
						}
						FI

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
						, GetPipelineStateCallback( [](){ return crg::getPipelineState( VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT ); } )
						, [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); }
						, GetPassIndexCallback( [this](){ return doGetPassIndex(); } )
						, IsEnabledCallback( [this](){ return doIsEnabled(); } )
						, IsComputePassCallback( [](){ return true; } ) }
					, crg::ru::Config{ 6u, true /* resettable */ } }
				, m_clusters{ clusters }
				, m_lightCache{ clusters.getCamera().getScene()->getLightCache() }
				, m_lightType{ lightType }
				, m_bottom{ framePass, context, graph, device, clusters.getConfig(), true, this, m_lightType }
				, m_top{ framePass, context, graph, device, clusters.getConfig(), false, this, m_lightType }
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
				castor::Map< uint32_t, ProgramData > programs;

				Pipeline( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph
					, RenderDevice const & device
					, ClustersConfig const & config
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
							.programCreator( { 6u, [this, &device, &config, bottomLevel, lightType]( uint32_t passIndex ){ return doCreateProgram( device, config, passIndex, bottomLevel, lightType ); } } )
							.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, 4u } )
						, VK_PIPELINE_BIND_POINT_COMPUTE
						, 6u }
				{
				}

			private:
				crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device
					, ClustersConfig const & config
					, uint32_t passIndex
					, bool bottomLevel
					, LightType lightType )
				{
					auto [it, res] = programs.try_emplace( passIndex );

					if ( res )
					{
						auto & program = it->second;
						program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT
							, cuT( "BuildLightsBVH/" ) + ( bottomLevel ? castor::String{ cuT( "Bottom/" ) } : castor::String{ cuT( "Top/" ) } ) + getName( lightType )
							, createShader( device, config, lightType, bottomLevel ) };
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

				if ( m_clusters.getConfig().enableBVHWarpOptimisation )
				{
					result += 3u;
				}

				if ( m_clusters.getConfig().sortLights )
				{
					auto totalValues = m_lightCache.getLightsBufferCount( m_lightType );

					if ( auto numChunks = getLightsMortonCodeChunkCount( totalValues );
						numChunks > 1u )
					{
						result += ( ( numChunks - 1u ) % 2u );
					}

					result += 1u;
				}

				return result;
			}

			bool doIsEnabled()const
			{
				return m_clusters.getConfig().useLightsBVH
					&& m_lightCache.getLightsCount( m_lightType ) > 0u
					&& ( ( m_bottom.cpConfig.isEnabled ? ( *m_bottom.cpConfig.isEnabled )() : false )
						|| ( m_top.cpConfig.isEnabled ? ( *m_top.cpConfig.isEnabled )() : false ) );
			}

			void doRecordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				// Build bottom level of the BVH.
				auto maxLeaves = m_lightCache.getLightsBufferCount( m_lightType );
				auto numThreadGroups = castor::divRoundUp( maxLeaves, NumThreads );
				m_bottom.pipeline.recordInto( context, commandBuffer, index );
				m_context.vkCmdDispatch( commandBuffer, numThreadGroups, 1u, 1u );
				uint32_t maxLevels = FrustumClusters::getNumLevels( maxLeaves );
				doBarriers( context, commandBuffer, index, 0 );

				// Now build upper levels of the BVH.
				if ( maxLevels > 1u )
				{
					m_top.pipeline.recordInto( context, commandBuffer, index );

					for ( uint32_t level = maxLevels - 1u; level > 0; --level )
					{
						doBarriers( context, commandBuffer, index, 1 );
						m_context.vkCmdPushConstants( commandBuffer, m_top.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 4u, &level );
						uint32_t numChildNodes = FrustumClusters::getNumLevelNodes( level );
						numThreadGroups = castor::divRoundUp( numChildNodes, NumThreads );
						m_context.vkCmdDispatch( commandBuffer, numThreadGroups, 1u, 1u );
					}
				}

				doBarriers( context, commandBuffer, index, 2 );
			}

			void doBarriers( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t passIndex
				, int idx )const
			{
				for ( auto & attach : m_pass.buffers )
				{
					auto buffer = attach.buffer;

					if ( !attach.isNoTransition()
						&& attach.isStorageBuffer()
						&& attach.isClearableBuffer() )
					{
						auto currentState = context.getAccessState( buffer.buffer.buffer( passIndex )
							, buffer.range );
						context.memoryBarrier( commandBuffer
							, buffer.buffer.buffer( passIndex )
							, buffer.range
							, currentState.access
							, currentState.pipelineStage
							, ( ( idx == 2 )
								? crg::AccessState{ VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
								: crg::AccessState{ VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } )
							, true );
					}
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

	//*********************************************************************************************

	crg::FramePassArray createBuildLightsBVHPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, RenderDevice const & device
		, FrustumClusters & clusters )
	{
		// Point lights
		auto & point = graph.createPass( "BuildLightsBVH/Point"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< lgtbvh::FramePass >( framePass
					, context
					, graph
					, device
					, clusters
					, LightType::ePoint );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		point.addDependency( *previousPasses.front() );
		clusters.getClustersUbo().createPassBinding( point, lgtbvh::eClusters );
		createInputStoragePassBinding( point, uint32_t( lgtbvh::eAllLightsAABB ), cuT( "C3D_AllLightsAABB" ), clusters.getAllLightsAABBBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( point, uint32_t( lgtbvh::eLightIndices ), cuT( "C3D_PointLightIndices" )
			, { &clusters.getOutputPointLightIndicesBuffer(), &clusters.getInputPointLightIndicesBuffer(), &clusters.getOutputPointLightIndicesBuffer()
				, &clusters.getOutputPointLightIndicesBuffer(), &clusters.getInputPointLightIndicesBuffer(), &clusters.getOutputPointLightIndicesBuffer() }
			, 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( point, uint32_t( lgtbvh::eLightBVH ), cuT( "C3D_PointLightBVH" ), clusters.getPointLightBVHBuffer(), 0u, ashes::WholeSize );

		// Spot lights
		auto & spot = graph.createPass( "BuildLightsBVH/Spot"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< lgtbvh::FramePass >( framePass
					, context
					, graph
					, device
					, clusters
					, LightType::eSpot );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		spot.addDependency( *previousPasses.back() );
		clusters.getClustersUbo().createPassBinding( spot, lgtbvh::eClusters );
		createInputStoragePassBinding( spot, uint32_t( lgtbvh::eAllLightsAABB ), cuT( "C3D_AllLightsAABB" ), clusters.getAllLightsAABBBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( spot, uint32_t( lgtbvh::eLightIndices ), cuT( "C3D_SpotLightIndices" )
			, { &clusters.getOutputSpotLightIndicesBuffer(), &clusters.getInputSpotLightIndicesBuffer(), &clusters.getOutputSpotLightIndicesBuffer()
				, &clusters.getOutputSpotLightIndicesBuffer(), &clusters.getInputSpotLightIndicesBuffer(), &clusters.getOutputSpotLightIndicesBuffer() }
			, 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( spot, uint32_t( lgtbvh::eLightBVH ), cuT( "C3D_SpotLightBVH" ), clusters.getSpotLightBVHBuffer(), 0u, ashes::WholeSize );

		return { &point, &spot };
	}

	//*********************************************************************************************
}
