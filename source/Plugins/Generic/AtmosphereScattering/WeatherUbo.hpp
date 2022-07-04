/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_WeatherUbo_H___
#define ___C3DAS_WeatherUbo_H___

#include "WeatherConfig.hpp"

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
			, sdw::FloatField< "perlinAmplitude" >
			, sdw::FloatField< "perlinFrequency" >
			, sdw::FloatField< "perlinScale" >
			, sdw::UIntField< "perlinOctaves" > >
	{
		WeatherData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto perlinAmplitude()const { return getMember< "perlinAmplitude" >(); }
		auto perlinFrequency()const { return getMember< "perlinFrequency" >(); }
		auto perlinScale()const { return getMember< "perlinScale" >(); }
		auto perlinOctaves()const { return getMember< "perlinOctaves" >(); }
	};

	Writer_Parameter( WeatherData );

	class WeatherUbo
	{
	private:
		using Configuration = WeatherConfig;

	public:
		explicit WeatherUbo( castor3d::RenderDevice const & device
			, bool & dirty );
		~WeatherUbo();
		void cpuUpdate( Configuration const & config );

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
		CheckedWeatherConfig m_config;
	};
}

#define C3D_Weather( writer, binding, set )\
	auto weatherBuffer = writer.declUniformBuffer<>( atmosphere_scattering::WeatherUbo::Buffer, binding, set );\
	auto c3d_weatherData = weatherBuffer.declMember< atmosphere_scattering::WeatherData >( atmosphere_scattering::WeatherUbo::Data );\
	weatherBuffer.end()

#endif
