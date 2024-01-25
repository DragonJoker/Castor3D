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
			, std::enable_if_t< !hasBaseParameterTypeV< Type > && !isArithmeticTypeV< Type > > >
		: public ParserParameterBase
	{
	public:
		//!\~english	The parameter value type.
		//!\~french		Le type de valeur du paramètre.
		using ValueType = ParserParameterValueType< Type >;

	public:
		ParserParameter() = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructor.
		 */
		explicit ParserParameter( ValueType rhs )
			: m_value{ castor::move( rhs ) }
			, m_set{ true }
		{
		}
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		ParameterType getType()const override
		{
			return ParserParameterParamType< Type >;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		StringView getStrType()const override
		{
			return ParserParameterStringType< Type >;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		ParserParameterBaseSPtr clone()const override
		{
			return castor::make_shared< ParserParameter< Type > >( *this );
		}
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		bool parse( LoggerInstance & logger
			, String & params )override
		{
			m_set = ValueParser< Type >::parse( logger, params, m_value );
			return m_set;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::isSet
		 */
		bool isSet()override
		{
			return m_set;
		}

	public:
		//!\~english	The parameter value.
		//!\~french		La valeur du paramètre.
		ValueType m_value{};
		//!\~english	The parameter value.
		//!\~french		La valeur du paramètre.
		bool m_set{};
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
		using ValueType = ParserParameterValueType< Type >;

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
		 *\~french
		 *\brief		Constructor.
		 */
		explicit ParserParameter( ValueType rhs )
			: m_value{ castor::move( rhs ) }
			, m_range{ makeRange( std::numeric_limits< ValueType >::lowest(), std::numeric_limits< ValueType >::max() ) }
			, m_set{ true }
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
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	rhs		The value.
		 *\param[in]	range	The range to parse.
		 *\~french
		 *\brief		Constructor.
		 *\param[in]	rhs		La valeur.
		 *\param[in]	range	L'intervalle à parser.
		 */
		explicit ParserParameter( ValueType rhs
			, Range< ValueType > const & range )
			: m_value{ castor::move( rhs ) }
			, m_range{ range }
			, m_set{ true }
		{
		}
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		ParameterType getType()const override
		{
			return ParserParameterParamType< Type >;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		StringView getStrType()const override
		{
			return ParserParameterStringType< Type >;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		ParserParameterBaseSPtr clone()const override
		{
			return castor::make_shared< ParserParameter< Type > >( *this );
		}
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		bool parse( LoggerInstance & logger
			, String & params )override
		{
			m_set = ValueParser< Type >::parse( logger, params, m_value, m_range );
			return m_set;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::isSet
		 */
		bool isSet()override
		{
			return m_set;
		}

	public:
		//!\~english	The parameter value.
		//!\~french		La valeur du paramètre.
		ValueType m_value{};
		//!\~english	The parameter value range.
		//!\~french		L'intervalle de la valeur du paramètre.
		Range< ValueType > m_range;
		//!\~english	The parameter value.
		//!\~french		La valeur du paramètre.
		bool m_set{};
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
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructor.
		 */
		explicit ParserParameter( String rhs )
			: ParserParameter< ParameterType::eText >{ castor::move( rhs ) }
		{
		}
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
		StringView getStrType()const override;
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
			, StringView name = ParserParameterStringType< ParameterType::eCheckedText > );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructor.
		 */
		explicit ParserParameter( uint32_t value
			, UInt32StrMap const & values
			, StringView name = ParserParameterStringType< ParameterType::eCheckedText > )
			: ParserParameter< ParameterType::eUInt32 >{ value }
			, m_name{ name }
			, m_values{ values }
		{
		}
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
		StringView getStrType()const override;
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
		StringView m_name;
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
			, StringView name = ParserParameterStringType< ParameterType::eBitwiseOred32BitsCheckedText > );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructor.
		 */
		explicit ParserParameter( uint32_t value
			, UInt32StrMap const & values
			, StringView name = ParserParameterStringType< ParameterType::eBitwiseOred32BitsCheckedText > )
			: ParserParameter< ParameterType::eUInt32 >{ value }
			, m_name{ name }
			, m_values{ values }
		{
		}
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
		StringView getStrType()const override;
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
		StringView m_name;
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
			, StringView name = ParserParameterStringType< ParameterType::eBitwiseOred64BitsCheckedText > );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructor.
		 */
		explicit ParserParameter( uint64_t value
			, UInt64StrMap const & values
			, StringView name = ParserParameterStringType< ParameterType::eBitwiseOred64BitsCheckedText > )
			: ParserParameter< ParameterType::eUInt64 >{ value }
			, m_name{ name }
			, m_values{ values }
		{
		}
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
		StringView getStrType()const override;
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
		StringView m_name;
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
		return castor::make_shared< ParserParameter< Type > >();
	}
	/**
	 *\~english
	 *\brief		Creates a parameter of given type.
	 *\return		The created parameter.
	 *\~french
	 *\brief		Crée un paramètre du type donné.
	 *\return		Le paramètre créé.
	 */
	template< ParameterType Type, typename ... Params >
	ParserParameterBaseSPtr makeDefaultedParameter( ParserParameterValueType< Type > defaultValue
		, Params && ... params )
	{
		return castor::make_shared< ParserParameter< Type > >( defaultValue
			, castor::forward< Params >( params )... );
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
		static_assert( ( Type == ParameterType::eInt8 && std::is_same_v< T, int8_t > )
				|| ( Type == ParameterType::eInt16 && std::is_same_v< T, int16_t > )
				|| ( Type == ParameterType::eInt32 && std::is_same_v< T, int32_t > )
				|| ( Type == ParameterType::eInt64 && std::is_same_v< T, int64_t > )
				|| ( Type == ParameterType::eUInt8 && std::is_same_v< T, uint8_t > )
				|| ( Type == ParameterType::eUInt16 && std::is_same_v< T, uint16_t > )
				|| ( Type == ParameterType::eUInt32 && std::is_same_v< T, uint32_t > )
				|| ( Type == ParameterType::eUInt64 && std::is_same_v< T, uint64_t > )
				|| ( Type == ParameterType::eFloat && std::is_same_v< T, float > )
				|| ( Type == ParameterType::eDouble && std::is_same_v< T, double > )
				|| ( Type == ParameterType::eLongDouble && std::is_same_v< T, long double > )
			, "C type and ParameterType must match." );

		return castor::make_shared< ParserParameter< Type > >( range );
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
	ParserParameterBaseSPtr makeParameter( StringView name
		, UInt32StrMap const & values )
	{
		static_assert( Type == ParameterType::eCheckedText || Type == ParameterType::eBitwiseOred32BitsCheckedText
			, "Only for ParameterType::eCheckedText or ParameterType::eBitwiseOred32BitsCheckedText" );
		return castor::make_shared< ParserParameter< Type > >( values, name );
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
	ParserParameterBaseSPtr makeParameter( StringView name
		, UInt64StrMap const & values )
	{
		static_assert( Type == ParameterType::eBitwiseOred64BitsCheckedText
			, "Only for ParameterType::eBitwiseOred64BitsCheckedText" );
		return castor::make_shared< ParserParameter< Type > >( values, name );
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
		return castor::make_shared< ParserParameter< Type > >( ParserEnumTraits< EnumType >::Values
			, ParserEnumTraits< EnumType >::Name );
	}
}

#include "ParserParameter.inl"

#endif
