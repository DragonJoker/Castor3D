/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CASTOR_PARSER_PARAMETER_H___
#define ___CASTOR_PARSER_PARAMETER_H___

#include "ParserParameterHelpers.hpp"

#include "Colour.hpp"
#include "Path.hpp"
#include "Point.hpp"
#include "Position.hpp"
#include "Rectangle.hpp"
#include "Size.hpp"

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
	template< ePARAMETER_TYPE Type, typename Enable = void >
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
	template< ePARAMETER_TYPE Type, typename Enable = void >
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
	template< ePARAMETER_TYPE Type >
	class ParserParameter< Type, typename std::enable_if< !has_base_parameter_type< Type >::value >::type >
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
		inline ePARAMETER_TYPE GetType()
		{
			return ParserParameterHelper< Type >::ParameterType;
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
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_NAME.
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_NAME.
	\remark
	*/
	template<>
	class ParserParameter< ePARAMETER_TYPE_NAME >
		: public ParserParameter< ePARAMETER_TYPE_TEXT >
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
		inline ePARAMETER_TYPE GetType();
		/**
		 *\copydoc		Castor::ParserParameterBase::GetBaseType
		 */
		inline ePARAMETER_TYPE GetBaseType();
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
		inline bool Parse( String & p_strParams );
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_CHECKED_TEXT.
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_CHECKED_TEXT.
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >
		: public ParserParameter< ePARAMETER_TYPE_UINT32 >
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
		inline ePARAMETER_TYPE GetType();
		/**
		 *\copydoc		Castor::ParserParameterBase::GetBaseType
		 */
		inline ePARAMETER_TYPE GetBaseType();
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
		inline bool Parse( String & p_strParams );

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
	template<> class ParserParameter< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT >
		: public ParserParameter< ePARAMETER_TYPE_UINT32 >
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
		inline ePARAMETER_TYPE GetType();
		/**
		 *\copydoc		Castor::ParserParameterBase::GetBaseType
		 */
		inline ePARAMETER_TYPE GetBaseType();
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
		inline bool Parse( String & p_strParams );

	public:
		UIntStrMap const & m_values;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		10/12/2015
	\version	0.8.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT.
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT.
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >
		: public ParserParameter< ePARAMETER_TYPE_UINT64 >
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
		inline ePARAMETER_TYPE GetType();
		/**
		 *\copydoc		Castor::ParserParameterBase::GetBaseType
		 */
		inline ePARAMETER_TYPE GetBaseType();
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
		inline bool Parse( String & p_strParams );

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
	template< ePARAMETER_TYPE Type >
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
	template< ePARAMETER_TYPE Type >
	ParserParameterBaseSPtr MakeParameter( UIntStrMap const & p_values )
	{
		static_assert( Type == ePARAMETER_TYPE_CHECKED_TEXT || Type == ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT, "Only for ePARAMETER_TYPE_CHECKED_TEXT or ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT" );
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
	template< ePARAMETER_TYPE Type >
	ParserParameterBaseSPtr MakeParameter( UInt64StrMap const & p_values )
	{
		static_assert( Type == ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT, "Only for ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT" );
		return std::make_shared< ParserParameter< Type > >( p_values );
	}
}

#include "ParserParameter.inl"

#endif
