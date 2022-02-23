/*
See LICENSE file in root folder
*/
#ifndef ___C3DWR_WaterUbo_H___
#define ___C3DWR_WaterUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

#define Water_Debug 1

namespace water
{
#if Water_Debug
	enum WaterDisplayData : uint32_t
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
		CU_EnumBounds( WaterDisplayData, eResult )
	};
	castor::StringArray const & getWaterDisplayDataNames();
#endif

	struct WaterUboConfiguration
	{
		float time{ 0.0f };
		float dampeningFactor{ 5.0f };
		float depthSofteningDistance{ 0.5f };
		float dummy1{};
		float refractionRatio{ 1.2f };
		float refractionDistortionFactor{ 0.04f };
		float refractionHeightFactor{ 2.5f };
		float refractionDistanceFactor{ 15.0f };
		castor::Point2f normalMapScrollSpeed{ 0.01f, 0.01f };
		castor::Point2f dummy2{};
		castor::Point4f normalMapScroll{ 1.0f, 0.0f, 0.0f, 1.0f };
		float ssrStepSize{ 0.5f };
		float ssrForwardStepsCount{ 20.0f };
		float ssrBackwardStepsCount{ 10.0f };
		float ssrDepthMult{ 20.0f };
#if Water_Debug
		uint32_t debug{ eResult };
		uint32_t dummy[3]{};
#endif
	};

	struct WaterData
		: public sdw::StructInstance
	{
		WaterData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , WaterData );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	private:
		sdw::Vec4 m_base;
		sdw::Vec4 m_refraction;
		sdw::Vec4 m_scrSpdOth;

	public:
		sdw::Vec4 normalMapScroll;
		sdw::Vec4 ssrSettings;
#if Water_Debug
		sdw::UVec4 debug;
#endif
		sdw::Float time;
		sdw::Float dampeningFactor;
		sdw::Float depthSofteningDistance;
		sdw::Float refractionRatio;
		sdw::Float refractionDistortionFactor;
		sdw::Float refractionHeightFactor;
		sdw::Float refractionDistanceFactor;
		sdw::Vec2 normalMapScrollSpeed;
	};

	class WaterUbo
	{
	private:
		using Configuration = WaterUboConfiguration;

	public:
		explicit WaterUbo( castor3d::RenderDevice const & device );
		~WaterUbo();
		void cpuUpdate( WaterUboConfiguration const & config );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, Buffer + "Cfg", binding );
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

#define C3D_Water( writer, binding, set )\
	auto waterBuffer = writer.declUniformBuffer( water::WaterUbo::Buffer, binding, set );\
	auto c3d_waterData = waterBuffer.declMember< water::WaterData >( water::WaterUbo::Data );\
	waterBuffer.end()

#endif
