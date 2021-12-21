/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_OceanFFT_H___
#define ___C3DORFFT_OceanFFT_H___

#include "GenerateDisplacementPass.hpp"
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
	crg::FramePass const & createGeneratePassT( castor::String const & prefix
		, castor::String const & name
		, castor3d::RenderDevice const & device
		, crg::FrameGraph & graph
		, crg::FramePassArray previousPasses
		, VkExtent2D const & extent
		, OceanUbo const & ubo
		, ashes::BufferBase const & input
		, ashes::BufferBase const & output )
	{
		auto & result = graph.createPass( prefix + "/Generate" + name
			, [&device, extent]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
			auto res = std::make_unique< GeneratePassT >( framePass
				, context
				, runnableGraph
				, device
				, extent );
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
		result.addOutputStorageBuffer( { output, name + "Generated" }
			, GeneratePassT::eOutput
			, 0u
			, output.getSize() );
		return result;
	}

	template< typename GeneratePassT >
	struct FullGeneratePassT
	{
		FullGeneratePassT( castor::String const & prefix
			, castor::String const & name
			, crg::FrameGraph & graph
			, crg::FramePassArray previousPasses
			, OceanUbo const & ubo
			, VkExtent2D dimensions
			, FFTConfig const & pfftConfig
			, FFTMode mode )
			: fftConfig{ pfftConfig }
			, distribution{ castor3d::makeBuffer< cfloat >( fftConfig.device
				, dimensions.width * dimensions.height
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, prefix + name + "Distribution" ) }
			, generated{ castor3d::makeBuffer< cfloat >( fftConfig.device
					, dimensions.width * dimensions.height
					, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					, prefix + name + "Generated" ) }
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
			, generate{ &createGeneratePassT< GeneratePassT >( prefix
				, name
				, fftConfig.device
				, graph
				, previousPasses
				, dimensions
				, ubo
				, distribution->getBuffer()
				, generated->getBuffer() ) }
			, processFFT( &createProcessFFTPass( name
				, fftConfig.device
				, graph
				, *generate
				, dimensions
				, fftConfig
				, generated->getBuffer()
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

		ashes::Buffer< cfloat > & getDistribution()
		{
			return *distribution;
		}

	private:
		FFTConfig const & fftConfig;
		ashes::BufferPtr< cfloat > distribution;
		ashes::BufferPtr< cfloat > generated;
		std::array< ashes::BufferBasePtr, 2u > result;
		crg::FramePass const * generate{};
		crg::FramePass const * processFFT{};
	};

	class OceanFFT
	{
	public:
		using Config = OceanFFTConfig;
		using TrackedConfig = OceanFFTTrackedConfig;

	public:
		OceanFFT( castor3d::RenderDevice const & device
			, crg::FrameGraph & graph
			, crg::FramePassArray previousPasses
			, OceanUbo const & ubo );
		~OceanFFT();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( castor3d::RenderTechniqueVisitor & visitor );

		void update();

		TrackedConfig const & getConfig()const
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
		cfloat phillips( castor::Point2f const & k
			, float maxWaveLength
			, castor::Point2f const & windDirection );
		std::vector< cfloat > generateDistribution( float amplitude
			, float maxWaveLength
			, castor::Point2f const & size
			, ashes::Buffer< cfloat > & distribBuffer );
		void downsampleDistribution( std::vector< cfloat > const & distribution
			, uint32_t downsample
			, ashes::Buffer< cfloat > & distribBuffer );

	private:
		bool m_dirty{ true };
		TrackedConfig m_config;
		std::default_random_engine m_engine;
		std::normal_distribution< float > m_normDis{ 0.0f, 1.0f };
		VkExtent2D m_heightMapSamples{ 2u, 2u };
		uint32_t m_displacementDownsample{ 1u };
		FFTConfig m_fftConfig;
		FullGeneratePassT< GenerateHeightmapPass > m_height;
		FullGeneratePassT< GenerateNormalPass > m_normal;
		FullGeneratePassT< GenerateDisplacementPass > m_displacement;
		std::array< castor3d::Texture, 2u > m_heightDisplacement;
		std::array< castor3d::Texture, 2u > m_gradientJacobian;
		castor3d::Texture m_normals;
		crg::FramePass const * m_bakeHeightGradient{};
		crg::FramePass const * m_generateHeightDispMips{};
		crg::FramePass const * m_generateGradJacobMips{};
		crg::FramePass const * m_generateNormalsMips{};
		float L;
	};
}

#endif
