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

#include "Uniform.hpp"

namespace castor3d
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
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Updates this variable's value, in the shader.
		 *\~french
		 *\brief		Met à la jour la valeur de cette variable, dans le shader.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\return		The parent program.
		 *\~french
		 *\return		La programme parent.
		 */
		inline ShaderProgram & getProgram()
		{
			return m_program;
		}
		/**
		 *\~english
		 *\return		The parent program.
		 *\~french
		 *\return		La programme parent.
		 */
		inline ShaderProgram const & getProgram()const
		{
			return m_program;
		}
		/**
		 *\~english
		 *\return		The uniform variable.
		 *\~french
		 *\return		La variable uniforme.
		 */
		virtual Uniform const & getBaseUniform()const = 0;
		/**
		 *\~english
		 *\return		The uniform variable.
		 *\~french
		 *\return		La variable uniforme.
		 */
		virtual Uniform & getBaseUniform() = 0;

	private:
		/**
		 *\~english
		 *\brief		Initialises the variable.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Initialise la variable.
		 *\return		\p false if any problem occured.
		 */
		C3D_API virtual bool doInitialise() = 0;
		/**
		 *\~english
		 *\brief		Updates this variable's value, in the shader.
		 *\~french
		 *\brief		Met à la jour la valeur de cette variable, dans le shader.
		 */
		C3D_API virtual void doUpdate()const = 0;

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
	public:
		using type = TUniform< Type >;
		using value_type = typename type::value_type;
		using value_sub_type = typename type::value_sub_type;
		using param_type = typename type::param_type;
		using return_type = typename type::return_type;
		using return_const_type = typename type::return_const_type;
		using typed_value = typename type::typed_value;
		static constexpr auto stride = type::stride;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_program		The program.
		 *\param[in]	p_occurences	The array dimensions.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_program		Le programme.
		 *\param[in]	p_occurences	Les dimensions du tableau.
		 */
		TPushUniform( ShaderProgram & p_program, uint32_t p_occurences );
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
		inline return_type & getValue();
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A constant reference to the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Une référence constante sur la valeur
		 */
		inline return_const_type const & getValue()const;
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
		inline return_type & getValue( uint32_t p_index );
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
		inline return_const_type const & getValue( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_value	La valeur
		 */
		inline void setValue( param_type const & p_value );
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
		inline void setValue( param_type const & p_value, uint32_t p_index );
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
		inline void setValues( param_type const * p_values, size_t p_size );
		/**
		 *\~english
		 *\brief		Defines the values of the variable.
		 *\param[in]	p_values	The values buffer.
		 *\~french
		 *\brief		Définit les valeurs de la variable.
		 *\param[in]	p_values	Les valeurs.
		 */
		template< size_t N >
		inline void setValues( param_type const( &p_values )[N] );
		/**
		 *\~english
		 *\brief		Defines the values of the variable.
		 *\param[in]	p_values	The values buffer.
		 *\~french
		 *\brief		Définit les valeurs de la variable.
		 *\param[in]	p_values	Les valeurs.
		 */
		template< size_t N >
		inline void setValues( std::array< param_type, N > const & p_values );
		/**
		 *\~english
		 *\brief		Defines the values of the variable.
		 *\param[in]	p_values	The values buffer.
		 *\~french
		 *\brief		Définit les valeurs de la variable.
		 *\param[in]	p_values	Les valeurs.
		 */
		inline void setValues( std::vector< param_type > const & p_values );
		/**
		 *\~english
		 *\brief		Array subscript operator
		 *\remarks		doesn't check the index bounds
		 *\param[in]	p_index	The index
		 *\return		A reference to the value at given index
		 *\~french
		 *\brief		Opérateur d'accès de type tableau
		 *\remarks		Ne vérifie pas que l'index est dans les bornes
		 *\param[in]	p_index	L'indice
		 *\return		Une référence sur la valeur à l'index donné
		 */
		inline return_type & operator[]( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Array subscript operator
		 *\remarks		doesn't check the index bounds
		 *\param[in]	p_index	The index
		 *\return		A constant reference to the value at given index
		 *\~french
		 *\brief		Opérateur d'accès de type tableau
		 *\remarks		Ne vérifie pas que l'index est dans les bornes
		 *\param[in]	p_index	L'indice
		 *\return		Une référence constante sur la valeur à l'index donné
		 */
		inline return_const_type const & operator[]( uint32_t p_index )const;
		/**
		 *\~english
		 *\return		The parent program.
		 *\~french
		 *\return		La programme parent.
		 */
		inline type & getUniform()
		{
			return m_uniform;
		}
		/**
		 *\~english
		 *\return		The parent program.
		 *\~french
		 *\return		La programme parent.
		 */
		inline type const & getUniform()const
		{
			return m_uniform;
		}
		/**
		 *\~english
		 *\return		The uniform variable.
		 *\~french
		 *\return		La variable uniforme.
		 */
		inline Uniform & getBaseUniform()override
		{
			return m_uniform;
		}
		/**
		 *\~english
		 *\return		The uniform variable.
		 *\~french
		 *\return		La variable uniforme.
		 */
		inline Uniform const & getBaseUniform()const override
		{
			return m_uniform;
		}

	protected:
		//!\~english	The parent shader program.
		//!\~french		Le programme parent.
		type m_uniform;
	};
}

#include "PushUniform.inl"

#endif
