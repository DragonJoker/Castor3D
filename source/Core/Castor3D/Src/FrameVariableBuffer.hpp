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

#include <OwnedBy.hpp>

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
	class FrameVariableBuffer
		: public Castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_program		The program
		 *\param[in]	p_name		The buffer name
		 *\param[in]	p_renderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_program		Le programme
		 *\param[in]	p_name		Le nom du tampon
		 *\param[in]	p_renderSystem	Le render system
		 */
		C3D_API FrameVariableBuffer( Castor::String const & p_name, RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~FrameVariableBuffer();
		/**
		 *\~english
		 *\brief		Initialises all the variables and the GPU buffer associated
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Initialise toutes les variables et le tampon GPU associé
		 *\param[in]	p_program	Le programme
		 *\return		\p false if any problem occured
		 */
		C3D_API bool Initialise( ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Cleans all the variables up and the GPU buffer associated
		 *\~french
		 *\brief		Nettoie toutes les variables et le tampon GPU associé
		 */
		C3D_API void Cleanup();
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
		C3D_API bool Bind( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Unbinds all variables
		 *\param[in]	p_index	The buffer index
		 *\~french
		 *\brief		Désactive toutes les variables
		 *\param[in]	p_program	L'index du tampon GPU
		 */
		C3D_API void Unbind( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Creates a variable of the wanted type
		 *\param[in]	p_type		The wanted type
		 *\param[in]	p_name	The variable name
		 *\param[in]	p_occurences	The array dimension
		 *\return		The created variable, nullptr if failed
		 *\~french
		 *\brief		Crée une variable du type demandé
		 *\param[in]	p_type		Le type voulu
		 *\param[in]	p_name	Le nom de la variable
		 *\param[in]	p_occurences	Les dimensions du tableau
		 *\return		La variable créée, nullptr en cas d'échec
		 */
		C3D_API FrameVariableSPtr CreateVariable( ShaderProgram & p_program, eFRAME_VARIABLE_TYPE p_type, Castor::String const & p_name, uint32_t p_occurences = 1 );
		/**
		 *\~english
		 *\brief		Removes a variable from this buffer
		 *\param[in]	p_name	The variable name
		 *\~french
		 *\brief		Supprime une variable de ce tampon
		 *\param[in]	p_name	Le nom de la variable
		 */
		C3D_API void RemoveVariable( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Retrieves a variable by name
		 *\param[in]	p_name	The variable name
		 *\param[out]	p_variable	Receives the found variable, nullptr if failed
		 *\return		\p false if failed
		 *\~french
		 *\brief		Récupère une variable par son nom
		 *\param[in]	p_name	Le nom de la variable
		 *\param[out]	p_variable	Reçoit la variable récupérée, nullptr en cas d'échec
		 *\return		\p false en cas d'échec
		 */
		template< typename T >
		std::shared_ptr< OneFrameVariable< T > > GetVariable( Castor::String const & p_name, std::shared_ptr< OneFrameVariable< T > > & p_variable )const;
		/**
		 *\~english
		 *\brief		Retrieves a variable by name
		 *\param[in]	p_name	The variable name
		 *\param[out]	p_variable	Receives the found variable, nullptr if failed
		 *\return		\p false if failed
		 *\~french
		 *\brief		Récupère une variable par son nom
		 *\param[in]	p_name	Le nom de la variable
		 *\param[out]	p_variable	Reçoit la variable récupérée, nullptr en cas d'échec
		 *\return		\p false en cas d'échec
		 */
		template< typename T, uint32_t Count >
		std::shared_ptr< PointFrameVariable< T, Count > > GetVariable( Castor::String const & p_name, std::shared_ptr< PointFrameVariable< T, Count > > & p_variable )const;
		/**
		 *\~english
		 *\brief		Retrieves a variable by name
		 *\param[in]	p_name	The variable name
		 *\param[out]	p_variable	Receives the found variable, nullptr if failed
		 *\return		\p false if failed
		 *\~french
		 *\brief		Récupère une variable par son nom
		 *\param[in]	p_name	Le nom de la variable
		 *\param[out]	p_variable	Reçoit la variable récupérée, nullptr en cas d'échec
		 *\return		\p false en cas d'échec
		 */
		template< typename T, uint32_t Rows, uint32_t Columns >
		std::shared_ptr< MatrixFrameVariable< T, Rows, Columns > > GetVariable( Castor::String const & p_name, std::shared_ptr< MatrixFrameVariable< T, Rows, Columns > > & p_variable )const;
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
			return m_name;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the beginnning of the variables list
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la liste de variables
		 *\return		L'itérateur
		 */
		inline FrameVariablePtrList::iterator begin()
		{
			return m_listVariables.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the beginnning of the variables list
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la liste de variables
		 *\return		L'itérateur
		 */
		inline FrameVariablePtrList::const_iterator begin()const
		{
			return m_listVariables.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the variables list
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la liste de variables
		 *\return		L'itérateur
		 */
		inline FrameVariablePtrList::iterator end()
		{
			return m_listVariables.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the variables list
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la liste de variables
		 *\return		L'itérateur
		 */
		inline FrameVariablePtrList::const_iterator end()const
		{
			return m_listVariables.end();
		}

	protected:
		/**
		 *\~english
		 *\brief		Creates a variable of the wanted type
		 *\param[in]	p_type		The wanted type
		 *\param[in]	p_name	The variable name
		 *\param[in]	p_occurences	The array dimension
		 *\return		The created variable, nullptr if failed
		 *\~french
		 *\brief		Crée une variable du type demandé
		 *\param[in]	p_type		Le type voulu
		 *\param[in]	p_name	Le nom de la variable
		 *\param[in]	p_occurences	Les dimensions du tableau
		 *\return		La variable créée, nullptr en cas d'échec
		 */
		C3D_API virtual FrameVariableSPtr DoCreateVariable( ShaderProgram * p_program, eFRAME_VARIABLE_TYPE p_type, Castor::String const & p_name, uint32_t p_occurences = 1 ) = 0;
		/**
		 *\~english
		 *\brief		Initialises all the variables and the GPU buffer associated
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Initialise toutes les variables et le tampon GPU associé
		 *\param[in]	p_program	Le programme
		 *\return		\p false if any problem occured
		 */
		C3D_API virtual bool DoInitialise( ShaderProgram * p_program ) = 0;
		/**
		 *\~english
		 *\brief		Cleans all the variables up and the GPU buffer associated
		 *\~french
		 *\brief		Nettoie toutes les variables et le tampon GPU associé
		 */
		C3D_API virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Binds all the variables, through the GPU buffer if supported (OpenGL UBO, Direct3D Constants buffers)
		 *\param[in]	p_index	The buffer index
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Active toutes les variables, au traves du tampon GPU si supporté (OpenGL UBO, Direct3D Constants buffers)
		 *\param[in]	p_program	L'index du tampon GPU
		 *\return		\p false if any problem occured
		 */
		C3D_API virtual bool DoBind( uint32_t p_index ) = 0;
		/**
		 *\~english
		 *\brief		Unbinds all variables
		 *\param[in]	p_index	The buffer index
		 *\~french
		 *\brief		Désactive toutes les variables
		 *\param[in]	p_program	L'index du tampon GPU
		 */
		C3D_API virtual void DoUnbind( uint32_t p_index ) = 0;

	protected:
		//!\~english The buffers count	\~french Le compte des tampons
		static uint32_t sm_uiCount;
		//!\~english The buffer's index	\~french L'index du tampon
		uint32_t m_index;
		//!\~english The variables list	\~french La liste de variables
		FrameVariablePtrList m_listVariables;
		//!\~english The initialised variables list	\~french La liste de variables initialisées
		FrameVariablePtrList m_listInitialised;
		//!\~english The variables ordered by name	\~french Les variables, triées par nom
		FrameVariablePtrStrMap m_mapVariables;
		//!\~english The buffer name	\~french Le nom du tampon
		Castor::String m_name;
		//!\~english The data buffer	\~french Le tampon de données
		Castor::ByteArray m_buffer;
	};
}

#include "FrameVariableBuffer.inl"

#endif
