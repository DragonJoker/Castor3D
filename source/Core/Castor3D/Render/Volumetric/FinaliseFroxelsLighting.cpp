#include "Castor3D/Render/Volumetric/FinaliseFroxelsLighting.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Volumetric/FrustumFroxels.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Ubos/FroxelsUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace finfxl
	{
		static void blur( sdw::ComputeWriter & writer
			, shader::FroxelsData const & froxelsData, sdw::Vec3Array gsInputCache, sdw::RWImage3DT< sdw::type::ImageFormat::eRgba32f > const & output
			, sdw::UInt const & groupIndex, sdw::U32Vec3 const & threadGroupID, sdw::U32Vec3 const & threaID, sdw::UInt const & maxDim )
		{
			auto coeffCount = froxelsData.blurFilterSize() + 1U; // +1 to account for the current pixel
			sdwIF( writer, groupIndex < maxDim )
			{
				auto basePixel = writer.declLocale( "basePixel", writer.cast< sdw::Int >( groupIndex ) );
				auto offset = writer.declLocale( "offset", 0_i );
				auto colour = writer.declLocale( "colour", gsInputCache[basePixel] * froxelsData.blurKernelWeight( 0_u ) );
				sdwFOR( writer, sdw::UInt, i, 1U, i < coeffCount, ++i )
				{
					++offset;
					colour += froxelsData.blurKernelWeight( i ) * gsInputCache[max( basePixel - offset, 0_i )];
					colour += froxelsData.blurKernelWeight( i ) * gsInputCache[min( basePixel + offset, writer.cast< sdw::Int >( maxDim ) - 1 )];
				}
				sdwROF
				output.store( sdw::i32vec3( threadGroupID + threaID ), sdw::vec4( colour, 1.0f ) );
			}
			sdwFI
		}

		enum class BindingsU32
		{
			eFroxels,
			eInputR,
			eInputG,
			eInputB,
			eOutput,
		};

		static ShaderPtr createShaderU32( RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			C3D_Froxels( writer, BindingsU32::eFroxels, 0u );
			auto c3d_inputR = writer.declStorageImg< RUImg3DR32 >( "c3d_inputR", BindingsU32::eInputR, 0u );
			auto c3d_inputG = writer.declStorageImg< RUImg3DR32 >( "c3d_inputG", BindingsU32::eInputG, 0u );
			auto c3d_inputB = writer.declStorageImg< RUImg3DR32 >( "c3d_inputB", BindingsU32::eInputB, 0u );

			auto c3d_output = writer.declStorageImg< RWFImg3DRgba32 >( "c3d_output", BindingsU32::eOutput, 0u );

			auto gsInputCache = writer.declSharedVariable< sdw::Vec3 >( "gsInputCache", MaxFroxelImageSize );

			writer.implementMainT< sdw::VoidT >( MaxFroxelImageSize
				, [&writer, &c3d_froxelsData, &c3d_inputR, &c3d_inputG, &c3d_inputB, &gsInputCache, &c3d_output]( sdw::ComputeIn const & in )
				{
					auto groupIndex = writer.declLocale( "groupIndex", in.localInvocationIndex );
					auto groupID = writer.declLocale( "groupID", in.workGroupID );
					auto maxDim = writer.declLocale( "maxDim", c3d_froxelsData.dimensions().x() );
					auto threadGroupID = writer.declLocale( "groupID", sdw::u32vec3( 0u, groupID.x(), groupID.y() ) );
					auto threadID = writer.declLocale( "threadID", sdw::u32vec3( groupIndex, 0u, 0u ) );

					// Fill cache
					sdwIF( writer, groupIndex < maxDim )
					{
						auto sourcePixelIndex = writer.declLocale( "sourcePixelIndex", sdw::i32vec3( threadGroupID + threadID ) );
						gsInputCache[groupIndex] = sdw::vec3( c3d_froxelsData.dequantize( c3d_inputR.load( sourcePixelIndex ) )
							, c3d_froxelsData.dequantize( c3d_inputG.load( sourcePixelIndex ) )
							, c3d_froxelsData.dequantize( c3d_inputB.load( sourcePixelIndex ) ) );
					}
					sdwFI
					shader::groupMemoryBarrierWithGroupSync( writer );

					// Blur from cache
					blur( writer, c3d_froxelsData, gsInputCache, c3d_output, groupIndex, threadGroupID, threadID, maxDim );
				} );
			return writer.getBuilder().releaseShader();
		}

		enum class Bindings
		{
			eFroxels,
			eInput,
			eOutput,
		};

		static ShaderPtr createShader( RenderDevice const & device
			, bool depth )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			C3D_Froxels( writer, Bindings::eFroxels, 0u );
			auto c3d_input = writer.declStorageImg< RWFImg3DRgba32 >( "c3d_input", Bindings::eInput, 0u );

			auto c3d_output = writer.declStorageImg< RWFImg3DRgba32 >( "c3d_output", Bindings::eOutput, 0u );

			auto gsInputCache = writer.declSharedVariable< sdw::Vec3 >( "gsInputCache", MaxFroxelImageSize );

			writer.implementMainT< sdw::VoidT >( MaxFroxelImageSize
				, [depth , &writer, &c3d_froxelsData, &c3d_input, &gsInputCache, &c3d_output]( sdw::ComputeIn const & in )
				{
					auto groupIndex = writer.declLocale( "groupIndex", in.localInvocationIndex );
					auto groupID = writer.declLocale( "groupID", in.workGroupID );
					auto maxDim = writer.declLocale( "maxDim", depth ? c3d_froxelsData.dimensions().z() : c3d_froxelsData.dimensions().y() );
					auto threadGroupID = writer.declLocale( "groupID", depth ? sdw::u32vec3( groupID.x(), 0u, groupID.y() ) : sdw::u32vec3( groupID.x(), groupID.y(), 0u ) );
					auto threadID = writer.declLocale( "threadID", depth ? sdw::u32vec3( 0u, groupIndex, 0u ) : sdw::u32vec3( 0u, 0u, groupIndex ) );

					// Fill cache
					sdwIF( writer, groupIndex < maxDim )
					{
						auto sourcePixelIndex = writer.declLocale( "sourcePixelIndex", sdw::i32vec3( threadGroupID + threadID ) );
						gsInputCache[groupIndex] = c3d_input.load( sourcePixelIndex ).xyz();
					}
					sdwFI
					shader::groupMemoryBarrierWithGroupSync( writer );

					// Blur from cache
					blur( writer, c3d_froxelsData, gsInputCache, c3d_output, groupIndex, threadGroupID, threadID, maxDim );
				} );
			return writer.getBuilder().releaseShader();
		}

		class FramePassU32
			: private DataHolderT< ShaderModule >
			, private DataHolderT< ashes::PipelineShaderStageCreateInfoArray >
			, public crg::ComputePass
		{
			using ShaderHolder = DataHolderT< ShaderModule >;
			using CreateInfoHolder = DataHolderT< ashes::PipelineShaderStageCreateInfoArray >;

		public:
			FramePassU32( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, crg::cp::Config config )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "Blur/X" ), createShaderU32( device ) } }
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
				, crg::cp::Config config
				, bool depth )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, String{ cuT( "Blur/" ) } + ( depth ? cuT( "Z" ) : cuT( "Y" ) ), createShader( device, depth ) } }
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

	void createFinaliseFroxelsLightingPass( crg::FramePassGroup & graph, RenderDevice const & device, FrustumFroxels const & froxels
		, Texture const & rawFroxelsLightingU32R, Texture const & rawFroxelsLightingU32G, Texture const & rawFroxelsLightingU32B
		, Texture & rawFroxelsLighting, Texture & finalFroxelsLighting )
	{
		auto const & scene = froxels.getScene();
		{
			auto & pass = graph.createPass( "Blur/X"
				, [&device, &froxels, &scene]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto runPass = makeRawUnique< finfxl::FramePassU32 >( framePass, context, runGraph, device
						, crg::cp::Config{}
							.groupCountX( froxels.getDimensions()->y )
							.groupCountY( froxels.getDimensions()->z )
							.isEnabled( crg::RunnablePass::IsEnabledCallback( [&scene](){ return scene.hasClusteredLights(); } ) ) );
					c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
						, runPass->getTimer() );
					return runPass;
				} );
			froxels.getFroxelsUbo().createPassBinding( pass, finfxl::BindingsU32::eFroxels );
			pass.addInputStorageT( *rawFroxelsLightingU32R.getLastAttach(), finfxl::BindingsU32::eInputR );
			pass.addInputStorageT( *rawFroxelsLightingU32G.getLastAttach(), finfxl::BindingsU32::eInputG );
			pass.addInputStorageT( *rawFroxelsLightingU32B.getLastAttach(), finfxl::BindingsU32::eInputB );
			finalFroxelsLighting.setLastAttach( pass.addOutputStorageImageT( finalFroxelsLighting.getWholeViewId(), finfxl::BindingsU32::eOutput ) );
		}
		{
			auto & pass = graph.createPass( "Blur/Y"
				, [&device, &froxels, &scene]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto runPass = makeRawUnique< finfxl::FramePass >( framePass, context, runGraph, device
						, crg::cp::Config{}
							.groupCountX( froxels.getDimensions()->x )
							.groupCountY( froxels.getDimensions()->z )
							.isEnabled( crg::RunnablePass::IsEnabledCallback( [&scene](){ return scene.hasClusteredLights(); } ) )
						, false );
					c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
						, runPass->getTimer() );
					return runPass;
				} );
			froxels.getFroxelsUbo().createPassBinding( pass, finfxl::Bindings::eFroxels );
			pass.addInputStorageT( *finalFroxelsLighting.getLastAttach(), finfxl::Bindings::eInput );
			rawFroxelsLighting.setLastAttach( pass.addOutputStorageImageT( rawFroxelsLighting.getWholeViewId(), finfxl::Bindings::eOutput ) );
		}
		{
			auto & pass = graph.createPass( "Blur/Z"
				, [&device, &froxels, &scene]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto runPass = makeRawUnique< finfxl::FramePass >( framePass, context, runGraph, device
						, crg::cp::Config{}
							.groupCountX( froxels.getDimensions()->x )
							.groupCountY( froxels.getDimensions()->y )
							.isEnabled( crg::RunnablePass::IsEnabledCallback( [&scene](){ return scene.hasClusteredLights(); } ) )
						, true );
					c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
						, runPass->getTimer() );
					return runPass;
				} );
			froxels.getFroxelsUbo().createPassBinding( pass, finfxl::Bindings::eFroxels );
			pass.addInputStorageT( *rawFroxelsLighting.getLastAttach(), finfxl::Bindings::eInput );
			finalFroxelsLighting.setLastAttach( pass.addInOutStorageT( *finalFroxelsLighting.getLastAttach(), finfxl::Bindings::eOutput ) );
		}
	}

	//*********************************************************************************************
}
