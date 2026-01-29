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
	void createGenerateFrequencyPassT( c3d::String const & name
		, c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, c3d::Extent2D const & extent
		, OceanUbo const & ubo
		, c3d::BufferBase const & input
		, c3d::BufferBase & output )
	{
		auto mbName = c3d::toUtf8( name );
		auto & pass = graph.createPass( "GenerateFrequency" + mbName
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
				c3d::getEngine( device ).registerTimer( c3d::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		ubo.createPassBinding( pass, GeneratePassT::Bindings::eConfig );
		pass.addInputStorageT( *input.getLastAttach(), GeneratePassT::Bindings::eInput );
		output.setLastAttach( pass.addOutputStorageBufferT( output.bufferViewId, GeneratePassT::Bindings::eOutput ) );
	}

	template< typename DistributionPassT, typename FrequencyPassT >
	struct GenerateFFTPassT
	{
		GenerateFFTPassT( c3d::String const & prefix
			, c3d::String const & name
			, crg::FramePassGroup & graph
			, OceanUbo const & ubo
			, c3d::Extent2D dimensions
			, VkFFTConfig const & pfftConfig
			, c3d::BufferT< cfloat > const & distribution )
			: fftConfig{ pfftConfig }
			, frequency{ c3d::makeBuffer< cfloat >( fftConfig.device
					, c3d::getEngine( fftConfig.device ).getGraphResourceCache()
					, dimensions.width * dimensions.height
					, c3d::BufferUsageFlags::eStorageBuffer | c3d::BufferUsageFlags::eTransferSrc | c3d::BufferUsageFlags::eTransferDst
					, c3d::MemoryPropertyFlags::eDeviceLocal
					, prefix + name + cuT( "Frequency" ) ) }
			, result{ c3d::makeBufferBase( pfftConfig.device
					, c3d::getEngine( fftConfig.device ).getGraphResourceCache()
					, sizeof( cfloat ) * dimensions.width * dimensions.height
					, c3d::BufferUsageFlags::eStorageBuffer | c3d::BufferUsageFlags::eTransferSrc | c3d::BufferUsageFlags::eTransferDst
					, c3d::MemoryPropertyFlags::eDeviceLocal
					, prefix + name + cuT( "Result0" ) )
				, c3d::makeBufferBase( pfftConfig.device
					, c3d::getEngine( fftConfig.device ).getGraphResourceCache()
					, sizeof( cfloat ) * dimensions.width * dimensions.height
					, c3d::BufferUsageFlags::eStorageBuffer | c3d::BufferUsageFlags::eTransferSrc | c3d::BufferUsageFlags::eTransferDst
					, c3d::MemoryPropertyFlags::eDeviceLocal
					, prefix + name + cuT( "Result1" ) ) }
		{
			createGenerateFrequencyPassT< FrequencyPassT >( name, fftConfig.device, graph, dimensions
				, ubo, distribution, *frequency );
			createProcessFFTPass( name, fftConfig.device, graph, dimensions, fftConfig
				, *frequency, result );
		}

		~GenerateFFTPassT()noexcept
		{
			result[0]->destroy();
			result[1]->destroy();
			frequency->destroy();
		}

		c3d::Buffer const & getResult()
		{
			return *result[0];
		}

	private:
		VkFFTConfig const & fftConfig;
		c3d::BufferUPtrT< cfloat > frequency;
		c3d::Array< c3d::BufferUPtr, 2u > result;
	};

	class OceanFFT
	{
	public:
		using Config = OceanFFTConfig;

	public:
		OceanFFT( c3d::RenderDevice const & device
			, crg::ResourcesCache & resources
			, crg::FramePassGroup & graph
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

	public:
		static c3d::String const Name;

	private:
		void generateDistributionSeeds( c3d::BufferT< cfloat > const & distribBuffer );

	private:
		c3d::RenderDevice const & m_device;
		crg::FramePassGroup & m_group;
		OceanFFTConfig m_config;
		std::default_random_engine m_engine;
		std::normal_distribution< float > m_normDis{ 0.0f, 1.0f };
		c3d::Extent2D m_heightMapSamples{ 2u, 2u };
		uint32_t m_displacementDownsample{ 1u };
		VkFFTConfig m_fftConfig;
		c3d::BufferUPtrT< cfloat > m_heightSeeds;
		c3d::BufferUPtrT< cfloat > m_heightDistribution;
		GenerateFFTPassT< GenerateDistributionPass, GenerateHeightmapPass > m_height;
		c3d::BufferUPtrT< cfloat > m_displacementDistribution;
		GenerateFFTPassT< GenerateDistributionPass, GenerateDisplacementPass > m_displacement;
		c3d::Array< c3d::Texture, 2u > m_heightDisplacement;
		c3d::Array< c3d::Texture, 2u > m_gradientJacobian;
		c3d::BufferUPtrT< cfloat > m_normalSeeds;
		c3d::BufferUPtrT< cfloat > m_normalDistribution;
		GenerateFFTPassT< GenerateDistributionPass, GenerateNormalPass > m_normal;
		c3d::Texture m_normals;
	};
}

#endif
