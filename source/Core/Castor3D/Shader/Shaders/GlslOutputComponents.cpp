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

			return result;
		}

		//***********************************************************************************************

		OutputComponents::OutputComponents( sdw::ShaderWriter & writer )
			: OutputComponents{ { writer, "outDiffuse" }
				, { writer, "outSpecular" }
				, { writer, "outScattering" }
				, { writer, "coatingSpecular" } }
		{
		}

		OutputComponents::OutputComponents( sdw::InOutVec3 const & diffuse
			, sdw::InOutVec3 const & specular
			, sdw::InOutVec3 const & scattering
			, sdw::InOutVec3 const & coatingSpecular )
			: m_diffuse{ diffuse }
			, m_specular{ specular }
			, m_scattering{ scattering }
			, m_coatingSpecular{ coatingSpecular }
			, m_expr{ sdw::expr::makeComma( makeExpr( m_diffuse )
				, sdw::expr::makeComma( makeExpr( m_specular )
					, sdw::expr::makeComma( makeExpr( m_scattering )
						, makeExpr( m_coatingSpecular ) ) ) ) }
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
		}
	}
}
