/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_OceanUbo_H___
#define ___C3DORFFT_OceanUbo_H___

#include "OceanFFTRenderingPrerequisites.hpp"

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

#define Ocean_Debug 1

namespace ocean_fft
{
#if Ocean_Debug
	enum OceanDisplayData : uint32_t
	{
		eResult,
		eGradJacobian,
		eNoiseGradient,
		eJacobian,
		eTurbulence,
		eNormal,
		eColorMod,
		eMatSpecular,
		eLightDiffuse,
		eLightSpecular,
		eIndirectOcclusion,
		eLightIndirectDiffuse,
		eLightIndirectSpecular,
		eIndirectAmbient,
		eIndirectDiffuse,
		eRawBackgroundReflection,
		eFresnelBackgroundReflection,
		eSSRResult,
		eSSRFactor,
		eSSRResultFactor,
		eCombinedReflection,
		eRawRefraction,
		eLightAbsorbtion,
		eWaterTransmission,
		eRefractionResult,
		eDepthSoftenedAlpha,
		eHeightMixedRefraction,
		eDistanceMixedRefraction,
		eFresnelFactor,
		eFinalReflection,
		eFinalRefraction,
		CU_EnumBounds( OceanDisplayData, eResult )
	};
	castor::StringArray const & getOceanDisplayDataNames();
#endif

	struct OceanUboConfiguration
	{
		float time{ 0.0f };
		float depthSofteningDistance{ 0.5f };
		float distanceMod{};
		float density{ 1.0f };

		float refractionRatio{ 1.2f };
		float refractionDistortionFactor{ 0.04f };
		float refractionHeightFactor{ 2.5f };
		float refractionDistanceFactor{ 15.0f };

		castor::Point4f ssrSettings{ 0.5f, 20.0f, 10.0f, 20.0f };

		castor::Point2f otherMod{};
		castor::Point2f normalMod{};

		castor::Point2f tileScale{};
		castor::Point2f normalScale{};

		castor::Point2f blockOffset{};
		castor::Point2f fftScale{};

		castor::Point2f patchSize{};
		castor::Point2f maxTessLevel{};

		castor::Point2f invHeightmapSize{};
		castor::Point2f windDirection{};

		float amplitude{};
		float maxWaveLength{};
		float L{};
		float dummy2{};

		castor::Point2ui size{};
		uint32_t displacementDownsample{};
#if Ocean_Debug
		uint32_t debug{ eResult };
#else
		uint32_t dummy3;
#endif
	};

	struct OceanData
		: public sdw::StructInstance
	{
		OceanData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , OceanData );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	private:
		sdw::Vec4 m_base;
		sdw::Vec4 m_refraction;
	public:
		sdw::Vec4 ssrSettings;
	private:
		sdw::Vec4 m_mods;
		sdw::Vec4 m_tileNormalScales;
		sdw::Vec4 m_blockOffFftScale;
		sdw::Vec4 m_patchSizes;
		sdw::Vec4 m_fftSizes;
		sdw::Vec4 m_fftDistrib;
		sdw::UVec4 m_size;

	public:
		sdw::Float time;
		sdw::Float depthSofteningDistance;
		sdw::Float distanceMod;
		sdw::Float density;
		sdw::Float refractionRatio;
		sdw::Float refractionDistortionFactor;
		sdw::Float refractionHeightFactor;
		sdw::Float refractionDistanceFactor;
		sdw::Vec2 otherMod;
		sdw::Vec2 normalMod;
		sdw::Vec2 tileScale;
		sdw::Vec2 normalScale;
		sdw::Vec2 blockOffset;
		sdw::Vec2 fftScale;
		sdw::Vec2 patchSize;
		sdw::Vec2 maxTessLevel;
		sdw::Vec2 invHeightmapSize;
		sdw::Vec2 windDirection;
		sdw::Float amplitude;
		sdw::Float maxWaveLength;
		sdw::Float L;
		sdw::UVec2 size;
		sdw::Int displacementDownsample;
#if Ocean_Debug
		sdw::UInt debug;
#endif
	};

	class OceanUbo
	{
	private:
		using Configuration = OceanUboConfiguration;

	public:
		explicit OceanUbo( castor3d::RenderDevice const & device );
		~OceanUbo();
		void cpuUpdate( OceanUboConfiguration const & config
			, OceanFFTConfig const & fftConfig
			, castor::Point3f const & cameraPosition );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, "OceanCfg", binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		ashes::WriteDescriptorSet getDescriptorWrite( uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			return m_ubo.getDescriptorWrite( dstBinding, dstArrayElement );
		}

		castor3d::UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

		castor3d::UniformBufferOffsetT< Configuration > & getUbo()
		{
			return m_ubo;
		}

	public:
		static const castor::String Buffer;
		static const castor::String Data;

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_OCEAN( writer, binding, set )\
	auto oceanBuffer = writer.declUniformBuffer( ocean_fft::OceanUbo::Buffer, binding, set );\
	auto c3d_oceanData = oceanBuffer.declStructMember< ocean_fft::OceanData >( ocean_fft::OceanUbo::Data );\
	oceanBuffer.end()

#endif
