/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereScatteringUbo_H___
#define ___C3DAS_AtmosphereScatteringUbo_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace atmosphere_scattering
{
#if AtmosphereScattering_Debug
	enum AtmosphereScatteringDisplayData : uint32_t
	{
		eResult,
		eSunContribution,
		eSkyContribution,
		CU_EnumBounds( AtmosphereScatteringDisplayData, eResult )
	};
	castor::StringArray const & getDisplayDataNames();
#endif

	struct AtmosphereScatteringUboConfiguration
	{
		castor::Point3f earthPos{ 0.0, 0.0, 6360010.0 };
		float sunIntensity{ 100.0f };

		// ----------------------------------------------------------------------------
		// PHYSICAL MODEL PARAMETERS
		// ----------------------------------------------------------------------------

		float scale{ 1000.0f };
		float Rg{ 6360.0f * scale };
		float Rt{ 6420.0f * scale };
		float RL{ 6421.0f * scale };

		float averageGroundReflectance{ 0.1f };
		float HR{ 8.0f * scale };
		float HM{ 1.2f * scale };
		float betaMSca{ 4e-3f / scale };

		castor::Point3f betaR{ 5.8e-3f / scale, 1.35e-2f / scale, 3.31e-2f / scale };
		float betaMDiv{ 0.9f };

		float mieG{ 0.8f };
		float g{ 9.81f };
		castor::Point2f dummy1{};

		// ----------------------------------------------------------------------------
		// NUMERICAL INTEGRATION PARAMETERS
		// ----------------------------------------------------------------------------

		int transmittanceIntegralSamples{ 500 };
		int inscatterIntegralSamples{ 50 };
		int irradianceIntegralSamples{ 32 };
		int inscatterSphericalIntegralSamples{ 16 };

		// ----------------------------------------------------------------------------
		// PARAMETERIZATION OPTIONS
		// ----------------------------------------------------------------------------

		int transmittanceW{ 256 };
		int transmittanceH{ 64 };
		int skyW{ 64 };
		int skyH{ 16 };

		int resR{ 32 };
		int resMu{ 128 };
		int resMuS{ 32 };
		int resNu{ 8 };

		castor::Point2ui size{};
		uint32_t vertexCount{};
#if AtmosphereScattering_Debug
		uint32_t debug{ eResult };
#else
		uint32_t dummy4{};
#endif
	};

	struct AtmosphereData
		: public sdw::StructInstance
	{
		AtmosphereData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , AtmosphereData );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		sdw::Vec3 getSunDir();

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	private:
		sdw::Vec4 m_sunEarth;
		sdw::Vec4 m_atmos0;
		sdw::Vec4 m_atmos1;
		sdw::Vec4 m_atmos2;
		sdw::Vec4 m_atmos3;
		sdw::IVec4 m_atmos4;
		sdw::IVec4 m_atmos5;
		sdw::IVec4 m_atmos6;
	private:
		sdw::UVec4 m_sd;

	public:
		sdw::Vec3 earthPos;
		sdw::Float sunIntensity;

		sdw::Float scale;
		sdw::Float Rg;
		sdw::Float Rt;
		sdw::Float RL;

		sdw::Float averageGroundReflectance;
		sdw::Float HR;
		sdw::Float HM;
		sdw::Float betaMSca;

		sdw::Vec3 betaR;
		sdw::Float betaMDiv;

		sdw::Float mieG;
		sdw::Float g;
		sdw::Int transmittanceIntegralSamples;
		sdw::Int inscatterIntegralSamples;

		sdw::Int irradianceIntegralSamples;
		sdw::Int inscatterSphericalIntegralSamples;
		sdw::Int transmittanceW;
		sdw::Int transmittanceH;

		sdw::Int skyW;
		sdw::Int skyH;
		sdw::Int resR;
		sdw::Int resMu;

		sdw::Int resMuS;
		sdw::Int resNu;

		sdw::UVec2 size;
		sdw::UInt vertexCount;
#if AtmosphereScattering_Debug
		sdw::UInt debug;
#endif
	};

	class AtmosphereScatteringUbo
	{
	private:
		using Configuration = AtmosphereScatteringUboConfiguration;

	public:
		explicit AtmosphereScatteringUbo( castor3d::RenderDevice const & device );
		~AtmosphereScatteringUbo();
		void cpuUpdate( Configuration const & config );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, "AtmosphereCfg", binding );
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

#define C3D_AtmosphereScattering( writer, binding, set )\
	auto atmosphereBuffer = writer.declUniformBuffer<>( atmosphere_scattering::AtmosphereScatteringUbo::Buffer, binding, set );\
	auto c3d_atmosphereData = atmosphereBuffer.declMember< atmosphere_scattering::AtmosphereData >( atmosphere_scattering::AtmosphereScatteringUbo::Data );\
	atmosphereBuffer.end()

#endif
