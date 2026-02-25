/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_CloudsUbo_H___
#define ___C3DAS_CloudsUbo_H___

#include "CloudsConfig.hpp"

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
	struct CloudsData
		: public sdw::StructInstanceHelperT< "C3D_ATM_CloudsData"
			, sdw::type::MemoryLayout::eStd140
			, sdw::FloatField< "speed" >
			, sdw::FloatField< "coverage" >
			, sdw::FloatField< "crispiness" >
			, sdw::FloatField< "curliness" >
			, sdw::FloatField< "density" >
			, sdw::FloatField< "absorption" >
			, sdw::FloatField< "innerRadius" >
			, sdw::FloatField< "outerRadius" >
			, sdw::Vec3Field< "topColor" >
			, sdw::FloatField< "time" >
			, sdw::Vec3Field< "bottomColor" >
			, sdw::IntField< "enablePowder" >
			, sdw::Vec3Field< "windDirection" >
			, sdw::FloatField< "topOffset" > >
	{
		SDW_DeclStructInstance( , CloudsData );

		CloudsData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
		{
		}

		auto cloudSpeed()const { return getMember< "speed" >(); }
		auto coverage()const { return getMember< "coverage" >(); }
		auto crispiness()const { return getMember< "crispiness" >(); }
		auto curliness()const { return getMember< "curliness" >(); }

		auto density()const { return getMember< "density" >(); }
		auto absorption()const { return getMember< "absorption" >(); }
		auto innerRadius()const { return getMember< "innerRadius" >(); }
		auto outerRadius()const { return getMember< "outerRadius" >(); }

		auto topColor()const { return getMember< "topColor" >(); }
		auto time()const { return getMember< "time" >(); }

		auto bottomColor()const { return getMember< "bottomColor" >(); }
		auto enablePowder()const { return getMember< "enablePowder" >(); }

		auto windDirection()const { return getMember< "windDirection" >(); }
		auto topOffset()const { return getMember< "topOffset" >(); }
	};

	Writer_Parameter( CloudsData );

	class CloudsUbo
		: public c3d::UboT< CloudsConfig >
	{
	private:
		using Configuration = CloudsConfig;

	public:
		CloudsUbo( c3d::RenderDevice const & device
			, bool & dirty );

		void cpuUpdate( Configuration const & config
			, float totalTime );

	public:
		static const c3d::MbString Buffer;
		static const c3d::MbString Data;

	private:
		CheckedCloudsConfig m_config;
	};
}

#define C3D_Clouds( writer, binding, set )\
	auto cloudsBuffer = writer.declUniformBuffer<>( atmosphere_scattering::CloudsUbo::Buffer, binding, set );\
	auto c3d_cloudsData = cloudsBuffer.declMember< atmosphere_scattering::CloudsData >( atmosphere_scattering::CloudsUbo::Data );\
	cloudsBuffer.end()

#endif
