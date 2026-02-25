/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Uncharted2_Uncharted2Ubo_H___
#define ___C3D_Uncharted2_Uncharted2Ubo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Shader/Ubos/Ubo.hpp>

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/CompositeTypes/Function.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace Uncharted2
{
	struct Uncharted2Data
		: public sdw::StructInstanceHelperT< "C3D_Uncharted2Data"
			, sdw::type::MemoryLayout::eStd140
			, sdw::FloatField< "shoulderStrength" >
			, sdw::FloatField< "linearStrength" >
			, sdw::FloatField< "linearAngle" >
			, sdw::FloatField< "toeStrength" >
			, sdw::FloatField< "toeNumerator" >
			, sdw::FloatField< "toeDenominator" >
			, sdw::FloatField< "linearWhitePointValue" >
			, sdw::FloatField< "exposureBias" > >
	{
		SDW_DeclStructInstance( , Uncharted2Data );

		Uncharted2Data( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			, shoulderStrength{ getMember< "shoulderStrength" >() }
			, linearStrength{ getMember< "linearStrength" >() }
			, linearAngle{ getMember< "linearAngle" >() }
			, toeStrength{ getMember< "toeStrength" >() }
			, toeNumerator{ getMember< "toeNumerator" >() }
			, toeDenominator{ getMember< "toeDenominator" >() }
			, linearWhitePointValue{ getMember< "linearWhitePointValue" >() }
			, exposureBias{ getMember< "exposureBias" >() }
		{
		}

		sdw::RetVec3 toneMap( sdw::Vec3 const & x );

		sdw::Float shoulderStrength;
		sdw::Float linearStrength;
		sdw::Float linearAngle;
		sdw::Float toeStrength;
		sdw::Float toeNumerator;
		sdw::Float toeDenominator;
		sdw::Float linearWhitePointValue;
		sdw::Float exposureBias;

	private:
		sdw::Function< sdw::Vec3
			, sdw::InVec3 > m_toneMap;
	};

	struct Uncharted2UboConfiguration
	{
		void accept( c3d::ConfigurationVisitorBase & visitor );

		float shoulderStrength{ 0.15f };
		float linearStrength{ 0.50f };
		float linearAngle{ 0.10f };
		float toeStrength{ 0.20f };
		float toeNumerator{ 0.02f };
		float toeDenominator{ 0.30f };
		float linearWhitePointValue{ 11.2f };
		float exposureBias{ 2.0f };
	};

	class Uncharted2Ubo
		: public c3d::UboT< Uncharted2UboConfiguration >
	{
	private:
		using Configuration = Uncharted2UboConfiguration;

	public:
		explicit Uncharted2Ubo( c3d::RenderDevice const & device );

		void update( Configuration const & config );

	public:
		static const c3d::String Buffer;
		static const c3d::String Data;
	};
}

#define C3D_Uncharted2( writer, binding, set )\
	sdw::UniformBuffer uncharted2{ writer\
		, Uncharted2::Uncharted2Ubo::Buffer\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_uncharted2Data = uncharted2.declMember< Uncharted2::Uncharted2Data >( Uncharted2::Uncharted2Ubo::Data );\
	uncharted2.end()

#endif
