#include "Castor3D/Render/Clustered/ReduceLightsAABB.hpp"

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

#include <limits>

#define C3D_DebugEnableWarpOptimisation 0

namespace castor3d
{
	//*********************************************************************************************

	namespace rdclgb
	{
		enum BindingPoints
		{
			eCamera,
			eLights,
			eClusters,
			eLightsAABB,
		};

		static uint32_t constexpr NumThreads = 512u;
		static float constexpr FltMax = std::numeric_limits< float >::max();

		static ShaderPtr createShader( bool first )
		{
			sdw::ComputeWriter writer;

			// Inputs
			C3D_CameraEx( writer
				, eCamera
				, 0u
				, first );
			shader::LightsBuffer lights{ writer
				, eLights
				, 0u
				, first };
			C3D_ClustersEx( writer
				, eClusters
				, 0u
				, first );
			C3D_LightsAABB( writer
				, eLightsAABB
				, 0u );

			sdw::PushConstantBuffer pcb{ writer, "C3D_DispatchData", "c3d_dispatchData" };
			auto c3d_numThreadGroups = pcb.declMember< sdw::UInt >( "numThreadGroups" );
			auto c3d_reduceNumElements = pcb.declMember< sdw::UInt >( "reduceNumElements" );
			pcb.end();

			auto gsAABBMin = writer.declSharedVariable< sdw::Vec4 >( "gsAABBMin", NumThreads );
			auto gsAABBMax = writer.declSharedVariable< sdw::Vec4 >( "gsAABBMax", NumThreads );

			// Perform log-step reduction in group shared memory.
			auto logStepReduction = [&]( sdw::UInt const & groupIndex
				, sdw::UInt const & groupID )
			{
				// If we can assume that NUM_THREADS is a power of 2, we can compute
				// the reduction index by performing a bit shift. This is equivalent to 
				// halving the number of values (and threads) that must perform the reduction
				// operation.
				auto reduceIndex = NumThreads >> 1u;

#if C3D_DebugEnableWarpOptimisation
				while ( reduceIndex > 32u )
#else
				while ( reduceIndex > 1u )
#endif
				{
					IF( writer, groupIndex < reduceIndex )
					{
						gsAABBMin[groupIndex] = min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
						gsAABBMax[groupIndex] = max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );
					}
					FI;

					// Sync group shared memory writes.
					shader::groupMemoryBarrierWithGroupSync( writer );

					// Halve the number of threads that participate in the reduction.
					reduceIndex >>= 1u;
				}

#if C3D_DebugEnableWarpOptimisation
				// Within a warp (of 32 threads), instructions are warp-synchronous
				// and the GroupMemoryBarrierWithGroupSync() is no longer needed to ensure
				// the previous writes to groups shared memory have completed.
				// Source: DirectCompute Optimizations and Best Practices (2010), Eric Young.
				// Source: The CUDA Handbook (2013), Nicholas Wilt
				IF( writer, groupIndex < 32_u )
				{
					while ( writer, reduceIndex > 0u )
					{
						// To avoid out-of-bounds memory access, the number of threads in the 
						// group must be at least 2x the reduce index. For example, the 
						// thread at index 31 will access elements 31 and 63 so the size of the thread group
						// must be at least 64.
						if ( NumThreads >= ( reduceIndex << 1u ) )
						{
							gsAABBMin[groupIndex] = min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
							gsAABBMax[groupIndex] = max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );
						}

						reduceIndex >>= 1u;
					}

					IF( writer, groupIndex == 0_u )
					{
						c3d_lightsAABB[groupID].min() = gsAABBMin[groupIndex];
						c3d_lightsAABB[groupID].max() = gsAABBMax[groupIndex];
					}
					FI;
				}
				FI;
#else
				IF( writer, groupIndex == 0_u )
				{
					c3d_lightsAABB[groupID].min() = min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
					c3d_lightsAABB[groupID].max() = max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );
				}
				FI;
#endif
			};

			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&]( sdw::ComputeIn in )
				{
					auto groupIndex = in.localInvocationIndex;
					auto groupID = in.workGroupID.x();
					auto threadIndex = in.globalInvocationID.x();

					auto aabbMin = writer.declLocale( "aabbMin"
						, vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f ) );
					auto aabbMax = writer.declLocale( "aabbMax"
						, vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f ) );

					if ( first )
					{
						// The 1st pass of the reduction operates on the light buffers.
						// First compute point lights AABB.
						FOR( writer, sdw::UInt, i, threadIndex, i < c3d_clustersData.pointLightCount(), i += c3d_numThreadGroups * NumThreads )
						{
							auto lightOffset = writer.declLocale( "lightOffset"
								, lights.getDirectionalsEnd() + i * PointLight::LightDataComponents );
							auto point = writer.declLocale( "point"
								, lights.getPointLight( lightOffset ) );
							auto vsPosition = writer.declLocale( "vsPosition"
								, c3d_cameraData.worldToCurView( vec4( point.position(), 1.0_f ) ) );

							aabbMin = min( aabbMin, vsPosition - point.radius() );
							aabbMax = max( aabbMax, vsPosition + point.radius() );
						}
						ROF;

						// Next, expand AABB for spot lights.
						FOR( writer, sdw::UInt, i, threadIndex, i < c3d_clustersData.spotLightCount(), i += c3d_numThreadGroups * NumThreads )
						{
							auto lightOffset = writer.declLocale( "lightOffset"
								, lights.getPointsEnd() + i * SpotLight::LightDataComponents );
							auto spot = writer.declLocale( "spot"
								, lights.getSpotLight( lightOffset ) );
							auto vsPosition = writer.declLocale( "vsPosition"
								, c3d_cameraData.worldToCurView( vec4( spot.position(), 1.0_f ) ) );

							aabbMin = min( aabbMin, vsPosition - spot.radius() );
							aabbMax = max( aabbMax, vsPosition + spot.radius() );
						}
						ROF;

						gsAABBMin[groupIndex] = aabbMin;
						gsAABBMax[groupIndex] = aabbMax;

						// Sync group shared memory writes.
						shader::groupMemoryBarrierWithGroupSync( writer );

						// Perform log-step reduction to allow each thread group in the dispatch
						// to reduce to a single element.
						logStepReduction( groupIndex, groupID );
					}
					else
					{
						// The subsequent passes of the reduction operate on the global AABB computed 
						// in previous pass.
						// This step is repeated until we are reduced to a single thread group.
						FOR( writer, sdw::UInt, i, groupIndex, i < c3d_reduceNumElements, i += NumThreads * c3d_numThreadGroups )
						{
							aabbMin = min( aabbMin, c3d_lightsAABB[i].min() );
							aabbMax = max( aabbMax, c3d_lightsAABB[i].max() );
						}
						ROF;

						gsAABBMin[groupIndex] = aabbMin;
						gsAABBMax[groupIndex] = aabbMax;

						// Sync group shared memory writes.
						shader::groupMemoryBarrierWithGroupSync( writer );

						// Perform log-step reduction to allow each thread group in the dispatch
						// to reduce to a single element. If there was only a single thread group
						// in this dispatch, then this will reduce to a single element.
						logStepReduction( groupIndex, groupID );

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
				, FrustumClusters const & clusters )
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
				, m_lightCache{ clusters.getCamera().getScene()->getLightCache() }
				, m_first{ framePass, context, graph, device, true, clusters }
				, m_second{ framePass, context, graph, device, false, clusters }
			{
			}

			CRG_API void resetPipeline( crg::VkPipelineShaderStageCreateInfoArray config
				, uint32_t index )
			{
				resetCommandBuffer( index );
				m_first.pipeline.resetPipeline( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_first.createInfo ), index );
				m_second.pipeline.resetPipeline( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_second.createInfo ), index );
				doCreatePipeline( index, m_first );
				doCreatePipeline( index, m_second );
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
					, bool first
					, FrustumClusters const & clusters )
					: shader{ VK_SHADER_STAGE_COMPUTE_BIT, "ReduceLightsAABB" + ( first ? std::string{ "/First" } : std::string{ "/Second" } ), createShader( first ) }
					, createInfo{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, shader ) } }
					, cpConfig{ crg::getDefaultV< InitialiseCallback >()
						, nullptr
						, IsEnabledCallback( [&clusters]() { return clusters.needsLightsUpdate(); } )
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
						, 1u }
				{
				}
			};

		private:
			LightCache const & m_lightCache;
			Pipeline m_first;
			Pipeline m_second;

		private:
			void doInitialise( uint32_t index )
			{
				m_first.pipeline.initialise();
				m_second.pipeline.initialise();
				doCreatePipeline( index, m_first );
				doCreatePipeline( index, m_second );
			}

			bool doIsEnabled()const
			{
				return ( m_first.cpConfig.isEnabled ? ( *m_first.cpConfig.isEnabled )() : false )
					|| ( m_second.cpConfig.isEnabled ? ( *m_second.cpConfig.isEnabled )() : false );
			}

			void doRecordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				struct
				{
					uint32_t numThreadGroups{};
					uint32_t reduceNumElements{};
				} dispatchData;

				auto pointLightsCount = m_lightCache.getLightsBufferCount( LightType::ePoint );
				auto spoLightsCount = m_lightCache.getLightsBufferCount( LightType::eSpot );
				auto maxLightsCount = std::max( pointLightsCount, spoLightsCount );

				// Don't dispatch more than 512 thread groups. The reduction algorithm depends on the
				// number of thread groups to be no more than 512. The buffer which stores the reduced AABB is sized
				// for a maximum of 512 thread groups.
				dispatchData.numThreadGroups = std::min( 512u
					, uint32_t( std::ceil( float( maxLightsCount ) / 512.0f ) ) );

				// First pass
				m_first.pipeline.recordInto( context, commandBuffer, index );
				m_context.vkCmdPushConstants( commandBuffer, m_first.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &dispatchData );
				m_context.vkCmdDispatch( commandBuffer, dispatchData.numThreadGroups, 1u, 1u );

				if ( dispatchData.numThreadGroups > 1u )
				{
					// In the first pass, the number of lights determines the number of
					// elements to be reduced.
					// In the second pass, the number of elements to be reduced is the 
					// number of thread groups from the first pass.
					dispatchData.reduceNumElements = dispatchData.numThreadGroups;
					dispatchData.numThreadGroups = 1u;
					// Second pass
					m_second.pipeline.recordInto( context, commandBuffer, index );
					m_context.vkCmdPushConstants( commandBuffer, m_second.pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &dispatchData );
					m_context.vkCmdDispatch( commandBuffer, dispatchData.numThreadGroups, 1u, 1u );
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

	crg::FramePass const & createReduceLightsAABBPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, FrustumClusters & clusters )
	{
		auto & pass = graph.createPass( "ReduceLightsAABB"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< rdclgb::FramePass >( framePass
					, context
					, graph
					, device
					, clusters );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *previousPass );
		cameraUbo.createPassBinding( pass, rdclgb::eCamera );
		auto & lights = clusters.getCamera().getScene()->getLightCache();
		lights.createPassBinding( pass, rdclgb::eLights );
		clusters.getClustersUbo().createPassBinding( pass, rdclgb::eClusters );
		createClearableOutputStorageBinding( pass, uint32_t( rdclgb::eLightsAABB ), "C3D_LightsAABB", clusters.getLightsAABBBuffer(), 0u, ashes::WholeSize );
		return pass;
	}

	//*********************************************************************************************
}
