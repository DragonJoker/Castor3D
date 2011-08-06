//*************************************************************************************************

template <> struct FrameVariableCreator<OneIntFrameVariable>
{
	static shared_ptr<OneIntFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create1IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Point2iFrameVariable>
{
	static shared_ptr<Point2iFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Point3iFrameVariable>
{
	static shared_ptr<Point3iFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Point4iFrameVariable>
{
	static shared_ptr<Point4iFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<OneUIntFrameVariable>
{
	static shared_ptr<OneUIntFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create1UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Point2uiFrameVariable>
{
	static shared_ptr<Point2uiFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Point3uiFrameVariable>
{
	static shared_ptr<Point3uiFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Point4uiFrameVariable>
{
	static shared_ptr<Point4uiFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<OneFloatFrameVariable>
{
	static shared_ptr<OneFloatFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create1FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Point2fFrameVariable>
{
	static shared_ptr<Point2fFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Point3fFrameVariable>
{
	static shared_ptr<Point3fFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Point4fFrameVariable>
{
	static shared_ptr<Point4fFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<OneDoubleFrameVariable>
{
	static shared_ptr<OneDoubleFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create1DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Point2dFrameVariable>
{
	static shared_ptr<Point2dFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Point3dFrameVariable>
{
	static shared_ptr<Point3dFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Point4dFrameVariable>
{
	static shared_ptr<Point4dFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix2x2iFrameVariable>
{
	static shared_ptr<Matrix2x2iFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2x2IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix2x3iFrameVariable>
{
	static shared_ptr<Matrix2x3iFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2x3IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix2x4iFrameVariable>
{
	static shared_ptr<Matrix2x4iFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2x4IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix3x2iFrameVariable>
{
	static shared_ptr<Matrix3x2iFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3x2IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix3x3iFrameVariable>
{
	static shared_ptr<Matrix3x3iFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3x3IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix3x4iFrameVariable>
{
	static shared_ptr<Matrix3x4iFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3x4IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix4x2iFrameVariable>
{
	static shared_ptr<Matrix4x2iFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4x2IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix4x3iFrameVariable>
{
	static shared_ptr<Matrix4x3iFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4x3IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix4x4iFrameVariable>
{
	static shared_ptr<Matrix4x4iFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4x4IntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix2x2uiFrameVariable>
{
	static shared_ptr<Matrix2x2uiFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2x2UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix2x3uiFrameVariable>
{
	static shared_ptr<Matrix2x3uiFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2x3UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix2x4uiFrameVariable>
{
	static shared_ptr<Matrix2x4uiFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2x4UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix3x2uiFrameVariable>
{
	static shared_ptr<Matrix3x2uiFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3x2UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix3x3uiFrameVariable>
{
	static shared_ptr<Matrix3x3uiFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3x3UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix3x4uiFrameVariable>
{
	static shared_ptr<Matrix3x4uiFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3x4UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix4x2uiFrameVariable>
{
	static shared_ptr<Matrix4x2uiFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4x2UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix4x3uiFrameVariable>
{
	static shared_ptr<Matrix4x3uiFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4x3UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix4x4uiFrameVariable>
{
	static shared_ptr<Matrix4x4uiFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4x4UIntVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix2x2fFrameVariable>
{
	static shared_ptr<Matrix2x2fFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2x2FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix2x3fFrameVariable>
{
	static shared_ptr<Matrix2x3fFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2x3FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix2x4fFrameVariable>
{
	static shared_ptr<Matrix2x4fFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2x4FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix3x2fFrameVariable>
{
	static shared_ptr<Matrix3x2fFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3x2FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix3x3fFrameVariable>
{
	static shared_ptr<Matrix3x3fFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3x3FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix3x4fFrameVariable>
{
	static shared_ptr<Matrix3x4fFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3x4FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix4x2fFrameVariable>
{
	static shared_ptr<Matrix4x2fFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4x2FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix4x3fFrameVariable>
{
	static shared_ptr<Matrix4x3fFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4x3FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix4x4fFrameVariable>
{
	static shared_ptr<Matrix4x4fFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4x4FloatVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix2x2dFrameVariable>
{
	static shared_ptr<Matrix2x2dFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2x2DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix2x3dFrameVariable>
{
	static shared_ptr<Matrix2x3dFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2x3DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix2x4dFrameVariable>
{
	static shared_ptr<Matrix2x4dFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create2x4DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix3x2dFrameVariable>
{
	static shared_ptr<Matrix3x2dFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3x2DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix3x3dFrameVariable>
{
	static shared_ptr<Matrix3x3dFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3x3DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix3x4dFrameVariable>
{
	static shared_ptr<Matrix3x4dFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create3x4DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix4x2dFrameVariable>
{
	static shared_ptr<Matrix4x2dFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4x2DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix4x3dFrameVariable>
{
	static shared_ptr<Matrix4x3dFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4x3DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

template <> struct FrameVariableCreator<Matrix4x4dFrameVariable>
{
	static shared_ptr<Matrix4x4dFrameVariable> Create( ShaderProgramBase * p_pProgram, int p_iNbOcc, ShaderObjectPtr p_pTarget)
	{
		return p_pProgram->_create4x4DoubleVariable( p_iNbOcc, p_pTarget);
	}
};

//*************************************************************************************************

template <class Ty>
inline shared_ptr<Ty> ShaderProgramBase :: CreateFrameVariable( int p_iNbOcc, eSHADER_TYPE p_eTarget)
{
	CASTOR_ASSERT( p_eTarget > eSHADER_TYPE_NONE && p_eTarget < eSHADER_TYPE_COUNT);
	shared_ptr<Ty> l_pReturn = ShaderProgramBase::CreateFrameVariable<Ty>( p_iNbOcc, m_pShaders[p_eTarget]);
	return l_pReturn;
}

template <class Ty>
inline shared_ptr<Ty> ShaderProgramBase :: CreateFrameVariable( int p_iNbOcc, ShaderObjectPtr p_pTarget)
{
	shared_ptr<Ty> l_pReturn = FrameVariableCreator<Ty>::Create( this, p_iNbOcc, p_pTarget);
	m_listFrameVariables.push_back( l_pReturn);
	p_pTarget->AddFrameVariable( l_pReturn);
	return l_pReturn;
}

//*************************************************************************************************