/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PARSER_PARAMETER_H___
#define ___CASTOR_PARSER_PARAMETER_H___

#include "CastorUtils/FileParser/ParserParameterHelpers.hpp"

#include "CastorUtils/Graphics/Colour.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Math/Point.hpp"
#include "CastorUtils/Math/Range.hpp"
#include "CastorUtils/Graphics/Position.hpp"
#include "CastorUtils/Graphics/Rectangle.hpp"
#include "CastorUtils/Graphics/Size.hpp"

namespace castor
{
	/**
	\~english
	\brief		Specified parser parameter.
	\~french
	\brief		Parmètre de parseur spécifié.
	*/
	template< ParameterType Type >
	class ParserParameter < Type
			, std::enable_if_t< !HasBaseParameterType< Type >::value && !IsArithmeticType< Type >::value > >
		: public ParserParameterBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructor.
		 */
		ParserParameter()
		{
		}
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		ParameterType getType()const override
		{
			return ParserParameterHelper< Type >::ParamType;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		xchar const * getStrType()const override
		{
			return ParserParameterHelper< Type >::StringType;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		ParserParameterBaseSPtr clone()const override
		{
			return std::make_shared< ParserParameter< Type > >( *this );
		}
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		bool parse( LoggerInstance & logger
			, String & params )override
		{
			return ValueParser< Type >::parse( logger, params, m_value );
		}

	public:
		//!\~english	The parameter value type.
		//!\~french		Le type de valeur du paramètre.
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		//!\~english	The parameter value.
		//!\~french		La valeur du paramètre.
		ValueType m_value{};
	};
	/**
	\~english
	\brief		Specified parser parameter.
	\~french
	\brief		Parmètre de parseur spécifié.
	*/
	template< ParameterType Type >
	class ParserParameter < Type
			, std::enable_if_t< IsArithmeticType< Type >::value > >
		: public ParserParameterBase
	{
	public:
		//!\~english	The parameter value type.
		//!\~french		Le type de valeur du paramètre.
		using ValueType = typename ParserParameterHelper< Type >::ValueType;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructor.
		 */
		ParserParameter()
			: m_range{ makeRange( std::numeric_limits< ValueType >::lowest(), std::numeric_limits< ValueType >::max() ) }
		{
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	range	The range to parse.
		 *\~french
		 *\brief		Constructor.
		 *\param[in]	range	L'intervalle à parser.
		 */
		explicit ParserParameter( Range< ValueType > const & range )
			: m_range{ range }
		{
		}
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		ParameterType getType()const override
		{
			return ParserParameterHelper< Type >::ParamType;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		xchar const * getStrType()const override
		{
			return ParserParameterHelper< Type >::StringType;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		ParserParameterBaseSPtr clone()const override
		{
			return std::make_shared< ParserParameter< Type > >( *this );
		}
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		bool parse( LoggerInstance & logger
			, String & params )override
		{
			return ValueParser< Type >::parse( logger, params, m_value, m_range );
		}

	public:
		//!\~english	The parameter value.
		//!\~french		La valeur du paramètre.
		ValueType m_value{};
		//!\~english	The parameter value range.
		//!\~french		L'intervalle de la valeur du paramètre.
		Range< ValueType > m_range;
	};
	/**
	\~english
	\brief		ParserParameter specialisation for ParameterType::eName.
	\~french
	\brief		Specialisation de ParserParameter pour ParameterType::eName.
	\remark
	*/
	template<>
	class ParserParameter< ParameterType::eName >
		: public ParserParameter< ParameterType::eText >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructor.
		 */
		ParserParameter();
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		ParameterType getType()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::getBaseType
		 */
		ParameterType getBaseType()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		xchar const * getStrType()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		ParserParameterBaseSPtr clone()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		bool parse( LoggerInstance & logger
			, String & params )override;
	};
	/**
	\~english
	\brief		ParserParameter specialisation for ParameterType::eCheckedText.
	\~french
	\brief		Specialisation de ParserParameter pour ParameterType::eCheckedText.
	\remark
	*/
	template<> class ParserParameter< ParameterType::eCheckedText >
		: public ParserParameter< ParameterType::eUInt32 >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	values	The values used to validate the parsed value.
		 *\param[in]	name	The parameter type name.
		 *\~french
		 *\brief		Constructor.
		 *\param[in]	values	Les valeurs utilisées pour valider la valeur récupérée.
		 *\param[in]	name	Le nom du type de paramètre.
		 */
		explicit ParserParameter( UInt32StrMap const & values
			, xchar const * name = ParserParameterHelper< ParameterType::eCheckedText >::StringType );
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		ParameterType getType()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::getBaseType
		 */
		ParameterType getBaseType()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		xchar const * getStrType()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		ParserParameterBaseSPtr clone()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		bool parse( LoggerInstance & logger
			, String & params )override;

	public:
		xchar const * m_name;
		UInt32StrMap const m_values;
	};
	/**
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT.
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT.
	\remark
	*/
	template<> class ParserParameter< ParameterType::eBitwiseOred32BitsCheckedText >
		: public ParserParameter< ParameterType::eUInt32 >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	values	The values used to validate the parsed value.
		 *\param[in]	name	The parameter type name.
		 *\~french
		 *\brief		Constructor.
		 *\param[in]	values	Les valeurs utilisées pour valider la valeur récupérée.
		 *\param[in]	name	Le nom du type de paramètre.
		 */
		explicit ParserParameter( UInt32StrMap const & values
			, xchar const * name = ParserParameterHelper< ParameterType::eBitwiseOred32BitsCheckedText >::StringType );
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		ParameterType getType()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::getBaseType
		 */
		ParameterType getBaseType()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		xchar const * getStrType()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		ParserParameterBaseSPtr clone()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		bool parse( LoggerInstance & logger
			, String & params )override;

	public:
		xchar const * m_name;
		UInt32StrMap const m_values;
	};
	/**
	\~english
	\brief		ParserParameter specialisation for ParameterType::eBitwiseOred64BitsCheckedText.
	\~french
	\brief		Specialisation de ParserParameter pour ParameterType::eBitwiseOred64BitsCheckedText.
	\remark
	*/
	template<> class ParserParameter< ParameterType::eBitwiseOred64BitsCheckedText >
		: public ParserParameter< ParameterType::eUInt64 >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	values	The values used to validate the parsed value.
		 *\param[in]	name	The parameter type name.
		 *\~french
		 *\brief		Constructor.
		 *\param[in]	values	Les valeurs utilisées pour valider la valeur récupérée.
		 *\param[in]	name	Le nom du type de paramètre.
		 */
		explicit ParserParameter( UInt64StrMap const & values
			, xchar const * name = ParserParameterHelper< ParameterType::eBitwiseOred64BitsCheckedText >::StringType );
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		ParameterType getType()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::getBaseType
		 */
		ParameterType getBaseType()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		xchar const * getStrType()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		ParserParameterBaseSPtr clone()const override;
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		bool parse( LoggerInstance & logger
			, String & params )override;

	public:
		xchar const * m_name;
		UInt64StrMap const m_values;
	};
	/**
	 *\~english
	 *\brief		Creates a parameter of given type.
	 *\return		The created parameter.
	 *\~french
	 *\brief		Crée un paramètre du type donné.
	 *\return		Le paramètre créé.
	 */
	template< ParameterType Type >
	ParserParameterBaseSPtr makeParameter()
	{
		return std::make_shared< ParserParameter< Type > >();
	}
	/**
	 *\~english
	 *\brief		Creates a parameter of given type.
	 *\return		The created parameter.
	 *\~french
	 *\brief		Crée un paramètre du type donné.
	 *\return		Le paramètre créé.
	 */
	template< ParameterType Type, typename T >
	ParserParameterBaseSPtr makeParameter( Range< T > const & range )
	{
		static_assert( Type >= ParameterType::eInt8 && Type <= ParameterType::eLongDouble
			, "Only for arithmetic types" );
		static_assert( ( Type == ParameterType::eInt8 && std::is_same< T, int8_t >::value )
				|| ( Type == ParameterType::eInt16 && std::is_same< T, int16_t >::value )
				|| ( Type == ParameterType::eInt32 && std::is_same< T, int32_t >::value )
				|| ( Type == ParameterType::eInt64 && std::is_same< T, int64_t >::value )
				|| ( Type == ParameterType::eUInt8 && std::is_same< T, uint8_t >::value )
				|| ( Type == ParameterType::eUInt16 && std::is_same< T, uint16_t >::value )
				|| ( Type == ParameterType::eUInt32 && std::is_same< T, uint32_t >::value )
				|| ( Type == ParameterType::eUInt64 && std::is_same< T, uint64_t >::value )
				|| ( Type == ParameterType::eFloat && std::is_same< T, float >::value )
				|| ( Type == ParameterType::eDouble && std::is_same< T, double >::value )
				|| ( Type == ParameterType::eLongDouble && std::is_same< T, long double >::value )
			, "C type and ParameterType must match." );

		return std::make_shared< ParserParameter< Type > >( range );
	}
	/**
	 *\~english
	 *\brief		Creates a parameter of given type.
	 *\param[in]	name	The parameter type name.
	 *\param[in]	values	The values used to validate the parsed value.
	 *\return		The created parameter.
	 *\~french
	 *\brief		Crée un paramètre du type donné.
	 *\param[in]	name	Le nom du type de paramètre.
	 *\param[in]	values	Les valeurs utilisées pour valider la valeur récupérée.
	 *\return		Le paramètre créé.
	 */
	template< ParameterType Type >
	ParserParameterBaseSPtr makeParameter( xchar const * name
		, UInt32StrMap const & values )
	{
		static_assert( Type == ParameterType::eCheckedText || Type == ParameterType::eBitwiseOred32BitsCheckedText, "Only for ParameterType::eCheckedText or ParameterType::eBitwiseOred32BitsCheckedText" );
		return std::make_shared< ParserParameter< Type > >( values, name );
	}
	/**
	 *\~english
	 *\brief		Creates a parameter of given type.
	 *\param[in]	name	The parameter type name.
	 *\param[in]	values	The values used to validate the parsed value.
	 *\return		The created parameter.
	 *\~french
	 *\brief		Crée un paramètre du type donné.
	 *\param[in]	name	Le nom du type de paramètre.
	 *\param[in]	values	Les valeurs utilisées pour valider la valeur récupérée.
	 *\return		Le paramètre créé.
	 */
	template< ParameterType Type >
	ParserParameterBaseSPtr makeParameter( xchar const * name
		, UInt64StrMap const & values )
	{
		static_assert( Type == ParameterType::eBitwiseOred64BitsCheckedText, "Only for ParameterType::eBitwiseOred64BitsCheckedText" );
		return std::make_shared< ParserParameter< Type > >( values, name );
	}
	/**
	 *\~english
	 *\brief		Creates a parameter of given type.
	 *\return		The created parameter.
	 *\~french
	 *\brief		Crée un paramètre du type donné.
	 *\return		Le paramètre créé.
	 */
	template< ParameterType Type, typename EnumType >
	ParserParameterBaseSPtr makeParameter()
	{
		static_assert( Type == ParameterType::eCheckedText || Type == ParameterType::eBitwiseOred32BitsCheckedText || Type == ParameterType::eBitwiseOred64BitsCheckedText
			, "Only for ParameterType::eCheckedText or ParameterType::eBitwiseOred32BitsCheckedText or ParameterType::eBitwiseOred64BitsCheckedText" );
		return std::make_shared< ParserParameter< Type > >( ParserEnumTraits< EnumType >::Values
			, ParserEnumTraits< EnumType >::Name );
	}
}

#include "ParserParameter.inl"

#endif
