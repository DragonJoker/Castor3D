#include "FFTOceanRendering/OceanFFT.hpp"

#include "FFTOceanRendering/BakeHeightGradientPass.hpp"
#include "FFTOceanRendering/GenerateHeightmapPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/DirectUploadData.hpp>
#include <Castor3D/Buffer/InstantUploadData.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/GlslToSpv.hpp>
#include <Castor3D/Shader/ShaderModule.hpp>

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <RenderGraph/RunnablePasses/BufferToImageCopy.hpp>

#include <ShaderWriter/Source.hpp>

namespace ocean_fft
{
	//*********************************************************************************************

	namespace
	{
		crg::FramePass const & createGenerateMipmapsPass( c3d::String const & name
			, c3d::RenderDevice const & device
			, crg::FramePassGroup & graph
			, crg::FramePass const * previousPass
			, crg::ImageViewId imageView )
		{
			auto & result = graph.createPass( "GenMips" + c3d::toUtf8( name )
				, [&device]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
						auto res = c3d::makeRawUnique< crg::GenerateMipmaps >( framePass
							, context
							, graph
							, c3d::ImageLayout::eShaderReadOnly
							, crg::ru::Config{}
							, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
							, crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
						device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
							, res->getTimer() );
						return res;
				} );
			result.addDependency( *previousPass );
			result.addTransferInOutView( imageView );
			return result;
		}

