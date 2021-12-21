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

	template< typename GeneratePassT >
	struct FullGeneratePassT;

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

	template< template< typename ValueT > typename WrapperT >
	struct OceanFFTConfigT
	{
		WrapperT< castor::Point2f > size;
		WrapperT< castor::Point2f > normalFreqMod;
		WrapperT< castor::Point2f > windVelocity;
		WrapperT< castor::Point2f > patchSize;
		WrapperT< castor::Point2ui > blocksCount;
		WrapperT< uint32_t > heightMapSamples;
		WrapperT< uint32_t > displacementDownsample;
		WrapperT< float > amplitude;
		WrapperT< float > maxWaveLength;
		WrapperT< float > lod0Distance;
		WrapperT< bool > disableRandomSeed;
	};
	using OceanFFTConfig = OceanFFTConfigT< crg::RawTypeT >;
	using OceanFFTTrackedConfig = OceanFFTConfigT< castor::GroupChangeTracked >;

	enum class FFTMode
	{
		eC2C,
		eR2C,
		eC2R,
		eR2R
	};

	bool checkFFTResult( castor::xchar const * action
		, VkFFTResult result );
	void checkFFTResultMandat( castor::xchar const * action
		, VkFFTResult result );
	crg::FramePass const & createProcessFFTPass( castor::String const & name
		, castor3d::RenderDevice const & device
		, crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, VkExtent2D const & extent
		, FFTConfig const & config
		, ashes::BufferBase const & input
		, std::array< ashes::BufferBasePtr, 2u > const & output );
	crg::FramePass const & createBakeHeightGradientPass( castor3d::RenderDevice const & device
		, crg::FrameGraph & graph
		, crg::FramePassArray previousPasses
		, VkExtent2D const & extent
		, castor::Point2f const & heightMapSize
		, uint32_t displacementDownsample
		, OceanUbo const & ubo
		, ashes::BufferBase const & height
		, ashes::BufferBase const & displacement
		, std::array< castor3d::Texture, 2u > const & heightDisp
		, std::array< castor3d::Texture, 2u > const & gradJacob );
}

#endif
