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
#ifndef ___C3D_Uniform_H___
#define ___C3D_Uniform_H___

#include "UniformHelpers.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/08/2010
	\~english
	\brief		Shader variable representation.
	\remark		This is a variable that is given to a shader program during it's execution.
	\~french
	\brief		Représentation d'une variable de shader.
	\remark		Il s'agit d'une variable donnée à un shader au cours de son exécution.
	*/
	class Uniform
		: public castor::NonCopyable
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		14/08/2010
		\~english
		\brief		Uniform loader.
		\~french
		\brief		Loader de Uniform.
		*/
		class TextWriter
			: public castor::TextWriter< Uniform >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a Uniform into a text file.
			 *\param[in]	p_file		The file to save the Uniform in.
			 *\param[in]	p_variable	The Uniform to save.
			 *\~french
			 *\brief		Ecrit une Uniform dans un fichier texte.
			 *\param[in]	p_file		Le fichier.
			 *\param[in]	p_variable	La Uniform.
			 */
			C3D_API bool operator()( Uniform const & p_variable, castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_occurences	The array dimensions.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_occurences	Les dimensions du tableau.
		 */
		C3D_API Uniform( uint32_t p_occurences );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~Uniform();
		/**
		 *\~english
		 *\brief		Defines the value of the variable, from a string.
		 *\param[in]	p_value	The string containing the value.
		 *\~french
		 *\brief		Définit la valeur de la variable à partir d'une chaîne.
		 *\param[in]	p_value	La chaîne.
		 */
		C3D_API void setStrValues( castor::String const & p_value );
		/**
		 *\~english
		 *\brief		Retrieves the string values of the variable.
		 *\~french
		 *\brief		Récupère les valeurs chaîne de la variable.
		 */
		C3D_API castor::String getStrValues()const;
		/**
		 *\~english
		 *\brief		Defines the value of the variable, from a string.
		 *\param[in]	p_value	The string containing the value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit la valeur de la variable à partir d'une chaîne.
		 *\param[in]	p_value	La chaîne.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		C3D_API void setStrValue( castor::String const & p_value, uint32_t p_index = 0 );
		/**
		 *\~english
		 *\brief		Retrieves the string value of the variable.
		 *\param[in]	p_index	The value index.
		 *\return		The variable string value.
		 *\~french
		 *\brief		Récupère la valeur chaîne de la variable.
		 *\param[in]	p_index	L'indice de la valeur.
		 *\return		La valeur chaîne de la variable.
		 */
		C3D_API castor::String getStrValue( uint32_t p_index = 0 )const;
		/**
		 *\~english
		 *\return		The pointer to the variable data.
		 *\~french
		 *\return		Le pointeur sur les données de la variable.
		 */
		C3D_API virtual uint8_t const * const constPtr()const = 0;
		/**
		 *\~english
		 *\return		The byte size of the variable.
		 *\~french
		 *\return		La taille en octets de la variable.
		 */
		C3D_API virtual uint32_t size()const = 0;
		/**
		 *\~english
		 *\return		The variable's type.
		 *\~english
		 *\return		La type de la variable.
		 */
		C3D_API virtual VariableType getType()const = 0;
		/**
		 *\~english
		 *\return		The variable's full type.
		 *\~english
		 *\return		Le type complet de la variable.
		 */
		C3D_API virtual UniformType getFullType()const = 0;
		/**
		 *\~english
		 *\return		The variable's full type name.
		 *\~english
		 *\return		Le nom du type complet de la variable.
		 */
		C3D_API virtual castor::String const & getFullTypeName()const = 0;
		/**
		 *\~english
		 *\return		The variable's data type name.
		 *\~french
		 *\return		Le nom du type de données de la variable.
		 */
		C3D_API virtual castor::String const & getDataTypeName()const = 0;
		/**
		 *\~english
		 *\brief		Defines the buffer holding the frame variable.
		 *\remarks		The variable no longer owns it's buffer.
		 *\param[in]	p_buffer	The buffer.
		 *\param[in]	p_stride	The stride between each element in the buffer.
		 *\~french
		 *\brief		Définit le tampon de cette variable.
		 *\remarks		La variable perd la responsabilité de son tampon.
		 *\param[in]	p_buffer	Le tampon.
		 *\param[in]	p_stride	La distance binaire entre chaque valeur dans le tampon.
		 */
		C3D_API virtual void link( uint8_t * p_buffer, uint32_t p_stride = 0u ) = 0;
		/**
		 *\~english
		 *\brief		Defines the name of the variable, as it appears in the shader program.
		 *\param[in]	p_name	The variable name.
		 *\~french
		 *\brief		Définit le nom de la variable.
		 *\param[in]	p_name	Le nom.
		 */
		inline void setName( castor::String const & p_name )
		{
			m_name = p_name;
		}
		/**
		 *\~english
		 *\return		The name of the variable, as it appears in the shader program.
		 *\~french
		 *\return		Le nom de la variable, tel qu'il apparaît dans le programme shader.
		 */
		inline castor::String getName()const
		{
			return m_name;
		}
		/**
		 *\~english
		 *\return		The occurences count.
		 *\~french
		 *\return		Le nombre d'occurences.
		 */
		inline const uint32_t & getOccCount()const
		{
			return m_occurences;
		}
		/**
		 *\~english
		 *\return		The changed status.
		 *\~french
		 *\return		Le statut de changement.
		 */
		inline bool isChanged()const
		{
			return m_changed;
		}
		/**
		 *\~english
		 *\brief		Defines the changed status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\return		Définit statut de changement.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setChanged( bool p_value = true )
		{
			m_changed = p_value;
		}

	private:
		/**
		 *\~english
		 *\brief		Defines the value of the variable, from a string.
		 *\param[in]	p_value	The string containing the value.
		 *\param[in]	p_index	The value index.
		 *\~french
		 *\brief		Définit la valeur de la variable à partir d'une chaîne.
		 *\param[in]	p_value	La chaîne.
		 *\param[in]	p_index	L'indice de la valeur.
		 */
		virtual void doSetStrValue( castor::String const & p_value, uint32_t p_index = 0 ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the string value of the variable.
		 *\param[in]	p_index	The value index.
		 *\return		The variable string value.
		 *\~french
		 *\brief		Récupère la valeur chaîne de la variable.
		 *\param[in]	p_index	L'indice de la valeur.
		 *\return		La valeur chaîne de la variable.
		 */
		virtual castor::String doGetStrValue( uint32_t p_index = 0 )const = 0;

	protected:
		//!\~english	The variable name as it appears in the shader program.
		//!\~french		Le nom de la variable tel qu'il apparaît dans le shader.
		castor::String m_name;
		//!\~english	Tells if the variable has changed since last execution of the shader.
		//!\~french		Dit si la valeur de la variable a changé depuis la dernière exécution du shader.
		mutable bool m_changed;
		//!\~english	The array dimension if the variable represents an array.
		//!\~french		Les dimensions du tableau si la variable représente un tableau.
		uint32_t m_occurences;
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
	template< UniformType Type >
	class TUniform
		: public Uniform
	{
	public:
		using type = typename UniformTypeTraits< Type >::type;
		using value_type = typename UniformTypeTraits< Type >::value_type;
		using value_sub_type = typename UniformTypeTraits< Type >::value_sub_type;
		using param_type = typename UniformTypeTraits< Type >::param_type;
		using return_type = typename UniformTypeTraits< Type >::return_type;
		using return_const_type = typename UniformTypeTraits< Type >::return_const_type;
		using typed_value = typename UniformTypeTraits< Type >::typed_value_type;
		static constexpr auto stride = VariableTypeTraits< Type >::count * sizeof( value_sub_type );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_occurences	The array dimension.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_occurences	Les dimensions du tableau.
		 */
		inline TUniform( uint32_t p_occurences );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		inline ~TUniform();
		/**
		 *\~english
		 *\return		The values.
		 *\~french
		 *\return		Les valeurs.
		 */
		inline value_sub_type const * getValues()const;
		/**
		 *\~english
		 *\return		A reference to the value.
		 *\~french
		 *\return		Une référence sur la valeur.
		 */
		inline return_type & getValue();
		/**
		 *\~english
		 *\return		A constant reference to the value.
		 *\~french
		 *\return		Une référence constante sur la valeur.
		 */
		inline return_const_type const & getValue()const;
		/**
		 *\~english
		 *\brief		Retrieves the value at given index.
		 *\remarks		Check the index bounds.
		 *\param[in]	p_index	The index.
		 *\return		A reference to the value at given index.
		 *\~french
		 *\brief		Récupère la valeur à l'index donné.
		 *\remarks		Vérifie que l'index est dans les bornes.
		 *\param[in]	p_index	L'indice.
		 *\return		Une référence sur la valeur à l'index donné0
		 */
		inline return_type & getValue( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Retrieves the value at given index.
		 *\remarks		Check the index bounds.
		 *\param[in]	p_index	The index.
		 *\return		A constant reference to the value at given index.
		 *\~french
		 *\brief		Récupère la valeur à l'index donné.
		 *\remarks		Vérifie que l'index est dans les bornes.
		 *\param[in]	p_index	L'indice.
		 *\return		Une référence constante sur la valeur à l'index donné.
		 */
		inline return_const_type const & getValue( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Defines the value of the variable.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la valeur de la variable.
		 *\param[in]	p_value	La valeur.
		 */
		inline void setValue( param_type const & p_value );
		/**
		 *\~english
		 *\brief		Defines the value of the variable.
		 *\param[in]	p_value	The new value.
		 *\param[in]	p_index	The index of the value.
		 *\~french
		 *\brief		Définit la valeur de la variable.
		 *\param[in]	p_value	La valeur.
		 *\param[in]	p_index	L'index de la valeur à modifier.
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
		inline void setValues( param_type const( & p_values )[N] );
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
		 *\brief		Array subscript operator.
		 *\remarks		doesn't check the index bounds.
		 *\param[in]	p_index	The index.
		 *\return		A reference to the value at given index.
		 *\~french
		 *\brief		Opérateur d'accès de type tableau.
		 *\remarks		Ne vérifie pas que l'index est dans les bornes.
		 *\param[in]	p_index	L'indice.
		 *\return		Une référence sur la valeur à l'index donné.
		 */
		inline return_type & operator[]( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Array subscript operator.
		 *\remarks		doesn't check the index bounds.
		 *\param[in]	p_index	The index.
		 *\return		A constant reference to the value at given index.
		 *\~french
		 *\brief		Opérateur d'accès de type tableau.
		 *\remarks		Ne vérifie pas que l'index est dans les bornes.
		 *\param[in]	p_index	L'indice.
		 *\return		Une référence constante sur la valeur à l'index donné.
		 */
		inline return_const_type const & operator[]( uint32_t p_index )const;
		/**
		 *\~english
		 *\return		The type of the variable.
		 *\~french
		 *\return		Le type de la variable.
		 */
		static inline constexpr VariableType getVariableType();
		/**
		 *\~english
		 *\return		The variable's full type.
		 *\~english
		 *\return		Le type complet de la variable.
		 */
		static inline constexpr UniformType getUniformType();
		/**
		 *\~english
		 *\return		The variable's full type name.
		 *\~english
		 *\return		Le nom du type complet de la variable.
		 */
		static inline castor::String const & getUniformTypeName();
		/**
		 *\copydoc		castor3d::Uniform::size
		 */
		inline uint32_t size()const override;
		/**
		 *\copydoc		castor3d::Uniform::constPtr
		 */
		inline uint8_t const * const constPtr()const override;
		/**
		 *\copydoc		castor3d::Uniform::link
		 */
		void link( uint8_t * p_buffer, uint32_t p_stride )override;
		/**
		 *\copydoc		castor3d::Uniform::getDataTypeName
		 */
		inline castor::String const & getDataTypeName()const override;
		/**
		 *\copydoc		castor3d::Uniform::getType
		 */
		inline VariableType getType()const override;
		/**
		 *\copydoc		castor3d::Uniform::getFullType
		 */
		inline UniformType getFullType()const override;
		/**
		 *\copydoc		castor3d::Uniform::getFullTypeName
		 */
		inline castor::String const & getFullTypeName()const override;

	private:
		/**
		 *\copydoc		castor3d::Uniform::doSetStrValue
		 */
		inline void doSetStrValue( castor::String const & p_value, uint32_t p_index = 0 )override;
		/**
		 *\copydoc		castor3d::Uniform::doGetStrValue
		 */
		inline castor::String doGetStrValue( uint32_t p_index = 0 )const override;
		/**
		 *\~english
		 *\return		Cleans up the internal buffer of values.
		 *\~english
		 *\return		Nettoie le tampon interne de valeurs.
		 */
		inline void doCleanupBuffer();

	protected:
		//!\~english	Tells the variable owns it's buffer.
		//!\~french		Dit si la variable est responsable de son tampon.
		bool m_ownBuffer{ true };
		//!\~english	The buffer containing all values.
		//!\~french		Le tampon contenant toutes les valeurs.
		value_sub_type * m_values{ nullptr };
		//!\~english	The buffer containing typed values.
		//!\~french		Le tampon contenant toutes les valeurs. typées
		std::vector< typed_value > m_typedValues;
		//!\~english	The stride between each value in the buffer.
		//!\~french		La distance binaire entrechaque valeur dans le tampon.
		uint32_t m_stride{ 0u };
	};
}

#include "Uniform.inl"

#endif
