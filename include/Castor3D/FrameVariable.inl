//*************************************************************************************************

//!\~english	Helper structure containing definitions for a frame variable type	\~french	Structure d'aide contenant des informations sur un type de frame variable
template< typename T > struct OneFrameVariableDefinitions;
//!\~english	Helper structure containing definitions for a frame variable type	\~french	Structure d'aide contenant des informations sur un type de frame variable
template< typename T, uint32_t Count > struct PntFrameVariableDefinitions;
//!\~english	Helper structure containing definitions for a frame variable type	\~french	Structure d'aide contenant des informations sur un type de frame variable
template< typename T, uint32_t Rows, uint32_t Columns > struct MtxFrameVariableDefinitions;

template<> struct OneFrameVariableDefinitions<	bool						> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 1 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_BOOL;		};
template<> struct OneFrameVariableDefinitions<	int							> { static const uint32_t Size=uint32_t( sizeof(	int				) * 1 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_INT;		};
template<> struct OneFrameVariableDefinitions<	uint32_t					> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 1 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_UINT;		};
template<> struct OneFrameVariableDefinitions<	float						> { static const uint32_t Size=uint32_t( sizeof(	float			) * 1 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_FLOAT;	};
template<> struct OneFrameVariableDefinitions<	double						> { static const uint32_t Size=uint32_t( sizeof(	double			) * 1 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_DOUBLE;	};
template<> struct OneFrameVariableDefinitions<	TextureBase*				> { static const uint32_t Size=uint32_t( sizeof(	TextureBase*	) * 1 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_SAMPLER;	};
template<> struct PntFrameVariableDefinitions<	bool,				2		> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 2 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2B;	};
template<> struct PntFrameVariableDefinitions<	int,				2		> { static const uint32_t Size=uint32_t( sizeof(	int				) * 2 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2I;	};
template<> struct PntFrameVariableDefinitions<	uint32_t,			2		> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 2 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2UI;	};
template<> struct PntFrameVariableDefinitions<	float,				2		> { static const uint32_t Size=uint32_t( sizeof(	float			) * 2 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2F;	};
template<> struct PntFrameVariableDefinitions<	double,				2		> { static const uint32_t Size=uint32_t( sizeof(	double			) * 2 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2D;	};
template<> struct PntFrameVariableDefinitions<	bool,				3		> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 3 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3B;	};
template<> struct PntFrameVariableDefinitions<	int,				3		> { static const uint32_t Size=uint32_t( sizeof(	int				) * 3 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3I;	};
template<> struct PntFrameVariableDefinitions<	uint32_t,			3		> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 3 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3UI;	};
template<> struct PntFrameVariableDefinitions<	float,				3		> { static const uint32_t Size=uint32_t( sizeof(	float			) * 3 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3F;	};
template<> struct PntFrameVariableDefinitions<	double,				3		> { static const uint32_t Size=uint32_t( sizeof(	double			) * 3 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3D;	};
template<> struct PntFrameVariableDefinitions<	bool,				4		> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 4 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4B;	};
template<> struct PntFrameVariableDefinitions<	int,				4		> { static const uint32_t Size=uint32_t( sizeof(	int				) * 4 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4I;	};
template<> struct PntFrameVariableDefinitions<	uint32_t,			4		> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 4 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4UI;	};
template<> struct PntFrameVariableDefinitions<	float,				4		> { static const uint32_t Size=uint32_t( sizeof(	float			) * 4 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4F;	};
template<> struct PntFrameVariableDefinitions<	double,				4		> { static const uint32_t Size=uint32_t( sizeof(	double			) * 4 * 1 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4D;	};
template<> struct MtxFrameVariableDefinitions<	bool,				2,	2	> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 2 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X2B;	};
template<> struct MtxFrameVariableDefinitions<	bool,				2,	3	> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 2 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X3B;	};
template<> struct MtxFrameVariableDefinitions<	bool,				2,	4	> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 2 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X4B;	};
template<> struct MtxFrameVariableDefinitions<	int,				2,	2	> { static const uint32_t Size=uint32_t( sizeof(	int				) * 2 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X2I;	};
template<> struct MtxFrameVariableDefinitions<	int,				2,	3	> { static const uint32_t Size=uint32_t( sizeof(	int				) * 2 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X3I;	};
template<> struct MtxFrameVariableDefinitions<	int,				2,	4	> { static const uint32_t Size=uint32_t( sizeof(	int				) * 2 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X4I;	};
template<> struct MtxFrameVariableDefinitions<	uint32_t,			2,	2	> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 2 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X2UI;	};
template<> struct MtxFrameVariableDefinitions<	uint32_t,			2,	3	> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 2 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X3UI;	};
template<> struct MtxFrameVariableDefinitions<	uint32_t,			2,	4	> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 2 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X4UI;	};
template<> struct MtxFrameVariableDefinitions<	float,				2,	2	> { static const uint32_t Size=uint32_t( sizeof(	float			) * 2 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X2F;	};
template<> struct MtxFrameVariableDefinitions<	float,				2,	3	> { static const uint32_t Size=uint32_t( sizeof(	float			) * 2 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X3F;	};
template<> struct MtxFrameVariableDefinitions<	float,				2,	4	> { static const uint32_t Size=uint32_t( sizeof(	float			) * 2 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X4F;	};
template<> struct MtxFrameVariableDefinitions<	double,				2,	2	> { static const uint32_t Size=uint32_t( sizeof(	double			) * 2 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X2D;	};
template<> struct MtxFrameVariableDefinitions<	double,				2,	3	> { static const uint32_t Size=uint32_t( sizeof(	double			) * 2 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X3D;	};
template<> struct MtxFrameVariableDefinitions<	double,				2,	4	> { static const uint32_t Size=uint32_t( sizeof(	double			) * 2 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT2X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT2X4D;	};
template<> struct MtxFrameVariableDefinitions<	bool,				3,	2	> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 3 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X2B;	};
template<> struct MtxFrameVariableDefinitions<	bool,				3,	3	> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 3 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X3B;	};
template<> struct MtxFrameVariableDefinitions<	bool,				3,	4	> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 3 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X4B;	};
template<> struct MtxFrameVariableDefinitions<	int,				3,	2	> { static const uint32_t Size=uint32_t( sizeof(	int				) * 3 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X2I;	};
template<> struct MtxFrameVariableDefinitions<	int,				3,	3	> { static const uint32_t Size=uint32_t( sizeof(	int				) * 3 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X3I;	};
template<> struct MtxFrameVariableDefinitions<	int,				3,	4	> { static const uint32_t Size=uint32_t( sizeof(	int				) * 3 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X4I;	};
template<> struct MtxFrameVariableDefinitions<	uint32_t,			3,	2	> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 3 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X2UI;	};
template<> struct MtxFrameVariableDefinitions<	uint32_t,			3,	3	> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 3 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X3UI;	};
template<> struct MtxFrameVariableDefinitions<	uint32_t,			3,	4	> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 3 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X4UI;	};
template<> struct MtxFrameVariableDefinitions<	float,				3,	2	> { static const uint32_t Size=uint32_t( sizeof(	float			) * 3 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X2F;	};
template<> struct MtxFrameVariableDefinitions<	float,				3,	3	> { static const uint32_t Size=uint32_t( sizeof(	float			) * 3 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X3F;	};
template<> struct MtxFrameVariableDefinitions<	float,				3,	4	> { static const uint32_t Size=uint32_t( sizeof(	float			) * 3 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X4F;	};
template<> struct MtxFrameVariableDefinitions<	double,				3,	2	> { static const uint32_t Size=uint32_t( sizeof(	double			) * 3 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X2D;	};
template<> struct MtxFrameVariableDefinitions<	double,				3,	3	> { static const uint32_t Size=uint32_t( sizeof(	double			) * 3 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X3D;	};
template<> struct MtxFrameVariableDefinitions<	double,				3,	4	> { static const uint32_t Size=uint32_t( sizeof(	double			) * 3 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT3X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT3X4D;	};
template<> struct MtxFrameVariableDefinitions<	bool,				4,	2	> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 4 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X2B;	};
template<> struct MtxFrameVariableDefinitions<	bool,				4,	3	> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 4 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X3B;	};
template<> struct MtxFrameVariableDefinitions<	bool,				4,	4	> { static const uint32_t Size=uint32_t( sizeof(	bool			) * 4 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X4B;	};
template<> struct MtxFrameVariableDefinitions<	int,				4,	2	> { static const uint32_t Size=uint32_t( sizeof(	int				) * 4 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X2I;	};
template<> struct MtxFrameVariableDefinitions<	int,				4,	3	> { static const uint32_t Size=uint32_t( sizeof(	int				) * 4 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X3I;	};
template<> struct MtxFrameVariableDefinitions<	int,				4,	4	> { static const uint32_t Size=uint32_t( sizeof(	int				) * 4 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X4I;	};
template<> struct MtxFrameVariableDefinitions<	uint32_t,			4,	2	> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 4 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X2UI;	};
template<> struct MtxFrameVariableDefinitions<	uint32_t,			4,	3	> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 4 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X3UI;	};
template<> struct MtxFrameVariableDefinitions<	uint32_t,			4,	4	> { static const uint32_t Size=uint32_t( sizeof(	uint32_t		) * 4 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X4UI;	};
template<> struct MtxFrameVariableDefinitions<	float,				4,	2	> { static const uint32_t Size=uint32_t( sizeof(	float			) * 4 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X2F;	};
template<> struct MtxFrameVariableDefinitions<	float,				4,	3	> { static const uint32_t Size=uint32_t( sizeof(	float			) * 4 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X3F;	};
template<> struct MtxFrameVariableDefinitions<	float,				4,	4	> { static const uint32_t Size=uint32_t( sizeof(	float			) * 4 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X4F;	};
template<> struct MtxFrameVariableDefinitions<	double,				4,	2	> { static const uint32_t Size=uint32_t( sizeof(	double			) * 4 * 2 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X2;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X2D;	};
template<> struct MtxFrameVariableDefinitions<	double,				4,	3	> { static const uint32_t Size=uint32_t( sizeof(	double			) * 4 * 3 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X3;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X3D;	};
template<> struct MtxFrameVariableDefinitions<	double,				4,	4	> { static const uint32_t Size=uint32_t( sizeof(	double			) * 4 * 4 );	static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_MAT4X4;	static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_MAT4X4D;	};

//*************************************************************************************************

template< typename T >
bool FrameVariableBuffer :: GetVariable( Castor::String const & p_strName, std::shared_ptr< OneFrameVariable< T > > & p_pVariable )const
{
	bool l_bReturn = false;
	FrameVariablePtrStrMapConstIt l_it = m_mapVariables.find( p_strName );

	if( l_it != m_mapVariables.end() )
	{
		FrameVariableSPtr l_pVariable = l_it->second.lock();

		if( l_pVariable->GetFullType() == OneFrameVariableDefinitions< T >::Full )
		{
			p_pVariable = std::static_pointer_cast< OneFrameVariable< T > >( l_pVariable );
			l_bReturn = true;
		}
	}

	return l_bReturn;
}

template< typename T, uint32_t Count >
bool FrameVariableBuffer :: GetVariable( Castor::String const & p_strName, std::shared_ptr< PointFrameVariable< T, Count > > & p_pVariable )const
{
	bool l_bReturn = false;
	FrameVariablePtrStrMapConstIt l_it = m_mapVariables.find( p_strName );

	if( l_it != m_mapVariables.end() )
	{
		FrameVariableSPtr l_pVariable = l_it->second.lock();

		if( l_pVariable->GetFullType() == PntFrameVariableDefinitions< T, Count >::Full )
		{
			p_pVariable = std::static_pointer_cast< PointFrameVariable< T, Count > >( l_pVariable );
			l_bReturn = true;
		}
	}

	return l_bReturn;
}

template< typename T, uint32_t Rows, uint32_t Columns >
bool FrameVariableBuffer :: GetVariable( Castor::String const & p_strName, std::shared_ptr< MatrixFrameVariable< T, Rows, Columns > > & p_pVariable )const
{
	bool l_bReturn = false;
	FrameVariablePtrStrMapConstIt l_it = m_mapVariables.find( p_strName );

	if( l_it != m_mapVariables.end() )
	{
		FrameVariableSPtr l_pVariable = l_it->second.lock();

		if( l_pVariable->GetFullType() == MtxFrameVariableDefinitions< T, Rows, Columns >::Full )
		{
			p_pVariable = std::static_pointer_cast< MatrixFrameVariable< T, Rows, Columns > >( l_pVariable );
			l_bReturn = true;
		}
	}

	return l_bReturn;
}

//*************************************************************************************************

template< typename T >
TFrameVariable< T > :: TFrameVariable( ShaderProgramBase * p_pProgram )
	:	FrameVariable	( p_pProgram	)
	,	m_pValues		( NULL			)
	,	m_bOwnBuffer	( true			)
{
	// m_pValues initialised in child classes
}

template< typename T >
TFrameVariable< T > :: TFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc )
	:	FrameVariable	( p_pProgram, p_uiOcc	)
	,	m_pValues		( NULL					)
	,	m_bOwnBuffer	( true					)
{
	// m_pValues initialised in child classes
}

template< typename T >
TFrameVariable< T > :: TFrameVariable( TFrameVariable< T > const & p_rVariable )
	:	FrameVariable	( p_rVariable				)
	,	m_pValues		( NULL						)
	,	m_bOwnBuffer	( p_rVariable.m_bOwnBuffer	)
{
	if( p_rVariable.m_bOwnBuffer )
	{
		uint8_t * l_pBuffer = new uint8_t[p_rVariable.size()];
		m_pValues = reinterpret_cast< T* >( l_pBuffer );
		memcpy( m_pValues, p_rVariable.m_pValues, p_rVariable.size() );
	}
	else
	{
		m_pValues = p_rVariable.m_pValues;
	}

	for( uint32_t i = 0; i < m_uiOcc; i++ )
	{
		m_strValue[i] = p_rVariable.m_strValue[i];
	}
}

template< typename T >
TFrameVariable< T > :: TFrameVariable( TFrameVariable< T > && p_rVariable)
	:	FrameVariable	( std::move( p_rVariable )				)
	,	m_pValues		( std::move( p_rVariable.m_pValues )	)
	,	m_bOwnBuffer	( std::move( p_rVariable.m_bOwnBuffer )	)
{
	for( uint32_t i = 0; i < m_uiOcc; i++ )
	{
		m_strValue[i] = std::move( p_rVariable.m_strValue[i] );
		p_rVariable.m_strValue[i].clear();
	}

	p_rVariable.m_pValues = NULL;
}

template< typename T >
TFrameVariable< T > & TFrameVariable< T > :: operator =( TFrameVariable< T > const & p_rVariable )
{
	TFrameVariable< T > l_tmp( p_rVariable );
	std::swap( *this, l_tmp );
	return *this;
}

template< typename T >
TFrameVariable< T > & TFrameVariable< T > :: operator =( TFrameVariable< T > && p_rVariable )
{
	FrameVariable::operator =( std::move( p_rVariable ) );

	if( this != &p_rVariable )
	{
		DoCleanupBuffer();
		m_bOwnBuffer	= std::move( p_rVariable.m_bOwnBuffer );
		m_pValues		= std::move( p_rVariable.m_pValues );

		for( uint32_t i = 0; i < m_uiOcc; i++ )
		{
			m_strValue[i] = std::move( p_rVariable.m_strValue[i] );
			p_rVariable.m_strValue[i].clear();
		}
	}

	return *this;
}

template< typename T >
TFrameVariable< T > :: ~TFrameVariable()
{
	DoCleanupBuffer();
}

template< typename T >
void TFrameVariable< T > :: link( uint8_t * p_pBuffer )
{
	memcpy( p_pBuffer, m_pValues, size() );
	DoCleanupBuffer();
	m_pValues = reinterpret_cast< T* >( p_pBuffer );
	m_bOwnBuffer = false;
}

template< typename T >
inline void TFrameVariable< T > :: DoCleanupBuffer()
{
	if( m_bOwnBuffer )
	{
		delete_array m_pValues;
	}
}

//*************************************************************************************************

template< typename T >
OneFrameVariable< T > :: OneFrameVariable( ShaderProgramBase * p_pProgram )
	:	TFrameVariable< T >	( p_pProgram	)
{
	this->m_pValues = new T[1];
	memset( this->m_pValues, 0, size() );
}

template< typename T >
OneFrameVariable< T > :: OneFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc )
	:	TFrameVariable< T >	( p_pProgram, p_uiOcc	)
{
	this->m_pValues = new T[p_uiOcc];
	memset( this->m_pValues, 0, size() );
}

template< typename T >
OneFrameVariable< T > :: OneFrameVariable( OneFrameVariable< T > const & p_rVariable)
	:	TFrameVariable< T >	( p_rVariable	)
{
}

template< typename T >
OneFrameVariable< T > :: OneFrameVariable( OneFrameVariable< T > && p_rVariable)
	:	TFrameVariable< T > ( std::move( p_rVariable	) )
{
}

template< typename T >
OneFrameVariable< T > & OneFrameVariable< T > :: operator =( OneFrameVariable< T > const & p_rVariable )
{
	OneFrameVariable< T > l_tmp( p_rVariable );
	std::swap( *this, l_tmp );
	return *this;
}

template< typename T >
OneFrameVariable< T > & OneFrameVariable< T > :: operator =( OneFrameVariable< T > && p_rVariable )
{
	TFrameVariable< T >::operator =( std::move( p_rVariable ) );

	if( this = &p_rVariable )
	{
	}

	return *this;
}

template< typename T >
OneFrameVariable< T > :: ~OneFrameVariable()
{
}

template< typename T >
inline void OneFrameVariable< T > :: SetValue( T const & p_tValue )
{
	OneFrameVariable< T >::SetValue( p_tValue, 0 );
}

template< typename T >
inline void OneFrameVariable< T > :: SetValue( T const & p_tValue, uint32_t p_uiIndex)
{
	policy::assign( this->m_pValues[p_uiIndex], p_tValue);
	TFrameVariable< T >::m_bChanged = true;
}

template< typename T >
inline T & OneFrameVariable< T > :: GetValue()throw( std::out_of_range )
{
	return OneFrameVariable< T >::GetValue( 0 );
}

template< typename T >
inline T const & OneFrameVariable< T > :: GetValue()const throw( std::out_of_range )
{
	return OneFrameVariable< T >::GetValue( 0 );
}

template< typename T >
inline T & OneFrameVariable< T > :: GetValue( uint32_t p_uiIndex )throw( std::out_of_range )
{
	if( p_uiIndex < TFrameVariable< T >::m_uiOcc )
	{
		return this->m_pValues[p_uiIndex];
	}
	else
	{
		throw( std::out_of_range )( "OneFrameVariable< T > array subscript out of range" );
	}
}

template< typename T >
inline T const & OneFrameVariable< T > :: GetValue( uint32_t p_uiIndex )const throw( std::out_of_range )
{
	if( p_uiIndex < TFrameVariable< T >::m_uiOcc )
	{
		return this->m_pValues[p_uiIndex];
	}
	else
	{
		throw( std::out_of_range )( "OneFrameVariable< T > array subscript out of range" );
	}
}

template< typename T >
inline eFRAME_VARIABLE_TYPE OneFrameVariable< T > :: GetFrameVariableType()
{
	return eFRAME_VARIABLE_TYPE( OneFrameVariableDefinitions< T >::Full );
}

template< typename T >
uint32_t OneFrameVariable< T > :: size()const
{
	return OneFrameVariableDefinitions< T >::Size * this->m_uiOcc;
}

template< typename T >
inline void OneFrameVariable< T > :: DoSetValueStr( Castor::String const & p_strValue, uint32_t p_uiIndex )
{
	Castor::str_utils::parse( p_strValue, this->m_pValues[p_uiIndex] );
}

//*************************************************************************************************

template< typename T, uint32_t Count >
PointFrameVariable< T, Count > :: PointFrameVariable( ShaderProgramBase * p_pProgram )
	:	TFrameVariable< T >	( p_pProgram	)
{
	this->m_pValues = new T[Count];
	memset( this->m_pValues, 0, size() );
}

template< typename T, uint32_t Count >
PointFrameVariable< T, Count > :: PointFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc )
	:	TFrameVariable< T >	( p_pProgram, p_uiOcc	)
{
	this->m_pValues = new T[p_uiOcc * Count];
	memset( this->m_pValues, 0, size() );
}

template< typename T, uint32_t Count >
PointFrameVariable< T, Count > :: PointFrameVariable( PointFrameVariable< T, Count> const & p_rVariable )
	:	TFrameVariable< T >	( p_rVariable	)
{
}

template< typename T, uint32_t Count >
PointFrameVariable< T, Count > :: PointFrameVariable( PointFrameVariable< T, Count > && p_rVariable )
	:	TFrameVariable< T >	( std::move( p_rVariable			) )
{
}

template< typename T, uint32_t Count >
PointFrameVariable< T, Count > & PointFrameVariable< T, Count > :: operator =( PointFrameVariable< T, Count> const & p_rVariable )
{
	PointFrameVariable< T, Count > l_tmp( p_rVariable );
	std::swap( *this, l_tmp );
	return *this;
}

template< typename T, uint32_t Count >
PointFrameVariable< T, Count > & PointFrameVariable< T, Count > :: operator =( PointFrameVariable< T, Count > && p_rVariable )
{
	TFrameVariable< T >::operator =( std::move( p_rVariable ) );

	if( this != &p_rVariable )
	{
	}

	return *this;
}

template< typename T, uint32_t Count >
PointFrameVariable< T, Count > :: ~PointFrameVariable()
{
}

template< typename T, uint32_t Count >
inline void PointFrameVariable< T, Count > :: SetValue( Castor::Point< T, Count > const & p_ptValue )
{
	PointFrameVariable< T, Count >::SetValue( p_ptValue, 0 );
}

template< typename T, uint32_t Count >
inline void PointFrameVariable< T, Count > :: SetValue( Castor::Point< T, Count > const & p_ptValue, uint32_t p_uiIndex )
{
	Castor::Coords< T, Count > l_ptValue( GetValue( p_uiIndex ) );
	memcpy( l_ptValue.ptr(), p_ptValue.const_ptr(), l_ptValue.size() );
	TFrameVariable< T >::m_bChanged = true;
}

template< typename T, uint32_t Count >
inline Castor::Coords< T, Count > PointFrameVariable< T, Count > :: GetValue()throw( std::out_of_range )
{
	return PointFrameVariable< T, Count >::GetValue( 0 );
}

template< typename T, uint32_t Count >
inline Castor::Point< T, Count > PointFrameVariable< T, Count > :: GetValue()const throw( std::out_of_range )
{
	return PointFrameVariable< T, Count >::GetValue( 0 );
}

template< typename T, uint32_t Count >
inline Castor::Coords< T, Count > PointFrameVariable< T, Count > :: GetValue( uint32_t p_uiIndex )throw( std::out_of_range )
{
	if( p_uiIndex < this->m_uiOcc )
	{
		return Castor::Coords< T, Count >( &this->m_pValues[p_uiIndex * Count] );
	}
	else
	{
		throw( std::out_of_range )( "PointFrameVariable< T, Count > array subscript out of range" );
	}
}

template< typename T, uint32_t Count >
inline Castor::Point< T, Count > PointFrameVariable< T, Count > :: GetValue( uint32_t p_uiIndex )const throw( std::out_of_range )
{
	if( p_uiIndex < this->m_uiOcc )
	{
		return Castor::Point< T, Count >( &this->m_pValues[p_uiIndex * Count] );
	}
	else
	{
		throw( std::out_of_range )( "PointFrameVariable< T, Count > array subscript out of range" );
	}
}

template< typename T, uint32_t Count >
inline eVARIABLE_TYPE PointFrameVariable< T, Count > :: GetVariableType()
{
	return eVARIABLE_TYPE( PntFrameVariableDefinitions< T, Count >::Count );
}

template< typename T, uint32_t Count >
inline eFRAME_VARIABLE_TYPE PointFrameVariable< T, Count > :: GetFrameVariableType()
{
	return eFRAME_VARIABLE_TYPE( PntFrameVariableDefinitions< T, Count >::Full );
}

template< typename T, uint32_t Count >
uint32_t PointFrameVariable< T, Count > :: size()const
{
	return PntFrameVariableDefinitions< T, Count >::Size * this->m_uiOcc;
}

template< typename T, uint32_t Count >
inline void PointFrameVariable< T, Count > :: DoSetValueStr( Castor::String const & p_strValue, uint32_t p_uiIndex)
{
	Castor::StringArray l_arraySplitted = Castor::str_utils::split( p_strValue, cuT( ", \t" ) );
	Castor::Coords< T, Count > l_ptValue( GetValue( p_uiIndex ) );

	if (l_arraySplitted.size() == Count)
	{
		for( uint32_t i = 0; i < Count; i++ )
		{
			policy::assign( l_ptValue[i], policy::parse( l_arraySplitted[i]));
		}
	}
}

//*************************************************************************************************

template< typename T, uint32_t Rows, uint32_t Columns >
MatrixFrameVariable< T, Rows, Columns > :: MatrixFrameVariable( ShaderProgramBase * p_pProgram )
	:	TFrameVariable< T >	( p_pProgram	)
{
	this->m_pValues = new T[Rows * Columns];
	memset( this->m_pValues, 0, sizeof( T ) * Rows * Columns);
	m_mtxValue = new Castor::Matrix< T, Rows, Columns >[1];
	m_mtxValue[0].link( &this->m_pValues[0] );
}

template< typename T, uint32_t Rows, uint32_t Columns >
MatrixFrameVariable< T, Rows, Columns > :: MatrixFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc )
	:	TFrameVariable< T >	( p_pProgram, p_uiOcc	)
{
	this->m_pValues = new T[Rows * Columns * p_uiOcc];
	memset( this->m_pValues, 0, sizeof( T) * p_uiOcc * Rows * Columns);
	m_mtxValue = new Castor::Matrix <T, Rows, Columns>[p_uiOcc];

	for( uint32_t i = 0; i < p_uiOcc; i++ )
	{
		m_mtxValue[i].link( &this->m_pValues[i * Rows * Columns]);
	}
}

template< typename T, uint32_t Rows, uint32_t Columns >
MatrixFrameVariable< T, Rows, Columns > :: MatrixFrameVariable( MatrixFrameVariable< T, Rows, Columns > const & p_rVariable )
	:	TFrameVariable< T >	( p_rVariable	)
{
	m_mtxValue = new Castor::Matrix <T, Rows, Columns>[p_rVariable.m_uiOcc];

	for( uint32_t i = 0; i < p_rVariable.m_uiOcc; i++ )
	{
		m_mtxValue[i].link( &this->m_pValues[i * Rows * Columns] );
	}
}

template< typename T, uint32_t Rows, uint32_t Columns >
MatrixFrameVariable< T, Rows, Columns > :: MatrixFrameVariable( MatrixFrameVariable< T, Rows, Columns > && p_rVariable )
	:	TFrameVariable< T >	( std::move( p_rVariable			) )
	,	m_mtxValue			( std::move( p_rVariable.m_mtxValue	) )
{
	p_rVariable.m_mtxValue	= NULL;
}

template< typename T, uint32_t Rows, uint32_t Columns >
MatrixFrameVariable< T, Rows, Columns > & MatrixFrameVariable< T, Rows, Columns > :: operator =( MatrixFrameVariable< T, Rows, Columns > const & p_rVariable )
{
	MatrixFrameVariable< T, Rows, Columns > l_tmp( p_rVariable );
	std::swap( *this, l_tmp );
	return *this;
}

template< typename T, uint32_t Rows, uint32_t Columns >
MatrixFrameVariable< T, Rows, Columns > & MatrixFrameVariable< T, Rows, Columns > :: operator =( MatrixFrameVariable< T, Rows, Columns > && p_rVariable )
{
	TFrameVariable< T >::operator =( std::move( p_rVariable ) );

	if( this != &p_rVariable )
	{
		delete_array m_mtxValue;
		m_mtxValue	= std::move( p_rVariable.m_mtxValue	);
		p_rVariable.m_pValues	= NULL;
		p_rVariable.m_mtxValue	= NULL;
	}

	return *this;
}

template< typename T, uint32_t Rows, uint32_t Columns >
MatrixFrameVariable< T, Rows, Columns > :: ~MatrixFrameVariable()
{
	delete_array m_mtxValue;
}

template< typename T, uint32_t Rows, uint32_t Columns >
inline void MatrixFrameVariable< T, Rows, Columns > :: SetValue( Castor::Matrix< T, Rows, Columns > const & p_mtxValue )
{
	MatrixFrameVariable< T, Rows, Columns >::SetValue( p_mtxValue, 0 );
}

template< typename T, uint32_t Rows, uint32_t Columns >
inline void MatrixFrameVariable< T, Rows, Columns > :: SetValue( Castor::Matrix< T, Rows, Columns > const & p_mtxValue, uint32_t p_uiIndex )
{
	m_mtxValue[p_uiIndex] = p_mtxValue;
	TFrameVariable< T >::m_bChanged = true;
}

template< typename T, uint32_t Rows, uint32_t Columns >
inline Castor::Matrix< T, Rows, Columns > & MatrixFrameVariable< T, Rows, Columns > :: GetValue()throw( std::out_of_range )
{
	return MatrixFrameVariable< T, Rows, Columns >::GetValue( 0 );
}

template< typename T, uint32_t Rows, uint32_t Columns >
inline Castor::Matrix< T, Rows, Columns > const & MatrixFrameVariable< T, Rows, Columns > :: GetValue()const throw( std::out_of_range )
{
	return MatrixFrameVariable< T, Rows, Columns >::GetValue( 0 );
}

template< typename T, uint32_t Rows, uint32_t Columns >
inline Castor::Matrix< T, Rows, Columns > & MatrixFrameVariable< T, Rows, Columns > :: GetValue( uint32_t p_uiIndex )throw( std::out_of_range )
{
	if( p_uiIndex < TFrameVariable< T >::m_uiOcc )
	{
		return m_mtxValue[p_uiIndex];
	}
	else
	{
		throw( std::out_of_range )( "MatrixFrameVariable< T, Rows, Columns > array subscript out of range" );
	}
}

template< typename T, uint32_t Rows, uint32_t Columns >
inline Castor::Matrix< T, Rows, Columns > const & MatrixFrameVariable< T, Rows, Columns > :: GetValue( uint32_t p_uiIndex )const throw( std::out_of_range )
{
	if( p_uiIndex < TFrameVariable< T >::m_uiOcc )
	{
		return m_mtxValue[p_uiIndex];
	}
	else
	{
		throw( std::out_of_range )( "MatrixFrameVariable< T, Rows, Columns > array subscript out of range" );
	}
}

template< typename T, uint32_t Rows, uint32_t Columns >
inline eVARIABLE_TYPE MatrixFrameVariable< T, Rows, Columns > :: GetVariableType()
{
	return eVARIABLE_TYPE( MtxFrameVariableDefinitions< T, Rows, Columns >::Count );
}

template< typename T, uint32_t Rows, uint32_t Columns >
inline eFRAME_VARIABLE_TYPE MatrixFrameVariable< T, Rows, Columns > :: GetFrameVariableType()
{
	return eFRAME_VARIABLE_TYPE( MtxFrameVariableDefinitions< T, Rows, Columns >::Full );
}

template< typename T, uint32_t Rows, uint32_t Columns >
uint32_t MatrixFrameVariable< T, Rows, Columns > :: size()const
{
	return MtxFrameVariableDefinitions< T, Rows, Columns >::Size * this->m_uiOcc;
}

template< typename T, uint32_t Rows, uint32_t Columns >
void MatrixFrameVariable< T, Rows, Columns > :: link( uint8_t * p_pBuffer )
{
	memcpy( p_pBuffer, this->m_pValues, size() );
	T * l_pBuffer = reinterpret_cast< T* >( p_pBuffer );

	for( uint32_t i = 0; i < this->m_uiOcc; i++ )
	{
		m_mtxValue[i].link( &l_pBuffer[i * Rows * Columns]);
	}

	TFrameVariable< T >::link( p_pBuffer );
}

template< typename T, uint32_t Rows, uint32_t Columns >
inline void MatrixFrameVariable< T, Rows, Columns > :: DoSetValueStr( Castor::String const & p_strValue, uint32_t p_uiIndex )
{
	Castor::StringArray l_arrayLines = Castor::str_utils::split( p_strValue, cuT( ";" ) );

	if (l_arrayLines.size() == Rows)
	{
		bool l_bOK = true;

		for (uint32_t i = 0; i < Rows && l_bOK; i++)
		{
			l_bOK = false;
			Castor::StringArray l_arraySplitted = Castor::str_utils::split( l_arrayLines[i], cuT( ", \t" ) );

			if (l_arraySplitted.size() == Columns)
			{
				l_bOK = true;

				for (uint32_t j = 0; j < Columns; j++)
				{
					policy::assign( m_mtxValue[p_uiIndex][j][i], policy::parse( l_arraySplitted[j]));
				}
			}
		}
	}
}

//*************************************************************************************************