		crg::FramePass const & createGenerateSpecMipmapsPass( c3d::String const & name
			, c3d::RenderDevice const & device
			, crg::FramePassGroup & graph
			, crg::FramePass const * previousPass
			, crg::ImageViewId imageView )
		{
			auto & result = graph.createPass( "GenMips" + c3d::toUtf8( name )
				, [&device]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
						auto res = c3d::makeRawUnique< GenerateMipmapsPass >( framePass
							, context
							, graph
							, device
							, crg::ru::Config{}
							, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
							, crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
						device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
							, res->getTimer() );
						return res;
				} );
			result.addDependency( *previousPass );
			result.addTransferInOutView( imageView
				, crg::Attachment::Flag::NoTransition );
			return result;
		}

		crg::FramePass const & createCopyAndGenerateMipmapsPass( c3d::String const & name
			, c3d::RenderDevice const & device
			, crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, ashes::BufferBase const & srcBuffer
			, crg::ImageViewId dstImageView )
		{
			auto mbName = c3d::toUtf8( name );
			auto data = *dstImageView.data;
			data.name = data.image.data->name + "_L0";
			data.info.subresourceRange.levelCount = 1u;
			auto viewId = graph.createView( data );
			auto extent = getExtent( viewId );
			auto & copy = graph.createPass( "CopyTo" + mbName
				, [&device, extent]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto res = c3d::makeRawUnique< crg::BufferToImageCopy >( framePass
						, context
						, graph
						, c3d::Offset3D{}
						, extent
						, crg::ru::Config{}
						, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
						, crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
					device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
						, res->getTimer() );
					return res;
				} );
			copy.addDependency( previousPass );
			copy.addInputStorageBuffer( { srcBuffer, mbName + "FFTResult" }, 0u, 0u, ashes::WholeSize );
			copy.addTransferOutputView( dstImageView );

			auto & result = graph.createPass( "GenMips" + mbName
				, [&device]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
						auto res = c3d::makeRawUnique< crg::GenerateMipmaps >( framePass
							, context
							, graph
							, c3d::ImageLayout::eShaderReadOnly
							, crg::ru::Config{}
							, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
							, crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
						device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
							, res->getTimer() );
						return res;
				} );
			result.addDependency( copy );
			result.addTransferInOutView( dstImageView );
			return result;
		}

		c3d::Texture createTexture( c3d::RenderDevice const & device
			, crg::ResourcesCache & resources
			, c3d::Extent2D heightMapSamples
			, c3d::String const & name
			, c3d::PixelFormat format
			, c3d::MipmapMode mipMode )
		{
			c3d::Extent3D dimensions{ heightMapSamples.width, heightMapSamples.height, 1u };
			c3d::Texture result{ device
				, resources
				, name
				, { c3d::ImageCreateFlags::eNone
					, dimensions, 1u, ashes::getMaxMipCount( convert( dimensions ) ) - 2u
					, format
					, ( c3d::ImageUsageFlags::eSampled
						| c3d::ImageUsageFlags::eStorage
						| c3d::ImageUsageFlags::eTransferDst ) }
				, { { .borderColor = c3d::BorderColour::eFloatTransparentBlack
					, .addressMode = c3d::WrapMode::eClampToEdge
					, .mipFilter = mipMode } } };
			result.create();
			return result;
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

	c3d::String const OceanFFT::Name{ cuT( "OceanFFT" ) };

	OceanFFT::OceanFFT( c3d::RenderDevice const & device
			, crg::ResourcesCache & resources
			, crg::FramePassGroup & graph
			, crg::FramePassArray previousPasses
			, OceanUbo const & ubo
			, OceanFFTConfig const & config )
		: m_device{ device }
		, m_group{ graph }
		, m_config{ config }
		, m_engine{ createRandomEngine( m_config.disableRandomSeed ) }
		, m_heightMapSamples{ m_config.heightMapSamples, m_config.heightMapSamples }
		, m_displacementDownsample{ m_config.displacementDownsample }
		, m_fftConfig{ device, m_heightMapSamples }
		, m_heightSeeds{ c3d::makeBuffer< cfloat >( device
			, m_heightMapSamples.width * m_heightMapSamples.height
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, Name + cuT( "HeightSeeds" ) ) }
		, m_heightDistribution{ c3d::makeBuffer< cfloat >( device
			, m_heightMapSamples.width * m_heightMapSamples.height
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, Name + cuT( "HeightDistribution" ) ) }
		, m_generateHeightDistribution{ &createGenerateDistributionPass( Name
			, cuT( "Height" )
			, device
			, m_group
			, previousPasses
			, m_heightMapSamples
			, false
			, ubo
			, m_heightSeeds->getBuffer()
			, m_heightDistribution->getBuffer() ) }
		, m_height{ Name
			, cuT( "Height" )
			, m_group
			, { m_generateHeightDistribution }
			, ubo
			, m_heightMapSamples
			, m_fftConfig
			, *m_heightDistribution
			, FFTMode::eC2R }
		, m_displacementDistribution{ c3d::makeBuffer< cfloat >( device
			, ( m_heightMapSamples.width >> m_displacementDownsample ) * ( m_heightMapSamples.height >> m_displacementDownsample )
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, Name + cuT( "NormalsDistribution" ) ) }
		, m_generateDisplacementDistribution{ &createDownsampleDistributionPass( Name
			, cuT( "Displacement" )
			, device
			, m_group
			, { m_generateHeightDistribution }
			, m_heightMapSamples
			, m_displacementDownsample
			, ubo
			, m_heightDistribution->getBuffer()
			, m_displacementDistribution->getBuffer() ) }
		, m_displacement{ Name
			, cuT( "Displacement" )
			, m_group
			, { m_generateDisplacementDistribution }
			, ubo
			, { m_heightMapSamples.width >> m_displacementDownsample, m_heightMapSamples.height >> m_displacementDownsample }
			, m_fftConfig
			, *m_displacementDistribution
			, FFTMode::eC2C }
		, m_heightDisplacement{ createTexture( device
				, resources
				, m_heightMapSamples
				, cuT( "OceanFFTHeightDisplacement0" )
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::MipmapMode::eNearest )
			, createTexture( device
				, resources
				, m_heightMapSamples
				, cuT( "OceanFFTHeightDisplacement1" )
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::MipmapMode::eNearest ) }
		, m_gradientJacobian{ createTexture( device
				, resources
				, m_heightMapSamples
				, cuT( "OceanFFTGradientJacobian0" )
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::MipmapMode::eLinear )
			, createTexture( device
				, resources
				, m_heightMapSamples
				, cuT( "OceanFFTGradientJacobian1" )
				, c3d::PixelFormat::eR16G16B16A16_SFLOAT
				, c3d::MipmapMode::eLinear ) }
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
			, m_gradientJacobian ) }
		, m_generateHeightDispMips{ &createGenerateSpecMipmapsPass( cuT( "HeightDisplacement" )
			, device
			, m_group
			, m_bakeHeightGradient
			, m_heightDisplacement.front().sampledViewId ) }
		, m_generateGradJacobMips{ &createGenerateMipmapsPass( cuT( "GradientJacobian" )
			, device
			, m_group
			, m_bakeHeightGradient
			, m_gradientJacobian.front().sampledViewId ) }
		, m_normalSeeds{ c3d::makeBuffer< cfloat >( device
			, m_heightMapSamples.width * m_heightMapSamples.height
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, Name + cuT( "NormalsSeeds" ) ) }
		, m_normalDistribution{ c3d::makeBuffer< cfloat >( device
			, m_heightMapSamples.width * m_heightMapSamples.height
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, Name + cuT( "NormalsDistribution" ) ) }
		, m_generateNormalDistribution{ &createGenerateDistributionPass( Name
			, cuT( "Normals" )
			, device
			, m_group
			, previousPasses
			, m_heightMapSamples
			, true
			, ubo
			, m_normalSeeds->getBuffer()
			, m_normalDistribution->getBuffer() ) }
		, m_normal{ Name
			, cuT( "Normals" )
			, m_group
			, { m_generateNormalDistribution }
			, ubo
			, m_heightMapSamples
			, m_fftConfig
			, *m_normalDistribution
			, FFTMode::eC2C }
		, m_normals{ createTexture( device
			, resources
			, m_heightMapSamples
			, cuT( "OceanFFTNormals" )
			, c3d::PixelFormat::eR32G32_SFLOAT
			, c3d::MipmapMode::eLinear ) }
		, m_generateNormalsMips{ &createCopyAndGenerateMipmapsPass( cuT( "Normals" )
			, device
			, m_group
			, m_normal.getLastPass()
			, m_normal.getResult()
			, m_normals.sampledViewId ) }
	{
		generateDistributionSeeds( *m_heightSeeds );
		generateDistributionSeeds( *m_normalSeeds );

		m_group.addGroupOutput( m_gradientJacobian.front().sampledViewId );
		m_group.addGroupOutput( m_gradientJacobian.back().sampledViewId );
		m_group.addGroupOutput( m_heightDisplacement.back().sampledViewId );
		m_group.addGroupOutput( m_heightDisplacement.back().sampledViewId );
		m_group.addGroupOutput( m_normals.sampledViewId );
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

	void OceanFFT::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( cuT( "Tile XZ size" ), m_config.size );
		visitor.visit( cuT( "Amplitude" ), m_config.amplitude );
		visitor.visit( cuT( "Wind XZ direction" ), m_config.windDirection );
		visitor.visit( cuT( "Wind velocity" ), m_config.windVelocity );
		visitor.visit( cuT( "Normal Freq. Mod." ), m_config.normalFreqMod );
		visitor.visit( cuT( "Patch XZ size" ), m_config.patchSize );
		visitor.visit( cuT( "Blocks XZ Count" ), m_config.blocksCount );
		visitor.visit( cuT( "LOD 0 Distance" ), m_config.lod0Distance );
	}

	crg::FramePassArray OceanFFT::getLastPasses()
	{
		return { m_generateHeightDispMips, m_generateGradJacobMips, m_generateNormalsMips };
	}

	void OceanFFT::generateDistributionSeeds( ashes::Buffer< cfloat > & distribBuffer )
	{
		auto Nx = int32_t( m_heightMapSamples.width );
		auto Nz = int32_t( m_heightMapSamples.height );
		c3d::Vector< cfloat > distribution;
		distribution.resize( distribBuffer.getCount() );

		for ( int32_t z = 0; z < Nz; z++ )
		{
			for ( int32_t x = 0; x < Nx; x++ )
			{
				distribution[size_t( z * Nx + x )] = cfloat{ m_normDis( m_engine ), m_normDis( m_engine ) };
			}
		}
		{
			auto queueData = m_device.graphicsData();
			c3d::InstantDirectUploadData uploader{ *queueData->queue
				, m_device
				, cuT( "OceanFFTDistributionSeeds" )
				, *queueData->commandPool};
			uploader->pushUpload( distribution.data()
				, distribution.size() * sizeof( cfloat )
				, distribBuffer.getBuffer()
				, 0u
				, c3d::ComputeShaderReadState );
		}
	}

	//************************************************************************************************
}
