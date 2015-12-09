#include "GlslVariables.hpp"

#include "OpenGl.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	namespace GLSL
	{
		//***********************************************************************************************

		VariablesBase * VariablesBase::Get( OpenGl const & p_gl )
		{
			if ( p_gl.HasUbo() )
			{
				return &variablesUbo;
			}

			return &variablesStd;
		}

		//***********************************************************************************************

		String VariablesUbo::GetVertexOutMatrices()const
		{
			return cuT( "" );
		}

		String VariablesUbo::GetVertexMatrixCopy()const
		{
			return cuT( "" );
		}

		String VariablesUbo::GetPixelInMatrices()const
		{
			return constantsUbo.Matrices();
		}

		String VariablesUbo::GetPixelMtxModelView()const
		{
			return cuT( "c3d_mtxModelView" );
		}

		//***********************************************************************************************

		String VariablesStd::GetVertexOutMatrices()const
		{
			return
				cuT( "out mat4 vtx_mtxModelView;\n" )
				cuT( "out mat4 vtx_mtxView;" );
		}

		String VariablesStd::GetVertexMatrixCopy()const
		{
			return
				cuT( "	vtx_mtxModelView = c3d_mtxModelView;\n" )
				cuT( "	vtx_mtxView = c3d_mtxView;" );
		}

		String VariablesStd::GetPixelInMatrices()const
		{
			return
				cuT( "in mat4 vtx_mtxModelView;\n" )
				cuT( "in mat4 vtx_mtxView;" );
		}

		String VariablesStd::GetPixelMtxModelView()const
		{
			return cuT( "vtx_mtxModelView" );
		}

		//***********************************************************************************************
	}
}
