#include "FFTOceanRendering/OceanFFT.hpp"

#include "FFTOceanRendering/BakeHeightGradientPass.hpp"
#include "FFTOceanRendering/GenerateHeightmapPass.hpp"
#include "FFTOceanRendering/OceanFFTRenderPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/GlslToSpv.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>

#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>

#include <RenderGraph/RunnablePasses/BufferToImageCopy.hpp>

#include <ShaderWriter/Source.hpp>

namespace ocean_fft
{
	//*********************************************************************************************

	namespace
	{
		crg::FramePass const & createGenerateMipmapsPass( castor::String const & name
			, castor3d::RenderDevice const & device
			, crg::FramePassGroup & graph
			, crg::FramePass const * previousPass
			, crg::ImageViewId imageView
			, std::shared_ptr< castor3d::IsRenderPassEnabled > isEnabled )
		{
			auto & result = graph.createPass( "GenMips" + name
				, [&device, isEnabled]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
						auto res = std::make_unique< crg::GenerateMipmaps >( framePass
							, context
							, graph
							, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
							, crg::ru::Config{}
							, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
							, crg::RunnablePass::IsEnabledCallback( [isEnabled](){ return ( *isEnabled )(); } ) );
						device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
							, res->getTimer() );
						return res;
				} );
			result.addDependency( *previousPass );
			result.addTransferInOutView( imageView );
			return result;
		}

		crg::FramePass const & createGenerateSpecMipmapsPass( castor::String const & name
			, castor3d::RenderDevice const & device
			, crg::FramePassGroup & graph
			, crg::FramePass const * previousPass
			, crg::ImageViewId imageView
			, std::shared_ptr< castor3d::IsRenderPassEnabled > isEnabled )
		{
			auto & result = graph.createPass( "GenMips" + name
				, [&device, isEnabled]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
						auto res = std::make_unique< GenerateMipmapsPass >( framePass
							, context
							, graph
							, device
							, crg::ru::Config{}
							, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
							, crg::RunnablePass::IsEnabledCallback( [isEnabled](){ return ( *isEnabled )(); } ) );
						device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
							, res->getTimer() );
						return res;
				} );
			result.addDependency( *previousPass );
			result.addTransferInOutView( imageView
				, crg::Attachment::Flag::NoTransition );
			return result;
		}

		crg::FramePass const & createCopyAndGenerateMipmapsPass( castor::String const & name
			, castor3d::RenderDevice const & device
			, crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, ashes::BufferBase const & srcBuffer
			, crg::ImageViewId dstImageView
			, std::shared_ptr< castor3d::IsRenderPassEnabled > isEnabled )
		{
			auto data = *dstImageView.data;
			data.name = data.image.data->name + "_L0";
			data.info.subresourceRange.levelCount = 1u;
			auto viewId = graph.createView( data );
			auto extent = getExtent( viewId );
			auto & copy = graph.createPass( "CopyTo" + name
				, [&device, isEnabled, extent]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto res = std::make_unique< crg::BufferToImageCopy >( framePass
						, context
						, graph
						, VkOffset3D{}
						, extent
						, crg::ru::Config{}
						, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
						, crg::RunnablePass::IsEnabledCallback( [isEnabled](){ return ( *isEnabled )(); } ) );
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, res->getTimer() );
					return res;
				} );
			copy.addDependency( previousPass );
			copy.addInputStorageBuffer( { srcBuffer, name + "FFTResult" }, 0u, 0u, ashes::WholeSize );
			copy.addTransferOutputView( dstImageView );

			auto & result = graph.createPass( "GenMips" + name
				, [&device, isEnabled]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
						auto res = std::make_unique< crg::GenerateMipmaps >( framePass
							, context
							, graph
							, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
							, crg::ru::Config{}
							, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
							, crg::RunnablePass::IsEnabledCallback( [isEnabled](){ return ( *isEnabled )(); } ) );
						device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
							, res->getTimer() );
						return res;
				} );
			result.addDependency( copy );
			result.addTransferInOutView( dstImageView );
			return result;
		}

		castor3d::Texture createTexture( castor3d::RenderDevice const & device
			, crg::ResourcesCache & resources
			, VkExtent2D heightMapSamples
			, std::string const & name
			, VkFormat format
			, VkSamplerMipmapMode mipMode )
		{
			VkExtent3D dimensions{ heightMapSamples.width, heightMapSamples.height, 1u };
			castor3d::Texture result{ device
				, resources
				, name
				, 0u
				, dimensions
				, 1u
				, ashes::getMaxMipCount( dimensions ) - 2u
				, format
				, ( VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_STORAGE_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT )
				, VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, mipMode
				, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
				, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK };
			result.create();
			return result;
		}

		OceanFFT::Config getConfig( castor3d::Engine const & engine )
		{
			castor3d::initialiseGlslang();
			OceanFFT::Config config{};
			auto params = engine.getRenderPassTypeConfiguration( OceanRenderPass::Type );
			params.get( OceanRenderPass::ParamFFT, config );
			return config;
		}

		std::default_random_engine createRandomEngine( bool disableRandomSeed )
		{
			if ( disableRandomSeed )
			{
				return std::default_random_engine{};
			}

			std::random_device r;
			return std::default_random_engine{ r() };
		}
	}

	//************************************************************************************************

	castor::String const OceanFFT::Name{ cuT( "OceanFFT" ) };

	OceanFFT::OceanFFT( castor3d::RenderDevice const & device
			, crg::ResourcesCache & resources
			, crg::FramePassGroup & graph
			, crg::FramePassArray previousPasses
			, OceanUbo const & ubo
			, std::shared_ptr< castor3d::IsRenderPassEnabled > isEnabled )
		: m_device{ device }
		, m_group{ graph }
		, m_config{ ocean_fft::getConfig( *device.renderSystem.getEngine() ) }
		, m_engine{ createRandomEngine( m_config.disableRandomSeed ) }
		, m_heightMapSamples{ m_config.heightMapSamples, m_config.heightMapSamples }
		, m_displacementDownsample{ m_config.displacementDownsample }
		, m_fftConfig{ device, m_heightMapSamples }
		, m_heightSeeds{ castor3d::makeBuffer< cfloat >( device
			, m_heightMapSamples.width * m_heightMapSamples.height
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, Name + "HeightSeeds" ) }
		, m_heightDistribution{ castor3d::makeBuffer< cfloat >( device
			, m_heightMapSamples.width * m_heightMapSamples.height
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, Name + "HeightDistribution" ) }
		, m_generateHeightDistribution{ &createGenerateDistributionPass( Name
			, "Height"
			, device
			, m_group
			, previousPasses
			, m_heightMapSamples
			, false
			, ubo
			, m_heightSeeds->getBuffer()
			, m_heightDistribution->getBuffer()
			, isEnabled ) }
		, m_height{ Name
			, "Height"
			, m_group
			, { m_generateHeightDistribution }
			, ubo
			, m_heightMapSamples
			, m_fftConfig
			, *m_heightDistribution
			, FFTMode::eC2R
			, isEnabled }
		, m_displacementDistribution{ castor3d::makeBuffer< cfloat >( device
			, ( m_heightMapSamples.width >> m_displacementDownsample ) * ( m_heightMapSamples.height >> m_displacementDownsample )
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, Name + "NormalsDistribution" ) }
		, m_generateDisplacementDistribution{ &createDownsampleDistributionPass( Name
			, "Displacement"
			, device
			, m_group
			, { m_generateHeightDistribution }
			, m_heightMapSamples
			, m_displacementDownsample
			, ubo
			, m_heightDistribution->getBuffer()
			, m_displacementDistribution->getBuffer()
			, isEnabled ) }
		, m_displacement{ Name
			, "Displacement"
			, m_group
			, { m_generateDisplacementDistribution }
			, ubo
			, { m_heightMapSamples.width >> m_displacementDownsample, m_heightMapSamples.height >> m_displacementDownsample }
			, m_fftConfig
			, *m_displacementDistribution
			, FFTMode::eC2C
			, isEnabled }
		, m_heightDisplacement{ createTexture( device
				, resources
				, m_heightMapSamples
				, "OceanFFTHeightDisplacement0"
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, VK_SAMPLER_MIPMAP_MODE_NEAREST )
			, createTexture( device
				, resources
				, m_heightMapSamples
				, "OceanFFTHeightDisplacement1"
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, VK_SAMPLER_MIPMAP_MODE_NEAREST ) }
		, m_gradientJacobian{ createTexture( device
				, resources
				, m_heightMapSamples
				, "OceanFFTGradientJacobian0"
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, VK_SAMPLER_MIPMAP_MODE_LINEAR )
			, createTexture( device
				, resources
				, m_heightMapSamples
				, "OceanFFTGradientJacobian1"
				, VK_FORMAT_R16G16B16A16_SFLOAT
				, VK_SAMPLER_MIPMAP_MODE_LINEAR ) }
		, m_bakeHeightGradient{ &createBakeHeightGradientPass( m_fftConfig.device
			, m_group
			, { &m_height.getLastPass(), &m_displacement.getLastPass() }
			, m_heightMapSamples
			, m_config.size
			, m_displacementDownsample
			, ubo
			, m_height.getResult()
			, m_displacement.getResult()
			, m_heightDisplacement
			, m_gradientJacobian
			, isEnabled ) }
		, m_generateHeightDispMips{ &createGenerateSpecMipmapsPass( "HeightDisplacement"
			, device
			, m_group
			, m_bakeHeightGradient
			, m_heightDisplacement.front().sampledViewId
			, isEnabled ) }
		, m_generateGradJacobMips{ &createGenerateMipmapsPass( "GradientJacobian"
			, device
			, m_group
			, m_bakeHeightGradient
			, m_gradientJacobian.front().sampledViewId
			, isEnabled ) }
		, m_normalSeeds{ castor3d::makeBuffer< cfloat >( device
			, m_heightMapSamples.width * m_heightMapSamples.height
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, Name + "NormalsSeeds" ) }
		, m_normalDistribution{ castor3d::makeBuffer< cfloat >( device
			, m_heightMapSamples.width * m_heightMapSamples.height
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, Name + "NormalsDistribution" ) }
		, m_generateNormalDistribution{ &createGenerateDistributionPass( Name
			, "Normals"
			, device
			, m_group
			, previousPasses
			, m_heightMapSamples
			, true
			, ubo
			, m_normalSeeds->getBuffer()
			, m_normalDistribution->getBuffer()
			, isEnabled ) }
		, m_normal{ Name
			, "Normals"
			, m_group
			, { m_generateNormalDistribution }
			, ubo
			, m_heightMapSamples
			, m_fftConfig
			, *m_normalDistribution
			, FFTMode::eC2C
			, isEnabled }
		, m_normals{ createTexture( device
			, resources
			, m_heightMapSamples
			, "OceanFFTNormals"
			, VK_FORMAT_R32G32_SFLOAT
			, VK_SAMPLER_MIPMAP_MODE_LINEAR ) }
		, m_generateNormalsMips{ &createCopyAndGenerateMipmapsPass( "Normals"
			, device
			, m_group
			, m_normal.getLastPass()
			, m_normal.getResult()
			, m_normals.sampledViewId
			, isEnabled ) }
	{
		generateDistributionSeeds( *m_heightSeeds );
		generateDistributionSeeds( *m_normalSeeds );
	}

	OceanFFT::~OceanFFT()
	{
		for ( auto & tex : m_heightDisplacement )
		{
			tex.destroy();
		}

		for ( auto & tex : m_gradientJacobian )
		{
			tex.destroy();
		}

		m_normals.destroy();
	}

	void OceanFFT::accept( castor3d::RenderTechniqueVisitor & visitor )
	{
		visitor.visit( cuT( "Tile XZ size" )
			, m_config.size
			, nullptr );
		visitor.visit( cuT( "Amplitude" )
			, m_config.amplitude
			, nullptr );
		visitor.visit( cuT( "Wind XZ direction" )
			, m_config.windDirection
			, nullptr );
		visitor.visit( cuT( "Wind velocity" )
			, m_config.windVelocity
			, nullptr );
		visitor.visit( cuT( "Normal Freq. Mod." )
			, m_config.normalFreqMod
			, nullptr );
		visitor.visit( cuT( "Patch XZ size" )
			, m_config.patchSize
			, nullptr );
		visitor.visit( cuT( "Blocks XZ Count" )
			, m_config.blocksCount
			, nullptr );
		visitor.visit( cuT( "LOD 0 Distance" )
			, m_config.lod0Distance
			, nullptr );
	}

	crg::FramePassArray OceanFFT::getLastPasses()
	{
		return { m_generateHeightDispMips, m_generateGradJacobMips, m_generateNormalsMips };
	}

	void OceanFFT::generateDistributionSeeds( ashes::Buffer< cfloat > & distribBuffer )
	{
		auto Nx = int32_t( m_heightMapSamples.width );
		auto Nz = int32_t( m_heightMapSamples.height );
		std::vector< cfloat > distribution;
		distribution.resize( distribBuffer.getCount() );

		for ( int32_t z = 0; z < Nz; z++ )
		{
			for ( int32_t x = 0; x < Nx; x++ )
			{
				distribution[size_t( z * Nx + x )] = cfloat{ m_normDis( m_engine ), m_normDis( m_engine ) };
			}
		}

		ashes::StagingBuffer staging{ *m_fftConfig.device
			, "OceanFFTDistributionSeedsStaging"
			, VkBufferUsageFlags{}
			, distribution.size() * sizeof( cfloat ) };
		auto queueData = m_device.graphicsData();
		staging.uploadBufferData( *queueData->queue
			, *queueData->commandPool
			, distribution
			, distribBuffer );
	}

	//************************************************************************************************
}
