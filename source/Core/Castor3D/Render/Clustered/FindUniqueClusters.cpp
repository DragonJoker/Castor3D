#include "Castor3D/Render/Clustered/FindUniqueClusters.hpp"

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

	namespace fndunq
	{
		enum BindingPoints
		{
			eClustersFlags,
			eUniqueClusters,
			eClustersIndirect,
		};

		static uint32_t constexpr NumThreads = 1024u;

		static ShaderPtr createShader()
		{
			sdw::ComputeWriter writer;

			// Inputs
			C3D_ClusterFlags( writer
				, eClustersFlags
				, 0u );
			C3D_UniqueClusters( writer
				, eUniqueClusters
				, 0u );
			C3D_ClustersIndirect( writer
				, eClustersIndirect
				, 0u );

			writer.implementMainT< sdw::VoidT >(NumThreads
				, [&](sdw::ComputeIn in)
				{
					auto clusterID = writer.declLocale( "clusterID"
						, in.globalInvocationID.x() );

					IF( writer, clusterID == 0_u )
					{
						c3d_clustersCountY = 1_u;
						c3d_clustersCountZ = 1_u;
					}
					FI;

					IF( writer, c3d_clusterFlags[clusterID] != 0_u )
					{
						auto i = writer.declLocale ("i"
							, atomicAdd( c3d_clustersCountX, 1_u ) );
						c3d_uniqueClusters[i] = clusterID;
					}
					FI;
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
				, FrustumClusters const & clusters )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, "FindUniqueClusters", createShader() } }
				, CreateInfoHolder{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, ShaderHolder::getData() ) } }
				, EnabledHolder{ true }
				, crg::ComputePass{framePass
					, context
					, graph
					, crg::ru::Config{}
					, crg::cp::Config{}
						.groupCountX( uint32_t( std::ceil( float( clusters.getDimensions()->x * clusters.getDimensions()->y * clusters.getDimensions()->z ) / float( NumThreads ) ) ) )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( CreateInfoHolder::getData() ) ) }
			{
			}
		};
	}

	//*********************************************************************************************

	crg::FramePass const & createFindUniqueClustersPass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, FrustumClusters & clusters )
	{
		auto & pass = graph.createPass( "FindUniqueClusters"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< fndunq::FramePass >( framePass
					, context
					, graph
					, device
					, clusters );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( previousPass );
		createInputStoragePassBinding( pass, uint32_t( fndunq::eClustersFlags ), "C3D_ClustersFlags", clusters.getClusterFlagsBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( fndunq::eUniqueClusters ), "C3D_UniqueClusters", clusters.getUniqueClustersBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( fndunq::eClustersIndirect ), "C3D_ClustersIndirect", clusters.getClustersIndirectBuffer(), 0u, ashes::WholeSize );
		return pass;
	}

	//*********************************************************************************************
}
