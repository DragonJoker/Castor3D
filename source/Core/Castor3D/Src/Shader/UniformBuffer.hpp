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
#ifndef ___C3D_UniformBuffer_H___
#define ___C3D_UniformBuffer_H___

#include "Castor3DPrerequisites.hpp"

#include "Uniform.hpp"
#include "Mesh/Buffer/GpuBuffer.hpp"

#include <Design/OwnedBy.hpp>
#include <Design/Named.hpp>

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
	class UniformBuffer
		: public Castor::OwnedBy< RenderSystem >
		, public Castor::Named
	{
		friend class Castor::TextWriter< Castor3D::UniformBuffer >;

	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		UniformBuffer loader.
		\~french
		\brief		Loader de UniformBuffer.
		*/
		class TextWriter
			: public Castor::TextWriter< UniformBuffer >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief			Writes a UniformBuffer into a text file.
			 *\param[in]		p_object	The UniformBuffer.
			 *\param[in,out]	p_file		The file.
			 *\~french
			 *\brief			Ecrit UniformBuffer dans un fichier texte.
			 *\param[in]		p_object	Le UniformBuffer.
			 *\param[in,out]	p_file		Le fichier.
			 */
			C3D_API bool operator()( UniformBuffer const & p_object, Castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name			The buffer name.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_index			The binding index for the buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name			Le nom du tampon.
		 *\param[in]	p_renderSystem	Le render system.
		 *\param[in]	p_index			L'indice de binding du tampon.
		 */
		C3D_API UniformBuffer( Castor::String const & p_name, RenderSystem & p_renderSystem, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~UniformBuffer();
		/**
		 *\~english
		 *\brief		Initialises all the variables and the GPU buffer associated.
		 *\param[in]	p_program	The program.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Initialise toutes les variables et le tampon GPU associé.
		 *\param[in]	p_program	Le programme.
		 *\return		\p false if any problem occured.
		 */
		C3D_API bool Initialise( ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Cleans all the variables up and the GPU buffer associated.
		 *\~french
		 *\brief		Nettoie toutes les variables et le tampon GPU associé.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Binds the buffer to a program.
		 *\param[in]	p_program	The program.
		 *\~french
		 *\brief		Lie le tampon à un programme.
		 *\param[in]	p_program	Le programme.
		 */
		C3D_API void BindTo( ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Updates the GPU storage.
		 *\~french
		 *\brief		Met à jour le stockage GPU.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Creates a variable of the wanted type.
		 *\param[in]	p_type			The wanted type.
		 *\param[in]	p_name			The variable name.
		 *\param[in]	p_occurences	The array dimension.
		 *\return		The created variable, nullptr if failed.
		 *\~french
		 *\brief		Crée une variable du type demandé.
		 *\param[in]	p_program		Le programme.
		 *\param[in]	p_type			Le type voulu.
		 *\param[in]	p_name			Le nom de la variable.
		 *\param[in]	p_occurences	Les dimensions du tableau.
		 *\return		La variable créée, nullptr en cas d'échec.
		 */
		C3D_API UniformSPtr CreateUniform( UniformType p_type, Castor::String const & p_name, uint32_t p_occurences = 1 );
		/**
		 *\~english
		 *\brief		Creates a variable.
		 *\param[in]	p_name			The variable name.
		 *\param[in]	p_occurences	The array dimension.
		 *\return		The created variable, nullptr if failed.
		 *\~french
		 *\brief		Crée une variable.
		 *\param[in]	p_name			Le nom de la variable.
		 *\param[in]	p_occurences	Les dimensions du tableau.
		 *\return		La variable créée, nullptr en cas d'échec.
		 */
		template< UniformType Type >
		inline std::shared_ptr< typename uniform_type< Type >::type > CreateUniform( Castor::String const & p_name, int p_occurences = 1 )
		{
			return std::static_pointer_cast< typename uniform_type< Type >::type >( CreateUniform( Type, p_name, p_occurences ) );
		}
		/**
		 *\~english
		 *\brief		Removes a variable from this buffer.
		 *\param[in]	p_name	The variable name.
		 *\~french
		 *\brief		Supprime une variable de ce tampon.
		 *\param[in]	p_name	Le nom de la variable.
		 */
		C3D_API void RemoveUniform( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Retrieves a variable by name.
		 *\param[in]	p_name		The variable name.
		 *\param[out]	p_variable	Receives the found variable, nullptr if failed.
		 *\return		\p false if failed.
		 *\~french
		 *\brief		Récupère une variable par son nom.
		 *\param[in]	p_name		Le nom de la variable.
		 *\param[out]	p_variable	Reçoit la variable récupérée, nullptr en cas d'échec.
		 *\return		\p false en cas d'échec.
		 */
		template< UniformType Type >
		std::shared_ptr< TUniform< Type > > GetUniform( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\return		The iterator to the beginnning of the variables list.
		 *\~french
		 *\return		L'itérateur sur le début de la liste de variables.
		 */
		inline UniformList::iterator begin()
		{
			return m_listVariables.begin();
		}
		/**
		 *\~english
		 *\return		The iterator to the beginnning of the variables list.
		 *\~french
		 *\return		L'itérateur sur le début de la liste de variables.
		 */
		inline UniformList::const_iterator begin()const
		{
			return m_listVariables.begin();
		}
		/**
		 *\~english
		 *\return		The iterator to the end of the variables list.
		 *\~french
		 *\return		L'itérateur sur la fin de la liste de variables.
		 */
		inline UniformList::iterator end()
		{
			return m_listVariables.end();
		}
		/**
		 *\~english
		 *\return		The iterator to the end of the variables list.
		 *\~french
		 *\return		L'itérateur sur la fin de la liste de variables.
		 */
		inline UniformList::const_iterator end()const
		{
			return m_listVariables.end();
		}

	protected:
		/**
		 *\~english
		 *\brief		Creates a variable of the wanted type.
		 *\param[in]	p_type			The wanted type.
		 *\param[in]	p_name			The variable name.
		 *\param[in]	p_occurences	The array dimension.
		 *\return		The created variable, nullptr if failed.
		 *\~french
		 *\brief		Crée une variable du type demandé.
		 *\param[in]	p_type			Le type voulu.
		 *\param[in]	p_name			Le nom de la variable.
		 *\param[in]	p_occurences	Les dimensions du tableau.
		 *\return		La variable créée, nullptr en cas d'échec.
		 */
		UniformSPtr DoCreateVariable( UniformType p_type, Castor::String const & p_name, uint32_t p_occurences );
		/**
		 *\~english
		 *\brief		Initialises all the variables and the GPU buffer associated.
		 *\param[in]	p_program	The program.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Initialise toutes les variables et le tampon GPU associé.
		 *\param[in]	p_program	Le programme.
		 *\return		\p false if any problem occured.
		 */
		C3D_API virtual bool DoInitialise( ShaderProgram & p_program ) = 0;
		/**
		 *\~english
		 *\brief		Cleans all the variables up and the GPU buffer associated.
		 *\~french
		 *\brief		Nettoie toutes les variables et le tampon GPU associé.
		 */
		C3D_API virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Binds the buffer to a program.
		 *\param[in]	p_program	The program.
		 *\~french
		 *\brief		Lie le tampon à un programme.
		 *\param[in]	p_program	Le programme.
		 */
		C3D_API virtual void DoBindTo( ShaderProgram & p_program ) = 0;
		/**
		 *\~english
		 *\brief		Updates the GPU storage.
		 *\~french
		 *\brief		Met à jour le stockage GPU.
		 */
		C3D_API virtual void DoUpdate() = 0;

	protected:
		//!\~english	The buffer's index.
		//!\~french		L'index du tampon.
		uint32_t m_index{ 0u };
		//!\~english	The variables list.
		//!\~french		La liste de variables.
		UniformList m_listVariables;
		//!\~english	The variables ordered by name.
		//!\~french		Les variables, triées par nom.
		UniformMap m_mapVariables;
		//!\~english	The data buffer.
		//!\~french		Le tampon de données.
		Castor::ByteArray m_buffer;
	};
}

#include "UniformBuffer.inl"

#endif
