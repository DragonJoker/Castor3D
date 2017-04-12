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
#include "UniformBufferBinding.hpp"
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
		UniformBuffer( UniformBuffer const & ) = delete;
		UniformBuffer( UniformBuffer && ) = default;
		UniformBuffer & operator=( UniformBuffer const & ) = delete;
		UniformBuffer & operator=( UniformBuffer && ) = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name			The buffer name.
		 *\param[in]	p_renderSystem	The render system.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name			Le nom du tampon.
		 *\param[in]	p_renderSystem	Le render system.
		 */
		C3D_API UniformBuffer( Castor::String const & p_name, RenderSystem & p_renderSystem );
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
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Updates the GPU storage.
		 *\~french
		 *\brief		Met à jour le stockage GPU.
		 */
		C3D_API void Update()const;
		/**
		 *\~english
		 *\brief		Creates a binding of this unifor buffer to given program.
		 *\remarks		If this is the first binding created, the GPU storage and variables will be initialised.
		 *\param[in]	p_program	The program.
		 *\return		The created binding.
		 *\~french
		 *\brief		Crée un binding entre ce tampon d'uniformes et le porgramme donné.
		 *\remarks		Si c'est le premier binding créé, le stokage GPU et les variables seront initialisées.
		 *\param[in]	p_program	Le programme.
		 *\return		Le binding créé.
		 */
		C3D_API UniformBufferBinding & CreateBinding( ShaderProgram & p_program );
		/**
		 *\~english
		 *\brief		Retrieves the binding for given program.
		 *\param[in]	p_program	The program.
		 *\return		The retrieved binding.
		 *\~french
		 *\brief		Récupère le binding pour le programme donné.
		 *\param[in]	p_program	Le programme.
		 *\return		Le binding récupéré.
		 */
		C3D_API UniformBufferBinding & GetBinding( ShaderProgram & p_program )const;
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
		 *\return		The retrieved variable, \p nullptr if not found.
		 *\~french
		 *\brief		Récupère une variable par son nom.
		 *\param[in]	p_name		Le nom de la variable.
		 *\return		La variable récupérée, nullptr si non trouvée.
		 */
		template< UniformType Type >
		inline std::shared_ptr< TUniform< Type > > GetUniform( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Fills a UBO with matrix related variables.
		 *\param[in]	p_ubo	The UBO to fill.
		 *\~french
		 *\brief		Remplit un UBO avec les variable relatives aux matrices.
		 *\param[in]	p_ubo	L'UBO à remplir.
		 */
		C3D_API static void FillMatrixBuffer( UniformBuffer & p_ubo );
		/**
		 *\~english
		 *\brief		Fills a UBO with model matrix related variables.
		 *\param[in]	p_ubo	The UBO to fill.
		 *\~french
		 *\brief		Remplit un UBO avec les variable relatives aux matrices modèle.
		 *\param[in]	p_ubo	L'UBO à remplir.
		 */
		C3D_API static void FillModelMatrixBuffer( UniformBuffer & p_ubo );
		/**
		 *\~english
		 *\brief		Fills a UBO with scene related variables.
		 *\param[in]	p_ubo	The UBO to fill.
		 *\~french
		 *\brief		Remplit un UBO avec les variable relatives à la scène.
		 *\param[in]	p_ubo	L'UBO à remplir.
		 */
		C3D_API static void FillSceneBuffer( UniformBuffer & p_ubo );
		/**
		 *\~english
		 *\brief		Fills a UBO with pass related variables.
		 *\param[in]	p_ubo	The UBO to fill.
		 *\~french
		 *\brief		Remplit un UBO avec les variable relatives à la passe.
		 *\param[in]	p_ubo	L'UBO à remplir.
		 */
		C3D_API static void FillPassBuffer( UniformBuffer & p_ubo );
		/**
		 *\~english
		 *\brief		Fills a UBO with model related variables.
		 *\param[in]	p_ubo	The UBO to fill.
		 *\~french
		 *\brief		Remplit un UBO avec les variable relatives au modèle.
		 *\param[in]	p_ubo	L'UBO à remplir.
		 */
		C3D_API static void FillModelBuffer( UniformBuffer & p_ubo );
		/**
		 *\~english
		 *\brief		Fills a UBO with skinning animation related variables.
		 *\param[in]	p_ubo	The UBO to fill.
		 *\~french
		 *\brief		Remplit un UBO avec les variable relatives aux animations de skinning.
		 *\param[in]	p_ubo	L'UBO à remplir.
		 */
		C3D_API static void FillSkinningBuffer( UniformBuffer & p_ubo );
		/**
		 *\~english
		 *\brief		Fills a UBO with morphing animation related variables.
		 *\param[in]	p_ubo	The UBO to fill.
		 *\~french
		 *\brief		Remplit un UBO avec les variable relatives aux animations de morphing.
		 *\param[in]	p_ubo	L'UBO à remplir.
		 */
		C3D_API static void FillMorphingBuffer( UniformBuffer & p_ubo );
		/**
		 *\~english
		 *\brief		Fills a UBO with billboard related variables.
		 *\param[in]	p_ubo	The UBO to fill.
		 *\~french
		 *\brief		Remplit un UBO avec les variable relatives au billboard.
		 *\param[in]	p_ubo	L'UBO à remplir.
		 */
		C3D_API static void FillBillboardBuffer( UniformBuffer & p_ubo );
		/**
		 *\~english
		 *\brief		Fills a UBO with overlay related variables.
		 *\param[in]	p_ubo	The UBO to fill.
		 *\~french
		 *\brief		Remplit un UBO avec les variable relatives aux incrustations.
		 *\param[in]	p_ubo	L'UBO à remplir.
		 */
		C3D_API static void FillOverlayBuffer( UniformBuffer & p_ubo );
		/**
		 *\~english
		 *\return		The GPU storage.
		 *\~french
		 *\return		Le stockage GPU.
		 */
		inline GpuBuffer< uint8_t > & GetStorage()const
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
		 *\param[in]	p_binding	The binding from which the layout is retrieved.
		 *\~french
		 *\brief		Initialise toutes les variables et le tampon GPU associé.
		 *\param[in]	p_binding	Le binding depuis lequel le layout est récupéré.
		 */
		void DoInitialise( UniformBufferBinding const & p_binding );
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

	protected:
		//!\~english	The variables list.
		//!\~french		La liste de variables.
		UniformList m_listVariables;
		//!\~english	The variables ordered by name.
		//!\~french		Les variables, triées par nom.
		UniformMap m_mapVariables;
		//!\~english	The data buffer.
		//!\~french		Le tampon de données.
		Castor::ByteArray m_buffer;
		//!\~english	The GPU buffer.
		//!\~french		Le tampon GPU.
		std::unique_ptr< GpuBuffer< uint8_t > > m_storage;
		//!\~english	The bindings per program.
		//!\~french		Les bindings par programme.
		UniformBufferBindingMap m_bindings;
	};
}

#include "UniformBuffer.inl"

#endif
