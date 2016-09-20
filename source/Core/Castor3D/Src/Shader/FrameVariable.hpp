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
#ifndef ___C3D_FRAME_VARIABLE_H___
#define ___C3D_FRAME_VARIABLE_H___

#include "Castor3DPrerequisites.hpp"

#include "Texture/TextureLayout.hpp"

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
	class FrameVariable
		: public Castor::NonCopyable
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		14/08/2010
		\~english
		\brief		FrameVariable loader
		\~french
		\brief		Loader de FrameVariable
		*/
		class TextWriter
			: public Castor::TextWriter< FrameVariable >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a FrameVariable into a text file
			 *\param[in]	p_file		The file to save the FrameVariable in
			 *\param[in]	p_variable	The FrameVariable to save
			 *\~french
			 *\brief		Ecrit une FrameVariable dans un fichier texte
			 *\param[in]	p_file		Le fichier
			 *\param[in]	p_variable	La FrameVariable
			 */
			C3D_API bool operator()( FrameVariable const & p_variable, Castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_program	The program
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_program	Le programme
		 */
		C3D_API explicit FrameVariable( ShaderProgram & p_program );
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
		C3D_API FrameVariable( ShaderProgram & p_program, uint32_t p_occurences );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~FrameVariable();
		/**
		 *\~english
		 *\brief		Initialises the variable
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Initialise la variable
		 *\return		\p false if any problem occured
		 */
		C3D_API virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans up the variable
		 *\~french
		 *\brief		Nettoie la variable
		 */
		C3D_API virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Binds this variable to the shader
		 *\~french
		 *\brief		Lie cette variable au shader
		 */
		C3D_API virtual void Bind() = 0;
		/**
		 *\~english
		 *\brief		Unbinds this variable from the shader
		 *\~french
		 *\brief		Délie cette variable du shader
		 */
		C3D_API virtual void Unbind() = 0;
		/**
		 *\~english
		 *\brief		Gives the variable full type
		 *\return		The type
		 *\~english
		 *\brief		Donne le type complet de la variable
		 *\return		Le type
		 */
		C3D_API virtual FrameVariableType GetFullType()const = 0;
		/**
		 *\~english
		 *\brief		Gives the variable dimension
		 *\return		The dimension
		 *\~english
		 *\brief		Donne la dimension de la variable
		 *\return		La dimension
		 */
		C3D_API virtual VariableType GetType()const = 0;
		/**
		 *\~english
		 *\brief		Gives the variable full type name
		 *\return		The type
		 *\~english
		 *\brief		Donne le nom du type complet de la variable
		 *\return		Le type
		 */
		C3D_API virtual Castor::String GetFullTypeName()const = 0;
		/**
		 *\~english
		 *\brief		Gives the variable data type name
		 *\return		The data type name
		 *\~french
		 *\brief		Donne le nom du type de données de la variable
		 *\return		Le nom du type de données
		 */
		C3D_API virtual Castor::String GetDataTypeName()const = 0;
		/**
		 *\~english
		 *\brief		Defines the value of the variable, from a string
		 *\param[in]	p_value	The string containing the value
		 *\~french
		 *\brief		Définit la valeur de cette variable à partir d'une chaîne
		 *\param[in]	p_value	La chaîne contenant la valeur
		 */
		C3D_API virtual void SetValueStr( Castor::String const & p_value );
		/**
		 *\~english
		 *\brief		Defines the value of the variable, from a string
		 *\param[in]	p_index	The index of the value
		 *\param[in]	p_value	The string containing the value
		 *\~french
		 *\brief		Définit la valeur de cette variable à partir d'une chaîne
		 *\param[in]	p_index	L'index de la valeur
		 *\param[in]	p_value	La chaîne contenant la valeur
		 */
		C3D_API virtual void SetValueStr( Castor::String const & p_value, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Defines the name of the variable, as it appears in the shader program
		 *\param[in]	p_name	The variable name
		 *\~french
		 *\brief		Définit le nom de la variable
		 *\param[in]	p_name	Le nom
		 */
		virtual void SetName( Castor::String const & p_name )
		{
			m_name = p_name;
		}
		/**
		 *\~english
		 *\brief		Gives the name of the variable, as it appears in the shader program
		 *\return		The variable name
		 *\~french
		 *\brief		Récupère le nom de la variable
		 *\return		Le nom
		 */
		inline Castor::String GetName()const
		{
			return m_name;
		}
		/**
		 *\~english
		 *\brief		Retrieves the variable string value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur chaîne de la variable
		 *\return		La valeur
		 */
		inline Castor::String GetStrValue()const
		{
			return m_strValue[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves the variable string value at given index
		 *\param[in]	p_index	The index
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur chaîne de la variable à l'index donné
		 *\param[in]	p_index	L'index
		 *\return		La valeur
		 */
		inline Castor::String GetStrValue( uint32_t p_index )const
		{
			REQUIRE( p_index < m_occurences );
			return m_strValue[p_index];
		}
		/**
		 *\~english
		 *\brief		Retrieves a pointer to the variable data
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur sur les données de la variable
		 *\return		Le pointeur
		 */
		virtual uint8_t const * const const_ptr()const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the byte size of the variable
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille en octets de la variable
		 *\return		La taille
		 */
		virtual uint32_t size()const = 0;
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
		virtual void link( uint8_t * p_buffer ) = 0;
		/**
		 *\~english
		 *\brief			Retrieves the occurences count
		 *\return		The value
		 *\~french
		 *\brief			Récupère le nombre d'occurences
		 *\return		La valeur
		 */
		inline const uint32_t & GetOccCount()const
		{
			return m_occurences;
		}
		/**
		 *\~english
		 *\brief			Retrieves the parent program
		 *\return		The program
		 *\~french
		 *\brief			Récupère le programme parent
		 *\return		La programme
		 */
		inline ShaderProgram & GetProgram()
		{
			return m_program;
		}
		/**
		 *\~english
		 *\brief			Retrieves the parent program
		 *\return		The program
		 *\~french
		 *\brief			Récupère le programme parent
		 *\return		La programme
		 */
		inline ShaderProgram const & GetProgram()const
		{
			return m_program;
		}

	protected:
		/**
		 *\~english
		 *\brief		Defines the value of the variable, from a string
		 *\param[in]	p_index	The index of the value
		 *\param[in]	p_value	The string containing the value
		 *\~french
		 *\brief		Définit la valeur de cette variable à partir d'une chaîne
		 *\param[in]	p_index	L'index de la valeur
		 *\param[in]	p_value	La chaîne contenant la valeur
		 */
		C3D_API virtual void DoSetValueStr( Castor::String const & p_value, uint32_t p_index ) = 0;

	protected:
		//!\~english The variable name as it appears in the shader program	\~french Le nom de la variable tel qu'il apparaît dans le shader
		Castor::String m_name;
		//!\~english Tells if the variable has changed since last execution of the shader	\~french Dit si la valeur de la variable a changé depuis la dernière exécution du shader
		bool m_changed;
		//!\~english The array dimension if the variable represents an array	\~french Les dimensions du tableau si la variable représente un tableau
		uint32_t m_occurences;
		//!\~english The value of the variable	\~french La valeur de la variable
		Castor::StringArray m_strValue;
		//!\~english The parent shader program	\~french Le programme parent
		ShaderProgram & m_program;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		10/09/2015
	\~english
	\brief		Helper structure used to retrieve the frame variable data type name.
	\~french
	\brief		Structure d'aide pour récupérer le nom du type de données d'une variable de frame.
	*/
	template< typename T > struct FrameVariableDataTyper;
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/08/2010
	\~english
	\brief		Shader variable representation with variable type
	\~french
	\brief		Représentation d'une variable de shader à type variable
	*/
	template< typename T >
	class TFrameVariable
		: public FrameVariable
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
		explicit TFrameVariable( ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_program		The program
		 *\param[in]	p_occurences	The array dimension
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_occurences	Les dimensions du tableau
		 *\param[in]	p_program		Le programme
		 */
		TFrameVariable( ShaderProgram & p_program, uint32_t p_occurences );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TFrameVariable();
		/**
		 *\~english
		 *\brief		Gives the variable data type
		 *\return		The data type name
		 *\~french
		 *\brief		Donne le type de données de la variable
		 *\return		Le nom du type de données
		 */
		inline Castor::String GetDataTypeName()const;
		/**
		 *\~english
		 *\brief		Retrieves a pointer to the variable data
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur sur les données de la variable
		 *\return		Le pointeur
		 */
		virtual uint8_t const * const const_ptr()const;
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

	protected:
		inline void DoCleanupBuffer();

	protected:
		typedef Castor::Policy< T > policy;
		//!\~english Tells the variable owns it's buffer	\~french Dit si la variable est responsable de son buffer
		bool m_bOwnBuffer;
		//!\~english The buffer containing all values	\~french Le buffer contenant toutes les valeurs
		T * m_values;
	};
}

#include "FrameVariable.inl"

#endif
