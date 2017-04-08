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
#ifndef ___C3D_SHADER_PROGRAM_H___
#define ___C3D_SHADER_PROGRAM_H___

#include "Castor3DPrerequisites.hpp"

#include "PushUniform.hpp"
#include "ProgramInputLayout.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"

#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	template< typename KeyType >
	struct ElementProducer< ShaderProgram, KeyType >
	{
		using Type = std::function< std::shared_ptr< ShaderProgram >( KeyType const & ) >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.2
	\date		24/01/2011
	\~english
	\brief		Base shader program implementation, used to share functions through the supported shader languages.
	\~french
	\brief		Implémentation de base d'un programme de shader, utilisé afin d'exposer les fonctions communes aux langages de shader.
	*/
	class ShaderProgram
		: public Castor::OwnedBy< RenderSystem >
		, public std::enable_shared_from_this< ShaderProgram >
	{
		friend class Castor::TextWriter< Castor3D::ShaderProgram >;

	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		ShaderProgram loader.
		\~french
		\brief		Loader de ShaderProgram.
		*/
		class TextWriter
			: public Castor::TextWriter< ShaderProgram >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API TextWriter( Castor::String const & p_tabs, Castor::String const & p_name = cuT( "shader_program" ) );
			/**
			 *\~english
			 *\brief			Writes a ShaderProgram into a text file.
			 *\param[in]		p_program	The ShaderProgram.
			 *\param[in,out]	p_file		The file.
			 *\~french
			 *\brief			Ecrit ShaderProgram dans un fichier texte.
			 *\param[in]		p_program	Le ShaderProgram.
			 *\param[in,out]	p_file		Le fichier.
			 */
			C3D_API bool operator()( ShaderProgram const & p_program, Castor::TextFile & p_file )override;

		private:
			Castor::String m_name;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderSystem	The RenderSystem instance.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderSystem	L'instance du RenderSystem.
		 */
		C3D_API explicit ShaderProgram( RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~ShaderProgram();
		/**
		 *\~english
		 *\brief		Creates the wanted shader object.
		 *\param[in]	p_type		The shader object concerned.
		 *\~french
		 *\brief		Crée le shader object voulu.
		 *\param[in]	p_type		Le shader object concerné.
		 */
		C3D_API ShaderObjectSPtr CreateObject( ShaderType p_type );
		/**
		 *\~english
		 *\brief		Sets all objects file.
		 *\param[in]	p_eModel	The shader model.
		 *\param[in]	p_pathFile	The file name.
		 *\~french
		 *\brief		Définit le fichier utilisé par tous les objets.
		 *\param[in]	p_eModel	Le modèle de shader.
		 *\param[in]	p_pathFile	Le nom du fichier.
		 */
		C3D_API void SetFile( ShaderModel p_eModel, Castor::Path const & p_pathFile );
		/**
		 *\~english
		 *\brief		Creates a shader storage buffer.
		 *\param[in]	p_name			The buffer name.
		 *\param[in]	p_shaderMask	ShaderTypeFlag combination, to set at what shaders it is to be bound.
		 *\return		The created or retrieved Frame variable buffer.
		 *\~french
		 *\brief		Crée un tampon de stockage.
		 *\param[in]	p_name			Le nom du tampon.
		 *\param[in]	p_shaderMask	Combinaison de ShaderTypeFlag, pour déterminer les shaders auxquels il doit être lié.
		 *\return		Le tampon de stockage créé ou récupéré.
		 */
		C3D_API ShaderStorageBuffer & CreateStorageBuffer( Castor::String const & p_name, ShaderTypeFlags const & p_shaderMask );
		/**
		 *\~english
		 *\brief		Finds a storage buffer.
		 *\param[in]	p_name	The buffer name.
		 *\return		The found buffer, nullptr if failed.
		 *\~french
		 *\brief		Trouve un tampon de stockage.
		 *\param[in]	p_name	Le nom du tampon.
		 *\return		Le tampon trouvé, nullptr en cas d'échec.
		 */
		C3D_API ShaderStorageBufferSPtr FindStorageBuffer( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Creates an atomic counter buffer.
		 *\param[in]	p_name			The buffer name.
		 *\param[in]	p_shaderMask	ShaderTypeFlag combination, to set at what shaders it is to be bound.
		 *\return		The created or retrieved Frame variable buffer.
		 *\~french
		 *\brief		Crée un tampon de compteurs atomiques.
		 *\param[in]	p_name			Le nom du tampon.
		 *\param[in]	p_shaderMask	Combinaison de ShaderTypeFlag, pour déterminer les shaders auxquels il doit être lié.
		 *\return		Le tampon de stockage créé ou récupéré.
		 */
		C3D_API AtomicCounterBuffer & CreateAtomicCounterBuffer( Castor::String const & p_name, ShaderTypeFlags const & p_shaderMask );
		/**
		 *\~english
		 *\brief		Finds an atomic counter buffer.
		 *\param[in]	p_name	The buffer name.
		 *\return		The found buffer, nullptr if failed.
		 *\~french
		 *\brief		Trouve un tampon de compteurs atomiques.
		 *\param[in]	p_name	Le nom du tampon.
		 *\return		Le tampon trouvé, nullptr en cas d'échec.
		 */
		C3D_API AtomicCounterBufferSPtr FindAtomicCounterBuffer( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Resets compilation variables to be able to compile again.
		 *\~french
		 *\brief		Réinitialise les variables de compilation afin de pouvoir compiler le shader à nouveau.
		 */
		C3D_API void ResetToCompile();
		/**
		 *\~english
		 *\brief		Sets the input primitives type.
		 *\param[in]	p_target	The shader object concerned.
		 *\param[in]	p_topology	The input primitives type.
		 *\~french
		 *\brief		Définit le type des primitives en entrée.
		 *\param[in]	p_target	Le shader object concerné.
		 *\param[in]	p_topology	Le type des primitives en entrée.
		 */
		C3D_API void SetInputType( ShaderType p_target, Topology p_topology );
		/**
		 *\~english
		 *\brief		Sets the output primitives type.
		 *\param[in]	p_target	The shader object concerned.
		 *\param[in]	p_topology	The output primitives type.
		 *\~french
		 *\brief		Définit le type des primitives en sortie.
		 *\param[in]	p_target	Le shader object concerné.
		 *\param[in]	p_topology	Le type des primitives en sortie.
		 */
		C3D_API void SetOutputType( ShaderType p_target, Topology p_topology );
		/**
		 *\~english
		 *\brief		Sets the output vertex count.
		 *\param[in]	p_target	The shader object concerned.
		 *\param[in]	p_count	The count.
		 *\~french
		 *\brief		Définit le nombre de vertices générés.
		 *\param[in]	p_target	Le shader object concerné.
		 *\param[in]	p_count	Le compte.
		 */
		C3D_API void SetOutputVtxCount( ShaderType p_target, uint8_t p_count );
		/**
		 *\~english
		 *\brief		Sets the shader file for given model.
		 *\remarks		The loaded file will be the one of the highest supported profile.
		 *\param[in]	p_target	The shader object concerned.
		 *\param[in]	p_eModel	The shader model.
		 *\param[in]	p_pathFile	The file name.
		 *\~french
		 *\brief		Définit le fichier du shader pour le modèle donné.
		 *\remarks		Le fichier chargé sera celui du plus haut profil supporté.
		 *\param[in]	p_target	Le shader object concerné.
		 *\param[in]	p_eModel	Le modèle de shader.
		 *\param[in]	p_pathFile	Le nom du fichier.
		 */
		C3D_API void SetFile( ShaderType p_target, ShaderModel p_eModel, Castor::Path const & p_pathFile );
		/**
		 *\~english
		 *\brief		Retrieves the shader file for given model.
		 *\param[in]	p_target	The shader object concerned.
		 *\param[in]	p_eModel	The shader model.
		 *\return		The file name.
		 *\~french
		 *\brief		Récupère le fichier du shader pour le modèle donné.
		 *\param[in]	p_target	Le shader object concerné.
		 *\param[in]	p_eModel	Le modèle de shader.
		 *\return		Le nom du fichier.
		 */
		C3D_API Castor::Path GetFile( ShaderType p_target, ShaderModel p_eModel )const;
		/**
		 *\~english
		 *\brief		Tells if the shader object has a source file, whatever model it is.
		 *\param[in]	p_target	The shader object concerned.
		 *\return		\p true if the shader object has a source file.
		 *\~french
		 *\brief		Dit si le shader a un fichier source, quel que soit son modèle.
		 *\param[in]	p_target	Le shader object concerné.
		 *\return		\p true si le shader a un fichier source.
		 */
		C3D_API bool HasFile( ShaderType p_target )const;
		/**
		 *\~english
		 *\brief		Sets the shader source for given model.
		 *\remarks		The loaded source will be the one of the highest supported profile.
		 *\param[in]	p_target	The shader object concerned.
		 *\param[in]	p_eModel	The shader model.
		 *\param[in]	p_strSource	The source code.
		 *\~french
		 *\brief		Définit la source du shader pour le modèle donné.
		 *\remarks		La source chargée sera celle du plus haut profil supporté.
		 *\param[in]	p_target	Le shader object concerné.
		 *\param[in]	p_eModel	Le modèle de shader.
		 *\param[in]	p_strSource	Le code de la source.
		 */
		C3D_API void SetSource( ShaderType p_target, ShaderModel p_eModel, Castor::String const & p_strSource );
		/**
		 *\~english
		 *\brief		Retrieves the shader source for given model.
		 *\param[in]	p_target	The shader object concerned.
		 *\param[in]	p_eModel	The shader model.
		 *\return		The source code.
		 *\~french
		 *\brief		Récupère la source du shader pour le modèle donné.
		 *\param[in]	p_target	Le shader object concerné.
		 *\param[in]	p_eModel	Le modèle de shader.
		 *\return		Le code de la source.
		 */
		C3D_API Castor::String GetSource( ShaderType p_target, ShaderModel p_eModel )const;
		/**
		 *\~english
		 *\brief		Tells if the shader object has a source code, whatever model it is.
		 *\param[in]	p_target	The shader object concerned.
		 *\return		\p true if the shader object has a source code.
		 *\~french
		 *\brief		Dit si le shader a un code source, quel que soit son modèle.
		 *\param[in]	p_target	Le shader object concerné.
		 *\return		\p true si le shader a un code source.
		 */
		C3D_API bool HasSource( ShaderType p_target )const;
		/**
		 *\~english
		 *\brief		Tests if the program has an object for given type.
		 *\param[in]	p_target	The shader object type.
		 *\return		\p true if the shader object exists.
		 *\~french
		 *\brief		Dit si le programme a un shader du type donné.
		 *\param[in]	p_target	Le type du shader object.
		 *\return		\p true si le shader object existe.
		 */
		C3D_API bool HasObject( ShaderType p_target )const;
		/**
		 *\~english
		 *\return		The given object type status.
		 *\~french
		 *\return		Le statut du type d'objet donné.
		 */
		C3D_API ShaderStatus GetObjectStatus( ShaderType p_target )const;
		/**
		 *\~english
		 *\brief		Creates a variable.
		 *\param[in]	p_type		The variable type.
		 *\param[in]	p_name		The variable name.
		 *\param[in]	p_shader	The shader type.
		 *\param[in]	p_nbOcc		The array dimension.
		 *\return		The created variable, nullptr if failed.
		 *\~french
		 *\brief		Crée une variable
		 *\param[in]	p_type		Le type de variable.
		 *\param[in]	p_name		Le nom de la variable.
		 *\param[in]	p_shader	Le type du shader.
		 *\param[in]	p_nbOcc		Les dimensions du tableau.
		 *\return		La variable créée, nullptr en cas d'échec.
		 */
		C3D_API PushUniformSPtr CreateUniform( UniformType p_type, Castor::String const & p_name, ShaderType p_shader, int p_nbOcc = 1 );
		/**
		 *\~english
		 *\brief		Creates a variable.
		 *\param[in]	p_name		The variable name.
		 *\param[in]	p_shader	The shader type.
		 *\param[in]	p_nbOcc		The array dimension.
		 *\return		The created variable, nullptr if failed.
		 *\~french
		 *\brief		Crée une variable.
		 *\param[in]	p_name		Le nom de la variable.
		 *\param[in]	p_shader	Le type du shader.
		 *\param[in]	p_nbOcc		Les dimensions du tableau.
		 *\return		La variable créée, nullptr en cas d'échec.
		 */
		template< UniformType Type >
		inline std::shared_ptr< TPushUniform< Type > > CreateUniform( Castor::String const & p_name, ShaderType p_shader, int p_nbOcc = 1 )
		{
			return std::static_pointer_cast< TPushUniform< Type > >( CreateUniform( Type, p_name, p_shader, p_nbOcc ) );
		}
		/**
		 *\~english
		 *\brief		Looks for a variable.
		 *\param[in]	p_type		The variable type.
		 *\param[in]	p_name		The variable name.
		 *\param[in]	p_shader	The shader type.
		 *\return		The found variable, nullptr if failed.
		 *\~french
		 *\brief		Cherche une variable.
		 *\param[in]	p_type		Le type de variable.
		 *\param[in]	p_name		Le nom de la variable.
		 *\param[in]	p_shader	Le type du shader.
		 *\return		La variable trouvée, nullptr en cas d'échec.
		 */
		C3D_API PushUniformSPtr FindUniform( UniformType p_type, Castor::String const & p_name, ShaderType p_shader )const;
		/**
		 *\~english
		 *\brief		Looks for a variable.
		 *\param[in]	p_name		The variable name.
		 *\param[in]	p_shader	The shader type.
		 *\return		The found variable, nullptr if failed.
		 *\~french
		 *\brief		Cherche une variable.
		 *\param[in]	p_name		Le nom de la variable.
		 *\param[in]	p_shader	Le type du shader.
		 *\return		La variable trouvé, nullptr en cas d'échec.
		 */
		template< UniformType Type >
		inline std::shared_ptr< TPushUniform< Type > > FindUniform( Castor::String const & p_name, ShaderType p_shader )const
		{
			return std::static_pointer_cast< TPushUniform< Type > >( FindUniform( Type, p_name, p_shader ) );
		}
		/**
		 *\~english
		 *\brief		Retrieves the frame variables buffer bound to one shader type.
		 *\param[in]	p_type	The shader type.
		 *\return		The list.
		 *\~french
		 *\brief		Récupère les variables de frame liées à un type de shader particulier.
		 *\param[in]	p_type	The shader type.
		 *\return		La liste.
		 */
		C3D_API PushUniformList & GetUniforms( ShaderType p_type );
		/**
		 *\~english
		 *\brief		Retrieves the frame variables bound to one shader type.
		 *\param[in]	p_type	The shader type.
		 *\return		The list.
		 *\~french
		 *\brief		Récupère les variables de frame liées à un type de shader particulier.
		 *\param[in]	p_type	The shader type.
		 *\return		La liste.
		 */
		C3D_API PushUniformList const & GetUniforms( ShaderType p_type )const;
		/**
		 *\~english
		 *\brief		Cleans the program up.
		 *\~french
		 *\brief		Nettoie le programme.
		 */
		C3D_API virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Initialises the program.
		 *\~french
		 *\brief		Initialise le programme.
		 */
		C3D_API virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Activates the program.
		 *\~french
		 *\brief		Active le programme.
		 */
		C3D_API virtual void Bind()const = 0;
		/**
		 *\~english
		 *\brief		Deactivates the program.
		 *\~french
		 *\brief		Désactive le programme.
		 */
		C3D_API virtual void Unbind()const = 0;
		/**
		 *\~english
		 *\brief		Links all shader objects held by the program.
		 *\~french
		 *\brief		Link tous les objets du programme.
		 */
		C3D_API virtual bool Link() = 0;
		/**
		 *\~english
		 *\return		The program vertex layout.
		 *\~french
		 *\return		Le layout des sommets du programme.
		 */
		C3D_API virtual ProgramInputLayout const & GetLayout()const = 0;
		/**
		 *\~english
		 *\return		The program vertex layout.
		 *\~french
		 *\return		Le layout des sommets du programme.
		 */
		C3D_API virtual ProgramInputLayout & GetLayout() = 0;
		/**
		 *\~english
		 *\brief		Sets the transform feedback layout.
		 *\param[in]	p_declaration	The layout.
		 *\~french
		 *\brief		Définit le layout de transform feedback.
		 *\param[in]	p_declaration	Le layout.
		 */
		inline void SetTransformLayout( BufferDeclaration const & p_declaration )
		{
			m_declaration = p_declaration;
		}
		/**
		 *\~english
		 *\brief		Retrieves the storage buffers bound to one shader type
		 *\param[in]	p_type	The shader type
		 *\return		The list
		 *\~french
		 *\brief		Récupère les tampons de stockage liés à un type de shader particulier
		 *\param[in]	p_type	The shader type
		 *\return		La liste
		 */
		inline ShaderStorageBufferPtrList & GetShaderStorageBuffers( ShaderType p_type )
		{
			return m_storageBuffers[size_t( p_type )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the storage buffers bound to one shader type
		 *\param[in]	p_type	The shader type
		 *\return		The list
		 *\~french
		 *\brief		Récupère les tampons de stockage liés à un type de shader particulier
		 *\param[in]	p_type	The shader type
		 *\return		La liste
		 */
		inline ShaderStorageBufferPtrList const & GetShaderStorageBuffers( ShaderType p_type )const
		{
			return m_storageBuffers[size_t( p_type )];
		}
		/**
		 *\~english
		 *\return		The storage buffers list.
		 *\~french
		 *\return		La liste de tampons de stockage.
		 */
		inline ShaderStorageBufferPtrList & GetShaderStorageBuffers()
		{
			return m_listStorageBuffers;
		}
		/**
		 *\~english
		 *\return		The storage buffers list.
		 *\~french
		 *\return		La liste de tampons de stockage.
		 */
		inline ShaderStorageBufferPtrList const & GetShaderStorageBuffers()const
		{
			return m_listStorageBuffers;
		}
		/**
		 *\~english
		 *\brief		Retrieves the atomic counter buffers bound to one shader type
		 *\param[in]	p_type	The shader type
		 *\return		The list
		 *\~french
		 *\brief		Récupère les tampons de compteurs atomiques liés à un type de shader particulier
		 *\param[in]	p_type	The shader type
		 *\return		La liste
		 */
		inline AtomicCounterBufferPtrList & GetAtomicCounterBuffers( ShaderType p_type )
		{
			return m_atomicCounterBuffers[size_t( p_type )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the atomic counter buffers bound to one shader type
		 *\param[in]	p_type	The shader type
		 *\return		The list
		 *\~french
		 *\brief		Récupère les tampons de compteurs atomiques liés à un type de shader particulier
		 *\param[in]	p_type	The shader type
		 *\return		La liste
		 */
		inline AtomicCounterBufferPtrList const & GetAtomicCounterBuffers( ShaderType p_type )const
		{
			return m_atomicCounterBuffers[size_t( p_type )];
		}
		/**
		 *\~english
		 *\return		The atomic counter buffers list.
		 *\~french
		 *\return		La liste de tampons de compteurs atomiques.
		 */
		inline AtomicCounterBufferPtrList & GetAtomicCounterBuffers()
		{
			return m_listAtomicCounterBuffers;
		}
		/**
		 *\~english
		 *\return		The atomic counter buffers list.
		 *\~french
		 *\return		La liste de tampons de compteurs atomiques.
		 */
		inline AtomicCounterBufferPtrList const & GetAtomicCounterBuffers()const
		{
			return m_listAtomicCounterBuffers;
		}
		/**
		 *\~english
		 *\brief		Retrieves the program link status.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le statut de link du programme.
		 *\return		La valeur.
		 */
		inline ProgramStatus GetStatus()const
		{
			return m_status;
		}

	protected:
		/**
		 *\~english
		 *\brief		Cleans the program up.
		 *\~french
		 *\brief		Nettoie le programme.
		 */
		C3D_API void DoCleanup();
		/**
		 *\~english
		 *\brief		Initialises the program.
		 *\~french
		 *\brief		Initialise le programme.
		 */
		C3D_API bool DoInitialise();
		/**
		 *\~english
		 *\brief		Activates the program.
		 *\~french
		 *\brief		Active le programme.
		 */
		C3D_API void DoBind()const;
		/**
		 *\~english
		 *\brief		Deactivates the program.
		 *\~french
		 *\brief		Désactive le programme.
		 */
		C3D_API void DoUnbind()const;
		/**
		 *\~english
		 *\brief		Links all shader objects held by the program.
		 *\~french
		 *\brief		Link tous les objets du programme.
		 */
		C3D_API bool DoLink();

	private:
		/**
		 *\~english
		 *\brief		Creates the wanted shader object.
		 *\param[in]	p_type	The shader object concerned.
		 *\~french
		 *\brief		Crée le shader object voulu.
		 *\param[in]	p_type	Le shader object concerné.
		 */
		virtual ShaderObjectSPtr DoCreateObject( ShaderType p_type ) = 0;
		/**
		 *\~english
		 *\brief		Creates a texture frame variable.
		 *\param[in]	p_iNbOcc	The array dimension.
		 *\return		The created variable, nullptr if failed.
		 *\~french
		 *\brief		Crée une variable de frame texture.
		 *\param[in]	p_iNbOcc	Les dimensions du tableau.
		 *\return		La variable créée, nullptr en cas d'échec.
		 */
		virtual PushUniformSPtr DoCreateUniform( UniformType p_type, int p_iNbOcc ) = 0;

	public:
		/**@name Attributes */
		//@{

		//!\~english	Name of the position attribute.
		//!\~french		Nom de l'attribut position.
		C3D_API static const Castor::String Position;
		//!\~english	Name of the normal attribute.
		//!\~french		Nom de l'attribut normale.
		C3D_API static const Castor::String Normal;
		//!\~english	Name of the tangent attribute.
		//!\~french		Nom de l'attribut tangente.
		C3D_API static const Castor::String Tangent;
		//!\~english	Name of the bitangent attribute.
		//!\~french		Nom de l'attribut bitangente.
		C3D_API static const Castor::String Bitangent;
		//!\~english	Name of the texture attribute.
		//!\~french		Nom du de l'attribut texture.
		C3D_API static const Castor::String Texture;
		//!\~english	Name of the colour attribute.
		//!\~french		Nom du de l'attribut couleur.
		C3D_API static const Castor::String Colour;
		//!\~english	Name of the position attribute for per-vertex animations.
		//!\~french		Nom de l'attribut position pour les animations par sommet.
		C3D_API static const Castor::String Position2;
		//!\~english	Name of the normal attribute for per-vertex animations.
		//!\~french		Nom de l'attribut normale pour les animations par sommet.
		C3D_API static const Castor::String Normal2;
		//!\~english	Name of the tangent attribute for per-vertex animations.
		//!\~french		Nom de l'attribut tangente pour les animations par sommet.
		C3D_API static const Castor::String Tangent2;
		//!\~english	Name of the bitangent attribute for per-vertex animations.
		//!\~french		Nom de l'attribut bitangente pour les animations par sommet.
		C3D_API static const Castor::String Bitangent2;
		//!\~english	Name of the texture attribute for per-vertex animations.
		//!\~french		Nom du de l'attribut texture pour les animations par sommet.
		C3D_API static const Castor::String Texture2;
		//!\~english	Name of the colour attribute for per-vertex animations.
		//!\~french		Nom du de l'attribut couleur pour les animations par sommet.
		C3D_API static const Castor::String Colour2;
		//!\~english	Name of the text overlay texture attribute.
		//!\~french		Nom du de l'attribut texture pour les incrustations texte.
		C3D_API static const Castor::String Text;
		//!\~english	Name of the first bones ID attribute.
		//!\~french		Nom du premier attribut d'ID des bones.
		C3D_API static const Castor::String BoneIds0;
		//!\~english	Name of the second bones ID attribute.
		//!\~french		Nom du second attribut d'ID des bones.
		C3D_API static const Castor::String BoneIds1;
		//!\~english	Name of the first bones weight attribute.
		//!\~french		Nom du premier attribut de poids des bones.
		C3D_API static const Castor::String Weights0;
		//!\~english	Name of the second bones weight attribute.
		//!\~french		Nom du second attribut de poids des bones.
		C3D_API static const Castor::String Weights1;
		//!\~english	Name of the instance transform attribute.
		//!\~french		Nom de l'attribut de transformation d'instance.
		C3D_API static const Castor::String Transform;

		//@}
		/**@name Scene */
		//@{

		//!\~english	Name of the camera position frame variable.
		//!\~french		Nom de la frame variable contenant la position de la caméra.
		C3D_API static const Castor::String CameraPos;
		//!\~english	Name of the camera far plane frame variable.
		//!\~french		Nom de la frame variable contenant la valeur du plan éloigné de la caméra.
		C3D_API static const Castor::String CameraFarPlane;
		//!\~english	Name of the ambient light frame variable.
		//!\~french		Nom de la frame variable contenant la lumière ambiante.
		C3D_API static const Castor::String AmbientLight;
		//!\~english	Name of the background colour frame variable.
		//!\~french		Nom de la frame variable contenant la couleur de fond.
		C3D_API static const Castor::String BackgroundColour;
		//!\~english	Name of the fog type frame variable.
		//!\~french		Nom de la frame variable contenant le type de brouillard.
		C3D_API static const Castor::String FogType;
		//!\~english	Name of the fog's density frame variable.
		//!\~french		Nom de la frame variable contenant la densité du brouillard.
		C3D_API static const Castor::String FogDensity;
		//!\~english	Name of the lights count frame variable.
		//!\~french		Nom de la frame variable contenant le compte des lumières.
		C3D_API static const Castor::String LightsCount;
		//!\~english	Name of the lights frame variable.
		//!\~french		Nom de la frame variable contenant les lumières.
		C3D_API static const Castor::String Lights;

		//@}
		/**@name Pass */
		//@{

		//!\~english	Name of the diffuse material colour frame variable.
		//!\~french		Nom de la frame variable contenant la couleur diffuse du matériau.
		C3D_API static const Castor::String MatDiffuse;
		//!\~english	Name of the specular material colour frame variable.
		//!\~french		Nom de la frame variable contenant la couleur spéculaire du matériau.
		C3D_API static const Castor::String MatSpecular;
		//!\~english	Name of the emissive material colour frame variable.
		//!\~french		Nom de la frame variable contenant la couleur émissive du matériau.
		C3D_API static const Castor::String MatEmissive;
		//!\~english	Name of the material shininess frame variable.
		//!\~french		Nom de la frame variable contenant l'exposante du matériau.
		C3D_API static const Castor::String MatShininess;
		//!\~english	Name of the material opacity frame variable.
		//!\~french		Nom de la frame variable contenant l'opacité du matériau.
		C3D_API static const Castor::String MatOpacity;
		//!\~english	Name of the material environment index frame variable.
		//!\~french		Nom de la frame variable contenant l'indice de la map d'environnement du matériau.
		C3D_API static const Castor::String MatEnvironmentIndex;
		//!\~english	Name of the material refraction ratio frame variable.
		//!\~french		Nom de la frame variable contenant le ratio de réfraction du matériau.
		C3D_API static const Castor::String MatRefractionRatio;
		//!\~english	Name of the shadow map image frame variable.
		//!\~french		Nom de la frame variable contenant l'image de shadow map.
		C3D_API static const Castor::String MapShadow;
		//!\~english	Name of the overlay text image frame variable.
		//!\~french		Nom de la frame variable contenant l'image de texte pour les overlays.
		C3D_API static const Castor::String MapText;

		//@}
		/**@name Model */
		//@{

		//!\~english	Name of the shadow receiver status frame variable.
		//!\~french		Nom de la frame variable contenant le statut de réception d'ombres.
		C3D_API static const Castor::String ShadowReceiver;

		//@}
		/**@name Animation */
		//@{

		//!\~english	Name of the bones matrices attribute.
		//!\~french		Nom de l'attribut de de matrices d'os.
		C3D_API static const Castor::String Bones;
		//!\~english	Name of the morphing time attribute.
		//!\~french		Nom de l'attribut du temps d'animation par sommet.
		C3D_API static const Castor::String Time;

		//@}
		/**@name Billboard */
		//@{

		//!\~english	Name of the billboard dimensions frame variable.
		//!\~french		Nom de la frame variable contenant les dimensions du billboard.
		C3D_API static const Castor::String Dimensions;
		//!\~english	Name of the window dimensions frame variable.
		//!\~french		Nom de la frame variable contenant les dimensions de la fenêtre.
		C3D_API static const Castor::String WindowSize;

		//@}
		/**@name Overlay */
		//@{

		//!\~english	Name of the overlay position frame variable.
		//!\~french		Nom de la frame variable contenant la position de l'incrustation.
		C3D_API static const Castor::String OvPosition;

		//@}
		/**@name Textures */
		//@{

		//!\~english	Name of the diffuse texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture de couleur diffuse.
		C3D_API static const Castor::String MapDiffuse;
		//!\~english	Name of the specular texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture de couleur spéculaire.
		C3D_API static const Castor::String MapSpecular;
		//!\~english	Name of the emissive texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture de couleur émissive.
		C3D_API static const Castor::String MapEmissive;
		//!\~english	Name of the emissive texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture de normales.
		C3D_API static const Castor::String MapNormal;
		//!\~english	Name of the opacity texture frame variable.
		//!\~french		Nom de la frame variable contenant texture d'opacité.
		C3D_API static const Castor::String MapOpacity;
		//!\~english	Name of the shininess texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture d'exposante.
		C3D_API static const Castor::String MapGloss;
		//!\~english	Name of the height texture frame variable.
		//!\~french		Nom de la frame variable contenant texture de hauteur.
		C3D_API static const Castor::String MapHeight;
		//!\~english	Name of the environment texture frame variable.
		//!\~french		Nom de la frame variable contenant la texture d'environnement.
		C3D_API static const Castor::String MapEnvironment;

		//@}
		/**@name Frame Variable Buffers */
		//@{

		//!\~english	Name of the matrix frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les matrices.
		C3D_API static const Castor::String BufferMatrix;
		//!\~english	Name of the model matrix frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les matrices modèle.
		C3D_API static const Castor::String BufferModelMatrix;
		//!\~english	Name of the scene frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données de scène.
		C3D_API static const Castor::String BufferScene;
		//!\~english	Name of the pass frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données de passe.
		C3D_API static const Castor::String BufferPass;
		//!\~english	Name of the model frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données de modèle.
		C3D_API static const Castor::String BufferModel;
		//!\~english	Name of the billboards frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données de billboards.
		C3D_API static const Castor::String BufferBillboards;
		//!\~english	Name of the skinning animation frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données d'animation de skinning.
		C3D_API static const Castor::String BufferSkinning;
		//!\~english	Name of the morphing animation frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données d'animation de morphing.
		C3D_API static const Castor::String BufferMorphing;
		//!\~english	Name of the overlay information frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les informations de l'incrustation.
		C3D_API static const Castor::String BufferOverlay;

		//@}


	protected:
		//!<\~english	The program status.
		//!\~french		Le statut du programme.
		ProgramStatus m_status{ ProgramStatus::eNotLinked };
		//!\~english	The shaders array.
		//!\~french		Le tableau de shaders.
		std::array< ShaderObjectSPtr, size_t( ShaderType::eCount ) > m_shaders;
		//!\~english	The active shaders array.
		//!\~french		Le tableau de shaders actifs.
		std::vector< ShaderObjectSPtr > m_activeShaders;
		//!\~english	Array of files path, sorted by shader model.
		//!\~french		Tableau des chemins de fichiers, triés par modèle de shader.
		std::array< Castor::Path, size_t( ShaderModel::eCount ) > m_arrayFiles;
		//!\~english	The storage buffers map, ordered by name.
		//!\~french		La liste des tampons de stockage, triés par nom.
		ShaderStorageBufferPtrStrMap m_storageBuffersByName;
		//!\~english	The storage buffers map, ordered by shader type.
		//!\~french		La liste des tampons de stockage, triés par type de shader.
		std::array< ShaderStorageBufferPtrList, size_t( ShaderType::eCount ) > m_storageBuffers;
		//!\~english	The storage buffers map.
		//!\~french		La liste des tampons de stockage.
		ShaderStorageBufferPtrList m_listStorageBuffers;
		//!\~english	The atomic counter buffers map, ordered by name.
		//!\~french		La liste des tampons de compteurs atomiques, triés par nom.
		AtomicCounterBufferPtrStrMap m_atomicCounterBuffersByName;
		//!\~english	The atomic counter buffers map, ordered by shader type.
		//!\~french		La liste des tampons de compteurs atomiques, triés par type de shader.
		std::array< AtomicCounterBufferPtrList, size_t( ShaderType::eCount ) > m_atomicCounterBuffers;
		//!\~english	The atomic counter buffers map.
		//!\~french		La liste des tampons de compteurs atomiques.
		AtomicCounterBufferPtrList m_listAtomicCounterBuffers;
		//!\~english	The transform feedback layout.
		//!\~french		Le layout de transform feedback.
		BufferDeclaration m_declaration;
	};
}

#endif
