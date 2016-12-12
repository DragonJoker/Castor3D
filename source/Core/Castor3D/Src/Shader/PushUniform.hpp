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
#ifndef ___C3D_PushUniform_H___
#define ___C3D_PushUniform_H___

#include "Castor3DPrerequisites.hpp"
#include "UniformTyper.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/08/2010
	\~english
	\brief		Shader variable representation
	\remark		This is a variable that is given to a shader program during it's execution
	\~french
	\brief		Représentation d'une variable de shader
	\remark		Il s'agit d'une variable donnée à un shader au cours de son exécution
	*/
	class PushUniform
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_program	The program.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_program	Le programme.
		 */
		C3D_API PushUniform( ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~PushUniform();
		/**
		 *\~english
		 *\brief		Initialises the variable.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Initialise la variable.
		 *\return		\p false if any problem occured.
		 */
		C3D_API virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Updates this variable's value, in the shader.
		 *\~french
		 *\brief		Met à la jour la valeur de cette variable, dans le shader.
		 */
		C3D_API virtual void Update()const = 0;
		/**
		 *\~english
		 *\return		The parent program.
		 *\~french
		 *\return		La programme parent.
		 */
		inline ShaderProgram & GetProgram()
		{
			return m_program;
		}
		/**
		 *\~english
		 *\return		The parent program.
		 *\~french
		 *\return		La programme parent.
		 */
		inline ShaderProgram const & GetProgram()const
		{
			return m_program;
		}
		/**
		 *\~english
		 *\return		The uniform variable.
		 *\~french
		 *\return		La variable uniforme.
		 */
		virtual Uniform const & GetBaseUniform()const = 0;
		/**
		 *\~english
		 *\return		The uniform variable.
		 *\~french
		 *\return		La variable uniforme.
		 */
		virtual Uniform & GetBaseUniform() = 0;

	protected:
		//!\~english	The parent shader program.
		//!\~french		Le programme parent.
		ShaderProgram & m_program;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/08/2010
	\~english
	\brief		Shader variable representation with variable type.
	\~french
	\brief		Représentation d'une variable de shader à type variable.
	*/
	template< UniformType Type  >
	class TPushUniform
		: public PushUniform
	{
		using type = typename UniformTyper< Type >::type;
		using value_type = typename UniformTyper< Type >::value_type;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_program		The program.
		 *\param[in]	p_occurences	The array dimension.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_occurences	Les dimensions du tableau.
		 *\param[in]	p_program		Le programme.
		 */
		TPushUniform( ShaderProgram & p_program, typename UniformTyper< Type >::type & p_uniform, uint32_t p_occurences );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~TPushUniform();
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A reference to the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Une référence sur la valeur
		 */
		inline value_type & GetValue();
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A constant reference to the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Une référence constante sur la valeur
		 */
		inline value_type const & GetValue()const;
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
		inline value_type & GetValue( uint32_t p_index );
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
		inline value_type const & GetValue( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_value	La valeur
		 */
		inline void SetValue( value_type const & p_value );
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
		inline void SetValue( value_type const & p_value, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Defines the values of the variable.
		 *\param[in]	p_values	The values buffer.
		 *\param[in]	p_size		The values count.
		 *\~french
		 *\brief		Définit les valeurs de la variable.
		 *\param[in]	p_values	Les valeurs.
		 *\param[in]	p_size		Le nombre de valeurs.
		 */
		inline void SetValues( value_type const * p_values, size_t p_size );
		/**
		 *\~english
		 *\brief		Defines the values of the variable.
		 *\param[in]	p_values	The values buffer.
		 *\~french
		 *\brief		Définit les valeurs de la variable.
		 *\param[in]	p_values	Les valeurs.
		 */
		template< size_t N >
		inline void SetValues( value_type const( & p_values )[N] )
		{
			SetValues( p_values, N );
		}
		/**
		 *\~english
		 *\brief		Defines the values of the variable.
		 *\param[in]	p_values	The values buffer.
		 *\~french
		 *\brief		Définit les valeurs de la variable.
		 *\param[in]	p_values	Les valeurs.
		 */
		template< size_t N >
		inline void SetValues( std::array< value_type, N > const & p_values )
		{
			SetValues( p_values.data(), N );
		}
		/**
		 *\~english
		 *\brief		Defines the values of the variable.
		 *\param[in]	p_values	The values buffer.
		 *\~french
		 *\brief		Définit les valeurs de la variable.
		 *\param[in]	p_values	Les valeurs.
		 */
		inline void SetValues( std::vector< value_type > const & p_values )
		{
			SetValues( p_values.data(), p_values.size() );
		}
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
		inline value_type & operator[]( uint32_t p_index );
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
		inline value_type const & operator[]( uint32_t p_index )const;
		/**
		 *\~english
		 *\return		The parent program.
		 *\~french
		 *\return		La programme parent.
		 */
		virtual type & GetUniform()
		{
			return m_uniform;
		}
		/**
		 *\~english
		 *\return		The parent program.
		 *\~french
		 *\return		La programme parent.
		 */
		virtual type const & GetUniform()const
		{
			return m_uniform;
		}
		/**
		 *\~english
		 *\return		The uniform variable.
		 *\~french
		 *\return		La variable uniforme.
		 */
		Uniform & GetBaseUniform()override
		{
			return m_uniform;
		}
		/**
		 *\~english
		 *\return		The uniform variable.
		 *\~french
		 *\return		La variable uniforme.
		 */
		Uniform const & GetBaseUniform()const override
		{
			return m_uniform;
		}

	public:
		//!\~english	The parent shader program.
		//!\~french		Le programme parent.
		type & m_uniform;
	};
}

#include "PushUniform.inl"

#endif
