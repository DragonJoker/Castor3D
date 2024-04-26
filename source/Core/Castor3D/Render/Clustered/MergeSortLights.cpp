#include "Castor3D/Render/Clustered/MergeSortLights.hpp"

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
#include "Castor3D/Shader/Shaders/GlslRadixSort.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace merge
	{
		static uint32_t constexpr NumThreadsPerThreadGroup = 256u;
		static uint32_t constexpr NumValuesPerThread = 8u;
		static uint32_t constexpr NumValuesPerThreadGroup = NumThreadsPerThreadGroup * NumValuesPerThread;

		enum BindingPoints
		{
			eInputKeys,
			eInputValues,
			eOutputKeys,
			eOutputValues,
			eMergePathPartitions,
		};

		static ShaderPtr createShader( RenderDevice const & device
			, bool mergePathPartitions )
		{
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto inputKeysBuffer = writer.declStorageBuffer( "c3d_inputKeysBuffer"
				, uint32_t( eInputKeys )
				, 0u );
			auto c3d_inputKeys = inputKeysBuffer.declMemberArray< sdw::UInt >( "ik" );
			inputKeysBuffer.end();

			auto inputValuesBuffer = writer.declStorageBuffer( "c3d_inputValuesBuffer"
				, uint32_t( eInputValues )
				, 0u );
			auto c3d_inputValues = inputValuesBuffer.declMemberArray< sdw::UInt >( "iv" );
			inputValuesBuffer.end();

			auto outputKeysBuffer = writer.declStorageBuffer( "c3d_outputKeysBuffer"
				, uint32_t( eOutputKeys )
				, 0u );
			auto c3d_outputKeys = outputKeysBuffer.declMemberArray< sdw::UInt >( "ok" );
			outputKeysBuffer.end();

			auto outputValuesBuffer = writer.declStorageBuffer( "c3d_outputValuesBuffer"
				, uint32_t( eOutputValues )
				, 0u );
			auto c3d_outputValues = outputValuesBuffer.declMemberArray< sdw::UInt >( "ov" );
			outputValuesBuffer.end();

			auto mergePathPartitionsBuffer = writer.declStorageBuffer( "c3d_mergePathPartitionsBuffer"
				, uint32_t( eMergePathPartitions )
				, 0u );
			auto c3d_mergePathPartitions = mergePathPartitionsBuffer.declMemberArray< sdw::Int >( "mp" );
			mergePathPartitionsBuffer.end();

			sdw::PushConstantBuffer pcb{ writer, "C3D_DispatchData", "c3d_dispatchData" };
			auto c3d_numElements = pcb.declMember< sdw::UInt >( "c3d_numElements" );
			auto c3d_chunkSize = pcb.declMember< sdw::UInt >( "c3d_chunkSize" );
			pcb.end();

			auto gsKeys = writer.declSharedVariable< sdw::UInt >( "gsKeys", NumValuesPerThreadGroup );		// Intermediate keys.		(8,192 Bytes)
			auto gsValues = writer.declSharedVariable< sdw::UInt >( "gsValues", NumValuesPerThreadGroup );	// Intermediate values.		(8,192 Bytes)

			/**
			* MergePath is a binary search over two sorted arrays that finds the
			* point in list A and list B to begin a merge operation.
			* Based on: https://moderngpu.github.io/bulkinsert.html#mergepath
			* Retrieved on: Aug 9, 2016.
			*
			* @param a0 The first element in list A.
			* @param aCount The number of elements in A.
			* @param b0 The first element in list B.
			* @param bCount The number of elements in B.
			* @param diag The cross diagonal of the merge matrix where the merge path is computed.
			* @param bUseSharedMem Whether to read from shared memory or global memory.
			* @return
			*/
			auto mergePath = writer.implementFunction< sdw::Int >( "c3d_mergePath"
				, [&]( sdw::Int const & a0, sdw::Int const & aCount
					, sdw::Int const & b0, sdw::Int const & bCount
					, sdw::Int const & diag, sdw::Boolean const & bUseSharedMem )
				{
					auto begin = writer.declLocale( "begin", max( 0_i, diag - bCount ) );
					auto end = writer.declLocale( "end", min( diag, aCount ) );

					WHILE( writer, begin < end )
					{
						// Find the mid-point to start searching from.
						auto mid = writer.declLocale( "mid", ( begin + end ) >> 1_i );
						auto a = writer.declLocale( "a", writer.ternary( bUseSharedMem, gsKeys[a0 + mid], c3d_inputKeys[a0 + mid] ) );
						auto b = writer.declLocale( "b", writer.ternary( bUseSharedMem, gsKeys[b0 + diag - 1 - mid], c3d_inputKeys[b0 + diag - 1 - mid] ) );

						IF( writer, a < b )
						{
							begin = mid + 1_i;
						}
						ELSE
						{
							end = mid;
						}
						FI
					}
					ELIHW

					writer.returnStmt( begin );
				}
				, sdw::InInt{ writer, "a0" }
				, sdw::InInt{ writer, "aCount" }
				, sdw::InInt{ writer, "b0" }
				, sdw::InInt{ writer, "bCount" }
				, sdw::InInt{ writer, "diag" }
				, sdw::InBoolean{ writer, "bUseSharedMem" } );
			/**
			* Perform a serial merge using shared memory. Write results to global memory.
			*/
			auto serialMerge = writer.implementFunction< sdw::Void >( "c3d_serialMerge"
				, [&]( sdw::Int a0, sdw::Int const & a1
					, sdw::Int b0, sdw::Int const & b1
					, sdw::Int const & diag
					, sdw::Int const & numValues, sdw::Int const & out0 )
				{
					auto aKey = writer.declLocale( "aKey", gsKeys[a0] );
					auto bKey = writer.declLocale( "bKey", gsKeys[b0] );

					auto aValue = writer.declLocale( "aValue", gsValues[a0] );
					auto bValue = writer.declLocale( "bValue", gsValues[b0] );

					FOR( writer, sdw::Int, i, 0_i, i < int( NumValuesPerThread ) && diag + i < numValues, ++i )
					{
						IF( writer, b0 >= b1 || ( a0 < a1 && aKey < bKey ) )
						{
							c3d_outputKeys[out0 + diag + i] = aKey;
							c3d_outputValues[out0 + diag + i] = aValue;

							++a0;

							aKey = gsKeys[a0];
							aValue = gsValues[a0];
						}
						ELSE
						{
							c3d_outputKeys[out0 + diag + i] = bKey;
							c3d_outputValues[out0 + diag + i] = bValue;

							++b0;

							bKey = gsKeys[b0];
							bValue = gsValues[b0];
						}
						FI
					}
					ROF
				}
				, sdw::InInt{ writer, "a0" }
				, sdw::InInt{ writer, "a1" }
				, sdw::InInt{ writer, "b0" }
				, sdw::InInt{ writer, "b1" }
				, sdw::InInt{ writer, "diag" }
				, sdw::InInt{ writer, "numValues" }
				, sdw::InInt{ writer, "out0" } );

			writer.implementMainT< sdw::VoidT >( NumThreadsPerThreadGroup
				, [&]( sdw::ComputeIn const & in )
				{
					auto const & threadIndex = in.globalInvocationID.x();
					auto const & chunkSize = c3d_chunkSize;
					// Number of chunks to sort.
					auto numChunks = writer.declLocale( "numChunks", writer.cast< sdw::UInt >( ceil( writer.cast< sdw::Float >( c3d_numElements ) / writer.cast< sdw::Float >( chunkSize ) ) ) );
					// Num values to sort per sort group.
					auto numValuesPerSortGroup = writer.declLocale( "numValuesPerSortGroup", min( chunkSize * 2_u, c3d_numElements ) );

					if ( mergePathPartitions )
					{
						// Number of sort groups needed to sort all chunks.
						auto numSortGroups = writer.declLocale( "numSortGroups", numChunks / 2_u );

						// Total number of partitions per sort group.
						auto numPartitionsPerSortGroup = writer.declLocale( "numPartitionsPerSortGroup", writer.cast< sdw::UInt >( ceil( writer.cast< sdw::Float >( numValuesPerSortGroup ) / float( NumValuesPerThreadGroup ) ) ) + 1_u );
						// The sort group this thread is operating on.
						auto sortGroup = writer.declLocale( "sortGroup", threadIndex / numPartitionsPerSortGroup );
						// The partition this thread is computing within the sort group.
						auto partitionInSortGroup = writer.declLocale( "partitionInSortGroup", threadIndex % numPartitionsPerSortGroup );

						// The partition across all sort groups.
						auto globalPartition = writer.declLocale( "globalPartition", threadIndex );
						// Compute the maximum number of partitions to compute.
						auto maxPartitions = writer.declLocale( "maxPartitions", numSortGroups * numPartitionsPerSortGroup );

						IF( writer, globalPartition < maxPartitions )
						{
							auto a0 = writer.declLocale( "a0", writer.cast< sdw::Int >( sortGroup * numValuesPerSortGroup ) );
							auto a1 = writer.declLocale( "a1", min( a0 + writer.cast< sdw::Int >( chunkSize ), writer.cast< sdw::Int >( c3d_numElements ) ) );
							auto aCount = writer.declLocale( "aCount", a1 - a0 );
							auto b0 = writer.declLocale( "b0", a1 );
							auto b1 = writer.declLocale( "b1", min( b0 + writer.cast< sdw::Int >( chunkSize ), writer.cast< sdw::Int >( c3d_numElements ) ) );
							auto bCount = writer.declLocale( "bCount", b1 - b0 );
							// Number of values to sort in this sort group.
							auto numValues = writer.declLocale( "numValues", aCount + bCount );
							// The diagonal in the merge matrix of this sort group.
							auto diag = writer.declLocale( "diag", min( writer.cast< sdw::Int >( partitionInSortGroup * NumValuesPerThreadGroup ), numValues ) );

							// Find the merge path for this partition using global memory.
							auto mergedPath = writer.declLocale( "mergedPath", mergePath( a0, aCount, b0, bCount, diag, 0_b ) );

							// Write the merge path to global memory.
							c3d_mergePathPartitions[globalPartition] = mergedPath;
						}
						FI
					}
					else
					{
						auto const & groupID = in.workGroupID.x();
						auto const & groupIndex = in.localInvocationIndex;

						// Compute the number of thread groups required to sort a single sort group.
						auto numThreadGroupsPerSortGroup = writer.declLocale( "numThreadGroupsPerSortGroup", writer.cast< sdw::UInt >( ceil( writer.cast< sdw::Float >( numValuesPerSortGroup ) / float( NumValuesPerThreadGroup ) ) ) );
						// The number of partitions per sort group.
						// We add 1 to account for the merge path partition at the end of the sort group.
						auto numPartitionsPerSortGroup = writer.declLocale( "numPartitionsPerSortGroup", numThreadGroupsPerSortGroup + 1_u );

						// Compute the sort group that this thread is operating on.
						auto sortGroup = writer.declLocale( "sortGroup", groupID / numThreadGroupsPerSortGroup );
						// The merge path partition within the sort group.
						auto partition = writer.declLocale( "partition", groupID % numThreadGroupsPerSortGroup );
						// num values for the current sort group, clamped to avoid out of bounds access
						auto numValuesThisSortGroup = writer.declLocale( "numValuesThisSortGroup", min( numValuesPerSortGroup, c3d_numElements - sortGroup * numValuesPerSortGroup ) );

						auto globalPartition = writer.declLocale( "globalPartition", ( sortGroup * numPartitionsPerSortGroup ) + partition );

						// Load the keys into shared memory based on the mergepath for this thread group.
						auto mergePath0 = writer.declLocale( "mergePath0", c3d_mergePathPartitions[globalPartition] );
						auto mergePath1 = writer.declLocale( "mergePath1", c3d_mergePathPartitions[globalPartition + 1_u] );
						auto diag0 = writer.declLocale( "diag0", writer.cast< sdw::Int >( min( partition * NumValuesPerThreadGroup, numValuesThisSortGroup ) ) );
						auto diag1 = writer.declLocale( "diag1", writer.cast< sdw::Int >( min( ( partition + 1_u ) * NumValuesPerThreadGroup, numValuesThisSortGroup ) ) );

						// Compute the chunk ranges in the input set.
						auto chunkOffsetA0 = writer.declLocale( "chunkOffsetA0", writer.cast< sdw::Int >( min( sortGroup * numValuesPerSortGroup, c3d_numElements ) ) );
						auto chunkOffsetA1 = writer.declLocale( "chunkOffsetA1", min( chunkOffsetA0 + writer.cast< sdw::Int >( chunkSize ), writer.cast< sdw::Int >( c3d_numElements ) ) );
						auto chunkSizeA = writer.declLocale( "chunkSizeA", chunkOffsetA1 - chunkOffsetA0 );

						auto chunkOffsetB0 = writer.declLocale( "chunkOffsetB0", chunkOffsetA1 );
						auto chunkOffsetB1 = writer.declLocale( "chunkOffsetB1", min( chunkOffsetB0 + writer.cast< sdw::Int >( chunkSize ), writer.cast< sdw::Int >( c3d_numElements ) ) );
						auto chunkSizeB = writer.declLocale( "chunkSizeB", chunkOffsetB1 - chunkOffsetB0 );

						// The total number of values to be sorted.
						auto numValues = writer.declLocale( "numValues", chunkSizeA + chunkSizeB );

						auto a0 = writer.declLocale( "a0", mergePath0 );
						auto a1 = writer.declLocale( "a1", mergePath1 );
						auto numA = writer.declLocale( "numA", min( a1 - a0, chunkSizeA ) );

						auto b0 = writer.declLocale( "b0", diag0 - mergePath0 );
						auto b1 = writer.declLocale( "b1", diag1 - mergePath1 );
						auto numB = writer.declLocale( "numB", min( b1 - b0, chunkSizeB ) );

						// Compute the diagonal for this thread within the threadgroup.
						auto diag = writer.declLocale( "diag", writer.cast< sdw::Int >( groupIndex * NumValuesPerThread ) );

						auto a = writer.declLocale( "a", 0_i );
						auto b = writer.declLocale( "b", 0_i );
						auto key = writer.declLocale( "key", 0_u );
						auto value = writer.declLocale( "value", 0_u );

						// Load the keys and values into shared memory.
						for ( s32 i = 0; i < s32( NumValuesPerThread ); ++i )
						{
							a = a0 + diag + i;
							b = b0 + ( a - a1 );

							IF ( writer, a < a1 )
							{
								key = c3d_inputKeys[chunkOffsetA0 + a];
								value = c3d_inputValues[chunkOffsetA0 + a];
							}
							ELSE
							{
								key = c3d_inputKeys[chunkOffsetB0 + b];
								value = c3d_inputValues[chunkOffsetB0 + b];
							}
							FI

							gsKeys[diag + i] = key;
							gsValues[diag + i] = value;
						}

						// Sync loading of keys/values in shared memory.
						shader::groupMemoryBarrierWithGroupSync( writer );

						// Compute the mergepath for this thread using shared memory.
						auto mergedPath = writer.declLocale( "mergedPath", mergePath( 0_i, numA, numA, numB, diag, 1_b ) );

						// Perform the serial merge using shared memory.
						serialMerge( mergedPath, numA, numA + diag - mergedPath, numA + numB, diag0 + diag, numValues, chunkOffsetA0 );
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
				, FrustumClusters & clusters
				, LightType lightType )
				: crg::RunnablePass{ framePass
					, context
					, graph
					, { [this]( uint32_t index ){ doInitialise( index ); }
						, GetPipelineStateCallback( [](){ return crg::getPipelineState( VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT ); } )
						, [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); }
						, crg::getDefaultV< GetPassIndexCallback >()
						, IsEnabledCallback( [this](){ return doIsEnabled(); } )
						, IsComputePassCallback( [](){ return true; } ) }
					, crg::ru::Config{ 1u, true /* resettable */ } }
				, m_clusters{ clusters }
				, m_lightCache{ clusters.getCamera().getScene()->getLightCache() }
				, m_lightType{ lightType }
				, m_partitions{ framePass, context, graph, device, true, this, m_lightType }
				, m_merge{ framePass, context, graph, device, false, this, m_lightType }
			{
			}

			void resetPipeline( uint32_t index )
			{
				resetCommandBuffer( index );
				m_partitions.pipeline.resetPipeline( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_partitions.createInfo ), index );
				m_merge.pipeline.resetPipeline( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_merge.createInfo ), index );
				doCreatePipeline( index, m_partitions );
				doCreatePipeline( index, m_merge );
				reRecordCurrent();
			}

		private:
			struct Pipeline
			{
				ShaderModule shader;
				ashes::PipelineShaderStageCreateInfoArray createInfo;
				crg::cp::ConfigData cpConfig;
				crg::PipelineHolder pipeline;

				Pipeline( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph
					, RenderDevice const & device
					, bool mergePathPartitions
					, FramePass const * parent
					, LightType lightType )
					: shader{ VK_SHADER_STAGE_COMPUTE_BIT
						, ( mergePathPartitions ? castor::String{ cuT( "MergePathPartitions/" ) } : castor::String{ cuT( "MergeSort/" ) } ) + getName( lightType )
						, createShader( device, mergePathPartitions ) }
					, createInfo{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, shader ) } }
					, cpConfig{ crg::getDefaultV< InitialiseCallback >()
						, nullptr
						, IsEnabledCallback( [parent](){ return parent->doIsEnabled();} )
						, crg::getDefaultV< GetPassIndexCallback >()
						, crg::getDefaultV< RecordCallback >()
						, crg::getDefaultV< RecordCallback >()
						, 1u
						, 1u
						, 1u }
					, pipeline{ framePass
						, context
						, graph
						, crg::pp::Config{}
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( createInfo ) )
							.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u } )
						, VK_PIPELINE_BIND_POINT_COMPUTE
						, 2u }
				{
				}
			};

		private:
			struct DispatchData
			{
				u32 numElements;
				u32 chunkSize;
			};

			FrustumClusters & m_clusters;
			LightCache const & m_lightCache;
			LightType m_lightType;
			Pipeline m_partitions;
			Pipeline m_merge;

		private:
			void doInitialise( uint32_t index )
			{
				m_partitions.pipeline.initialise();
				m_merge.pipeline.initialise();
				doCreatePipeline( index, m_partitions );
				doCreatePipeline( index, m_merge );
			}

			bool doIsEnabled()const
			{
				return m_clusters.getConfig().sortLights
					&& m_clusters.needsLightsUpdate()
					&& m_lightCache.getLightsBufferCount( m_lightType ) > 0;
			}

			void doRecordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				// The number of threads per thread group.
				constexpr u32 threadsPerThreadGroupCount = NumThreadsPerThreadGroup;
				// The number of values that each thread sorts.
				constexpr u32 valuesPerThreadCount = NumValuesPerThread;
				// The number of values that each thread group will sort.
				constexpr u32 valuesPerThreadGroupCount = threadsPerThreadGroupCount * valuesPerThreadCount;

				auto totalValues = m_lightCache.getLightsBufferCount( m_lightType );
				DispatchData data{ totalValues, 0u };

				// The size of a single chunk that keys will be sorted into.
				auto chunkSize = FrustumClusters::getBucketSortBucketSize();

				// The total number of complete chunks to sort.
				auto chunksCount = getLightsMortonCodeChunkCount( totalValues );

				while ( chunksCount > 1u )
				{
					data.chunkSize = chunkSize;

					// Number of sort groups required to sort all chunks.
					// Each sort group merge sorts 2 chunks into a single chunk.
					auto sortGroupsCount = chunksCount / 2u;

					// The number of thread groups that are required per sort group.
					auto threadGroupsPerSortGroupCount = castor::divRoundUp( chunkSize * 2u, valuesPerThreadGroupCount );

					// Compute merge path partitions per thread group.
					{
						m_partitions.pipeline.recordInto( context, commandBuffer, index );

						// The number of merge path partitions that need to be computed.
						auto mergePathPartitionsPerSortGroupCount = threadGroupsPerSortGroupCount + 1u;
						auto totalMergePathPartitions = mergePathPartitionsPerSortGroupCount * sortGroupsCount;

						// The number of thread groups needed to compute all merge path partitions.
						auto threadGroupsCount = castor::divRoundUp( totalMergePathPartitions, threadsPerThreadGroupCount );

						m_context.vkCmdPushConstants( commandBuffer, m_partitions.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &data );
						m_context.vkCmdDispatch( commandBuffer, threadGroupsCount, 1u, 1u );

						// Add an explicit barrier for MergePathPartitions.
						// This is required since the MergePathPartitions structured buffer is being used
						// as a UAV in the MergePathPartions compute shader and as an SRV in the MergeSort
						// compute shader. Because the MergePathPartions argument is not rebound between
						// dispatches, no implicit UAV barrier will be added to the command list and MergeSort
						// will likely not see the correct merge path partitions.
						// To resolve this, an explicit UAV barrier is added for the resource.
						doBarriers( context, commandBuffer, index );
					}

					// Perform merge sort using merge path partitions computed from the previous step.
					{
						m_merge.pipeline.recordInto( context, commandBuffer, index );

						// Don't dispatch thread groups that will perform no work:
						// we need at least one thread group for each sort group; no more than the number of sort groups times
						// the number of thread groups per sort groups (if there is an odd number of chunks, the last chunk
						// needs no merge); enough thread groups to sort all values.


						// The number of thread groups required to sort all values.
						const u32 numThreadGroups = std::max( sortGroupsCount
							, std::min( threadGroupsPerSortGroupCount * sortGroupsCount
								, castor::divRoundUp( totalValues, valuesPerThreadGroupCount ) ) );
						m_context.vkCmdPushConstants( commandBuffer, m_merge.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &data );
						m_context.vkCmdDispatch( commandBuffer, numThreadGroups, 1u, 1u );

						if ( chunksCount & 1 )
						{
							// copy last chunk, there is no merging required but we still need data in the destination buffer
							// note: no additional barriers as we are still doing read from source-> write to dest, so that should be good
							u32 lastChunkOffset = chunkSize * ( chunksCount - 1 ) * sizeof( u32 );
							auto lastChunkSize = u32( totalValues * sizeof( u32 ) - lastChunkOffset );
							auto srcMorton = m_pass.buffers[0].buffer( index );
							auto srcIndices = m_pass.buffers[1].buffer( index );
							auto dstMorton = m_pass.buffers[0].buffer( 1u - index );
							auto dstIndices = m_pass.buffers[1].buffer( 1u - index );
							VkBufferCopy region{ lastChunkOffset, lastChunkOffset, lastChunkSize };
							m_context.vkCmdCopyBuffer( commandBuffer, srcMorton, dstMorton, 1u, &region );
							m_context.vkCmdCopyBuffer( commandBuffer, srcIndices, dstIndices, 1u, &region );
						}
					}

					// Ping-pong the buffers
					index = 1u - index;

					chunkSize *= 2;
					chunksCount = castor::divRoundUp( totalValues, chunkSize );
				}
			}

			void doBarriers( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t passIndex )const
			{
				for ( auto & attach : m_pass.buffers )
				{
					if ( !attach.isNoTransition()
						&& attach.isStorageBuffer()
						&& attach.isClearableBuffer() )
					{
						auto currentState = context.getAccessState( attach.buffer( passIndex )
							, attach.getBufferRange() );
						context.memoryBarrier( commandBuffer
							, attach.buffer( passIndex )
							, attach.getBufferRange()
							, currentState.access
							, currentState.pipelineStage
							, crg::AccessState{ VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
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

	u32 getLightsMortonCodeChunkCount( u32 lightCount )
	{
		auto chunkSize = merge::NumThreadsPerThreadGroup;

		// The total number of complete chunks to sort.
		auto numChunks = castor::divRoundUp( lightCount, chunkSize );

		return numChunks;
	}

	crg::FramePassArray createMergeSortLightsPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, RenderDevice const & device
		, FrustumClusters & clusters )
	{
		// Point lights
		auto & point = graph.createPass( "MergeSort/Point"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< merge::FramePass >( framePass
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
		createInOutStoragePassBinding( point, uint32_t( merge::eInputKeys ), cuT( "C3D_InMortonCodes" ), clusters.getInputPointLightMortonCodesBuffers(), 0u, ashes::WholeSize );
		createInOutStoragePassBinding( point, uint32_t( merge::eInputValues ), cuT( "C3D_InLightIndices" ), clusters.getInputPointLightIndicesBuffers(), 0u, ashes::WholeSize );
		createInOutStoragePassBinding( point, uint32_t( merge::eOutputKeys ), cuT( "C3D_OutMortonCodes" ), clusters.getOutputPointLightMortonCodesBuffers(), 0u, ashes::WholeSize );
		createInOutStoragePassBinding( point, uint32_t( merge::eOutputValues ), cuT( "C3D_OutLightIndices" ), clusters.getOutputPointLightIndicesBuffers(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( point, uint32_t( merge::eMergePathPartitions ), cuT( "C3D_MergePathPartitions" ), clusters.getMergePathPartitionsBuffer(), 0u, ashes::WholeSize );

		// Spot lights
		auto & spot = graph.createPass( "MergeSort/Spot"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< merge::FramePass >( framePass
					, context
					, graph
					, device
					, clusters
					, LightType::eSpot );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		spot.addDependency( point );
		spot.addDependency( *previousPasses.back() );
		createInOutStoragePassBinding( spot, uint32_t( merge::eInputKeys ), cuT( "C3D_InMortonCodes" ), clusters.getInputSpotLightMortonCodesBuffers(), 0u, ashes::WholeSize );
		createInOutStoragePassBinding( spot, uint32_t( merge::eInputValues ), cuT( "C3D_InLightIndices" ), clusters.getInputSpotLightIndicesBuffers(), 0u, ashes::WholeSize );
		createInOutStoragePassBinding( spot, uint32_t( merge::eOutputKeys ), cuT( "C3D_OutMortonCodes" ), clusters.getOutputSpotLightMortonCodesBuffers(), 0u, ashes::WholeSize );
		createInOutStoragePassBinding( spot, uint32_t( merge::eOutputValues ), cuT( "C3D_OutLightIndices" ), clusters.getOutputSpotLightIndicesBuffers(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( spot, uint32_t( merge::eMergePathPartitions ), cuT( "C3D_MergePathPartitions" ), clusters.getMergePathPartitionsBuffer(), 0u, ashes::WholeSize );

		return { &point, &spot };
	}

	//*********************************************************************************************
}
