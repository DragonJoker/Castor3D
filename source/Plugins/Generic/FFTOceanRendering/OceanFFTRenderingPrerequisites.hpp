/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_OceanFFTRenderingPrerquisites_H___
#define ___C3DORFFT_OceanFFTRenderingPrerquisites_H___

#include <ashes/ashes.h>

#pragma warning( push )
#pragma warning( disable:4189 )
#pragma warning( disable:4244 )
#pragma warning( disable:4245 )
#pragma warning( disable:4365 )
#pragma warning( disable:4456 )
#pragma warning( disable:4616 )
#pragma warning( disable:5219 )
#include <vkFFT/vkFFT.h>
#pragma warning( pop )

#include <Castor3D/Render/Technique/TechniqueModule.hpp>
#include <Castor3D/Render/RenderPass.hpp>

#include <complex>

namespace ocean_fft
{
	class GenerateDisplacementPass;
	class GenerateHeightmapPass;
	class GenerateMipmapsPass;
	class GenerateNormalPass;
	class OceanFFT;
	class OceanUbo;
	class OceanRenderPass;
	class ProcessFFTPass;

	template< typename DistributionPassT, typename FrequencyPassT >
	struct GenerateFFTPassT;

	using cfloat = std::complex< float >;

	struct FFTConfig
	{
		FFTConfig( castor3d::RenderDevice const & device
			, VkExtent2D const & dimensions );

		castor3d::RenderDevice const & device;
		castor3d::QueueData const & queueData;
		ashes::FencePtr fence;
		mutable VkPhysicalDevice vkPhysicalDevice{};
		mutable VkDevice vkDevice{};
		mutable VkQueue vkQueue{};
		mutable VkCommandPool vkCommandPool{};
		mutable VkFence vkFence{};
		uint32_t bufferSize{};
		VkFFTApplication application{};
	};

	struct OceanFFTConfig
	{
		castor::Point2f size{ 200.0f, 200.0f };
		castor::Point2f normalFreqMod{ 7.3f, 7.3f };
		castor::Point2f windDirection{ 0.76339f, -0.64594f };
		castor::Point2f patchSize{ 32.0f, 32.0f };
		castor::Point2ui blocksCount{ 64u, 64u };
		uint32_t heightMapSamples{ 256u };
		uint32_t displacementDownsample{ 1u };
		float amplitude{ 1.0f };
		float windVelocity{ 340.0f };
		float lod0Distance{ 50.0f };
		bool disableRandomSeed{ false };
	};

	enum class FFTMode
	{
		eC2C,
		eR2C,
		eC2R,
		eR2R
	};

	struct IsRenderPassEnabled
	{
		IsRenderPassEnabled( castor3d::SceneRenderPass const & pass )
			: m_pass{ &pass }
		{}

		IsRenderPassEnabled()
			: m_pass{}
		{}

		void setPass( castor3d::SceneRenderPass const & pass )
		{
			m_pass = &pass;
		}

		bool operator()()const
		{
			return m_pass->isPassEnabled();
		}

		castor3d::SceneRenderPass const * m_pass;
	};

	bool checkFFTResult( castor::xchar const * action
		, VkFFTResult result );
	void checkFFTResultMandat( castor::xchar const * action
		, VkFFTResult result );
}

#endif
