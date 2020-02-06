﻿/*
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
#ifndef ___C3D_POINT_FRAME_VARIABLE_H___
#define ___C3D_POINT_FRAME_VARIABLE_H___

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
	\brief		Point shader variable with variable type and dimensions
	\~french
	\brief		Variable point à type et dimensions variables
	*/
	template <typename T, uint32_t Count>
	class PointFrameVariable : public TFrameVariable<T>
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
		PointFrameVariable( ShaderProgramBase * p_pProgram );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_uiOcc		The array dimension
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_uiOcc		Les dimensions du tableau
		 */
		PointFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_object	L'objet à copier
		 */
		PointFrameVariable( PointFrameVariable< T, Count > const & p_object );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_object	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 */
		PointFrameVariable( PointFrameVariable< T, Count > && p_object );
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
		PointFrameVariable & operator =( PointFrameVariable< T, Count > const & p_object );
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
		PointFrameVariable & operator =( PointFrameVariable< T, Count > && p_object );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PointFrameVariable();
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
		inline Castor::Coords<T, Count > operator []( uint32_t p_uiIndex )
		{
			return Castor::Coords< T, Count >( &this->m_pValues[p_uiIndex * Count] );
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
		inline Castor::Point< T, Count > operator []( uint32_t p_uiIndex )const
		{
			return Castor::Point< T, Count >( &this->m_pValues[p_uiIndex * Count] );
		}
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A Coords containing the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Un Coords contenant la valeur
		 */
		inline Castor::Coords< T, Count > GetValue()throw( std::out_of_range );
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A Point containing the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Un Point contenant la valeur
		 */
		inline Castor::Point< T, Count > GetValue()const throw( std::out_of_range );
		/**
		 *\~english
		 *\brief		Retrieves the value at given index
		 *\remark		Check the index bounds
		 *\param[in]	p_uiIndex	The index
		 *\return		A Coords containing the value at given index
		 *\~french
		 *\brief		Récupère la valeur à l'index donné
		 *\remark		Vérifie que l'index est dans les bornes
		 *\param[in]	p_uiIndex	L'indice
		 *\return		Un Coords contenant la valeur à l'index donné
		 */
		inline Castor::Coords< T, Count > GetValue( uint32_t p_uiIndex )throw( std::out_of_range );
		/**
		 *\~english
		 *\brief		Retrieves the value at given index
		 *\remark		Check the index bounds
		 *\param[in]	p_uiIndex	The index
		 *\return		A Point containing the value at given index
		 *\~french
		 *\brief		Récupère la valeur à l'index donné
		 *\remark		Vérifie que l'index est dans les bornes
		 *\param[in]	p_uiIndex	L'indice
		 *\return		Un Point contenant la valeur à l'index donné
		 */
		inline Castor::Point< T, Count > GetValue( uint32_t p_uiIndex )const throw( std::out_of_range );
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
			return PointFrameVariable< T, Count >::GetVariableType();
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
			return PointFrameVariable< T, Count >::GetFrameVariableType();
		}
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_ptValue	The new value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_ptValue	La valeur
		 */
		inline void SetValue( Castor::Point< T, Count > const & p_ptValue );
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_ptValue	The new value
		 *\param[in]	p_uiIndex	The index of the value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_ptValue	La valeur
		 *\param[in]	p_uiIndex	L'index de la valeur à modifier
		 */
		inline void SetValue( Castor::Point< T, Count > const & p_ptValue, uint32_t p_uiIndex );
		/**
		 *\~english
		 *\brief		Gives the full type of the variable
		 *\return		The type of the variable
		 *\~french
		 *\brief		Donne le type complet de la variable
		 *\return		Le type complet
		 */
		static inline eVARIABLE_TYPE GetVariableType();
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
		 *\brief		Retrieves the byte size of the variable
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille en octets de la variable
		 *\return		La taille
		 */
		virtual uint32_t size()const;

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

#include "PointFrameVariable.inl"

#pragma warning( pop )

#endif
