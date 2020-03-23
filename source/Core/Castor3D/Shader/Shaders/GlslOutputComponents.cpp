#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"

#include <ShaderAST/Expr/ExprComma.hpp>
#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		//***********************************************************************************************

		ast::expr::ExprList makeFnArg( ast::Shader & shader
			, OutputComponents const & value )
		{
			ast::expr::ExprList result;
			auto args = sdw::makeFnArg( shader, value.m_diffuse );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			args = sdw::makeFnArg( shader, value.m_specular );

			for ( auto & expr : args )
			{
				result.emplace_back( std::move( expr ) );
			}

			return result;
		}

		//***********************************************************************************************

		OutputComponents::OutputComponents( ShaderWriter & writer )
			: OutputComponents{ { writer, "outDiffuse" }, { writer, "outSpecular" } }
		{
		}

		OutputComponents::OutputComponents( InOutVec3 const & diffuse
			, InOutVec3 const & specular )
			: m_diffuse{ diffuse }
			, m_specular{ specular }
			, m_expr{ expr::makeComma( makeExpr( m_diffuse ), makeExpr( m_specular ) ) }
		{
		}

		ast::expr::Expr * OutputComponents::getExpr()const
		{
			return m_expr.get();
		}

		ast::Shader * OutputComponents::getShader()const
		{
			return findShader( m_diffuse, m_specular );
		}

		void OutputComponents::setVar( ast::var::VariableList::const_iterator & var )
		{
			m_diffuse.setVar( var );
			m_specular.setVar( var );
		}
	}
}
