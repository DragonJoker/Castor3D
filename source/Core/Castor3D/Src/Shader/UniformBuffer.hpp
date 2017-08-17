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

#include "Mesh/Buffer/GpuBuffer.hpp"
#include "Shader/UniformBufferBinding.hpp"
#include "Shader/Uniform/Uniform.hpp"

#include <Design/OwnedBy.hpp>
#include <Design/Named.hpp>

namespace castor3d
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
		: public castor::OwnedBy< RenderSystem >
		, public castor::Named
	{
		friend class castor::TextWriter< castor3d::UniformBuffer >;

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
			: public castor::TextWriter< UniformBuffer >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief			Writes a UniformBuffer into a text file.
			 *\param[in]		object	The UniformBuffer.
			 *\param[in,out]	file		The file.
			 *\~french
			 *\brief			Ecrit UniformBuffer dans un fichier texte.
			 *\param[in]		object	Le UniformBuffer.
			 *\param[in,out]	file		Le fichier.
			 */
			C3D_API bool operator()( UniformBuffer const & object, castor::TextFile & file )override;
		};

	public:
		UniformBuffer( UniformBuffer const & ) = delete;
		UniformBuffer( UniformBuffer && ) = default;
		UniformBuffer & operator=( UniformBuffer const & ) = delete;
		UniformBuffer & operator=( UniformBuffer && ) = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name			The buffer name.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	bindingPoint	The binding point.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name			Le nom du tampon.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	bindingPoint	Le point d'attache.
		 */
		C3D_API UniformBuffer( castor::String const & name
			, RenderSystem & renderSystem
			, uint32_t bindingPoint );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~UniformBuffer();
		/**
		 *\~english
		 *\brief		Cleans all the variables up and the GPU buffer associated.
		 *\~french
		 *\brief		Nettoie toutes les variables et le tampon GPU associé.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the GPU storage.
		 *\~french
		 *\brief		Met à jour le stockage GPU.
		 */
		C3D_API void update()const;
		/**
		 *\~english
		 *\brief		sets the buffer's binding point.
		 *\param[in]	index	The binding point.
		 *\~french
		 *\brief		Définit le point d'attache du tampon.
		 *\param[in]	index	Le point d'attache.
		 */
		C3D_API void bindTo( uint32_t index )const;
		/**
		 *\~english
		 *\return		The buffer's binding point.
		 *\~french
		 *\return		Le point d'attache du tampon.
		 */
		C3D_API uint32_t getBindingPoint()const;
		/**
		 *\~english
		 *\brief		Creates a binding of this unifor buffer to given program.
		 *\remarks		If this is the first binding created, the GPU storage and variables will be initialised.
		 *\param[in]	program	The program.
		 *\return		The created binding.
		 *\~french
		 *\brief		Crée un binding entre ce tampon d'uniformes et le porgramme donné.
		 *\remarks		Si c'est le premier binding créé, le stokage GPU et les variables seront initialisées.
		 *\param[in]	program	Le programme.
		 *\return		Le binding créé.
		 */
		C3D_API UniformBufferBinding & createBinding( ShaderProgram & program );
		/**
		 *\~english
		 *\brief		Retrieves the binding for given program.
		 *\param[in]	program	The program.
		 *\return		The retrieved binding.
		 *\~french
		 *\brief		Récupère le binding pour le programme donné.
		 *\param[in]	program	Le programme.
		 *\return		Le binding récupéré.
		 */
		C3D_API UniformBufferBinding & getBinding( ShaderProgram & program )const;
		/**
		 *\~english
		 *\brief		Creates a variable of the wanted type.
		 *\param[in]	type		The wanted type.
		 *\param[in]	name		The variable name.
		 *\param[in]	occurences	The array dimension.
		 *\return		The created variable, nullptr if failed.
		 *\~french
		 *\brief		Crée une variable du type demandé.
		 *\param[in]	type		Le type voulu.
		 *\param[in]	name		Le nom de la variable.
		 *\param[in]	occurences	Les dimensions du tableau.
		 *\return		La variable créée, nullptr en cas d'échec.
		 */
		C3D_API UniformSPtr createUniform( UniformType type
			, castor::String const & name
			, uint32_t occurences = 1 );
		/**
		 *\~english
		 *\brief		Creates a variable.
		 *\param[in]	name		The variable name.
		 *\param[in]	occurences	The array dimension.
		 *\return		The created variable, nullptr if failed.
		 *\~french
		 *\brief		Crée une variable.
		 *\param[in]	name		Le nom de la variable.
		 *\param[in]	occurences	Les dimensions du tableau.
		 *\return		La variable créée, nullptr en cas d'échec.
		 */
		template< UniformType Type >
		inline std::shared_ptr< typename UniformTypeTraits< Type >::type > createUniform( castor::String const & name
			, int occurences = 1 )
		{
			return std::static_pointer_cast< typename UniformTypeTraits< Type >::type >( createUniform( Type
				, name
				, occurences ) );
		}
		/**
		 *\~english
		 *\brief		Removes a variable from this buffer.
		 *\param[in]	name	The variable name.
		 *\~french
		 *\brief		Supprime une variable de ce tampon.
		 *\param[in]	name	Le nom de la variable.
		 */
		C3D_API void removeUniform( castor::String const & name );
		/**
		 *\~english
		 *\brief		Retrieves a variable by name.
		 *\param[in]	name	The variable name.
		 *\return		The retrieved variable, \p nullptr if not found.
		 *\~french
		 *\brief		Récupère une variable par son nom.
		 *\param[in]	name	Le nom de la variable.
		 *\return		La variable récupérée, nullptr si non trouvée.
		 */
		template< UniformType Type >
		inline std::shared_ptr< TUniform< Type > > getUniform( castor::String const & name )const;
		/**
		 *\~english
		 *\return		The GPU storage.
		 *\~french
		 *\return		Le stockage GPU.
		 */
		inline GpuBuffer & getStorage()const
		{
			REQUIRE( m_storage );
			return *m_storage;
		}
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

	private:
		/**
		 *\~english
		 *\brief		Initialises all the variables and the GPU buffer associated.
		 *\param[in]	binding	The binding from which the layout is retrieved.
		 *\~french
		 *\brief		Initialise toutes les variables et le tampon GPU associé.
		 *\param[in]	binding	Le binding depuis lequel le layout est récupéré.
		 */
		void doInitialise( UniformBufferBinding const & binding );
		/**
		 *\~english
		 *\brief		Creates a variable of the wanted type.
		 *\param[in]	type		The wanted type.
		 *\param[in]	name		The variable name.
		 *\param[in]	occurences	The array dimension.
		 *\return		The created variable, nullptr if failed.
		 *\~french
		 *\brief		Crée une variable du type demandé.
		 *\param[in]	type		Le type voulu.
		 *\param[in]	name		Le nom de la variable.
		 *\param[in]	occurences	Les dimensions du tableau.
		 *\return		La variable créée, nullptr en cas d'échec.
		 */
		UniformSPtr doCreateVariable( UniformType type
			, castor::String const & name
			, uint32_t occurences );

	protected:
		//!\~english	The variables list.
		//!\~french		La liste de variables.
		UniformList m_listVariables;
		//!\~english	The variables ordered by name.
		//!\~french		Les variables, triées par nom.
		UniformMap m_mapVariables;
		//!\~english	The data buffer.
		//!\~french		Le tampon de données.
		castor::ByteArray m_buffer;
		//!\~english	The GPU buffer.
		//!\~french		Le tampon GPU.
		GpuBufferUPtr m_storage;
		//!\~english	The bindings per program.
		//!\~french		Les bindings par programme.
		UniformBufferBindingMap m_bindings;
		//!\~english	The initial binding point.
		//!\~french		Le point d'attache initial.
		uint32_t m_bindingPoint{ 0u };
	};
}

#include "UniformBuffer.inl"

#endif
