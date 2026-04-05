/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_WeatherUbo_H___
#define ___C3DAS_WeatherUbo_H___

#include "WeatherConfig.hpp"

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Shader/Ubos/Ubo.hpp>

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
		SDW_DeclStructInstance( , WeatherData );

		WeatherData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
		{
		}

		auto perlinAmplitude()const { return getMember< "perlinAmplitude" >(); }
		auto perlinFrequency()const { return getMember< "perlinFrequency" >(); }
		auto perlinScale()const { return getMember< "perlinScale" >(); }
		auto perlinOctaves()const { return getMember< "perlinOctaves" >(); }
	};

	Writer_Parameter( WeatherData );

	class WeatherUbo
		: public c3d::UboT< WeatherConfig >
	{
	private:
		using Configuration = WeatherConfig;

	public:
		WeatherUbo( c3d::RenderDevice const & device );

		void cpuUpdate( Configuration const & config );

	public:
		static const c3d::MbString Buffer;
		static const c3d::MbString Data;
	};
}

#define C3D_Weather( writer, binding, set )\
	auto weatherBuffer = writer.declUniformBuffer<>( atmosphere_scattering::WeatherUbo::Buffer, binding, set );\
	auto c3d_weatherData = weatherBuffer.declMember< atmosphere_scattering::WeatherData >( atmosphere_scattering::WeatherUbo::Data );\
	weatherBuffer.end()

#endif
