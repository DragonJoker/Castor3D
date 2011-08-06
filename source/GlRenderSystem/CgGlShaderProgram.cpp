#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/CgGlShaderProgram.hpp"
#include "GlRenderSystem/CgGlShaderObject.hpp"
#include "GlRenderSystem/CgGlFrameVariable.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/OpenGl.hpp"

using namespace Castor3D;

CgGlShaderProgram :: CgGlShaderProgram()
	:	CgShaderProgram()
{
}

CgGlShaderProgram :: CgGlShaderProgram( const StringArray & p_shaderFiles)
	:	CgShaderProgram( p_shaderFiles)
{
}

CgGlShaderProgram :: ~CgGlShaderProgram()
{
}

int CgGlShaderProgram :: GetAttributeLocation( String const & p_strName)const
{
	int l_iReturn = GL_INVALID_INDEX;
	CgShaderObjectPtr l_pShader = static_pointer_cast<CgShaderObject>( m_pShaders[eSHADER_TYPE_VERTEX]);

	if (l_pShader)
	{
		CGprogram l_program = l_pShader->GetProgram();

		if (l_program)
		{
			GLuint l_uiProgram = cgGLGetProgramID( l_program);

			if (l_uiProgram != GL_INVALID_INDEX && l_uiProgram != 0)
			{
				l_iReturn = OpenGl::GetAttribLocation( l_uiProgram, p_strName.char_str());
			}
		}
	}

	return l_iReturn;
}

ShaderObjectPtr CgGlShaderProgram :: _createObject(eSHADER_TYPE p_eType)
{
	ShaderObjectPtr l_pReturn( new CgGlShaderObject( this, p_eType));
	return l_pReturn;
}

shared_ptr<OneIntFrameVariable> CgGlShaderProgram :: _create1IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<OneIntFrameVariable> l_pReturn( new CgGlOneFrameVariable<int>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point2iFrameVariable> CgGlShaderProgram :: _create2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point2iFrameVariable> l_pReturn( new CgGlPointFrameVariable<int, 2>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point3iFrameVariable> CgGlShaderProgram :: _create3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point3iFrameVariable> l_pReturn( new CgGlPointFrameVariable<int, 3>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point4iFrameVariable> CgGlShaderProgram :: _create4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point4iFrameVariable> l_pReturn( new CgGlPointFrameVariable<int, 4>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<OneUIntFrameVariable> CgGlShaderProgram :: _create1UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<OneUIntFrameVariable>();
}

shared_ptr<Point2uiFrameVariable> CgGlShaderProgram :: _create2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Point2uiFrameVariable>();
}

shared_ptr<Point3uiFrameVariable> CgGlShaderProgram :: _create3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Point3uiFrameVariable>();
}

shared_ptr<Point4uiFrameVariable> CgGlShaderProgram :: _create4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Point4uiFrameVariable>();
}

