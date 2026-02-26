#include "Castor3D/Render/Clustered/ComputeClustersAABB.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"
#include "Castor3D/Shader/Ubos/RenderUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace cptclsb
	{
		enum class Bindings
		{
			eCamera,
			eRender,
			eClusters,
			eReducedLightsAABB,
			eClustersAABB,
		};

		static ShaderPtr createShader( RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };
			shader::Utils utils{ writer };

			// Inputs
			C3D_Camera( writer
				, Bindings::eCamera
				, 0u );
			C3D_Render( writer
				, Bindings::eRender
				, 0u );
			C3D_Clusters( writer
				, Bindings::eClusters
				, 0u );
			C3D_ReducedLightsAABB( writer
				, Bindings::eReducedLightsAABB
				, 0u );
			C3D_ClustersAABB( writer
				, Bindings::eClustersAABB
				, 0u );

			writer.implementMainT< sdw::VoidT >( 1u, 1u, 1u
				, [&writer, &c3d_cameraData, &c3d_renderData, &c3d_clustersData, &c3d_clustersLightsData, &c3d_lightsAABBRange, &c3D_clustersAABB
					, &utils]( sdw::ComputeIn const & in )
				{
					auto const & clusterIndex3D = in.globalInvocationID;
					auto tileNearFar = writer.declLocale( "tileNearFar"
						, c3d_clustersData.getClusterDepthBounds( clusterIndex3D, c3d_clustersLightsData, c3d_lightsAABBRange ) );

					auto clusterIndex1D = writer.declLocale( "clusterIndex1D"
						, c3d_clustersData.computeClusterIndex1D( clusterIndex3D ) );
					c3D_clustersAABB[clusterIndex1D] = utils.computeAABB( clusterIndex3D, c3d_clustersData.clusterSize(), tileNearFar
						, c3d_cameraData.getInvProjMtx(), c3d_renderData.invRenderSize() );

				} );
			return writer.getBuilder().releaseShader();
		}

		class FramePass
			: private DataHolderT< ShaderModule >
			, private DataHolderT< ashes::PipelineShaderStageCreateInfoArray >
			, public crg::ComputePass
		{
			using ShaderHolder = DataHolderT< ShaderModule >;
			using CreateInfoHolder = DataHolderT< ashes::PipelineShaderStageCreateInfoArray >;

		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, crg::cp::Config config )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "ComputeClustersAABB" ), createShader( device ) } }
				, CreateInfoHolder{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, ShaderHolder::getData() ) } }
				, crg::ComputePass{ framePass
					, context
					, graph
					, crg::ru::Config{}
					, config
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( CreateInfoHolder::getData() ) ) }
			{
			}
		};
	}

	namespace dspclsb
	{
		enum class Bindings
		{
			eMainCamera,
			eClustersCamera,
			eClusters,
			eClustersAABB,
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
			C3D_Clusters( writer
				, Bindings::eClusters
				, 0u );
			C3D_ClustersAABB( writer
				, Bindings::eClustersAABB
				, 0u );

			writer.implementEntryPointT< shader::Position4FT, shader::Colour4FT >( [&c3d_cameraDataMain, &c3d_cameraDataClusters, &c3D_clustersAABB, &c3d_clustersData
				, &writer]( sdw::VertexInT< shader::Position4FT > const & in
					, sdw::VertexOutT< shader::Colour4FT > out )
				{
					auto clusterIndex3D = writer.declLocale( "clusterIndex3D"
						, c3d_clustersData.computeClusterIndex3D( writer.cast< sdw::UInt >( in.instanceIndex ) ) );
					auto aabb = writer.declLocale( "aabb"
						, c3D_clustersAABB[in.instanceIndex] );
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

					out.colour() = vec4( writer.cast< sdw::Float >( clusterIndex3D.x() ) / writer.cast< sdw::Float >( c3d_clustersData.dimensions().x() )
						, writer.cast< sdw::Float >( clusterIndex3D.y() ) / writer.cast< sdw::Float >( c3d_clustersData.dimensions().y() )
						, writer.cast< sdw::Float >( clusterIndex3D.z() ) / writer.cast< sdw::Float >( c3d_clustersData.dimensions().z() )
						, 1.0_f );
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

	void createComputeClustersAABBPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters const & clusters
		, CameraUbo const & clustersCameraUbo
		, RenderUbo const & renderUbo
		, BufferBase const & reducedLightsAABB
		, BufferBase & clustersAABB )
	{
		auto & pass = graph.createPass( "ComputeClustersAABB"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< cptclsb::FramePass >( framePass
					, context
					, runGraph
					, device
					, crg::cp::Config{}
						.getGroupCountX( crg::cp::GetGroupCountCallback( [&clusters](){ return clusters.getDimensions()->x; } ) )
						.getGroupCountY( crg::cp::GetGroupCountCallback( [&clusters](){ return clusters.getDimensions()->y; } ) )
						.getGroupCountZ( crg::cp::GetGroupCountCallback( [&clusters](){ return clusters.getDimensions()->z; } ) )
						.enabled( &clusters.needsClustersUpdate() ) );
				c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		renderUbo.createPassBinding( pass, cptclsb::Bindings::eRender );
		clustersCameraUbo.createPassBinding( pass, cptclsb::Bindings::eCamera );
		clusters.getClustersUbo().createPassBinding( pass, cptclsb::Bindings::eClusters );
		pass.addInputStorage( *reducedLightsAABB.getLastAttach(), uint32_t( cptclsb::Bindings::eReducedLightsAABB ) );
		clustersAABB.setLastAttach( pass.addClearableOutputStorageBuffer( clustersAABB.bufferViewId, uint32_t( cptclsb::Bindings::eClustersAABB ) ) );
	}

	void createDisplayClustersAABBProgram( RenderDevice const & device
		, FrustumClusters const & clusters
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, ashes::WriteDescriptorSetArray & writes
		, BufferBase const & clustersAABB )
	{
		ProgramModule programModule{ "ClustersAABB", dspclsb::createDebugDisplayShader( device ) };
		program = makeProgramStates( device, programModule );

		mainCameraUbo.addLayoutBindingT( bindings, dspclsb::Bindings::eMainCamera, VK_SHADER_STAGE_VERTEX_BIT );
		clustersCameraUbo.addLayoutBindingT( bindings, dspclsb::Bindings::eClustersCamera, VK_SHADER_STAGE_VERTEX_BIT );
		clusters.getClustersUbo().addLayoutBindingT( bindings, dspclsb::Bindings::eClusters, VK_SHADER_STAGE_VERTEX_BIT );
		clustersAABB.addLayoutBindingT( bindings, dspclsb::Bindings::eClustersAABB, VK_SHADER_STAGE_VERTEX_BIT );

		mainCameraUbo.addDescriptorWriteT( writes, dspclsb::Bindings::eMainCamera );
		clustersCameraUbo.addDescriptorWriteT( writes, dspclsb::Bindings::eMainCamera );
		clusters.getClustersUbo().addDescriptorWriteT( writes, dspclsb::Bindings::eClusters );
		clustersAABB.addDescriptorWriteT( writes, dspclsb::Bindings::eClustersAABB );
	}

	//*********************************************************************************************
}
