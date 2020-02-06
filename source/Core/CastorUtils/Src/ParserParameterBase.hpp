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
#ifndef ___CASTOR_PARSER_PARAMETER_BASE_H___
#define ___CASTOR_PARSER_PARAMETER_BASE_H___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		04/02/2013
	\~english
	\brief		Parser function parameter types enumeration
	\~french
	\brief		Enum�ration des types de param�tres pour une fonction d'analyse
	*/
	typedef enum ePARAMETER_TYPE
	CASTOR_TYPE( uint8_t )
	{
		ePARAMETER_TYPE_TEXT,
		ePARAMETER_TYPE_NAME,
		ePARAMETER_TYPE_PATH,
		ePARAMETER_TYPE_CHECKED_TEXT,
		ePARAMETER_TYPE_BOOL,
		ePARAMETER_TYPE_INT8,
		ePARAMETER_TYPE_INT16,
		ePARAMETER_TYPE_INT32,
		ePARAMETER_TYPE_INT64,
		ePARAMETER_TYPE_UINT8,
		ePARAMETER_TYPE_UINT16,
		ePARAMETER_TYPE_UINT32,
		ePARAMETER_TYPE_UINT64,
		ePARAMETER_TYPE_FLOAT,
		ePARAMETER_TYPE_DOUBLE,
		ePARAMETER_TYPE_LONGDOUBLE,
		ePARAMETER_TYPE_PIXELFORMAT,
		ePARAMETER_TYPE_POINT2I,
		ePARAMETER_TYPE_POINT3I,
		ePARAMETER_TYPE_POINT4I,
		ePARAMETER_TYPE_POINT2F,
		ePARAMETER_TYPE_POINT3F,
		ePARAMETER_TYPE_POINT4F,
		ePARAMETER_TYPE_POINT2D,
		ePARAMETER_TYPE_POINT3D,
		ePARAMETER_TYPE_POINT4D,
		ePARAMETER_TYPE_SIZE,
		ePARAMETER_TYPE_POSITION,
		ePARAMETER_TYPE_RECTANGLE,
		ePARAMETER_TYPE_COLOUR,
		ePARAMETER_TYPE_COUNT,
	}	ePARAMETER_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		Template structure holding parameter specific data
	\~french
	\brief		Structure template contenant les donn�es sp�cifiques du param�tre
	*/
	class ParserParameterBase
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves the parameter type
		 *\return		The type
		 *\~french
		 *\brief		R�cup�re le type du param�tre
		 *\return		Le type
		 */
		virtual ePARAMETER_TYPE GetType() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the parameter base type (like ePARAMETER_TYPE_TEXT for ePARAMETER_TYPE_NAME)
		 *\return		The type
		 *\~french
		 *\brief		R�cup�re le type de base du param�tre (comme ePARAMETER_TYPE_TEXT pour ePARAMETER_TYPE_NAME)
		 *\return		Le type
		 */
		virtual ePARAMETER_TYPE GetBaseType()
		{
			return GetType();
		}
		/**
		 *\~english
		 *\brief		Retrieves the parameter string type
		 *\return		The type
		 *\~french
		 *\brief		R�cup�re le type du cha�ne param�tre
		 *\return		Le type
		 */
		virtual xchar const * GetStrType() = 0;
		/**
		 *\~english
		 *\brief			Checks the parameter
		 *\param[in,out]	p_strParams	The text containing the parameter value
		 *\return			\p false if any error occured
		 *\~french
		 *\brief			V�rifie le param�tre
		 *\param[in,out]	p_strParams	Le texte contenant la valeur du param�tre
		 *\return			\p si un probl�me quelconque est arriv�
		 */
		virtual bool Parse( String & p_strParams ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the parameter value
		 *\param[out]	p_value	Receives the value
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re la valeur du param�tre
		 *\param[out]	p_value	Re�oit la valeur
		 *\return		La valeur
		 */
		template< typename T > T const & Get( T & p_value );
	};
}

#endif
