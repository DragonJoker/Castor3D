/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CASTOR_PARSER_PARAMETER_H___
#define ___CASTOR_PARSER_PARAMETER_H___

#include "ParserParameterHelpers.hpp"

#include "Graphics/Colour.hpp"
#include "Data/Path.hpp"
#include "Math/Point.hpp"
#include "Graphics/Position.hpp"
#include "Graphics/Rectangle.hpp"
#include "Graphics/Size.hpp"

namespace Castor
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
	class ParserParameter < Type, typename std::enable_if < !has_base_parameter_type< Type >::value >::type >
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
		 *\copydoc		Castor::ParserParameterBase::GetType
		 */
		inline ParameterType GetType()
		{
			return ParserParameterHelper< Type >::ParamType;
		}
		/**
		 *\copydoc		Castor::ParserParameterBase::GetStrType
		 */
		inline xchar const * const GetStrType()
		{
			return ParserParameterHelper< Type >::StringType;
		}
		/**
		 *\copydoc		Castor::ParserParameterBase::Clone
		 */
		inline ParserParameterBaseSPtr Clone()
		{
			return std::make_shared< ParserParameter< Type > >( *this );
		}
		/**
		 *\copydoc		Castor::ParserParameterBase::Parse
		 */
		inline bool Parse( String & p_params )
		{
			return ValueParser< Type >::Parse( p_params, m_value );
		}

	public:
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = typename ParserParameterHelper< Type >::ValueType;
		//!~english The parameter value.	\~french La valeur du paramètre.
		ValueType m_value;
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
		 *\copydoc		Castor::ParserParameterBase::GetType
		 */
		inline ParameterType GetType();
		/**
		 *\copydoc		Castor::ParserParameterBase::GetBaseType
		 */
		inline ParameterType GetBaseType();
		/**
		 *\copydoc		Castor::ParserParameterBase::GetStrType
		 */
		inline xchar const * const GetStrType();
		/**
		 *\copydoc		Castor::ParserParameterBase::Clone
		 */
		inline ParserParameterBaseSPtr Clone();
		/**
		 *\copydoc		Castor::ParserParameterBase::Parse
		 */
		inline bool Parse( String & p_params );
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
		 *\copydoc		Castor::ParserParameterBase::GetType
		 */
		inline ParameterType GetType();
		/**
		 *\copydoc		Castor::ParserParameterBase::GetBaseType
		 */
		inline ParameterType GetBaseType();
		/**
		 *\copydoc		Castor::ParserParameterBase::GetStrType
		 */
		inline xchar const * const GetStrType();
		/**
		 *\copydoc		Castor::ParserParameterBase::Clone
		 */
		inline ParserParameterBaseSPtr Clone();
		/**
		 *\copydoc		Castor::ParserParameterBase::Parse
		 */
		inline bool Parse( String & p_params );

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
		 *\copydoc		Castor::ParserParameterBase::GetType
		 */
		inline ParameterType GetType();
		/**
		 *\copydoc		Castor::ParserParameterBase::GetBaseType
		 */
		inline ParameterType GetBaseType();
		/**
		 *\copydoc		Castor::ParserParameterBase::GetStrType
		 */
		inline xchar const * const GetStrType();
		/**
		 *\copydoc		Castor::ParserParameterBase::Clone
		 */
		inline ParserParameterBaseSPtr Clone();
		/**
		 *\copydoc		Castor::ParserParameterBase::Parse
		 */
		inline bool Parse( String & p_params );

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
		 *\copydoc		Castor::ParserParameterBase::GetType
		 */
		inline ParameterType GetType();
		/**
		 *\copydoc		Castor::ParserParameterBase::GetBaseType
		 */
		inline ParameterType GetBaseType();
		/**
		 *\copydoc		Castor::ParserParameterBase::GetStrType
		 */
		inline xchar const * const GetStrType();
		/**
		 *\copydoc		Castor::ParserParameterBase::Clone
		 */
		inline ParserParameterBaseSPtr Clone();
		/**
		 *\copydoc		Castor::ParserParameterBase::Parse
		 */
		inline bool Parse( String & p_params );

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
	ParserParameterBaseSPtr MakeParameter()
	{
		return std::make_shared< ParserParameter< Type > >();
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
	ParserParameterBaseSPtr MakeParameter( UIntStrMap const & p_values )
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
	ParserParameterBaseSPtr MakeParameter( UInt64StrMap const & p_values )
	{
		static_assert( Type == ParameterType::eBitwiseOred64BitsCheckedText, "Only for ParameterType::eBitwiseOred64BitsCheckedText" );
		return std::make_shared< ParserParameter< Type > >( p_values );
	}
}

#include "ParserParameter.inl"

#endif
