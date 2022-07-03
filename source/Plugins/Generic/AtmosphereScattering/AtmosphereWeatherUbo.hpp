/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereWeatherUbo_H___
#define ___C3DAS_AtmosphereWeatherUbo_H___

#include "AtmosphereWeatherConfig.hpp"

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>

#include <CastorUtils/Math/RangedValue.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructHelper.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace atmosphere_scattering
{
	struct WeatherData
		: public sdw::StructInstanceHelperT< "WeatherData"
			, sdw::type::MemoryLayout::eStd140
			, sdw::StructFieldT< sdw::Float, "cloudSpeed" >
			, sdw::StructFieldT< sdw::Float, "coverage" >
			, sdw::StructFieldT< sdw::Float, "crispiness" >
			, sdw::StructFieldT< sdw::Float, "curliness" >
			, sdw::StructFieldT< sdw::Float, "density" >
			, sdw::StructFieldT< sdw::Float, "absorption" >
			, sdw::StructFieldT< sdw::Float, "sphereInnerRadius" >
			, sdw::StructFieldT< sdw::Float, "sphereOuterRadius" >
			, sdw::StructFieldT< sdw::Float, "perlinAmplitude" >
			, sdw::StructFieldT< sdw::Float, "perlinFrequency" >
			, sdw::StructFieldT< sdw::Float, "perlinScale" >
			, sdw::StructFieldT< sdw::UInt, "perlinOctaves" >
			, sdw::StructFieldT< sdw::Vec3, "cloudColorTop" >
			, sdw::StructFieldT< sdw::Float, "time" >
			, sdw::StructFieldT< sdw::Vec3, "cloudColorBottom" >
			, sdw::StructFieldT< sdw::Int, "enablePowder" >
			, sdw::StructFieldT< sdw::Vec3, "seed" >
			, sdw::StructFieldT< sdw::Float, "pad0" >
			, sdw::StructFieldT< sdw::Vec3, "windDirection" >
			, sdw::StructFieldT< sdw::Float, "pad1" > >
	{
		WeatherData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto cloudSpeed()const { return getMember< "cloudSpeed" >(); }
		auto coverage()const { return getMember< "coverage" >(); }
		auto crispiness()const { return getMember< "crispiness" >(); }
		auto curliness()const { return getMember< "curliness" >(); }

		auto density()const { return getMember< "density" >(); }
		auto absorption()const { return getMember< "absorption" >(); }
		auto sphereInnerRadius()const { return getMember< "sphereInnerRadius" >(); }
		auto sphereOuterRadius()const { return getMember< "sphereOuterRadius" >(); }

		auto perlinAmplitude()const { return getMember< "perlinAmplitude" >(); }
		auto perlinFrequency()const { return getMember< "perlinFrequency" >(); }
		auto perlinScale()const { return getMember< "perlinScale" >(); }
		auto perlinOctaves()const { return getMember< "perlinOctaves" >(); }

		auto cloudColorTop()const { return getMember< "cloudColorTop" >(); }
		auto time()const { return getMember< "time" >(); }

		auto cloudColorBottom()const { return getMember< "cloudColorBottom" >(); }
		auto enablePowder()const { return getMember< "enablePowder" >(); }

		auto seed()const { return getMember< "seed" >(); }

		auto windDirection()const { return getMember< "windDirection" >(); }
	};

	class AtmosphereWeatherUbo
	{
	private:
		using Configuration = AtmosphereWeatherConfig;

	public:
		explicit AtmosphereWeatherUbo( castor3d::RenderDevice const & device
			, bool & dirty );
		~AtmosphereWeatherUbo();
		void cpuUpdate( Configuration const & config
			, float totalTime );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, "WeatherCfg", binding );
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
		bool & m_dirty;
		CheckedAtmosphereWeatherConfig m_config;
	};
}

#define C3D_AtmosphereWeather( writer, binding, set )\
	auto weatherBuffer = writer.declUniformBuffer<>( atmosphere_scattering::AtmosphereWeatherUbo::Buffer, binding, set );\
	auto c3d_weatherData = weatherBuffer.declMember< atmosphere_scattering::WeatherData >( atmosphere_scattering::AtmosphereWeatherUbo::Data );\
	weatherBuffer.end()

#endif
