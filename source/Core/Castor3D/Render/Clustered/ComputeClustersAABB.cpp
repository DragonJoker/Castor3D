#include "Castor3D/Render/Clustered/ComputeClustersAABB.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
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

			auto screenToView = writer.implementFunction< sdw::Vec4 >( "screenToView"
				, [&writer, &c3d_renderData, &c3d_cameraData]( sdw::Vec4 const & screen )
				{
					// Convert to normalized texture coordinates in the range [0 .. 1].
					auto texCoord = writer.declLocale( "texCoord"
						, screen.xy() * c3d_renderData.invRenderSize() );

					// Convert to clip space
					auto clip = writer.declLocale( "clip"
						, sdw::vec4( sdw::fma( texCoord.xy(), vec2( 2.0_f ), vec2( -1.0_f ) )
							, screen.zw() ) );

					auto view = writer.declLocale( "view"
						, c3d_cameraData.projToView( clip ) );
					view /= view.w();
					writer.returnStmt( view );
				}
				, sdw::InVec4{ writer, "screen" } );

			auto intersectLinePlane = writer.implementFunction< sdw::Vec3 >( "c3d_intersectLinePlane"
				, [&writer]( sdw::Vec3 const & a
					, sdw::Vec3 const & b
					, sdw::Float const & d )
				{
					auto ab = b - a;
					auto normal = vec3( 0.0_f, 0.0_f, 1.0_f );
					auto t = writer.declLocale( "t"
						, ( d - dot( normal, a ) ) / dot( normal, ab ) );
					writer.returnStmt( a + ab * t );
				}
				, sdw::InVec3{ writer, "a" }
				, sdw::InVec3{ writer, "b" }
				, sdw::InFloat{ writer, "d" } );

			writer.implementMainT< sdw::VoidT >( 1u, 1u, 1u
				, [&writer, &c3d_clustersData, &c3d_clustersLightsData, &c3d_lightsAABBRange, &c3D_clustersAABB
					, screenToView, &intersectLinePlane]( sdw::ComputeIn const & in )
				{
					auto const & clusterIndex3D = in.globalInvocationID;
					auto clusterIndex1D = writer.declLocale( "clusterIndex1D"
						, c3d_clustersData.computeClusterIndex1D( clusterIndex3D ) );

					// Compute the near and far planes for cluster K.
					auto tileNearFar = writer.declLocale( "tileNearFar"
						, c3d_clustersData.getClusterDepthBounds( clusterIndex3D
							, c3d_clustersLightsData
							, c3d_lightsAABBRange ) );

					// Reversed depth implies maxZ is 0.0f instead of 1.0f.
					float constexpr maxZ = 0.0f;
					// The top-left point of cluster K in screen space.
					auto pMin = writer.declLocale( "pMin"
						, sdw::vec4( vec2( clusterIndex3D.xy() * c3d_clustersData.clusterSize() ), maxZ, 1.0f ) );
					// The bottom-right point of cluster K in screen space.
					auto pMax = writer.declLocale( "pMax"
						, sdw::vec4( vec2( ( clusterIndex3D.xy() + u32vec2( 1_u ) ) * c3d_clustersData.clusterSize() ), maxZ, 1.0f ) );

					// Transform the screen space points to view space.
					pMin = screenToView( pMin );
					pMax = screenToView( pMax );

					// Find the min and max points on the near and far planes.
					// Origin (camera eye position)
					auto eye = writer.declLocale< sdw::Vec3 >( "eye"
						, vec3( 0.0_f ) );
					auto nearMin = writer.declLocale( "nearMin"
						, intersectLinePlane( eye, pMin.xyz(), tileNearFar.x() ) );
					auto nearMax = writer.declLocale( "nearMax"
						, intersectLinePlane( eye, pMax.xyz(), tileNearFar.x() ) );
					auto farMin = writer.declLocale( "farMin"
						, intersectLinePlane( eye, pMin.xyz(), tileNearFar.y() ) );
					auto farMax = writer.declLocale( "farMax"
						, intersectLinePlane( eye, pMax.xyz(), tileNearFar.y() ) );

					auto aabbMin = writer.declLocale( "aabbMin"
						, min( nearMin, min( nearMax, min( farMin, farMax ) ) ) );
					auto aabbMax = writer.declLocale( "aabbMax"
						, max( nearMin, max( nearMax, max( farMin, farMax ) ) ) );

					c3D_clustersAABB[clusterIndex1D] = shader::AABB{ vec4( aabbMin, 1.0f )
						, vec4( aabbMax, 1.0f ) };

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
						.groupCountX( clusters.getDimensions()->x )
						.groupCountY( clusters.getDimensions()->y )
						.groupCountZ( clusters.getDimensions()->z )
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

		c3d::addDescriptorSetLayoutBindingT( bindings, dspclsb::Bindings::eMainCamera, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );
		c3d::addDescriptorSetLayoutBindingT( bindings, dspclsb::Bindings::eClustersCamera, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );
		c3d::addDescriptorSetLayoutBindingT( bindings, dspclsb::Bindings::eClusters, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );
		c3d::addDescriptorSetLayoutBindingT( bindings, dspclsb::Bindings::eClustersAABB, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );

		writes.emplace_back( uint32_t( dspclsb::Bindings::eMainCamera ), 0u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ mainCameraUbo.getUbo().getBuffer().getBuffer(), mainCameraUbo.getUbo().getByteOffset(), mainCameraUbo.getUbo().getByteRange() } } );
		writes.emplace_back( uint32_t( dspclsb::Bindings::eClustersCamera ), 0u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ clustersCameraUbo.getUbo().getBuffer().getBuffer(), clustersCameraUbo.getUbo().getByteOffset(), clustersCameraUbo.getUbo().getByteRange() } } );
		auto & clustersUbo = clusters.getClustersUbo();
		writes.emplace_back( uint32_t( dspclsb::Bindings::eClusters ), 0u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ clustersUbo.getUbo().getBuffer().getBuffer(), clustersUbo.getUbo().getByteOffset(), clustersUbo.getUbo().getByteRange() } } );
		writes.emplace_back( uint32_t( dspclsb::Bindings::eClustersAABB ), 0u, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ clustersAABB.getBuffer(), 0u, clustersAABB.getSize() } } );
	}

	//*********************************************************************************************
}
