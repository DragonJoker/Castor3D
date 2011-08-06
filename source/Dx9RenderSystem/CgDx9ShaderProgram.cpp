#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/CgDx9ShaderProgram.hpp"
#include "Dx9RenderSystem/CgDx9ShaderObject.hpp"
#include "Dx9RenderSystem/CgDx9FrameVariable.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"

using namespace Castor3D;

CgDx9ShaderProgram :: CgDx9ShaderProgram()
	:	CgShaderProgram()
{
}

CgDx9ShaderProgram :: CgDx9ShaderProgram( const StringArray & p_shaderFiles)
	:	CgShaderProgram( p_shaderFiles)
{
}

CgDx9ShaderProgram :: ~CgDx9ShaderProgram()
{
}

ShaderObjectPtr CgDx9ShaderProgram :: _createObject(eSHADER_TYPE p_eType)
{
	ShaderObjectPtr l_pReturn( new CgDx9ShaderObject( this, p_eType));
	return l_pReturn;
}

shared_ptr<OneIntFrameVariable> CgDx9ShaderProgram :: _create1IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<OneIntFrameVariable> l_pReturn( new CgDx9OneFrameVariable<int>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point2iFrameVariable> CgDx9ShaderProgram :: _create2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point2iFrameVariable> l_pReturn( new CgDx9PointFrameVariable<int, 2>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point3iFrameVariable> CgDx9ShaderProgram :: _create3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point3iFrameVariable> l_pReturn( new CgDx9PointFrameVariable<int, 3>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point4iFrameVariable> CgDx9ShaderProgram :: _create4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point4iFrameVariable> l_pReturn( new CgDx9PointFrameVariable<int, 4>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<OneUIntFrameVariable> CgDx9ShaderProgram :: _create1UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<OneUIntFrameVariable>();
}

shared_ptr<Point2uiFrameVariable> CgDx9ShaderProgram :: _create2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Point2uiFrameVariable>();
}

shared_ptr<Point3uiFrameVariable> CgDx9ShaderProgram :: _create3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Point3uiFrameVariable>();
}

shared_ptr<Point4uiFrameVariable> CgDx9ShaderProgram :: _create4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Point4uiFrameVariable>();
}

shared_ptr<OneFloatFrameVariable> CgDx9ShaderProgram :: _create1FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<OneFloatFrameVariable> l_pReturn( new CgDx9OneFrameVariable<float>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point2fFrameVariable> CgDx9ShaderProgram :: _create2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point2fFrameVariable> l_pReturn( new CgDx9PointFrameVariable<float, 2>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point3fFrameVariable> CgDx9ShaderProgram :: _create3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point3fFrameVariable> l_pReturn( new CgDx9PointFrameVariable<float, 3>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point4fFrameVariable> CgDx9ShaderProgram :: _create4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point4fFrameVariable> l_pReturn( new CgDx9PointFrameVariable<float, 4>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<OneDoubleFrameVariable> CgDx9ShaderProgram :: _create1DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<OneDoubleFrameVariable> l_pReturn( new CgDx9OneFrameVariable<double>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point2dFrameVariable> CgDx9ShaderProgram :: _create2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point2dFrameVariable> l_pReturn( new CgDx9PointFrameVariable<double, 2>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point3dFrameVariable> CgDx9ShaderProgram :: _create3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point3dFrameVariable> l_pReturn( new CgDx9PointFrameVariable<double, 3>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Point4dFrameVariable> CgDx9ShaderProgram :: _create4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Point4dFrameVariable> l_pReturn( new CgDx9PointFrameVariable<double, 4>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x2iFrameVariable> CgDx9ShaderProgram :: _create2x2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x2iFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<int, 2, 2>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x3iFrameVariable> CgDx9ShaderProgram :: _create2x3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x3iFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<int, 2, 3>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x4iFrameVariable> CgDx9ShaderProgram :: _create2x4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x4iFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<int, 2, 4>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x2iFrameVariable> CgDx9ShaderProgram :: _create3x2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x2iFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<int, 3, 2>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x3iFrameVariable> CgDx9ShaderProgram :: _create3x3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x3iFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<int, 3, 3>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x4iFrameVariable> CgDx9ShaderProgram :: _create3x4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x4iFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<int, 3, 4>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x2iFrameVariable> CgDx9ShaderProgram :: _create4x2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x2iFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<int, 4, 2>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x3iFrameVariable> CgDx9ShaderProgram :: _create4x3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x3iFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<int, 4, 3>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x4iFrameVariable> CgDx9ShaderProgram :: _create4x4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x4iFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<int, 4, 4>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x2uiFrameVariable> CgDx9ShaderProgram :: _create2x2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x2uiFrameVariable>();
}

shared_ptr<Matrix2x3uiFrameVariable> CgDx9ShaderProgram :: _create2x3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x3uiFrameVariable>();
}

shared_ptr<Matrix2x4uiFrameVariable> CgDx9ShaderProgram :: _create2x4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x4uiFrameVariable>();
}

shared_ptr<Matrix3x2uiFrameVariable> CgDx9ShaderProgram :: _create3x2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x2uiFrameVariable>();
}

shared_ptr<Matrix3x3uiFrameVariable> CgDx9ShaderProgram :: _create3x3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x3uiFrameVariable>();
}

shared_ptr<Matrix3x4uiFrameVariable> CgDx9ShaderProgram :: _create3x4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x4uiFrameVariable>();
}

shared_ptr<Matrix4x2uiFrameVariable> CgDx9ShaderProgram :: _create4x2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x2uiFrameVariable>();
}

shared_ptr<Matrix4x3uiFrameVariable> CgDx9ShaderProgram :: _create4x3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x3uiFrameVariable>();
}

