#include "Castor3D/Render/Volumetric/ComputeLightsSliceBounds.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Volumetric/FrustumFroxels.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Ubos/FroxelsUbo.hpp"

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ShaderWriter/ComputeWriter.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace lgtslcbnd
	{
		enum class Bindings
		{
			eFroxels,
			eInput,
			eLightsSliceBounds,
		};

		template< ast::type::ImageFormat FormatT >
		static ShaderPtr createShaderT( RenderDevice const & device )
		{
			uint32_t NumThreads = device.getSubgroupSize();

			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			// Inputs
			C3D_Froxels( writer
				, Bindings::eFroxels
				, 0u );

			auto c3d_input = writer.declStorageImg< FormatT, RWImg3D >( "c3d_input", Bindings::eInput, 0u );

			auto lightsSliceBoundsBuffer = writer.declStorageBuffer( "c3d_lightsSliceBoundsBuffer", uint32_t( Bindings::eLightsSliceBounds ), 0u );
			auto c3d_lightsSliceBounds = lightsSliceBoundsBuffer.declMemberArray< sdw::UInt >( "lsb" );
			lightsSliceBoundsBuffer.end();

			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&writer, &c3d_input, &c3d_lightsSliceBounds, &c3d_froxelsData]( sdw::SubgroupIn const & in )
				{
					auto gridDim = writer.declLocale( "gridDim", c3d_froxelsData.dimensions() );
					auto froxelIndex2D = writer.declLocale( "froxelIndex2D", in.workGroupID.xy() );
					auto maxTilesOffset = writer.declLocale( "maxTilesOffset", gridDim.x() * gridDim.y() );
					auto minTileZ = writer.declLocale( "minTileZ", gridDim.z() );
					auto maxTileZ = writer.declLocale( "maxTileZ", 0_u );

					sdwFOR( writer, sdw::UInt, slice, in.subgroupInvocationID, slice < gridDim.z(), slice += in.subgroupSize )
					{
						auto color = writer.declLocale< sdw::Vec3 >( "color", shader::makeVec3( writer, c3d_input.load( ivec3( froxelIndex2D, slice ) ) ) );
						sdwIF( writer, color.x() > 0.0_f || color.y() > 0.0_f || color.z() > 0.0_f )
						{
							minTileZ = min( minTileZ, slice );
							maxTileZ = max( maxTileZ, slice );
						}
						sdwFI
					}
					sdwROF

					minTileZ = subgroupMin( minTileZ );
					maxTileZ = subgroupMax( maxTileZ );

					sdwIF( writer, subgroupElect( writer ) )
					{
						auto tileIndex = writer.declLocale( "tileIndex", froxelIndex2D.y() * gridDim.x() + froxelIndex2D.x() );
						c3d_lightsSliceBounds[tileIndex] = minTileZ;
						c3d_lightsSliceBounds[maxTilesOffset + tileIndex] = maxTileZ;
					}
					sdwFI
				} );
			return writer.getBuilder().releaseShader();
		}

		static ShaderPtr createShader( RenderDevice const & device
			, sdw::type::ImageFormat imageFormat )
		{
			switch ( imageFormat )
			{
			case sdw::type::ImageFormat::eRgba32f: return createShaderT< sdw::type::ImageFormat::eRgba32f >( device );
			case sdw::type::ImageFormat::eRgba16f: return createShaderT< sdw::type::ImageFormat::eRgba16f >( device );
			case sdw::type::ImageFormat::eRg32f: return createShaderT< sdw::type::ImageFormat::eRg32f >( device );
			case sdw::type::ImageFormat::eRg16f: return createShaderT< sdw::type::ImageFormat::eRg16f >( device );
			case sdw::type::ImageFormat::eR32f: return createShaderT< sdw::type::ImageFormat::eR32f >( device );
			case sdw::type::ImageFormat::eR16f: return createShaderT< sdw::type::ImageFormat::eR16f >( device );
			case sdw::type::ImageFormat::eR11fG11fB10f: return createShaderT< sdw::type::ImageFormat::eR11fG11fB10f >( device );
			case sdw::type::ImageFormat::eRgba32i: return createShaderT< sdw::type::ImageFormat::eRgba32i >( device );
			case sdw::type::ImageFormat::eRgba16i: return createShaderT< sdw::type::ImageFormat::eRgba16i >( device );
			case sdw::type::ImageFormat::eRgba8i: return createShaderT< sdw::type::ImageFormat::eRgba8i >( device );
			case sdw::type::ImageFormat::eRg32i: return createShaderT< sdw::type::ImageFormat::eRg32i >( device );
			case sdw::type::ImageFormat::eRg16i: return createShaderT< sdw::type::ImageFormat::eRg16i >( device );
			case sdw::type::ImageFormat::eRg8i: return createShaderT< sdw::type::ImageFormat::eRg8i >( device );
			case sdw::type::ImageFormat::eR32i: return createShaderT< sdw::type::ImageFormat::eR32i >( device );
			case sdw::type::ImageFormat::eR16i: return createShaderT< sdw::type::ImageFormat::eR16i >( device );
			case sdw::type::ImageFormat::eR8i: return createShaderT< sdw::type::ImageFormat::eR8i >( device );
			case sdw::type::ImageFormat::eRgba32u: return createShaderT< sdw::type::ImageFormat::eRgba32u >( device );
			case sdw::type::ImageFormat::eRgba16u: return createShaderT< sdw::type::ImageFormat::eRgba16u >( device );
			case sdw::type::ImageFormat::eRgba8u: return createShaderT< sdw::type::ImageFormat::eRgba8u >( device );
			case sdw::type::ImageFormat::eRg32u: return createShaderT< sdw::type::ImageFormat::eRg32u >( device );
			case sdw::type::ImageFormat::eRg16u: return createShaderT< sdw::type::ImageFormat::eRg16u >( device );
			case sdw::type::ImageFormat::eRg8u: return createShaderT< sdw::type::ImageFormat::eRg8u >( device );
			case sdw::type::ImageFormat::eR32u: return createShaderT< sdw::type::ImageFormat::eR32u >( device );
			case sdw::type::ImageFormat::eR16u: return createShaderT< sdw::type::ImageFormat::eR16u >( device );
			case sdw::type::ImageFormat::eR8u: return createShaderT< sdw::type::ImageFormat::eR8u >( device );
			case sdw::type::ImageFormat::eRgb10A2u: return createShaderT< sdw::type::ImageFormat::eRgb10A2u >( device );
			case sdw::type::ImageFormat::eRgba16Snorm: return createShaderT< sdw::type::ImageFormat::eRgba16Snorm >( device );
			case sdw::type::ImageFormat::eRgba8Snorm: return createShaderT< sdw::type::ImageFormat::eRgba8Snorm >( device );
			case sdw::type::ImageFormat::eRg16Snorm: return createShaderT< sdw::type::ImageFormat::eRg16Snorm >( device );
			case sdw::type::ImageFormat::eRg8Snorm: return createShaderT< sdw::type::ImageFormat::eRg8Snorm >( device );
			case sdw::type::ImageFormat::eR16Snorm: return createShaderT< sdw::type::ImageFormat::eR16Snorm >( device );
			case sdw::type::ImageFormat::eR8Snorm: return createShaderT< sdw::type::ImageFormat::eR8Snorm >( device );
			case sdw::type::ImageFormat::eRgba16Unorm: return createShaderT< sdw::type::ImageFormat::eRgba16Unorm >( device );
			case sdw::type::ImageFormat::eRgba8Unorm: return createShaderT< sdw::type::ImageFormat::eRgba8Unorm >( device );
			case sdw::type::ImageFormat::eRg16Unorm: return createShaderT< sdw::type::ImageFormat::eRg16Unorm >( device );
			case sdw::type::ImageFormat::eRg8Unorm: return createShaderT< sdw::type::ImageFormat::eRg8Unorm >( device );
			case sdw::type::ImageFormat::eR16Unorm: return createShaderT< sdw::type::ImageFormat::eR16Unorm >( device );
			case sdw::type::ImageFormat::eR8Unorm: return createShaderT< sdw::type::ImageFormat::eR8Unorm >( device );
			case sdw::type::ImageFormat::eRgb10A2Unorm: return createShaderT< sdw::type::ImageFormat::eRgb10A2Unorm >( device );
			default:
				CU_Failure( "Unsupported ImageFormat" );
				return nullptr;
			}
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
				, sdw::type::ImageFormat imageFormat
				, crg::cp::Config config )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "ComputeSliceBounds" ), createShader( device, imageFormat ) } }
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

	//*********************************************************************************************

	void createComputeLightsSliceBoundsPass( crg::FramePassGroup & graph, RenderDevice const & device, FrustumFroxels const & froxels
		, Texture const & finalFroxelsLighting, BufferBase & lightsSliceBounds )
	{
		auto imageFormat = getImageFormat( finalFroxelsLighting.getFormat() );
		Scene const & scene = froxels.getScene();

		auto & pass = graph.createPass( "ComputeSliceBounds"
			, [&device, &froxels, &scene, imageFormat]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< lgtslcbnd::FramePass >( framePass, context, runGraph
					, device, imageFormat
					, crg::cp::Config{}
						.groupCountX( froxels.getDimensions()->x )
						.groupCountY( froxels.getDimensions()->y )
						.isEnabled( crg::RunnablePass::IsEnabledCallback( [&scene](){ return scene.hasClusteredLights(); } ) ) );
				c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		froxels.getFroxelsUbo().createPassBinding( pass, lgtslcbnd::Bindings::eFroxels );
		pass.addInputStorageT( *finalFroxelsLighting.getLastAttach(), lgtslcbnd::Bindings::eInput );

		lightsSliceBounds.setLastAttach( pass.addOutputStorageBufferT( lightsSliceBounds.bufferViewId, lgtslcbnd::Bindings::eLightsSliceBounds ) );
	}

	//*********************************************************************************************
}
