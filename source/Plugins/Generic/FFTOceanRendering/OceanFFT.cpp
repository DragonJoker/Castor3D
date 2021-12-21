#include "FFTOceanRendering/OceanFFT.hpp"

#include "FFTOceanRendering/GenerateHeightmapPass.hpp"
#include "FFTOceanRendering/OceanFFTRenderPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
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
		int32_t alias( int32_t x, int32_t N )
		{
			if ( x > N / 2 )
				x -= N;
			return x;
		}

		crg::FramePass const & createGenerateMipmapsPass( castor::String const & name
			, castor3d::RenderDevice const & device
			, crg::FrameGraph & graph
			, crg::FramePass const * previousPass
			, crg::ImageViewId imageView )
		{
			auto & result = graph.createPass( OceanFFT::Name + "/GenMips" + name
				, [&device]( crg::FramePass const & pass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
						auto res = std::make_unique< crg::GenerateMipmaps >( pass
							, context
							, graph
							, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
						device.renderSystem.getEngine()->registerTimer( graph.getName() + "/" + pass.name
							, res->getTimer() );
						return res;
				} );
			result.addDependency( *previousPass );
			result.addTransferInOutView( imageView );
			return result;
		}

		crg::FramePass const & createGenerateSpecMipmapsPass( castor::String const & name
			, castor3d::RenderDevice const & device
			, crg::FrameGraph & graph
			, crg::FramePass const * previousPass
			, crg::ImageViewId imageView )
		{
			auto & result = graph.createPass( OceanFFT::Name + "/GenMips" + name
				, [&device]( crg::FramePass const & pass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
						auto res = std::make_unique< GenerateMipmapsPass >( pass
							, context
							, graph
							, device );
						device.renderSystem.getEngine()->registerTimer( graph.getName() + "/" + pass.name
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
			, crg::FrameGraph & graph
			, crg::FramePass const & previousPass
			, ashes::BufferBase const & srcBuffer
			, crg::ImageViewId dstImageView )
		{
			auto data = *dstImageView.data;
			data.name = data.image.data->name + "_L0";
			data.info.subresourceRange.levelCount = 1u;
			auto viewId = graph.createView( data );
			auto extent = getExtent( viewId );
			auto & copy = graph.createPass( OceanFFT::Name + "/CopyTo" + name
				, [&device, extent]( crg::FramePass const & pass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto res = std::make_unique< crg::BufferToImageCopy >( pass
						, context
						, graph
						, VkOffset3D{}
						, extent );
					device.renderSystem.getEngine()->registerTimer( graph.getName() + "/" + pass.name
						, res->getTimer() );
					return res;
				} );
			copy.addDependency( previousPass );
			copy.addInputStorageBuffer( { srcBuffer, name + "FFTResult" }, 0u, 0u, ashes::WholeSize );
			copy.addTransferOutputView( dstImageView );

			auto & result = graph.createPass( OceanFFT::Name + "/GenMips" + name
				, [&device]( crg::FramePass const & pass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
						auto res = std::make_unique< crg::GenerateMipmaps >( pass
							, context
							, graph
							, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
						device.renderSystem.getEngine()->registerTimer( graph.getName() + "/" + pass.name
							, res->getTimer() );
						return res;
				} );
			result.addDependency( copy );
			result.addTransferInOutView( dstImageView );
			return result;
		}

		castor3d::Texture createTexture( castor3d::RenderDevice const & device
			, crg::FrameGraph & graph
			, VkExtent2D heightMapSamples
			, std::string const & name
			, VkFormat format
			, VkSamplerMipmapMode mipMode )
		{
			VkExtent3D dimensions{ heightMapSamples.width, heightMapSamples.height, 1u };
			castor3d::Texture result{ device
				, graph.getHandler()
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
				, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK
				, true };
			result.create();
			return result;
		}

		OceanFFT::TrackedConfig getConfig( castor3d::Engine const & engine
			, bool & dirty )
		{
			castor3d::initialiseGlslang();
			OceanFFT::Config config{};
			auto params = engine.getRenderPassTypeConfiguration( OceanRenderPass::Type );

			if ( params.get( OceanRenderPass::ParamFFT, config ) )
			{
				return { castor::GroupChangeTracked< castor::Point2f >{ dirty, config.size }
					, castor::GroupChangeTracked< castor::Point2f >{ dirty, config.normalFreqMod }
					, castor::GroupChangeTracked< castor::Point2f >{ dirty, config.windVelocity }
					, castor::GroupChangeTracked< castor::Point2f >{ dirty, config.patchSize }
					, castor::GroupChangeTracked< castor::Point2ui >{ dirty, config.blocksCount }
					, castor::GroupChangeTracked< uint32_t >{ dirty, config.heightMapSamples }
					, castor::GroupChangeTracked< uint32_t >{ dirty, config.displacementDownsample }
					, castor::GroupChangeTracked< float >{ dirty, config.amplitude }
					, castor::GroupChangeTracked< float >{ dirty, config.maxWaveLength }
					, castor::GroupChangeTracked< float >{ dirty, config.lod0Distance }
					, castor::GroupChangeTracked< bool >{ dirty, config.disableRandomSeed } };
			}
			
			return { castor::GroupChangeTracked< castor::Point2f >{ dirty, { 200.0f, 200.0f } }
				, castor::GroupChangeTracked< castor::Point2f >{ dirty, { 7.3f, 7.3f } }
				, castor::GroupChangeTracked< castor::Point2f >{ dirty, { 1.0f, 1.0f } }
				, castor::GroupChangeTracked< castor::Point2f >{ dirty, { 32.0f, 32.0f } }
				, castor::GroupChangeTracked< castor::Point2ui >{ dirty, { 64u, 64u } }
				, castor::GroupChangeTracked< uint32_t >{ dirty, 256u }
				, castor::GroupChangeTracked< uint32_t >{ dirty, 1u }
				, castor::GroupChangeTracked< float >{ dirty, 1.0f }
				, castor::GroupChangeTracked< float >{ dirty, 1.0f }
				, castor::GroupChangeTracked< float >{ dirty, 50.0f }
				, castor::GroupChangeTracked< bool >{ dirty, false } };
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
			, crg::FrameGraph & graph
			, crg::FramePassArray previousPasses
			, OceanUbo const & ubo )
		: m_config{ ocean_fft::getConfig( *device.renderSystem.getEngine(), m_dirty ) }
		, m_engine{ createRandomEngine( *m_config.disableRandomSeed ) }
		, m_heightMapSamples{ m_config.heightMapSamples.value(), m_config.heightMapSamples.value() }
		, m_displacementDownsample{ m_config.displacementDownsample.value() }
		, m_fftConfig{ device, m_heightMapSamples }
		, m_height{ Name
			, "Height"
			, graph
			, previousPasses
			, ubo
			, m_heightMapSamples
			, m_fftConfig
			, FFTMode::eC2R }
		, m_normal{ Name
			, "Normals"
			, graph
			, previousPasses
			, ubo
			, m_heightMapSamples
			, m_fftConfig
			, FFTMode::eC2C }
		, m_displacement{ Name
			, "Displacement"
			, graph
			, previousPasses
			, ubo
			, { m_heightMapSamples.width >> m_displacementDownsample, m_heightMapSamples.height >> m_displacementDownsample }
			, m_fftConfig
			, FFTMode::eC2C }
		, m_heightDisplacement{ createTexture( device, graph, m_heightMapSamples, "OceanFFTHeightDisplacement0", VK_FORMAT_R16G16B16A16_SFLOAT, VK_SAMPLER_MIPMAP_MODE_NEAREST )
			, createTexture( device, graph, m_heightMapSamples, "OceanFFTHeightDisplacement1", VK_FORMAT_R16G16B16A16_SFLOAT, VK_SAMPLER_MIPMAP_MODE_NEAREST ) }
		, m_gradientJacobian{ createTexture( device, graph, m_heightMapSamples, "OceanFFTGradientJacobian0", VK_FORMAT_R16G16B16A16_SFLOAT, VK_SAMPLER_MIPMAP_MODE_LINEAR )
			, createTexture( device, graph, m_heightMapSamples, "OceanFFTGradientJacobian1", VK_FORMAT_R16G16B16A16_SFLOAT, VK_SAMPLER_MIPMAP_MODE_LINEAR ) }
		, m_normals{ createTexture( device, graph, m_heightMapSamples, "OceanFFTNormals", VK_FORMAT_R32G32_SFLOAT, VK_SAMPLER_MIPMAP_MODE_LINEAR ) }
		, m_bakeHeightGradient{ &createBakeHeightGradientPass( m_fftConfig.device
			, graph
			, { &m_height.getLastPass(), &m_displacement.getLastPass() }
			, m_heightMapSamples
			, m_config.size.value()
			, m_displacementDownsample
			, ubo
			, m_height.getResult()
			, m_displacement.getResult()
			, m_heightDisplacement
			, m_gradientJacobian ) }
		, m_generateHeightDispMips{ &createGenerateSpecMipmapsPass( "HeightDisplacement"
			, device
			, graph
			, m_bakeHeightGradient
			, m_heightDisplacement.front().sampledViewId ) }
		, m_generateGradJacobMips{ &createGenerateMipmapsPass( "GradientJacobian"
			, device
			, graph
			, m_bakeHeightGradient
			, m_gradientJacobian.front().sampledViewId ) }
		, m_generateNormalsMips{ &createCopyAndGenerateMipmapsPass( "Normals"
			, device
			, graph
			, m_normal.getLastPass()
			, m_normal.getResult()
			, m_normals.sampledViewId ) }
	{
		m_dirty = true;
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
			, m_config.size );
		visitor.visit( cuT( "Amplitude" )
			, m_config.amplitude );
		visitor.visit( cuT( "Max. wave length" )
			, m_config.maxWaveLength );
		visitor.visit( cuT( "Wind XZ velocity" )
			, m_config.windVelocity );
		visitor.visit( cuT( "Normal Freq. Mod." )
			, m_config.normalFreqMod );

		visitor.visit( cuT( "Patch XZ size" )
			, *m_config.patchSize
			, nullptr );
		visitor.visit( cuT( "Blocks XZ Count" )
			, *m_config.blocksCount
			, nullptr );
		visitor.visit( cuT( "LOD 0 Distance" )
			, *m_config.lod0Distance
			, nullptr );
	}

	void OceanFFT::update()
	{
		if ( m_dirty )
		{
			static constexpr float G = 9.81f;
			L = castor::point::dot( m_config.windVelocity.value(), m_config.windVelocity.value() ) / G;
			auto amplitude = float( m_config.amplitude.value() * 0.3f / sqrt( m_heightMapSamples.width * m_heightMapSamples.height ) );
			auto height = generateDistribution( amplitude
				, m_config.maxWaveLength.value()
				, m_config.size
				, m_height.getDistribution() );
			generateDistribution( amplitude * sqrt( m_config.normalFreqMod.value()->x * m_config.normalFreqMod.value()->y )
				, m_config.maxWaveLength.value()
				, m_config.size.value() / m_config.normalFreqMod.value()
				, m_normal.getDistribution() );
			downsampleDistribution( height
				, m_displacementDownsample
				, m_displacement.getDistribution() );
			m_dirty = false;
		}
	}

	crg::FramePassArray OceanFFT::getLastPasses()
	{
		return { m_generateHeightDispMips, m_generateGradJacobMips, m_generateNormalsMips };
	}

	cfloat OceanFFT::phillips( castor::Point2f const & k
		, float maxWaveLength
		, castor::Point2f const & windDirection )
	{
		auto kLen = float( castor::point::length( k ) );

		if ( kLen == 0.0f )
		{
			return 0.0f;
		}

		float kL = kLen * L;
		auto kDir = castor::point::getNormalised( k );
		float kw = castor::point::dot( kDir, windDirection );
		return float( pow( kw * kw, 1.0f )								// Directional
			* exp( -1.0 * kLen * kLen * maxWaveLength * maxWaveLength )	// Suppress small waves at ~maxWaveLength.
			* exp( -1.0f / ( kL * kL ) )
			* pow( kLen, -4.0f ) );
	}

	std::vector< cfloat > OceanFFT::generateDistribution( float amplitude
		, float maxWaveLength
		, castor::Point2f const & size
		, ashes::Buffer< cfloat > & distribBuffer )
	{
		auto Nx = int32_t( m_heightMapSamples.width );
		auto Nz = int32_t( m_heightMapSamples.height );
		auto windDirection = castor::point::getNormalised( m_config.windVelocity.value() );
		std::vector< cfloat > distribution;
		distribution.resize( distribBuffer.getCount() );
		auto mod = castor::Point2f{ castor::PiMult2< float >, castor::PiMult2< float > } / size;

		for ( int32_t z = 0; z < Nz; z++ )
		{
			for ( int32_t x = 0; x < Nx; x++ )
			{
				auto & v = distribution[size_t( z * Nx + x )];
				auto k = mod * castor::Point2f{ alias( x, Nx ), alias( z, Nz ) };
				auto dist = cfloat{ m_normDis( m_engine ), m_normDis( m_engine ) };
				v = dist * amplitude * sqrt( 0.5f * phillips( k, maxWaveLength, windDirection ) );
			}
		}

		ashes::StagingBuffer staging{ *m_fftConfig.device
			, "OceanFFTDistributionStaging"
			, VkBufferUsageFlags{}
			, distribution.size() * sizeof( cfloat ) };
		staging.uploadBufferData( *m_fftConfig.queueData.queue
			, *m_fftConfig.queueData.commandPool
			, distribution
			, distribBuffer );
		return distribution;
	}

	void OceanFFT::downsampleDistribution( std::vector< cfloat > const & distribution
		, uint32_t downsample
		, ashes::Buffer< cfloat > & distribBuffer )
	{
		auto Nx = int32_t( m_heightMapSamples.width );
		auto Nz = int32_t( m_heightMapSamples.height );
		std::vector< cfloat > downsampled;
		downsampled.resize( distribBuffer.getCount() );
		// Pick out the lower frequency samples only which is the same as downsampling "perfectly".
		auto outWidth = Nx >> downsample;
		auto outHeight = Nz >> downsample;

		for ( int32_t z = 0; z < outHeight; z++ )
		{
			for ( int32_t x = 0; x < outWidth; x++ )
			{
				auto alias_x = alias( x, outWidth );
				auto alias_z = alias( z, outHeight );

				if ( alias_x < 0 )
				{
					alias_x += Nx;
				}

				if ( alias_z < 0 )
				{
					alias_z += Nz;
				}

				downsampled[size_t( z * outWidth + x )] = distribution[size_t( alias_z * Nx + alias_x )];
			}
		}

		ashes::StagingBuffer staging{ *m_fftConfig.device
			, "OceanFFTDownsampleStaging"
			, VkBufferUsageFlags{}
			, downsampled.size() * sizeof( cfloat ) };
		staging.uploadBufferData( *m_fftConfig.queueData.queue
			, *m_fftConfig.queueData.commandPool
			, downsampled
			, distribBuffer );
	}

	//************************************************************************************************
}
