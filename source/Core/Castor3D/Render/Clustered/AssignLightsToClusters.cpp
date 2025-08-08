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

namespace c3d
{
	//*********************************************************************************************

	namespace dspclst
	{
		enum BindingPoints
		{
			eCamera,
			eLights,
			eClusters,
			eAllLightsAABB,
			eClustersAABB,
			ePointLightBVH,
			eSpotLightBVH,
			ePointLightIndices,
			eSpotLightIndices,
			ePointLightIndex,
			ePointLightCluster,
			eSpotLightIndex,
			eSpotLightCluster,
		};

		static ShaderPtr createShader( RenderDevice const & device
			, ClustersConfig const & config )
		{
			uint32_t NumThreads = 32u;

			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_numChildNodes = writer.declConstantArray< sdw::UInt >( "c3d_numChildNodes"
				, { 1_u			/* 1 level   =32^0 */
				, 33_u			/* 2 levels  +32^1 */
				, 1057_u		/* 3 levels  +32^2 */
				, 33825_u		/* 4 levels  +32^3 */
				, 1082401_u		/* 5 levels  +32^4 */
				, 34636833_u	/* 6 levels  +32^5 */ } );

			// Inputs
			C3D_Camera( writer
				, eCamera
				, 0u );
			shader::LightsBuffer lights{ writer
				, eLights
				, 0u };
			C3D_Clusters( writer
				, eClusters
				, 0u );
			C3D_AllLightsAABB( writer
				, eAllLightsAABB
				, 0u );
			C3D_ClustersAABB( writer
				, eClustersAABB
				, 0u );
			C3D_PointLightBVH( writer
				, ePointLightBVH
				, 0u );
			C3D_SpotLightBVH( writer
				, eSpotLightBVH
				, 0u );
			C3D_PointLightIndices( writer
				, ePointLightIndices
				, 0u );
			C3D_SpotLightIndices( writer
				, eSpotLightIndices
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

			static constexpr s32 MaxValues = 1024;

			// Using a stack of node IDs to traverse the BVH was inspired by:
			// Source: https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-ii-tree-traversal-gpu/
			// Author: Tero Karras (NVIDIA)
			// Retrieved: September 13, 2016
			auto gsNodeStack = writer.declSharedVariable< sdw::UInt >( "gsNodeStack", u32( MaxValues ) ); // This should be enough to push 32 layers of nodes (32 nodes per layer).
			auto gsStackPtr = writer.declSharedVariable< sdw::Int >( "gsStackPtr" ); // The current index in the node stack.
			auto gsParentIndex = writer.declSharedVariable< sdw::UInt >( "gsParentIndex" ); // The index of the parent node in the BVH that is currently being processed.

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

			Function< sdw::UInt( sdw::UInt, sdw::UInt ) > getFirstChild;
			Function< sdw::Boolean( sdw::UInt, sdw::UInt ) > isLeafNode;
			Function< sdw::UInt( sdw::UInt, sdw::UInt ) > getLeafIndex;

			pushNode = writer.implementFunction< sdw::Void >( "pushNode"
				, [&writer, &gsStackPtr, &gsNodeStack]( sdw::UInt const & nodeIndex )
				{
					auto stackPtr = writer.declLocale( "stackPtr"
						, sdw::atomicAdd( gsStackPtr, 1_i ) );

					sdwIF( writer, stackPtr < MaxValues )
					{
						gsNodeStack[stackPtr] = nodeIndex;
					}
					sdwFI
				}
				, sdw::InUInt{ writer, "nodeIndex" } );

			popNode = writer.implementFunction< sdw::UInt >( "popNode"
				, [&writer, &gsStackPtr, &gsNodeStack]()
				{
					auto nodeIndex = writer.declLocale( "nodeIndex"
						, 0_u );
					auto stackPtr = writer.declLocale( "stackPtr"
						, sdw::atomicAdd( gsStackPtr, -1_i ) );

					sdwIF( writer, stackPtr > 0 && stackPtr < MaxValues )
					{
						nodeIndex = gsNodeStack[stackPtr - 1];
					}
					sdwFI

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

						sdwIF( writer, v < aabb.min()[i] )
						{
							sqDistance += pow( aabb.min()[i] - v, 2.0_f );
						}
						sdwFI
						sdwIF( writer, v > aabb.max()[i] )
						{
							sqDistance += pow( v - aabb.max()[i], 2.0_f );
						}
						sdwFI
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
						auto lightIndex = c3d_pointLightIndices[leafIndex];
						auto aabb = writer.declLocale( "aabb"
							, c3d_allLightsAABB[lightIndex] );
						auto center = writer.declLocale( "center"
							, aabb.min().xyz() + ( aabb.max().xyz() - aabb.min().xyz() ) / 2.0_f );

						sdwIF( writer, sphereInsideAABB( vec4( center, aabb.min().w() ), gsClusterAABB ) )
						{
							gsPointLights.appendData( lightIndex, MaxLightsPerCluster );
						}
						sdwFI
					};

					auto processSpotLightAABB = [&]( sdw::UInt const & leafIndex )
					{
						auto lightIndex = c3d_spotLightIndices[leafIndex];
						auto aabb = writer.declLocale( "aabb"
							, c3d_allLightsAABB[c3d_clustersData.pointLightCount() + lightIndex] );

						sdwIF( writer, aabbIntersectAABB( aabb, gsClusterAABB ) )
						{
							auto spot = writer.declLocale( "spot"
								, lights.getSpotLight( lights.getPointsEnd() + lightIndex * SpotLightInstance::LightDataComponents ) );
							auto cone = writer.declLocale( "cone"
								, shader::Cone{ c3d_cameraData.worldToCurView( vec4( spot.position(), 1.0_f ) ).xyz()
									, c3d_cameraData.worldToCurView( -spot.direction() )
									, computeRange( spot )
									, spot.outerCutOffCos()
									, spot.outerCutOffSin()
									, spot.outerCutOffTan() } );

							sdwIF( writer, coneInsideSphere( cone, gsClusterSphere ) )
							{
								gsSpotLights.appendData( lightIndex, MaxLightsPerCluster );
							}
							sdwFI
						}
						sdwFI
					};

					sdwIF( writer, groupIndex == 0_u )
					{
						gsPointLights.resetCount();
						gsSpotLights.resetCount();
						gsStackPtr = 0_i;
						gsParentIndex = 0_u;

						gsClusterIndex1D = c3d_clustersData.computeClusterIndex1D( uvec3( in.workGroupID ) );
						gsClusterAABB = c3D_clustersAABB[gsClusterIndex1D];
						auto aabbCenter = writer.declLocale( "aabbCenter"
							, gsClusterAABB.min().xyz() + ( gsClusterAABB.max().xyz() - gsClusterAABB.min().xyz() ) / 2.0_f );
						gsClusterSphere = vec4( aabbCenter, distance( gsClusterAABB.max().xyz(), aabbCenter ) );

						pushNode( 0_u );
					}
					sdwFI

					shader::groupMemoryBarrierWithGroupSync( writer );

					auto childOffset = writer.declLocale( "childOffset", groupIndex );

					// Check point light BVH
					sdwDOWHILE( writer, gsParentIndex > 0_u )
					{
						auto childIndex = writer.declLocale( "childIndex"
							, getFirstChild( gsParentIndex, c3d_clustersData.pointLightLevels() ) + childOffset );

						sdwIF( writer, isLeafNode( childIndex, c3d_clustersData.pointLightLevels() ) )
						{
							auto leafIndex = writer.declLocale( "leafIndex"
								, getLeafIndex( childIndex, c3d_clustersData.pointLightLevels() ) );

							sdwIF( writer, leafIndex < c3d_clustersData.pointLightCount() )
							{
								processPointLightAABB( leafIndex );
							}
							sdwFI
						}
						sdwELSEIF( aabbIntersectAABB( gsClusterAABB, c3d_pointLightBVH[childIndex] ) )
						{
							pushNode( childIndex );
						}
						sdwFI

						shader::groupMemoryBarrierWithGroupSync( writer );

						sdwIF( writer, groupIndex == 0_u )
						{
							gsParentIndex = popNode();
						}
						sdwFI

						shader::groupMemoryBarrierWithGroupSync( writer );
					}
					sdwELIHWOD

					shader::groupMemoryBarrierWithGroupSync( writer );

					// Reset the stack.
					sdwIF( writer, groupIndex == 0_u )
					{
						gsStackPtr = 0_i;
						gsParentIndex = 0_u;

						// Push the root node (at index 0) on the node stack.
						pushNode( 0_u );
					}
					sdwFI

					shader::groupMemoryBarrierWithGroupSync( writer );

					// Check spot light BVH
					sdwDOWHILE( writer, gsParentIndex > 0_u )
					{
						auto childIndex = writer.declLocale( "childIndex"
							, getFirstChild( gsParentIndex, c3d_clustersData.spotLightLevels() ) + childOffset );

						sdwIF( writer, isLeafNode( childIndex, c3d_clustersData.spotLightLevels() ) )
						{
							auto leafIndex = writer.declLocale( "leafIndex"
								, getLeafIndex( childIndex, c3d_clustersData.spotLightLevels() ) );

							sdwIF( writer, leafIndex < c3d_clustersData.spotLightCount() )
							{
								processSpotLightAABB( leafIndex );
							}
							sdwFI
						}
						sdwELSEIF( aabbIntersectAABB( gsClusterAABB, c3d_spotLightBVH[childIndex] ) )
						{
							pushNode( childIndex );
						}
						sdwFI

						shader::groupMemoryBarrierWithGroupSync( writer );

						sdwIF( writer, groupIndex == 0_u )
						{
							gsParentIndex = popNode();
						}
						sdwFI

						shader::groupMemoryBarrierWithGroupSync( writer );
					}
					sdwELIHWOD

					shader::groupMemoryBarrierWithGroupSync( writer );

					// Now update the global light grids with the light lists and light counts.
					sdwIF( writer, groupIndex == 0u )
					{
						sdwIF( writer, gsPointLights.getCount() > 0_u )
						{
							gsPointLights.getCount() = min( sdw::UInt{ MaxLightsPerCluster }, gsPointLights.getCount() );
							gsPointLightStartOffset = sdw::atomicAdd( c3d_pointLightClusterListCount, gsPointLights.getCount() );
							c3d_pointLightClusterGrid[gsClusterIndex1D] = sdw::uvec2( gsPointLightStartOffset, gsPointLights.getCount() );
						}
						sdwFI

						sdwIF( writer, gsSpotLights.getCount() > 0_u )
						{
							gsSpotLights.getCount() = min( sdw::UInt{ MaxLightsPerCluster }, gsSpotLights.getCount() );
							gsSpotLightStartOffset = sdw::atomicAdd( c3d_spotLightClusterListCount, gsSpotLights.getCount() );
							c3d_spotLightClusterGrid[gsClusterIndex1D] = sdw::uvec2( gsSpotLightStartOffset, gsSpotLights.getCount() );
						}
						sdwFI
					}
					sdwFI

					shader::groupMemoryBarrierWithGroupSync( writer );

					// Now update the global light index lists with the group shared light lists.
					sdwFOR( writer, sdw::UInt, i, groupIndex, i < gsPointLights.getCount(), i += NumThreads )
					{
						c3d_pointLightClusterIndex[gsPointLightStartOffset + i] = gsPointLights[i];
					}
					sdwROF

					sdwFOR( writer, sdw::UInt, i, groupIndex, i < gsSpotLights.getCount(), i += NumThreads )
					{
						c3d_spotLightClusterIndex[gsSpotLightStartOffset + i] = gsSpotLights[i];
					}
					sdwROF
				} );
			return writer.getBuilder().releaseShader();
		}

