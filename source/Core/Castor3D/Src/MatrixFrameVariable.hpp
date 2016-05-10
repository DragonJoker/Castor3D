/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_MATRIX_FRAME_VARIABLE_H___
#define ___C3D_MATRIX_FRAME_VARIABLE_H___

#include "FrameVariable.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/08/2010
	\~english
	\brief		Helper structure containing definitions for a matrix frame variable type.
	\~french
	\brief		Structure d'aide contenant des informations sur une variable de frame de type matrice.
	*/
	template< typename T, uint32_t Rows, uint32_t Columns > struct MtxFrameVariableDefinitions;
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/08/2010
	\~english
	\brief		Matrix shader variable with variable type and dimensions
	\~french
	\brief		Variable matrice à type et dimensions variables
	*/
	template< typename T, uint32_t Rows, uint32_t Columns >
	class MatrixFrameVariable
		: public TFrameVariable< T >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_program	The program
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_program	Le programme
		 */
		MatrixFrameVariable( ShaderProgram * p_program );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_program		The program
		 *\param[in]	p_occurences	The array dimension
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_program		Le programme
		 *\param[in]	p_occurences	Les dimensions du tableau
		 */
		MatrixFrameVariable( ShaderProgram * p_program, uint32_t p_occurences );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_object	L'objet à copier
		 */
		MatrixFrameVariable( MatrixFrameVariable< T, Rows, Columns > const & p_object );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_object	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 */
		MatrixFrameVariable( MatrixFrameVariable< T, Rows, Columns > && p_object );
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
		MatrixFrameVariable & operator=( MatrixFrameVariable< T, Rows, Columns > const & p_object );
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
		MatrixFrameVariable & operator=( MatrixFrameVariable< T, Rows, Columns > && p_object );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~MatrixFrameVariable();
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A reference to the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Une référence sur la valeur
		 */
		inline Castor::Matrix< T, Rows, Columns > & GetValue();
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A constant reference to the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Une référence constante sur la valeur
		 */
		inline Castor::Matrix< T, Rows, Columns > const & GetValue()const;
		/**
		 *\~english
		 *\brief		Retrieves the value at given index
		 *\remarks		Check the index bounds
		 *\param[in]	p_index	The index
		 *\return		A reference to the value at given index
		 *\~french
		 *\brief		Récupère la valeur à l'index donné
		 *\remarks		Vérifie que l'index est dans les bornes
		 *\param[in]	p_index	L'indice
		 *\return		Une référence sur la valeur à l'index donné
		 */
		inline Castor::Matrix< T, Rows, Columns > & GetValue( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Retrieves the value at given index
		 *\remarks		Check the index bounds
		 *\param[in]	p_index	The index
		 *\return		A constant reference to the value at given index
		 *\~french
		 *\brief		Récupère la valeur à l'index donné
		 *\remarks		Vérifie que l'index est dans les bornes
		 *\param[in]	p_index	L'indice
		 *\return		Une référence constante sur la valeur à l'index donné
		 */
		inline Castor::Matrix< T, Rows, Columns > const & GetValue( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_value	La valeur
		 */
		inline void SetValue( Castor::Matrix< T, Rows, Columns > const & p_value );
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_value	The new value
		 *\param[in]	p_index	The index of the value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_value	La valeur
		 *\param[in]	p_index	L'index de la valeur à modifier
		 */
		inline void SetValue( Castor::Matrix< T, Rows, Columns > const & p_value, uint32_t p_index );
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
		 *\brief		Defines the buffer holding the frame variable.
		 *\remarks		The variable no longer owns it's buffer
		 *\param[in]	p_buffer	The buffer
		 *\~french
		 *\brief		Définit le buffer de cette variable
		 *\remarks		La variable perd la responsabilité de son buffer
		 *\param[in]	p_buffer	Le buffer
		 */
		virtual void link( uint8_t * p_buffer );
		/**
		 *\~english
		 *\brief		Gives the full type of the variable
		 *\return		The type of the variable
		 *\~french
		 *\brief		Donne le type complet de la variable
		 *\return		Le type complet
		 */
		static eVARIABLE_TYPE GetVariableType();
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
		 *\brief		Gives the variable full type name
		 *\return		The name
		 *\~english
		 *\brief		Donne le nom du type complet de la variable
		 *\return		Le nom
		 */
		static inline Castor::String GetFrameVariableTypeName();
		/**
		 *\~english
		 *\brief		Array subscript operator
		 *\remarks		Doesn't check the index bounds
		 *\param[in]	p_index	The index
		 *\return		A reference to the value at given index
		 *\~french
		 *\brief		Opérateur d'accès de type tableau
		 *\remarks		Ne vérifie pas que l'index est dans les bornes
		 *\param[in]	p_index	L'indice
		 *\return		Une référence sur la valeur à l'index donné
		 */
		inline Castor::Matrix< T, Rows, Columns > & operator[]( uint32_t p_index )
		{
			return m_mtxValue[p_index];
		}
		/**
		 *\~english
		 *\brief		Array subscript operator
		 *\remarks		Doesn't check the index bounds
		 *\param[in]	p_index	The index
		 *\return		A constant reference to the value at given index
		 *\~french
		 *\brief		Opérateur d'accès de type tableau
		 *\remarks		Ne vérifie pas que l'index est dans les bornes
		 *\param[in]	p_index	L'indice
		 *\return		Une référence constante sur la valeur à l'index donné
		 */
		inline Castor::Matrix< T, Rows, Columns > const & operator[]( uint32_t p_index )const
		{
			return m_mtxValue[p_index];
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
			return MatrixFrameVariable< T, Rows, Columns >::GetVariableType();
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
			return MatrixFrameVariable< T, Rows, Columns >::GetFrameVariableType();
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
			return MatrixFrameVariable< T, Rows, Columns >::GetFrameVariableTypeName();
		}

	private:
		/**
		 *\~english
		 *\brief		Defines the value of the variable, from a string
		 *\param[in]	p_value	The string containing the value
		 *\param[in]	p_index	The index of the value
		 *\~french
		 *\brief		Définit la valeur de la variable à partir d'une chaîne
		 *\param[in]	p_value	La chaîne
		 *\param[in]	p_index	L'index de la valeur à modifier
		 */
		inline void DoSetValueStr( Castor::String const & p_value, uint32_t p_index );

	protected:
		typedef Castor::Policy<T> policy;
		//!\~english The matrix values	\~french Les valeurs matrices
		Castor::Matrix< T, Rows, Columns > * m_mtxValue;
	};
}

#include "MatrixFrameVariable.inl"

#endif
