#include "Miscellaneous/Utils.hpp"
#include "Log/Logger.hpp"
#include "Graphics/PixelFormat.hpp"
#include "FileParserContext.hpp"
#include "ParserParameterTypeException.hpp"

namespace castor
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
	inline bool parseValues( String & p_params, size_t p_count, T * p_value )
	{
		bool result = false;

		try
		{
			String regexString = RegexFormat< T >::Value;

			for ( size_t i = 1; i < p_count; ++i )
			{
				regexString += String( details::VALUE_SEPARATOR ) + RegexFormat< T >::Value;
			}

			regexString += details::IGNORED_END;

			const Regex regex{ regexString };
			auto begin = std::begin( p_params );
			auto end = std::end( p_params );
			const SRegexIterator it( begin, end, regex );
			const SRegexIterator endit;
			result = it != endit && it->size() >= p_count;

			if ( result )
			{
				for ( size_t i = 1; i <= p_count; ++i )
				{
					std::basic_istringstream< xchar > stream( ( *it )[i] );
					stream >> p_value[i - 1];
				}

				if ( it->size() > p_count )
				{
					String params;

					for ( size_t i = p_count + 1; i < it->size(); ++i )
					{
						if ( ( *it )[i].matched )
						{
							params += ( *it )[i];
						}
					}

					p_params = params;
				}
			}
			else
			{
				Logger::logWarning( StringStream() << cuT( "Couldn't parse from " ) << p_params );
			}
		}
		catch ( std::exception & p_exc )
		{
			Logger::logError( StringStream() << cuT( "Couldn't parse from " ) << p_params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
		}

		return result;
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
	inline bool parseValues( String & p_params, Point< T, Count > & p_value )
	{
		return parseValues( p_params, Count, p_value.ptr() );
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
	inline bool parseValues( String & p_params, Coords< T, Count > & p_value )
	{
		return parseValues( p_params, Count, p_value.ptr() );
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
	inline bool parseValues( String & p_params, Size & p_value )
	{
		return parseValues( p_params, Count, p_value.ptr() );
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
	inline bool parseValues( String & p_params, Position & p_value )
	{
		return parseValues( p_params, Count, p_value.ptr() );
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
	inline bool parseValues( String & p_params, Rectangle & p_value )
	{
		return parseValues( p_params, Count, p_value.ptr() );
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
	struct ValueParser < Type, typename std::enable_if < ( Type >= ParameterType::eInt8 && Type <= ParameterType::eLongDouble ) >::type >
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
		static inline bool parse( String & p_params, ValueType & p_value )
		{
			return parseValues( p_params, 1, &p_value );
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
	struct ValueParser < Type, typename std::enable_if < ( Type >= ParameterType::ePoint2I && Type <= ParameterType::eRectangle && Type != ParameterType::eSize ) >::type >
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
		static inline bool parse( String & p_params, ValueType & p_value )
		{
			return parseValues( p_params, p_value );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for ParameterType::eText.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::eText.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::eText >::type >
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
		static inline bool parse( String & p_params, ValueType & p_value )
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
	\brief		ValueParser specialisation for ParameterType::ePath.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::ePath.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::ePath >::type >
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
		static inline bool parse( String & p_params, ValueType & p_value )
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
						std::size_t index = p_value.find( cuT( '\"' ) );

						if ( index != String::npos )
						{
							if ( index != p_value.size() - 1 )
							{
								p_params = p_value.substr( index + 1 );
								string::trim( p_params );
							}

							p_value = Path{ p_value.substr( 0, index ) };
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
	\brief		ValueParser specialisation for ParameterType::eBool.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::eBool.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::eBool >::type >
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
		static inline bool parse( String & p_params, ValueType & p_value )
		{
			bool result = false;
			StringArray values = string::split( p_params, cuT( " \t,;" ), 1, false );
			p_params.clear();

			if ( !values.empty() )
			{
				p_value = string::toLowerCase( values[0] ) == cuT( "true" );
				result = values[0] == cuT( "true" ) || values[0] == cuT( "false" );

				if ( values.size() > 1 )
				{
					p_params = values[1];
				}
			}

			return result;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for ParameterType::ePixelFormat.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::ePixelFormat.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::ePixelFormat >::type >
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
		static inline bool parse( String & p_params, ValueType & p_value )
		{
			bool result = false;
			StringArray values = string::split( p_params, cuT( " \t,;" ), 1, false );
			p_params.clear();

			if ( values.size() )
			{
				p_value = PF::getFormatByName( values[0] );
				result = p_value != PixelFormat::eCount;

				if ( values.size() > 1 )
				{
					p_params = values[1];
				}
			}

			return result;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for ParameterType::eSize.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::eSize.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::eSize >::type >
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
		static inline bool parse( String & p_params, ValueType & p_value )
		{
			bool result = false;

			if ( p_params == cuT( "screen_size" ) )
			{
				result = castor::System::getScreenSize( 0, p_value );
			}
			else
			{
				result = parseValues( p_params, p_value );
			}

			return result;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for ParameterType::eColour.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::eColour.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::eColour >::type >
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
		static inline bool parse( String & p_params, ValueType & p_value )
		{
			bool result = false;
			StringArray values = string::split( p_params, cuT( " \t,;" ), 5, false );

			if ( values.size() >= size_t( Component::eCount ) )
			{
				Point4f value;
				result = parseValues( p_params, value );

				if ( result )
				{
					for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
					{
						p_value[Component( i )] = value[i];
					}
				}
			}
			else if ( values.size() == 3 )
			{
				Point3f value;
				result = parseValues( p_params, value );

				if ( result )
				{
					for ( uint8_t i = 0; i < 3; i++ )
					{
						p_value[Component( i )] = value[i];
					}

					p_value[Component::eAlpha] = 1.0;
				}
			}
			else
			{
				try
				{
					String regexString = RegexFormat< Colour >::Value;
					regexString += details::IGNORED_END;

					const Regex regex{ regexString };
					auto begin = std::begin( p_params );
					auto end = std::end( p_params );
					const SRegexIterator it( begin, end, regex );
					const SRegexIterator endit;
					result = it != endit && it->size() >= 1;

					if ( result )
					{
						uint32_t value{ 0u };

						for ( size_t i = 0; i < it->size() && value == 0u; ++i )
						{
							auto match = ( *it )[i];

							if ( match.matched )
							{
								String text = match;

								if ( text.size() == 6 )
								{
									text = "FF" + text;
								}

								std::basic_istringstream< xchar > stream{ text };
								stream >> std::hex >> value;
							}
						}

						p_value = Colour::fromARGB( value );
					}
					else
					{
						Logger::logWarning( StringStream() << cuT( "Couldn't parse from " ) << p_params );
					}
				}
				catch ( std::exception & p_exc )
				{
					Logger::logError( StringStream() << cuT( "Couldn't parse from " ) << p_params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
				}

				return result;
			}

			return result;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ValueParser specialisation for ParameterType::eColour.
	\~french
	\brief		Spécialisation de ValueParser pour ParameterType::eColour.
	*/
	template< ParameterType Type >
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::eHdrColour >::type >
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
		static inline bool parse( String & p_params, ValueType & p_value )
		{
			bool result = false;
			StringArray values = string::split( p_params, cuT( " \t,;" ), 5, false );

			if ( values.size() >= size_t( Component::eCount ) )
			{
				Point4f value;
				result = parseValues( p_params, value );

				if ( result )
				{
					for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
					{
						p_value[Component( i )] = value[i];
					}
				}
			}
			else if ( values.size() == 3 )
			{
				Point3f value;
				result = parseValues( p_params, value );

				if ( result )
				{
					for ( uint8_t i = 0; i < 3; i++ )
					{
						p_value[Component( i )] = value[i];
					}

					p_value[Component::eAlpha] = 1.0;
				}
			}
			else
			{
				try
				{
					String regexString = RegexFormat< HdrColour >::Value;
					regexString += details::IGNORED_END;

					const Regex regex{ regexString };
					auto begin = std::begin( p_params );
					auto end = std::end( p_params );
					const SRegexIterator it( begin, end, regex );
					const SRegexIterator endit;
					result = it != endit && it->size() >= 1;

					if ( result )
					{
						uint32_t value{ 0u };

						for ( size_t i = 0; i < it->size() && value == 0u; ++i )
						{
							auto match = ( *it )[i];

							if ( match.matched )
							{
								String text = match;

								if ( text.size() == 6 )
								{
									text = "FF" + text;
								}

								std::basic_istringstream< xchar > stream{ text };
								stream >> std::hex >> value;
							}
						}

						p_value = HdrColour::fromARGB( value );
					}
					else
					{
						Logger::logWarning( StringStream() << cuT( "Couldn't parse from " ) << p_params );
					}
				}
				catch ( std::exception & p_exc )
				{
					Logger::logError( StringStream() << cuT( "Couldn't parse from " ) << p_params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
				}

				return result;
			}

			return result;
		}
	};

	//*************************************************************************************************

	inline ParserParameter< ParameterType::eName >::ParserParameter()
		: ParserParameter< ParameterType::eText >()
	{
	}

	inline ParameterType ParserParameter< ParameterType::eName >::getType()
	{
		return ParserParameterHelper< ParameterType::eName >::ParamType;
	}

	inline ParameterType ParserParameter< ParameterType::eName >::getBaseType()
	{
		return ParserParameterHelper< ParameterType::eName >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ParameterType::eName >::getStrType()
	{
		return ParserParameterHelper< ParameterType::eName >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ParameterType::eName >::clone()
	{
		return std::make_shared< ParserParameter< ParameterType::eName > >( *this );
	}

	inline bool ParserParameter< ParameterType::eName >::parse( String & p_params )
	{
		Regex regex{ cuT( "[^\"]*\"([^\"]*)\"" ) + String{ details::IGNORED_END } };
		auto begin = std::begin( p_params );
		auto end = std::end( p_params );
		SRegexIterator it( begin, end, regex );
		SRegexIterator endit;

		if ( it != endit && it->size() > 2 )
		{
			m_value = ( *it )[1];
			p_params = ( *it )[2];
		}

		return !m_value.empty();
	}

	//*************************************************************************************************

	inline ParserParameter< ParameterType::eCheckedText >::ParserParameter( UIntStrMap const & p_values )
		: ParserParameter< ParameterType::eUInt32 >()
		, m_values( p_values )
	{
	}

	inline ParameterType ParserParameter< ParameterType::eCheckedText >::getType()
	{
		return ParserParameterHelper< ParameterType::eCheckedText >::ParamType;
	}

	inline ParameterType ParserParameter< ParameterType::eCheckedText >::getBaseType()
	{
		return ParserParameterHelper< ParameterType::eCheckedText >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ParameterType::eCheckedText >::getStrType()
	{
		return ParserParameterHelper< ParameterType::eCheckedText >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ParameterType::eCheckedText >::clone()
	{
		return std::make_shared< ParserParameter< ParameterType::eCheckedText > >( *this );
	}

	inline bool ParserParameter< ParameterType::eCheckedText >::parse( String & p_params )
	{
		bool result = false;
		StringArray values = string::split( p_params, cuT( " \t,;" ), 1, false );
		p_params.clear();

		if ( !values.empty() )
		{
			auto it = m_values.find( values[0] );

			if ( it != m_values.end() )
			{
				m_value = it->second;
				result = true;
			}

			if ( values.size() > 1 )
			{
				p_params = values[1];
			}
		}

		return result;
	}

	//*************************************************************************************************

	inline ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::ParserParameter( UIntStrMap const & p_values )
		: ParserParameter< ParameterType::eUInt32 >()
		, m_values( p_values )
	{
	}

	inline ParameterType ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::getType()
	{
		return ParserParameterHelper< ParameterType::eBitwiseOred32BitsCheckedText >::ParamType;
	}

	inline ParameterType ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::getBaseType()
	{
		return ParserParameterHelper< ParameterType::eBitwiseOred32BitsCheckedText >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::getStrType()
	{
		return ParserParameterHelper< ParameterType::eBitwiseOred32BitsCheckedText >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::clone()
	{
		return std::make_shared< ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText > >( *this );
	}

	inline bool ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::parse( String & p_params )
	{
		bool result = false;
		m_value = 0;
		StringArray parameters = string::split( p_params, cuT( " \t,;" ), 1, false );
		p_params.clear();

		if ( !parameters.empty() )
		{
			StringArray values = string::split( parameters[0]
				, cuT( "|" )
				, uint32_t( std::count( parameters[0].begin(), parameters[0].end(), cuT( '|' ) ) + 1 )
				, false );

			for ( auto value : values )
			{
				auto it = m_values.find( value );

				if ( it != m_values.end() )
				{
					m_value |= it->second;
					result = true;
				}
			}

			if ( parameters.size() > 1 )
			{
				p_params = parameters[1];
			}
		}

		return result;
	}

	//*************************************************************************************************

	inline ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >::ParserParameter( UInt64StrMap const & p_values )
		: ParserParameter< ParameterType::eUInt64 >()
		, m_values( p_values )
	{
	}

	inline ParameterType ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >::getType()
	{
		return ParserParameterHelper< ParameterType::eBitwiseOred64BitsCheckedText >::ParamType;
	}

	inline ParameterType ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >::getBaseType()
	{
		return ParserParameterHelper< ParameterType::eBitwiseOred64BitsCheckedText >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >::getStrType()
	{
		return ParserParameterHelper< ParameterType::eBitwiseOred64BitsCheckedText >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >::clone()
	{
		return std::make_shared< ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText > >( *this );
	}

	inline bool ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >::parse( String & p_params )
	{
		bool result = false;
		m_value = 0;
		StringArray parameters = string::split( p_params, cuT( " \t,;" ), 1, false );
		p_params.clear();

		if ( !parameters.empty() )
		{
			StringArray values = string::split( parameters[0]
				, cuT( "|" )
				, uint32_t( std::count( parameters[0].begin(), parameters[0].end(), cuT( '|' ) ) + 1 )
				, false );

			for ( auto value : values )
			{
				auto it = m_values.find( value );

				if ( it != m_values.end() )
				{
					m_value |= it->second;
					result = true;
				}
			}

			if ( parameters.size() > 1 )
			{
				p_params = parameters[1];
			}
		}

		return result;
	}

	//*************************************************************************************************

	template< typename T >
	T const & ParserParameterBase::get( T & p_value )
	{
		static const ParameterType given = ParserValueTyper< T >::Type;
		static const ParameterType expected = getBaseType();

		if ( given == expected )
		{
			p_value = static_cast< ParserParameter< given >* >( this )->m_value;
		}
		else
		{
			throw ParserParameterTypeException< given >( expected );
		}

		return p_value;
	}

	//*************************************************************************************************
}
