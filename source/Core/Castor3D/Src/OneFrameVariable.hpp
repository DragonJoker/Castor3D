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
#ifndef ___C3D_ONE_FRAME_VARIABLE_H___
#define ___C3D_ONE_FRAME_VARIABLE_H___

#include "FrameVariable.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )
#pragma warning( disable:4290 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/08/2010
	\~english
	\brief		Helper structure containing definitions for a frame variable type.
	\~french
	\brief		Structure d'aide contenant des informations sur une variable de frame.
	*/
	template< typename T > struct OneFrameVariableDefinitions;
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/08/2010
	\~english
	\brief		Single shader variable with variable type
	\~french
	\brief		Variable simple à type variable
	*/
	template <typename T>
	class OneFrameVariable
		: public TFrameVariable<T>
	{
	protected:
		typedef Castor::Policy<T> policy;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		OneFrameVariable( ShaderProgramBase * p_pProgram );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_uiOcc		The array dimension
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_uiOcc		Les dimensions du tableau
		 */
		OneFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_object	L'objet à copier
		 */
		OneFrameVariable( OneFrameVariable< T > const & p_object );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_object	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 */
		OneFrameVariable( OneFrameVariable< T > && p_object );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_object	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_object	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		OneFrameVariable & operator=( OneFrameVariable< T > const & p_object );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_object	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		OneFrameVariable & operator=( OneFrameVariable< T > && p_object );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OneFrameVariable();
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A reference to the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Une référence sur la valeur
		 */
		inline T & GetValue()throw( std::out_of_range );
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A constant reference to the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Une référence constante sur la valeur
		 */
		inline T const & GetValue()const throw( std::out_of_range );
		/**
		 *\~english
		 *\brief		Retrieves the value at given index
		 *\remark		Check the index bounds
		 *\param[in]	p_uiIndex	The index
		 *\return		A reference to the value at given index
		 *\~french
		 *\brief		Récupère la valeur à l'index donné
		 *\remark		Vérifie que l'index est dans les bornes
		 *\param[in]	p_uiIndex	L'indice
		 *\return		Une référence sur la valeur à l'index donné
		 */
		inline T & GetValue( uint32_t p_uiIndex )throw( std::out_of_range );
		/**
		 *\~english
		 *\brief		Retrieves the value at given index
		 *\remark		Check the index bounds
		 *\param[in]	p_uiIndex	The index
		 *\return		A constant reference to the value at given index
		 *\~french
		 *\brief		Récupère la valeur à l'index donné
		 *\remark		Vérifie que l'index est dans les bornes
		 *\param[in]	p_uiIndex	L'indice
		 *\return		Une référence constante sur la valeur à l'index donné
		 */
		inline T const & GetValue( uint32_t p_uiIndex )const throw( std::out_of_range );
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_value	La valeur
		 */
		inline void SetValue( T const & p_value );
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_value	The new value
		 *\param[in]	p_uiIndex	The index of the value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_value	La valeur
		 *\param[in]	p_uiIndex	L'index de la valeur à modifier
		 */
		inline void SetValue( T const & p_value, uint32_t p_uiIndex );
		/**
		 *\~english
		 *\brief		Retrieves the byte size of the variable
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille en octets de la variable
		 *\return		La taille
		 */
		virtual uint32_t size()const;
		/**
		 *\~english
		 *\brief		Gives the variable full type
		 *\return		The type
		 *\~english
		 *\brief		Donne le type complet de la variable
		 *\return		Le type
		 */
		static inline eFRAME_VARIABLE_TYPE GetFrameVariableType();
		/**
		 *\~english
		 *\brief		Gives the variable full type
		 *\return		The type
		 *\~english
		 *\brief		Donne le type complet de la variable
		 *\return		Le type
		 */
		static inline Castor::String GetFrameVariableTypeName();
		/**
		 *\~english
		 *\brief		Array subscript operator
		 *\remark		Doesn't check the index bounds
		 *\param[in]	p_uiIndex	The index
		 *\return		A reference to the value at given index
		 *\~french
		 *\brief		Opérateur d'accès de type tableau
		 *\remark		Ne vérifie pas que l'index est dans les bornes
		 *\param[in]	p_uiIndex	L'indice
		 *\return		Une référence sur la valeur à l'index donné
		 */
		inline T & operator[]( uint32_t p_uiIndex )
		{
			return this->m_pValues[p_uiIndex];
		}
		/**
		 *\~english
		 *\brief		Array subscript operator
		 *\remark		Doesn't check the index bounds
		 *\param[in]	p_uiIndex	The index
		 *\return		A constant reference to the value at given index
		 *\~french
		 *\brief		Opérateur d'accès de type tableau
		 *\remark		Ne vérifie pas que l'index est dans les bornes
		 *\param[in]	p_uiIndex	L'indice
		 *\return		Une référence constante sur la valeur à l'index donné
		 */
		inline T const & operator[]( uint32_t p_uiIndex )const
		{
			return this->m_pValues[p_uiIndex];
		}
		/**
		 *\~english
		 *\brief		Gives the full type of the variable
		 *\return		The type of the variable
		 *\~french
		 *\brief		Donne le type complet de la variable
		 *\return		Le type complet
		 */
		static inline eVARIABLE_TYPE GetVariableType()
		{
			return eVARIABLE_TYPE_ONE;
		}
		/**
		 *\~english
		 *\brief		Retrieves the variable type
		 *\return		The variable type
		 *\~french
		 *\brief		Récupère le type de la variable
		 *\return		Le type de variable
		 */
		inline eVARIABLE_TYPE GetType()const
		{
			return OneFrameVariable< T >::GetVariableType();
		}
		/**
		 *\~english
		 *\brief		Retrieves the variable full type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type complet de la variable
		 *\return		Le type
		 */
		inline eFRAME_VARIABLE_TYPE GetFullType()const
		{
			return OneFrameVariable< T >::GetFrameVariableType();
		}
		/**
		 *\~english
		 *\brief		Gives the variable full type name
		 *\return		The type
		 *\~english
		 *\brief		Donne le nom du type complet de la variable
		 *\return		Le type
		 */
		inline Castor::String GetFullTypeName()const
		{
			return OneFrameVariable< T >::GetFrameVariableTypeName();
		}

	private:
		/**
		 *\~english
		 *\brief		Defines the value of the variable, from a string
		 *\param[in]	p_strValue	The string containing the value
		 *\param[in]	p_uiIndex	The index of the value
		 *\~french
		 *\brief		Définit la valeur de la variable à partir d'une chaîne
		 *\param[in]	p_strValue	La chaîne
		 *\param[in]	p_uiIndex	L'index de la valeur à modifier
		 */
		inline void DoSetValueStr( Castor::String const & p_strValue, uint32_t p_uiIndex );
	};
}

#include "OneFrameVariable.inl"

#pragma warning( pop )

#endif
