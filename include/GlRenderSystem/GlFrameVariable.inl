template <> struct OneVariableApplyer<int>
{
	static inline void Apply( GLint p_iGlIndex, int const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform1iv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<int, 1>
{
	static inline void Apply( GLint p_iGlIndex, int const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform1iv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<int, 2>
{
	static inline void Apply( GLint p_iGlIndex, int const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform2iv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<int, 3>
{
	static inline void Apply( GLint p_iGlIndex, int const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform3iv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<int, 4>
{
	static inline void Apply( GLint p_iGlIndex, int const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform4iv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct OneVariableApplyer<unsigned int>
{
	static inline void Apply( GLint p_iGlIndex, unsigned int const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform1uiv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<unsigned int, 1>
{
	static inline void Apply( GLint p_iGlIndex, unsigned int const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform1uiv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<unsigned int, 2>
{
	static inline void Apply( GLint p_iGlIndex, unsigned int const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform2uiv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<unsigned int, 3>
{
	static inline void Apply( GLint p_iGlIndex, unsigned int const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform3uiv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<unsigned int, 4>
{
	static inline void Apply( GLint p_iGlIndex, unsigned int const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform4uiv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct OneVariableApplyer<float>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform1fv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<float, 1>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform1fv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<float, 2>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform2fv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<float, 3>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform3fv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<float, 4>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform4fv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct OneVariableApplyer<double>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform1dv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<double, 1>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform1dv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<double, 2>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform2dv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<double, 3>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform3dv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct PointVariableApplyer<double, 4>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::Uniform4dv( p_iGlIndex, p_uiOcc, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<float, 2, 2>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix2fv( p_iGlIndex, 4 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<float, 2, 3>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix2x3fv( p_iGlIndex, 6 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<float, 2, 4>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix2x4fv( p_iGlIndex, 8 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<float, 3, 2>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix3x2fv( p_iGlIndex, 6 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<float, 3, 3>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix3fv( p_iGlIndex, 9 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<float, 3, 4>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix3x4fv( p_iGlIndex, 12 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<float, 4, 2>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix4x2fv( p_iGlIndex, 8 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<float, 4, 3>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix4x3fv( p_iGlIndex, 12 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<float, 4, 4>
{
	static inline void Apply( GLint p_iGlIndex, float const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix4fv( p_iGlIndex, 16 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<double, 2, 2>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix2dv( p_iGlIndex, 4 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<double, 2, 3>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix2x3dv( p_iGlIndex, 6 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<double, 2, 4>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix2x4dv( p_iGlIndex, 8 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<double, 3, 2>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix3x2dv( p_iGlIndex, 6 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<double, 3, 3>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix3dv( p_iGlIndex, 9 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<double, 3, 4>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix3x4dv( p_iGlIndex, 12 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<double, 4, 2>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix4x2dv( p_iGlIndex, 8 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<double, 4, 3>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix4x3dv( p_iGlIndex, 12 * p_uiOcc, false, p_pValue);
	}
};

template <> struct MatrixVariableApplyer<double, 4, 4>
{
	static inline void Apply( GLint p_iGlIndex, double const * p_pValue, size_t p_uiOcc)
	{
		OpenGl::UniformMatrix4dv( p_iGlIndex, 16 * p_uiOcc, false, p_pValue);
	}
};

template <typename T> struct OneVariableApplyer
{
	static inline void Apply( GLint p_iGlIndex, T const * p_pValue, size_t p_uiOcc)
	{
		CASTOR_EXCEPTION( "OneVariableApplyer - Unsupported arguments");
	}
};

template <typename T, size_t Count> struct PointVariableApplyer
{
	static inline void Apply( GLint p_iGlIndex, T const * p_pValue, size_t p_uiOcc)
	{
		CASTOR_EXCEPTION( "PointVariableApplyer - Unsupported arguments");
	}
};

template <typename T, size_t Rows, size_t Columns> struct MatrixVariableApplyer
{
	static inline void Apply( GLint p_iGlIndex, T const * p_pValue, size_t p_uiOcc)
	{
		CASTOR_EXCEPTION( "MatrixVariableApplyer - Unsupported arguments");
	}
};

//***********************************************************************************************************************

template <typename Type>
inline void GlFrameVariableBase :: _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc)
{
	if (m_bPresentInProgram)
	{
		if (m_iGlIndex == GL_INVALID_INDEX)
		{
			GetVariableLocation( p_strName.char_str());
		}
		if (m_iGlIndex != GL_INVALID_INDEX)
		{
			OneVariableApplyer<Type>::Apply( m_iGlIndex, p_pValue, p_uiOcc);
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}
template <typename Type, int Count>
inline void GlFrameVariableBase :: _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc)
{
	if (m_bPresentInProgram)
	{
		if (m_iGlIndex == GL_INVALID_INDEX)
		{
			GetVariableLocation( p_strName.char_str());
		}
		if (m_iGlIndex != GL_INVALID_INDEX)
		{
			PointVariableApplyer<Type, Count>::Apply( m_iGlIndex, p_pValue, p_uiOcc);
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}
template <typename Type, int Rows, int Columns>
inline void GlFrameVariableBase :: _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc)
{
	if (m_bPresentInProgram)
	{
		if (m_iGlIndex == GL_INVALID_INDEX)
		{
			GetVariableLocation( p_strName.char_str());
		}
		if (m_iGlIndex != GL_INVALID_INDEX)
		{
			MatrixVariableApplyer<Type, Rows, Columns>::Apply( m_iGlIndex, p_pValue, p_uiOcc);
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}

//***********************************************************************************************************************

template <typename T>
GlOneFrameVariable<T> :: GlOneFrameVariable( GlShaderProgram * p_pProgram, GlShaderObject * p_pObject, size_t p_uiOcc)
	:	GlFrameVariableBase( p_pProgram->GetGlProgram())
	,	OneFrameVariable<T>( p_pProgram, p_pObject, p_uiOcc)
{
}

template <typename T>
GlOneFrameVariable<T> :: GlOneFrameVariable( OneFrameVariable<T> * p_pVariable)
	:	GlFrameVariableBase( static_cast<GlShaderProgram *>( p_pVariable->GetProgram())->GetGlProgram())
	,	OneFrameVariable<T>( * p_pVariable)
{
}

template <typename T>
GlOneFrameVariable<T> :: ~GlOneFrameVariable()
{
}

template <typename T>
void GlOneFrameVariable<T> :: Apply()
{
	if (this->m_bChanged)
	{
		_apply<T>( FrameVariable::m_strName, OneFrameVariable<T>::m_tValue, FrameVariable::m_uiOcc);
		FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************

template <typename T, size_t Count>
GlPointFrameVariable<T, Count> :: GlPointFrameVariable( GlShaderProgram * p_pProgram, GlShaderObject * p_pObject, size_t p_uiOcc)
	:	GlFrameVariableBase( p_pProgram->GetGlProgram())
	,	PointFrameVariable<T, Count>( p_pProgram, p_pObject, p_uiOcc)
{
}

template <typename T, size_t Count>
GlPointFrameVariable<T, Count> :: GlPointFrameVariable( PointFrameVariable<T, Count> * p_pVariable)
	:	GlFrameVariableBase( static_cast<GlShaderProgram *>( p_pVariable->GetProgram())->GetGlProgram())
	,	PointFrameVariable<T, Count>( * p_pVariable)
{
}

template <typename T, size_t Count>
GlPointFrameVariable<T, Count> :: ~GlPointFrameVariable()
{
}

template <typename T, size_t Count>
void GlPointFrameVariable<T, Count> :: Apply()
{
	if (this->m_bChanged)
	{
		_apply<T, Count>( FrameVariable::m_strName, PointFrameVariable<T, Count>::m_pValues, FrameVariable::m_uiOcc);
		FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************

template <typename T, size_t Rows, size_t Columns>
GlMatrixFrameVariable<T, Rows, Columns> :: GlMatrixFrameVariable( GlShaderProgram * p_pProgram, GlShaderObject * p_pObject, size_t p_uiOcc)
	:	GlFrameVariableBase( p_pProgram->GetGlProgram())
	,	MatrixFrameVariable<T, Rows, Columns>( p_pProgram, p_pObject, p_uiOcc)
{
}

template <typename T, size_t Rows, size_t Columns>
GlMatrixFrameVariable<T, Rows, Columns> :: GlMatrixFrameVariable( MatrixFrameVariable<T, Rows, Columns> * p_pVariable)
	:	GlFrameVariableBase( static_cast<GlShaderProgram *>( p_pVariable->GetProgram())->GetGlProgram())
	,	MatrixFrameVariable<T, Rows, Columns>( * p_pVariable)
{
}

template <typename T, size_t Rows, size_t Columns>
GlMatrixFrameVariable<T, Rows, Columns> :: ~GlMatrixFrameVariable()
{
}

template <typename T, size_t Rows, size_t Columns>
void GlMatrixFrameVariable<T, Rows, Columns> :: Apply()
{
	if (this->m_bChanged)
	{
		_apply<T, Rows, Columns>( FrameVariable::m_strName, MatrixFrameVariable<T, Rows, Columns>::m_pValues, FrameVariable::m_uiOcc);
		FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************