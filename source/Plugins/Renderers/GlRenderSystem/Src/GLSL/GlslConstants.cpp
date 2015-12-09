#include "GlslConstants.hpp"

#include "OpenGl.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	namespace GLSL
	{
		//***********************************************************************************************

		ConstantsBase * ConstantsBase::Get( OpenGl const & p_gl )
		{
			if ( p_gl.HasUbo() )
			{
				return &constantsUbo;
			}

			return &constantsStd;
		}

		//***********************************************************************************************

		String ConstantsStd::Matrices()
		{
			return
				cuT( "uniform <mat4> c3d_mtxProjection;\n" )
				cuT( "uniform <mat4> c3d_mtxModel;\n" )
				cuT( "uniform <mat4> c3d_mtxView;\n" )
				cuT( "uniform <mat4> c3d_mtxModelView;\n" )
				cuT( "uniform <mat4> c3d_mtxProjectionView;\n" )
				cuT( "uniform <mat4> c3d_mtxProjectionModelView;\n" )
				cuT( "uniform <mat4> c3d_mtxNormal;\n" )
				cuT( "uniform <mat4> c3d_mtxTexture0;\n" )
				cuT( "uniform <mat4> c3d_mtxTexture1;\n" )
				cuT( "uniform <mat4> c3d_mtxTexture2;\n" )
				cuT( "uniform <mat4> c3d_mtxTexture3;\n" )
				cuT( "uniform <mat4> c3d_mtxBones[100];\n" );
		}

		String ConstantsStd::Scene()
		{
			return
				cuT( "uniform vec4 c3d_v4AmbientLight;\n" )
				cuT( "uniform vec4 c3d_v4BackgroundColour;\n" )
				cuT( "uniform ivec4 c3d_iLightsCount;\n" )
				cuT( "uniform <vec3> c3d_v3CameraPosition;\n" );
		}

		String ConstantsStd::Pass()
		{
			return
				cuT( "uniform vec4 c3d_v4MatAmbient;\n" )
				cuT( "uniform vec4 c3d_v4MatDiffuse;\n" )
				cuT( "uniform vec4 c3d_v4MatEmissive;\n" )
				cuT( "uniform vec4 c3d_v4MatSpecular;\n" )
				cuT( "uniform float c3d_fMatShininess;\n" )
				cuT( "uniform float c3d_fMatOpacity;\n" );
		}

		String ConstantsStd::Billboard()
		{
			return
				cuT( "uniform ivec2 c3d_v2iDimensions;" );
		}

		//***********************************************************************************************

		String ConstantsUbo::Matrices()
		{
			return
				cuT( "<layout>uniform Matrices\n" )
				cuT( "{\n" )
				cuT( "	<mat4> c3d_mtxProjection;\n" )
				cuT( "	<mat4> c3d_mtxModel;\n" )
				cuT( "	<mat4> c3d_mtxView;\n" )
				cuT( "	<mat4> c3d_mtxModelView;\n" )
				cuT( "	<mat4> c3d_mtxProjectionView;\n" )
				cuT( "	<mat4> c3d_mtxProjectionModelView;\n" )
				cuT( "	<mat4> c3d_mtxNormal;\n" )
				cuT( "	<mat4> c3d_mtxTexture0;\n" )
				cuT( "	<mat4> c3d_mtxTexture1;\n" )
				cuT( "	<mat4> c3d_mtxTexture2;\n" )
				cuT( "	<mat4> c3d_mtxTexture3;\n" )
				cuT( "	<mat4> c3d_mtxBones[100];\n" )
				cuT( "};\n" );
		}

		String ConstantsUbo::Scene()
		{
			return
				cuT( "<layout>uniform Scene\n" )
				cuT( "{\n" )
				cuT( "	vec4 c3d_v4AmbientLight;\n" )
				cuT( "	vec4 c3d_v4BackgroundColour;\n" )
				cuT( "	ivec4 c3d_iLightsCount;\n" )
				cuT( "	<vec3> c3d_v3CameraPosition;\n" )
				cuT( "};\n" );
		}

		String ConstantsUbo::Pass()
		{
			return
				cuT( "<layout>uniform Pass\n" )
				cuT( "{\n" )
				cuT( "	vec4 c3d_v4MatAmbient;\n" )
				cuT( "	vec4 c3d_v4MatDiffuse;\n" )
				cuT( "	vec4 c3d_v4MatEmissive;\n" )
				cuT( "	vec4 c3d_v4MatSpecular;\n" )
				cuT( "	float c3d_fMatShininess;\n" )
				cuT( "	float c3d_fMatOpacity;\n" )
				cuT( "};\n" );
		}

		String ConstantsUbo::Billboard()
		{
			return
				cuT( "<layout>uniform User\n" )
				cuT( "{\n" )
				cuT( "	ivec2 c3d_v2iDimensions;\n" )
				cuT( "};\n" );
		}

		//***********************************************************************************************
	}
}
