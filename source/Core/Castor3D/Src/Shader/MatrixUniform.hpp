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
#ifndef ___C3D_MATRIX_FRAME_VARIABLE_H___
#define ___C3D_MATRIX_FRAME_VARIABLE_H___

#include "Uniform.hpp"

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
	template< typename T, uint32_t Rows, uint32_t Columns > struct MtxUniformDefinitions;
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/08/2010
	\~english
	\brief		Uniform shader variable with variable type and dimensions
	\~french
	\brief		Variable matrice à type et dimensions variables
	*/
	template< typename T, uint32_t Rows, uint32_t Columns >
	class MatrixUniform
		: public TUniform< T >
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
		explicit MatrixUniform( ShaderProgram & p_program );
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
		MatrixUniform( ShaderProgram & p_program, uint32_t p_occurences );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~MatrixUniform();
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
		 *\copydoc		Castor3D::::size
		 */
		uint32_t size()const override;
		/**
		 *\copydoc		Castor3D::Uniform::link
		 */
		void link( uint8_t * p_buffer, uint32_t p_stride )override;
		/**
		 *\~english
		 *\return		The type of the variable.
		 *\~french
		 *\return		Le type de la variable.
		 */
		static inline constexpr VariableType GetVariableType();
		/**
		 *\~english
		 *\return		The variable's full type.
		 *\~english
		 *\return		Le type complet de la variable.
		 */
		static inline constexpr UniformType GetUniformType();
		/**
		 *\~english
		 *\return		The variable's full type name.
		 *\~english
		 *\return		Le nom du type complet de la variable.
		 */
		static inline Castor::String const & GetUniformTypeName();
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
			return m_mtxValues[p_index];
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
			return m_mtxValues[p_index];
		}
		/**
		 *\copydoc		Castor3D::::GetType
		 */
		inline VariableType GetType()const override
		{
			return MatrixUniform< T, Rows, Columns >::GetVariableType();
		}
		/**
		 *\copydoc		Castor3D::::GetFullType
		 */
		inline UniformType GetFullType()const override
		{
			return MatrixUniform< T, Rows, Columns >::GetUniformType();
		}
		/**
		 *\copydoc		Castor3D::Uniform::GetFullTypeName
		 */
		inline Castor::String const & GetFullTypeName()const override
		{
			return MatrixUniform< T, Rows, Columns >::GetUniformTypeName();
		}

	private:
		/**
		 *\copydoc		Castor3D::Uniform::DoSetStrValue
		 */
		inline void DoSetStrValue( Castor::String const & p_value, uint32_t p_index = 0 )override;
		/**
		 *\copydoc		Castor3D::Uniform::DoGetStrValue
		 */
		inline Castor::String DoGetStrValue( uint32_t p_index = 0 )const override;

	protected:
		typedef Castor::Policy<T> policy;
		//!\~english	The matrix values.
		//!\~french		Les valeurs matrices.
		std::vector< Castor::Matrix< T, Rows, Columns > > m_mtxValues;
	};
}

#include "MatrixUniform.inl"

#endif
