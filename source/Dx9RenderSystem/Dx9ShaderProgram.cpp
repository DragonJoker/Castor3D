#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9ShaderProgram.hpp"
#include "Dx9RenderSystem/Dx9ShaderObject.hpp"
#include "Dx9RenderSystem/Dx9FrameVariable.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"

using namespace Castor3D;

Dx9ShaderProgram :: Dx9ShaderProgram()
	:	HlslShaderProgram()
{
}

Dx9ShaderProgram :: Dx9ShaderProgram( const StringArray & p_shaderFiles)
	:	HlslShaderProgram( p_shaderFiles)
{
}

Dx9ShaderProgram :: ~Dx9ShaderProgram()
{
}

void Dx9ShaderProgram :: RetrieveLinkerLog( String & strLog)
{
	if ( ! RenderSystem::UseShaders())
	{
		strLog = D3dEnum::GetHlslErrorString( 0);
		return;
	}
}

ShaderObjectPtr Dx9ShaderProgram :: _createObject(eSHADER_TYPE p_eType)
{
	ShaderObjectPtr l_pReturn;
	switch (p_eType)
	{
	case eSHADER_TYPE_VERTEX:
		l_pReturn.reset( new Dx9VertexShader( this));
		break;

	case eSHADER_TYPE_PIXEL:
		l_pReturn.reset( new Dx9FragmentShader( this));
		break;
	}
	return l_pReturn;
}

shared_ptr<OneIntFrameVariable> Dx9ShaderProgram :: _create1IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<OneIntFrameVariable> l_pReturn( new Dx9OneFrameVariable<int>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Point2iFrameVariable> Dx9ShaderProgram :: _create2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Point2iFrameVariable> l_pReturn( new Dx9PointFrameVariable<int, 2>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Point3iFrameVariable> Dx9ShaderProgram :: _create3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Point3iFrameVariable> l_pReturn( new Dx9PointFrameVariable<int, 3>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Point4iFrameVariable> Dx9ShaderProgram :: _create4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Point4iFrameVariable> l_pReturn( new Dx9PointFrameVariable<int, 4>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<OneUIntFrameVariable> Dx9ShaderProgram :: _create1UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<OneUIntFrameVariable>();
}

shared_ptr<Point2uiFrameVariable> Dx9ShaderProgram :: _create2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Point2uiFrameVariable>();
}

shared_ptr<Point3uiFrameVariable> Dx9ShaderProgram :: _create3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Point3uiFrameVariable>();
}

shared_ptr<Point4uiFrameVariable> Dx9ShaderProgram :: _create4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Point4uiFrameVariable>();
}

shared_ptr<OneFloatFrameVariable> Dx9ShaderProgram :: _create1FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<OneFloatFrameVariable> l_pReturn( new Dx9OneFrameVariable<float>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Point2fFrameVariable> Dx9ShaderProgram :: _create2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Point2fFrameVariable> l_pReturn( new Dx9PointFrameVariable<float, 2>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Point3fFrameVariable> Dx9ShaderProgram :: _create3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Point3fFrameVariable> l_pReturn( new Dx9PointFrameVariable<float, 3>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Point4fFrameVariable> Dx9ShaderProgram :: _create4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Point4fFrameVariable> l_pReturn( new Dx9PointFrameVariable<float, 4>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<OneDoubleFrameVariable> Dx9ShaderProgram :: _create1DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<OneDoubleFrameVariable> l_pReturn( new Dx9OneFrameVariable<double>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Point2dFrameVariable> Dx9ShaderProgram :: _create2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Point2dFrameVariable> l_pReturn( new Dx9PointFrameVariable<double, 2>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Point3dFrameVariable> Dx9ShaderProgram :: _create3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Point3dFrameVariable> l_pReturn( new Dx9PointFrameVariable<double, 3>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Point4dFrameVariable> Dx9ShaderProgram :: _create4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Point4dFrameVariable> l_pReturn( new Dx9PointFrameVariable<double, 4>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix2x2iFrameVariable> Dx9ShaderProgram :: _create2x2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix2x2iFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<int, 2, 2>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix2x3iFrameVariable> Dx9ShaderProgram :: _create2x3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix2x3iFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<int, 2, 3>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix2x4iFrameVariable> Dx9ShaderProgram :: _create2x4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix2x4iFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<int, 2, 4>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix3x2iFrameVariable> Dx9ShaderProgram :: _create3x2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix3x2iFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<int, 3, 2>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix3x3iFrameVariable> Dx9ShaderProgram :: _create3x3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix3x3iFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<int, 3, 3>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix3x4iFrameVariable> Dx9ShaderProgram :: _create3x4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix3x4iFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<int, 3, 4>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix4x2iFrameVariable> Dx9ShaderProgram :: _create4x2IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix4x2iFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<int, 4, 2>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix4x3iFrameVariable> Dx9ShaderProgram :: _create4x3IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix4x3iFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<int, 4, 3>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix4x4iFrameVariable> Dx9ShaderProgram :: _create4x4IntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix4x4iFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<int, 4, 4>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix2x2uiFrameVariable> Dx9ShaderProgram :: _create2x2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x2uiFrameVariable>();
}

shared_ptr<Matrix2x3uiFrameVariable> Dx9ShaderProgram :: _create2x3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x3uiFrameVariable>();
}

shared_ptr<Matrix2x4uiFrameVariable> Dx9ShaderProgram :: _create2x4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix2x4uiFrameVariable>();
}

shared_ptr<Matrix3x2uiFrameVariable> Dx9ShaderProgram :: _create3x2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x2uiFrameVariable>();
}

shared_ptr<Matrix3x3uiFrameVariable> Dx9ShaderProgram :: _create3x3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x3uiFrameVariable>();
}

shared_ptr<Matrix3x4uiFrameVariable> Dx9ShaderProgram :: _create3x4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix3x4uiFrameVariable>();
}

shared_ptr<Matrix4x2uiFrameVariable> Dx9ShaderProgram :: _create4x2UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x2uiFrameVariable>();
}

shared_ptr<Matrix4x3uiFrameVariable> Dx9ShaderProgram :: _create4x3UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x3uiFrameVariable>();
}

