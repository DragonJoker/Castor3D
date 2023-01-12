#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"

#include <ShaderAST/Expr/ExprComma.hpp>
#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	namespace shader
	{
		//***********************************************************************************************

		sdw::expr::ExprList makeFnArg( sdw::ShaderWriter & writer
			, OutputComponents const & value )
		{
			sdw::expr::ExprList result;
			auto args = sdw::makeFnArg( writer, value.m_diffuse );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( writer, value.m_specular );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( writer, value.m_scattering );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( writer, value.m_coatingSpecular );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( writer, value.m_sheen );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			return result;
		}

		//***********************************************************************************************

		OutputComponents::OutputComponents( sdw::ShaderWriter & writer
			, bool isParam )
			: OutputComponents{ ( isParam
					? sdw::InOutVec3{ writer, "outDiffuse" }
					: sdw::InOutVec3{ writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ) } )
				, ( isParam
					? sdw::InOutVec3{ writer, "outSpecular" }
					: sdw::InOutVec3{ writer.declLocale( "lightSpecular", vec3( 0.0_f ) ) } )
				, ( isParam
					? sdw::InOutVec3{ writer, "outScattering" }
					: sdw::InOutVec3{ writer.declLocale( "lightScattering", vec3( 0.0_f ) ) } )
				, ( isParam
					? sdw::InOutVec3{ writer, "outCoatingSpecular" }
					: sdw::InOutVec3{ writer.declLocale( "lightCoatingSpecular", vec3( 0.0_f ) ) } )
				, ( isParam
					? sdw::InOutVec2{ writer, "outSheen" }
					: sdw::InOutVec2{ writer.declLocale( "lightSheen", vec2( 0.0_f ) ) } ) }
		{
		}

		OutputComponents::OutputComponents( sdw::InOutVec3 const & diffuse
			, sdw::InOutVec3 const & specular
			, sdw::InOutVec3 const & scattering
			, sdw::InOutVec3 const & coatingSpecular
			, sdw::InOutVec2 const & sheen )
			: m_diffuse{ diffuse }
			, m_specular{ specular }
			, m_scattering{ scattering }
			, m_coatingSpecular{ coatingSpecular }
			, m_sheen{ sheen }
			, m_expr{ sdw::expr::makeComma( makeExpr( m_diffuse )
				, sdw::expr::makeComma( makeExpr( m_specular )
					, sdw::expr::makeComma( makeExpr( m_scattering )
						, sdw::expr::makeComma( makeExpr( m_coatingSpecular )
							, makeExpr( m_sheen ) ) ) ) ) }
		{
		}

		sdw::expr::Expr * OutputComponents::getExpr()const
		{
			return m_expr.get();
		}

		sdw::ShaderWriter * OutputComponents::getWriter()const
		{
			return findWriter( m_diffuse, m_specular, m_scattering, m_coatingSpecular );
		}

		void OutputComponents::setVar( sdw::var::VariableList::const_iterator & var )
		{
			m_diffuse.setVar( var );
			m_specular.setVar( var );
			m_scattering.setVar( var );
			m_coatingSpecular.setVar( var );
			m_sheen.setVar( var );
		}
	}
}
