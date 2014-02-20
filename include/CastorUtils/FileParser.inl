//*************************************************************************************************

namespace
{
	/**
	 *\~english
	 *\brief		Parses a vector from a line
	 *\param[in]	p_strParams	The line containing the vector
	 *\param[out]	p_vResult	Receives the result
	 *\param[in]	p_pContext	The current parsing context
	 *\return		\p true if OK
	 *\~french
	 *\brief		Extrait un vecteur à partir d'une ligne
	 *\param[in]	p_strParams	La ligne contenant le vecteur
	 *\param[out]	p_vResult	Reçoit le résultat
	 *\param[in]	p_pContext	Le context d'analyse
	 *\return		\p true si tout s'est bien passé
	 */
	inline bool ParseSize( String & p_strParams, Size & p_vResult )
	{
		bool l_bReturn = false;

		if( p_strParams == cuT( "screen_size" ) )
		{
			l_bReturn = Castor::GetScreenSize( p_vResult );
		}
		else
		{
			StringArray l_arrayValues = str_utils::split( p_strParams, cuT( " \t,;" ) );

			if( l_arrayValues.size() >= 2 )
			{
				p_strParams.clear();

				std::for_each( l_arrayValues.begin() + 2, l_arrayValues.end(), [&]( String const & p_strParam )
				{
					p_strParams += p_strParam + cuT( " " );
				} );

				str_utils::trim( p_strParams );

				{
					std::basic_istringstream< xchar > l_stream( l_arrayValues[0] );
					l_stream >> p_vResult.ptr()[0];
				}

				{
					std::basic_istringstream< xchar > l_stream( l_arrayValues[1] );
					l_stream >> p_vResult.ptr()[1];
				}

				l_bReturn = true;
			}
		}

		return l_bReturn;
	}
	/**
	 *\~english
	 *\brief		Parses a vector from a line
	 *\param[in]	p_strParams	The line containing the vector
	 *\param[out]	p_vResult	Receives the result
	 *\param[in]	p_pContext	The current parsing context
	 *\return		\p true if OK
	 *\~french
	 *\brief		Extrait un vecteur à partir d'une ligne
	 *\param[in]	p_strParams	La ligne contenant le vecteur
	 *\param[out]	p_vResult	Reçoit le résultat
	 *\param[in]	p_pContext	Le context d'analyse
	 *\return		\p true si tout s'est bien passé
	 */
	template< typename T, uint32_t Count >
	inline bool ParseVector( String & p_strParams, Point< T, Count > & p_vResult )
	{
		bool l_bReturn = false;
		StringArray l_arrayValues = str_utils::split( p_strParams, cuT( " \t,;" ) );

		if( l_arrayValues.size() >= Count )
		{
			p_strParams.clear();

			std::for_each( l_arrayValues.begin() + Count, l_arrayValues.end(), [&]( String const & p_strParam )
			{
				p_strParams += p_strParam + cuT( " " );
			} );

			str_utils::trim( p_strParams );

			for( uint32_t i = 0; i < Count; i++ )
			{
				std::basic_istringstream< xchar > l_stream( l_arrayValues[i] );
				l_stream >> p_vResult[i];
			}

			l_bReturn = true;
		}

		return l_bReturn;
	}
	/**
	 *\~english
	 *\brief		Parses a colour from a line
	 *\param[in]	p_strParams	The line containing the vector
	 *\param[out]	p_colour	Receives the result
	 *\param[in]	p_pContext	The current parsing context
	 *\return		\p true if OK
	 *\~french
	 *\brief		Extrait une couleur à partir d'une ligne
	 *\param[in]	p_strParams	La ligne contenant le vecteur
	 *\param[out]	p_colour	Reçoit le résultat
	 *\param[in]	p_pContext	Le context d'analyse
	 *\return		\p true si tout s'est bien passé
	 */
	inline bool ParseColour( String & p_strParams, Colour & p_colour )
	{
		bool l_bReturn = false;
		StringArray l_arrayValues = str_utils::split( p_strParams, cuT( " \t,;" ) );

		if( l_arrayValues.size() >= Colour::eCOMPONENT_COUNT )
		{
			p_strParams.clear();

			std::for_each( l_arrayValues.begin() + Colour::eCOMPONENT_COUNT, l_arrayValues.end(), [&]( String const & p_strParam )
			{
				p_strParams += p_strParam + cuT( " " );
			} );

			str_utils::trim( p_strParams );

			for( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT; i++ )
			{
				std::basic_istringstream< xchar > l_stream( l_arrayValues[i] );
				double l_dComponent;
				l_stream >> l_dComponent;
				p_colour[Colour::eCOMPONENT( i )] = l_dComponent;
			}

			l_bReturn = true;
		}
		else if( l_arrayValues.size() == 3 )
		{
			p_strParams.clear();

			for( uint8_t i = 0; i < 3; i++ )
			{
				std::basic_istringstream< xchar > l_stream( l_arrayValues[i] );
				double l_dComponent;
				l_stream >> l_dComponent;
				p_colour[Colour::eCOMPONENT( i )] = l_dComponent;
			}

			p_colour[Colour::eCOMPONENT_ALPHA] = 1.0;
			l_bReturn = true;
		}

		return l_bReturn;
	}
	/**
	 *\~english
	 *\brief		Parses a signed integer from a line
	 *\param[in]	p_strParams	The line containing the value
	 *\param[out]	p_tResult	Receives the result
	 *\~french
	 *\brief		Extrait un entier signé à partir d'une ligne
	 *\param[in]	p_strParams	La ligne contenant la valeur
	 *\param[out]	p_tResult	Reçoit le résultat
	 */
	template< typename T >
	inline bool ParseInteger( String & p_strParams, T & p_tResult, typename std::enable_if< !std::is_unsigned< T >::value >::type* = 0 )
	{
		bool l_bReturn = false;
		StringArray l_array = str_utils::split( p_strParams, cuT( " \t,;" ), 1, false );

		if( l_array.size() )
		{
			l_bReturn = str_utils::is_integer( l_array[0] );
			int64_t l_i64Tmp = 0;

			if( l_bReturn )
			{
				l_i64Tmp = str_utils::to_long_long( l_array[0] );
				l_bReturn = (l_i64Tmp > std::numeric_limits< T >::lowest()) && (l_i64Tmp < std::numeric_limits< T >::max());
			}

			if( l_bReturn )
			{
				p_tResult = static_cast< T >( l_i64Tmp );
			}

			p_strParams.clear();

			if( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return l_bReturn;
	}
	/**
	 *\~english
	 *\brief		Parses an unsigned integer from a line
	 *\param[in]	p_strParams	The line containing the value
	 *\param[out]	p_tResult	Receives the result
	 *\~french
	 *\brief		Extrait un entier non signé à partir d'une ligne
	 *\param[in]	p_strParams	La ligne contenant la valeur
	 *\param[out]	p_tResult	Reçoit le résultat
	 */
	template< typename T >
	inline bool ParseInteger( String & p_strParams, T & p_tResult, typename std::enable_if< std::is_unsigned< T >::value >::type* = 0 )
	{
		bool l_bReturn = false;
		StringArray l_array = str_utils::split( p_strParams, cuT( " \t,;" ), 1, false );

		if( l_array.size() )
		{
			l_bReturn = str_utils::is_integer( l_array[0] );
			uint64_t l_ui64Tmp = 0;

			if( l_bReturn )
			{
				l_ui64Tmp = str_utils::to_long_long( l_array[0] );
				l_bReturn = (l_ui64Tmp > std::numeric_limits< T >::lowest()) && (l_ui64Tmp < std::numeric_limits< T >::max());
			}

			if( l_bReturn )
			{
				p_tResult = static_cast< T >( l_ui64Tmp );
			}

			p_strParams.clear();

			if( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return l_bReturn;
	}
	/**
	 *\~english
	 *\brief		Parses an floating number from a line
	 *\param[in]	p_strParams	The line containing the value
	 *\param[out]	p_tResult	Receives the result
	 *\~french
	 *\brief		Extrait un flottant à partir d'une ligne
	 *\param[in]	p_strParams	La ligne contenant la valeur
	 *\param[out]	p_tResult	Reçoit le résultat
	 */
	template< typename T >
	inline bool ParseFloat( String & p_strParams, T & p_tResult )
	{
		bool l_bReturn = false;
		StringArray l_array = str_utils::split( p_strParams, cuT( " \t,;" ), 1, false );

		if( l_array.size() )
		{
			l_bReturn = str_utils::is_floating( l_array[0] );
			long double l_ldTmp = 0;

			if( l_bReturn )
			{
				l_ldTmp = str_utils::to_long_double( l_array[0] );
				l_bReturn = (l_ldTmp > std::numeric_limits< T >::lowest()) && (l_ldTmp < std::numeric_limits< T >::max());
			}

			if( l_bReturn )
			{
				p_tResult = static_cast< T >( l_ldTmp );
			}

			p_strParams.clear();

			if( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return l_bReturn;
	}
}

//*************************************************************************************************

template<> class ParserParameter< ePARAMETER_TYPE_TEXT > : public ParserParameterBase
{
public:
	typedef String value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_TEXT; }
	virtual xchar const * GetStrType() { return cuT( "text" ); }

	virtual bool Parse( String & p_strParams )
	{
		m_value = p_strParams;

		if( !m_value.empty() )
		{
			if( m_value[0] == cuT( '\"' ) )
			{
				m_value = m_value.substr( 1 );
				
				if( !m_value.empty() )
				{
					if( m_value[m_value.size() - 1] == cuT( '\"' ) )
					{
						m_value = m_value.substr( 0, m_value.size() - 1 );
					}
				}
			}
		}

		p_strParams.clear();
		return true;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_NAME > : public ParserParameter< ePARAMETER_TYPE_TEXT >
{
public:
	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_NAME; }
	virtual ePARAMETER_TYPE GetBaseType() { return ePARAMETER_TYPE_TEXT; }
	virtual xchar const * GetStrType() { return cuT( "name" ); }

	virtual bool Parse( String & p_strParams )
	{
		StringArray l_array = str_utils::split( p_strParams, cuT( " \t,;" ), 1, false );
		p_strParams.clear();

		if( l_array.size() )
		{
			m_value = l_array[0];
			str_utils::trim( m_value );

			if( !m_value.empty() )
			{
				if( m_value[0] == cuT( '\"' ) )
				{
					m_value = m_value.substr( 1 );
				
					if( !m_value.empty() )
					{
						std::size_t l_uiIndex = m_value.find( cuT( '\"' ) );

						if( l_uiIndex != String::npos )
						{
							if( l_uiIndex != m_value.size() - 1 )
							{
								l_array[1] = m_value.substr( l_uiIndex + 1 ) + l_array[1];
							}

							m_value = m_value.substr( 0, l_uiIndex );
						}
					}
				}
			}

			if( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return !m_value.empty();
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_PATH > : public ParserParameterBase
{
public:
	typedef Path value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_PATH; }
	virtual xchar const * GetStrType() { return cuT( "path" ); }

	virtual bool Parse( String & p_strParams )
	{
		m_value = p_strParams;
		p_strParams.clear();

		if( !m_value.empty() )
		{
			if( m_value[0] == cuT( '\"' ) )
			{
				m_value = m_value.substr( 1 );
				
				if( !m_value.empty() )
				{
					std::size_t l_uiIndex = m_value.find( cuT( '\"' ) );

					if( l_uiIndex != String::npos )
					{
						if( l_uiIndex != m_value.size() - 1 )
						{
							p_strParams = m_value.substr( l_uiIndex + 1 );
							str_utils::trim( p_strParams );
						}

						m_value = m_value.substr( 0, l_uiIndex );
					}
				}
			}
		}

		return !m_value.empty();
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_BOOL > : public ParserParameterBase
{
public:
	typedef bool value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_BOOL; }
	virtual xchar const * GetStrType() { return cuT( "boolean" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		StringArray l_array = str_utils::split( p_strParams, cuT( " \t,;" ), 1, false );
		p_strParams.clear();

		if( l_array.size() )
		{
			m_value = str_utils::to_lower_case( l_array[0] ) == cuT( "true" );
			l_bReturn = l_array[0] == cuT( "true" ) || l_array[0] == cuT( "false" );

			if( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_INT8 > : public ParserParameterBase
{
public:
	typedef int8_t value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_INT8; }
	virtual xchar const * GetStrType() { return cuT( "8 bits signed integer" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseInteger( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_INT16 > : public ParserParameterBase
{
public:
	typedef int16_t value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_INT16; }
	virtual xchar const * GetStrType() { return cuT( "16 bits signed integer" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseInteger( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_INT32 > : public ParserParameterBase
{
public:
	typedef int32_t value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_INT32; }
	virtual xchar const * GetStrType() { return cuT( "32 bits signed integer" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseInteger( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_INT64 > : public ParserParameterBase
{
public:
	typedef int64_t value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_INT64; }
	virtual xchar const * GetStrType() { return cuT( "64 bits signed integer" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseInteger( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_UINT8 > : public ParserParameterBase
{
public:
	typedef uint8_t value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_UINT8; }
	virtual xchar const * GetStrType() { return cuT( "8 bits unsigned integer" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseInteger( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_UINT16 > : public ParserParameterBase
{
public:
	typedef uint16_t value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_UINT16; }
	virtual xchar const * GetStrType() { return cuT( "16 bits unsigned integer" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseInteger( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_UINT32 > : public ParserParameterBase
{
public:
	typedef uint32_t value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_UINT32; }
	virtual xchar const * GetStrType() { return cuT( "32 bits unsigned integer" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseInteger( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT > : public ParserParameter< ePARAMETER_TYPE_UINT32 >
{
public:
	UIntStrMap m_mapValues;

	ParserParameter( UIntStrMap const & p_mapValues ) : m_mapValues( p_mapValues ) {}

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_CHECKED_TEXT; }
	virtual ePARAMETER_TYPE GetBaseType() { return ePARAMETER_TYPE_UINT32; }
	virtual xchar const * GetStrType() { return cuT( "checked text" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		StringArray l_array = str_utils::split( p_strParams, cuT( " \t,;" ), 1, false );
		p_strParams.clear();

		if( l_array.size() )
		{
			UIntStrMapIt l_it = m_mapValues.find( l_array[0] );

			if( l_it != m_mapValues.end() )
			{
				m_value = l_it->second;
				l_bReturn = true;
			}

			if( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_UINT64 > : public ParserParameterBase
{
public:
	typedef uint64_t value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_UINT64; }
	virtual xchar const * GetStrType() { return cuT( "64 bits unsigned integer" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseInteger( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_FLOAT > : public ParserParameterBase
{
public:
	typedef float value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_FLOAT; }
	virtual xchar const * GetStrType() { return cuT( "simple precision floating point" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseFloat( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_DOUBLE > : public ParserParameterBase
{
public:
	typedef double value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_DOUBLE; }
	virtual xchar const * GetStrType() { return cuT( "double precision floating point" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseFloat( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_LONGDOUBLE > : public ParserParameterBase
{
public:
	typedef long double value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_LONGDOUBLE; }
	virtual xchar const * GetStrType() { return cuT( "long double precision floating point" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseFloat( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_PIXELFORMAT > : public ParserParameterBase
{
public:
	typedef ePIXEL_FORMAT value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_PIXELFORMAT; }
	virtual xchar const * GetStrType() { return cuT( "pixel format" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		StringArray l_array = str_utils::split( p_strParams, cuT( " \t,;" ), 1, false );
		p_strParams.clear();

		if( l_array.size() )
		{
			m_value = PF::GetFormatByName( l_array[0] );
			l_bReturn = m_value != ePIXEL_FORMAT_COUNT;

			if( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_POINT2I > : public ParserParameterBase
{
public:
	typedef Point2i value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_POINT2I; }
	virtual xchar const * GetStrType() { return cuT( "2 integers point" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseVector( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_POINT3I > : public ParserParameterBase
{
public:
	typedef Point3i value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_POINT3I; }
	virtual xchar const * GetStrType() { return cuT( "3 integers point" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseVector( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_POINT4I > : public ParserParameterBase
{
public:
	typedef Point4i value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_POINT4I; }
	virtual xchar const * GetStrType() { return cuT( "4 integers point" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseVector( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_POINT2F > : public ParserParameterBase
{
public:
	typedef Point2f value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_POINT2F; }
	virtual xchar const * GetStrType() { return cuT( "2 simple precision floating point numbers point" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseVector( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_POINT3F > : public ParserParameterBase
{
public:
	typedef Point3f value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_POINT3F; }
	virtual xchar const * GetStrType() { return cuT( "3 simple precision floating point numbers point" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseVector( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_POINT4F > : public ParserParameterBase
{
public:
	typedef Point4f value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_POINT4F; }
	virtual xchar const * GetStrType() { return cuT( "4 simple precision floating point numbers point" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseVector( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_POINT2D > : public ParserParameterBase
{
public:
	typedef Point2d value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_POINT2D; }
	virtual xchar const * GetStrType() { return cuT( "2 double precision floating point numbers point" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseVector( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_POINT3D > : public ParserParameterBase
{
public:
	typedef Point3d value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_POINT3D; }
	virtual xchar const * GetStrType() { return cuT( "3 double precision floating point numbers point" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseVector( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_POINT4D > : public ParserParameterBase
{
public:
	typedef Point4d value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_POINT4D; }
	virtual xchar const * GetStrType() { return cuT( "4 double precision floating point numbers point" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseVector( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_SIZE > : public ParserParameterBase
{
public:
	typedef Size value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_SIZE; }
	virtual xchar const * GetStrType() { return cuT( "size" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseSize( p_strParams, m_value );
		return l_bReturn;
	}
};

template<> class ParserParameter< ePARAMETER_TYPE_COLOUR > : public ParserParameterBase
{
public:
	typedef Colour value_type;
	value_type m_value;

	virtual ePARAMETER_TYPE GetType() { return ePARAMETER_TYPE_COLOUR; }
	virtual xchar const * GetStrType() { return cuT( "colour" ); }

	virtual bool Parse( String & p_strParams )
	{
		bool l_bReturn = false;
		l_bReturn = ParseColour( p_strParams, m_value );
		return l_bReturn;
	}
};

//*************************************************************************************************

//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template< typename T > struct ParserValueTyper;
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< String			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_TEXT;			};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< Path			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_PATH;			};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< bool			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_BOOL;			};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< int8_t			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_INT8;			};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< int16_t			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_INT16;			};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< int32_t			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_INT32;			};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< int64_t			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_INT64;			};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< uint8_t			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_UINT8;			};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< uint16_t		>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_UINT16;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< uint32_t		>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_UINT32;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< uint64_t		>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_UINT64;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< float			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_FLOAT;			};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< double			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_DOUBLE;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< long double		>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_LONGDOUBLE;	};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< ePIXEL_FORMAT	>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_PIXELFORMAT;	};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< Point2i			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_POINT2I;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< Point3i			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_POINT3I;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< Point4i			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_POINT4I;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< Point2f			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_POINT2F;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< Point3f			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_POINT3F;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< Point4f			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_POINT4F;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< Point2d			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_POINT2D;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< Point3d			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_POINT3D;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< Point4d			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_POINT4D;		};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< Size			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_SIZE;			};
//!\~english	Retrieves parameter type from c++ type	\~french	Récupère le type de paramètre à partir du type C++
template<> struct ParserValueTyper< Colour			>{ static const ePARAMETER_TYPE Type=ePARAMETER_TYPE_COLOUR;		};

//*************************************************************************************************

template< typename T >
T const & ParserParameterBase :: Get( T & p_value )
{
	if( ParserValueTyper< T >::Type == GetBaseType() )
	{
		p_value = static_cast< ParserParameter< ParserValueTyper< T >::Type >* >( this )->m_value;
	}
	else
	{
		throw ParserParameterTypeException( ParserValueTyper< T >::Type, GetBaseType() );
	}

	return p_value;
}

//*************************************************************************************************