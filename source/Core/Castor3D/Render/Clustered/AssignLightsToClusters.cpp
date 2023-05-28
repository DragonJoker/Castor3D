#include "Castor3D/Render/Clustered/AssignLightsToClusters.hpp"

#include "Castor3D/DebugDefines.hpp"
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

	namespace dspclst
	{
		enum BindingPoints
		{
			eCamera,
			eLights,
			eClusters,
			eClustersAABB,
			ePointLightIndex,
			ePointLightCluster,
			eSpotLightIndex,
			eSpotLightCluster,
			ePointLightBVH,
			eSpotLightBVH,
			ePointLightIndices,
			eSpotLightIndices,
		};

#if C3D_DebugUseLightsBVH
		static uint32_t constexpr NumThreads = 32u;
#else
		static uint32_t constexpr NumThreads = MaxLightsPerCluster;
#endif

		static ShaderPtr createShader( bool useLightBVH )
		{
			sdw::ComputeWriter writer;

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
				, useLightBVH );
			C3D_SpotLightBVHEx( writer
				, eSpotLightBVH
				, 0u
				, useLightBVH );
			C3D_PointLightIndicesEx( writer
				, ePointLightIndices
				, 0u
				, useLightBVH );
			C3D_SpotLightIndicesEx( writer
				, eSpotLightIndices
				, 0u
				, useLightBVH );

			// Using a stack of node IDs to traverse the BVH was inspired by:
			// Source: https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-ii-tree-traversal-gpu/
			// Author: Tero Karras (NVIDIA)
			// Retrieved: September 13, 2016
			auto gsNodeStack = writer.declSharedVariable< sdw::UInt >( "gsNodeStack", 1024u, useLightBVH );	// This should be enough to push 32 layers of nodes (32 nodes per layer).
			auto gsStackPtr = writer.declSharedVariable< sdw::Int >( "gsStackPtr", useLightBVH );			// The current index in the node stack.
			auto gsParentIndex = writer.declSharedVariable< sdw::UInt >( "gsParentIndex", useLightBVH );	// The index of the parent node in the BVH that is currently being processed.

			auto gsPointLightStartOffset = writer.declSharedVariable< sdw::UInt >( "gsPointLightStartOffset" );
			auto gsPointLights = writer.declSharedVariable< shader::AppendArrayT< sdw::UInt > >( "gsPointLights"
				, true, "U32", MaxLightsPerCluster );

			auto gsSpotLightStartOffset = writer.declSharedVariable< sdw::UInt >( "gsSpotLightStartOffset" );
			auto gsSpotLights = writer.declSharedVariable< shader::AppendArrayT< sdw::UInt > >( "gsSpotLights"
				, true, "U32", MaxLightsPerCluster );

			auto gsClusterIndex3D = writer.declSharedVariable< sdw::U32Vec3 >( "gsClusterIndex3D" );
			auto gsClusterIndex1D = writer.declSharedVariable< sdw::UInt32 >( "gsClusterIndex1D" );
			auto gsClusterAABB = writer.declSharedVariable< shader::AABB >( "gsClusterAABB" );
			auto gsClusterSphere = writer.declSharedVariable< sdw::Vec4 >( "gsClusterSphere" );

			shader::Utils utils{ writer };
			sdw::Function< sdw::Void, sdw::InUInt > pushNode;
			sdw::Function< sdw::UInt > popNode;
			sdw::Function< sdw::Boolean, shader::InAABB, shader::InAABB > aabbIntersectAABB;

			std::function< sdw::UInt( sdw::UInt, sdw::UInt ) > getFirstChild;
			std::function< sdw::Boolean( sdw::UInt, sdw::UInt ) > isLeafNode;
			std::function< sdw::UInt( sdw::UInt, sdw::UInt ) > getLeafIndex;

			if ( useLightBVH )
			{
				pushNode = writer.implementFunction< sdw::Void >( "pushNode"
					, [&]( sdw::UInt const & nodeIndex )
					{
						auto stackPtr = writer.declLocale( "stackPtr"
							, sdw::atomicAdd( gsStackPtr, 1_i ) );

						IF( writer, stackPtr < 1024 )
						{
							gsNodeStack[stackPtr] = nodeIndex;
						}
						FI;
					}
					, sdw::InUInt{ writer, "nodeIndex" } );

				popNode = writer.implementFunction< sdw::UInt >( "popNode"
					, [&]()
					{
						auto nodeIndex = writer.declLocale( "nodeIndex"
							, 0_u );
						auto stackPtr = writer.declLocale( "stackPtr"
							, sdw::atomicAdd( gsStackPtr, -1_i ) );

						IF( writer, stackPtr > 0 && stackPtr < 1024 )
						{
							nodeIndex = gsNodeStack[stackPtr - 1];
						}
						FI;

						writer.returnStmt( nodeIndex );
					} );

				// Check to see if on AABB intersects another AABB.
				// Source: Real-time collision detection, Christer Ericson (2005)
				aabbIntersectAABB = writer.implementFunction< sdw::Boolean >( "aabbIntersectAABB"
					, [&]( shader::AABB const & a
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

				// Get the index of the the first child node in the BVH.
				getFirstChild = [&]( sdw::UInt const parentIndex
					, sdw::UInt const numLevels )
				{
					return writer.ternary( numLevels > 0_u
						, parentIndex * 32_u + 1_u
						, 0_u );
				};

				// Check to see if an index of the BVH is a leaf.
				isLeafNode = [&]( sdw::UInt const childIndex
					, sdw::UInt const numLevels )
				{
					return writer.ternary( numLevels > 0_u
						, childIndex > ( c3d_numChildNodes[numLevels - 1_u] - 1_u )
						, 1_b );
				};

				// Get the index of a leaf node given the node ID in the BVH.
				getLeafIndex = [&]( sdw::UInt const nodeIndex
					, sdw::UInt const numLevels )
				{
					return writer.ternary( numLevels > 0_u
						, nodeIndex - c3d_numChildNodes[numLevels - 1_u]
						, nodeIndex );
				};
			}

			auto sphereInsideAABB = writer.implementFunction< sdw::Boolean >( "sphereInsideAABB"
				, [&]( sdw::Vec3 const & sphereCenter
					, sdw::Float const & sphereRadius
					, shader::AABB const & aabb )
				{
					auto sqDistance = writer.declLocale( "sqDistance"
						, 0.0_f );
					auto v = writer.declLocale( "v"
						, 0.0_f );

					for ( int i = 0; i < 3; ++i )
					{
						v = sphereCenter[i];

						IF( writer, v < aabb.min()[i] )
						{
							sqDistance += pow( aabb.min()[i] - v, 2.0_f );
						}
						FI;
						IF( writer, v > aabb.max()[i] )
						{
							sqDistance += pow( v - aabb.max()[i], 2.0_f );
						}
						FI;
					}

					writer.returnStmt( sqDistance <= sphereRadius * sphereRadius );
				}
				, sdw::InVec3{ writer, "sphereCenter" }
				, sdw::InFloat{ writer, "sphereRadius" }
				, shader::InAABB{ writer, "aabb" } );

			auto coneInsideSphere = writer.implementFunction< sdw::Boolean >( "coneInsideSphere"
				, [&]( shader::Cone const cone
					, sdw::Vec4 const & sphere )
				{
					auto V = writer.declLocale( "V"
						, sphere.xyz() - cone.origin() );
					auto lenSqV = writer.declLocale( "lenSqV"
						, dot( V, V ) );
					auto lenV1 = writer.declLocale( "lenV1"
						, dot( V, cone.direction() ) );
					auto distanceClosestPoint = writer.declLocale( "distanceClosestPoint"
						, cos( 2.0f * cone.aperture() ) * sqrt( lenSqV - lenV1 * lenV1 ) - lenV1 * sin( 2.0f * cone.aperture() ) );

					auto angleCull = distanceClosestPoint > sphere.w();
					auto frontCull = lenV1 > sphere.w() + cone.size();
					auto backCull = lenV1 < -sphere.w();

					writer.returnStmt( !( angleCull || frontCull || backCull ) );
				}
				, shader::InCone{ writer, "cone" }
				, sdw::InVec4{ writer, "sphere" } );

			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&]( sdw::ComputeIn in )
				{
					auto groupIndex = in.localInvocationIndex;

					IF( writer, groupIndex == 0_u )
					{
						gsPointLights.resetCount();
						gsSpotLights.resetCount();
						gsStackPtr = 0_i;
						gsParentIndex = 0_u;

						gsClusterIndex1D = in.workGroupID.x();
						gsClusterIndex3D = c3d_clustersData.computeClusterIndex3D( gsClusterIndex1D );
						gsClusterAABB = c3D_clustersAABB[gsClusterIndex1D];
						auto aabbCenter = writer.declLocale( "aabbCenter"
							, gsClusterAABB.min().xyz() + ( gsClusterAABB.max().xyz() - gsClusterAABB.min().xyz() ) / 2.0_f );
						gsClusterSphere = vec4( aabbCenter, distance( gsClusterAABB.max().xyz(), aabbCenter ) );

						if ( useLightBVH )
						{
							pushNode( 0_u );
						}
					}
					FI;

					shader::groupMemoryBarrierWithGroupSync( writer );

					if ( useLightBVH )
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
									auto lightOffset = writer.declLocale( "lightOffset"
										, c3d_pointLightIndices[leafIndex] );
									auto point = writer.declLocale( "point"
										, lights.getPointLight( lightOffset ) );

									IF( writer, sphereInsideAABB( c3d_cameraData.worldToCurView( vec4( point.position(), 1.0_f ) ).xyz()
										, point.base().farPlane()
										, gsClusterAABB ) )
									{
										gsPointLights.appendData( lightOffset, MaxLightsPerCluster );
									}
									FI;
								}
								FI;
							}
							ELSEIF( aabbIntersectAABB( gsClusterAABB, c3d_pointLightBVH[childIndex] ) )
							{
								pushNode( childIndex );
							}
							FI;

							shader::groupMemoryBarrierWithGroupSync( writer );

							IF( writer, groupIndex == 0_u )
							{
								gsParentIndex = popNode();
							}
							FI;

							shader::groupMemoryBarrierWithGroupSync( writer );
						}
						ELIHWOD;

						shader::groupMemoryBarrierWithGroupSync( writer );

						// Reset the stack.
						IF( writer, groupIndex == 0_u )
						{
							gsStackPtr = 0_i;
							gsParentIndex = 0_u;

							// Push the root node (at index 0) on the node stack.
							pushNode( 0_u );
						}
						FI;

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
									auto lightOffset = writer.declLocale( "lightOffset"
										, c3d_spotLightIndices[leafIndex] );
									auto spot = writer.declLocale( "spot"
										, lights.getSpotLight( lightOffset ) );

