/*
See LICENSE file in root folder
*/
#ifndef ___C3DWR_OceanUbo_H___
#define ___C3DWR_OceanUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

#define Ocean_Debug 1

namespace ocean
{
#if Ocean_Debug
	enum OceanDisplayData : uint32_t
	{
		eResult,
		eFinalNormal,
		eMatSpecular,
		eLightDiffuse,
		eLightSpecular,
		eNoisedSpecular,
		eSpecularNoise,
		eIndirectOcclusion,
		eLightIndirectDiffuse,
		eLightIndirectSpecular,
		eIndirectAmbient,
		eIndirectDiffuse,
		eBackgroundReflection,
		eSSRResult,
		eSSRFactor,
		eSSRResultFactor,
		eReflection,
		eRefraction,
		eDepthSoftenedAlpha,
		eHeightMixedRefraction,
		eDistanceMixedRefraction,
		eFresnelFactor,
		eFinalReflection,
		eFinalRefraction,
		eWaterBaseColour,
		CU_EnumBounds( OceanDisplayData, eResult )
	};
	castor::StringArray const & getOceanDisplayDataNames();
#endif

	static uint32_t constexpr MaxWaves = 8u;

	struct WaveConfigurationData
	{
		castor::Point4f direction;
		float steepness;
		float length;
		float amplitude;
		float speed;
	};

	struct OceanUboConfiguration
	{
		float tessellationFactor{ 7.0f };
		float numWaves{ 2.0f };
		float time{ 0.0f };
		float dampeningFactor{ 5.0f };
		float refractionRatio{ 1.2f };
		float refractionDistortionFactor{ 0.04f };
		float refractionHeightFactor{ 2.5f };
		float refractionDistanceFactor{ 15.0f };
		float foamHeightStart{ 0.8f };
		float foamFadeDistance{ 0.4f };
		float foamTiling{ 2.0f };
		float foamAngleExponent{ 80.0f };
		float foamBrightness{ 4.0f };
		float depthSofteningDistance{ 0.5f };
		castor::Point2f normalMapScrollSpeed{ 0.01f, 0.01f };
		castor::Point4f normalMapScroll{ 1.0f, 0.0f, 0.0f, 1.0f };
		castor::Point4f ssrSettings{ 0.5f, 20.0f, 10.0f, 20.0f };
#if Ocean_Debug
		uint32_t debug{ eResult };
		uint32_t dummy[3]{};
#endif
		std::array< WaveConfigurationData, MaxWaves > waves{ WaveConfigurationData{ castor::Point4f{ 0.3f, 0.0f, -0.7f }, 1.79f, 3.75f, 0.85f, 1.21f }
			, WaveConfigurationData{ castor::Point4f{ 0.5f, 0.0f, -0.2f }, 1.79f, 4.1f, 0.52f, 1.03f }
			, WaveConfigurationData{}
			, WaveConfigurationData{}
			, WaveConfigurationData{}
			, WaveConfigurationData{}
			, WaveConfigurationData{}
			, WaveConfigurationData{} };
	};

	struct Wave
		: sdw::StructInstance
	{
		Wave( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true );

		SDW_DeclStructInstance( , Wave );

		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache );

	private:
		sdw::Vec4 m_direction;
		sdw::Vec4 m_other;

	public:
		sdw::Vec3 direction;
		sdw::Float steepness;
		sdw::Float length;
		sdw::Float amplitude;
		sdw::Float speed;
	};

	Writer_Parameter( Wave );

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
		sdw::Vec4 m_foam;
		sdw::Vec4 m_other;

	public:
		sdw::Vec4 normalMapScroll;
		sdw::Vec4 ssrSettings;
#if Ocean_Debug
		sdw::UVec4 debug;
#endif
		sdw::Array< Wave > waves;
		sdw::Float tessellationFactor;
		sdw::Float numWaves;
		sdw::Float time;
		sdw::Float dampeningFactor;
		sdw::Float refractionRatio;
		sdw::Float refractionDistortionFactor;
		sdw::Float refractionHeightFactor;
		sdw::Float refractionDistanceFactor;
		sdw::Float foamHeightStart;
		sdw::Float foamFadeDistance;
		sdw::Float foamTiling;
		sdw::Float foamAngleExponent;
		sdw::Float foamBrightness;
		sdw::Float depthSofteningDistance;
		sdw::Vec2 normalMapScrollSpeed;
	};

	class OceanUbo
	{
	private:
		using Configuration = OceanUboConfiguration;

	public:
		explicit OceanUbo( castor3d::RenderDevice const & device );
		~OceanUbo();
		void cpuUpdate( OceanUboConfiguration const & config );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, "WaveCfg", binding );
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
	auto oceanBuffer = writer.declUniformBuffer( ocean::OceanUbo::Buffer, binding, set );\
	auto c3d_oceanData = oceanBuffer.declStructMember< ocean::OceanData >( ocean::OceanUbo::Data );\
	oceanBuffer.end()

#endif
