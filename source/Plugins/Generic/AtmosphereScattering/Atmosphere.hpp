/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereConfig_H___
#define ___C3DAS_AtmosphereConfig_H___

#include "AtmosphereScatteringUbo.hpp"

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/BaseTypes/CombinedImage.hpp>
#include <ShaderWriter/CompositeTypes/Function.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace atmosphere_scattering
{
	template< sdw::var::Flag FlagT >
	struct SurfaceT
		: public sdw::StructInstance
	{
		SurfaceT( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, u{ this->getMember< sdw::Vec2 >( "u" ) }
			, P{ this->getMember< sdw::Vec3 >( "P" ) }
		{
		}

		SDW_DeclStructInstance( , SurfaceT );

		static ast::type::IOStructPtr makeIOType( ast::type::TypesCache & cache )
		{
			auto result = cache.getIOStruct( ast::type::MemoryLayout::eC
				, "C3DAS_" + ( FlagT == sdw::var::Flag::eShaderOutput
					? std::string{ "Output" }
					: std::string{ "Input" } ) + "Surface"
				, FlagT );

			if ( result->empty() )
			{
				uint32_t index = 0u;
				result->declMember( "u"
					, ast::type::Kind::eVec2F
					, ast::type::NotArray
					, index++ );
				result->declMember( "P"
					, ast::type::Kind::eVec3F
					, ast::type::NotArray
					, index++ );
			}

			return result;
		}

		sdw::Vec2 u;
		sdw::Vec3 P;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;
	};

	struct AtmosphereConfig
	{
		AtmosphereConfig( sdw::ShaderWriter & pwriter
			, AtmosphereData const & patmosphereData );

		sdw::Vec3 getTransmittance( sdw::Float const & pr
			, sdw::Float const & pmu
			, sdw::CombinedImage2DRgba32 transmittanceMap );
		sdw::Vec3 getSkyRadiance( sdw::Vec3 const & camera
			, sdw::Vec3 const & viewdir
			, sdw::Vec3 const & sundir
			, sdw::CombinedImage2DRgba32 const & transmittanceMap
			, sdw::CombinedImage3DRgba32 const & inscatterMap
			, sdw::Vec3 & extinction );
		sdw::Vec3 getIrradiance( sdw::CombinedImage2DRgba32 sampler
				, sdw::Float r
				, sdw::Float muS );
		sdw::Vec3 getSkyIrradiance( sdw::CombinedImage2DRgba32 skyIrradiance
			, sdw::Float const & r
			, sdw::Float const & muS );

	private:
		sdw::Vec2 getTransmittanceUV( sdw::Float const & pr
			, sdw::Float const & pmu );
		void getTransmittanceRMu( sdw::Vec2 const & pfragCoord
			, sdw::Float & pr
			, sdw::Float & pmuS );
		sdw::Vec2 getIrradianceUV( sdw::Float const & pr
			, sdw::Float const & pmuS );
		void getIrradianceRMuS( sdw::Vec2 const & pfragCoord
			, sdw::Float & pr
			, sdw::Float & pmuS );
		sdw::Vec4 texture4D( sdw::CombinedImage3DRgba32 ptable
			, sdw::Float pr
			, sdw::Float pmu
			, sdw::Float pmuS
			, sdw::Float pnu );
		void getMuMuSNu( sdw::Float const & pr
			, sdw::Vec4 const & pdhdH
			, sdw::Vec2 const & pfragCoord
			, sdw::Float & pmu
			, sdw::Float & pmuS
			, sdw::Float & pnu );
		sdw::Float phaseFunctionR( sdw::Float const & mu );
		sdw::Float phaseFunctionM( sdw::Float const & mu );
		sdw::Vec3 getMie( sdw::Vec4 const & rayMie );

	private:
		sdw::ShaderWriter & writer;
		AtmosphereData const & atmosphereData;
		bool transmittanceNonLinear{ true };
		bool inscatterNonLinear{ true };

	public:
		// Mie
		// DEFAULT
		sdw::Vec3 betaMEx;
		// CLEAR SKY
		/*float HM = 1.2 * SCALE;
		vec3 betaMSca = vec3(20e-3) / SCALE;
		vec3 betaMEx = betaMSca / 0.9;
		float mieG = 0.76;*/
		// PARTLY CLOUDY
		/*float HM = 3.0 * SCALE;
		vec3 betaMSca = vec3(3e-3) / SCALE;
		vec3 betaMEx = betaMSca / 0.9;
		float mieG = 0.65;*/

	private:
		sdw::Function< sdw::Vec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InCombinedImage2DRgba32 > m_getTransmittance;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InCombinedImage2DRgba32
			, sdw::InCombinedImage3DRgba32
			, sdw::OutVec3 > m_getSkyRadiance;
		sdw::Function< sdw::Vec3
			, sdw::InCombinedImage2DRgba32
			, sdw::InFloat
			, sdw::InFloat > m_getIrradiance;
		sdw::Function< sdw::Vec3
			, sdw::InCombinedImage2DRgba32
			, sdw::InFloat
			, sdw::InFloat > m_getSkyIrradiance;

		sdw::Function< sdw::Vec2
			, sdw::InFloat
			, sdw::InFloat > m_getTransmittanceUV;
		sdw::Function< sdw::Void
			, sdw::InVec2
			, sdw::OutFloat
			, sdw::OutFloat > m_getTransmittanceRMu;
		sdw::Function< sdw::Vec2
			, sdw::InFloat
			, sdw::InFloat > m_getIrradianceUV;
		sdw::Function< sdw::Void
			, sdw::InVec2
			, sdw::OutFloat
			, sdw::OutFloat > m_getIrradianceRMuS;
		sdw::Function< sdw::Vec4
			, sdw::InCombinedImage3DRgba32
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_texture4D;
		sdw::Function< sdw::Void
			, sdw::InFloat
			, sdw::InVec4
			, sdw::InVec2
			, sdw::OutFloat
			, sdw::OutFloat
			, sdw::OutFloat > m_getMuMuSNu;
		sdw::Function< sdw::Float
			, sdw::InFloat > m_phaseFunctionR;
		sdw::Function< sdw::Float
			, sdw::InFloat > m_phaseFunctionM;
		sdw::Function< sdw::Vec3
			, sdw::InVec4 > m_getMie;
	};
}

#endif
