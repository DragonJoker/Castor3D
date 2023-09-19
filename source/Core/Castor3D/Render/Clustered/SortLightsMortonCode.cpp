#include "Castor3D/Render/Clustered/SortLightsMortonCode.hpp"

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

	namespace srtmrt
	{
		static uint32_t constexpr NumThreads = 256u;
		static uint32_t constexpr NumThreadsPerThreadGroup = 256u;
		static uint32_t constexpr NumValuesPerThread = 8u;
		static uint32_t constexpr NumValuesPerThreadGroup = NumThreads * NumValuesPerThread;

		struct DispatchData
		{
			u32 numElements;
			u32 chunkSize;
		};

		using InMortonCodesCallback = ashes::BufferBase & ( FrustumClusters::* )()const noexcept;
		using OutMortonCodesCallback = ashes::BufferBase & ( FrustumClusters::* )()const noexcept;
		using InIndicesCallback = ashes::BufferBase & ( FrustumClusters::* )()const noexcept;
		using OutIndicesCallback = ashes::BufferBase & ( FrustumClusters::* )()const noexcept;

		struct LightData
		{
			LightType lightType;
			std::string name;
			InMortonCodesCallback m_inMortonCodes;
			OutMortonCodesCallback m_outMortonCodes;
			InIndicesCallback m_inIndices;
			OutIndicesCallback m_outIndices;

			ashes::BufferBase & inMortonCodes( FrustumClusters & clusters )
			{
				return ( clusters.*m_inMortonCodes )();
			}

			ashes::BufferBase & outMortonCodes( FrustumClusters & clusters )
			{
				return ( clusters.*m_outMortonCodes )();
			}

			ashes::BufferBase & inIndices( FrustumClusters & clusters )
			{
				return ( clusters.*m_inIndices )();
			}

			ashes::BufferBase & outIndices( FrustumClusters & clusters )
			{
				return ( clusters.*m_outIndices )();
			}
		};

		//*****************************************************************************************

		namespace radix
		{
			enum BindingPoints
			{
				eInputKeys,
				eInputValues,
				eOutputKeys,
				eOutputValues,
			};

			static ShaderPtr createShader()
			{
				sdw::ComputeWriter writer;

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

				sdw::PushConstantBuffer pcb{ writer, "C3D_DispatchData", "c3d_dispatchData" };
				auto c3d_numElements = pcb.declMember< sdw::UInt >( "c3d_numElements" );
				auto c3d_chunkSize = pcb.declMember< sdw::UInt >( "c3d_chunkSize" );
				pcb.end();

				auto gsKeys = writer.declSharedVariable< sdw::UInt >( "gsKeys", NumThreads );		// A temporary buffer to store the input keys.                                          (1,024 Bytes)
				auto gsValues = writer.declSharedVariable< sdw::UInt >( "gsValues", NumThreads );	// A temporary buffer to store the input values.                                        (1,024 Bytes)
				auto gsE = writer.declSharedVariable< sdw::UInt >( "gsE", NumThreads );				// Set a 1 for all false sort keys (b == 0) and a 0 for all true sort keys (b == 1)     (1,024 Bytes)
				auto gsF = writer.declSharedVariable< sdw::UInt >( "gsF", NumThreads );				// Scan the splits. This results in the output index of all false sort keys (b == 0)    (1,024 Bytes)
				auto gsD = writer.declSharedVariable< sdw::UInt >( "gsD", NumThreads );				// The desination index for the ouput key and value.                                    (1,024 Bytes)
				auto gsTotalFalses = writer.declSharedVariable< sdw::UInt >( "gsTotalFalses" );		// The result of e[NUM_THREADS - 1] + f[NUM_THREADS - 1];                               (4 Bytes)

				writer.implementMainT< sdw::VoidT >( NumThreads
					, [&]( sdw::ComputeIn in )
					{
						// The number of bits to consider sorting.
						// In this case, the input keys are 30-bit morton codes.
						const u32 NumBits = 30u;

						auto groupIndex = in.localInvocationIndex;
						auto threadIndex = in.globalInvocationID.x();

						// Store the input key and values into shared memory.
						gsKeys[groupIndex] = writer.ternary( threadIndex < c3d_numElements, c3d_inputKeys[threadIndex], sdw::UInt{ UINT_MAX } );
						gsValues[groupIndex] = writer.ternary( threadIndex < c3d_numElements, c3d_inputValues[threadIndex], sdw::UInt{ UINT_MAX } );

						// Loop over the bits starting at the least-significant bit.
						FOR( writer, sdw::UInt, b, 0_u, b < NumBits, ++b )
						{
							// 1. In a temporary buffer in shared memory, we set a 1 for all false 
							//    sort keys (b = 0) and a 0 for all true sort keys.
							gsE[groupIndex] = writer.ternary( ( ( gsKeys[groupIndex] >> b ) & 1_u ) == 0_u
								, 1_u
								, 0_u );

							// Sync group shared memory writes.
							shader::groupMemoryBarrierWithGroupSync( writer );

							IF( writer, groupIndex == 0_u )
							{
								gsF[groupIndex] = 0_u;
							}
							ELSE
							{
								gsF[groupIndex] = gsE[groupIndex - 1_u];
							}
							FI;

							// Sync group shared memory writes.
							shader::groupMemoryBarrierWithGroupSync( writer );
							auto temp = writer.declLocale( "temp", 0_u );

							// 2. We then scan (prefix sum) this buffer. This is the enumerate operation; 
							//    each false sort key now contains its destination address in the scan 
							//    output, which we will call f. These first two steps are equivalent to 
							//    a stream compaction operation on all false sort keys.
							for ( u32 i = 1; i < NumThreads; i <<= 1u )
							{
								temp = gsF[groupIndex];

								IF( writer, groupIndex > i )
								{
									temp += gsF[groupIndex - i];
								}
								FI;

								// Sync group shared memory reads before writes.
								shader::groupMemoryBarrierWithGroupSync( writer );

								gsF[groupIndex] = temp;

								// Sync group shared memory writes.
								shader::groupMemoryBarrierWithGroupSync( writer );
							}

							// 3. The last element in the scan's output now contains the total 
							//    number of false sort keys. We write this value to a shared 
							//    variable, gs_TotalFalses.
							IF ( writer, groupIndex == 0_u )
							{
								gsTotalFalses = gsE[NumThreads - 1u] + gsF[NumThreads - 1u];
							}
							FI;

							// Sync group shared memory writes.
							shader::groupMemoryBarrierWithGroupSync( writer );

							// 4. Now we compute the destination address for the true sort keys. For 
							// a sort key at index i, this address is t = i - f + totalFalses. We 
							// then select between t and f depending on the value of b to get the 
							// destination address d of each fragment.
							gsD[groupIndex] = writer.ternary( gsE[groupIndex] == 1u
								, gsF[groupIndex]
								, groupIndex - gsF[groupIndex] + gsTotalFalses );

							// 5. Finally, we scatter the original sort keys to destination address 
							//    d. The scatter pattern is a perfect permutation of the input, so 
							//    we see no write conflicts with this scatter.
							auto key = writer.declLocale( "key", gsKeys[groupIndex] );
							auto value = writer.declLocale( "value", gsValues[groupIndex] );

							// Sync group shared memory reads before writes.
							shader::groupMemoryBarrierWithGroupSync( writer );

							gsKeys[gsD[groupIndex]] = key;
							gsValues[gsD[groupIndex]] = value;

							// Sync group shared memory writes.
							shader::groupMemoryBarrierWithGroupSync( writer );
						}
						ROF;

						// Now commit the results to global memory.
						c3d_outputKeys[threadIndex] = gsKeys[groupIndex];
						c3d_outputValues[threadIndex] = gsValues[groupIndex];
					} );
				return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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
					, LightData lightData )
					: crg::RunnablePass{ framePass
						, context
						, graph
						, { [this]( uint32_t index ){ doInitialise( index ); }
							, GetPipelineStateCallback( [](){ return crg::getPipelineState( VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT ); } )
							, [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); }
							, GetPassIndexCallback( [](){ return 0u; } )
							, IsEnabledCallback( [this](){ return doIsEnabled(); } )
							, IsComputePassCallback( [](){ return true; } ) }
						, crg::ru::Config{ 1u, true /* resettable */ } }
					, m_clusters{ clusters }
					, m_lightCache{ clusters.getCamera().getScene()->getLightCache() }
					, m_lightData{ std::move( lightData ) }
					, m_pipeline{ framePass, context, graph, device, this }
				{
				}

				CRG_API void resetPipeline( crg::VkPipelineShaderStageCreateInfoArray config
					, uint32_t index )
				{
					resetCommandBuffer( index );
					m_pipeline.pipeline.resetPipeline( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_pipeline.createInfo ), index );
					doCreatePipeline( index, m_pipeline );
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
						, FramePass * parent )
						: shader{ VK_SHADER_STAGE_COMPUTE_BIT, "RadixSort", createShader() }
						, createInfo{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, shader ) } }
						, cpConfig{ crg::getDefaultV< InitialiseCallback >()
							, nullptr
							, IsEnabledCallback( [parent]() { return parent->doIsEnabled(); } )
							, GetPassIndexCallback( []() { return 0u; } )
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
							, 1u }
					{
					}
				};

			private:
				FrustumClusters & m_clusters;
				LightCache const & m_lightCache;
				LightData m_lightData;
				Pipeline m_pipeline;

			private:
				void doInitialise( uint32_t index )
				{
					m_pipeline.pipeline.initialise();
					doCreatePipeline( index, m_pipeline );
				}

				bool doIsEnabled()const
				{
					return m_clusters.getConfig().sortLights
						&& m_clusters.needsLightsUpdate()
						&& m_lightCache.getLightsBufferCount( m_lightData.lightType ) > 0;
				}

				void doRecordInto( crg::RecordContext & context
					, VkCommandBuffer commandBuffer
					, uint32_t index )
				{
					// Build bottom level of the BVH.
					auto lightsCount = m_lightCache.getLightsBufferCount( m_lightData.lightType );
					auto numThreadGroups = castor::divRoundUp( lightsCount, NumThreadsPerThreadGroup );
					DispatchData data{ lightsCount, 0u };
					m_pipeline.pipeline.recordInto( context, commandBuffer, index );
					m_context.vkCmdPushConstants( commandBuffer, m_pipeline.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &data );
					m_context.vkCmdDispatch( commandBuffer, numThreadGroups, 1u, 1u );
				}

				void doCreatePipeline( uint32_t index
					, Pipeline & pipeline )
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

		namespace merge
		{
			enum BindingPoints
			{
				eInputKeys,
				eInputValues,
				eInputMergePathPartitions,
				eOutputKeys,
				eOutputValues,
				eOutputMergePathPartitions,
			};

			static ShaderPtr createShader( bool mergePathPartitions )
			{
				sdw::ComputeWriter writer;

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

				auto inputMergePathPartitionsBuffer = writer.declStorageBuffer( "c3d_inputMergePathPartitionsBuffer"
					, uint32_t( eInputMergePathPartitions )
					, 0u );
				auto c3d_inputMergePathPartitions = inputMergePathPartitionsBuffer.declMemberArray< sdw::Int >( "ip" );
				inputMergePathPartitionsBuffer.end();

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

				auto outputMergePathPartitionsBuffer = writer.declStorageBuffer( "c3d_outputMergePathPartitionsBuffer"
					, uint32_t( eOutputMergePathPartitions )
					, 0u );
				auto c3d_outputMergePathPartitions = outputMergePathPartitionsBuffer.declMemberArray< sdw::Int >( "op" );
				outputMergePathPartitionsBuffer.end();

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
					, [&]( sdw::Int a0, sdw::Int aCount
						, sdw::Int b0, sdw::Int bCount
						, sdw::Int diag, sdw::Boolean bUseSharedMem )
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
							FI;
						}
						ELIHW;

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
					, [&]( sdw::Int a0, sdw::Int a1
						, sdw::Int b0, sdw::Int b1
						, sdw::Int diag
						, sdw::Int numValues, sdw::Int out0 )
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
							FI;
						}
						ROF;
					}
					, sdw::InInt{ writer, "a0" }
					, sdw::InInt{ writer, "a1" }
					, sdw::InInt{ writer, "b0" }
					, sdw::InInt{ writer, "b1" }
					, sdw::InInt{ writer, "diag" }
					, sdw::InInt{ writer, "numValues" }
					, sdw::InInt{ writer, "out0" } );

				writer.implementMainT< sdw::VoidT >( NumThreads
					, [&]( sdw::ComputeIn in )
					{
						auto threadIndex = in.globalInvocationID.x();
						auto chunkSize = c3d_chunkSize;
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
							auto globalPartition = writer.declLocale( "globalPartition", ( sortGroup * numPartitionsPerSortGroup ) + partitionInSortGroup );
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
								auto mergPath = writer.declLocale( "mergePath", mergePath( a0, aCount, b0, bCount, diag, 0_b ) );

								// Write the merge path to global memory.
								c3d_outputMergePathPartitions[globalPartition] = mergPath;
							}
							FI;
						}
						else
						{
							auto groupID = in.workGroupID.x();
							auto groupIndex = in.localInvocationIndex;

							// Number of sort groups needed to sort all chunks.
							auto numSortGroups = writer.declLocale( "numSortGroups", max( numChunks / 2_u, 1_u ) );

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
							auto mergePath0 = writer.declLocale( "mergePath0", c3d_inputMergePathPartitions[globalPartition] );
							auto mergePath1 = writer.declLocale( "mergePath1", c3d_inputMergePathPartitions[globalPartition + 1_u] );
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
								FI;

								gsKeys[diag + i] = key;
								gsValues[diag + i] = value;
							}

							// Sync loading of keys/values in shared memory.
							shader::groupMemoryBarrierWithGroupSync( writer );

							// Compute the mergepath for this thread using shared memory.
							auto mergPath = writer.declLocale( "mergePath", mergePath( 0_i, numA, numA, numB, diag, 1_b ) );

							// Perform the serial merge using shared memory.
							serialMerge( mergPath, numA, numA + diag - mergPath, numA + numB, diag0 + diag, numValues, chunkOffsetA0 );
						}
					} );
				return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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
					, LightData lightData )
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
					, m_lightData{ std::move( lightData ) }
					, m_partitions{ framePass, context, graph, device, true, this }
					, m_merge{ framePass, context, graph, device, false, this }
				{
				}

				CRG_API void resetPipeline( crg::VkPipelineShaderStageCreateInfoArray config
					, uint32_t index )
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
						, FramePass * parent )
						: shader{ VK_SHADER_STAGE_COMPUTE_BIT, mergePathPartitions ? std::string{ "MergePathPartitions" } : std::string{ "MergeSort" }, createShader( mergePathPartitions ) }
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
				FrustumClusters & m_clusters;
				LightCache const & m_lightCache;
				LightData m_lightData;
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
						&& m_lightCache.getLightsBufferCount( m_lightData.lightType ) > 0;
				}

				void doRecordInto( crg::RecordContext & context
					, VkCommandBuffer commandBuffer
					, uint32_t index )
				{
					auto totalValues = m_lightCache.getLightsBufferCount( m_lightData.lightType );
					auto chunkSize = NumThreadsPerThreadGroup;

					// The total number of complete chunks to sort.
					auto numChunks = getLightsMortonCodeChunkCount( totalValues );
					DispatchData data{ totalValues, 0u };

					while ( numChunks > 1u )
					{
						data.chunkSize = chunkSize;

						// Number of sort groups required to sort all chunks.
						// Each sort group merge sorts 2 chunks into a single chunk.
						auto numSortGroups = numChunks / 2u;

						// Compute merge path partitions per thread group.
						{
							m_partitions.pipeline.recordInto( context, commandBuffer, index );

							// The number of thread groups that are required per sort group.
							auto numThreadGroupsPerSortGroup = castor::divRoundUp( chunkSize * 2u, NumValuesPerThreadGroup );

							// The number of merge path partitions that need to be computed.
							auto numMergePathPartitionsPerSortGroup = numThreadGroupsPerSortGroup + 1u;
							auto totalMergePathPartitions = numMergePathPartitionsPerSortGroup * numSortGroups;

							// The number of thread groups needed to compute all merge path partitions.
							auto numThreadGroups = castor::divRoundUp( totalMergePathPartitions, NumThreadsPerThreadGroup );

							m_context.vkCmdPushConstants( commandBuffer, m_partitions.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &data );
							m_context.vkCmdDispatch( commandBuffer, numThreadGroups, 1u, 1u );

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

							// The number of values that each sort group will sort.
							// Each sort group merges 2 chunks into 1.
							auto numValuesPerSortGroup = std::min( chunkSize * 2u, totalValues );

							// The number of thread groups required to sort all values.
							auto numThreadGroupsPerSortGroup = castor::divRoundUp( numValuesPerSortGroup, NumValuesPerThreadGroup );

							m_context.vkCmdPushConstants( commandBuffer, m_merge.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &data );
							m_context.vkCmdDispatch( commandBuffer, numThreadGroupsPerSortGroup * numSortGroups, 1u, 1u );
						}

						// Ping-pong the buffers
						index = 1u - index;

						chunkSize *= 2;
						numChunks = castor::divRoundUp( totalValues, chunkSize );
					}
				}

				void doBarriers( crg::RecordContext & context
					, VkCommandBuffer commandBuffer
					, uint32_t passIndex )
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
								, crg::AccessState{ VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT }
								, true );
						}
					}
				}

				void doCreatePipeline( uint32_t index
					, Pipeline & pipeline )
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

		static crg::FramePass const & createSortLightMortonCodesPasses( crg::FramePassGroup & graph
			, crg::FramePass const * previousPass
			, RenderDevice const & device
			, FrustumClusters & clusters
			, LightData lightData )
		{
			// Create Radix sort pass
			auto & radix = graph.createPass( "RadixSort" + lightData.name
				, [&clusters, &device, lightData]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = std::make_unique< radix::FramePass >( framePass
						, context
						, graph
						, device
						, clusters
						, lightData );
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, result->getTimer() );
					return result;
				} );
			radix.addDependency( *previousPass );
			createInputStoragePassBinding( radix, uint32_t( radix::eInputKeys ), "C3D_In" + lightData.name + "LightMortonCodes", lightData.outMortonCodes( clusters ), 0u, ashes::WholeSize );
			createInputStoragePassBinding( radix, uint32_t( radix::eInputValues ), "C3D_In" + lightData.name + "LightIndices", lightData.outIndices( clusters ), 0u, ashes::WholeSize );
			createClearableOutputStorageBinding( radix, uint32_t( radix::eOutputKeys ), "C3D_Out" + lightData.name + "LightMortonCodes", lightData.inMortonCodes( clusters ), 0u, ashes::WholeSize );
			createClearableOutputStorageBinding( radix, uint32_t( radix::eOutputValues ), "C3D_Out" + lightData.name + "LightIndices", lightData.inIndices( clusters ), 0u, ashes::WholeSize );

			// Create Merge sort pass
			auto & mergeSort = graph.createPass( "MergeSort" + lightData.name
				, [&clusters, &device, lightData]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = std::make_unique< merge::FramePass >( framePass
						, context
						, graph
						, device
						, clusters
						, lightData );
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, result->getTimer() );
					return result;
				} );
			mergeSort.addDependency( radix );
			createInOutStoragePassBinding( mergeSort, uint32_t( merge::eInputKeys ), "C3D_In" + lightData.name + "MortonCodes", { &lightData.inMortonCodes( clusters ), &lightData.outMortonCodes( clusters ) }, 0u, ashes::WholeSize );
			createInOutStoragePassBinding( mergeSort, uint32_t( merge::eInputValues ), "C3D_In" + lightData.name + "LightIndices", { &lightData.inIndices( clusters ), &lightData.outIndices( clusters ) }, 0u, ashes::WholeSize );
			createInputStoragePassBinding( mergeSort, uint32_t( merge::eInputMergePathPartitions ), "C3D_In" + lightData.name + "MergePathPartitions", clusters.getMergePathPartitionsBuffer(), 0u, ashes::WholeSize );
			createInOutStoragePassBinding( mergeSort, uint32_t( merge::eOutputKeys ), "C3D_Out" + lightData.name + "MortonCodes", { &lightData.outMortonCodes( clusters ), &lightData.inMortonCodes( clusters ) }, 0u, ashes::WholeSize );
			createInOutStoragePassBinding( mergeSort, uint32_t( merge::eOutputValues ), "C3D_Out" + lightData.name + "LightIndices", { &lightData.outIndices( clusters ), &lightData.inIndices( clusters ) }, 0u, ashes::WholeSize );
			createClearableOutputStorageBinding( mergeSort, uint32_t( merge::eOutputMergePathPartitions ), "C3D_Out" + lightData.name + "MergePathPartitions", clusters.getMergePathPartitionsBuffer(), 0u, ashes::WholeSize );

			return mergeSort;
		}
	}

	//*********************************************************************************************

	u32 getLightsMortonCodeChunkCount( u32 lightCount )
	{
		auto chunkSize = srtmrt::NumThreadsPerThreadGroup;

		// The total number of complete chunks to sort.
		auto numChunks = castor::divRoundUp( lightCount, chunkSize );

		return numChunks;
	}

	crg::FramePassArray createSortLightsMortonCodePass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, FrustumClusters & clusters )
	{
		// Radix sort
		auto & point = srtmrt::createSortLightMortonCodesPasses( graph
			, previousPass
			, device
			, clusters
			, { LightType::ePoint
				, "Point"
				, &FrustumClusters::getInputPointLightMortonCodesBuffer
				, &FrustumClusters::getOutputPointLightMortonCodesBuffer
				, &FrustumClusters::getInputPointLightIndicesBuffer
				, &FrustumClusters::getOutputPointLightIndicesBuffer } );

		// Merge sort
		auto & spot =  srtmrt::createSortLightMortonCodesPasses( graph
			, previousPass
			, device
			, clusters
			, { LightType::eSpot
				, "Spot"
				, &FrustumClusters::getInputSpotLightMortonCodesBuffer
				, &FrustumClusters::getOutputSpotLightMortonCodesBuffer
				, &FrustumClusters::getInputSpotLightIndicesBuffer
				, &FrustumClusters::getOutputSpotLightIndicesBuffer } );

		return { &point, &spot };
	}

	//*********************************************************************************************
}
