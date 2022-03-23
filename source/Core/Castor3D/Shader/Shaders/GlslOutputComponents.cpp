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

			return result;
		}

		//***********************************************************************************************

		OutputComponents::OutputComponents( sdw::ShaderWriter & writer )
			: OutputComponents{ { writer, "outDiffuse" }, { writer, "outSpecular" } }
		{
		}

		OutputComponents::OutputComponents( sdw::InOutVec3 const & diffuse
			, sdw::InOutVec3 const & specular )
			: m_diffuse{ diffuse }
			, m_specular{ specular }
			, m_expr{ sdw::expr::makeComma( makeExpr( m_diffuse ), makeExpr( m_specular ) ) }
		{
		}

		sdw::expr::Expr * OutputComponents::getExpr()const
		{
			return m_expr.get();
		}

		sdw::ShaderWriter * OutputComponents::getWriter()const
		{
			return findWriter( m_diffuse, m_specular );
		}

		void OutputComponents::setVar( sdw::var::VariableList::const_iterator & var )
		{
			m_diffuse.setVar( var );
			m_specular.setVar( var );
		}
	}
}