shared_ptr<OneFloatFrameVariable> CgGlShaderProgram :: _create1FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<OneFloatFrameVariable> l_pReturn( new CgGlOneFrameVariable<float>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point2fFrameVariable> CgGlShaderProgram :: _create2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point2fFrameVariable> l_pReturn( new CgGlPointFrameVariable<float, 2>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point3fFrameVariable> CgGlShaderProgram :: _create3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point3fFrameVariable> l_pReturn( new CgGlPointFrameVariable<float, 3>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point4fFrameVariable> CgGlShaderProgram :: _create4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point4fFrameVariable> l_pReturn( new CgGlPointFrameVariable<float, 4>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<OneDoubleFrameVariable> CgGlShaderProgram :: _create1DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<OneDoubleFrameVariable> l_pReturn( new CgGlOneFrameVariable<double>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point2dFrameVariable> CgGlShaderProgram :: _create2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point2dFrameVariable> l_pReturn( new CgGlPointFrameVariable<double, 2>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point3dFrameVariable> CgGlShaderProgram :: _create3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point3dFrameVariable> l_pReturn( new CgGlPointFrameVariable<double, 3>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point4dFrameVariable> CgGlShaderProgram :: _create4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point4dFrameVariable> l_pReturn( new CgGlPointFrameVariable<double, 4>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x2iFrameVariable> CgGlShaderProgram :: _create2x2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x2iFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<int, 2, 2>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x3iFrameVariable> CgGlShaderProgram :: _create2x3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x3iFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<int, 2, 3>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x4iFrameVariable> CgGlShaderProgram :: _create2x4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x4iFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<int, 2, 4>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x2iFrameVariable> CgGlShaderProgram :: _create3x2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x2iFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<int, 3, 2>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x3iFrameVariable> CgGlShaderProgram :: _create3x3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x3iFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<int, 3, 3>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x4iFrameVariable> CgGlShaderProgram :: _create3x4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x4iFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<int, 3, 4>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x2iFrameVariable> CgGlShaderProgram :: _create4x2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x2iFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<int, 4, 2>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x3iFrameVariable> CgGlShaderProgram :: _create4x3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x3iFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<int, 4, 3>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x4iFrameVariable> CgGlShaderProgram :: _create4x4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x4iFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<int, 4, 4>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x2uiFrameVariable> CgGlShaderProgram :: _create2x2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x2uiFrameVariable>();
}

shared_ptr<Matrix2x3uiFrameVariable> CgGlShaderProgram :: _create2x3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x3uiFrameVariable>();
}

shared_ptr<Matrix2x4uiFrameVariable> CgGlShaderProgram :: _create2x4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x4uiFrameVariable>();
}

shared_ptr<Matrix3x2uiFrameVariable> CgGlShaderProgram :: _create3x2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x2uiFrameVariable>();
}

shared_ptr<Matrix3x3uiFrameVariable> CgGlShaderProgram :: _create3x3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x3uiFrameVariable>();
}

shared_ptr<Matrix3x4uiFrameVariable> CgGlShaderProgram :: _create3x4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x4uiFrameVariable>();
}

shared_ptr<Matrix4x2uiFrameVariable> CgGlShaderProgram :: _create4x2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x2uiFrameVariable>();
}

shared_ptr<Matrix4x3uiFrameVariable> CgGlShaderProgram :: _create4x3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x3uiFrameVariable>();
}

shared_ptr<Matrix4x4uiFrameVariable> CgGlShaderProgram :: _create4x4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x4uiFrameVariable>();
}

shared_ptr<Matrix2x2fFrameVariable> CgGlShaderProgram :: _create2x2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x2fFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<float, 2, 2>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x3fFrameVariable> CgGlShaderProgram :: _create2x3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x3fFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<float, 2, 3>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x4fFrameVariable> CgGlShaderProgram :: _create2x4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x4fFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<float, 2, 4>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x2fFrameVariable> CgGlShaderProgram :: _create3x2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x2fFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<float, 3, 2>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x3fFrameVariable> CgGlShaderProgram :: _create3x3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x3fFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<float, 3, 3>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x4fFrameVariable> CgGlShaderProgram :: _create3x4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x4fFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<float, 3, 4>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x2fFrameVariable> CgGlShaderProgram :: _create4x2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x2fFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<float, 4, 2>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x3fFrameVariable> CgGlShaderProgram :: _create4x3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x3fFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<float, 4, 3>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x4fFrameVariable> CgGlShaderProgram :: _create4x4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x4fFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<float, 4, 4>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x2dFrameVariable> CgGlShaderProgram :: _create2x2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x2dFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<double, 2, 2>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x3dFrameVariable> CgGlShaderProgram :: _create2x3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x3dFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<double, 2, 3>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x4dFrameVariable> CgGlShaderProgram :: _create2x4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x4dFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<double, 2, 4>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x2dFrameVariable> CgGlShaderProgram :: _create3x2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x2dFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<double, 3, 2>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x3dFrameVariable> CgGlShaderProgram :: _create3x3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x3dFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<double, 3, 3>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x4dFrameVariable> CgGlShaderProgram :: _create3x4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x4dFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<double, 3, 4>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x2dFrameVariable> CgGlShaderProgram :: _create4x2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x2dFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<double, 4, 2>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x3dFrameVariable> CgGlShaderProgram :: _create4x3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x3dFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<double, 4, 3>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x4dFrameVariable> CgGlShaderProgram :: _create4x4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x4dFrameVariable> l_pReturn( new CgGlMatrixFrameVariable<double, 4, 4>( this, static_cast<CgGlShaderObject *>( p_pObject.get()), static_pointer_cast<CgGlShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}
