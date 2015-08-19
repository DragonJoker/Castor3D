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
#ifndef ___C3D_FRAME_VARIABLE_BUFFER_H___
#define ___C3D_FRAME_VARIABLE_BUFFER_H___

#include "Castor3DPrerequisites.hpp"
#include "Texture.hpp"
#include "BinaryParser.hpp"

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
		FrameVariableSPtr CreateVariable( ShaderProgramBase & p_program, eFRAME_VARIABLE_TYPE p_eType, Castor::String const & p_strName, uint32_t p_uiNbOcc = 1 );
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
		 *\param[in]	p_index	The buffer index
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Active toutes les variables, au travers du tampon GPU si supporté (OpenGL UBO, Direct3D Constants buffers)
		 *\param[in]	p_index	L'index du tampon GPU
		 *\return		\p false en cas de problème
		 */
		bool Bind( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Unbinds all variables
		 *\param[in]	p_index	The buffer index
		 *\~french
		 *\brief		Désactive toutes les variables
		 *\param[in]	p_pProgram	L'index du tampon GPU
		 */
		void Unbind( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Retrieves the variables buffer name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom du buffer de variables
		 *\return		Le nom
		 */
		inline Castor::String const & GetName()const
		{
			return m_strName;
		}

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
		virtual FrameVariableSPtr DoCreateVariable( ShaderProgramBase * p_pProgram, eFRAME_VARIABLE_TYPE p_eType, Castor::String const & p_strName, uint32_t p_uiNbOcc = 1 ) = 0;
		/**
		 *\~english
		 *\brief		Initialises all the variables and the GPU buffer associated
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Initialise toutes les variables et le tampon GPU associé
		 *\param[in]	p_pProgram	Le programme
		 *\return		\p false if any problem occured
		 */
		virtual bool DoInitialise( ShaderProgramBase * p_pProgram ) = 0;
		/**
		 *\~english
		 *\brief		Cleans all the variables up and the GPU buffer associated
		 *\~french
		 *\brief		Nettoie toutes les variables et le tampon GPU associé
		 */
		virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Binds all the variables, through the GPU buffer if supported (OpenGL UBO, Direct3D Constants buffers)
		 *\param[in]	p_index	The buffer index
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Active toutes les variables, au traves du tampon GPU si supporté (OpenGL UBO, Direct3D Constants buffers)
		 *\param[in]	p_pProgram	L'index du tampon GPU
		 *\return		\p false if any problem occured
		 */
		virtual bool DoBind( uint32_t p_index ) = 0;
		/**
		 *\~english
		 *\brief		Unbinds all variables
		 *\param[in]	p_index	The buffer index
		 *\~french
		 *\brief		Désactive toutes les variables
		 *\param[in]	p_pProgram	L'index du tampon GPU
		 */
		virtual void DoUnbind( uint32_t p_index ) = 0;

	protected:
		//!\~english The buffers count	\~french Le compte des tampons
		static uint32_t sm_uiCount;
		//!\~english The buffer's index	\~french L'index du tampon
		uint32_t m_uiIndex;
		//!\~english The variables list	\~french La liste de variables
		FrameVariablePtrList m_listVariables;
		//!\~english The initialised variables list	\~french La liste de variables initialisées
		FrameVariablePtrList m_listInitialised;
		//!\~english The variables ordered by name	\~french Les variables, triées par nom
		FrameVariablePtrStrMap m_mapVariables;
		//!\~english The render system	\~french Le render system
		RenderSystem * m_pRenderSystem;
		//!\~english The buffer name	\~french Le nom du tampon
		Castor::String m_strName;
		//!\~english The data buffer	\~french Le tampon de données
		Castor::ByteArray m_buffer;
	};
}

#include "FrameVariableBuffer.inl"

#pragma warning( pop )

#endif
