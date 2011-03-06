template <size_t Rows, size_t Columns> struct CgMatrixVariableTyper
{
	enum
	{
		Value = FrameVariable::eMat1
	};
};

template <> struct CgMatrixVariableTyper<2, 2>
{
	enum
	{
		Value = FrameVariable::eMat2x2
	};
};

template <> struct CgMatrixVariableTyper<2, 3>
{
	enum
	{
		Value = FrameVariable::eMat2x3
	};
};

template <> struct CgMatrixVariableTyper<2, 4>
{
	enum
	{
		Value = FrameVariable::eMat2x4
	};
};

template <> struct CgMatrixVariableTyper<3, 2>
{
	enum
	{
		Value = FrameVariable::eMat3x2
	};
};

template <> struct CgMatrixVariableTyper<3, 3>
{
	enum
	{
		Value = FrameVariable::eMat3x3
	};
};

template <> struct CgMatrixVariableTyper<3, 4>
{
	enum
	{
		Value = FrameVariable::eMat3x4
	};
};

template <> struct CgMatrixVariableTyper<4, 2>
{
	enum
	{
		Value = FrameVariable::eMat4x2
	};
};

template <> struct CgMatrixVariableTyper<4, 3>
{
	enum
	{
		Value = FrameVariable::eMat4x3
	};
};

template <> struct CgMatrixVariableTyper<4, 4>
{
	enum
	{
		Value = FrameVariable::eMat4x4
	};
};

template <size_t Count> struct CgPointVariableTyper
{
	enum
	{
		Value = FrameVariable::eVec1
	};
};

template <> struct CgPointVariableTyper<2>
{
	enum
	{
		Value = FrameVariable::eVec2
	};
};

template <> struct CgPointVariableTyper<3>
{
	enum
	{
		Value = FrameVariable::eVec3
	};
};

template <> struct CgPointVariableTyper<4>
{
	enum
	{
		Value = FrameVariable::eVec4
	};
};

//*************************************************************************************************

template <typename T>
TCgFrameVariable<T> :: TCgFrameVariable( CGprogram p_cgProgram, size_t p_uiOcc)
	:	CgFrameVariable( p_cgProgram, p_uiOcc)
{
}

template <typename T>
TCgFrameVariable<T> :: TCgFrameVariable( const TCgFrameVariable & p_rVariable)
	:	CgFrameVariable( p_rVariable)
{
}

template <typename T>
TCgFrameVariable<T> :: ~TCgFrameVariable()
{
}

//*************************************************************************************************

template <typename T>
CgOneFrameVariable<T> :: CgOneFrameVariable( CGprogram p_cgProgram, size_t p_uiOcc)
	:	TCgFrameVariable<T>( p_cgProgram, p_uiOcc)
{
	m_tValue = new T[p_uiOcc];
}

template <typename T>
CgOneFrameVariable<T> :: CgOneFrameVariable( const CgOneFrameVariable & p_rVariable)
	:	TCgFrameVariable<T>( p_rVariable)
{
	m_tValue = new T[p_rVariable.m_uiOcc];

	for (size_t i = 0 ; i < p_rVariable.m_uiOcc ; i++)
	{
		policy::assign( m_tValue[i], p_rVariable.m_tValue[i]);
	}
}

template <typename T>
CgOneFrameVariable<T> :: ~CgOneFrameVariable()
{
	delete [] m_tValue;
}

template <typename T>
void CgOneFrameVariable<T> :: SetValue( const String & p_strValue, size_t p_uiIndex)
{
	TCgFrameVariable<T>::SetStrValue( p_strValue, p_uiIndex);
	policy::assign( m_tValue[p_uiIndex], atof( p_strValue.c_str()));
}

//*************************************************************************************************

template <typename T, size_t Count>
CgPointFrameVariable<T, Count> :: CgPointFrameVariable( CGprogram p_cgProgram, size_t p_uiOcc)
	:	TCgFrameVariable<T>( p_cgProgram, p_uiOcc)
{
	m_pValues = new T[p_uiOcc * Count];
	m_ptValue = new Point<T, Count>[p_uiOcc];
}

