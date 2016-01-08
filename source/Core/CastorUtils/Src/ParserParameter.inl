#include "Utils.hpp"
#include "Logger.hpp"
#include "PixelFormat.hpp"
#include "FileParserContext.hpp"
#include "ParserParameterTypeException.hpp"

namespace Castor
{
	namespace
	{
		/**
		 *\~english
		 *\brief		Parses a vector from a line
		 *\param[in]	p_strParams	The line containing the vector
		 *\param[out]	p_vResult	Receives the result
		 *\param[in]	p_context	The current parsing context
		 *\return		\p true if OK
		 *\~french
		 *\brief		Extrait un vecteur à partir d'une ligne
		 *\param[in]	p_strParams	La ligne contenant le vecteur
		 *\param[out]	p_vResult	Reàoit le ràsultat
		 *\param[in]	p_context	Le context d'analyse
		 *\return		\p true si tout s'est bien passà
		 */
		inline bool ParseSize( String & p_strParams, Size & p_vResult )
		{
			bool l_return = false;

			if ( p_strParams == cuT( "screen_size" ) )
			{
				l_return = Castor::System::GetScreenSize( 0, p_vResult );
			}
			else
			{
				StringArray l_arrayValues = string::split( p_strParams, cuT( " \t,;" ) );

				if ( l_arrayValues.size() >= 2 )
				{
					p_strParams.clear();
					std::for_each( l_arrayValues.begin() + 2, l_arrayValues.end(), [&]( String const & p_strParam )
					{
						p_strParams += p_strParam + cuT( " " );
					} );
					string::trim( p_strParams );
					{
						std::basic_istringstream< xchar > l_stream( l_arrayValues[0] );
						l_stream >> p_vResult.ptr()[0];
					}
					{
						std::basic_istringstream< xchar > l_stream( l_arrayValues[1] );
						l_stream >> p_vResult.ptr()[1];
					}
					l_return = true;
				}
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Parses a vector from a line
		 *\param[in]	p_strParams	The line containing the vector
		 *\param[out]	p_vResult	Receives the result
		 *\param[in]	p_context	The current parsing context
		 *\return		\p true if OK
		 *\~french
		 *\brief		Extrait un vecteur à partir d'une ligne
		 *\param[in]	p_strParams	La ligne contenant le vecteur
		 *\param[out]	p_vResult	Reàoit le ràsultat
		 *\param[in]	p_context	Le context d'analyse
		 *\return		\p true si tout s'est bien passà
		 */
		inline bool ParsePosition( String & p_strParams, Position & p_vResult )
		{
			bool l_return = false;

			StringArray l_arrayValues = string::split( p_strParams, cuT( " \t,;" ) );

			if ( l_arrayValues.size() >= 2 )
			{
				p_strParams.clear();
				std::for_each( l_arrayValues.begin() + 2, l_arrayValues.end(), [&]( String const & p_strParam )
				{
					p_strParams += p_strParam + cuT( " " );
				} );
				string::trim( p_strParams );
				{
					std::basic_istringstream< xchar > l_stream( l_arrayValues[0] );
					l_stream >> p_vResult.ptr()[0];
				}
				{
					std::basic_istringstream< xchar > l_stream( l_arrayValues[1] );
					l_stream >> p_vResult.ptr()[1];
				}
				l_return = true;
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Parses a vector from a line
		 *\param[in]	p_strParams	The line containing the vector
		 *\param[out]	p_vResult	Receives the result
		 *\param[in]	p_context	The current parsing context
		 *\return		\p true if OK
		 *\~french
		 *\brief		Extrait un vecteur à partir d'une ligne
		 *\param[in]	p_strParams	La ligne contenant le vecteur
		 *\param[out]	p_vResult	Reàoit le ràsultat
		 *\param[in]	p_context	Le context d'analyse
		 *\return		\p true si tout s'est bien passà
		 */
		inline bool ParseRectangle( String & p_strParams, Rectangle & p_vResult )
		{
			bool l_return = false;

			StringArray l_arrayValues = string::split( p_strParams, cuT( " \t,;" ) );

			if ( l_arrayValues.size() >= 4 )
			{
				p_strParams.clear();
				std::for_each( l_arrayValues.begin(), l_arrayValues.end(), [&]( String const & p_strParam )
				{
					p_strParams += p_strParam + cuT( " " );
				} );
				string::trim( p_strParams );
				{
					std::basic_istringstream< xchar > l_stream( l_arrayValues[0] );
					l_stream >> p_vResult.ptr()[0];
				}
				{
					std::basic_istringstream< xchar > l_stream( l_arrayValues[1] );
					l_stream >> p_vResult.ptr()[1];
				}
				{
					std::basic_istringstream< xchar > l_stream( l_arrayValues[2] );
					l_stream >> p_vResult.ptr()[2];
				}
				{
					std::basic_istringstream< xchar > l_stream( l_arrayValues[3] );
					l_stream >> p_vResult.ptr()[3];
				}
				l_return = true;
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Parses a vector from a line
		 *\param[in]	p_strParams	The line containing the vector
		 *\param[out]	p_vResult	Receives the result
		 *\param[in]	p_context	The current parsing context
		 *\return		\p true if OK
		 *\~french
		 *\brief		Extrait un vecteur à partir d'une ligne
		 *\param[in]	p_strParams	La ligne contenant le vecteur
		 *\param[out]	p_vResult	Reàoit le ràsultat
		 *\param[in]	p_context	Le context d'analyse
		 *\return		\p true si tout s'est bien passà
		 */
		template< typename T, uint32_t Count >
		inline bool ParseVector( String & p_strParams, Point< T, Count > & p_vResult )
		{
			bool l_return = false;
			StringArray l_arrayValues = string::split( p_strParams, cuT( " \t,;" ), Count + 1, false );

			if ( l_arrayValues.size() >= Count )
			{
				p_strParams.clear();
				std::for_each( l_arrayValues.begin() + Count, l_arrayValues.end(), [&]( String const & p_strParam )
				{
					p_strParams += p_strParam + cuT( " " );
				} );
				string::trim( p_strParams );

				for ( uint32_t i = 0; i < Count; i++ )
				{
					std::basic_istringstream< xchar > l_stream( l_arrayValues[i] );
					l_stream >> p_vResult[i];
				}

				l_return = true;
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Parses a colour from a line
		 *\param[in]	p_strParams	The line containing the vector
		 *\param[out]	p_colour	Receives the result
		 *\param[in]	p_context	The current parsing context
		 *\return		\p true if OK
		 *\~french
		 *\brief		Extrait une couleur à partir d'une ligne
		 *\param[in]	p_strParams	La ligne contenant le vecteur
		 *\param[out]	p_colour	Reàoit le ràsultat
		 *\param[in]	p_context	Le context d'analyse
		 *\return		\p true si tout s'est bien passà
		 */
		inline bool ParseColour( String & p_strParams, Colour & p_colour )
		{
			bool l_return = false;
			StringArray l_arrayValues = string::split( p_strParams, cuT( " \t,;" ) );

			if ( l_arrayValues.size() >= Colour::eCOMPONENT_COUNT )
			{
				p_strParams.clear();
				std::for_each( l_arrayValues.begin() + Colour::eCOMPONENT_COUNT, l_arrayValues.end(), [&]( String const & p_strParam )
				{
					p_strParams += p_strParam + cuT( " " );
				} );
				string::trim( p_strParams );

				for ( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT; i++ )
				{
					std::basic_istringstream< xchar > l_stream( l_arrayValues[i] );
					double l_dComponent;
					l_stream >> l_dComponent;
					p_colour[Colour::eCOMPONENT( i )] = l_dComponent;
				}

				l_return = true;
			}
			else if ( l_arrayValues.size() == 3 )
			{
				p_strParams.clear();

				for ( uint8_t i = 0; i < 3; i++ )
				{
					std::basic_istringstream< xchar > l_stream( l_arrayValues[i] );
					double l_dComponent;
					l_stream >> l_dComponent;
					p_colour[Colour::eCOMPONENT( i )] = l_dComponent;
				}

				p_colour[Colour::eCOMPONENT_ALPHA] = 1.0;
				l_return = true;
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Parses a signed integer from a line
		 *\param[in]	p_strParams	The line containing the value
		 *\param[out]	p_tResult	Receives the result
		 *\~french
		 *\brief		Extrait un entier signà à partir d'une ligne
		 *\param[in]	p_strParams	La ligne contenant la valeur
		 *\param[out]	p_tResult	Reàoit le ràsultat
		 */
		template< typename T >
		inline bool ParseInteger( String & p_strParams, T & p_tResult, typename std::enable_if < !std::is_unsigned< T >::value >::type * = 0 )
		{
			bool l_return = false;
			StringArray l_array = string::split( p_strParams, cuT( " \t,;" ), 1, false );

			if ( l_array.size() )
			{
				l_return = string::is_integer( l_array[0] );
				int64_t l_i64Tmp = 0;

				if ( l_return )
				{
					l_i64Tmp = string::to_long_long( l_array[0] );
					l_return = ( l_i64Tmp > std::numeric_limits< T >::lowest() ) && ( l_i64Tmp < std::numeric_limits< T >::max() );
				}

				if ( l_return )
				{
					p_tResult = static_cast< T >( l_i64Tmp );
				}

				p_strParams.clear();

				if ( l_array.size() > 1 )
				{
					p_strParams = l_array[1];
				}
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Parses an unsigned integer from a line
		 *\param[in]	p_strParams	The line containing the value
		 *\param[out]	p_tResult	Receives the result
		 *\~french
		 *\brief		Extrait un entier non signà à partir d'une ligne
		 *\param[in]	p_strParams	La ligne contenant la valeur
		 *\param[out]	p_tResult	Reàoit le ràsultat
		 */
		template< typename T >
		inline bool ParseInteger( String & p_strParams, T & p_tResult, typename std::enable_if< std::is_unsigned< T >::value >::type * = 0 )
		{
			bool l_return = false;
			StringArray l_array = string::split( p_strParams, cuT( " \t,;" ), 1, false );

			if ( l_array.size() )
			{
				l_return = string::is_integer( l_array[0] );
				uint64_t l_ui64Tmp = 0;

				if ( l_return )
				{
					l_ui64Tmp = string::to_long_long( l_array[0] );
					l_return = ( l_ui64Tmp > std::numeric_limits< T >::lowest() ) && ( l_ui64Tmp < std::numeric_limits< T >::max() );
				}

				if ( l_return )
				{
					p_tResult = static_cast< T >( l_ui64Tmp );
				}

				p_strParams.clear();

				if ( l_array.size() > 1 )
				{
					p_strParams = l_array[1];
				}
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Parses an floating number from a line
		 *\param[in]	p_strParams	The line containing the value
		 *\param[out]	p_tResult	Receives the result
		 *\~french
		 *\brief		Extrait un flottant à partir d'une ligne
		 *\param[in]	p_strParams	La ligne contenant la valeur
		 *\param[out]	p_tResult	Reàoit le ràsultat
		 */
		template< typename T >
		inline bool ParseFloat( String & p_strParams, T & p_tResult )
		{
			bool l_return = false;
			StringArray l_array = string::split( p_strParams, cuT( " \t,;" ), 1, false );

			if ( l_array.size() )
			{
				l_return = string::is_floating( l_array[0] );
				long double l_ldTmp = 0;

				if ( l_return )
				{
					l_ldTmp = string::to_long_double( l_array[0] );
					l_return = ( l_ldTmp > std::numeric_limits< T >::lowest() ) && ( l_ldTmp < std::numeric_limits< T >::max() );
				}

				if ( l_return )
				{
					p_tResult = static_cast< T >( l_ldTmp );
				}

				p_strParams.clear();

				if ( l_array.size() > 1 )
				{
					p_strParams = l_array[1];
				}
			}

			return l_return;
		}
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_TEXT >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_TEXT >::GetType()
	{
		return ePARAMETER_TYPE_TEXT;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_TEXT >::GetStrType()
	{
		return cuT( "text" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_TEXT >::Parse( String & p_strParams )
	{
		m_value = p_strParams;

		if ( !m_value.empty() )
		{
			if ( m_value[0] == cuT( '\"' ) )
			{
				m_value = m_value.substr( 1 );

				if ( !m_value.empty() )
				{
					if ( m_value[m_value.size() - 1] == cuT( '\"' ) )
					{
						m_value = m_value.substr( 0, m_value.size() - 1 );
					}
				}
			}
		}

		p_strParams.clear();
		return true;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_NAME >::ParserParameter( String const & p_functionName )
		: ParserParameter< ePARAMETER_TYPE_TEXT >( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_NAME >::GetType()
	{
		return ePARAMETER_TYPE_NAME;
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_NAME >::GetBaseType()
	{
		return ePARAMETER_TYPE_TEXT;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_NAME >::GetStrType()
	{
		return cuT( "name" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_NAME >::Parse( String & p_strParams )
	{
		StringArray l_array = string::split( p_strParams, cuT( " \t,;" ), 1, false );
		p_strParams.clear();

		if ( !l_array.empty() )
		{
			m_value = l_array[0];
			string::trim( m_value );

			if ( !m_value.empty() )
			{
				if ( m_value[0] == cuT( '\"' ) )
				{
					m_value = m_value.substr( 1 );

					if ( !m_value.empty() )
					{
						std::size_t l_index = m_value.find( cuT( '\"' ) );

						if ( l_index != String::npos )
						{
							if ( l_index != m_value.size() - 1 )
							{
								l_array[1] = m_value.substr( l_index + 1 ) + l_array[1];
							}

							m_value = m_value.substr( 0, l_index );
						}
					}
				}
			}

			if ( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return !m_value.empty();
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_PATH >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_PATH >::GetType()
	{
		return ePARAMETER_TYPE_PATH;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_PATH >::GetStrType()
	{
		return cuT( "path" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_PATH >::Parse( String & p_strParams )
	{
		m_value = p_strParams;
		p_strParams.clear();

		if ( !m_value.empty() )
		{
			if ( m_value[0] == cuT( '\"' ) )
			{
				m_value = m_value.substr( 1 );

				if ( !m_value.empty() )
				{
					std::size_t l_index = m_value.find( cuT( '\"' ) );

					if ( l_index != String::npos )
					{
						if ( l_index != m_value.size() - 1 )
						{
							p_strParams = m_value.substr( l_index + 1 );
							string::trim( p_strParams );
						}

						m_value = m_value.substr( 0, l_index );
					}
				}
			}
		}

		return !m_value.empty();
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_BOOL >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_BOOL >::GetType()
	{
		return ePARAMETER_TYPE_BOOL;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_BOOL >::GetStrType()
	{
		return cuT( "boolean" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_BOOL >::Parse( String & p_strParams )
	{
		bool l_return = false;
		StringArray l_array = string::split( p_strParams, cuT( " \t,;" ), 1, false );
		p_strParams.clear();

		if ( !l_array.empty() )
		{
			m_value = string::to_lower_case( l_array[0] ) == cuT( "true" );
			l_return = l_array[0] == cuT( "true" ) || l_array[0] == cuT( "false" );

			if ( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_INT8 >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_INT8 >::GetType()
	{
		return ePARAMETER_TYPE_INT8;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_INT8 >::GetStrType()
	{
		return cuT( "8 bits signed integer" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_INT8 >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseInteger( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_INT16 >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_INT16 >::GetType()
	{
		return ePARAMETER_TYPE_INT16;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_INT16 >::GetStrType()
	{
		return cuT( "16 bits signed integer" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_INT16 >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseInteger( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_INT32 >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_INT32 >::GetType()
	{
		return ePARAMETER_TYPE_INT32;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_INT32 >::GetStrType()
	{
		return cuT( "32 bits signed integer" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_INT32 >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseInteger( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_INT64 >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_INT64 >::GetType()
	{
		return ePARAMETER_TYPE_INT64;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_INT64 >::GetStrType()
	{
		return cuT( "64 bits signed integer" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_INT64 >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseInteger( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_UINT8 >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_UINT8 >::GetType()
	{
		return ePARAMETER_TYPE_UINT8;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_UINT8 >::GetStrType()
	{
		return cuT( "8 bits unsigned integer" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_UINT8 >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseInteger( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_UINT16 >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_UINT16 >::GetType()
	{
		return ePARAMETER_TYPE_UINT16;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_UINT16 >::GetStrType()
	{
		return cuT( "16 bits unsigned integer" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_UINT16 >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseInteger( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_UINT32 >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_UINT32 >::GetType()
	{
		return ePARAMETER_TYPE_UINT32;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_UINT32 >::GetStrType()
	{
		return cuT( "32 bits unsigned integer" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_UINT32 >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseInteger( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >::ParserParameter( String const & p_functionName, UIntStrMap const & p_mapValues )
		: ParserParameter< ePARAMETER_TYPE_UINT32 >( p_functionName )
		, m_mapValues( p_mapValues )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >::GetType()
	{
		return ePARAMETER_TYPE_CHECKED_TEXT;
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >::GetBaseType()
	{
		return ePARAMETER_TYPE_UINT32;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >::GetStrType()
	{
		return cuT( "checked text" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >::Parse( String & p_strParams )
	{
		bool l_return = false;
		StringArray l_array = string::split( p_strParams, cuT( " \t,;" ), 1, false );
		p_strParams.clear();

		if ( !l_array.empty() )
		{
			auto l_it = m_mapValues.find( l_array[0] );

			if ( l_it != m_mapValues.end() )
			{
				m_value = l_it->second;
				l_return = true;
			}

			if ( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT >::ParserParameter( String const & p_functionName, UIntStrMap const & p_mapValues )
		: ParserParameter< ePARAMETER_TYPE_UINT32 >( p_functionName )
		, m_mapValues( p_mapValues )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT >::GetType()
	{
		return ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT;
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT >::GetBaseType()
	{
		return ePARAMETER_TYPE_UINT32;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT >::GetStrType()
	{
		return cuT( "32bits bitwise ORed checked texts" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT >::Parse( String & p_strParams )
	{
		bool l_return = false;
		m_value = 0;
		StringArray l_array = string::split( p_strParams, cuT( " \t,;" ), 1, false );
		p_strParams.clear();

		if ( !l_array.empty() )
		{
			StringArray l_values = string::split( l_array[0], cuT( "|" ), std::count( l_array[0].begin(), l_array[0].end(), cuT( '|' ) ) + 1, false );

			for ( auto && l_value : l_values )
			{
				auto l_it = m_mapValues.find( l_value );

				if ( l_it != m_mapValues.end() )
				{
					m_value |= l_it->second;
					l_return = true;
				}
			}

			if ( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_UINT64 >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_UINT64 >::GetType()
	{
		return ePARAMETER_TYPE_UINT64;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_UINT64 >::GetStrType()
	{
		return cuT( "64 bits unsigned integer" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_UINT64 >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseInteger( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::ParserParameter( String const & p_functionName, UInt64StrMap const & p_mapValues )
		: ParserParameter< ePARAMETER_TYPE_UINT64 >( p_functionName )
		, m_mapValues( p_mapValues )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::GetType()
	{
		return ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT;
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::GetBaseType()
	{
		return ePARAMETER_TYPE_UINT64;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::GetStrType()
	{
		return cuT( "64 bits bitwise ORed checked texts" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::Parse( String & p_strParams )
	{
		bool l_return = false;
		m_value = 0;
		StringArray l_array = string::split( p_strParams, cuT( " \t,;" ), 1, false );
		p_strParams.clear();

		if ( !l_array.empty() )
		{
			StringArray l_values = string::split( l_array[0], cuT( "|" ), std::count( l_array[0].begin(), l_array[0].end(), cuT( '|' ) ) + 1, false );

			for ( auto && l_value : l_values )
			{
				auto l_it = m_mapValues.find( l_value );

				if ( l_it != m_mapValues.end() )
				{
					m_value |= l_it->second;
					l_return = true;
				}
			}

			if ( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_FLOAT >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_FLOAT >::GetType()
	{
		return ePARAMETER_TYPE_FLOAT;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_FLOAT >::GetStrType()
	{
		return cuT( "simple precision floating point" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_FLOAT >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseFloat( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_DOUBLE >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_DOUBLE >::GetType()
	{
		return ePARAMETER_TYPE_DOUBLE;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_DOUBLE >::GetStrType()
	{
		return cuT( "double precision floating point" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_DOUBLE >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseFloat( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_LONGDOUBLE >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_LONGDOUBLE >::GetType()
	{
		return ePARAMETER_TYPE_LONGDOUBLE;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_LONGDOUBLE >::GetStrType()
	{
		return cuT( "long double precision floating point" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_LONGDOUBLE >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseFloat( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_PIXELFORMAT >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_PIXELFORMAT >::GetType()
	{
		return ePARAMETER_TYPE_PIXELFORMAT;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_PIXELFORMAT >::GetStrType()
	{
		return cuT( "pixel format" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_PIXELFORMAT >::Parse( String & p_strParams )
	{
		bool l_return = false;
		StringArray l_array = string::split( p_strParams, cuT( " \t,;" ), 1, false );
		p_strParams.clear();

		if ( l_array.size() )
		{
			m_value = PF::GetFormatByName( l_array[0] );
			l_return = m_value != ePIXEL_FORMAT_COUNT;

			if ( l_array.size() > 1 )
			{
				p_strParams = l_array[1];
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_POINT2I >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_POINT2I >::GetType()
	{
		return ePARAMETER_TYPE_POINT2I;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_POINT2I >::GetStrType()
	{
		return cuT( "2 integers point" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_POINT2I >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseVector( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_POINT3I >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_POINT3I >::GetType()
	{
		return ePARAMETER_TYPE_POINT3I;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_POINT3I >::GetStrType()
	{
		return cuT( "3 integers point" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_POINT3I >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseVector( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_POINT4I >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_POINT4I >::GetType()
	{
		return ePARAMETER_TYPE_POINT4I;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_POINT4I >::GetStrType()
	{
		return cuT( "4 integers point" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_POINT4I >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseVector( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_POINT2F >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_POINT2F >::GetType()
	{
		return ePARAMETER_TYPE_POINT2F;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_POINT2F >::GetStrType()
	{
		return cuT( "2 simple precision floating point numbers point" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_POINT2F >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseVector( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_POINT3F >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_POINT3F >::GetType()
	{
		return ePARAMETER_TYPE_POINT3F;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_POINT3F >::GetStrType()
	{
		return cuT( "3 simple precision floating point numbers point" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_POINT3F >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseVector( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_POINT4F >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_POINT4F >::GetType()
	{
		return ePARAMETER_TYPE_POINT4F;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_POINT4F >::GetStrType()
	{
		return cuT( "4 simple precision floating point numbers point" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_POINT4F >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseVector( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_POINT2D >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_POINT2D >::GetType()
	{
		return ePARAMETER_TYPE_POINT2D;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_POINT2D >::GetStrType()
	{
		return cuT( "2 double precision floating point numbers point" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_POINT2D >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseVector( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_POINT3D >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_POINT3D >::GetType()
	{
		return ePARAMETER_TYPE_POINT3D;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_POINT3D >::GetStrType()
	{
		return cuT( "3 double precision floating point numbers point" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_POINT3D >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseVector( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_POINT4D >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_POINT4D >::GetType()
	{
		return ePARAMETER_TYPE_POINT4D;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_POINT4D >::GetStrType()
	{
		return cuT( "4 double precision floating point numbers point" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_POINT4D >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseVector( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_SIZE >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_SIZE >::GetType()
	{
		return ePARAMETER_TYPE_SIZE;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_SIZE >::GetStrType()
	{
		return cuT( "size" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_SIZE >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseSize( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_POSITION >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_POSITION >::GetType()
	{
		return ePARAMETER_TYPE_POSITION;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_POSITION >::GetStrType()
	{
		return cuT( "position" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_POSITION >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParsePosition( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_RECTANGLE >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_RECTANGLE >::GetType()
	{
		return ePARAMETER_TYPE_RECTANGLE;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_RECTANGLE >::GetStrType()
	{
		return cuT( "rectangle" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_RECTANGLE >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseRectangle( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_COLOUR >::ParserParameter( String const & p_functionName )
		: ParserParameterBase( p_functionName )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_COLOUR >::GetType()
	{
		return ePARAMETER_TYPE_COLOUR;
	}

	inline xchar const * ParserParameter< ePARAMETER_TYPE_COLOUR >::GetStrType()
	{
		return cuT( "colour" );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_COLOUR >::Parse( String & p_strParams )
	{
		bool l_return = false;
		l_return = ParseColour( p_strParams, m_value );
		return l_return;
	}

	//*************************************************************************************************

	template< typename T >
	T const & ParserParameterBase::Get( T & p_value )
	{
		static const ePARAMETER_TYPE l_given = ParserValueTyper< T >::Type;
		ePARAMETER_TYPE l_expected = GetBaseType();

		if ( l_given == l_expected )
		{
			p_value = static_cast< ParserParameter< l_given >* >( this )->m_value;
		}
		else
		{
			throw ParserParameterTypeException( m_functionName, l_given, l_expected );
		}

		return p_value;
	}
}

