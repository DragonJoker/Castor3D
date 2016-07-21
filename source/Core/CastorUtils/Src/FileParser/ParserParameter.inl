#include "Miscellaneous/Utils.hpp"
#include "Log/Logger.hpp"
#include "Graphics/PixelFormat.hpp"
#include "FileParser/FileParserContext.hpp"
#include "FileParser/ParserParameterTypeException.hpp"

namespace Castor
{
	//*************************************************************************************************

	namespace
	{
		static xchar const * const VALUE_SEPARATOR = cuT( "[ \\t]*[ \\t,;][ \\t]*" );
		static xchar const * const IGNORED_END = cuT( "([^\\r\\n]*)" );
		/**
		 *\~english
		 *\brief		Parses a vector from a line.
		 *\param[in]	p_params	The line containing the vector.
		 *\param[out]	p_value		Receives the result.
		 *\param[in]	p_count		The elements count.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Extrait un vecteur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant le vecteur.
		 *\param[out]	p_value		Reçoit le résultat.
		 *\param[in]	p_count		Le nombre d'éléments.
		 *\return		\p true si tout s'est bien passé.
		 */
		template< typename T >
		inline bool ParseValues( String & p_params, size_t p_count, T * p_value )
		{
			bool l_return = false;

			try
			{
				String l_regexString = RegexFormat< T >::Value;

				for ( size_t i = 1; i < p_count; ++i )
				{
					l_regexString += String( VALUE_SEPARATOR ) + RegexFormat< T >::Value;
				}

				l_regexString += IGNORED_END;

				const Regex l_regex{ l_regexString };
				auto l_begin = std::begin( p_params );
				auto l_end = std::end( p_params );
				const SRegexIterator l_it( l_begin, l_end, l_regex );
				const SRegexIterator l_endit;
				String l_result;
				l_return = l_it != l_endit && l_it->size() >= p_count;

				if ( l_return )
				{
					for ( size_t i = 1; i <= p_count; ++i )
					{
						std::basic_istringstream< xchar > l_stream( ( *l_it )[i] );
						l_stream >> p_value[i - 1];
					}

					if ( l_it->size() > p_count )
					{
						String l_params;

						for ( size_t i = p_count + 1; i < l_it->size(); ++i )
						{
							if ( ( *l_it )[i].matched )
							{
								l_params += ( *l_it )[i];
							}
						}

						p_params = l_params;
					}
				}
				else
				{
					Logger::LogWarning( StringStream() << cuT( "Couldn't parse from " ) << p_params );
				}
			}
			catch ( std::exception & p_exc )
			{
				Logger::LogError( StringStream() << cuT( "Couldn't parse from " ) << p_params << cuT( ": " ) << string::string_cast< xchar >( p_exc.what() ) );
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Parses a vector from a line.
		 *\param[in]	p_params	The line containing the vector.
		 *\param[out]	p_value		Receives the result.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Extrait un vecteur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant le vecteur.
		 *\param[out]	p_value		Reçoit le résultat.
		 *\return		\p true si tout s'est bien passé.
		 */
		template< typename T, uint32_t Count >
		inline bool ParseValues( String & p_params, Point< T, Count > & p_value )
		{
			return ParseValues( p_params, Count, p_value.ptr() );
		}
		/**
		 *\~english
		 *\brief		Parses a vector from a line.
		 *\param[in]	p_params	The line containing the vector.
		 *\param[out]	p_value		Receives the result.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Extrait un vecteur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant le vecteur.
		 *\param[out]	p_value		Reçoit le résultat.
		 *\return		\p true si tout s'est bien passé.
		 */
		template< typename T, uint32_t Count >
		inline bool ParseValues( String & p_params, Coords< T, Count > & p_value )
		{
			return ParseValues( p_params, Count, p_value.ptr() );
		}
		/**
		 *\~english
		 *\brief		Parses a vector from a line.
		 *\param[in]	p_params	The line containing the vector.
		 *\param[out]	p_value		Receives the result.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Extrait un vecteur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant le vecteur.
		 *\param[out]	p_value		Reçoit le résultat.
		 *\return		\p true si tout s'est bien passé.
		 */
		template< typename T, uint32_t Count >
		inline bool ParseValues( String & p_params, Size & p_value )
		{
			return ParseValues( p_params, Count, p_value.ptr() );
		}
		/**
		 *\~english
		 *\brief		Parses a vector from a line.
		 *\param[in]	p_params	The line containing the vector.
		 *\param[out]	p_value		Receives the result.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Extrait un vecteur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant le vecteur.
		 *\param[out]	p_value		Reçoit le résultat.
		 *\return		\p true si tout s'est bien passé.
		 */
		template< typename T, uint32_t Count >
		inline bool ParseValues( String & p_params, Position & p_value )
		{
			return ParseValues( p_params, Count, p_value.ptr() );
		}
		/**
		 *\~english
		 *\brief		Parses a vector from a line.
		 *\param[in]	p_params	The line containing the vector.
		 *\param[out]	p_value		Receives the result.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Extrait un vecteur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant le vecteur.
		 *\param[out]	p_value		Reçoit le résultat.
		 *\return		\p true si tout s'est bien passé.
		 */
		template< typename T, uint32_t Count >
		inline bool ParseValues( String & p_params, Rectangle & p_value )
		{
			return ParseValues( p_params, Count, p_value.ptr() );
		}
	}

	//*************************************************************************************************

	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for signed integer types.
	\~french
	\brief		Spécialisation de ValueParser pour les type entiers signés.
	*/
	template< ePARAMETER_TYPE Type >
	struct ValueParser< Type, typename std::enable_if< ( Type >= ePARAMETER_TYPE_INT8 && Type <= ePARAMETER_TYPE_LONGDOUBLE ) >::type >
	{
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		/**
		 *\~english
		 *\brief		Parses a value from a line.
		 *\param[in]	p_params	The line containing the value.
		 *\param[out]	p_value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant la valeur.
		 *\param[out]	p_value		Reçoit le résultat.
		 */
		static inline bool Parse( String & p_params, ValueType & p_value )
		{
			return ParseValues( p_params, 1, &p_value );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for point types.
	\~french
	\brief		Spécialisation de ValueParser pour les type points.
	*/
	template< ePARAMETER_TYPE Type >
	struct ValueParser< Type, typename std::enable_if< ( Type >= ePARAMETER_TYPE_POINT2I && Type <= ePARAMETER_TYPE_RECTANGLE && Type != ePARAMETER_TYPE_SIZE ) >::type >
	{
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		/**
		 *\~english
		 *\brief		Parses a value from a line.
		 *\param[in]	p_params	The line containing the value.
		 *\param[out]	p_value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant la valeur.
		 *\param[out]	p_value		Reçoit le résultat.
		 */
		static inline bool Parse( String & p_params, ValueType & p_value )
		{
			return ParseValues( p_params, p_value );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for ePARAMETER_TYPE_TEXT.
	\~french
	\brief		Spécialisation de ValueParser pour ePARAMETER_TYPE_TEXT.
	*/
	template< ePARAMETER_TYPE Type >
	struct ValueParser< Type, typename std::enable_if< Type == ePARAMETER_TYPE_TEXT >::type >
	{
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		/**
		 *\~english
		 *\brief		Parses a value from a line.
		 *\param[in]	p_params	The line containing the value.
		 *\param[out]	p_value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant la valeur.
		 *\param[out]	p_value		Reçoit le résultat.
		 */
		static inline bool Parse( String & p_params, ValueType & p_value )
		{
			p_value = p_params;
			p_params.clear();

			if ( !p_value.empty() )
			{
				if ( p_value[0] == cuT( '\"' ) )
				{
					p_value = p_value.substr( 1 );

					if ( !p_value.empty() )
					{
						if ( p_value[p_value.size() - 1] == cuT( '\"' ) )
						{
							p_value = p_value.substr( 0, p_value.size() - 1 );
						}
					}
				}
			}

			return true;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for ePARAMETER_TYPE_PATH.
	\~french
	\brief		Spécialisation de ValueParser pour ePARAMETER_TYPE_PATH.
	*/
	template< ePARAMETER_TYPE Type >
	struct ValueParser< Type, typename std::enable_if< Type == ePARAMETER_TYPE_PATH >::type >
	{
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		/**
		 *\~english
		 *\brief		Parses a value from a line.
		 *\param[in]	p_params	The line containing the value.
		 *\param[out]	p_value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant la valeur.
		 *\param[out]	p_value		Reçoit le résultat.
		 */
		static inline bool Parse( String & p_params, ValueType & p_value )
		{
			p_value = Path{ p_params };
			p_params.clear();

			if ( !p_value.empty() )
			{
				if ( p_value[0] == cuT( '\"' ) )
				{
					p_value = Path{ p_value.substr( 1 ) };

					if ( !p_value.empty() )
					{
						std::size_t l_index = p_value.find( cuT( '\"' ) );

						if ( l_index != String::npos )
						{
							if ( l_index != p_value.size() - 1 )
							{
								p_params = p_value.substr( l_index + 1 );
								string::trim( p_params );
							}

							p_value = Path{ p_value.substr( 0, l_index ) };
						}
					}
				}
			}

			return !p_value.empty();
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for ePARAMETER_TYPE_BOOL.
	\~french
	\brief		Spécialisation de ValueParser pour ePARAMETER_TYPE_BOOL.
	*/
	template< ePARAMETER_TYPE Type >
	struct ValueParser< Type, typename std::enable_if< Type == ePARAMETER_TYPE_BOOL >::type >
	{
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		/**
		 *\~english
		 *\brief		Parses a value from a line.
		 *\param[in]	p_params	The line containing the value.
		 *\param[out]	p_value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant la valeur.
		 *\param[out]	p_value		Reçoit le résultat.
		 */
		static inline bool Parse( String & p_params, ValueType & p_value )
		{
			bool l_return = false;
			StringArray l_values = string::split( p_params, cuT( " \t,;" ), 1, false );
			p_params.clear();

			if ( !l_values.empty() )
			{
				p_value = string::to_lower_case( l_values[0] ) == cuT( "true" );
				l_return = l_values[0] == cuT( "true" ) || l_values[0] == cuT( "false" );

				if ( l_values.size() > 1 )
				{
					p_params = l_values[1];
				}
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for ePARAMETER_TYPE_PIXELFORMAT.
	\~french
	\brief		Spécialisation de ValueParser pour ePARAMETER_TYPE_PIXELFORMAT.
	*/
	template< ePARAMETER_TYPE Type >
	struct ValueParser< Type, typename std::enable_if< Type == ePARAMETER_TYPE_PIXELFORMAT >::type >
	{
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		/**
		 *\~english
		 *\brief		Parses a value from a line.
		 *\param[in]	p_params	The line containing the value.
		 *\param[out]	p_value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant la valeur.
		 *\param[out]	p_value		Reçoit le résultat.
		 */
		static inline bool Parse( String & p_params, ValueType & p_value )
		{
			bool l_return = false;
			StringArray l_values = string::split( p_params, cuT( " \t,;" ), 1, false );
			p_params.clear();

			if ( l_values.size() )
			{
				p_value = PF::GetFormatByName( l_values[0] );
				l_return = p_value != ePIXEL_FORMAT_COUNT;

				if ( l_values.size() > 1 )
				{
					p_params = l_values[1];
				}
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for ePARAMETER_TYPE_SIZE.
	\~french
	\brief		Spécialisation de ValueParser pour ePARAMETER_TYPE_SIZE.
	*/
	template< ePARAMETER_TYPE Type >
	struct ValueParser< Type, typename std::enable_if< Type == ePARAMETER_TYPE_SIZE >::type >
	{
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		/**
		 *\~english
		 *\brief		Parses a value from a line.
		 *\param[in]	p_params	The line containing the value.
		 *\param[out]	p_value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant la valeur.
		 *\param[out]	p_value		Reçoit le résultat.
		 */
		static inline bool Parse( String & p_params, ValueType & p_value )
		{
			bool l_return = false;

			if ( p_params == cuT( "screen_size" ) )
			{
				l_return = Castor::System::GetScreenSize( 0, p_value );
			}
			else
			{
				l_return = ParseValues( p_params, p_value );
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for ePARAMETER_TYPE_COLOUR.
	\~french
	\brief		Spécialisation de ValueParser pour ePARAMETER_TYPE_COLOUR.
	*/
	template< ePARAMETER_TYPE Type >
	struct ValueParser< Type, typename std::enable_if< Type == ePARAMETER_TYPE_COLOUR >::type >
	{
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		/**
		 *\~english
		 *\brief		Parses a value from a line.
		 *\param[in]	p_params	The line containing the value.
		 *\param[out]	p_value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	p_params	La ligne contenant la valeur.
		 *\param[out]	p_value		Reçoit le résultat.
		 */
		static inline bool Parse( String & p_params, ValueType & p_value )
		{
			bool l_return = false;
			StringArray l_values = string::split( p_params, cuT( " \t,;" ) );

			if ( l_values.size() >= Colour::eCOMPONENT_COUNT )
			{
				p_params.clear();

				std::for_each( l_values.begin() + Colour::eCOMPONENT_COUNT, l_values.end(), [&]( String const & p_param )
				{
					p_params += p_param + cuT( " " );
				} );

				string::trim( p_params );

				for ( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT; i++ )
				{
					std::basic_istringstream< xchar > l_stream( l_values[i] );
					double l_dComponent;
					l_stream >> l_dComponent;
					p_value[Colour::eCOMPONENT( i )] = l_dComponent;
				}

				l_return = true;
			}
			else if ( l_values.size() == 3 )
			{
				p_params.clear();

				for ( uint8_t i = 0; i < 3; i++ )
				{
					std::basic_istringstream< xchar > l_stream( l_values[i] );
					double l_dComponent;
					l_stream >> l_dComponent;
					p_value[Colour::eCOMPONENT( i )] = l_dComponent;
				}

				p_value[Colour::eCOMPONENT_ALPHA] = 1.0;
				l_return = true;
			}

			return l_return;
		}
	};

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_NAME >::ParserParameter()
		: ParserParameter< ePARAMETER_TYPE_TEXT >()
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_NAME >::GetType()
	{
		return ParserParameterHelper< ePARAMETER_TYPE_NAME >::ParameterType;
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_NAME >::GetBaseType()
	{
		return ParserParameterHelper< ePARAMETER_TYPE_NAME >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ePARAMETER_TYPE_NAME >::GetStrType()
	{
		return ParserParameterHelper< ePARAMETER_TYPE_NAME >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ePARAMETER_TYPE_NAME >::Clone()
	{
		return std::make_shared< ParserParameter< ePARAMETER_TYPE_NAME > >( *this );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_NAME >::Parse( String & p_params )
	{
		Regex l_regex{ cuT( "[^\"]*\"([^\"]*)\"" ) + String{ IGNORED_END } };
		auto l_begin = std::begin( p_params );
		auto l_end = std::end( p_params );
		SRegexIterator l_it( l_begin, l_end, l_regex );
		SRegexIterator l_endit;
		String l_result;

		if ( l_it != l_endit && l_it->size() > 2 )
		{
			m_value = ( *l_it )[1];
			p_params = ( *l_it )[2];
		}

		return !m_value.empty();
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >::ParserParameter( UIntStrMap const & p_values )
		: ParserParameter< ePARAMETER_TYPE_UINT32 >()
		, m_values( p_values )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >::GetType()
	{
		return ParserParameterHelper< ePARAMETER_TYPE_CHECKED_TEXT >::ParameterType;
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >::GetBaseType()
	{
		return ParserParameterHelper< ePARAMETER_TYPE_CHECKED_TEXT >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >::GetStrType()
	{
		return ParserParameterHelper< ePARAMETER_TYPE_CHECKED_TEXT >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >::Clone()
	{
		return std::make_shared< ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT > >( *this );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >::Parse( String & p_params )
	{
		bool l_return = false;
		StringArray l_values = string::split( p_params, cuT( " \t,;" ), 1, false );
		p_params.clear();

		if ( !l_values.empty() )
		{
			auto l_it = m_values.find( l_values[0] );

			if ( l_it != m_values.end() )
			{
				m_value = l_it->second;
				l_return = true;
			}

			if ( l_values.size() > 1 )
			{
				p_params = l_values[1];
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT >::ParserParameter( UIntStrMap const & p_values )
		: ParserParameter< ePARAMETER_TYPE_UINT32 >()
		, m_values( p_values )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT >::GetType()
	{
		return ParserParameterHelper< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT >::ParameterType;
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT >::GetBaseType()
	{
		return ParserParameterHelper< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT >::GetStrType()
	{
		return ParserParameterHelper< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT >::Clone()
	{
		return std::make_shared< ParserParameter< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT > >( *this );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT >::Parse( String & p_params )
	{
		bool l_return = false;
		m_value = 0;
		StringArray l_values = string::split( p_params, cuT( " \t,;" ), 1, false );
		p_params.clear();

		if ( !l_values.empty() )
		{
			StringArray l_values = string::split( l_values[0], cuT( "|" ), uint32_t( std::count( l_values[0].begin(), l_values[0].end(), cuT( '|' ) ) + 1 ), false );

			for ( auto l_value : l_values )
			{
				auto l_it = m_values.find( l_value );

				if ( l_it != m_values.end() )
				{
					m_value |= l_it->second;
					l_return = true;
				}
			}

			if ( l_values.size() > 1 )
			{
				p_params = l_values[1];
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	inline ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::ParserParameter( UInt64StrMap const & p_values )
		: ParserParameter< ePARAMETER_TYPE_UINT64 >()
		, m_values( p_values )
	{
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::GetType()
	{
		return ParserParameterHelper< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::ParameterType;
	}

	inline ePARAMETER_TYPE ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::GetBaseType()
	{
		return ParserParameterHelper< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::GetStrType()
	{
		return ParserParameterHelper< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::Clone()
	{
		return std::make_shared< ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT > >( *this );
	}

	inline bool ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >::Parse( String & p_params )
	{
		bool l_return = false;
		m_value = 0;
		StringArray l_params = string::split( p_params, cuT( " \t,;" ), 1, false );
		p_params.clear();

		if ( !l_params.empty() )
		{
			StringArray l_values = string::split( l_params[0], cuT( "|" ), uint32_t( std::count( l_params[0].begin(), l_params[0].end(), cuT( '|' ) ) + 1 ), false );

			for ( auto l_value : l_values )
			{
				auto l_it = m_values.find( l_value );

				if ( l_it != m_values.end() )
				{
					m_value |= l_it->second;
					l_return = true;
				}
			}

			if ( l_params.size() > 1 )
			{
				p_params = l_params[1];
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	template< typename T >
	T const & ParserParameterBase::Get( T & p_value )
	{
		static const ePARAMETER_TYPE l_given = ParserValueTyper< T >::Type;
		static const ePARAMETER_TYPE l_expected = GetBaseType();

		if ( l_given == l_expected )
		{
			p_value = static_cast< ParserParameter< l_given >* >( this )->m_value;
		}
		else
		{
			throw ParserParameterTypeException< l_given >( l_expected );
		}

		return p_value;
	}

	//*************************************************************************************************
}