template <typename T, size_t Count>
CgPointFrameVariable<T, Count> :: CgPointFrameVariable( const CgPointFrameVariable<T, Count> & p_rVariable)
	:	TCgFrameVariable<T>( p_rVariable)
{
	m_pValues = new T[p_rVariable.m_uiOcc * Count];
	m_ptValue = new Point<T, Count>[p_rVariable.m_uiOcc];

	for (size_t i = 0 ; i < p_rVariable.m_uiOcc * Count ; i++)
	{
		policy::assign( m_pValues[i], p_rVariable.m_pValues[i]);
	}

	for (size_t i = 0 ; i < p_rVariable.m_uiOcc ; i++)
	{
		m_ptValue[i].LinkCoords( & m_pValues[i * Count]);
	}
}

template <typename T, size_t Count>
CgPointFrameVariable<T, Count> :: ~CgPointFrameVariable()
{
	delete [] m_ptValue;
}

template <typename T, size_t Count>
FrameVariable::eTYPE CgPointFrameVariable<T, Count> :: GetType()
{
	return FrameVariable::eTYPE( CgPointVariableTyper<Count>::Value);
}

template <typename T, size_t Count>
void CgPointFrameVariable<T, Count> :: SetValue( const String & p_strValue, size_t p_uiIndex)
{
	TCgFrameVariable<T>::SetStrValue( p_strValue, p_uiIndex);
	StringArray l_arraySplitted = p_strValue.split( ", \t");

	if (l_arraySplitted.size() == Count)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			policy::assign( m_ptValue[p_uiIndex][i], atof( l_arraySplitted[i].c_str()));
		}
	}
}

//*************************************************************************************************

template <typename T, size_t Rows, size_t Columns>
CgMatrixFrameVariable<T, Rows, Columns> :: CgMatrixFrameVariable( CGprogram p_cgProgram, size_t p_uiOcc)
	:	TCgFrameVariable<T>( p_cgProgram, p_uiOcc)
{
	m_pValues = new T[Rows * Columns * p_uiOcc];
	m_mValue = new Matrix <T, Rows, Columns>[p_uiOcc];
}

template <typename T, size_t Rows, size_t Columns>
CgMatrixFrameVariable<T, Rows, Columns> :: CgMatrixFrameVariable( const CgMatrixFrameVariable & p_rVariable)
	:	TCgFrameVariable<T>( p_rVariable)
{
	m_pValues = new T[Rows * Columns * p_rVariable.m_uiOcc];
	m_mValue = new Matrix <T, Rows, Columns>[p_rVariable.m_uiOcc];

	for (size_t i = 0 ; i < Rows * Columns * p_rVariable.m_uiOcc ; i++)
	{
		policy::assign( m_pValues[i], p_rVariable.m_pValues[i]);
	}

	for (size_t i = 0 ; i < p_rVariable.m_uiOcc ; i++)
	{
		m_mValue[i].LinkCoords( & m_pValues[i * Rows * Columns]);
	}
}

template <typename T, size_t Rows, size_t Columns>
CgMatrixFrameVariable<T, Rows, Columns> :: ~CgMatrixFrameVariable()
{
	delete [] m_mValue;
}

template <typename T, size_t Rows, size_t Columns>
FrameVariable::eTYPE CgMatrixFrameVariable<T, Rows, Columns> :: GetType()
{
	return FrameVariable::eTYPE( CgMatrixVariableTyper<Rows, Columns>::Value);
}

template <typename T, size_t Rows, size_t Columns>
void CgMatrixFrameVariable<T, Rows, Columns> :: SetValue( const String & p_strValue, size_t p_uiIndex)
{
	TCgFrameVariable<T>::SetStrValue( p_strValue, p_uiIndex);
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
					policy::assign( m_mValue[p_uiIndex][j][i], atof( l_arraySplitted[j].c_str()));
				}
			}
		}
	}
}

//*************************************************************************************************
