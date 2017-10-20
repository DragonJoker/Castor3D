/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PARSER_PARAMETER_H___
#define ___CASTOR_PARSER_PARAMETER_H___

#include "ParserParameterHelpers.hpp"

#include "Graphics/Colour.hpp"
#include "Data/Path.hpp"
#include "Math/Point.hpp"
#include "Math/Range.hpp"
#include "Graphics/Position.hpp"
#include "Graphics/Rectangle.hpp"
#include "Graphics/Size.hpp"

namespace castor
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		Specified parser parameter.
	\~french
	\brief		Parmètre de parseur spécifié.
	*/
	template< ParameterType Type, typename Enable = void >
	class ParserParameter;
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to parse a value.
	\~french
	\brief		Structure d'aide pour récupérer une valeur.
	*/
	template< ParameterType Type, typename Enable = void >
	struct ValueParser;
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		Specified parser parameter.
	\~french
	\brief		Parmètre de parseur spécifié.
	*/
	template< ParameterType Type >
	class ParserParameter < Type, typename std::enable_if < !HasBaseParameterType< Type >::value && !IsArithmeticType< Type >::value >::type >
		: public ParserParameterBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructor.
		 */
		inline ParserParameter()
		{
		}
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		inline ParameterType getType()
		{
			return ParserParameterHelper< Type >::ParamType;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		inline xchar const * const getStrType()
		{
			return ParserParameterHelper< Type >::StringType;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		inline ParserParameterBaseSPtr clone()
		{
			return std::make_shared< ParserParameter< Type > >( *this );
		}
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		inline bool parse( String & p_params )
		{
			return ValueParser< Type >::parse( p_params, m_value );
		}

	public:
		//!\~english	The parameter value type.
		//!\~french		Le type de valeur du paramètre.
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		//!\~english	The parameter value.
		//!\~french		La valeur du paramètre.
		ValueType m_value;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		Specified parser parameter.
	\~french
	\brief		Parmètre de parseur spécifié.
	*/
	template< ParameterType Type >
	class ParserParameter < Type, typename std::enable_if < IsArithmeticType< Type >::value >::type >
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
		inline ParserParameter()
			: m_range{ makeRange( std::numeric_limits< ValueType >::lowest(), std::numeric_limits< ValueType >::max() ) }
		{
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructor.
		 */
		inline ParserParameter( Range< ValueType > const & range )
			: m_range{ range }
		{
		}
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		inline ParameterType getType()
		{
			return ParserParameterHelper< Type >::ParamType;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		inline xchar const * const getStrType()
		{
			return ParserParameterHelper< Type >::StringType;
		}
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		inline ParserParameterBaseSPtr clone()
		{
			return std::make_shared< ParserParameter< Type > >( *this );
		}
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		inline bool parse( String & p_params )
		{
			return ValueParser< Type >::parse( p_params, m_value, m_range );
		}

	public:
		//!\~english	The parameter value.
		//!\~french		La valeur du paramètre.
		ValueType m_value;
		//!\~english	The parameter value range.
		//!\~french		L'intervalle de la valeur du paramètre.
		Range< ValueType > m_range;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
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
		inline ParserParameter();
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		inline ParameterType getType();
		/**
		 *\copydoc		castor::ParserParameterBase::getBaseType
		 */
		inline ParameterType getBaseType();
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		inline xchar const * const getStrType();
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		inline ParserParameterBaseSPtr clone();
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		inline bool parse( String & p_params );
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
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
		 *\param[in]	p_values	The values used to validate the parsed value.
		 *\~french
		 *\brief		Constructor.
		 *\param[in]	p_values	Les valeurs utilisées pour valider la valeur récupérée.
		 */
		inline ParserParameter( UIntStrMap const & p_values );
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		inline ParameterType getType();
		/**
		 *\copydoc		castor::ParserParameterBase::getBaseType
		 */
		inline ParameterType getBaseType();
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		inline xchar const * const getStrType();
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		inline ParserParameterBaseSPtr clone();
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		inline bool parse( String & p_params );

	public:
		UIntStrMap const & m_values;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
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
		 *\param[in]	p_values	The values used to validate the parsed value.
		 *\~french
		 *\brief		Constructor.
		 *\param[in]	p_values	Les valeurs utilisées pour valider la valeur récupérée.
		 */
		inline ParserParameter( UIntStrMap const & p_values );
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		inline ParameterType getType();
		/**
		 *\copydoc		castor::ParserParameterBase::getBaseType
		 */
		inline ParameterType getBaseType();
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		inline xchar const * const getStrType();
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		inline ParserParameterBaseSPtr clone();
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		inline bool parse( String & p_params );

	public:
		UIntStrMap const & m_values;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		10/12/2015
	\version	0.8.0
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
		 *\param[in]	p_values	The values used to validate the parsed value.
		 *\~french
		 *\brief		Constructor.
		 *\param[in]	p_values	Les valeurs utilisées pour valider la valeur récupérée.
		 */
		inline ParserParameter( UInt64StrMap const & p_values );
		/**
		 *\copydoc		castor::ParserParameterBase::getType
		 */
		inline ParameterType getType();
		/**
		 *\copydoc		castor::ParserParameterBase::getBaseType
		 */
		inline ParameterType getBaseType();
		/**
		 *\copydoc		castor::ParserParameterBase::getStrType
		 */
		inline xchar const * const getStrType();
		/**
		 *\copydoc		castor::ParserParameterBase::clone
		 */
		inline ParserParameterBaseSPtr clone();
		/**
		 *\copydoc		castor::ParserParameterBase::parse
		 */
		inline bool parse( String & p_params );

	public:
		UInt64StrMap const & m_values;
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
	 *\param[in]	p_values	The values used to validate the parsed value.
	 *\return		The created parameter.
	 *\~french
	 *\brief		Crée un paramètre du type donné.
	 *\param[in]	p_values	Les valeurs utilisées pour valider la valeur récupérée.
	 *\return		Le paramètre créé.
	 */
	template< ParameterType Type >
	ParserParameterBaseSPtr makeParameter( UIntStrMap const & p_values )
	{
		static_assert( Type == ParameterType::eCheckedText || Type == ParameterType::eBitwiseOred32BitsCheckedText, "Only for ParameterType::eCheckedText or ParameterType::eBitwiseOred32BitsCheckedText" );
		return std::make_shared< ParserParameter< Type > >( p_values );
	}
	/**
	 *\~english
	 *\brief		Creates a parameter of given type.
	 *\param[in]	p_values	The values used to validate the parsed value.
	 *\return		The created parameter.
	 *\~french
	 *\brief		Crée un paramètre du type donné.
	 *\param[in]	p_values	Les valeurs utilisées pour valider la valeur récupérée.
	 *\return		Le paramètre créé.
	 */
	template< ParameterType Type >
	ParserParameterBaseSPtr makeParameter( UInt64StrMap const & p_values )
	{
		static_assert( Type == ParameterType::eBitwiseOred64BitsCheckedText, "Only for ParameterType::eBitwiseOred64BitsCheckedText" );
		return std::make_shared< ParserParameter< Type > >( p_values );
	}
}

#include "ParserParameter.inl"

#endif
