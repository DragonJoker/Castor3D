/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_OceanFFT_H___
#define ___C3DORFFT_OceanFFT_H___

#include "DownsampleDistributionPass.hpp"
#include "GenerateDisplacementPass.hpp"
#include "GenerateDistributionPass.hpp"
#include "GenerateHeightmapPass.hpp"
#include "GenerateMipmapsPass.hpp"
#include "GenerateNormalPass.hpp"
#include "OceanFFTUbo.hpp"
#include "ProcessFFTPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Math/Angle.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>

#include <random>
#include <complex>

namespace ocean_fft
{
	template< typename GeneratePassT >
	crg::FramePass const & createGenerateFrequencyPassT( c3d::String const & prefix
		, c3d::String const & name
		, c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, c3d::Extent2D const & extent
		, OceanUbo const & ubo
		, ashes::BufferBase const & input
		, ashes::BufferBase const & output )
	{
		auto mbName = c3d::toUtf8( name );
		auto & result = graph.createPass( "GenerateFrequency" + mbName
			, [&device, extent]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = c3d::makeRawUnique< GeneratePassT >( framePass
					, context
					, runnableGraph
					, device
					, extent
					, crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		ubo.createPassBinding( result
			, GeneratePassT::eConfig );
		result.addInputStorageBuffer( { input, mbName + "Distribution" }
			, GeneratePassT::eInput
			, 0u
			, input.getSize() );
		result.addOutputStorageBuffer( { output, mbName + "Frequency" }
			, GeneratePassT::eOutput
			, 0u
			, output.getSize() );
		return result;
	}

	template< typename DistributionPassT, typename FrequencyPassT >
	struct GenerateFFTPassT
	{
		GenerateFFTPassT( c3d::String const & prefix
			, c3d::String const & name
			, crg::FramePassGroup & graph
			, crg::FramePassArray previousPasses
			, OceanUbo const & ubo
			, c3d::Extent2D dimensions
			, VkFFTConfig const & pfftConfig
			, ashes::Buffer< cfloat > const & distribution
			, FFTMode mode )
			: fftConfig{ pfftConfig }
			, frequency{ c3d::makeBuffer< cfloat >( fftConfig.device
					, dimensions.width * dimensions.height
					, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					, prefix + name + cuT( "Frequency" ) ) }
			, result{ c3d::makeBufferBase( pfftConfig.device
					, sizeof( cfloat ) * dimensions.width * dimensions.height
					, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					, prefix + name + cuT( "Result0" ) )
				, c3d::makeBufferBase( pfftConfig.device
					, sizeof( cfloat ) * dimensions.width * dimensions.height
					, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					, prefix + name + cuT( "Result1" ) ) }
			, generateFrequency{ &createGenerateFrequencyPassT< FrequencyPassT >( prefix
				, name
				, fftConfig.device
				, graph
				, previousPasses
				, dimensions
				, ubo
				, distribution.getBuffer()
				, frequency->getBuffer() ) }
			, processFFT( &createProcessFFTPass( name
				, fftConfig.device
				, graph
				, *generateFrequency
				, dimensions
				, fftConfig
				, frequency->getBuffer()
				, result ) )
		{
		}

		ashes::BufferBase const & getResult()
		{
			return *result[0];
		}

		crg::FramePass const & getLastPass()
		{
			return *processFFT;
		}

	private:
		VkFFTConfig const & fftConfig;
		ashes::BufferPtr< cfloat > frequency;
		c3d::Array< ashes::BufferBasePtr, 2u > result;
		crg::FramePass const * generateFrequency{};
		crg::FramePass const * processFFT{};
	};

	class OceanFFT
	{
	public:
		using Config = OceanFFTConfig;

	public:
		OceanFFT( c3d::RenderDevice const & device
			, crg::ResourcesCache & resources
			, crg::FramePassGroup & graph
			, crg::FramePassArray previousPasses
			, OceanUbo const & ubo
			, OceanFFTConfig const & config );
		~OceanFFT();
		/**
		 *\copydoc		c3d::RenderTechniquePass::accept
		 */
		void accept( c3d::ConfigurationVisitorBase & visitor );

		Config const & getConfig()const
		{
			return m_config;
		}

		c3d::Texture const & getNormals()const
		{
			return m_normals;
		}

		c3d::Texture const & getHeightDisplacement()const
		{
			return m_heightDisplacement.front();
		}

		c3d::Texture const & getGradientJacobian()const
		{
			return m_gradientJacobian.front();
		}

		crg::FramePassArray getLastPasses();

	public:
		static c3d::String const Name;

	private:
		void generateDistributionSeeds( ashes::Buffer< cfloat > & distribBuffer );

	private:
		c3d::RenderDevice const & m_device;
		crg::FramePassGroup & m_group;
		OceanFFTConfig m_config;
		std::default_random_engine m_engine;
		std::normal_distribution< float > m_normDis{ 0.0f, 1.0f };
		c3d::Extent2D m_heightMapSamples{ 2u, 2u };
		uint32_t m_displacementDownsample{ 1u };
		VkFFTConfig m_fftConfig;
		ashes::BufferPtr< cfloat > m_heightSeeds;
		ashes::BufferPtr< cfloat > m_heightDistribution;
		crg::FramePass const * m_generateHeightDistribution{};
		GenerateFFTPassT< GenerateDistributionPass, GenerateHeightmapPass > m_height;
		ashes::BufferPtr< cfloat > m_displacementDistribution;
		crg::FramePass const * m_generateDisplacementDistribution{};
		GenerateFFTPassT< GenerateDistributionPass, GenerateDisplacementPass > m_displacement;
		c3d::Array< c3d::Texture, 2u > m_heightDisplacement;
		c3d::Array< c3d::Texture, 2u > m_gradientJacobian;
		crg::FramePass const * m_bakeHeightGradient{};
		crg::FramePass const * m_generateHeightDispMips{};
		crg::FramePass const * m_generateGradJacobMips{};
		ashes::BufferPtr< cfloat > m_normalSeeds;
		ashes::BufferPtr< cfloat > m_normalDistribution;
		crg::FramePass const * m_generateNormalDistribution{};
		GenerateFFTPassT< GenerateDistributionPass, GenerateNormalPass > m_normal;
		c3d::Texture m_normals;
		crg::FramePass const * m_generateNormalsMips{};
	};
}

#endif