#if 1
									IF( writer, sphereInsideAABB( c3d_cameraData.worldToCurView( vec4( spot.position(), 1.0_f ) ).xyz()
										, spot.base().farPlane()
										, gsClusterAABB ) )
#else
									IF( writer, coneInsideSphere( shader::Cone{ c3d_cameraData.worldToCurView( vec4( spot.position(), 1.0_f ) ).xyz()
											, spot.direction()
											, spot.base().farPlane()
											, spot.outerCutOff()
											, spot.outerCutOffCos()
											, spot.outerCutOffSin() }
										, gsClusterSphere ) )
#endif
									{
										gsSpotLights.appendData( lightOffset, MaxLightsPerCluster );
									}
									FI;
								}
								FI;
							}
							ELSEIF( aabbIntersectAABB( gsClusterAABB, c3d_spotLightBVH[childIndex] ) )
							{
								pushNode( childIndex );
							}
							FI;

							shader::groupMemoryBarrierWithGroupSync( writer );

							IF( writer, groupIndex == 0_u )
							{
								gsParentIndex = popNode();
							}
							FI;

							shader::groupMemoryBarrierWithGroupSync( writer );
						}
						ELIHWOD;

						shader::groupMemoryBarrierWithGroupSync( writer );
					}
					else
					{
						// Intersect point lights against AABB.
						auto cur = writer.declLocale( "cur"
							, lights.getDirectionalsEnd() + groupIndex * PointLight::LightDataComponents );
						auto end = writer.declLocale( "end"
							, lights.getPointsEnd() );

						WHILE( writer, cur < lights.getPointsEnd() )
						{
							auto point = writer.declLocale( "point"
								, lights.getPointLight( cur ) );

							IF( writer, sphereInsideAABB( c3d_cameraData.worldToCurView( vec4( point.position(), 1.0_f ) ).xyz()
								, point.base().farPlane()
								, gsClusterAABB ) )
							{
								gsPointLights.appendData( cur, MaxLightsPerCluster );
							}
							FI;

							cur += PointLight::LightDataComponents * MaxLightsPerCluster;
						}
						ELIHW;

						// Intersect spot lights against AABB.
						cur = end + groupIndex * SpotLight::LightDataComponents;
						end = lights.getSpotsEnd();

						WHILE( writer, cur < end )
						{
							auto spot = writer.declLocale( "spot"
								, lights.getSpotLight( cur ) );

#if 1
							IF( writer, sphereInsideAABB( c3d_cameraData.worldToCurView( vec4( spot.position(), 1.0_f ) ).xyz()
								, spot.base().farPlane()
								, gsClusterAABB ) )
#else
							IF( writer, coneInsideSphere( shader::Cone{ c3d_cameraData.worldToCurView( vec4( spot.position(), 1.0_f ) ).xyz()
									, spot.direction()
									, spot.base().farPlane()
									, spot.outerCutOff()
									, spot.outerCutOffCos()
									, spot.outerCutOffSin() }
								, gsClusterSphere ) )
#endif
							{
								gsSpotLights.appendData( cur, MaxLightsPerCluster );
							}
							FI;

							cur += SpotLight::LightDataComponents * MaxLightsPerCluster;
						}
						ELIHW;

						shader::groupMemoryBarrierWithGroupSync( writer );
					}

					// Now update the global light grids with the light lists and light counts.
					IF( writer, groupIndex == 0u )
					{
						gsPointLights.getCount() = min( sdw::UInt{ MaxLightsPerCluster }, gsPointLights.getCount() );
						gsPointLightStartOffset = sdw::atomicAdd( c3d_pointLightClusterListCount, gsPointLights.getCount() );
						c3d_pointLightClusterGrid[gsClusterIndex1D] = sdw::uvec2( gsPointLightStartOffset, gsPointLights.getCount() );

						gsSpotLights.getCount() = min( sdw::UInt{ MaxLightsPerCluster }, gsSpotLights.getCount() );
						gsSpotLightStartOffset = sdw::atomicAdd( c3d_spotLightClusterListCount, gsSpotLights.getCount() );
						c3d_spotLightClusterGrid[gsClusterIndex1D] = sdw::uvec2( gsSpotLightStartOffset, gsSpotLights.getCount() );
					}
					FI;

					shader::groupMemoryBarrierWithGroupSync( writer );

					// Now update the global light index lists with the group shared light lists.
					FOR( writer, sdw::UInt, i, groupIndex, i < gsPointLights.getCount(), i += NumThreads )
					{
						c3d_pointLightClusterIndex[gsPointLightStartOffset + i] = gsPointLights[i];
					}
					ROF;

					FOR( writer, sdw::UInt, i, groupIndex, i < gsSpotLights.getCount(), i += NumThreads )
					{
						c3d_spotLightClusterIndex[gsSpotLightStartOffset + i] = gsSpotLights[i];
					}
					ROF;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		class FramePass
			: private castor::DataHolderT< ShaderModule >
			, private castor::DataHolderT< ashes::PipelineShaderStageCreateInfoArray >
			, private castor::DataHolderT< bool >
			, public crg::ComputePass
		{
			using ShaderHolder = DataHolderT< ShaderModule >;
			using CreateInfoHolder = DataHolderT< ashes::PipelineShaderStageCreateInfoArray >;
			using EnabledHolder = DataHolderT< bool >;

		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, FrustumClusters const & clusters
				, crg::cp::Config config )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, "AssignLightsToClusters", dspclst::createShader( C3D_DebugUseLightsBVH != 0 ) } }
				, CreateInfoHolder{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, ShaderHolder::getData() ) } }
				, EnabledHolder{ true }
				, crg::ComputePass{framePass
					, context
					, graph
#if C3D_DebugUseLightsBVH && C3D_DebugSortLightsMortonCode
					, crg::ru::Config{ 2u }
#else
					, crg::ru::Config{ 1u }
#endif
					, config
						.getPassIndex( RunnablePass::GetPassIndexCallback( [this](){ return doGetPassIndex(); } ) )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( CreateInfoHolder::getData() ) )
						.enabled( &clusters.needsLightsUpdate() )
						.end( RecordCallback{ [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t idx ) { doPostRecord( ctx, cb, idx ); } } ) }
				, m_lightCache{ clusters.getCamera().getScene()->getLightCache() }
			{
			}

		private:
			uint32_t doGetPassIndex()
			{
#if C3D_DebugUseLightsBVH && C3D_DebugSortLightsMortonCode
				u32 result = {};

				auto pointLightsCount = m_lightCache.getLightsCount( LightType::ePoint );
				auto spoLightsCount = m_lightCache.getLightsCount( LightType::eSpot );
				auto totalValues = std::max( pointLightsCount, spoLightsCount );
				auto numChunks = getLightsMortonCodeChunkCount( totalValues );

				if ( numChunks > 1u )
				{
					result = ( ( numChunks - 1u ) % 2u );
				}

				return result;
#else
				return 0u;
#endif
			}

			void doPostRecord( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				for ( auto & attach : m_pass.buffers )
				{
					auto buffer = attach.buffer;

					if ( !attach.isNoTransition()
						&& attach.isStorageBuffer()
						&& attach.isClearableBuffer() )
					{
						auto currentState = context.getAccessState( buffer.buffer.buffer( index )
							, buffer.range );
						context.memoryBarrier( commandBuffer
							, buffer.buffer.buffer( index )
							, buffer.range
							, currentState.access
							, currentState.pipelineStage
							, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
					}
				}
			}

		private:
			LightCache const & m_lightCache;
		};
	}

	//*********************************************************************************************

	crg::FramePass const & createAssignLightsToClustersPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, FrustumClusters & clusters )
	{
		auto & pass = graph.createPass( "AssignLightsToClusters"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< dspclst::FramePass >( framePass
					, context
					, graph
					, device
					, clusters
					, crg::cp::Config{}
						.groupCountX( clusters.getDimensions()->x * clusters.getDimensions()->y * clusters.getDimensions()->z ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *previousPass );
		cameraUbo.createPassBinding( pass, dspclst::eCamera );
		auto & lights = clusters.getCamera().getScene()->getLightCache();
		lights.createPassBinding( pass, dspclst::eLights );
		clusters.getClustersUbo().createPassBinding( pass, dspclst::eClusters );
		createInputStoragePassBinding( pass, uint32_t( dspclst::eClustersAABB ), "C3D_ClustersAABB", clusters.getClustersAABBBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( dspclst::ePointLightIndex ), "C3D_PointLightClusterIndex", clusters.getPointLightClusterIndexBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( dspclst::ePointLightCluster ), "C3D_PointLightClusterGrid", clusters.getPointLightClusterGridBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( dspclst::eSpotLightIndex ), "C3D_SpotLightClusterIndex", clusters.getSpotLightClusterIndexBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( dspclst::eSpotLightCluster ), "C3D_SpotLightClusterGrid", clusters.getSpotLightClusterGridBuffer( ), 0u, ashes::WholeSize );
#if C3D_DebugUseLightsBVH
		createInputStoragePassBinding( pass, uint32_t( dspclst::ePointLightBVH ), "C3D_PointLightsBVH", clusters.getPointLightBVHBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( pass, uint32_t( dspclst::eSpotLightBVH ), "C3D_SpotLightsBVH", clusters.getSpotLightBVHBuffer(), 0u, ashes::WholeSize );
#	if C3D_DebugSortLightsMortonCode
		createInputStoragePassBinding( pass, uint32_t( dspclst::ePointLightIndices ), "C3D_PointLightIndices", { &clusters.getOutputPointLightIndicesBuffer(), &clusters.getInputPointLightIndicesBuffer() }, 0u, ashes::WholeSize );
		createInputStoragePassBinding( pass, uint32_t( dspclst::eSpotLightIndices ), "C3D_SpotLightIndices", { &clusters.getOutputSpotLightIndicesBuffer(), &clusters.getInputSpotLightIndicesBuffer() }, 0u, ashes::WholeSize );
#	else
		createInputStoragePassBinding( pass, uint32_t( dspclst::ePointLightIndices ), "C3D_PointLightIndices", clusters.getInputPointLightIndicesBuffer(), 0u, ashes::WholeSize );
		createInputStoragePassBinding( pass, uint32_t( dspclst::eSpotLightIndices ), "C3D_SpotLightIndices", clusters.getInputSpotLightIndicesBuffer(), 0u, ashes::WholeSize );
#	endif
#endif
		return pass;
	}

	//*********************************************************************************************
}
