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
			auto args = sdw::makeFnArg( writer, value.diffuse );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( writer, value.specular );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( writer, value.scattering );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( writer, value.coatingSpecular );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( writer, value.sheen );

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

		OutputComponents::OutputComponents( sdw::InOutVec3 const & pdiffuse
			, sdw::InOutVec3 const & pspecular
			, sdw::InOutVec3 const & pscattering
			, sdw::InOutVec3 const & pcoatingSpecular
			, sdw::InOutVec2 const & psheen )
			: diffuse{ pdiffuse }
			, specular{ pspecular }
			, scattering{ pscattering }
			, coatingSpecular{ pcoatingSpecular }
			, sheen{ psheen }
			, m_expr{ sdw::expr::makeComma( makeExpr( diffuse )
				, sdw::expr::makeComma( makeExpr( specular )
					, sdw::expr::makeComma( makeExpr( scattering )
						, sdw::expr::makeComma( makeExpr( coatingSpecular )
							, makeExpr( sheen ) ) ) ) ) }
		{
		}

		sdw::expr::Expr * OutputComponents::getExpr()const
		{
			return m_expr.get();
		}

		sdw::ShaderWriter * OutputComponents::getWriter()const
		{
			return findWriter( diffuse, specular, scattering, coatingSpecular );
		}

		void OutputComponents::setVar( sdw::var::VariableList::const_iterator & var )
		{
			diffuse.setVar( var );
			specular.setVar( var );
			scattering.setVar( var );
			coatingSpecular.setVar( var );
			sheen.setVar( var );
		}
	}
}
