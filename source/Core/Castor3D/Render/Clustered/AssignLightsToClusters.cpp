#include "Castor3D/Render/Clustered/AssignLightsToClusters.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/ClustersConfig.hpp"
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

	namespace dspclst
	{
		enum BindingPoints
		{
			eMainCamera,
			eClustersCamera,
			eLights,
			eClusters,
			eAllLightsAABB,
			eClustersAABB,
			ePointLightIndex,
			ePointLightCluster,
			eSpotLightIndex,
			eSpotLightCluster,
			ePointLightBVH,
			eSpotLightBVH,
			ePointLightIndices,
			eSpotLightIndices,
			eUniqueClusters,
		};

		static ShaderPtr createShader( RenderDevice const & device
			, ClustersConfig const & config )
		{
			uint32_t NumThreads = config.useLightsBVH
				? 32u
				: MaxLightsPerCluster;

			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_numChildNodes = writer.declConstantArray< sdw::UInt >( "c3d_numChildNodes"
				, { 1_u			/* 1 level   =32^0 */
				, 33_u			/* 2 levels  +32^1 */
				, 1057_u		/* 3 levels  +32^2 */
				, 33825_u		/* 4 levels  +32^3 */
				, 1082401_u		/* 5 levels  +32^4 */
				, 34636833_u	/* 6 levels  +32^5 */ } );

			// Inputs
			C3D_CameraNamed( writer
				, Main
				, eMainCamera
				, 0u );
			C3D_CameraNamed( writer
				, Clusters
				, eClustersCamera
				, 0u );
			shader::LightsBuffer lights{ writer
				, eLights
				, 0u };
			C3D_Clusters( writer
				, eClusters
				, 0u
				, &config );
			C3D_AllLightsAABB( writer
				, eAllLightsAABB
				, 0u );
			C3D_ClustersAABB( writer
				, eClustersAABB
				, 0u );
			C3D_PointLightClusterIndex( writer
				, ePointLightIndex
				, 0u );
			C3D_PointLightClusterGrid( writer
				, ePointLightCluster
				, 0u );
			C3D_SpotLightClusterIndex( writer
				, eSpotLightIndex
				, 0u );
			C3D_SpotLightClusterGrid( writer
				, eSpotLightCluster
				, 0u );
			C3D_PointLightBVHEx( writer
				, ePointLightBVH
				, 0u
				, config.useLightsBVH );
			C3D_SpotLightBVHEx( writer
				, eSpotLightBVH
				, 0u
				, config.useLightsBVH );
			C3D_PointLightIndicesEx( writer
				, ePointLightIndices
				, 0u
				, config.sortLights );
			C3D_SpotLightIndicesEx( writer
				, eSpotLightIndices
				, 0u
				, config.sortLights );
			C3D_UniqueClustersEx( writer
				, eUniqueClusters
				, 0u
				, config.parseDepthBuffer );

			static constexpr s32 MaxValues = 1024;

			// Using a stack of node IDs to traverse the BVH was inspired by:
			// Source: https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-ii-tree-traversal-gpu/
			// Author: Tero Karras (NVIDIA)
			// Retrieved: September 13, 2016
			auto gsNodeStack = writer.declSharedVariable< sdw::UInt >( "gsNodeStack", u32( MaxValues ), config.useLightsBVH.value() );	// This should be enough to push 32 layers of nodes (32 nodes per layer).
			auto gsStackPtr = writer.declSharedVariable< sdw::Int >( "gsStackPtr", config.useLightsBVH );			// The current index in the node stack.
			auto gsParentIndex = writer.declSharedVariable< sdw::UInt >( "gsParentIndex", config.useLightsBVH );	// The index of the parent node in the BVH that is currently being processed.

			auto gsPointLightStartOffset = writer.declSharedVariable< sdw::UInt >( "gsPointLightStartOffset" );
			auto gsPointLights = writer.declSharedVariable< shader::AppendArrayT< sdw::UInt > >( "gsPointLights"
				, true, "U32", MaxLightsPerCluster );

			auto gsSpotLightStartOffset = writer.declSharedVariable< sdw::UInt >( "gsSpotLightStartOffset" );
			auto gsSpotLights = writer.declSharedVariable< shader::AppendArrayT< sdw::UInt > >( "gsSpotLights"
				, true, "U32", MaxLightsPerCluster );

			auto gsClusterIndex1D = writer.declSharedVariable< sdw::UInt32 >( "gsClusterIndex1D" );
			auto gsClusterAABB = writer.declSharedVariable< shader::AABB >( "gsClusterAABB" );
			auto gsClusterSphere = writer.declSharedVariable< sdw::Vec4 >( "gsClusterSphere" );

			shader::Utils utils{ writer };
			sdw::Function< sdw::Void, sdw::InUInt > pushNode;
			sdw::Function< sdw::UInt > popNode;

			castor::Function< sdw::UInt( sdw::UInt, sdw::UInt ) > getFirstChild;
			castor::Function< sdw::Boolean( sdw::UInt, sdw::UInt ) > isLeafNode;
			castor::Function< sdw::UInt( sdw::UInt, sdw::UInt ) > getLeafIndex;

			if ( config.useLightsBVH )
			{
				pushNode = writer.implementFunction< sdw::Void >( "pushNode"
					, [&writer, &gsStackPtr, &gsNodeStack]( sdw::UInt const & nodeIndex )
					{
						auto stackPtr = writer.declLocale( "stackPtr"
							, sdw::atomicAdd( gsStackPtr, 1_i ) );

						IF( writer, stackPtr < MaxValues )
						{
							gsNodeStack[stackPtr] = nodeIndex;
						}
						FI
					}
					, sdw::InUInt{ writer, "nodeIndex" } );

				popNode = writer.implementFunction< sdw::UInt >( "popNode"
					, [&writer, &gsStackPtr, &gsNodeStack]()
					{
						auto nodeIndex = writer.declLocale( "nodeIndex"
							, 0_u );
						auto stackPtr = writer.declLocale( "stackPtr"
							, sdw::atomicAdd( gsStackPtr, -1_i ) );

						IF( writer, stackPtr > 0 && stackPtr < MaxValues )
						{
							nodeIndex = gsNodeStack[stackPtr - 1];
						}
						FI

						writer.returnStmt( nodeIndex );
					} );

				// Get the index of the the first child node in the BVH.
				getFirstChild = [&writer]( sdw::UInt const & parentIndex
					, sdw::UInt const & numLevels )
				{
					return writer.ternary( numLevels > 0_u
						, parentIndex * 32_u + 1_u
						, 0_u );
				};

				// Check to see if an index of the BVH is a leaf.
				isLeafNode = [&writer, &c3d_numChildNodes]( sdw::UInt const & childIndex
					, sdw::UInt const & numLevels )
				{
					return writer.ternary( numLevels > 0_u
						, childIndex > ( c3d_numChildNodes[numLevels - 1_u] - 1_u )
						, 1_b );
				};

				// Get the index of a leaf node given the node ID in the BVH.
				getLeafIndex = [&writer, &c3d_numChildNodes]( sdw::UInt const & nodeIndex
					, sdw::UInt const & numLevels )
				{
					return writer.ternary( numLevels > 0_u
						, nodeIndex - c3d_numChildNodes[numLevels - 1_u]
						, nodeIndex );
				};
			}

			// Check to see if on AABB intersects another AABB.
			// Source: Real-time collision detection, Christer Ericson (2005)
			auto aabbIntersectAABB = writer.implementFunction< sdw::Boolean >( "aabbIntersectAABB"
				, [&writer]( shader::AABB const & a
					, shader::AABB const & b )
				{
					auto result = writer.declLocale( "result"
						, 1_b );

					for ( int i = 0; i < 3; ++i )
					{
						result = result
							&& ( a.max()[i] >= b.min()[i]
								&& a.min()[i] <= b.max()[i] );
					}

					writer.returnStmt( result );
				}
				, shader::InAABB{ writer, "a" }
				, shader::InAABB{ writer, "b" } );

			auto sphereInsideAABB = writer.implementFunction< sdw::Boolean >( "sphereInsideAABB"
				, [&writer]( sdw::Vec4 const & sphere
					, shader::AABB const & aabb )
				{
					auto sqDistance = writer.declLocale( "sqDistance"
						, 0.0_f );
					auto v = writer.declLocale( "v"
						, 0.0_f );

					for ( int i = 0; i < 3; ++i )
					{
						v = sphere[i];

						IF( writer, v < aabb.min()[i] )
						{
							sqDistance += pow( aabb.min()[i] - v, 2.0_f );
						}
						FI
						IF( writer, v > aabb.max()[i] )
						{
							sqDistance += pow( v - aabb.max()[i], 2.0_f );
						}
						FI
					}

					writer.returnStmt( sqDistance <= sphere.w() * sphere.w() );
				}
				, sdw::InVec4{ writer, "sphere" }
				, shader::InAABB{ writer, "aabb" } );

			auto coneInsideSphere = writer.implementFunction< sdw::Boolean >( "coneInsideSphere"
				, [&writer]( shader::Cone const & cone
					, sdw::Vec4 const & sphere )
				{
					auto V = writer.declLocale( "V"
						, sphere.xyz() - cone.apex() );
					auto lenSqV = writer.declLocale( "lenSqV"
						, dot( V, V ) );
					auto lenV1 = writer.declLocale( "lenV1"
						, dot( V, cone.direction() ) );
					auto distanceClosestPoint = writer.declLocale( "distanceClosestPoint"
						, cone.apertureCos() * sqrt( lenSqV - lenV1 * lenV1 ) - lenV1 * cone.apertureSin() );

					auto angleCull = distanceClosestPoint > sphere.w();
					auto frontCull = lenV1 > sphere.w() + cone.range();
					auto backCull = lenV1 < -sphere.w();

					writer.returnStmt( !( angleCull || frontCull || backCull ) );
				}
				, shader::InCone{ writer, "cone" }
				, sdw::InVec4{ writer, "sphere" } );

			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&]( sdw::ComputeIn const & in )
				{
					auto const & groupIndex = in.localInvocationIndex;

					auto processPointLightAABB = [&]( sdw::UInt const & leafIndex )
					{
						auto lightIndex = config.sortLights ? c3d_pointLightIndices[leafIndex] : leafIndex;
						auto aabb = writer.declLocale( "aabb"
							, c3d_allLightsAABB[lightIndex] );
						auto center = writer.declLocale( "center"
							, aabb.min().xyz() + ( aabb.max().xyz() - aabb.min().xyz() ) / 2.0_f );

						IF( writer, sphereInsideAABB( vec4( center, aabb.min().w() ), gsClusterAABB ) )
						{
							gsPointLights.appendData( lightIndex, MaxLightsPerCluster );
						}
						FI
					};

					auto processSpotLightAABB = [&]( sdw::UInt const & leafIndex )
					{
						auto lightIndex = config.sortLights ? c3d_spotLightIndices[leafIndex] : leafIndex;
						auto aabb = writer.declLocale( "aabb"
							, c3d_allLightsAABB[c3d_clustersData.pointLightCount() + lightIndex] );

						IF( writer, aabbIntersectAABB( aabb, gsClusterAABB ) )
						{
							if ( config.useSpotBoundingCone )
							{
								auto spot = writer.declLocale( "spot"
									, lights.getSpotLight( lights.getPointsEnd() + lightIndex * castor3d::SpotLight::LightDataComponents ) );
								auto cone = writer.declLocale( "cone"
									, shader::Cone{ c3d_cameraDataClusters.worldToCurView( vec4( spot.position(), 1.0_f ) ).xyz()
										, c3d_cameraDataClusters.worldToCurView( -spot.direction() )
										, computeRange( spot )
										, spot.outerCutOffCos()
										, spot.outerCutOffSin()
										, spot.outerCutOffTan() } );

								IF( writer, coneInsideSphere( cone, gsClusterSphere ) )
								{
									gsSpotLights.appendData( lightIndex, MaxLightsPerCluster );
								}
								FI
							}
							else
							{
								gsSpotLights.appendData( lightIndex, MaxLightsPerCluster );
							}
						}
						FI
					};

					IF( writer, groupIndex == 0_u )
					{
						gsPointLights.resetCount();
						gsSpotLights.resetCount();
						gsStackPtr = 0_i;
						gsParentIndex = 0_u;

						gsClusterIndex1D = config.parseDepthBuffer
							? c3d_uniqueClusters[in.workGroupID.x()]
							: c3d_clustersData.computeClusterIndex1D( uvec3( in.workGroupID ) );
						gsClusterAABB = c3D_clustersAABB[gsClusterIndex1D];
						auto aabbCenter = writer.declLocale( "aabbCenter"
							, gsClusterAABB.min().xyz() + ( gsClusterAABB.max().xyz() - gsClusterAABB.min().xyz() ) / 2.0_f );
						gsClusterSphere = vec4( aabbCenter, distance( gsClusterAABB.max().xyz(), aabbCenter ) );

						if ( config.useLightsBVH )
						{
							pushNode( 0_u );
						}
					}
					FI

					shader::groupMemoryBarrierWithGroupSync( writer );

					if ( config.useLightsBVH )
					{
						auto childOffset = writer.declLocale( "childOffset", groupIndex );

						// Check point light BVH
						DOWHILE( writer, gsParentIndex > 0_u )
						{
							auto childIndex = writer.declLocale( "childIndex"
								, getFirstChild( gsParentIndex, c3d_clustersData.pointLightLevels() ) + childOffset );

							IF( writer, isLeafNode( childIndex, c3d_clustersData.pointLightLevels() ) )
							{
								auto leafIndex = writer.declLocale( "leafIndex"
									, getLeafIndex( childIndex, c3d_clustersData.pointLightLevels() ) );

								IF( writer, leafIndex < c3d_clustersData.pointLightCount() )
								{
									processPointLightAABB( leafIndex );
								}
								FI
							}
							ELSEIF( aabbIntersectAABB( gsClusterAABB, c3d_pointLightBVH[childIndex] ) )
							{
								pushNode( childIndex );
							}
							FI

							shader::groupMemoryBarrierWithGroupSync( writer );

							IF( writer, groupIndex == 0_u )
							{
								gsParentIndex = popNode();
							}
							FI

							shader::groupMemoryBarrierWithGroupSync( writer );
						}
						ELIHWOD

						shader::groupMemoryBarrierWithGroupSync( writer );

						// Reset the stack.
						IF( writer, groupIndex == 0_u )
						{
							gsStackPtr = 0_i;
							gsParentIndex = 0_u;

							// Push the root node (at index 0) on the node stack.
							pushNode( 0_u );
						}
						FI

						shader::groupMemoryBarrierWithGroupSync( writer );

						// Check spot light BVH
						DOWHILE( writer, gsParentIndex > 0_u )
						{
							auto childIndex = writer.declLocale( "childIndex"
								, getFirstChild( gsParentIndex, c3d_clustersData.spotLightLevels() ) + childOffset );

							IF( writer, isLeafNode( childIndex, c3d_clustersData.spotLightLevels() ) )
							{
								auto leafIndex = writer.declLocale( "leafIndex"
									, getLeafIndex( childIndex, c3d_clustersData.spotLightLevels() ) );

								IF( writer, leafIndex < c3d_clustersData.spotLightCount() )
								{
									processSpotLightAABB( leafIndex );
								}
								FI
							}
							ELSEIF( aabbIntersectAABB( gsClusterAABB, c3d_spotLightBVH[childIndex] ) )
							{
								pushNode( childIndex );
							}
							FI

							shader::groupMemoryBarrierWithGroupSync( writer );

							IF( writer, groupIndex == 0_u )
							{
								gsParentIndex = popNode();
							}
							FI

							shader::groupMemoryBarrierWithGroupSync( writer );
						}
						ELIHWOD

						shader::groupMemoryBarrierWithGroupSync( writer );
					}
					else
					{
						// Intersect point lights against AABB.
						FOR( writer, sdw::UInt, i, groupIndex, i < c3d_clustersData.pointLightCount(), i += NumThreads )
						{
							processPointLightAABB( i );
						}
						ROF

						// Intersect spot lights against AABB.
						FOR( writer, sdw::UInt, i, groupIndex, i < c3d_clustersData.spotLightCount(), i += NumThreads )
						{
							processSpotLightAABB( i );
						}
						ROF

						shader::groupMemoryBarrierWithGroupSync( writer );
					}

					// Now update the global light grids with the light lists and light counts.
					IF( writer, groupIndex == 0u )
					{
						IF( writer, gsPointLights.getCount() > 0_u )
						{
							gsPointLights.getCount() = min( sdw::UInt{ MaxLightsPerCluster }, gsPointLights.getCount() );
							gsPointLightStartOffset = sdw::atomicAdd( c3d_pointLightClusterListCount, gsPointLights.getCount() );
							c3d_pointLightClusterGrid[gsClusterIndex1D] = sdw::uvec2( gsPointLightStartOffset, gsPointLights.getCount() );
						}
						FI

						IF( writer, gsSpotLights.getCount() > 0_u )
						{
							gsSpotLights.getCount() = min( sdw::UInt{ MaxLightsPerCluster }, gsSpotLights.getCount() );
							gsSpotLightStartOffset = sdw::atomicAdd( c3d_spotLightClusterListCount, gsSpotLights.getCount() );
							c3d_spotLightClusterGrid[gsClusterIndex1D] = sdw::uvec2( gsSpotLightStartOffset, gsSpotLights.getCount() );
						}
						FI
					}
					FI

					shader::groupMemoryBarrierWithGroupSync( writer );

					// Now update the global light index lists with the group shared light lists.
					FOR( writer, sdw::UInt, i, groupIndex, i < gsPointLights.getCount(), i += NumThreads )
					{
						c3d_pointLightClusterIndex[gsPointLightStartOffset + i] = gsPointLights[i];
					}
					ROF

					FOR( writer, sdw::UInt, i, groupIndex, i < gsSpotLights.getCount(), i += NumThreads )
					{
						c3d_spotLightClusterIndex[gsSpotLightStartOffset + i] = gsSpotLights[i];
					}
					ROF
				} );
			return writer.getBuilder().releaseShader();
		}

		class FramePassNoDepth
			: public crg::ComputePass
		{
		public:
			FramePassNoDepth( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, FrustumClusters const & clusters
				, crg::cp::Config config )
				: crg::ComputePass{framePass
					, context
					, graph
					, crg::ru::Config{ 12u }
					, config
						.isEnabled( IsEnabledCallback( [this, &clusters]() { return doIsEnabled( clusters ); } ) )
						.getPassIndex( RunnablePass::GetPassIndexCallback( [this, &clusters](){ return doGetPassIndex( clusters ); } ) )
						.programCreator( { 12u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } )
						.end( RecordCallback{ [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t idx ) { doPostRecord( ctx, cb, idx ); } } ) }
				, m_device{ device }
				, m_config{ clusters.getConfig() }
			{
			}

		private:
			struct ProgramData
			{
				ProgramData() = default;
				ShaderModule shaderModule{};
				ashes::PipelineShaderStageCreateInfoArray stages{};
			};

		private:
			uint32_t doGetPassIndex( FrustumClusters const & clusters )const
			{
				u32 result = {};

				if ( m_config.useSpotBoundingCone )
				{
					result += 6u;
				}

				if ( m_config.useLightsBVH )
				{
					result += 3u;
				}

				if ( m_config.sortLights )
				{
					auto const & lightCache = clusters.getCamera().getScene()->getLightCache();

					auto pointLightsCount = lightCache.getLightsBufferCount( LightType::ePoint );
					auto spoLightsCount = lightCache.getLightsBufferCount( LightType::eSpot );
					auto totalValues = std::max( pointLightsCount, spoLightsCount );

					if ( auto numChunks = getLightsMortonCodeChunkCount( totalValues );
						numChunks > 1u )
					{
						result += ( ( numChunks - 1u ) % 2u );
					}

					result += 1u;
				}

				return result;
			}

			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( uint32_t passIndex )
			{
				auto [it, res] = m_programs.try_emplace( passIndex );

				if ( res )
				{
					auto & program = it->second;
					program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "AssignLightsToClusters" ), dspclst::createShader( m_device, m_config ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, program.shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

			bool doIsEnabled( FrustumClusters const & clusters )const
			{
				return !m_config.parseDepthBuffer
					&& clusters.getCamera().getScene()->getLightCache().hasClusteredLights();
			}

			void doPostRecord( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )const
			{
				for ( auto & attach : m_pass.buffers )
				{
					if ( !attach.isNoTransition()
						&& attach.isStorageBuffer()
						&& attach.isClearableBuffer() )
					{
						auto currentState = context.getAccessState( attach.buffer( index )
							, attach.getBufferRange() );
						context.memoryBarrier( commandBuffer
							, attach.buffer( index )
							, attach.getBufferRange()
							, currentState.access
							, currentState.pipelineStage
							, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
					}
				}
			}

		private:
			RenderDevice const & m_device;
			ClustersConfig const & m_config;
			castor::Map< uint32_t, ProgramData > m_programs;
		};

		class FramePassDepth
			: public crg::ComputePass
		{
		public:
			FramePassDepth( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, FrustumClusters const & clusters
				, crg::cp::Config config )
				: crg::ComputePass{ framePass
					, context
					, graph
					, crg::ru::Config{ 12u }
					, config
						.isEnabled( IsEnabledCallback( [this, &clusters]() { return doIsEnabled( clusters ); } ) )
						.getPassIndex( RunnablePass::GetPassIndexCallback( [this, &clusters](){ return doGetPassIndex( clusters ); } ) )
						.programCreator( { 12u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } )
						.recordInto( RunnablePass::RecordCallback( [this, &clusters]( crg::RecordContext & ctx, VkCommandBuffer cmd, uint32_t ){ doSubRecordInto( ctx, cmd, clusters ); } ) )
						.end( RecordCallback{ [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t idx ) { doPostRecord( ctx, cb, idx ); } } ) }
				, m_device{ device }
				, m_config{ clusters.getConfig() }
			{
			}

		private:
			struct ProgramData
			{
				ProgramData() = default;
				ShaderModule shaderModule;
				ashes::PipelineShaderStageCreateInfoArray stages;
			};

		private:
			uint32_t doGetPassIndex( FrustumClusters const & clusters )const
			{
				u32 result = {};

				if ( m_config.useSpotBoundingCone )
				{
					result += 6u;
				}

				if ( m_config.useLightsBVH )
				{
					result += 3u;
				}

				if ( m_config.sortLights )
				{
					auto const & lightCache = clusters.getCamera().getScene()->getLightCache();

					auto pointLightsCount = lightCache.getLightsBufferCount( LightType::ePoint );
					auto spoLightsCount = lightCache.getLightsBufferCount( LightType::eSpot );
					auto totalValues = std::max( pointLightsCount, spoLightsCount );

					if ( auto numChunks = getLightsMortonCodeChunkCount( totalValues );
						numChunks > 1u )
					{
						result += ( ( numChunks - 1u ) % 2u );
					}

					result += 1u;
				}

				return result;
			}

			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( uint32_t passIndex )
			{
				auto [it, res] = m_programs.try_emplace( passIndex );

				if ( res )
				{
					auto & program = it->second;
					program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "AssignLightsToClusters" ), dspclst::createShader( m_device, m_config ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, program.shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

			bool doIsEnabled( FrustumClusters const & clusters )const
			{
				return m_config.parseDepthBuffer
					&& ( clusters.getCamera().getScene()->getLightCache().hasClusteredLights()
						|| clusters.needsLightsUpdate() );
			}

			void doSubRecordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, FrustumClusters const & clusters )const
			{
				auto const & buffer = clusters.getClustersIndirectBuffer();
				context.memoryBarrier( commandBuffer
					, VkBuffer( buffer )
					, crg::BufferSubresourceRange{ 0u, ashes::WholeSize }
					, VkAccessFlags( VK_ACCESS_INDIRECT_COMMAND_READ_BIT )
					, VkPipelineStageFlags( VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT )
					, crg::AccessState{ VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT } );
			}

			void doPostRecord( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )const
			{
				for ( auto const & attach : m_pass.buffers )
				{
					if ( !attach.isNoTransition()
						&& attach.isStorageBuffer()
						&& attach.isClearableBuffer() )
					{
						auto currentState = context.getAccessState( attach.buffer( index )
							, attach.getBufferRange() );
						context.memoryBarrier( commandBuffer
							, attach.buffer( index )
							, attach.getBufferRange()
							, currentState.access
							, currentState.pipelineStage
							, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
					}
				}
			}

		private:
			RenderDevice const & m_device;
			ClustersConfig const & m_config;
			castor::Map< uint32_t, ProgramData > m_programs;
		};
	}

	//*********************************************************************************************

	crg::FramePass const & createAssignLightsToClustersPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, RenderDevice const & device
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, FrustumClusters & clusters )
	{
		auto const & lights = clusters.getCamera().getScene()->getLightCache();

		auto & passNoDepth = graph.createPass( "AssignLightsToClustersNoDepth"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< dspclst::FramePassNoDepth >( framePass
					, context
					, graph
					, device
					, clusters
					, crg::cp::Config{}
						.groupCountX( clusters.getDimensions()->x )
						.groupCountY( clusters.getDimensions()->y )
						.groupCountZ( clusters.getDimensions()->z ) );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		passNoDepth.addDependencies( previousPasses );
		mainCameraUbo.createPassBinding( passNoDepth, dspclst::eMainCamera );
		clustersCameraUbo.createPassBinding( passNoDepth, dspclst::eClustersCamera );
		lights.createPassBinding( passNoDepth, dspclst::eLights );
		clusters.getClustersUbo().createPassBinding( passNoDepth, dspclst::eClusters );
		createInputStoragePassBinding( passNoDepth, uint32_t( dspclst::eAllLightsAABB ), cuT( "C3D_AllLightsAABB" ), clusters.getAllLightsAABBBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( passNoDepth, uint32_t( dspclst::eClustersAABB ), cuT( "C3D_ClustersAABB" ), clusters.getClustersAABBBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( passNoDepth, uint32_t( dspclst::ePointLightIndex ), cuT( "C3D_PointLightClusterIndex" ), clusters.getPointLightClusterIndexBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( passNoDepth, uint32_t( dspclst::ePointLightCluster ), cuT( "C3D_PointLightClusterGrid" ), clusters.getPointLightClusterGridBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( passNoDepth, uint32_t( dspclst::eSpotLightIndex ), cuT( "C3D_SpotLightClusterIndex" ), clusters.getSpotLightClusterIndexBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( passNoDepth, uint32_t( dspclst::eSpotLightCluster ), cuT( "C3D_SpotLightClusterGrid" ), clusters.getSpotLightClusterGridBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( passNoDepth, uint32_t( dspclst::ePointLightBVH ), cuT( "C3D_PointLightsBVH" ), clusters.getPointLightBVHBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( passNoDepth, uint32_t( dspclst::eSpotLightBVH ), cuT( "C3D_SpotLightsBVH" ), clusters.getSpotLightBVHBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( passNoDepth, uint32_t( dspclst::ePointLightIndices ), cuT( "C3D_PointLightIndices" )
			, { &clusters.getOutputPointLightIndicesBuffer(), &clusters.getInputPointLightIndicesBuffer(), &clusters.getOutputPointLightIndicesBuffer()
				, &clusters.getOutputPointLightIndicesBuffer(), &clusters.getInputPointLightIndicesBuffer(), &clusters.getOutputPointLightIndicesBuffer()
				, &clusters.getOutputPointLightIndicesBuffer(), &clusters.getInputPointLightIndicesBuffer(), &clusters.getOutputPointLightIndicesBuffer()
				, &clusters.getOutputPointLightIndicesBuffer(), &clusters.getInputPointLightIndicesBuffer(), &clusters.getOutputPointLightIndicesBuffer() }
			, 0u, ashes::WholeSize );
		createInputStoragePassBinding( passNoDepth, uint32_t( dspclst::eSpotLightIndices ), cuT( "C3D_SpotLightIndices" )
			, { &clusters.getOutputSpotLightIndicesBuffer(), &clusters.getInputSpotLightIndicesBuffer(), &clusters.getOutputSpotLightIndicesBuffer()
				, &clusters.getOutputSpotLightIndicesBuffer(), &clusters.getInputSpotLightIndicesBuffer(), &clusters.getOutputSpotLightIndicesBuffer()
				, &clusters.getOutputSpotLightIndicesBuffer(), &clusters.getInputSpotLightIndicesBuffer(), &clusters.getOutputSpotLightIndicesBuffer()
				, &clusters.getOutputSpotLightIndicesBuffer(), &clusters.getInputSpotLightIndicesBuffer(), &clusters.getOutputSpotLightIndicesBuffer() }
			, 0u, ashes::WholeSize );

		auto & passDepth = graph.createPass( "AssignLightsToClustersDepth"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< dspclst::FramePassDepth >( framePass
					, context
					, graph
					, device
					, clusters
					, crg::cp::Config{}
						.indirectBuffer( crg::IndirectBuffer{ { clusters.getClustersIndirectBuffer(), "C3D_ClustersIndirect" }, uint32_t( sizeof( VkDispatchIndirectCommand ) ) } ) );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		passDepth.addDependency( passNoDepth );
		mainCameraUbo.createPassBinding( passDepth, dspclst::eMainCamera );
		clustersCameraUbo.createPassBinding( passDepth, dspclst::eClustersCamera );
		lights.createPassBinding( passDepth, dspclst::eLights );
		clusters.getClustersUbo().createPassBinding( passDepth, dspclst::eClusters );
		createInputStoragePassBinding( passDepth, uint32_t( dspclst::eAllLightsAABB ), cuT( "C3D_AllLightsAABB" ), clusters.getAllLightsAABBBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( passDepth, uint32_t( dspclst::eClustersAABB ), cuT( "C3D_ClustersAABB" ), clusters.getClustersAABBBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( passDepth, uint32_t( dspclst::ePointLightIndex ), cuT( "C3D_PointLightClusterIndex" ), clusters.getPointLightClusterIndexBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( passDepth, uint32_t( dspclst::ePointLightCluster ), cuT( "C3D_PointLightClusterGrid" ), clusters.getPointLightClusterGridBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( passDepth, uint32_t( dspclst::eSpotLightIndex ), cuT( "C3D_SpotLightClusterIndex" ), clusters.getSpotLightClusterIndexBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( passDepth, uint32_t( dspclst::eSpotLightCluster ), cuT( "C3D_SpotLightClusterGrid" ), clusters.getSpotLightClusterGridBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( passDepth, uint32_t( dspclst::ePointLightBVH ), cuT( "C3D_PointLightsBVH" ), clusters.getPointLightBVHBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( passDepth, uint32_t( dspclst::eSpotLightBVH ), cuT( "C3D_SpotLightsBVH" ), clusters.getSpotLightBVHBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( passDepth, uint32_t( dspclst::ePointLightIndices ), cuT( "C3D_PointLightIndices" )
			, { &clusters.getOutputPointLightIndicesBuffer(), &clusters.getInputPointLightIndicesBuffer(), &clusters.getOutputPointLightIndicesBuffer()
				, &clusters.getOutputPointLightIndicesBuffer(), &clusters.getInputPointLightIndicesBuffer(), &clusters.getOutputPointLightIndicesBuffer()
				, &clusters.getOutputPointLightIndicesBuffer(), &clusters.getInputPointLightIndicesBuffer(), &clusters.getOutputPointLightIndicesBuffer()
				, &clusters.getOutputPointLightIndicesBuffer(), &clusters.getInputPointLightIndicesBuffer(), &clusters.getOutputPointLightIndicesBuffer() }
			, 0u, ashes::WholeSize );
		createInputStoragePassBinding( passDepth, uint32_t( dspclst::eSpotLightIndices ), cuT( "C3D_SpotLightIndices" )
			, { &clusters.getOutputSpotLightIndicesBuffer(), &clusters.getInputSpotLightIndicesBuffer(), &clusters.getOutputSpotLightIndicesBuffer()
				, &clusters.getOutputSpotLightIndicesBuffer(), &clusters.getInputSpotLightIndicesBuffer(), &clusters.getOutputSpotLightIndicesBuffer()
				, &clusters.getOutputSpotLightIndicesBuffer(), &clusters.getInputSpotLightIndicesBuffer(), &clusters.getOutputSpotLightIndicesBuffer()
				, &clusters.getOutputSpotLightIndicesBuffer(), &clusters.getInputSpotLightIndicesBuffer(), &clusters.getOutputSpotLightIndicesBuffer() }
			, 0u, ashes::WholeSize );
		createInputStoragePassBinding( passDepth, uint32_t( dspclst::eUniqueClusters ), cuT( "C3D_UniqueClusters" ), clusters.getUniqueClustersBuffer(), 0u, ashes::WholeSize );

		return passDepth;
	}

	//*********************************************************************************************
}
