//*************************************************************************************************

template <> struct MatrixVariableTyper<2, 2>
{
	enum
	{
		Value = eVARIABLE_TYPE_MAT2X2
	};
};

template <> struct MatrixVariableTyper<2, 3>
{
	enum
	{
		Value = eVARIABLE_TYPE_MAT2X3
	};
};

template <> struct MatrixVariableTyper<2, 4>
{
	enum
	{
		Value = eVARIABLE_TYPE_MAT2X4
	};
};

template <> struct MatrixVariableTyper<3, 2>
{
	enum
	{
		Value = eVARIABLE_TYPE_MAT3X2
	};
};

template <> struct MatrixVariableTyper<3, 3>
{
	enum
	{
		Value = eVARIABLE_TYPE_MAT3X3
	};
};

template <> struct MatrixVariableTyper<3, 4>
{
	enum
	{
		Value = eVARIABLE_TYPE_MAT3X4
	};
};

template <> struct MatrixVariableTyper<4, 2>
{
	enum
	{
		Value = eVARIABLE_TYPE_MAT4X2
	};
};

template <> struct MatrixVariableTyper<4, 3>
{
	enum
	{
		Value = eVARIABLE_TYPE_MAT4X3
	};
};

template <> struct MatrixVariableTyper<4, 4>
{
	enum
	{
		Value = eVARIABLE_TYPE_MAT4X4
	};
};

//*************************************************************************************************

template <> struct PointVariableTyper<2>
{
	enum
	{
		Value = eVARIABLE_TYPE_VEC2
	};
};

template <> struct PointVariableTyper<3>
{
	enum
	{
		Value = eVARIABLE_TYPE_VEC3
	};
};

template <> struct PointVariableTyper<4>
{
	enum
	{
		Value = eVARIABLE_TYPE_VEC4
	};
};

//*************************************************************************************************

template <typename T>
TFrameVariable<T> :: TFrameVariable( ShaderProgramBase * p_pProgram, ShaderObjectBase * p_pObject, size_t p_uiOcc)
	:	FrameVariable( p_pProgram, p_pObject, p_uiOcc)
{
}

template <typename T>
template <typename Ty>
TFrameVariable<T> :: TFrameVariable( const TFrameVariable<Ty> & p_rVariable)
	:	FrameVariable( p_rVariable)
{
	for (size_t i = 0 ; i < m_uiOcc ; i++)
	{
		m_strValue[i] = p_rVariable.m_strValue[i];
	}
}

template <typename T>
TFrameVariable<T> :: ~TFrameVariable()
{
}

//*************************************************************************************************

template <typename T>
OneFrameVariable<T> :: OneFrameVariable( ShaderProgramBase * p_pProgram, ShaderObjectBase * p_pObject, size_t p_uiOcc)
	:	TFrameVariable<T>( p_pProgram, p_pObject, p_uiOcc)
{
	m_tValue = new T[p_uiOcc];
	memset( m_tValue, 0, sizeof( T) * p_uiOcc);
}

template <typename T>
template <typename Ty>
OneFrameVariable<T> :: OneFrameVariable( const OneFrameVariable<Ty> & p_rVariable)
	:	TFrameVariable<T>( p_rVariable)
{
	m_tValue = new T[p_rVariable.m_uiOcc];

	for (size_t i = 0 ; i < p_rVariable.m_uiOcc ; i++)
	{
		policy::assign( m_tValue[i], policy::convert( p_rVariable.m_tValue[i]));
	}
}

template <typename T>
OneFrameVariable<T> :: ~OneFrameVariable()
{
	delete [] m_tValue;
}

template <typename T>
inline void OneFrameVariable<T> :: SetValue( String const & p_strValue, size_t p_uiIndex)
{
	TFrameVariable<T>::SetStrValue( p_strValue, p_uiIndex);
	p_strValue.parse( m_tValue[p_uiIndex]);
}

template <typename T>
inline void OneFrameVariable<T> :: SetValue( T const & p_tValue, size_t p_uiIndex)
{
	policy::assign( m_tValue[p_uiIndex], p_tValue);
	TFrameVariable<T>::SetChanged();
}

//*************************************************************************************************

template <typename T, size_t Count>
PointFrameVariable<T, Count> :: PointFrameVariable( ShaderProgramBase * p_pProgram, ShaderObjectBase * p_pObject, size_t p_uiOcc)
	:	TFrameVariable<T>( p_pProgram, p_pObject, p_uiOcc)
{
	m_pValues = new T[p_uiOcc * Count];
	memset( m_pValues, 0, sizeof( T) * p_uiOcc * Count);
	m_ptValue = new Point<T, Count>[p_uiOcc];

	for (size_t i = 0 ; i < p_uiOcc ; i++)
	{
		m_ptValue[i].link( & m_pValues[i * Count]);
	}
}

