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
		template< sdw::type::ImageFormat FormatT >
		static void blur( sdw::ComputeWriter & writer
			, shader::FroxelsData const & froxelsData, sdw::Vec3Array const & gsInputCache, sdw::RWImage3DT< FormatT > const & output
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
				output.store( sdw::i32vec3( threadGroupID + threaID ), shader::makeFetchT< FormatT >( writer, colour ) );
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

		template< sdw::type::ImageFormat FormatT >
		static ShaderPtr createShaderU32T( RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			C3D_Froxels( writer, BindingsU32::eFroxels, 0u );
			auto c3d_inputR = writer.declStorageImg< RUImg3DR32 >( "c3d_inputR", BindingsU32::eInputR, 0u );
			auto c3d_inputG = writer.declStorageImg< RUImg3DR32 >( "c3d_inputG", BindingsU32::eInputG, 0u );
			auto c3d_inputB = writer.declStorageImg< RUImg3DR32 >( "c3d_inputB", BindingsU32::eInputB, 0u );

			auto c3d_output = writer.declStorageImg< FormatT, RWImg3D >( "c3d_output", BindingsU32::eOutput, 0u );

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

		static ShaderPtr createShaderU32( RenderDevice const & device
			, sdw::type::ImageFormat imageFormat )
		{
			switch ( imageFormat )
			{
			case sdw::type::ImageFormat::eRgba32f: return createShaderU32T< sdw::type::ImageFormat::eRgba32f >( device );
			case sdw::type::ImageFormat::eRgba16f: return createShaderU32T< sdw::type::ImageFormat::eRgba16f >( device );
			case sdw::type::ImageFormat::eRg32f: return createShaderU32T< sdw::type::ImageFormat::eRg32f >( device );
			case sdw::type::ImageFormat::eRg16f: return createShaderU32T< sdw::type::ImageFormat::eRg16f >( device );
			case sdw::type::ImageFormat::eR32f: return createShaderU32T< sdw::type::ImageFormat::eR32f >( device );
			case sdw::type::ImageFormat::eR16f: return createShaderU32T< sdw::type::ImageFormat::eR16f >( device );
			case sdw::type::ImageFormat::eR11fG11fB10f: return createShaderU32T< sdw::type::ImageFormat::eR11fG11fB10f >( device );
			case sdw::type::ImageFormat::eRgba32i: return createShaderU32T< sdw::type::ImageFormat::eRgba32i >( device );
			case sdw::type::ImageFormat::eRgba16i: return createShaderU32T< sdw::type::ImageFormat::eRgba16i >( device );
			case sdw::type::ImageFormat::eRgba8i: return createShaderU32T< sdw::type::ImageFormat::eRgba8i >( device );
			case sdw::type::ImageFormat::eRg32i: return createShaderU32T< sdw::type::ImageFormat::eRg32i >( device );
			case sdw::type::ImageFormat::eRg16i: return createShaderU32T< sdw::type::ImageFormat::eRg16i >( device );
			case sdw::type::ImageFormat::eRg8i: return createShaderU32T< sdw::type::ImageFormat::eRg8i >( device );
			case sdw::type::ImageFormat::eR32i: return createShaderU32T< sdw::type::ImageFormat::eR32i >( device );
			case sdw::type::ImageFormat::eR16i: return createShaderU32T< sdw::type::ImageFormat::eR16i >( device );
			case sdw::type::ImageFormat::eR8i: return createShaderU32T< sdw::type::ImageFormat::eR8i >( device );
			case sdw::type::ImageFormat::eRgba32u: return createShaderU32T< sdw::type::ImageFormat::eRgba32u >( device );
			case sdw::type::ImageFormat::eRgba16u: return createShaderU32T< sdw::type::ImageFormat::eRgba16u >( device );
			case sdw::type::ImageFormat::eRgba8u: return createShaderU32T< sdw::type::ImageFormat::eRgba8u >( device );
			case sdw::type::ImageFormat::eRg32u: return createShaderU32T< sdw::type::ImageFormat::eRg32u >( device );
			case sdw::type::ImageFormat::eRg16u: return createShaderU32T< sdw::type::ImageFormat::eRg16u >( device );
			case sdw::type::ImageFormat::eRg8u: return createShaderU32T< sdw::type::ImageFormat::eRg8u >( device );
			case sdw::type::ImageFormat::eR32u: return createShaderU32T< sdw::type::ImageFormat::eR32u >( device );
			case sdw::type::ImageFormat::eR16u: return createShaderU32T< sdw::type::ImageFormat::eR16u >( device );
			case sdw::type::ImageFormat::eR8u: return createShaderU32T< sdw::type::ImageFormat::eR8u >( device );
			case sdw::type::ImageFormat::eRgb10A2u: return createShaderU32T< sdw::type::ImageFormat::eRgb10A2u >( device );
			case sdw::type::ImageFormat::eRgba16Snorm: return createShaderU32T< sdw::type::ImageFormat::eRgba16Snorm >( device );
			case sdw::type::ImageFormat::eRgba8Snorm: return createShaderU32T< sdw::type::ImageFormat::eRgba8Snorm >( device );
			case sdw::type::ImageFormat::eRg16Snorm: return createShaderU32T< sdw::type::ImageFormat::eRg16Snorm >( device );
			case sdw::type::ImageFormat::eRg8Snorm: return createShaderU32T< sdw::type::ImageFormat::eRg8Snorm >( device );
			case sdw::type::ImageFormat::eR16Snorm: return createShaderU32T< sdw::type::ImageFormat::eR16Snorm >( device );
			case sdw::type::ImageFormat::eR8Snorm: return createShaderU32T< sdw::type::ImageFormat::eR8Snorm >( device );
			case sdw::type::ImageFormat::eRgba16Unorm: return createShaderU32T< sdw::type::ImageFormat::eRgba16Unorm >( device );
			case sdw::type::ImageFormat::eRgba8Unorm: return createShaderU32T< sdw::type::ImageFormat::eRgba8Unorm >( device );
			case sdw::type::ImageFormat::eRg16Unorm: return createShaderU32T< sdw::type::ImageFormat::eRg16Unorm >( device );
			case sdw::type::ImageFormat::eRg8Unorm: return createShaderU32T< sdw::type::ImageFormat::eRg8Unorm >( device );
			case sdw::type::ImageFormat::eR16Unorm: return createShaderU32T< sdw::type::ImageFormat::eR16Unorm >( device );
			case sdw::type::ImageFormat::eR8Unorm: return createShaderU32T< sdw::type::ImageFormat::eR8Unorm >( device );
			case sdw::type::ImageFormat::eRgb10A2Unorm: return createShaderU32T< sdw::type::ImageFormat::eRgb10A2Unorm >( device );
			default:
				CU_Failure( "Unsupported ImageFormat" );
				return nullptr;
			}
		}

		enum class Bindings
		{
			eFroxels,
			eInput,
			eOutput,
		};

		template< ast::type::ImageFormat FormatT >
		static ShaderPtr createShaderT( RenderDevice const & device
			, bool depth )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			C3D_Froxels( writer, Bindings::eFroxels, 0u );
			auto c3d_input = writer.declStorageImg< FormatT, RWImg3D >( "c3d_input", Bindings::eInput, 0u );

			auto c3d_output = writer.declStorageImg< FormatT, RWImg3D >( "c3d_output", Bindings::eOutput, 0u );

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
						gsInputCache[groupIndex] = shader::makeVec3( writer, c3d_input.load( sourcePixelIndex ) );
					}
					sdwFI
					shader::groupMemoryBarrierWithGroupSync( writer );

					// Blur from cache
					blur( writer, c3d_froxelsData, gsInputCache, c3d_output, groupIndex, threadGroupID, threadID, maxDim );
				} );
			return writer.getBuilder().releaseShader();
		}

		static ShaderPtr createShader( RenderDevice const & device
			, bool depth
			, sdw::type::ImageFormat imageFormat )
		{
			switch ( imageFormat )
			{
			case sdw::type::ImageFormat::eRgba32f: return createShaderT< sdw::type::ImageFormat::eRgba32f >( device, depth );
			case sdw::type::ImageFormat::eRgba16f: return createShaderT< sdw::type::ImageFormat::eRgba16f >( device, depth );
			case sdw::type::ImageFormat::eRg32f: return createShaderT< sdw::type::ImageFormat::eRg32f >( device, depth );
			case sdw::type::ImageFormat::eRg16f: return createShaderT< sdw::type::ImageFormat::eRg16f >( device, depth );
			case sdw::type::ImageFormat::eR32f: return createShaderT< sdw::type::ImageFormat::eR32f >( device, depth );
			case sdw::type::ImageFormat::eR16f: return createShaderT< sdw::type::ImageFormat::eR16f >( device, depth );
			case sdw::type::ImageFormat::eR11fG11fB10f: return createShaderT< sdw::type::ImageFormat::eR11fG11fB10f >( device, depth );
			case sdw::type::ImageFormat::eRgba32i: return createShaderT< sdw::type::ImageFormat::eRgba32i >( device, depth );
			case sdw::type::ImageFormat::eRgba16i: return createShaderT< sdw::type::ImageFormat::eRgba16i >( device, depth );
			case sdw::type::ImageFormat::eRgba8i: return createShaderT< sdw::type::ImageFormat::eRgba8i >( device, depth );
			case sdw::type::ImageFormat::eRg32i: return createShaderT< sdw::type::ImageFormat::eRg32i >( device, depth );
			case sdw::type::ImageFormat::eRg16i: return createShaderT< sdw::type::ImageFormat::eRg16i >( device, depth );
			case sdw::type::ImageFormat::eRg8i: return createShaderT< sdw::type::ImageFormat::eRg8i >( device, depth );
			case sdw::type::ImageFormat::eR32i: return createShaderT< sdw::type::ImageFormat::eR32i >( device, depth );
			case sdw::type::ImageFormat::eR16i: return createShaderT< sdw::type::ImageFormat::eR16i >( device, depth );
			case sdw::type::ImageFormat::eR8i: return createShaderT< sdw::type::ImageFormat::eR8i >( device, depth );
			case sdw::type::ImageFormat::eRgba32u: return createShaderT< sdw::type::ImageFormat::eRgba32u >( device, depth );
			case sdw::type::ImageFormat::eRgba16u: return createShaderT< sdw::type::ImageFormat::eRgba16u >( device, depth );
			case sdw::type::ImageFormat::eRgba8u: return createShaderT< sdw::type::ImageFormat::eRgba8u >( device, depth );
			case sdw::type::ImageFormat::eRg32u: return createShaderT< sdw::type::ImageFormat::eRg32u >( device, depth );
			case sdw::type::ImageFormat::eRg16u: return createShaderT< sdw::type::ImageFormat::eRg16u >( device, depth );
			case sdw::type::ImageFormat::eRg8u: return createShaderT< sdw::type::ImageFormat::eRg8u >( device, depth );
			case sdw::type::ImageFormat::eR32u: return createShaderT< sdw::type::ImageFormat::eR32u >( device, depth );
			case sdw::type::ImageFormat::eR16u: return createShaderT< sdw::type::ImageFormat::eR16u >( device, depth );
			case sdw::type::ImageFormat::eR8u: return createShaderT< sdw::type::ImageFormat::eR8u >( device, depth );
			case sdw::type::ImageFormat::eRgb10A2u: return createShaderT< sdw::type::ImageFormat::eRgb10A2u >( device, depth );
			case sdw::type::ImageFormat::eRgba16Snorm: return createShaderT< sdw::type::ImageFormat::eRgba16Snorm >( device, depth );
			case sdw::type::ImageFormat::eRgba8Snorm: return createShaderT< sdw::type::ImageFormat::eRgba8Snorm >( device, depth );
			case sdw::type::ImageFormat::eRg16Snorm: return createShaderT< sdw::type::ImageFormat::eRg16Snorm >( device, depth );
			case sdw::type::ImageFormat::eRg8Snorm: return createShaderT< sdw::type::ImageFormat::eRg8Snorm >( device, depth );
			case sdw::type::ImageFormat::eR16Snorm: return createShaderT< sdw::type::ImageFormat::eR16Snorm >( device, depth );
			case sdw::type::ImageFormat::eR8Snorm: return createShaderT< sdw::type::ImageFormat::eR8Snorm >( device, depth );
			case sdw::type::ImageFormat::eRgba16Unorm: return createShaderT< sdw::type::ImageFormat::eRgba16Unorm >( device, depth );
			case sdw::type::ImageFormat::eRgba8Unorm: return createShaderT< sdw::type::ImageFormat::eRgba8Unorm >( device, depth );
			case sdw::type::ImageFormat::eRg16Unorm: return createShaderT< sdw::type::ImageFormat::eRg16Unorm >( device, depth );
			case sdw::type::ImageFormat::eRg8Unorm: return createShaderT< sdw::type::ImageFormat::eRg8Unorm >( device, depth );
			case sdw::type::ImageFormat::eR16Unorm: return createShaderT< sdw::type::ImageFormat::eR16Unorm >( device, depth );
			case sdw::type::ImageFormat::eR8Unorm: return createShaderT< sdw::type::ImageFormat::eR8Unorm >( device, depth );
			case sdw::type::ImageFormat::eRgb10A2Unorm: return createShaderT< sdw::type::ImageFormat::eRgb10A2Unorm >( device, depth );
			default:
				CU_Failure( "Unsupported ImageFormat" );
				return nullptr;
			}
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
				, sdw::type::ImageFormat imageFormat
				, crg::cp::Config config )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "Blur/X" ), createShaderU32( device, imageFormat ) } }
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
				, sdw::type::ImageFormat imageFormat
				, bool depth
				, crg::cp::Config config )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, String{ cuT( "Blur/" ) } + ( depth ? cuT( "Z" ) : cuT( "Y" ) ), createShader( device, depth, imageFormat ) } }
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
		auto imageFormat = getImageFormat( finalFroxelsLighting.getFormat() );
		auto const & scene = froxels.getScene();
		{
			auto & pass = graph.createPass( "Blur/X"
				, [&device, &froxels, &scene, imageFormat]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto runPass = makeRawUnique< finfxl::FramePassU32 >( framePass, context, runGraph
						, device, imageFormat
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
				, [&device, &froxels, &scene, imageFormat]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto runPass = makeRawUnique< finfxl::FramePass >( framePass, context, runGraph
						, device, imageFormat, false
						, crg::cp::Config{}
							.groupCountX( froxels.getDimensions()->x )
							.groupCountY( froxels.getDimensions()->z )
							.isEnabled( crg::RunnablePass::IsEnabledCallback( [&scene](){ return scene.hasClusteredLights(); } ) ) );
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
				, [&device, &froxels, &scene, imageFormat]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto runPass = makeRawUnique< finfxl::FramePass >( framePass, context, runGraph
						, device, imageFormat, true
						, crg::cp::Config{}
							.groupCountX( froxels.getDimensions()->x )
							.groupCountY( froxels.getDimensions()->y )
							.isEnabled( crg::RunnablePass::IsEnabledCallback( [&scene](){ return scene.hasClusteredLights(); } ) ) );
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
