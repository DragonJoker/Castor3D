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
#ifndef ___C3D_FrameVariable___
#define ___C3D_FrameVariable___

#include "Prerequisites.hpp"
#include "Texture.hpp"
#include "BinaryLoader.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )
#pragma warning( disable:4290 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		19/04/2013
	\~english
	\brief		Shader variables buffer
	\remark		It stores all variables held by a ShaderProgram or a ShaderObject
				<br />It is also in charge of the creation of the variables
				<br />Uses GPU buffers if supported (OpenGL Uniform Buffer Objects, Direct3D Constants buffers)
	\~french
	\brief		Buffer de variables de shader
	\remark		Il stocke toutes les variables contenues dans un ShaderProgram ou un ShaderObject
				<br />Il est aussi responsable de la création des variables
				<br />Utilise les GPU buffers si supportés (OpenGL Uniform Buffer Objects, Direct3D Constants buffers)
	*/
	class C3D_API FrameVariableBuffer
	{
	protected:
		//!\~english	The buffers count	\~french	Le compte des tampons
		static uint32_t sm_uiCount;
		//!\~english	The buffer's index	\~french	L'index du tampon
		uint32_t m_uiIndex;
		//!\~english	The variables list	\~french	La liste de variables
		FrameVariablePtrList m_listVariables;
		//!\~english	The initialised variables list	\~french	La liste de variables initialisées
		FrameVariablePtrList m_listInitialised;
		//!\~english	The variables ordered by name	\~french	Les variables, triées par nom
		FrameVariablePtrStrMap m_mapVariables;
		//!\~english	The render system	\~french	Le render system
		RenderSystem * m_pRenderSystem;
		//!\~english	The buffer name	\~french	Le nom du tampon
		Castor::String m_strName;
		//!\~english	The data buffer	\~french	Le tampon de données
		Castor::ByteArray m_buffer;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pProgram		The program
		 *\param[in]	p_strName		The buffer name
		 *\param[in]	p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pProgram		Le programme
		 *\param[in]	p_strName		Le nom du tampon
		 *\param[in]	p_pRenderSystem	Le render system
		 */
		FrameVariableBuffer( Castor::String const & p_strName, RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~FrameVariableBuffer();
		/**
		 *\~english
		 *\brief		Creates a variable of the wanted type
		 *\param[in]	p_eType		The wanted type
		 *\param[in]	p_strName	The variable name
		 *\param[in]	p_uiNbOcc	The array dimension
		 *\return		The created variable, nullptr if failed
		 *\~french
		 *\brief		Crée une variable du type demandé
		 *\param[in]	p_eType		Le type voulu
		 *\param[in]	p_strName	Le nom de la variable
		 *\param[in]	p_uiNbOcc	Les dimensions du tableau
		 *\return		La variable créée, nullptr en cas d'échec
		 */
		FrameVariableSPtr CreateVariable( ShaderProgramBase & p_program, eFRAME_VARIABLE_TYPE p_eType, Castor::String const & p_strName, uint32_t p_uiNbOcc=1 );
		/**
		 *\~english
		 *\brief		Retrieves a variable by name
		 *\param[in]	p_strName	The variable name
		 *\param[out]	p_pVariable	Receives the found variable, nullptr if failed
		 *\return		\p false if failed
		 *\~french
		 *\brief		Récupère une variable par son nom
		 *\param[in]	p_strName	Le nom de la variable
		 *\param[out]	p_pVariable	Reçoit la variable récupérée, nullptr en cas d'échec
		 *\return		\p false en cas d'échec
		 */
		template< typename T > bool GetVariable( Castor::String const & p_strName, std::shared_ptr< OneFrameVariable< T > > & p_pVariable )const;
		/**
		 *\~english
		 *\brief		Retrieves a variable by name
		 *\param[in]	p_strName	The variable name
		 *\param[out]	p_pVariable	Receives the found variable, nullptr if failed
		 *\return		\p false if failed
		 *\~french
		 *\brief		Récupère une variable par son nom
		 *\param[in]	p_strName	Le nom de la variable
		 *\param[out]	p_pVariable	Reçoit la variable récupérée, nullptr en cas d'échec
		 *\return		\p false en cas d'échec
		 */
		template< typename T, uint32_t Count > bool GetVariable( Castor::String const & p_strName, std::shared_ptr< PointFrameVariable< T, Count > > & p_pVariable )const;
		/**
		 *\~english
		 *\brief		Retrieves a variable by name
		 *\param[in]	p_strName	The variable name
		 *\param[out]	p_pVariable	Receives the found variable, nullptr if failed
		 *\return		\p false if failed
		 *\~french
		 *\brief		Récupère une variable par son nom
		 *\param[in]	p_strName	Le nom de la variable
		 *\param[out]	p_pVariable	Reçoit la variable récupérée, nullptr en cas d'échec
		 *\return		\p false en cas d'échec
		 */
		template< typename T, uint32_t Rows, uint32_t Columns > bool GetVariable( Castor::String const & p_strName, std::shared_ptr< MatrixFrameVariable< T, Rows, Columns > > & p_pVariable )const;
		/**
		 *\~english
		 *\brief		Initialises all the variables and the GPU buffer associated
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Initialise toutes les variables et le tampon GPU associé
		 *\param[in]	p_pProgram	Le programme
		 *\return		\p false if any problem occured
		 */
		bool Initialise( ShaderProgramBase & p_pProgram );
		/**
		 *\~english
		 *\brief		Cleans all the variables up and the GPU buffer associated
		 *\~french
		 *\brief		Nettoie toutes les variables et le tampon GPU associé
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Binds all the variables, through the GPU buffer if supported (OpenGL UBO, Direct3D Constants buffers)
		 *\param[in]	p_pProgram	The program
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Active toutes les variables, au travers du tampon GPU si supporté (OpenGL UBO, Direct3D Constants buffers)
		 *\param[in]	p_pProgram	Le programme
		 *\return		\p false if any problem occured
		 */
		bool Bind();
		/**
		 *\~english
		 *\brief		Unbinds all variables
		 *\param[in]	p_pProgram	The program
		 *\~french
		 *\brief		Désactive toutes les variables
		 */
		void Unbind();

	protected:
		/**
		 *\~english
		 *\brief		Creates a variable of the wanted type
		 *\param[in]	p_eType		The wanted type
		 *\param[in]	p_strName	The variable name
		 *\param[in]	p_uiNbOcc	The array dimension
		 *\return		The created variable, nullptr if failed
		 *\~french
		 *\brief		Crée une variable du type demandé
		 *\param[in]	p_eType		Le type voulu
		 *\param[in]	p_strName	Le nom de la variable
		 *\param[in]	p_uiNbOcc	Les dimensions du tableau
		 *\return		La variable créée, nullptr en cas d'échec
		 */
		virtual FrameVariableSPtr DoCreateVariable( ShaderProgramBase * p_pProgram, eFRAME_VARIABLE_TYPE p_eType, Castor::String const & p_strName, uint32_t p_uiNbOcc=1 )=0;
		/**
		 *\~english
		 *\brief		Initialises all the variables and the GPU buffer associated
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Initialise toutes les variables et le tampon GPU associé
		 *\param[in]	p_pProgram	Le programme
		 *\return		\p false if any problem occured
		 */
		virtual bool DoInitialise( ShaderProgramBase * p_pProgram )=0;
		/**
		 *\~english
		 *\brief		Cleans all the variables up and the GPU buffer associated
		 *\~french
		 *\brief		Nettoie toutes les variables et le tampon GPU associé
		 */
		virtual void DoCleanup()=0;
		/**
		 *\~english
		 *\brief		Binds all the variables, through the GPU buffer if supported (OpenGL UBO, Direct3D Constants buffers)
		 *\param[in]	p_pProgram	The program
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Active toutes les variables, au traves du tampon GPU si supporté (OpenGL UBO, Direct3D Constants buffers)
		 *\param[in]	p_pProgram	Le programme
		 *\return		\p false if any problem occured
		 */
		virtual bool DoBind()=0;
		/**
		 *\~english
		 *\brief		Unbinds all variables
		 *\param[in]	p_pProgram	The program
		 *\~french
		 *\brief		Désactive toutes les variables
		 *\param[in]	p_pProgram	Le programme
		 */
		virtual void DoUnbind()=0;
	};
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
		class C3D_API TextLoader : public Castor::Loader< FrameVariable, Castor::eFILE_TYPE_TEXT, Castor::TextFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode=Castor::File::eENCODING_MODE_ASCII );
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
			virtual bool operator ()( FrameVariable const & p_variable, Castor::TextFile & p_file );
		};

		//!\~english	The names of the frame variable types	\~french	Le nom des types de frame variable
		static const Castor::String Names[];

	protected:
		//!\~english	The variable name as it appears in the shader program	\~french	Le nom de la variable tel qu'il apparaît dans le shader
		Castor::String m_strName;
		//!\~english	Tells if the variable has changed since last execution of the shader	\~french	Dit si la valeur de la variable a changé depuis la dernière exécution du shader
		bool m_bChanged;
		//!\~english	The array dimension if the variable represents an array	\~french	Les dimensions du tableau si la variable représente un tableau
		uint32_t m_uiOcc;
		//!\~english	The value of the variable	\~french	La valeur de la variable
		Castor::StringArray m_strValue;
		//!\~english	The parent shader program	\~french	Le programme parent
		ShaderProgramBase * m_pProgram;

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
		virtual bool Initialise()=0;
		/**
		 *\~english
		 *\brief		Cleans up the variable
		 *\~french
		 *\brief		Nettoie la variable
		 */
		virtual void Cleanup()=0;
		/**
		 *\~english
		 *\brief		Applies this variable
		 *\~french
		 *\brief		Applique cette variable
		 */
		virtual void Apply()=0;
		/**
		 *\~english
		 *\brief		Gives the variable full type
		 *\return		The type
		 *\~english
		 *\brief		Donne le type complet de la variable
		 *\return		Le type
		 */
		virtual eFRAME_VARIABLE_TYPE GetFullType()const=0;
		/**
		 *\~english
		 *\brief		Gives the variable dimension
		 *\return		The dimension
		 *\~english
		 *\brief		Donne la dimension de la variable
		 *\return		La dimension
		 */
		virtual eVARIABLE_TYPE GetType()const=0;
		/**
		 *\~english
		 *\brief		Gives the variable data type
		 *\return		The data type name
		 *\~french
		 *\brief		Donne le type de données de la variable
		 *\return		Le nom du type de données
		 */
		virtual Castor::String GetDataType()const=0;
		/**
		 *\~english
		 *\brief		Binds this variable to it's program or object, depending of implementation (Glsl, Hlsl or Cg)
		 *\~french
		 *\brief		Active cette variable
		 */
		virtual void Bind()=0;
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
		 *\param[in]	p_uiIndex	The index of the value
		 *\param[in]	p_strValue	The string containing the value
		 *\~french
		 *\brief		Définit la valeur de cette variable à partir d'une chaîne
		 *\param[in]	p_uiIndex	L'index de la valeur
		 *\param[in]	p_strValue	La chaîne contenant la valeur
		 */
		virtual void SetValueStr( Castor::String const & p_strValue, uint32_t p_uiIndex );
		/**
		 *\~english
		 *\brief		Defines the name of the variable, as it appears in the shader program
		 *\param[in]	p_strName	The variable name
		 *\~french
		 *\brief		Définit le nom de la variable
		 *\param[in]	p_strName	Le nom
		 */
		virtual void SetName( Castor::String const & p_strName) { m_strName = p_strName; }
		/**
		 *\~english
		 *\brief		Gives the name of the variable, as it appears in the shader program
		 *\return		The variable name
		 *\~french
		 *\brief		Récupère le nom de la variable
		 *\return		Le nom
		 */
		inline Castor::String GetName()const { return m_strName; }
		/**
		 *\~english
		 *\brief		Retrieves the variable string value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur chaîne de la variable
		 *\return		La valeur
		 */
		inline Castor::String GetStrValue()const { return m_strValue[0]; }
		/**
		 *\~english
		 *\brief		Retrieves the variable string value at given index
		 *\param[in]	p_uiIndex	The index
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur chaîne de la variable à l'index donné
		 *\param[in]	p_uiIndex	L'index
		 *\return		La valeur
		 */
		inline Castor::String GetStrValue( uint32_t p_uiIndex )const { CASTOR_ASSERT( p_uiIndex < m_uiOcc);return m_strValue[p_uiIndex]; }
		/**
		 *\~english
		 *\brief		Retrieves a pointer to the variable data
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur sur les données de la variable
		 *\return		Le pointeur
		 */
		virtual uint8_t const * const const_ptr()const=0;
		/**
		 *\~english
		 *\brief		Retrieves the byte size of the variable
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille en octets de la variable
		 *\return		La taille
		 */
		virtual uint32_t size()const=0;
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
		virtual void link( uint8_t * p_pBuffer )=0;
		/**
		*\~english
		*\brief			Retrieves the occurences count
		*\return		The value
		*\~french
		*\brief			Récupère le nombre d'occurences
		*\return		La valeur
		*/
		inline const uint32_t & GetOccCount()const { return m_uiOcc; }
		/**
		*\~english
		*\brief			Retrieves the parent program
		*\return		The program
		*\~french
		*\brief			Récupère le programme parent
		*\return		La programme
		*/
		inline ShaderProgramBase * GetProgram()const { return m_pProgram; }

	protected:
		/**
		 *\~english
		 *\brief		Defines the value of the variable, from a string
		 *\param[in]	p_uiIndex	The index of the value
		 *\param[in]	p_strValue	The string containing the value
		 *\~french
		 *\brief		Définit la valeur de cette variable à partir d'une chaîne
		 *\param[in]	p_uiIndex	L'index de la valeur
		 *\param[in]	p_strValue	La chaîne contenant la valeur
		 */
		virtual void DoSetValueStr( Castor::String const & p_strValue, uint32_t p_uiIndex )=0;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		14/08/2010
	\~english
	\brief		Shader variable representation with variable type
	\~french
	\brief		Représentation d'une variable de shader à type variable
	*/
	template <typename T>
	class TFrameVariable : public FrameVariable
	{
	protected:
		typedef Castor::Policy< T > policy;
		//!\~english	Tells the variable owns it's buffer	\~french	Dit si la variable est responsable de son buffer
		bool	m_bOwnBuffer;
		//!\~english	The buffer containing all values	\~french	Le buffer contenant toutes les valeurs
		T *		m_pValues;

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
		inline Castor::String GetDataType()const { return TFrameVariable< T >::StGetDataType(); }
		/**
		 *\~english
		 *\brief		Gives the variable data type
		 *\return		The data type name
		 *\~french
		 *\brief		Donne le type de données de la variable
		 *\return		Le nom du type de données
		 */
		static Castor::String StGetDataType() { return Castor::str_utils::from_str( typeid( T ).name() ); }
		/**
		 *\~english
		 *\brief		Retrieves a pointer to the variable data
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur sur les données de la variable
		 *\return		Le pointeur
		 */
		virtual uint8_t const * const const_ptr()const { return reinterpret_cast< uint8_t* >( m_pValues ); }
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
	};
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
	class OneFrameVariable : public TFrameVariable<T>
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
		OneFrameVariable & operator =( OneFrameVariable< T > const & p_object );
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
		OneFrameVariable & operator =( OneFrameVariable< T > && p_object );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OneFrameVariable();
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
		inline T & operator []( uint32_t p_uiIndex ) { return this->m_pValues[p_uiIndex]; }
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
		inline T const & operator []( uint32_t p_uiIndex )const { return this->m_pValues[p_uiIndex]; }
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
		 *\brief		Retrieves the variable type
		 *\return		The variable type
		 *\~french
		 *\brief		Récupère le type de la variable
		 *\return		Le type de variable
		 */
		inline eVARIABLE_TYPE GetType()const { return OneFrameVariable< T >::GetVariableType(); }
		/**
		 *\~english
		 *\brief		Retrieves the variable full type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type complet de la variable
		 *\return		Le type
		 */
		inline eFRAME_VARIABLE_TYPE GetFullType()const { return OneFrameVariable< T >::GetFrameVariableType(); }
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_tValue	The new value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_tValue	La valeur
		 */
		inline void SetValue( T const & p_tValue );
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_tValue	The new value
		 *\param[in]	p_uiIndex	The index of the value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_tValue	La valeur
		 *\param[in]	p_uiIndex	L'index de la valeur à modifier
		 */
		inline void SetValue( T const & p_tValue, uint32_t p_uiIndex );
		/**
		 *\~english
		 *\brief		Gives the full type of the variable
		 *\return		The type of the variable
		 *\~french
		 *\brief		Donne le type complet de la variable
		 *\return		Le type complet
		 */
		static inline eVARIABLE_TYPE GetVariableType() { return eVARIABLE_TYPE_ONE; }
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
		inline Castor::Coords<T, Count > operator []( uint32_t p_uiIndex ) { return Castor::Coords< T, Count >( &this->m_pValues[p_uiIndex * Count] ); }
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
		inline Castor::Point< T, Count > operator []( uint32_t p_uiIndex )const { return Castor::Point< T, Count >( &this->m_pValues[p_uiIndex * Count] ); }
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
		inline eVARIABLE_TYPE GetType()const { return PointFrameVariable< T, Count >::GetVariableType(); }
		/**
		 *\~english
		 *\brief		Retrieves the variable full type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type complet de la variable
		 *\return		Le type
		 */
		inline eFRAME_VARIABLE_TYPE GetFullType()const { return PointFrameVariable< T, Count >::GetFrameVariableType(); }
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
	class MatrixFrameVariable : public TFrameVariable< T >
	{
	protected:
		typedef Castor::Policy<T> policy;
		//!\~english	The matrix values	\~french	Les valeurs matrices
		Castor::Matrix< T, Rows, Columns > * m_mtxValue;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		MatrixFrameVariable( ShaderProgramBase * p_pProgram );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_uiOcc		The array dimension
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_uiOcc		Les dimensions du tableau
		 */
		MatrixFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc );
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
		MatrixFrameVariable & operator =( MatrixFrameVariable< T, Rows, Columns > const & p_object );
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
		MatrixFrameVariable & operator =( MatrixFrameVariable< T, Rows, Columns > && p_object );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~MatrixFrameVariable();
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
		inline Castor::Matrix< T, Rows, Columns > & operator []( uint32_t p_uiIndex ) { return m_mtxValue[p_uiIndex]; }
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
		inline Castor::Matrix< T, Rows, Columns > const & operator []( uint32_t p_uiIndex )const { return m_mtxValue[p_uiIndex]; }
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A reference to the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Une référence sur la valeur
		 */
		inline Castor::Matrix< T, Rows, Columns > & GetValue()throw( std::out_of_range );
		/**
		 *\~english
		 *\brief		Retrieves the value
		 *\return		A constant reference to the value
		 *\~french
		 *\brief		Récupère la valeur
		 *\return		Une référence constante sur la valeur
		 */
		inline Castor::Matrix< T, Rows, Columns > const & GetValue()const throw( std::out_of_range );
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
		inline Castor::Matrix< T, Rows, Columns > & GetValue( uint32_t p_uiIndex )throw( std::out_of_range );
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
		inline Castor::Matrix< T, Rows, Columns > const & GetValue( uint32_t p_uiIndex )const throw( std::out_of_range );
		/**
		 *\~english
		 *\brief		Retrieves the variable type
		 *\return		The variable type
		 *\~french
		 *\brief		Récupère le type de la variable
		 *\return		Le type de variable
		 */
		inline eVARIABLE_TYPE GetType()const { return MatrixFrameVariable< T, Rows, Columns >::GetVariableType(); }
		/**
		 *\~english
		 *\brief		Retrieves the variable full type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type complet de la variable
		 *\return		Le type
		 */
		inline eFRAME_VARIABLE_TYPE GetFullType()const { return MatrixFrameVariable< T, Rows, Columns >::GetFrameVariableType(); }
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_mtxValue	The new value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_mtxValue	La valeur
		 */
		inline void SetValue( Castor::Matrix< T, Rows, Columns > const & p_mtxValue );
		/**
		 *\~english
		 *\brief		Defines the value of the variable
		 *\param[in]	p_mtxValue	The new value
		 *\param[in]	p_uiIndex	The index of the value
		 *\~french
		 *\brief		Définit la valeur de la variable
		 *\param[in]	p_mtxValue	La valeur
		 *\param[in]	p_uiIndex	L'index de la valeur à modifier
		 */
		inline void SetValue( Castor::Matrix< T, Rows, Columns > const & p_mtxValue, uint32_t p_uiIndex );
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
		 *\remark		The variable no longer owns it's buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\~french
		 *\brief		Définit le buffer de cette variable
		 *\remark		La variable perd la responsabilité de son buffer
		 *\param[in]	p_pBuffer	Le buffer
		 */
		virtual void link( uint8_t * p_pBuffer );

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

#include "FrameVariable.inl"
}

#pragma warning( pop )

#endif
