#include "CastorUtils/Miscellaneous/Utils.hpp"
#include "CastorUtils/Log/Logger.hpp"
#include "CastorUtils/Graphics/PixelFormat.hpp"
#include "CastorUtils/FileParser/FileParserContext.hpp"
#include "CastorUtils/FileParser/ParserParameterTypeException.hpp"
#include "CastorUtils/Design/ArrayView.hpp"

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
	inline bool parseValues( LoggerInstance & logger
		, String & params
		, size_t count
		, T * value )
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
					String text = ( *it )[i];

					if ( text.find( "0x" ) == String::npos
						&& text.find( "0X" ) == String::npos )
					{
						auto stream = makeStringStream();
						stream << ( *it )[i];
						stream >> value[i - 1];
					}
					else
					{
						auto stream = makeStringStream();
						stream << std::hex << ( *it )[i];
						stream >> value[i - 1];
					}
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
				logger.logWarning( castor::makeStringStream() << cuT( "Couldn't parse from " ) << params );
			}
		}
		catch ( std::exception & p_exc )
		{
			logger.logError( castor::makeStringStream() << cuT( "Couldn't parse from " ) << params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
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
	inline bool parseValues( LoggerInstance & logger
		, String & params
		, size_t count
		, T * values
		, Range< T > const & range )
	{
		bool result = parseValues( logger
			, params
			, count
			, values );

		if ( result )
		{
			for ( auto & value : makeArrayView( values, values + count ) )
			{
				if ( value < range.getMin()
					|| value > range.getMax() )
				{
					logger.logWarning( castor::makeStringStream() << cuT( "Value " ) << value << cuT( " is outside of range [" ) << range.getMin() << cuT( ", " ) << range.getMax() << cuT( "]" ) );
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
	inline bool parseValues( LoggerInstance & logger
		, String & params
		, Point< T, Count > & value )
	{
		return parseValues( logger
			, params
			, Count
			, value.ptr() );
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
	inline bool parseValues( LoggerInstance & logger
		, String & params
		, Coords< T, Count > & value )
	{
		return parseValues( logger
			, params
			, Count
			, value.ptr() );
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
	inline bool parseValues( LoggerInstance & logger
		, String & params
		, Size & value )
	{
		return parseValues( logger
			, params
			, Count
			, value.ptr() );
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
	inline bool parseValues( LoggerInstance & logger
		, String & params
		, Position & value )
	{
		return parseValues( logger
			, params
			, Count
			, value.ptr() );
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
	inline bool parseValues( LoggerInstance & logger
		, String & params
		, Rectangle & value )
	{
		return parseValues( logger
			, params
			, Count
			, value.ptr() );
	}

	//*************************************************************************************************

	/**
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
		static inline bool parse( LoggerInstance & logger
			, String & params
			, ValueType & value
			, Range< ValueType > const & range )
		{
			return parseValues( logger, params, 1, &value, range );
		}
	};
	/**
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
		static inline bool parse( LoggerInstance & logger
			, String & 
			params, ValueType & value )
		{
			return parseValues( logger, params, value );
		}
	};
	/**
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
		static inline bool parse( LoggerInstance & CU_UnusedParam( logger )
			, String & params
			, ValueType & value )
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
	/**
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
		static inline bool parse( LoggerInstance & CU_UnusedParam( logger )
			, String & params
			, ValueType & value )
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
	/**
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
		static inline bool parse( LoggerInstance & CU_UnusedParam( logger )
			, String & params
			, ValueType & value )
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
	/**
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
		static inline bool parse( LoggerInstance & CU_UnusedParam( logger )
			, String & params
			, ValueType & value )
		{
			bool result = false;
			StringArray values = string::split( params, cuT( " \t,;" ), 1, false );
			params.clear();

			if ( values.size() )
			{
				value = getFormatByName( values[0] );
				result = value != PixelFormat::eCount;

				if ( values.size() > 1 )
				{
					params = values[1];
				}
			}

			return result;
		}
	};
	/**
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
		static inline bool parse( LoggerInstance & logger
			, String & params
			, ValueType & value )
		{
			bool result = false;

			if ( params == cuT( "screen_size" ) )
			{
				result = castor::System::getScreenSize( 0, value );
			}
			else
			{
				result = parseValues( logger, params, value );
			}

			return result;
		}
	};
	/**
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
		static inline bool parse( LoggerInstance & logger
			, String & params
			, ValueType & value )
		{
			bool result = false;
			StringArray values = string::split( params, cuT( " \t,;" ), 5, false );

			if ( values.size() >= size_t( RgbComponent::eCount ) )
			{
				Point3f colour;
				result = parseValues( logger, params, colour );

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
						logger.logWarning( makeStringStream() << cuT( "Couldn't parse from " ) << params );
					}
				}
				catch ( std::exception & p_exc )
				{
					logger.logError( makeStringStream() << cuT( "Couldn't parse from " ) << params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
				}

				return result;
			}

			return result;
		}
	};
	/**
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
		static inline bool parse( LoggerInstance & logger
			, String & params
			, ValueType & value )
		{
			bool result = false;
			StringArray values = string::split( params, cuT( " \t,;" ), 5, false );

			if ( values.size() >= size_t( RgbaComponent::eCount ) )
			{
				Point4f colour;
				result = parseValues( logger, params, colour );

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
				result = parseValues( logger, params, colour );

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
						logger.logWarning( makeStringStream() << cuT( "Couldn't parse from " ) << params );
					}
				}
				catch ( std::exception & p_exc )
				{
					logger.logError( makeStringStream() << cuT( "Couldn't parse from " ) << params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
				}

				return result;
			}

			return result;
		}
	};
	/**
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
		static inline bool parse( LoggerInstance & logger
			, String & params
			, ValueType & value )
		{
			bool result = false;
			StringArray values = string::split( params, cuT( " \t,;" ), 5, false );

			if ( values.size() >= size_t( RgbComponent::eCount ) )
			{
				Point4f colour;
				result = parseValues( logger, params, colour );

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
						logger.logWarning( makeStringStream() << cuT( "Couldn't parse from " ) << params );
					}
				}
				catch ( std::exception & p_exc )
				{
					logger.logError( makeStringStream() << cuT( "Couldn't parse from " ) << params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
				}

				return result;
			}

			return result;
		}
	};
	/**
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
		static inline bool parse( LoggerInstance & logger
			, String & params
			, ValueType & value )
		{
			bool result = false;
			StringArray values = string::split( params, cuT( " \t,;" ), 5, false );

			if ( values.size() >= size_t( RgbaComponent::eCount ) )
			{
				Point4f colour;
				result = parseValues( logger, params, colour );

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
				result = parseValues( logger, params, colour );

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
						logger.logWarning( makeStringStream() << cuT( "Couldn't parse from " ) << params );
					}
				}
				catch ( std::exception & p_exc )
				{
					logger.logError( makeStringStream() << cuT( "Couldn't parse from " ) << params << cuT( ": " ) << string::stringCast< xchar >( p_exc.what() ) );
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

	inline ParameterType ParserParameter< ParameterType::eName >::getType()const
	{
		return ParserParameterHelper< ParameterType::eName >::ParamType;
	}

	inline ParameterType ParserParameter< ParameterType::eName >::getBaseType()const
	{
		return ParserParameterHelper< ParameterType::eName >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ParameterType::eName >::getStrType()const
	{
		return ParserParameterHelper< ParameterType::eName >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ParameterType::eName >::clone()const
	{
		return std::make_shared< ParserParameter< ParameterType::eName > >( *this );
	}

	inline bool ParserParameter< ParameterType::eName >::parse( LoggerInstance & CU_UnusedParam( logger )
		, String & params )
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

	inline ParserParameter< ParameterType::eCheckedText >::ParserParameter( UInt32StrMap const & p_values )
		: ParserParameter< ParameterType::eUInt32 >()
		, m_values( p_values )
	{
	}

	inline ParameterType ParserParameter< ParameterType::eCheckedText >::getType()const
	{
		return ParserParameterHelper< ParameterType::eCheckedText >::ParamType;
	}

	inline ParameterType ParserParameter< ParameterType::eCheckedText >::getBaseType()const
	{
		return ParserParameterHelper< ParameterType::eCheckedText >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ParameterType::eCheckedText >::getStrType()const
	{
		return ParserParameterHelper< ParameterType::eCheckedText >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ParameterType::eCheckedText >::clone()const
	{
		return std::make_shared< ParserParameter< ParameterType::eCheckedText > >( *this );
	}

	inline bool ParserParameter< ParameterType::eCheckedText >::parse( LoggerInstance & CU_UnusedParam( logger )
		, String & params )
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

	inline ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::ParserParameter( UInt32StrMap const & p_values )
		: ParserParameter< ParameterType::eUInt32 >()
		, m_values( p_values )
	{
	}

	inline ParameterType ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::getType()const
	{
		return ParserParameterHelper< ParameterType::eBitwiseOred32BitsCheckedText >::ParamType;
	}

	inline ParameterType ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::getBaseType()const
	{
		return ParserParameterHelper< ParameterType::eBitwiseOred32BitsCheckedText >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::getStrType()const
	{
		return ParserParameterHelper< ParameterType::eBitwiseOred32BitsCheckedText >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::clone()const
	{
		return std::make_shared< ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText > >( *this );
	}

	inline bool ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >::parse( LoggerInstance & CU_UnusedParam( logger )
		, String & params )
	{
		static uint32_t constexpr Infinite = ~( 0u );
		bool result = false;
		m_value = 0u;
		StringArray parameters = string::split( params, cuT( "|" ), Infinite, false );
		params.clear();

		if ( !parameters.empty() )
		{
			bool nextParameter = false;
			String sep;

			for ( auto & value : parameters )
			{
				string::trim( value );

				if ( !nextParameter )
				{
					auto delim = value.find_first_of( cuT( " \t;," ) );

					if ( delim != String::npos )
					{
						nextParameter = true;
						params = value.substr( delim + 1u );
						string::trim( params );
						value = value.substr( 0, delim );
						sep = "|";
					}

					auto it = m_values.find( value );

					if ( it != m_values.end() )
					{
						m_value |= it->second;
						result = true;
					}
				}
				else
				{
					params += sep + value;
					sep = "|";
				}
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

	inline ParameterType ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >::getType()const
	{
		return ParserParameterHelper< ParameterType::eBitwiseOred64BitsCheckedText >::ParamType;
	}

	inline ParameterType ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >::getBaseType()const
	{
		return ParserParameterHelper< ParameterType::eBitwiseOred64BitsCheckedText >::ParameterBaseType;
	}

	inline xchar const * const ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >::getStrType()const
	{
		return ParserParameterHelper< ParameterType::eBitwiseOred64BitsCheckedText >::StringType;
	}

	inline ParserParameterBaseSPtr ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >::clone()const
	{
		return std::make_shared< ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText > >( *this );
	}

	inline bool ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >::parse( LoggerInstance & CU_UnusedParam( logger )
		, String & params )
	{
		static uint32_t constexpr Infinite = ~( 0u );
		bool result = false;
		m_value = 0ull;
		StringArray parameters = string::split( params, cuT( "|" ), Infinite, false );
		params.clear();

		if ( !parameters.empty() )
		{
			bool nextParameter = false;
			String sep;

			for ( auto & value : parameters )
			{
				string::trim( value );

				if ( !nextParameter )
				{
					auto delim = value.find_first_of( cuT( " \t;," ) );

					if ( delim != String::npos )
					{
						nextParameter = true;
						params = value.substr( delim + 1u );
						string::trim( params );
						value = value.substr( 0, delim );
						sep = "|";
					}

					auto it = m_values.find( value );

					if ( it != m_values.end() )
					{
						m_value |= it->second;
						result = true;
					}
				}
				else
				{
					params += sep + value;
					sep = "|";
				}
			}
		}

		return result;
	}

	//*************************************************************************************************
}
