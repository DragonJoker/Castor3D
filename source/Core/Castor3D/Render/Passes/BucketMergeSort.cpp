#include "Castor3D/Render/Passes/BucketMergeSort.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslRadixSort.hpp"

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace bucket
	{
		enum class Bindings
		{
			eInOutKeys,
			eInOutValues
		};

		static uint32_t getBucketSize()
		{
			return shader::RadixSortT< 4u >::bucketSize;
		}

		static ShaderPtr createShader( RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			auto keysBuffer = writer.declStorageBuffer( "c3d_keysBuffer"
				, Bindings::eInOutKeys
				, 0u );
			auto c3d_keys = keysBuffer.declMemberArray< sdw::UInt >( "iok" );
			keysBuffer.end();

			auto valuesBuffer = writer.declStorageBuffer( "c3d_valuesBuffer"
				, Bindings::eInOutValues
				, 0u );
			auto c3d_values = valuesBuffer.declMemberArray< sdw::UInt >( "iov" );
			valuesBuffer.end();

			sdw::PushConstantBuffer pcb{ writer, "C3D_DispatchData", "c3d_dispatchData" };
			auto c3d_numElements = pcb.declMember< sdw::UInt >( "c3d_numElements" );
			pcb.end();

			shader::RadixSortT< 4u > bucket{ writer, 30u };

			writer.implementMainT< sdw::VoidT >( bucket.threadsCount
				, [&writer, &bucket, &c3d_numElements, &c3d_keys, &c3d_values]( sdw::ComputeIn const & in )
				{
					//// In our case, the input keys are 30-bit morton codes.
					bucket.sortT( writer
						, 0_u, c3d_numElements
						, in.localInvocationIndex, in.globalInvocationID.x()
						, c3d_keys, 0_u, c3d_keys, sdw::UInt{ MaxLightsCount }
						, c3d_values, 0_u, c3d_values, sdw::UInt{ MaxLightsCount }, 0xFFFFFFFF_u );
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
				, GetNumKeysCallback const & getNumKeys
				, IsEnabledCallback const & isEnabled
				, String const & name )
				: crg::RunnablePass{ framePass
					, context
					, graph
					, crg::RunnablePass::Callbacks{}
						.onInitialise( [this]( uint32_t index ){ return doInitialise( index ); } )
						.onGetPipelineState( [](){ return crg::getPipelineState( PipelineStageFlags::eComputeShader ); } )
						.onRecord( [this]( crg::RecordContext const & ctx, VkCommandBuffer cb, uint32_t i ){ doRecordInto( ctx, cb, i ); } )
						.onIsEnabled( [this](){ return doIsEnabled(); } )
						.onIsComputePass( [](){ return true; } )
					, crg::ru::Config{ 1u } }
				, m_getNumKeys{ getNumKeys }
				, m_isEnabled{ isEnabled }
				, m_pipeline{ framePass, context, graph, device, this, name }
			{
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
					, FramePass const * parent
					, String const & name )
					: shader{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "BucketSort/" ) + name, createShader( device ) }
					, createInfo{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, shader ) } }
					, cpConfig{ crg::defaultV< crg::InitialiseCallback >
						, nullptr
						, crg::IsEnabledCallback( [parent]() { return parent->doIsEnabled(); } )
						, crg::GetPassIndexCallback( []() { return 0u; } )
						, crg::defaultV< crg::RecordCallback >
						, crg::defaultV< crg::RecordCallback >
						, 1u
						, 1u
						, 1u }
					, pipeline{ framePass
						, context
						, graph
						, crg::pp::Config{}
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( createInfo ) )
							.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, 4u } )
						, VK_PIPELINE_BIND_POINT_COMPUTE
						, 1u }
				{
				}
			};

		private:
			struct DispatchData
			{
				u32 numElements;
			};

			GetNumKeysCallback const & m_getNumKeys;
			IsEnabledCallback const & m_isEnabled;
			Pipeline m_pipeline;

		private:
			void doInitialise( uint32_t index )
			{
				m_pipeline.pipeline.initialise();
				doCreatePipeline( index, m_pipeline );
			}

			bool doIsEnabled()const
			{
				return m_isEnabled() && m_getNumKeys() > 0;
			}

			void doRecordInto( crg::RecordContext const & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				// Build bottom level of the BVH.
				auto lightsCount = m_getNumKeys();
				auto numThreadGroups = divRoundUp( lightsCount, getBucketSize() );
				DispatchData data{ lightsCount };
				m_pipeline.pipeline.recordInto( context, commandBuffer, index );
				context->vkCmdPushConstants( commandBuffer, m_pipeline.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 4u, &data );
				context->vkCmdDispatch( commandBuffer, numThreadGroups, 1u, 1u );
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

	namespace merge
	{
		static uint32_t constexpr NumThreadsPerThreadGroup = 256u;
		static uint32_t constexpr NumValuesPerThread = 8u;
		static uint32_t constexpr NumValuesPerThreadGroup = NumThreadsPerThreadGroup * NumValuesPerThread;

		enum class Bindings
		{
			eInOutKeys,
			eInOutValues,
			eMergePathPartitions,
		};

		static uint32_t getMaxMergePathPartitionsCount()
		{
			static uint32_t constexpr ElementsPerThread = 8u;

			// The maximum number of elements that need to be sorted.
			uint32_t maxElements = MaxLightsCount;

			// Radix sort will sort Morton codes (keys) into chunks of BucketSortBucketSize size.
			uint32_t chunkSize = bucket::getBucketSize();
			// The number of chunks that need to be merge sorted after Radix sort finishes.
			uint32_t numChunks = divRoundUp( maxElements, chunkSize );
			// The number of sort groups that are needed to sort the first set of chunks.
			// Each sort group will sort 2 chunks. So the maximum number of sort groups is 1/2 of the 
			// number of chunks.
			uint32_t maxSortGroups = numChunks / 2u;
			// The number of merge path partitions per sort group is the total values
			// to be sorted per sort group (2 chunks) divided by the number of elements 
			// that can be sorted per thread group. One is added to account for the 
			// merge path partition at the END of the chunk.
			uint32_t numMergePathPartitionsPerSortGroup = divRoundUp( chunkSize * 2u, ElementsPerThread * NumThreadsPerThreadGroup ) + 1u;

			// The maximum number of merge path partitions is the number of merge path partitions
			// needed by a single sort group multiplied by the maximum number of sort groups.
			return numMergePathPartitionsPerSortGroup * maxSortGroups;
		}

		static ShaderPtr createShader( RenderDevice const & device
			, bool mergePathPartitions )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			auto keysBuffer = writer.declStorageBuffer( "c3d_keysBuffer"
				, Bindings::eInOutKeys
				, 0u );
			auto c3d_keys = keysBuffer.declMemberArray< sdw::UInt >( "iok" );
			keysBuffer.end();

			auto valuesBuffer = writer.declStorageBuffer( "c3d_valuesBuffer"
				, Bindings::eInOutValues
				, 0u );
			auto c3d_values = valuesBuffer.declMemberArray< sdw::UInt >( "iov" );
			valuesBuffer.end();

			auto mergePathPartitionsBuffer = writer.declStorageBuffer( "c3d_mergePathPartitionsBuffer"
				, Bindings::eMergePathPartitions
				, 0u );
			auto c3d_mergePathPartitions = mergePathPartitionsBuffer.declMemberArray< sdw::Int >( "mp" );
			mergePathPartitionsBuffer.end();

			sdw::PushConstantBuffer pcb{ writer, "C3D_DispatchData", "c3d_dispatchData" };
			auto c3d_numElements = pcb.declMember< sdw::UInt >( "c3d_numElements" );
			auto c3d_chunkSize = pcb.declMember< sdw::UInt >( "c3d_chunkSize" );
			auto c3d_inputOffset = pcb.declMember< sdw::Int >( "c3d_inputOffset" );
			auto c3d_outputOffset = pcb.declMember< sdw::Int >( "c3d_outputOffset" );
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
			* @param useSharedMem Whether to read from shared memory or global memory.
			* @return
			*/
			auto mergePath = writer.implementFunction< sdw::Int >( "c3d_mergePath"
				, [&writer, &gsKeys, &c3d_keys, &c3d_inputOffset]( sdw::Int const & a0, sdw::Int const & aCount
					, sdw::Int const & b0, sdw::Int const & bCount
					, sdw::Int const & diag, sdw::Boolean const & useSharedMem )
				{
					auto begin = writer.declLocale( "begin", max( 0_i, diag - bCount ) );
					auto end = writer.declLocale( "end", min( diag, aCount ) );

					sdwWHILE( writer, begin < end )
					{
						// Find the mid-point to start searching from.
						auto mid = writer.declLocale( "mid", ( begin + end ) >> 1_i );
						auto a = writer.declLocale( "a", writer.ternary( useSharedMem, gsKeys[a0 + mid], c3d_keys[c3d_inputOffset + a0 + mid] ) );
						auto b = writer.declLocale( "b", writer.ternary( useSharedMem, gsKeys[b0 + diag - 1 - mid], c3d_keys[c3d_inputOffset + b0 + diag - 1 - mid] ) );

						sdwIF( writer, a < b )
						{
							begin = mid + 1_i;
						}
						sdwELSE
						{
							end = mid;
						}
						sdwFI
					}
					sdwELIHW

					writer.returnStmt( begin );
				}
				, sdw::InInt{ writer, "a0" }
				, sdw::InInt{ writer, "aCount" }
				, sdw::InInt{ writer, "b0" }
				, sdw::InInt{ writer, "bCount" }
				, sdw::InInt{ writer, "diag" }
				, sdw::InBoolean{ writer, "useSharedMem" } );
			/**
			* Perform a serial merge using shared memory. Write results to global memory.
			*/
			auto serialMerge = writer.implementFunction< sdw::Void >( "c3d_serialMerge"
				, [&writer, &gsKeys, &gsValues, &c3d_keys, &c3d_values, &c3d_outputOffset]( sdw::Int a0, sdw::Int const & a1
					, sdw::Int b0, sdw::Int const & b1
					, sdw::Int const & diag
					, sdw::Int const & numValues, sdw::Int const & out0 )
				{
					auto aKey = writer.declLocale( "aKey", gsKeys[a0] );
					auto bKey = writer.declLocale( "bKey", gsKeys[b0] );

					auto aValue = writer.declLocale( "aValue", gsValues[a0] );
					auto bValue = writer.declLocale( "bValue", gsValues[b0] );

					sdwFOR( writer, sdw::Int, i, 0_i, i < int( NumValuesPerThread ) && diag + i < numValues, ++i )
					{
						sdwIF( writer, b0 >= b1 || ( a0 < a1 && aKey < bKey ) )
						{
							c3d_keys[c3d_outputOffset + out0 + diag + i] = aKey;
							c3d_values[c3d_outputOffset + out0 + diag + i] = aValue;

							++a0;

							aKey = gsKeys[a0];
							aValue = gsValues[a0];
						}
						sdwELSE
						{
							c3d_keys[c3d_outputOffset + out0 + diag + i] = bKey;
							c3d_values[c3d_outputOffset + out0 + diag + i] = bValue;

							++b0;

							bKey = gsKeys[b0];
							bValue = gsValues[b0];
						}
						sdwFI
					}
					sdwROF
				}
				, sdw::InInt{ writer, "a0" }
				, sdw::InInt{ writer, "a1" }
				, sdw::InInt{ writer, "b0" }
				, sdw::InInt{ writer, "b1" }
				, sdw::InInt{ writer, "diag" }
				, sdw::InInt{ writer, "numValues" }
				, sdw::InInt{ writer, "out0" } );

			writer.implementMainT< sdw::VoidT >( NumThreadsPerThreadGroup
				, [&writer, &mergePath, &serialMerge
					, &c3d_keys, &c3d_values, &c3d_inputOffset, &gsKeys, &gsValues
					, &c3d_chunkSize, &c3d_numElements, &c3d_mergePathPartitions
					, mergePathPartitions]( sdw::ComputeIn const & in )
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

						sdwIF( writer, globalPartition < maxPartitions )
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
						sdwFI
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

							sdwIF( writer, a < a1 )
							{
								key = c3d_keys[c3d_inputOffset + chunkOffsetA0 + a];
								value = c3d_values[c3d_inputOffset + chunkOffsetA0 + a];
							}
							sdwELSE
							{
								key = c3d_keys[c3d_inputOffset + chunkOffsetB0 + b];
								value = c3d_values[c3d_inputOffset + chunkOffsetB0 + b];
							}
							sdwFI

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
				, GetNumKeysCallback const & getNumKeys
				, IsEnabledCallback const & isEnabled
				, String const & name )
				: crg::RunnablePass{ framePass
					, context
					, graph
					, crg::RunnablePass::Callbacks{}
						.onInitialise( [this]( uint32_t index ){ doInitialise( index ); } )
						.onGetPipelineState( [](){ return crg::getPipelineState( PipelineStageFlags::eComputeShader ); } )
						.onRecord( [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t ){ doRecordInto( recContext, cb ); } )
						.onIsEnabled( [this](){ return doIsEnabled(); } )
						.onIsComputePass( [](){ return true; } )
					, crg::ru::Config{ 1u } }
				, m_partitions{ framePass, context, graph, device, true, this, name }
				, m_merge{ framePass, context, graph, device, false, this, name }
				, m_getNumKeys{ getNumKeys }
				, m_isEnabled{ isEnabled }
			{
			}

		private:
			struct DispatchData
			{
				u32 numElements;
				u32 chunkSize;
				s32 inputOffset;
				s32 outputOffset;
			};

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
					, String const & name )
					: shader{ VK_SHADER_STAGE_COMPUTE_BIT
						, ( mergePathPartitions ? String{ cuT( "MergePathPartitions/" ) } : String{ cuT( "MergeSort" ) } ) + name
						, createShader( device, mergePathPartitions ) }
					, createInfo{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, shader ) } }
					, cpConfig{ crg::defaultV< crg::InitialiseCallback >
						, nullptr
						, crg::IsEnabledCallback( [parent](){ return parent->doIsEnabled(); } )
						, crg::defaultV< crg::GetPassIndexCallback >
						, crg::defaultV< crg::RecordCallback >
						, crg::defaultV< crg::RecordCallback >
						, 1u
						, 1u
						, 1u }
					, pipeline{ framePass
						, context
						, graph
						, crg::pp::Config{}
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( createInfo ) )
							.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, sizeof( DispatchData ) } )
						, VK_PIPELINE_BIND_POINT_COMPUTE
						, 2u }
				{
				}
			};

		private:
			Pipeline m_partitions;
			Pipeline m_merge;
			GetNumKeysCallback m_getNumKeys;
			IsEnabledCallback m_isEnabled;

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
				return m_isEnabled() && m_getNumKeys() > 0;
			}

			void doRecordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer )
			{
				auto & mortonAttach = *getPass().getInouts().find( uint32_t( merge::Bindings::eInOutKeys ) )->second;
				auto & indicesAttach = *getPass().getInouts().find( uint32_t( merge::Bindings::eInOutValues ) )->second;
				u32 srcOffset = MaxLightsCount;
				u32 dstOffset = 0u;

				// The number of threads per thread group.
				constexpr u32 threadsPerThreadGroupCount = NumThreadsPerThreadGroup;
				// The number of values that each thread sorts.
				constexpr u32 valuesPerThreadCount = NumValuesPerThread;
				// The number of values that each thread group will sort.
				constexpr u32 valuesPerThreadGroupCount = threadsPerThreadGroupCount * valuesPerThreadCount;

				auto totalValues = m_getNumKeys();
				DispatchData data{ totalValues, 0u };

				// The size of a single chunk that keys will be sorted into.
				auto chunkSize = bucket::getBucketSize();

				// The total number of complete chunks to sort.
				auto chunksCount = divRoundUp( totalValues, merge::NumThreadsPerThreadGroup );

				u32 index{};
				while ( chunksCount > 1u )
				{
					context->vkCmdBeginDebugBlock( commandBuffer
						, crg::DebugBlockInfo{ .markerName = "Subpass " + std::to_string( index ), .colour = getTimer().getColour() } );
					data.chunkSize = chunkSize;
					data.inputOffset = s32( srcOffset );
					data.outputOffset = s32( dstOffset );

					// Number of sort groups required to sort all chunks.
					// Each sort group merge sorts 2 chunks into a single chunk.
					auto sortGroupsCount = chunksCount / 2u;

					// The number of thread groups that are required per sort group.
					auto threadGroupsPerSortGroupCount = divRoundUp( chunkSize * 2u, valuesPerThreadGroupCount );

					// Compute merge path partitions per thread group.
					{
						// The number of merge path partitions that need to be computed.
						auto mergePathPartitionsPerSortGroupCount = threadGroupsPerSortGroupCount + 1u;
						auto totalMergePathPartitions = mergePathPartitionsPerSortGroupCount * sortGroupsCount;

						// The number of thread groups needed to compute all merge path partitions.
						auto threadGroupsCount = divRoundUp( totalMergePathPartitions, threadsPerThreadGroupCount );

						doMergeTransitionBarrier( context, commandBuffer );
						m_partitions.pipeline.recordInto( context, commandBuffer, 0u );
						context->vkCmdPushConstants( commandBuffer, m_partitions.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, sizeof( DispatchData ), &data );
						context->vkCmdDispatch( commandBuffer, threadGroupsCount, 1u, 1u );
					}

					// Perform merge sort using merge path partitions computed from the previous step.
					{
						// Don't dispatch thread groups that will perform no work:
						// we need at least one thread group for each sort group; no more than the number of sort groups times
						// the number of thread groups per sort groups (if there is an odd number of chunks, the last chunk
						// needs no merge); enough thread groups to sort all values.
						const u32 threadGroupsCount = std::max( sortGroupsCount
							, std::min( threadGroupsPerSortGroupCount * sortGroupsCount
								, divRoundUp( totalValues, valuesPerThreadGroupCount ) ) );

						doAllBarriers( context, commandBuffer );
						m_merge.pipeline.recordInto( context, commandBuffer, 0u );
						context->vkCmdPushConstants( commandBuffer, m_merge.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, sizeof( DispatchData ), &data );
						context->vkCmdDispatch( commandBuffer, threadGroupsCount, 1u, 1u );
					}

					if ( chunksCount & 1 )
					{
						// copy last chunk, there is no merging required but we still need data in the destination buffer
						// note: no additional barriers as we are still doing read from source-> write to dest, so that should be good
						u32 lastChunkOffset = chunkSize * ( chunksCount - 1 ) * sizeof( u32 );
						auto lastChunkSize = u32( totalValues * sizeof( u32 ) - lastChunkOffset );
						auto srcCopyOffset = lastChunkOffset + srcOffset * sizeof( u32 );
						auto dstCopyOffset = lastChunkOffset + dstOffset * sizeof( u32 );
						auto morton = mortonAttach.buffer();
						auto indices = indicesAttach.buffer();
						context.copyBuffer( commandBuffer, index
							, morton, morton
							, srcCopyOffset, dstCopyOffset, lastChunkSize );
						context.copyBuffer( commandBuffer, index
							, indices, indices
							, srcCopyOffset, dstCopyOffset, lastChunkSize );
					}

					// Ping-pong the buffers
					std::swap( srcOffset, dstOffset );
					++index;

					context->vkCmdEndDebugBlock( commandBuffer );

					chunkSize *= 2;
					chunksCount = divRoundUp( totalValues, chunkSize );
				}

				if ( dstOffset != 0u )
				{
					auto indices = indicesAttach.buffer();

					// Copy temp indices buffer into final indices buffer, if needed.
					auto currentState = context.getAccessState( indices.data->buffer, indicesAttach.getBufferRange() );
					context.memoryBarrier( commandBuffer, indices.data->buffer, indicesAttach.getBufferRange(), currentState, TransferReadWriteState, true );

					auto buffer = &getGraph().createBuffer( indices.data->buffer );
					VkBufferCopy copyRegion{ dstOffset, srcOffset, totalValues * sizeof( u32 ) };
					context->vkCmdCopyBuffer( commandBuffer, buffer->getBuffer(), buffer->getBuffer(), 1u, &copyRegion );

					context.memoryBarrier( commandBuffer, indices.data->buffer, indicesAttach.getBufferRange(), currentState, ComputeShaderReadWriteState, true );
				}
			}

			void doMergeTransitionBarrier( crg::RecordContext & context
				, VkCommandBuffer commandBuffer )const
			{
				auto & attach = *getPass().getOutputs().rbegin()->second;
				auto buffer = attach.buffer();
				auto currentState = context.getAccessState( buffer.data->buffer, attach.getBufferRange() );
				context.memoryBarrier( commandBuffer
					, buffer.data->buffer, attach.getBufferRange()
					, currentState, ComputeShaderReadWriteState
					, true );
			}

			void doAllBarriers( crg::RecordContext & context
				, VkCommandBuffer commandBuffer )const
			{
				uint32_t bufferIndex{};

				for ( auto & [binding, attach] : getPass().getInouts() )
				{
					if ( !attach->isNoTransition()
						&& attach->isStorageBuffer() )
					{
						auto buffer = attach->buffer();
						auto currentState = context.getAccessState( buffer.data->buffer, attach->getBufferRange() );
						AccessState dstState = ComputeShaderReadWriteState;
						context.memoryBarrier( commandBuffer
							, buffer.data->buffer
							, attach->getBufferRange()
							, { currentState.access, currentState.pipelineStage }
							, dstState
							, true );
					}

					++bufferIndex;
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

	void createBucketMergeSortPasses( crg::FramePassGroup & graph
		, RenderDevice const & device
		, crg::ResourcesCache & resources
		, String const & name
		, GetNumKeysCallback const & getNumKeys
		, IsEnabledCallback const & isEnabled
		, BufferBase & keys
		, BufferBase & values )
	{
		auto & bucketSort = graph.createPass( "BucketSort/" + name
			, [getNumKeys, isEnabled, &device, name]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto runPass = makeRawUnique< bucket::FramePass >( framePass, context, runnableGraph
					, device, getNumKeys, isEnabled, name );
				c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
					, runPass->getTimer() );
				return runPass;
			} );
		keys.setLastAttach( bucketSort.addInOutStorageT( *keys.getLastAttach(), bucket::Bindings::eInOutKeys ) );
		values.setLastAttach( bucketSort.addInOutStorageT( *values.getLastAttach(), bucket::Bindings::eInOutValues ) );

		BufferT< s32 > mergePathPartitions{ device, resources
			, cuT( "C3D_" ) + name + cuT( "MergePathPartitions" )
			, BufferCreateFlags::eNone
			, merge::getMaxMergePathPartitionsCount()
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eTransferDst };
		auto & mergeSort = graph.createPass( "MergeSort/" + name
			, [getNumKeys, isEnabled, &device, name]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto runPass = makeRawUnique< merge::FramePass >( framePass, context, runnableGraph
					, device, getNumKeys, isEnabled, name );
				c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
					, runPass->getTimer() );
				return runPass;
			} );
		keys.setLastAttach( mergeSort.addInOutStorageT( *keys.getLastAttach(), merge::Bindings::eInOutKeys ) );
		values.setLastAttach( mergeSort.addInOutStorageT( *values.getLastAttach(), merge::Bindings::eInOutValues ) );
		mergeSort.addClearableOutputStorageBufferT( mergePathPartitions.bufferViewId, merge::Bindings::eMergePathPartitions );
	}

	//*********************************************************************************************
}