		class FramePass
			: public crg::ComputePass
		{
		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, FrustumClusters const & clusters
				, crg::cp::Config config )
				: crg::ComputePass{framePass
					, context
					, graph
					, crg::ru::Config{ 2u }
					, config
						.isEnabled( IsEnabledCallback( [this, &clusters]() { return doIsEnabled( clusters ); } ) )
						.getPassIndex( RunnablePass::GetPassIndexCallback( [this, &clusters](){ return doGetPassIndex( clusters ); } ) )
						.programCreator( { 2u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } ) }
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

				auto const & lightCache = clusters.getCamera().getScene()->getLightCache();
				auto pointLightsCount = lightCache.getLightsBufferCount( LightType::ePoint );
				auto spoLightsCount = lightCache.getLightsBufferCount( LightType::eSpot );
				auto totalValues = std::max( pointLightsCount, spoLightsCount );
				if ( auto numChunks = getLightsMortonCodeChunkCount( totalValues );
					numChunks > 1u )
					result += ( ( numChunks - 1u ) % 2u );

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
				return clusters.getCamera().getScene()->getLightCache().hasClusteredLights();
			}

		private:
			RenderDevice const & m_device;
			ClustersConfig const & m_config;
			Map< uint32_t, ProgramData > m_programs;
		};
	}

	//*********************************************************************************************

	void createAssignLightsToClustersPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, CameraUbo const & clustersCameraUbo
		, BufferBase const & allLightsAABB
		, BufferBase const & clustersAABB
		, BufferBase const & pointLightBVH
		, BufferBase const & spotLightBVH
		, ClustersLightSortAttachs const & outputSortAttachs
		, BufferBase & pointLightClusterIndex
		, BufferBase & spotLightClusterIndex
		, BufferBase & pointLightClusterGrid
		, BufferBase & spotLightClusterGrid )
	{
		auto const & lights = clusters.getCamera().getScene()->getLightCache();

		auto & pass = graph.createPass( "AssignLightsToClusters"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = makeRawUnique< dspclst::FramePass >( framePass
					, context
					, graph
					, device
					, clusters
					, crg::cp::Config{}
						.groupCountX( clusters.getDimensions()->x )
						.groupCountY( clusters.getDimensions()->y )
						.groupCountZ( clusters.getDimensions()->z ) );
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		clustersCameraUbo.createPassBinding( pass, dspclst::eCamera );
		lights.createPassBinding( pass, dspclst::eLights );
		clusters.getClustersUbo().createPassBinding( pass, dspclst::eClusters );
		pass.addInputStorage( *allLightsAABB.getLastAttach(), uint32_t( dspclst::eAllLightsAABB ) );
		pass.addInputStorage( *clustersAABB.getLastAttach(), uint32_t( dspclst::eClustersAABB ) );
		pass.addInputStorage( *pointLightBVH.getLastAttach(), uint32_t( dspclst::ePointLightBVH ) );
		pass.addInputStorage( *spotLightBVH.getLastAttach(), uint32_t( dspclst::eSpotLightBVH ) );
		pass.addInputStorage( *outputSortAttachs.pointLightIndices, uint32_t( dspclst::ePointLightIndices ) );
		pass.addInputStorage( *outputSortAttachs.spotLightIndices, uint32_t( dspclst::eSpotLightIndices ) );

		pointLightClusterIndex.setLastAttach( pass.addClearableOutputStorageBuffer( pointLightClusterIndex.bufferViewId, uint32_t( dspclst::ePointLightIndex ) ) );
		pointLightClusterGrid.setLastAttach( pass.addClearableOutputStorageBuffer( pointLightClusterGrid.bufferViewId, uint32_t( dspclst::ePointLightCluster ) ) );
		spotLightClusterIndex.setLastAttach( pass.addClearableOutputStorageBuffer( spotLightClusterIndex.bufferViewId, uint32_t( dspclst::eSpotLightIndex ) ) );
		spotLightClusterGrid.setLastAttach( pass.addClearableOutputStorageBuffer( spotLightClusterGrid.bufferViewId, uint32_t( dspclst::eSpotLightCluster ) ) );
	}

	//*********************************************************************************************
}
