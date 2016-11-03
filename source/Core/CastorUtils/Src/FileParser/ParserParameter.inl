#include "Miscellaneous/Utils.hpp"
#include "Log/Logger.hpp"
#include "Graphics/PixelFormat.hpp"
#include "FileParserContext.hpp"
#include "ParserParameterTypeException.hpp"

namespace Castor
{
	//*************************************************************************************************

	namespace details
	{
		static xchar const * const VALUE_SEPARATOR = cuT( "[ \\t]*[ \\t,;][ \\t]*" );
		static xchar const * const IGNORED_END = cuT( "([^\\r\\n]*)" );
	}

	//*************************************************************************************************

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
				l_regexString += String( details::VALUE_SEPARATOR ) + RegexFormat< T >::Value;
			}

			l_regexString += details::IGNORED_END;

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
	template< ParameterType Type >
	struct ValueParser < Type, typename std::enable_if < ( Type >= ParameterType::Int8 && Type <= ParameterType::LongDouble ) >::type >
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
	template< ParameterType Type >
	struct ValueParser < Type, typename std::enable_if < ( Type >= ParameterType::Point2I && Type <= ParameterType::Rectangle && Type != ParameterType::Size ) >::type >
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
	\brief		ValueParser specialisation for ParameterType::Text.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::Text.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::Text >::type >
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
	\brief		ValueParser specialisation for ParameterType::Path.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::Path.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::Path >::type >
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
	\brief		ValueParser specialisation for ParameterType::Bool.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::Bool.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::Bool >::type >
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
	\brief		ValueParser specialisation for ParameterType::PixelFormat.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::PixelFormat.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::PixelFormat >::type >
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
				l_return = p_value != PixelFormat::Count;

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
	\brief		ValueParser specialisation for ParameterType::Size.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::Size.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::Size >::type >
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
	\brief		ValueParser specialisation for ParameterType::Colour.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::Colour.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::Colour >::type >
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
			StringArray l_values = string::split( p_params, cuT( " \t,;" ), 5, false );

			if ( l_values.size() >= Colour::eCOMPONENT_COUNT )
			{
				Point4f l_value;
				l_return = ParseValues( p_params, l_value );

				if ( l_return )
				{
					for ( uint8_t i = 0; i < Colour::eCOMPONENT_COUNT; i++ )
					{
						p_value[Colour::eCOMPONENT( i )] = l_value[i];
					}
				}
			}
			else if ( l_values.size() == 3 )
			{
				Point3f l_value;
				l_return = ParseValues( p_params, l_value );

				if ( l_return )
				{
					for ( uint8_t i = 0; i < 3; i++ )
					{
						p_value[Colour::eCOMPONENT( i )] = l_value[i];
					}

					p_value[Colour::eCOMPONENT_ALPHA] = 1.0;
				}
			}
			else
			{
				try
				{
					String l_regexString = RegexFormat< Colour >::Value;
					l_regexString += details::IGNORED_END;

					const Regex l_regex{ l_regexString };
					auto l_begin = std::begin( p_params );
					auto l_end = std::end( p_params );
					const SRegexIterator l_it( l_begin, l_end, l_regex );
					const SRegexIterator l_endit;
					String l_result;
					l_return = l_it != l_endit && l_it->size() >= 1;

					if ( l_return )
					{
						uint32_t l_value{ 0u };

						for ( size_t i = 0; i < l_it->size() && l_value == 0u; ++i )
						{
							auto l_match = ( *l_it )[i];

							if ( l_match.matched )
							{
								String l_text = l_match;

								if ( l_text.size() == 6 )
								{
									l_text = "FF" + l_text;
								}

								std::basic_istringstream< xchar > l_stream{ l_text };
								l_stream >> std::hex >> l_value;
							}
						}

						p_value = Colour::from_argb( l_value );
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

			return l_return;
		}
	};

	//*************************************************************************************************

	inline ParserParameter< ParameterType::Name >::ParserParameter()
		: ParserParameter< ParameterType::Text >()
	{
	}

	inline ParameterType ParserParameter< ParameterType::Name >::GetType()
	{
		return ParserParameterHelper< ParameterType::Name >::ParamType;
	}

	inline ParameterType ParserParameter< ParameterType::Name >::GetBaseType()
	{
		return ParserParameterHelper< ParameterType::Name >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ParameterType::Name >::GetStrType()
	{
		return ParserParameterHelper< ParameterType::Name >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ParameterType::Name >::Clone()
	{
		return std::make_shared< ParserParameter< ParameterType::Name > >( *this );
	}

	inline bool ParserParameter< ParameterType::Name >::Parse( String & p_params )
	{
		Regex l_regex{ cuT( "[^\"]*\"([^\"]*)\"" ) + String{ details::IGNORED_END } };
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

	inline ParserParameter< ParameterType::CheckedText >::ParserParameter( UIntStrMap const & p_values )
		: ParserParameter< ParameterType::UInt32 >()
		, m_values( p_values )
	{
	}

	inline ParameterType ParserParameter< ParameterType::CheckedText >::GetType()
	{
		return ParserParameterHelper< ParameterType::CheckedText >::ParamType;
	}

	inline ParameterType ParserParameter< ParameterType::CheckedText >::GetBaseType()
	{
		return ParserParameterHelper< ParameterType::CheckedText >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ParameterType::CheckedText >::GetStrType()
	{
		return ParserParameterHelper< ParameterType::CheckedText >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ParameterType::CheckedText >::Clone()
	{
		return std::make_shared< ParserParameter< ParameterType::CheckedText > >( *this );
	}

	inline bool ParserParameter< ParameterType::CheckedText >::Parse( String & p_params )
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

	inline ParserParameter< ParameterType::BitwiseOred32BitsCheckedText >::ParserParameter( UIntStrMap const & p_values )
		: ParserParameter< ParameterType::UInt32 >()
		, m_values( p_values )
	{
	}

	inline ParameterType ParserParameter< ParameterType::BitwiseOred32BitsCheckedText >::GetType()
	{
		return ParserParameterHelper< ParameterType::BitwiseOred32BitsCheckedText >::ParamType;
	}

	inline ParameterType ParserParameter< ParameterType::BitwiseOred32BitsCheckedText >::GetBaseType()
	{
		return ParserParameterHelper< ParameterType::BitwiseOred32BitsCheckedText >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ParameterType::BitwiseOred32BitsCheckedText >::GetStrType()
	{
		return ParserParameterHelper< ParameterType::BitwiseOred32BitsCheckedText >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ParameterType::BitwiseOred32BitsCheckedText >::Clone()
	{
		return std::make_shared< ParserParameter< ParameterType::BitwiseOred32BitsCheckedText > >( *this );
	}

	inline bool ParserParameter< ParameterType::BitwiseOred32BitsCheckedText >::Parse( String & p_params )
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

	inline ParserParameter< ParameterType::BitwiseOred64BitsCheckedText >::ParserParameter( UInt64StrMap const & p_values )
		: ParserParameter< ParameterType::UInt64 >()
		, m_values( p_values )
	{
	}

	inline ParameterType ParserParameter< ParameterType::BitwiseOred64BitsCheckedText >::GetType()
	{
		return ParserParameterHelper< ParameterType::BitwiseOred64BitsCheckedText >::ParamType;
	}

	inline ParameterType ParserParameter< ParameterType::BitwiseOred64BitsCheckedText >::GetBaseType()
	{
		return ParserParameterHelper< ParameterType::BitwiseOred64BitsCheckedText >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ParameterType::BitwiseOred64BitsCheckedText >::GetStrType()
	{
		return ParserParameterHelper< ParameterType::BitwiseOred64BitsCheckedText >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ParameterType::BitwiseOred64BitsCheckedText >::Clone()
	{
		return std::make_shared< ParserParameter< ParameterType::BitwiseOred64BitsCheckedText > >( *this );
	}

	inline bool ParserParameter< ParameterType::BitwiseOred64BitsCheckedText >::Parse( String & p_params )
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
		static const ParameterType l_given = ParserValueTyper< T >::Type;
		static const ParameterType l_expected = GetBaseType();

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
