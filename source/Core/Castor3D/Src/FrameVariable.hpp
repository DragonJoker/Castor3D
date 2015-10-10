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
#ifndef ___C3D_FRAME_VARIABLE_H___
#define ___C3D_FRAME_VARIABLE_H___

#include "Castor3DPrerequisites.hpp"
#include "Texture.hpp"
#include "BinaryParser.hpp"

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
	class C3D_API FrameVariable
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
		class C3D_API TextLoader : public Castor::Loader< FrameVariable, Castor::eFILE_TYPE_TEXT, Castor::TextFile >, public Castor::NonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
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
			virtual bool operator()( FrameVariable const & p_variable, Castor::TextFile & p_file );
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		FrameVariable( ShaderProgramBase * p_pProgram );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_uiOcc		The array dimension
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_uiOcc		Les dimensions du tableau
		 */
		FrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_object	L'objet à copier
		 */
		FrameVariable( FrameVariable const & p_object );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_object	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 */
		FrameVariable( FrameVariable && p_object );
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
		FrameVariable & operator =( FrameVariable const & p_object );
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
		FrameVariable & operator =( FrameVariable && p_object );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~FrameVariable();
		/**
		 *\~english
		 *\brief		Initialises the variable
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Initialise la variable
		 *\return		\p false if any problem occured
		 */
		virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans up the variable
		 *\~french
		 *\brief		Nettoie la variable
		 */
		virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Binds this variable to the shader
		 *\~french
		 *\brief		Lie cette variable au shader
		 */
		virtual void Bind() = 0;
		/**
		 *\~english
		 *\brief		Unbinds this variable from the shader
		 *\~french
		 *\brief		Délie cette variable du shader
		 */
		virtual void Unbind() = 0;
		/**
		 *\~english
		 *\brief		Gives the variable full type
		 *\return		The type
		 *\~english
		 *\brief		Donne le type complet de la variable
		 *\return		Le type
		 */
		virtual eFRAME_VARIABLE_TYPE GetFullType()const = 0;
		/**
		 *\~english
		 *\brief		Gives the variable dimension
		 *\return		The dimension
		 *\~english
		 *\brief		Donne la dimension de la variable
		 *\return		La dimension
		 */
		virtual eVARIABLE_TYPE GetType()const = 0;
		/**
		 *\~english
		 *\brief		Gives the variable full type name
		 *\return		The type
		 *\~english
		 *\brief		Donne le nom du type complet de la variable
		 *\return		Le type
		 */
		virtual Castor::String GetFullTypeName()const = 0;
		/**
		 *\~english
		 *\brief		Gives the variable data type name
		 *\return		The data type name
		 *\~french
		 *\brief		Donne le nom du type de données de la variable
		 *\return		Le nom du type de données
		 */
		virtual Castor::String GetDataTypeName()const = 0;
		/**
		 *\~english
		 *\brief		Defines the value of the variable, from a string
		 *\param[in]	p_strValue	The string containing the value
		 *\~french
		 *\brief		Définit la valeur de cette variable à partir d'une chaîne
		 *\param[in]	p_strValue	La chaîne contenant la valeur
		 */
		virtual void SetValueStr( Castor::String const & p_strValue );
		/**
		 *\~english
		 *\brief		Defines the value of the variable, from a string
		 *\param[in]	p_index	The index of the value
		 *\param[in]	p_strValue	The string containing the value
		 *\~french
		 *\brief		Définit la valeur de cette variable à partir d'une chaîne
		 *\param[in]	p_index	L'index de la valeur
		 *\param[in]	p_strValue	La chaîne contenant la valeur
		 */
		virtual void SetValueStr( Castor::String const & p_strValue, uint32_t p_index );
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
			m_strName = p_name;
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
			return m_strName;
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
			CASTOR_ASSERT( p_index < m_uiOcc );
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
		 *\remark		The variable no longer owns it's buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\~french
		 *\brief		Définit le buffer de cette variable
		 *\remark		La variable perd la responsabilité de son buffer
		 *\param[in]	p_pBuffer	Le buffer
		 */
		virtual void link( uint8_t * p_pBuffer ) = 0;
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
			return m_uiOcc;
		}
		/**
		*\~english
		*\brief			Retrieves the parent program
		*\return		The program
		*\~french
		*\brief			Récupère le programme parent
		*\return		La programme
		*/
		inline ShaderProgramBase * GetProgram()const
		{
			return m_pProgram;
		}

	protected:
		/**
		 *\~english
		 *\brief		Defines the value of the variable, from a string
		 *\param[in]	p_index	The index of the value
		 *\param[in]	p_strValue	The string containing the value
		 *\~french
		 *\brief		Définit la valeur de cette variable à partir d'une chaîne
		 *\param[in]	p_index	L'index de la valeur
		 *\param[in]	p_strValue	La chaîne contenant la valeur
		 */
		virtual void DoSetValueStr( Castor::String const & p_strValue, uint32_t p_index ) = 0;

	protected:
		//!\~english The variable name as it appears in the shader program	\~french Le nom de la variable tel qu'il apparaît dans le shader
		Castor::String m_strName;
		//!\~english Tells if the variable has changed since last execution of the shader	\~french Dit si la valeur de la variable a changé depuis la dernière exécution du shader
		bool m_bChanged;
		//!\~english The array dimension if the variable represents an array	\~french Les dimensions du tableau si la variable représente un tableau
		uint32_t m_uiOcc;
		//!\~english The value of the variable	\~french La valeur de la variable
		Castor::StringArray m_strValue;
		//!\~english The parent shader program	\~french Le programme parent
		ShaderProgramBase * m_pProgram;
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
		:	public FrameVariable
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		TFrameVariable( ShaderProgramBase * p_pProgram );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_uiOcc		The array dimension
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_uiOcc		Les dimensions du tableau
		 */
		TFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_object	L'objet à copier
		 */
		TFrameVariable( TFrameVariable< T > const & p_object );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_object	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 */
		TFrameVariable( TFrameVariable< T > && p_object );
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
		TFrameVariable & operator =( TFrameVariable< T > const & p_object );
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
		TFrameVariable & operator =( TFrameVariable< T > && p_object );
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
		 *\remark		The variable no longer owns it's buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\~french
		 *\brief		Définit le buffer de cette variable
		 *\remark		La variable perd la responsabilité de son buffer
		 *\param[in]	p_pBuffer	Le buffer
		 */
		virtual void link( uint8_t * p_pBuffer );

	protected:
		inline void DoCleanupBuffer();

	protected:
		typedef Castor::Policy< T > policy;
		//!\~english Tells the variable owns it's buffer	\~french Dit si la variable est responsable de son buffer
		bool m_bOwnBuffer;
		//!\~english The buffer containing all values	\~french Le buffer contenant toutes les valeurs
		T * m_pValues;
	};
}

#include "FrameVariable.inl"

#endif