template <typename T, size_t Count>
template <typename Ty>
PointFrameVariable<T, Count> :: PointFrameVariable( const PointFrameVariable<Ty, Count> & p_rVariable)
	:	TFrameVariable<T>( p_rVariable)
{
	m_pValues = new T[p_rVariable.m_uiOcc * Count];
	m_ptValue = new Point<T, Count>[p_rVariable.m_uiOcc];

	for (size_t i = 0 ; i < p_rVariable.m_uiOcc * Count ; i++)
	{
		policy::assign( m_pValues[i], policy::convert( p_rVariable.m_pValues[i]));
	}

	for (size_t i = 0 ; i < p_rVariable.m_uiOcc ; i++)
	{
		m_ptValue[i].link( & m_pValues[i * Count]);
	}
}

template <typename T, size_t Count>
PointFrameVariable<T, Count> :: ~PointFrameVariable()
{
	delete [] m_ptValue;
}

template <typename T, size_t Count>
inline eVARIABLE_TYPE PointFrameVariable<T, Count> :: GetType()const
{
	return eVARIABLE_TYPE( PointVariableTyper<Count>::Value);
}

template <typename T, size_t Count>
inline void PointFrameVariable<T, Count> :: SetValue( String const & p_strValue, size_t p_uiIndex)
{
	TFrameVariable<T>::SetStrValue( p_strValue, p_uiIndex);
	StringArray l_arraySplitted = p_strValue.split( ", \t");

	if (l_arraySplitted.size() == Count)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			policy::assign( m_ptValue[p_uiIndex][i], policy::parse( l_arraySplitted[i]));
		}
	}
}

template <typename T, size_t Count>
inline void PointFrameVariable<T, Count> :: SetValue( Point<T, Count> const & p_ptValue, size_t p_uiIndex)
{
	m_ptValue[p_uiIndex].copy( p_ptValue);
	TFrameVariable<T>::SetChanged();
}

//*************************************************************************************************

template <typename T, size_t Rows, size_t Columns>
MatrixFrameVariable<T, Rows, Columns> :: MatrixFrameVariable( ShaderProgramBase * p_pProgram, ShaderObjectBase * p_pObject, size_t p_uiOcc)
	:	TFrameVariable<T>( p_pProgram, p_pObject, p_uiOcc)
{
	m_pValues = new T[Rows * Columns * p_uiOcc];
	memset( m_pValues, 0, sizeof( T) * p_uiOcc * Rows * Columns);
	m_mtxValue = new Matrix <T, Rows, Columns>[p_uiOcc];

	for (size_t i = 0 ; i < p_uiOcc ; i++)
	{
		m_mtxValue[i].LinkCoords( & m_pValues[i * Rows * Columns]);
	}
}

template <typename T, size_t Rows, size_t Columns>
template <typename Ty>
MatrixFrameVariable<T, Rows, Columns> :: MatrixFrameVariable( const MatrixFrameVariable<Ty, Rows, Columns> & p_rVariable)
	:	TFrameVariable<T>( p_rVariable)
{
	m_pValues = new T[Rows * Columns * p_rVariable.m_uiOcc];
	m_mtxValue = new Matrix <T, Rows, Columns>[p_rVariable.m_uiOcc];

	for (size_t i = 0 ; i < Rows * Columns * p_rVariable.m_uiOcc ; i++)
	{
		policy::assign( m_pValues[i], policy::convert( p_rVariable.m_pValues[i]));
	}

	for (size_t i = 0 ; i < p_rVariable.m_uiOcc ; i++)
	{
		m_mtxValue[i].LinkCoords( & m_pValues[i * Rows * Columns]);
	}
}

template <typename T, size_t Rows, size_t Columns>
MatrixFrameVariable<T, Rows, Columns> :: ~MatrixFrameVariable()
{
	delete [] m_mtxValue;
}

template <typename T, size_t Rows, size_t Columns>
inline eVARIABLE_TYPE MatrixFrameVariable<T, Rows, Columns> :: GetType()const
{
	return eVARIABLE_TYPE( MatrixVariableTyper<Rows, Columns>::Value);
}

template <typename T, size_t Rows, size_t Columns>
inline void MatrixFrameVariable<T, Rows, Columns> :: SetValue( String const & p_strValue, size_t p_uiIndex)
{
	TFrameVariable<T>::SetStrValue( p_strValue, p_uiIndex);
	StringArray l_arrayLines = p_strValue.split( ";");

	if (l_arrayLines.size() == Rows)
	{
		bool l_bOK = true;

		for (size_t i = 0 ; i < Rows && l_bOK ; i++)
		{
			l_bOK = false;
			StringArray l_arraySplitted = l_arrayLines[i].split( ", \t");

			if (l_arraySplitted.size() == Columns)
			{
				l_bOK = true;

				for (size_t j = 0 ; j < Columns ; j++)
				{
					policy::assign( m_mtxValue[p_uiIndex][j][i], policy::parse( l_arraySplitted[j]));
				}
			}
		}
	}
}

template <typename T, size_t Rows, size_t Columns>
inline void MatrixFrameVariable<T, Rows, Columns> :: SetValue( Matrix<T, Rows, Columns> const & p_mtxValue, size_t p_uiIndex)
{
	m_mtxValue[p_uiIndex] = p_mtxValue;
	TFrameVariable<T>::SetChanged();
}

//*************************************************************************************************