shared_ptr<Matrix4x4uiFrameVariable> CgDx9ShaderProgram :: _create4x4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x4uiFrameVariable>();
}

shared_ptr<Matrix2x2fFrameVariable> CgDx9ShaderProgram :: _create2x2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x2fFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<float, 2, 2>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x3fFrameVariable> CgDx9ShaderProgram :: _create2x3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x3fFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<float, 2, 3>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x4fFrameVariable> CgDx9ShaderProgram :: _create2x4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x4fFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<float, 2, 4>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x2fFrameVariable> CgDx9ShaderProgram :: _create3x2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x2fFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<float, 3, 2>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x3fFrameVariable> CgDx9ShaderProgram :: _create3x3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x3fFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<float, 3, 3>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x4fFrameVariable> CgDx9ShaderProgram :: _create3x4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x4fFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<float, 3, 4>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x2fFrameVariable> CgDx9ShaderProgram :: _create4x2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x2fFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<float, 4, 2>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x3fFrameVariable> CgDx9ShaderProgram :: _create4x3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x3fFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<float, 4, 3>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x4fFrameVariable> CgDx9ShaderProgram :: _create4x4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x4fFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<float, 4, 4>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x2dFrameVariable> CgDx9ShaderProgram :: _create2x2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x2dFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<double, 2, 2>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x3dFrameVariable> CgDx9ShaderProgram :: _create2x3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x3dFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<double, 2, 3>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix2x4dFrameVariable> CgDx9ShaderProgram :: _create2x4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix2x4dFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<double, 2, 4>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x2dFrameVariable> CgDx9ShaderProgram :: _create3x2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x2dFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<double, 3, 2>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x3dFrameVariable> CgDx9ShaderProgram :: _create3x3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x3dFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<double, 3, 3>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix3x4dFrameVariable> CgDx9ShaderProgram :: _create3x4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix3x4dFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<double, 3, 4>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x2dFrameVariable> CgDx9ShaderProgram :: _create4x2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x2dFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<double, 4, 2>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x3dFrameVariable> CgDx9ShaderProgram :: _create4x3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x3dFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<double, 4, 3>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}

shared_ptr<Matrix4x4dFrameVariable> CgDx9ShaderProgram :: _create4x4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	shared_ptr<Matrix4x4dFrameVariable> l_pReturn( new CgDx9MatrixFrameVariable<double, 4, 4>( this, static_cast<CgDx9ShaderObject *>( p_pObject.get()), static_pointer_cast<CgDx9ShaderObject>( p_pObject)->GetProgram(), p_iNbOcc));
	return l_pReturn;
}
