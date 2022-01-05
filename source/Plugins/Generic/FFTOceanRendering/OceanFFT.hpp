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
#include <Castor3D/Buffer/UniformBufferPools.hpp>

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
	crg::FramePass const & createGenerateFrequencyPassT( castor::String const & prefix
		, castor::String const & name
		, castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, VkExtent2D const & extent
		, OceanUbo const & ubo
		, ashes::BufferBase const & input
		, ashes::BufferBase const & output
		, std::shared_ptr< IsRenderPassEnabled > isEnabled )
	{
		auto & result = graph.createPass( prefix + "/GenerateFrequency" + name
			, [&device, extent, isEnabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = std::make_unique< GeneratePassT >( framePass
					, context
					, runnableGraph
					, device
					, extent
					, crg::RunnablePass::IsEnabledCallback( [isEnabled](){ return ( *isEnabled )(); } ) );
				device.renderSystem.getEngine()->registerTimer( runnableGraph.getName() + "/" + framePass.name
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		ubo.createPassBinding( result
			, GeneratePassT::eConfig );
		result.addInputStorageBuffer( { input, name + "Distribution" }
			, GeneratePassT::eInput
			, 0u
			, input.getSize() );
		result.addOutputStorageBuffer( { output, name + "Frequency" }
			, GeneratePassT::eOutput
			, 0u
			, output.getSize() );
		return result;
	}

	template< typename DistributionPassT, typename FrequencyPassT >
	struct GenerateFFTPassT
	{
		GenerateFFTPassT( castor::String const & prefix
			, castor::String const & name
			, crg::FramePassGroup & graph
			, crg::FramePassArray previousPasses
			, OceanUbo const & ubo
			, VkExtent2D dimensions
			, FFTConfig const & pfftConfig
			, ashes::Buffer< cfloat > const & distribution
			, FFTMode mode
			, std::shared_ptr< IsRenderPassEnabled > isEnabled )
			: fftConfig{ pfftConfig }
			, frequency{ castor3d::makeBuffer< cfloat >( fftConfig.device
					, dimensions.width * dimensions.height
					, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					, prefix + name + "Frequency" ) }
			, result{ castor3d::makeBufferBase( pfftConfig.device
					, sizeof( cfloat ) * dimensions.width * dimensions.height
					, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					, prefix + name + "Result0" )
				, castor3d::makeBufferBase( pfftConfig.device
					, sizeof( cfloat ) * dimensions.width * dimensions.height
					, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					, prefix + name + "Result1" ) }
			, generateFrequency{ &createGenerateFrequencyPassT< FrequencyPassT >( prefix
				, name
				, fftConfig.device
				, graph
				, previousPasses
				, dimensions
				, ubo
				, distribution.getBuffer()
				, frequency->getBuffer()
				, isEnabled ) }
			, processFFT( &createProcessFFTPass( name
				, fftConfig.device
				, graph
				, *generateFrequency
				, dimensions
				, fftConfig
				, frequency->getBuffer()
				, result
				, isEnabled ) )
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
		FFTConfig const & fftConfig;
		ashes::BufferPtr< cfloat > frequency;
		std::array< ashes::BufferBasePtr, 2u > result;
		crg::FramePass const * generateFrequency{};
		crg::FramePass const * processFFT{};
	};

	class OceanFFT
	{
	public:
		using Config = OceanFFTConfig;

	public:
		OceanFFT( castor3d::RenderDevice const & device
			, crg::FramePassGroup & graph
			, crg::FramePassArray previousPasses
			, OceanUbo const & ubo
			, std::shared_ptr< IsRenderPassEnabled > isEnabled );
		~OceanFFT();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( castor3d::RenderTechniqueVisitor & visitor );

		Config const & getConfig()const
		{
			return m_config;
		}

		castor3d::Texture const & getNormals()const
		{
			return m_normals;
		}

		castor3d::Texture const & getHeightDisplacement()const
		{
			return m_heightDisplacement.front();
		}

		castor3d::Texture const & getGradientJacobian()const
		{
			return m_gradientJacobian.front();
		}

		crg::FramePassArray getLastPasses();

	public:
		static castor::String const Name;

	private:
		void generateDistributionSeeds( ashes::Buffer< cfloat > & distribBuffer );

	private:
		crg::FramePassGroup & m_group;
		Config m_config;
		std::default_random_engine m_engine;
		std::normal_distribution< float > m_normDis{ 0.0f, 1.0f };
		VkExtent2D m_heightMapSamples{ 2u, 2u };
		uint32_t m_displacementDownsample{ 1u };
		FFTConfig m_fftConfig;
		ashes::BufferPtr< cfloat > m_heightSeeds;
		ashes::BufferPtr< cfloat > m_heightDistribution;
		crg::FramePass const * m_generateHeightDistribution{};
		GenerateFFTPassT< GenerateDistributionPass, GenerateHeightmapPass > m_height;
		ashes::BufferPtr< cfloat > m_displacementDistribution;
		crg::FramePass const * m_generateDisplacementDistribution{};
		GenerateFFTPassT< GenerateDistributionPass, GenerateDisplacementPass > m_displacement;
		std::array< castor3d::Texture, 2u > m_heightDisplacement;
		std::array< castor3d::Texture, 2u > m_gradientJacobian;
		crg::FramePass const * m_bakeHeightGradient{};
		crg::FramePass const * m_generateHeightDispMips{};
		crg::FramePass const * m_generateGradJacobMips{};
		ashes::BufferPtr< cfloat > m_normalSeeds;
		ashes::BufferPtr< cfloat > m_normalDistribution;
		crg::FramePass const * m_generateNormalDistribution{};
		GenerateFFTPassT< GenerateDistributionPass, GenerateNormalPass > m_normal;
		castor3d::Texture m_normals;
		crg::FramePass const * m_generateNormalsMips{};
		float L;
	};
}

#endif
