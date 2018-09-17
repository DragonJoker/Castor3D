#include "Miscellaneous/Utils.hpp"
#include "Log/Logger.hpp"
#include "Graphics/PixelFormat.hpp"
#include "FileParserContext.hpp"
#include "ParserParameterTypeException.hpp"
#include "Design/ArrayView.hpp"

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
	 *\param[in]	params	The line containing the vector.
	 *\param[out]	value		Receives the result.
	 *\param[in]	count		The elements count.
	 *\return		\p true if OK.
	 *\~french
	 *\brief		Extrait un vecteur à partir d'une ligne.
	 *\param[in]	params	La ligne contenant le vecteur.
	 *\param[out]	value		Reçoit le résultat.
	 *\param[in]	count		Le nombre d'éléments.
	 *\return		\p true si tout s'est bien passé.
	 */
	template< typename T >
	inline bool parseValues( String & params, size_t count, T * value )
	{
		bool result = false;

		try
		{
			String regexString = RegexFormat< T >::Value;

			for ( size_t i = 1; i < count; ++i )
			{
				regexString += String( details::VALUE_SEPARATOR ) + RegexFormat< T >::Value;
			}

			regexString += details::IGNORED_END;

			const Regex regex{ regexString };
			auto begin = std::begin( params );
			auto end = std::end( params );
			const RegexIterator it( begin, end, regex );
			const RegexIterator endit;
			result = it != endit && it->size() >= count;

			if ( result )
			{
				for ( size_t i = 1; i <= count; ++i )
				{
					InputStringStream stream( ( *it )[i] );
					stream >> value[i - 1];
				}

				if ( it->size() > count )
				{
					String strParams;

					for ( size_t i = count + 1; i < it->size(); ++i )
					{
						if ( ( *it )[i].matched )
						{
							strParams += ( *it )[i];
						}
					}

					params = strParams;
				}
			}
			else
			{
				Logger::logWarning( castor::makeStringStream() << cuT( "Couldn't parse from " ) << params );
			}
		}
		catch ( std::exception & p_exc )
		{
			Logger::logError( castor::makeStringStream() << cuT( "Couldn't parse from " ) << params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
		}

		return result;
	}
	/**
	 *\~english
	 *\brief		Parses a vector from a line.
	 *\param[in]	params	The line containing the vector.
	 *\param[out]	value		Receives the result.
	 *\param[in]	count		The elements count.
	 *\return		\p true if OK.
	 *\~french
	 *\brief		Extrait un vecteur à partir d'une ligne.
	 *\param[in]	params	La ligne contenant le vecteur.
	 *\param[out]	value		Reçoit le résultat.
	 *\param[in]	count		Le nombre d'éléments.
	 *\return		\p true si tout s'est bien passé.
	 */
	template< typename T >
	inline bool parseValues( String & params
		, size_t count
		, T * values
		, Range< T > const & range )
	{
		bool result = parseValues( params, count, values );

		if ( result )
		{
			for ( auto & value : makeArrayView( values, values + count ) )
			{
				if ( value < range.getMin()
					|| value > range.getMax() )
				{
					Logger::logWarning( castor::makeStringStream() << cuT( "Value " ) << value << cuT( " is outside of range [" ) << range.getMin() << cuT( ", " ) << range.getMax() << cuT( "]" ) );
					value = range.clamp( value );
				}
			}
		}

		return result;
	}
	/**
	 *\~english
	 *\brief		Parses a vector from a line.
	 *\param[in]	params	The line containing the vector.
	 *\param[out]	value		Receives the result.
	 *\return		\p true if OK.
	 *\~french
	 *\brief		Extrait un vecteur à partir d'une ligne.
	 *\param[in]	params	La ligne contenant le vecteur.
	 *\param[out]	value		Reçoit le résultat.
	 *\return		\p true si tout s'est bien passé.
	 */
	template< typename T, uint32_t Count >
	inline bool parseValues( String & params, Point< T, Count > & value )
	{
		return parseValues( params, Count, value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Parses a vector from a line.
	 *\param[in]	params	The line containing the vector.
	 *\param[out]	value		Receives the result.
	 *\return		\p true if OK.
	 *\~french
	 *\brief		Extrait un vecteur à partir d'une ligne.
	 *\param[in]	params	La ligne contenant le vecteur.
	 *\param[out]	value		Reçoit le résultat.
	 *\return		\p true si tout s'est bien passé.
	 */
	template< typename T, uint32_t Count >
	inline bool parseValues( String & params, Coords< T, Count > & value )
	{
		return parseValues( params, Count, value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Parses a vector from a line.
	 *\param[in]	params	The line containing the vector.
	 *\param[out]	value		Receives the result.
	 *\return		\p true if OK.
	 *\~french
	 *\brief		Extrait un vecteur à partir d'une ligne.
	 *\param[in]	params	La ligne contenant le vecteur.
	 *\param[out]	value		Reçoit le résultat.
	 *\return		\p true si tout s'est bien passé.
	 */
	template< typename T, uint32_t Count >
	inline bool parseValues( String & params, Size & value )
	{
		return parseValues( params, Count, value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Parses a vector from a line.
	 *\param[in]	params	The line containing the vector.
	 *\param[out]	value		Receives the result.
	 *\return		\p true if OK.
	 *\~french
	 *\brief		Extrait un vecteur à partir d'une ligne.
	 *\param[in]	params	La ligne contenant le vecteur.
	 *\param[out]	value		Reçoit le résultat.
	 *\return		\p true si tout s'est bien passé.
	 */
	template< typename T, uint32_t Count >
	inline bool parseValues( String & params, Position & value )
	{
		return parseValues( params, Count, value.ptr() );
	}
	/**
	 *\~english
	 *\brief		Parses a vector from a line.
	 *\param[in]	params	The line containing the vector.
	 *\param[out]	value		Receives the result.
	 *\return		\p true if OK.
	 *\~french
	 *\brief		Extrait un vecteur à partir d'une ligne.
	 *\param[in]	params	La ligne contenant le vecteur.
	 *\param[out]	value		Reçoit le résultat.
	 *\return		\p true si tout s'est bien passé.
	 */
	template< typename T, uint32_t Count >
	inline bool parseValues( String & params, Rectangle & value )
	{
		return parseValues( params, Count, value.ptr() );
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
		 *\param[in]	params	The line containing the value.
		 *\param[out]	value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	params	La ligne contenant la valeur.
		 *\param[out]	value		Reçoit le résultat.
		 */
		static inline bool parse( String & params
			, ValueType & value
			, Range< ValueType > const & range )
		{
			return parseValues( params, 1, &value, range );
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
		 *\param[in]	params	The line containing the value.
		 *\param[out]	value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	params	La ligne contenant la valeur.
		 *\param[out]	value		Reçoit le résultat.
		 */
		static inline bool parse( String & params, ValueType & value )
		{
			return parseValues( params, value );
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
		 *\param[in]	params	The line containing the value.
		 *\param[out]	value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	params	La ligne contenant la valeur.
		 *\param[out]	value		Reçoit le résultat.
		 */
		static inline bool parse( String & params, ValueType & value )
		{
			value = params;
			params.clear();

			if ( !value.empty() )
			{
				if ( value[0] == cuT( '\"' ) )
				{
					value = value.substr( 1 );

					if ( !value.empty() )
					{
						if ( value[value.size() - 1] == cuT( '\"' ) )
						{
							value = value.substr( 0, value.size() - 1 );
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
		 *\param[in]	params	The line containing the value.
		 *\param[out]	value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	params	La ligne contenant la valeur.
		 *\param[out]	value		Reçoit le résultat.
		 */
		static inline bool parse( String & params, ValueType & value )
		{
			value = Path{ params };
			params.clear();

			if ( !value.empty() )
			{
				if ( value[0] == cuT( '\"' ) )
				{
					value = Path{ value.substr( 1 ) };

					if ( !value.empty() )
					{
						std::size_t index = value.find( cuT( '\"' ) );

						if ( index != String::npos )
						{
							if ( index != value.size() - 1 )
							{
								params = value.substr( index + 1 );
								string::trim( params );
							}

							value = Path{ value.substr( 0, index ) };
						}
					}
				}
			}

			return !value.empty();
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
		 *\param[in]	params	The line containing the value.
		 *\param[out]	value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	params	La ligne contenant la valeur.
		 *\param[out]	value		Reçoit le résultat.
		 */
		static inline bool parse( String & params, ValueType & value )
		{
			bool result = false;
			StringArray values = string::split( params, cuT( " \t,;" ), 1, false );
			params.clear();

			if ( !values.empty() )
			{
				value = string::toLowerCase( values[0] ) == cuT( "true" );
				result = values[0] == cuT( "true" ) || values[0] == cuT( "false" );

				if ( values.size() > 1 )
				{
					params = values[1];
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
		 *\param[in]	params	The line containing the value.
		 *\param[out]	value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	params	La ligne contenant la valeur.
		 *\param[out]	value		Reçoit le résultat.
		 */
		static inline bool parse( String & params, ValueType & value )
		{
			bool result = false;
			StringArray values = string::split( params, cuT( " \t,;" ), 1, false );
			params.clear();

			if ( values.size() )
			{
				value = PF::getFormatByName( values[0] );
				result = value != PixelFormat::eCount;

				if ( values.size() > 1 )
				{
					params = values[1];
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
		 *\param[in]	params	The line containing the value.
		 *\param[out]	value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	params	La ligne contenant la valeur.
		 *\param[out]	value		Reçoit le résultat.
		 */
		static inline bool parse( String & params, ValueType & value )
		{
			bool result = false;

			if ( params == cuT( "screen_size" ) )
			{
				result = castor::System::getScreenSize( 0, value );
			}
			else
			{
				result = parseValues( params, value );
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
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::eRgbColour >::type >
	{
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		/**
		 *\~english
		 *\brief		Parses a value from a line.
		 *\param[in]	params	The line containing the value.
		 *\param[out]	value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	params	La ligne contenant la valeur.
		 *\param[out]	value		Reçoit le résultat.
		 */
		static inline bool parse( String & params, ValueType & value )
		{
			bool result = false;
			StringArray values = string::split( params, cuT( " \t,;" ), 5, false );

			if ( values.size() >= size_t( RgbComponent::eCount ) )
			{
				Point3f colour;
				result = parseValues( params, colour );

				if ( result )
				{
					for ( uint8_t i = 0; i < size_t( RgbComponent::eCount ); i++ )
					{
						value[RgbComponent( i )] = colour[i];
					}
				}
			}
			else
			{
				try
				{
					String regexString = RegexFormat< RgbColour >::Value;
					regexString += details::IGNORED_END;

					const Regex regex{ regexString };
					auto begin = std::begin( params );
					auto end = std::end( params );
					const RegexIterator it( begin, end, regex );
					const RegexIterator endit;
					result = it != endit && it->size() >= 1;

					if ( result )
					{
						uint32_t colour{ 0u };

						for ( size_t i = 0; i < it->size() && colour == 0u; ++i )
						{
							auto match = ( *it )[i];

							if ( match.matched )
							{
								String text = match;

								if ( text.size() == 6 )
								{
									text = "FF" + text;
								}

								InputStringStream stream{ text };
								stream >> std::hex >> colour;
							}
						}

						value = RgbColour::fromARGB( colour );
					}
					else
					{
						Logger::logWarning( castor::makeStringStream() << cuT( "Couldn't parse from " ) << params );
					}
				}
				catch ( std::exception & p_exc )
				{
					Logger::logError( castor::makeStringStream() << cuT( "Couldn't parse from " ) << params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
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
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::eRgbaColour >::type >
	{
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		/**
		 *\~english
		 *\brief		Parses a value from a line.
		 *\param[in]	params	The line containing the value.
		 *\param[out]	value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	params	La ligne contenant la valeur.
		 *\param[out]	value		Reçoit le résultat.
		 */
		static inline bool parse( String & params, ValueType & value )
		{
			bool result = false;
			StringArray values = string::split( params, cuT( " \t,;" ), 5, false );

			if ( values.size() >= size_t( RgbaComponent::eCount ) )
			{
				Point4f colour;
				result = parseValues( params, colour );

				if ( result )
				{
					for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
					{
						value[RgbaComponent( i )] = colour[i];
					}
				}
			}
			else if ( values.size() == size_t( RgbComponent::eCount ) )
			{
				Point3f colour;
				result = parseValues( params, colour );

				if ( result )
				{
					for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
					{
						value[RgbaComponent( i )] = colour[i];
					}

					value[RgbaComponent::eAlpha] = 1.0;
				}
			}
			else
			{
				try
				{
					String regexString = RegexFormat< RgbaColour >::Value;
					regexString += details::IGNORED_END;

					const Regex regex{ regexString };
					auto begin = std::begin( params );
					auto end = std::end( params );
					const RegexIterator it( begin, end, regex );
					const RegexIterator endit;
					result = it != endit && it->size() >= 1;

					if ( result )
					{
						uint32_t colour{ 0u };

						for ( size_t i = 0; i < it->size() && colour == 0u; ++i )
						{
							auto match = ( *it )[i];

							if ( match.matched )
							{
								String text = match;

								if ( text.size() == 6 )
								{
									text = "FF" + text;
								}

								InputStringStream stream{ text };
								stream >> std::hex >> colour;
							}
						}

						value = RgbaColour::fromARGB( colour );
					}
					else
					{
						Logger::logWarning( castor::makeStringStream() << cuT( "Couldn't parse from " ) << params );
					}
				}
				catch ( std::exception & p_exc )
				{
					Logger::logError( castor::makeStringStream() << cuT( "Couldn't parse from " ) << params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
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
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::eHdrRgbColour >::type >
	{
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		/**
		 *\~english
		 *\brief		Parses a value from a line.
		 *\param[in]	params	The line containing the value.
		 *\param[out]	value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	params	La ligne contenant la valeur.
		 *\param[out]	value		Reçoit le résultat.
		 */
		static inline bool parse( String & params, ValueType & value )
		{
			bool result = false;
			StringArray values = string::split( params, cuT( " \t,;" ), 5, false );

			if ( values.size() >= size_t( RgbComponent::eCount ) )
			{
				Point4f colour;
				result = parseValues( params, colour );

				if ( result )
				{
					for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
					{
						value[RgbComponent( i )] = colour[i];
					}
				}
			}
			else
			{
				try
				{
					String regexString = RegexFormat< HdrRgbColour >::Value;
					regexString += details::IGNORED_END;

					const Regex regex{ regexString };
					auto begin = std::begin( params );
					auto end = std::end( params );
					const RegexIterator it( begin, end, regex );
					const RegexIterator endit;
					result = it != endit && it->size() >= 1;

					if ( result )
					{
						uint32_t colour{ 0u };

						for ( size_t i = 0; i < it->size() && colour == 0u; ++i )
						{
							auto match = ( *it )[i];

							if ( match.matched )
							{
								String text = match;

								if ( text.size() == 6 )
								{
									text = "FF" + text;
								}

								InputStringStream stream{ text };
								stream >> std::hex >> colour;
							}
						}

						value = HdrRgbColour::fromARGB( colour );
					}
					else
					{
						Logger::logWarning( castor::makeStringStream() << cuT( "Couldn't parse from " ) << params );
					}
				}
				catch ( std::exception & p_exc )
				{
					Logger::logError( castor::makeStringStream() << cuT( "Couldn't parse from " ) << params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
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
	struct ValueParser< Type, typename std::enable_if< Type == ParameterType::eHdrRgbaColour >::type >
	{
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		/**
		 *\~english
		 *\brief		Parses a value from a line.
		 *\param[in]	params	The line containing the value.
		 *\param[out]	value		Receives the result.
		 *\~french
		 *\brief		Extrait une valeur à partir d'une ligne.
		 *\param[in]	params	La ligne contenant la valeur.
		 *\param[out]	value		Reçoit le résultat.
		 */
		static inline bool parse( String & params, ValueType & value )
		{
			bool result = false;
			StringArray values = string::split( params, cuT( " \t,;" ), 5, false );

			if ( values.size() >= size_t( RgbaComponent::eCount ) )
			{
				Point4f colour;
				result = parseValues( params, colour );

				if ( result )
				{
					for ( uint8_t i = 0; i < uint8_t( RgbaComponent::eCount ); i++ )
					{
						value[RgbaComponent( i )] = colour[i];
					}
				}
			}
			else if ( values.size() == size_t( RgbComponent::eCount ) )
			{
				Point3f colour;
				result = parseValues( params, colour );

				if ( result )
				{
					for ( uint8_t i = 0; i < uint8_t( RgbComponent::eCount ); i++ )
					{
						value[RgbaComponent( i )] = colour[i];
					}

					value[RgbaComponent::eAlpha] = 1.0;
				}
			}
			else
			{
				try
				{
					String regexString = RegexFormat< HdrRgbaColour >::Value;
					regexString += details::IGNORED_END;

					const Regex regex{ regexString };
					auto begin = std::begin( params );
					auto end = std::end( params );
					const RegexIterator it( begin, end, regex );
					const RegexIterator endit;
					result = it != endit && it->size() >= 1;

					if ( result )
					{
						uint32_t colour{ 0u };

						for ( size_t i = 0; i < it->size() && colour == 0u; ++i )
						{
							auto match = ( *it )[i];

							if ( match.matched )
							{
								String text = match;

								if ( text.size() == 6 )
								{
									text = "FF" + text;
								}

								InputStringStream stream{ text };
								stream >> std::hex >> colour;
							}
						}

						value = HdrRgbaColour::fromARGB( colour );
					}
					else
					{
						Logger::logWarning( castor::makeStringStream() << cuT( "Couldn't parse from " ) << params );
					}
				}
				catch ( std::exception & p_exc )
				{
					Logger::logError( castor::makeStringStream() << cuT( "Couldn't parse from " ) << params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
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

	inline bool ParserParameter< ParameterType::eName >::parse( String & params )
	{
		Regex regex{ cuT( "[^\"]*\"([^\"]*)\"" ) + String{ details::IGNORED_END } };
		auto begin = std::begin( params );
		auto end = std::end( params );
		RegexIterator it( begin, end, regex );
		RegexIterator endit;

		if ( it != endit && it->size() > 2 )
		{
			m_value = ( *it )[1];
			params = ( *it )[2];
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

	inline bool ParserParameter< ParameterType::eCheckedText >::parse( String & params )
	{
		bool result = false;
		StringArray values = string::split( params, cuT( " \t,;" ), 1, false );
		params.clear();

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
				params = values[1];
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

	inline bool ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::parse( String & params )
	{
		bool result = false;
		m_value = 0;
		StringArray parameters = string::split( params, cuT( " \t,;" ), 1, false );
		params.clear();

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
				params = parameters[1];
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

	inline bool ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >::parse( String & params )
	{
		bool result = false;
		m_value = 0;
		StringArray parameters = string::split( params, cuT( " \t,;" ), 1, false );
		params.clear();

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
				params = parameters[1];
			}
		}

		return result;
	}

	//*************************************************************************************************

	template< typename T >
	T const & ParserParameterBase::get( T & value )
	{
		static const ParameterType given = ParserValueTyper< T >::Type;
		static const ParameterType expected = getBaseType();

		if ( given == expected )
		{
			value = static_cast< ParserParameter< given >* >( this )->m_value;
		}
		else
		{
			throw ParserParameterTypeException< given >( expected );
		}

		return value;
	}

	//*************************************************************************************************
}
