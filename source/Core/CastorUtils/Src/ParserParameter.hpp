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

#include "ParserParameterBase.hpp"

namespace Castor
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		Specified parser parameter
	\~french
	\brief		Parm�tre de parseur sp�cifi�
	\remark
	*/
	template< ePARAMETER_TYPE Type >
	class ParserParameter
		: public ParserParameterBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves the parameter type
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le type du param�tre
		 *\return		La valeur
		 */
		virtual ePARAMETER_TYPE GetType()
		{
			return ePARAMETER_TYPE_COUNT;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parameter type
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le type du param�tre
		 *\return		La valeur
		 */
		virtual xchar const * GetStrType()
		{
			return cuT( "unknown parameter type" );
		}
		/**
		 *\~english
		 *\brief			Defines the parameter value from a String
		 *\param[in,out]	p_strParam	The String containing the value
		 *\~french
		 *\brief			D�finit la valeur du param�tre, � partir d'un String
		 *\param[in,out]	p_strParam	Le String contenant la valeur
		 */
		virtual bool Parse( String & p_strParam );
	};
}

#include "ParserParameter.inl"

#endif
