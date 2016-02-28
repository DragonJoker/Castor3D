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
#ifndef ___C3D_SHADER_OBJECT_H___
#define ___C3D_SHADER_OBJECT_H___

#include "Castor3DPrerequisites.hpp"
#include "BinaryParser.hpp"

namespace Castor3D
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
	class ShaderObjectBase
	{
	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 19/10/2011
		\~english
		\brief ShaderObjectBase loader
		\~french
		\brief Loader de ShaderObjectBase
		*/
		class TextLoader
			: public Castor::Loader< ShaderObjectBase, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextLoader( Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief			Writes a ShaderObject into a text file
			 *\param[in]		p_shaderObject	The ShaderObject
			 *\param[in,out]	p_file			The file
			 *\~french
			 *\brief			Ecrit un ShaderObject dans un fichier texte
			 *\param[in]		p_shaderObject	Le ShaderObject
			 *\param[in,out]	p_file			Le fichier
			 */
			C3D_API virtual bool operator()( ShaderObjectBase const & p_shaderObject, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		15/04/2013
		\~english
		\brief		ShaderObjectBase loader
		\~french
		\brief		Loader de ShaderObjectBase
		*/
		class BinaryParser
			: public Castor3D::BinaryParser< ShaderObjectBase >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'accès au dossier courant
			 */
			C3D_API BinaryParser( Castor::Path const & p_path );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques
			 *\param[in]	p_obj	L'objet à écrire
			 *\param[out]	p_chunk	Le chunk à remplir
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			C3D_API virtual bool Fill( ShaderObjectBase const & p_obj, BinaryChunk & p_chunk )const;
			/**
			 *\~english
			 *\brief		Function used to retrieve specific data from the chunk
			 *\param[out]	p_obj	The object to read
			 *\param[in]	p_chunk	The chunk containing data
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
			 *\param[out]	p_obj	L'objet à lire
			 *\param[in]	p_chunk	Le chunk contenant les données
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			C3D_API virtual bool Parse( ShaderObjectBase & p_obj, BinaryChunk & p_chunk )const;
		};
	private:
		static const std::array< Castor::String, eSHADER_TYPE_COUNT >	string_type;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_parent	Parent program
		 *\param[in]	p_type		Shader type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_parent	programme parent
		 *\param[in]	p_type		Type de shader
		 */
		C3D_API ShaderObjectBase( ShaderProgram * p_parent, eSHADER_TYPE p_type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~ShaderObjectBase();
		/**
		 *\~english
		 *\brief		Creates the program on GPU
		 *\~french
		 *\brief		Crée le programme sur le GPU
		 */
		C3D_API virtual bool Create() = 0;
		/**
		 *\~english
		 *\brief		Destroys the program on GPU
		 *\~french
		 *\brief		Détruit le programme sur le GPU
		 */
		C3D_API virtual void Destroy() = 0;
		/**
		 *\~english
		 *\brief		Detaches this shader from it's program
		 *\~french
		 *\brief		Détache ce shader de son programme
		 */
		C3D_API virtual void Detach() = 0;
		/**
		 *\~english
		 *\brief		Attaches this shader to the given program
		 *\param[in]	p_program	The program
		 *\~french
		 *\brief		Attache ce shader au programme donné
		 *\param[in]	p_program	Le programme
		 */
		C3D_API virtual void AttachTo( ShaderProgram & p_program ) = 0;
		/**
		 *\~english
		 *\brief		Sets the shader file for given model
		 *\remark		The loaded file will be the one of the highest supported profile
		 *\param[in]	p_eModel	The shader model
		 *\param[in]	p_pathFile	The file name
		 *\~french
		 *\brief		Définit le fichier du shader pour le modèle donné
		 *\remark		Le fichier chargé sera celui du plus haut profil supporté
		 *\param[in]	p_eModel	Le modèle de shader
		 *\param[in]	p_pathFile	Le nom du fichier
		 */
		C3D_API void SetFile( eSHADER_MODEL p_eModel, Castor::Path const & p_pathFile );
		/**
		 *\~english
		 *\brief		Tells if the shader object has a source file, whatever model it is
		 *\return		\p true if the shader object has a source file
		 *\~french
		 *\brief		Dit si le shader a un fichier source, quel que soit son modèle
		 *\return		\p true si le shader a un fichier source
		 */
		C3D_API bool HasFile()const;
		/**
		 *\~english
		 *\brief		Sets the shader source for given model
		 *\remark		The loaded source will be the one of the highest supported profile
		 *\param[in]	p_eModel	The shader model
		 *\param[in]	p_strSource	The source code
		 *\~french
		 *\brief		Définit la source du shader pour le modèle donné
		 *\remark		La source chargée sera celle du plus haut profil supporté
		 *\param[in]	p_eModel	Le modèle de shader
		 *\param[in]	p_strSource	Le code de la source
		 */
		C3D_API void SetSource( eSHADER_MODEL p_eModel, Castor::String const & p_strSource );
		/**
		 *\~english
		 *\brief		Tells if the shader object has a source code, whatever model it is
		 *\return		\p true if the shader object has a source code
		 *\~french
		 *\brief		Dit si le shader a un code source, quel que soit son modèle
		 *\return		\p true si le shader a un code source
		 */
		C3D_API bool HasSource()const;
		/**
		 *\~english
		 *\brief		Activates the shader
		 *\~french
		 *\brief		Active le shader
		 */
		C3D_API void Bind();
		/**
		 *\~english
		 *\brief		Deactivates the shader
		 *\~french
		 *\brief		Désactive le shader
		 */
		C3D_API void Unbind();
		/**
		 *\~english
		 *\brief		Compiles the shader
		 *\return		\p true on success
		 *\~french
		 *\brief		Compile le shader
		 *\return		\p true en cas de succès
		 */
		C3D_API virtual bool Compile();
		/**
		 *\~english
		 *\brief		Adds a uniform variable to pass to the shader objects
		 *\param[in]	p_variable	The variable to pass
		 *\~french
		 *\brief		Crée une variable uniform à donner aux ShaderObjects
		 *\param[in]	p_variable	La variable à donner
		 */
		C3D_API virtual void AddFrameVariable( OneIntFrameVariableSPtr p_variable );
		/**
		 *\~english
		 *\brief		Finds a variable
		 *\return		The found variable, nullptr if failed
		 *\~french
		 *\brief		Trouve une variable
		 *\return		La variable trouvé, nullptr en cas d'échec
		 */
		C3D_API OneIntFrameVariableSPtr FindFrameVariable( Castor::String const & p_name )const;
		/**
		 *\~english
		 *\brief		Removes all frame variables
		 *\~french
		 *\brief		Vide la liste de frame variables
		 */
		C3D_API virtual void FlushFrameVariables();
		/**
		 *\~english
		 *\brief		Retrieves the frame variables bound to this shader
		 *\return		The list
		 *\~french
		 *\brief		Récupère les variables de frame liées à ce shader
		 *\return		La liste
		 */
		inline FrameVariablePtrList & GetFrameVariables()
		{
			return m_listFrameVariables;
		}
		/**
		 *\~english
		 *\brief		Retrieves the frame variables bound to this shader
		 *\param[in]	p_type	The shader type
		 *\return		The list
		 *\~french
		 *\brief		Récupère les variables de frame liées à ce shader
		 *\return		La liste
		 */
		inline FrameVariablePtrList const & GetFrameVariables()const
		{
			return m_listFrameVariables;
		}
		/**
		 *\~english
		 *\brief		Tells if the compiled shader has the given parameter
		 *\param[in]	p_name	The parameter name
		 *\return		\p true if the parameter was found
		 *\~french
		 *\brief		Dit si le shader compilé a le paramètre donné
		 *\param[in]	p_name	Le nom du paramètre
		 *\return		\p true si le paramètre a été trouvé
		 */
		virtual bool HasParameter( Castor::String const & CU_PARAM_UNUSED( p_name ) )
		{
			return false;
		}
		/**
		 *\~english
		 *\brief		Retrieves the shader source for given model
		 *\param[in]	p_eModel	The shader model
		 *\return		The source code
		 *\~french
		 *\brief		Récupère la source du shader pour le modèle donné
		 *\param[in]	p_eModel	Le modèle de shader
		 *\return		Le code de la source
		 */
		inline Castor::String const & GetSource( eSHADER_MODEL p_eModel )const
		{
			return m_arraySources[p_eModel];
		}
		/**
		 *\~english
		 *\brief		Retrieves the compiled shader source
		 *\return		The source code
		 *\~french
		 *\brief		Récupère la source du shader compilé
		 *\return		Le code de la source
		 */
		inline Castor::String const & GetLoadedSource()const
		{
			return m_loadedSource;
		}
		/**
		 *\~english
		 *\brief		Retrieves the shader file for given model
		 *\param[in]	p_eModel	The shader model
		 *\return		The file name
		 *\~french
		 *\brief		Récupère le fichier du shader pour le modèle donné
		 *\param[in]	p_eModel	Le modèle de shader
		 *\return		Le nom du fichier
		 */
		inline Castor::Path const & GetFile( eSHADER_MODEL p_eModel )const
		{
			return m_arrayFiles[p_eModel];
		}
		/**
		 *\~english
		 *\brief		Retrieves the shader file for current model
		 *\return		The file name
		 *\~french
		 *\brief		Récupère le fichier du shader pour le modèle courant
		 *\return		Le nom du fichier
		 */
		inline Castor::Path const & GetCurrentFile()const
		{
			return m_arrayFiles[m_eShaderModel];
		}
		/**
		 *\~english
		 *\brief		Retrieves the name of this object type
		 *\return		The type name
		 *\~french
		 *\brief		Récupère le nom du type de cet objet
		 *\return		Le nom du type
		 */
		inline Castor::String GetStrType()const
		{
			return string_type[m_type];
		}
		/**
		 *\~english
		 *\brief		Retrieves the this object type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type de cet objet
		 *\return		Le type
		 */
		inline eSHADER_TYPE GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Sets the input primitives type
		 *\param[in]	p_topology	The input primitives type
		 *\~french
		 *\brief		Définit le type des primitives en entrée
		 *\param[in]	p_topology	Le type des primitives en entrée
		 */
		inline void SetInputType( eTOPOLOGY val )
		{
			m_eInputType = val;
		}
		/**
		 *\~english
		 *\brief		Retrieves the intput primitives type
		 *\return		The primitives type
		 *\~french
		 *\brief		Récupère le type des primitives en entrée
		 *\return		Le type des primitives
		 */
		inline eTOPOLOGY GetInputType()const
		{
			return m_eInputType;
		}
		/**
		 *\~english
		 *\brief		Sets the output primitives type
		 *\param[in]	p_topology	The primitives type
		 *\~french
		 *\brief		Définit le type des primitives en sortie
		 *\param[in]	p_topology	Le type des primitives
		 */
		inline void SetOutputType( eTOPOLOGY val )
		{
			m_eOutputType = val;
		}
		/**
		 *\~english
		 *\brief		Retrieves the output primitives type
		 *\return		The primitives type
		 *\~french
		 *\brief		Récupère le type des primitives en sortie
		 *\return		Le type des primitives
		 */
		inline eTOPOLOGY GetOutputType()const
		{
			return m_eOutputType;
		}
		/**
		 *\~english
		 *\brief		Sets the output vertex count
		 *\param[in]	p_count	The count
		 *\~french
		 *\brief		Définit le nombre de vertices générés
		 *\param[in]	p_count	Le compte
		 */
		void SetOutputVtxCount( uint8_t p_count )
		{
			m_uiOutputVtxCount = p_count;
		}
		/**
		 *\~english
		 *\brief		Retrieves the output vertex count
		 *\return		The count
		 *\~french
		 *\brief		Récupère le nombre de vertices générés
		 *\return		Le compte
		 */
		uint8_t GetOutputVtxCount()const
		{
			return m_uiOutputVtxCount;
		}
		/**
		 *\~english
		 *\brief		Retrieves the object compile status
		 *\return		The status
		 *\~french
		 *\brief		Récupère le statut de compilation de l'objet
		 *\return		Le statut
		 */
		inline eSHADER_STATUS GetStatus()const
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
		inline ShaderProgram * GetParent()const
		{
			return m_parent;
		}
		/**
		 *\~english
		 *\brief		Defines the given parameter value
		 *\param[in]	p_name	The parameter name
		 *\param[in]	p_value	The parameter value
		 *\~french
		 *\brief		Définit la valeur du paramètre
		 *\param[in]	p_name	Le nom du paramètre
		 *\param[in]	p_value	La valeur du paramètre
		 */
		virtual void SetParameter( Castor::String const & CU_PARAM_UNUSED( p_name ), Castor::Matrix4x4r const & CU_PARAM_UNUSED( p_value ) ) {}
		/**
		 *\~english
		 *\brief		Defines the given parameter value
		 *\param[in]	p_name	The parameter name
		 *\param[in]	p_value	The parameter value
		 *\~french
		 *\brief		Définit la valeur du paramètre
		 *\param[in]	p_name	Le nom du paramètre
		 *\param[in]	p_value	La valeur du paramètre
		 */
		virtual void SetParameter( Castor::String const & CU_PARAM_UNUSED( p_name ), Castor::Matrix3x3r const & CU_PARAM_UNUSED( p_value ) ) {}

	protected:
		/**
		 *\~english
		 *\brief		Checks for compiler errors.
		 *\return		\p true if no error.
		 *\~french
		 *\brief		Vérifie les erreurs de compilation.
		 *\return		\p true s'il n'y a pas d'erreurs.
		 */
		C3D_API bool DoCheckErrors();
		/**
		 *\~english
		 *\return		Retrieve compiler messages.
		 *\~english
		 *\return		Les messages du compilateur.
		 */
		virtual Castor::String DoRetrieveCompilerLog() = 0;

	protected:
		//!\~english The parent shader program	\~french Le programme parent
		ShaderProgram * m_parent;
		//!<\~english The shader type	\~french Le type de shader
		eSHADER_TYPE m_type;
		//!\~english Array of files path, sorted by shader model	\~french Tableau des chemins de fichiers, triés par modèle de shader
		std::array< Castor::Path, eSHADER_MODEL_COUNT > m_arrayFiles;
		//!\~english Array of source codes, sorted by shader model	\~french Tableau des codes sources, triés par modèle de shader
		std::array< Castor::String, eSHADER_MODEL_COUNT > m_arraySources;
		//!<\~english Actually loaded ASCII Source-Code	\~french Le texte ASCII du shader chargé
		Castor::String m_loadedSource;
		//!<\~english Actually loaded file path	\~french Le chemin d'accès au fichier contenant le source du shader
		Castor::Path m_pathLoadedFile;
		//!<\~english The shader compile status	\~french Le statut de compilation du shader
		eSHADER_STATUS m_status;
		//!\~english The frame variables map, ordered by name	\~french La liste des variables de frame
		FrameVariablePtrStrMap m_mapFrameVariables;
		//!\~english The frame variables map	\~french La liste des variables de frame
		FrameVariablePtrList m_listFrameVariables;
		//!\~english The input primitive type (for geometry shaders)	\~french Le type de primitives en entrée (pour les geometry shaders)
		eTOPOLOGY m_eInputType;
		//!\~english The output primitive type (for geometry shaders)	\~french Le type de primitives en sortie (pour les geometry shaders)
		eTOPOLOGY m_eOutputType;
		//!\~english The output vertex count (for geometry shaders)	\~french Le nombre de vertex générés (pour les geometry shaders)
		uint8_t m_uiOutputVtxCount;
		//!\~english The current shader model	\~french Le modèle de shader actuel
		eSHADER_MODEL m_eShaderModel;
	};
}

#endif
