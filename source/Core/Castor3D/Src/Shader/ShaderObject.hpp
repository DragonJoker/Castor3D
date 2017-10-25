/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADER_OBJECT_H___
#define ___C3D_SHADER_OBJECT_H___

#include "Castor3DPrerequisites.hpp"

#include <GlslShader.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Base shader class
	\remark		Used to share functions through the supported shader languages
	\~french
	\brief		Classe de base pour les shaders
	\remark		Utilisée pour exposer les fonctions communes aux différents langages de shader
	*/
	class ShaderObject
	{
	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 19/10/2011
		\~english
		\brief ShaderObject loader
		\~french
		\brief Loader de ShaderObject
		*/
		class TextWriter
			: public castor::TextWriter< ShaderObject >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief			Writes a ShaderObject into a text file
			 *\param[in]		shaderObject	The ShaderObject
			 *\param[in,out]	file			The file
			 *\~french
			 *\brief			Ecrit un ShaderObject dans un fichier texte
			 *\param[in]		shaderObject	Le ShaderObject
			 *\param[in,out]	file			Le fichier
			 */
			C3D_API bool operator()( ShaderObject const & shaderObject, castor::TextFile & file )override;
		};

	private:
		static const std::array< castor::String, size_t( ShaderType::eCount ) > string_type;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	parent	Parent program
		 *\param[in]	type	Shader type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent	programme parent
		 *\param[in]	type	Type de shader
		 */
		C3D_API ShaderObject( ShaderProgram & parent, ShaderType type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~ShaderObject();
		/**
		 *\~english
		 *\brief		Creates the program on GPU
		 *\~french
		 *\brief		Crée le programme sur le GPU
		 */
		C3D_API virtual bool create() = 0;
		/**
		 *\~english
		 *\brief		Destroys the program on GPU
		 *\~french
		 *\brief		Détruit le programme sur le GPU
		 */
		C3D_API virtual void destroy() = 0;
		/**
		 *\~english
		 *\brief		Detaches this shader from it's program
		 *\~french
		 *\brief		Détache ce shader de son programme
		 */
		C3D_API virtual void detach() = 0;
		/**
		 *\~english
		 *\brief		Attaches this shader to the given program
		 *\param[in]	program	The program
		 *\~french
		 *\brief		Attache ce shader au programme donné
		 *\param[in]	program	Le programme
		 */
		C3D_API virtual void attachTo( ShaderProgram & program ) = 0;
		/**
		 *\~english
		 *\brief		Sets the shader file for given model
		 *\remarks		The loaded file will be the one of the highest supported profile
		 *\param[in]	pathFile	The file name
		 *\~french
		 *\brief		Définit le fichier du shader pour le modèle donné
		 *\remarks		Le fichier chargé sera celui du plus haut profil supporté
		 *\param[in]	pathFile	Le nom du fichier
		 */
		C3D_API void setFile( castor::Path const & pathFile );
		/**
		 *\~english
		 *\brief		Tells if the shader object has a source file, whatever model it is
		 *\return		\p true if the shader object has a source file
		 *\~french
		 *\brief		Dit si le shader a un fichier source, quel que soit son modèle
		 *\return		\p true si le shader a un fichier source
		 */
		C3D_API bool hasFile()const;
		/**
		 *\~english
		 *\brief		Sets the shader source for given model
		 *\remarks		The loaded source will be the one of the highest supported profile
		 *\param[in]	source	The source code
		 *\~french
		 *\brief		Définit la source du shader pour le modèle donné
		 *\remarks		La source chargée sera celle du plus haut profil supporté
		 *\param[in]	source	Le code de la source
		 */
		C3D_API void setSource( castor::String const & source );
		/**
		 *\~english
		 *\brief		Sets the shader source for given model
		 *\remarks		The loaded source will be the one of the highest supported profile
		 *\param[in]	source	The source code
		 *\~french
		 *\brief		Définit la source du shader pour le modèle donné
		 *\remarks		La source chargée sera celle du plus haut profil supporté
		 *\param[in]	source	Le code de la source
		 */
		C3D_API void setSource( glsl::Shader const & source );
		/**
		 *\~english
		 *\brief		Tells if the shader object has a source code, whatever model it is
		 *\return		\p true if the shader object has a source code
		 *\~french
		 *\brief		Dit si le shader a un code source, quel que soit son modèle
		 *\return		\p true si le shader a un code source
		 */
		C3D_API bool hasSource()const;
		/**
		 *\~english
		 *\brief		Activates the shader
		 *\~french
		 *\brief		Active le shader
		 */
		C3D_API void bind();
		/**
		 *\~english
		 *\brief		Deactivates the shader
		 *\~french
		 *\brief		Désactive le shader
		 */
		C3D_API void unbind();
		/**
		 *\~english
		 *\brief		Compiles the shader
		 *\return		\p true on success
		 *\~french
		 *\brief		Compile le shader
		 *\return		\p true en cas de succès
		 */
		C3D_API virtual bool compile();
		/**
		 *\~english
		 *\brief		Creates a variable.
		 *\param[in]	type	The variable type.
		 *\param[in]	name	The variable name.
		 *\param[in]	nbOcc	The array dimension.
		 *\return		The created variable, nullptr if failed.
		 *\~french
		 *\brief		Crée une variable
		 *\param[in]	type	Le type de variable.
		 *\param[in]	name	Le nom de la variable.
		 *\param[in]	nbOcc	Les dimensions du tableau.
		 *\return		La variable créée, nullptr en cas d'échec.
		 */
		C3D_API PushUniformSPtr createUniform( UniformType type
			, castor::String const & name
			, int nbOcc = 1 );
		/**
		 *\~english
		 *\brief		Creates a variable.
		 *\param[in]	name	The variable name.
		 *\param[in]	nbOcc	The array dimension.
		 *\return		The created variable, nullptr if failed.
		 *\~french
		 *\brief		Crée une variable.
		 *\param[in]	name	Le nom de la variable.
		 *\param[in]	nbOcc	Les dimensions du tableau.
		 *\return		La variable créée, nullptr en cas d'échec.
		 */
		template< UniformType Type >
		inline std::shared_ptr< TPushUniform< Type > > createUniform( castor::String const & name
			, int nbOcc = 1 )
		{
			return std::static_pointer_cast< TPushUniform< Type > >( createUniform( Type, name, nbOcc ) );
		}
		/**
		 *\~english
		 *\brief		Finds a variable.
		 *\param[in]	type	The variable type.
		 *\param[in]	name	The variable name.
		 *\return		The found variable, nullptr if failed.
		 *\~french
		 *\brief		Trouve une variable.
		 *\param[in]	type	Le type de variable.
		 *\param[in]	name	Le nom de la variable.
		 *\return		La variable trouvée, nullptr en cas d'échec.
		 */
		C3D_API PushUniformSPtr findUniform( UniformType type
			, castor::String const & name )const;
		/**
		 *\~english
		 *\brief		Looks for a variable.
		 *\param[in]	name	The variable name.
		 *\return		The found variable, nullptr if failed.
		 *\~french
		 *\brief		Cherche une variable.
		 *\param[in]	name	Le nom de la variable.
		 *\return		La variable trouvé, nullptr en cas d'échec.
		 */
		template< UniformType Type >
		inline std::shared_ptr< TPushUniform< Type > > findUniform( castor::String const & name )const
		{
			return std::static_pointer_cast< TPushUniform< Type > >( findUniform( Type, name ) );
		}
		/**
		 *\~english
		 *\brief		Removes all frame variables
		 *\~french
		 *\brief		Vide la liste de frame variables
		 */
		C3D_API void flushUniforms();
		/**
		 *\~english
		 *\return		The frame variables bound to this shader.
		 *\~french
		 *\return		Les variables de frame liées à ce shader.
		 */
		inline PushUniformList & getUniforms()
		{
			return m_listUniforms;
		}
		/**
		 *\~english
		 *\return		The frame variables bound to this shader.
		 *\~french
		 *\return		Les variables de frame liées à ce shader.
		 */
		inline PushUniformList const & getUniforms()const
		{
			return m_listUniforms;
		}
		/**
		 *\~english
		 *\return		The compiled shader source code.
		 *\~french
		 *\return		Le code source du shader compilé.
		 */
		inline castor::String const & getSource()const
		{
			return m_source.getSource();
		}
		/**
		 *\~english
		 *\brief		Retrieves the shader file.
		 *\return		The file name
		 *\~french
		 *\brief		Récupère le fichier du shader.
		 *\return		Le nom du fichier
		 */
		inline castor::Path const & getFile()const
		{
			return m_file;
		}
		/**
		 *\~english
		 *\return		The shader type name.
		 *\~french
		 *\return		Le nom du type de shader.
		 */
		inline castor::String getStrType()const
		{
			return string_type[size_t( m_type )];
		}
		/**
		 *\~english
		 *\return		The shader type.
		 *\~french
		 *\return		Le type de shader.
		 */
		inline ShaderType getType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Sets the input primitives type
		 *\param[in]	topology	The input primitives type
		 *\~french
		 *\brief		Définit le type des primitives en entrée
		 *\param[in]	topology	Le type des primitives en entrée
		 */
		inline void setInputType( Topology topology )
		{
			m_inputType = topology;
		}
		/**
		 *\~english
		 *\return		The intput primitives type.
		 *\~french
		 *\return		Le type des primitives en entrée.
		 */
		inline Topology getInputType()const
		{
			return m_inputType;
		}
		/**
		 *\~english
		 *\brief		Sets the output primitives type
		 *\param[in]	topology	The primitives type
		 *\~french
		 *\brief		Définit le type des primitives en sortie
		 *\param[in]	topology	Le type des primitives
		 */
		inline void setOutputType( Topology topology )
		{
			m_outputType = topology;
		}
		/**
		 *\~english
		 *\return		The output primitives type.
		 *\~french
		 *\return		Le type des primitives en sortie.
		 */
		inline Topology getOutputType()const
		{
			return m_outputType;
		}
		/**
		 *\~english
		 *\brief		Sets the output vertex count
		 *\param[in]	count	The count
		 *\~french
		 *\brief		Définit le nombre de vertices générés
		 *\param[in]	count	Le compte
		 */
		void setOutputVtxCount( uint8_t count )
		{
			m_outputVtxCount = count;
		}
		/**
		 *\~english
		 *\return		The output vertex count.
		 *\~french
		 *\return		Le nombre de sommets générés.
		 */
		uint8_t getOutputVtxCount()const
		{
			return m_outputVtxCount;
		}
		/**
		 *\~english
		 *\brief		Retrieves the object compile status
		 *\return		The status
		 *\~french
		 *\brief		Récupère le statut de compilation de l'objet
		 *\return		Le statut
		 */
		inline ShaderStatus getStatus()const
		{
			return m_status;
		}
		/**
		 *\~english
		 *\brief		Retrieves the object parent
		 *\return		The parent
		 *\~french
		 *\brief		Récupère le parent de l'objet
		 *\return		Le parent
		 */
		inline ShaderProgram const & getParent()const
		{
			return m_parent;
		}

	protected:
		/**
		 *\~english
		 *\brief		Adds a uniform variable to pass to the shader objects
		 *\param[in]	variable	The variable to pass
		 *\~french
		 *\brief		Crée une variable uniform à donner aux ShaderObjects
		 *\param[in]	variable	La variable à donner
		 */
		C3D_API void doAddUniform( PushUniformSPtr variable );
		/**
		 *\~english
		 *\brief		Checks for compiler errors.
		 *\return		\p true if no error.
		 *\~french
		 *\brief		Vérifie les erreurs de compilation.
		 *\return		\p true s'il n'y a pas d'erreurs.
		 */
		C3D_API bool doCheckErrors();
		/**
		 *\~english
		 *\return		Retrieve compiler messages.
		 *\~english
		 *\return		Les messages du compilateur.
		 */
		virtual castor::String doRetrieveCompilerLog() = 0;
		/**
		 *\~english
		 *\brief		Creates a texture frame variable.
		 *\param[in]	type			The variable type.
		 *\param[in]	nbOccurences	The array dimension.
		 *\return		The created variable, nullptr if failed.
		 *\~french
		 *\brief		Crée une variable de frame texture.
		 *\param[in]	type			Le type de variable.
		 *\param[in]	nbOccurences	Les dimensions du tableau.
		 *\return		La variable créée, nullptr en cas d'échec.
		 */
		virtual PushUniformSPtr doCreateUniform( UniformType type, int nbOccurences ) = 0;

	private:
		void doFillVariables();

	protected:
		//!<\~english	The shader type.
		//!\~french		Le type de shader.
		ShaderType m_type;
		//!\~english	The parent shader program.
		//!\~french		Le programme parent.
		ShaderProgram & m_parent;
		//!<\~english	The shader compile status.
		//!\~french		Le statut de compilation du shader.
		ShaderStatus m_status{ ShaderStatus::eNotCompiled };
		//!\~english	The input primitive type (for geometry shaders).
		//!\~french		Le type de primitives en entrée (pour les geometry shaders).
		Topology m_inputType{ Topology::eTriangles };
		//!\~english	The output primitive type (for geometry shaders).
		//!\~french		Le type de primitives en sortie (pour les geometry shaders).
		Topology m_outputType{ Topology::eTriangles };
		//!\~english	The output vertex count (for geometry shaders)..
		//!\~french		Le nombre de vertex générés (pour les geometry shaders).
		uint8_t m_outputVtxCount{ 3 };
		//!\~english	Array of files path, sorted by shader model..
		//!\~french		Tableau des chemins de fichiers, triés par modèle de shader.
		castor::Path m_file;
		//!\~english	The shader information.
		//!\~french		Les informations du shader.
		glsl::Shader m_source;
		//!\~english	The frame variables map, ordered by name.
		//!\~french		La liste des variables de frame.
		PushUniformMap m_mapUniforms;
		//!\~english	The frame variables map.
		//!\~french		La liste des variables de frame.
		PushUniformList m_listUniforms;
	};
}

#endif