shared_ptr<Matrix4x4uiFrameVariable> Dx9ShaderProgram :: _create4x4UIntVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	return shared_ptr<Matrix4x4uiFrameVariable>();
}

shared_ptr<Matrix2x2fFrameVariable> Dx9ShaderProgram :: _create2x2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix2x2fFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<float, 2, 2>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix2x3fFrameVariable> Dx9ShaderProgram :: _create2x3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix2x3fFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<float, 2, 3>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix2x4fFrameVariable> Dx9ShaderProgram :: _create2x4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix2x4fFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<float, 2, 4>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix3x2fFrameVariable> Dx9ShaderProgram :: _create3x2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix3x2fFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<float, 3, 2>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix3x3fFrameVariable> Dx9ShaderProgram :: _create3x3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix3x3fFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<float, 3, 3>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix3x4fFrameVariable> Dx9ShaderProgram :: _create3x4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix3x4fFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<float, 3, 4>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix4x2fFrameVariable> Dx9ShaderProgram :: _create4x2FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix4x2fFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<float, 4, 2>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix4x3fFrameVariable> Dx9ShaderProgram :: _create4x3FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix4x3fFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<float, 4, 3>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix4x4fFrameVariable> Dx9ShaderProgram :: _create4x4FloatVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix4x4fFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<float, 4, 4>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix2x2dFrameVariable> Dx9ShaderProgram :: _create2x2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix2x2dFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<double, 2, 2>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix2x3dFrameVariable> Dx9ShaderProgram :: _create2x3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix2x3dFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<double, 2, 3>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix2x4dFrameVariable> Dx9ShaderProgram :: _create2x4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix2x4dFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<double, 2, 4>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix3x2dFrameVariable> Dx9ShaderProgram :: _create3x2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix3x2dFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<double, 3, 2>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix3x3dFrameVariable> Dx9ShaderProgram :: _create3x3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix3x3dFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<double, 3, 3>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix3x4dFrameVariable> Dx9ShaderProgram :: _create3x4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix3x4dFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<double, 3, 4>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix4x2dFrameVariable> Dx9ShaderProgram :: _create4x2DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix4x2dFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<double, 4, 2>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix4x3dFrameVariable> Dx9ShaderProgram :: _create4x3DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix4x3dFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<double, 4, 3>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}

shared_ptr<Matrix4x4dFrameVariable> Dx9ShaderProgram :: _create4x4DoubleVariable( int p_iNbOcc, ShaderObjectPtr p_pObject)
{
	Dx9ShaderObjectPtr l_pObject = static_pointer_cast<Dx9ShaderObject>( p_pObject);
	shared_ptr<Matrix4x4dFrameVariable> l_pReturn( new Dx9MatrixFrameVariable<double, 4, 4>( this, l_pObject.get(), p_iNbOcc, l_pObject->GetInputConstantsPtr()));
	return l_pReturn;
}
