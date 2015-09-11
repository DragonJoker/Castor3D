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
#ifndef ___C3D_SHADER_PROGRAM_H___
#define ___C3D_SHADER_PROGRAM_H___

#include "Castor3DPrerequisites.hpp"
#include "FrameVariable.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	template< class Ty > struct FrameVariableCreator;
	template< class Ty > struct ShaderObjectCreator;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.2
	\date		24/01/2011
	\~english
	\brief		Base shader program implementation, used to share functions through the supported shader languages
	\~french
	\brief		Implémentation de base d'un programme de shader, utilisé afin d'exposer les fonctions communes aux langages de shader
	*/
	class C3D_API ShaderProgramBase
	{
		template< class Ty > friend struct FrameVariableCreator;
		template< class Ty > friend struct ShaderObjectCreator;
		friend class Castor::TextLoader< Castor3D::ShaderProgramBase >;

	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		ShaderProgramBase loader
		\~french
		\brief		Loader de ShaderProgramBase
		*/
		class C3D_API TextLoader : public Castor::Loader< ShaderProgramBase, Castor::eFILE_TYPE_TEXT, Castor::TextFile >, public Castor::NonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief			Writes a ShaderProgram into a text file
			 *\param[in]		p_program	The ShaderProgram
			 *\param[in,out]	p_file		The file
			 *\~french
			 *\brief			Ecrit ShaderProgram dans un fichier texte
			 *\param[in]		p_program	Le ShaderProgram
			 *\param[in,out]	p_file		Le fichier
			 */
			virtual bool operator()( ShaderProgramBase const & p_program, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		15/04/2013
		\~english
		\brief		ShaderProgramBase loader
		\~french
		\brief		Loader de ShaderProgramBase
		*/
		class C3D_API BinaryParser
			:	public Castor3D::BinaryParser< ShaderProgramBase >
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
			BinaryParser( Castor::Path const & p_path );
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
			virtual bool Fill( ShaderProgramBase const & p_obj, BinaryChunk & p_chunk )const;
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
			virtual bool Parse( ShaderProgramBase & p_obj, BinaryChunk & p_chunk )const;
		};

		/**@name Scene */
		//@{

		//!\~english Name of the camera position frame variable	\~french Nom de la frame variable contenant la position de la caméra
		static const Castor::String CameraPos;
		//!\~english Name of the ambient light frame variable	\~french Nom de la frame variable contenant la lumière ambiante
		static const Castor::String AmbientLight;
		//!\~english Name of the background colour frame variable	\~french Nom de la frame variable contenant la couleur de fond
		static const Castor::String BackgroundColour;
		//!\~english Name of the lights count frame variable	\~french Nom de la frame variable contenant le compte des lumières
		static const Castor::String LightsCount;
		//!\~english Name of the lights frame variable	\~french Nom de la frame variable contenant les lumières
		static const Castor::String Lights;

		//@}
		/**@name Pass */
		//@{

		//!\~english Name of the ambient material colour frame variable	\~french Nom de la frame variable contenant la couleur ambiante du matériau
		static const Castor::String MatAmbient;
		//!\~english Name of the diffuse material colour frame variable	\~french Nom de la frame variable contenant la couleur diffuse du matériau
		static const Castor::String MatDiffuse;
		//!\~english Name of the specular material colour frame variable	\~french Nom de la frame variable contenant la couleur spéculaire du matériau
		static const Castor::String MatSpecular;
		//!\~english Name of the emissive material colour frame variable	\~french Nom de la frame variable contenant la couleur émissive du matériau
		static const Castor::String MatEmissive;
		//!\~english Name of the material shininess frame variable	\~french Nom de la frame variable contenant l'exposante du matériau
		static const Castor::String MatShininess;
		//!\~english Name of the material opacity frame variable	\~french Nom de la frame variable contenant l'opacité du matériau
		static const Castor::String MatOpacity;
		//!\~english Name of the overlay text image frame variable	\~french Nom de la frame variable contenant l'image de texte pour les overlays
		static const Castor::String MapText;

		//@}
		/**@name Textures */
		//@{

		//!\~english Name of the colour texture frame variable	\~french Nom de la frame variable contenant la texture de couleur
		static const Castor::String MapColour;
		//!\~english Name of the ambient texture frame variable	\~french Nom de la frame variable contenant la texture de couleur ambiante
		static const Castor::String MapAmbient;
		//!\~english Name of the diffuse texture frame variable	\~french Nom de la frame variable contenant la texture de couleur diffuse
		static const Castor::String MapDiffuse;
		//!\~english Name of the specular texture frame variable	\~french Nom de la frame variable contenant la texture de couleur spéculaire
		static const Castor::String MapSpecular;
		//!\~english Name of the emissive texture frame variable	\~french Nom de la frame variable contenant la texture de normales
		static const Castor::String MapNormal;
		//!\~english Name of the opacity texture frame variable	\~french Nom de la frame variable contenant texture d'opacité
		static const Castor::String MapOpacity;
		//!\~english Name of the shininess texture frame variable	\~french Nom de la frame variable contenant la texture d'exposante
		static const Castor::String MapGloss;
		//!\~english Name of the height texture frame variable	\~french Nom de la frame variable contenant texture de hauteur
		static const Castor::String MapHeight;

		//@}
		/**@name Frame Variable Buffers */
		//@{

		//!\~english Name of the matrix frame variable buffer	\~french Nom du frame variable buffer contenant les matrices
		static const Castor::String BufferMatrix;
		//!\~english Name of the scene frame variable buffer	\~french Nom du frame variable buffer contenant les données de scène
		static const Castor::String BufferScene;
		//!\~english Name of the pass frame variable buffer	\~french Nom du frame variable buffer contenant les données de passe
		static const Castor::String BufferPass;

		//@}

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The RenderSystem instance
		 *\param[in]	p_eLanguage		The program language
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderSystem	L'instance du RenderSystem
		 *\param[in]	p_eLanguage		Le langage du programme
		 */
		ShaderProgramBase( RenderSystem * p_pRenderSystem, eSHADER_LANGUAGE p_eLanguage );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~ShaderProgramBase();
		/**
		 *\~english
		 *\brief		Cleans the program up
		 *\~french
		 *\brief		Nettoie le programme
		 */
		virtual void Cleanup();
		/**
		 *\~english
		 *\brief		Initialises the program
		 *\~french
		 *\brief		Initialise le programme
		 */
		virtual void Initialise();
		/**
		 *\~english
		 *\brief		Creates the wanted shader object
		 *\param[in]	p_eType		The shader object concerned
		 *\~french
		 *\brief		Crée le shader object voulu
		 *\param[in]	p_eType		Le shader object concerné
		 */
		ShaderObjectBaseSPtr CreateObject( eSHADER_TYPE p_eType );
		/**
		 *\~english
		 *\brief		Activates the program
		 *\param[in]	p_byIndex	The current pass index
		 *\param[in]	p_byCount	The material passes count
		 *\~french
		 *\brief		Active le programme
		 *\param[in]	p_byIndex	L'index de la passe courante
		 *\param[in]	p_byCount	Le compte des passes du material
		 */
		virtual void Bind( uint8_t p_byIndex, uint8_t p_byCount );
		/**
		 *\~english
		 *\brief		Deactivates the program
		 *\~french
		 *\brief		Désactive le programme
		 */
		virtual void Unbind();
		/**
		 *\~english
		 *\brief		Sets all objects file
		 *\param[in]	p_eModel	The shader model
		 *\param[in]	p_pathFile	The file name
		 *\~french
		 *\brief		Définit le fichier utilisé par tous les objets
		 *\param[in]	p_eModel	Le modèle de shader
		 *\param[in]	p_pathFile	Le nom du fichier
		 */
		virtual void SetFile( eSHADER_MODEL p_eModel, Castor::Path const & p_pathFile );
		/**
		 *\~english
		 *\brief		Links all shader objects held by the program
		 *\~french
		 *\brief		Link tous les objets du programme
		 */
		virtual bool Link();
		/**
		 *\~english
		 *\brief		Adds a variable buffer to add
		 *\param[in]	p_pVariableBuffer	The GPU variables buffer
		 *\param[in]	p_shaderMask		MASK_SHADER_TYPE combination, to set at what shaders it is to be bound
		 *\~french
		 *\brief		Crée une buffer de variables uniformes à ajouter.
		 *\param[in]	p_pVariableBuffer	Le tampon de variables GPU
		 *\param[in]	p_shaderMask		Combinaison de MASK_SHADER_TYPE, pour déterminer les shaders auxquels il doit être lié
		 */
		void AddFrameVariableBuffer( FrameVariableBufferSPtr p_pVariableBuffer, uint32_t p_shaderMask );
		/**
		 *\~english
		 *\brief		Retrieves the frame variables bound to one shader type
		 *\param[in]	p_type	The shader type
		 *\return		The list
		 *\~french
		 *\brief		Récupère les variables de frame liées à un type de shader particulier
		 *\param[in]	p_type	The shader type
		 *\return		La liste
		 */
		FrameVariablePtrList & GetFrameVariables( eSHADER_TYPE p_type );
		/**
		 *\~english
		 *\brief		Retrieves the frame variables bound to one shader type
		 *\param[in]	p_type	The shader type
		 *\return		The list
		 *\~french
		 *\brief		Récupère les variables de frame liées à un type de shader particulier
		 *\param[in]	p_type	The shader type
		 *\return		La liste
		 */
		FrameVariablePtrList const & GetFrameVariables( eSHADER_TYPE p_type )const;
		/**
		 *\~english
		 *\brief		Resets compilation variables to be able to compile again
		 *\~french
		 *\brief		Réinitialise les variables de compilation afin de pouvoir compiler le shader à nouveau
		 */
		void ResetToCompile();
		/**
		 *\~english
		 *\brief		Retrieves a Vertex Attribute index from the program
		 *\param[in]	p_strName	The attribute name
		 *\return		The index, -1 if unavailable
		 *\~french
		 *\brief		Récupère l'index d'un attribut de vertex dans le programme
		 *\param[in]	p_strName	Le nom de l'attribut
		 *\return		L'index, -1 si indisponible
		 */
		virtual int GetAttributeLocation( Castor::String const & p_strName )const = 0;
		/**
		 *\~english
		 *\brief		Sets the input primitives type
		 *\param[in]	p_eTarget			The shader object concerned
		 *\param[in]	p_ePrimitiveType	The input primitives type
		 *\~french
		 *\brief		Définit le type des primitives en entrée
		 *\param[in]	p_eTarget			Le shader object concerné
		 *\param[in]	p_ePrimitiveType	Le type des primitives en entrée
		 */
		void SetInputType( eSHADER_TYPE p_eTarget, eTOPOLOGY p_ePrimitiveType );
		/**
		 *\~english
		 *\brief		Sets the output primitives type
		 *\param[in]	p_eTarget			The shader object concerned
		 *\param[in]	p_ePrimitiveType	The output primitives type
		 *\~french
		 *\brief		Définit le type des primitives en sortie
		 *\param[in]	p_eTarget			Le shader object concerné
		 *\param[in]	p_ePrimitiveType	Le type des primitives en sortie
		 */
		void SetOutputType( eSHADER_TYPE p_eTarget, eTOPOLOGY p_ePrimitiveType );
		/**
		 *\~english
		 *\brief		Sets the output vertex count
		 *\param[in]	p_eTarget	The shader object concerned
		 *\param[in]	p_uiCount	The count
		 *\~french
		 *\brief		Définit le nombre de vertices générés
		 *\param[in]	p_eTarget	Le shader object concerné
		 *\param[in]	p_uiCount	Le compte
		 */
		void SetOutputVtxCount( eSHADER_TYPE p_eTarget, uint8_t p_uiCount );
		/**
		 *\~english
		 *\brief		Sets the shader file for given model
		 *\remarks		The loaded file will be the one of the highest supported profile
		 *\param[in]	p_eTarget	The shader object concerned
		 *\param[in]	p_eModel	The shader model
		 *\param[in]	p_pathFile	The file name
		 *\~french
		 *\brief		Définit le fichier du shader pour le modèle donné
		 *\remarks		Le fichier chargé sera celui du plus haut profil supporté
		 *\param[in]	p_eTarget	Le shader object concerné
		 *\param[in]	p_eModel	Le modèle de shader
		 *\param[in]	p_pathFile	Le nom du fichier
		 */
		void SetFile( eSHADER_TYPE p_eTarget, eSHADER_MODEL p_eModel, Castor::Path const & p_pathFile );
		/**
		 *\~english
		 *\brief		Retrieves the shader file for given model
		 *\param[in]	p_eTarget	The shader object concerned
		 *\param[in]	p_eModel	The shader model
		 *\return		The file name
		 *\~french
		 *\brief		Récupère le fichier du shader pour le modèle donné
		 *\param[in]	p_eTarget	Le shader object concerné
		 *\param[in]	p_eModel	Le modèle de shader
		 *\return		Le nom du fichier
		 */
		Castor::Path GetFile( eSHADER_TYPE p_eTarget, eSHADER_MODEL p_eModel )const;
		/**
		 *\~english
		 *\brief		Tells if the shader object has a source file, whatever model it is
		 *\param[in]	p_eTarget	The shader object concerned
		 *\return		\p true if the shader object has a source file
		 *\~french
		 *\brief		Dit si le shader a un fichier source, quel que soit son modèle
		 *\param[in]	p_eTarget	Le shader object concerné
		 *\return		\p true si le shader a un fichier source
		 */
		bool HasFile( eSHADER_TYPE p_eTarget )const;
		/**
		 *\~english
		 *\brief		Sets the shader source for given model
		 *\remarks		The loaded source will be the one of the highest supported profile
		 *\param[in]	p_eTarget	The shader object concerned
		 *\param[in]	p_eModel	The shader model
		 *\param[in]	p_strSource	The source code
		 *\~french
		 *\brief		Définit la source du shader pour le modèle donné
		 *\remarks		La source chargée sera celle du plus haut profil supporté
		 *\param[in]	p_eTarget	Le shader object concerné
		 *\param[in]	p_eModel	Le modèle de shader
		 *\param[in]	p_strSource	Le code de la source
		 */
		void SetSource( eSHADER_TYPE p_eTarget, eSHADER_MODEL p_eModel, Castor::String const & p_strSource );
		/**
		 *\~english
		 *\brief		Retrieves the shader source for given model
		 *\param[in]	p_eTarget	The shader object concerned
		 *\param[in]	p_eModel	The shader model
		 *\return		The source code
		 *\~french
		 *\brief		Récupère la source du shader pour le modèle donné
		 *\param[in]	p_eTarget	Le shader object concerné
		 *\param[in]	p_eModel	Le modèle de shader
		 *\return		Le code de la source
		 */
		Castor::String GetSource( eSHADER_TYPE p_eTarget, eSHADER_MODEL p_eModel )const;
		/**
		 *\~english
		 *\brief		Tells if the shader object has a source code, whatever model it is
		 *\param[in]	p_eTarget	The shader object concerned
		 *\return		\p true if the shader object has a source code
		 *\~french
		 *\brief		Dit si le shader a un code source, quel que soit son modèle
		 *\param[in]	p_eTarget	Le shader object concerné
		 *\return		\p true si le shader a un code source
		 */
		bool HasSource( eSHADER_TYPE p_eTarget )const;
		/**
		 *\~english
		 *\brief		Defines entry point for shader languages that need it
		 *\param[in]	p_eTarget	The shader object concerned
		 *\param[in]	p_strName	The entry point
		 *\~french
		 *\brief		Définit le point d'entrée pour ls langages en ayant besoin
		 *\param[in]	p_eTarget	Le shader object concerné
		 *\param[in]	p_strName	Le point d'entrée
		 */
		void SetEntryPoint( eSHADER_TYPE p_eTarget, Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Retrieves the entry point of the wanted shader object
		 *\param[in]	p_eTarget	The shader object concerned
		 *\return		The entry point
		 *\~french
		 *\brief		Récupère le point d'entrée du shader voulu
		 *\param[in]	p_eTarget	Le shader object concerné
		 *\return		Le point d'entrée
		 */
		Castor::String GetEntryPoint( eSHADER_TYPE p_eTarget )const;
		/**
		 *\~english
		 *\brief		Tests if the program has an object for given type
		 *\param[in]	p_eTarget	The shader object type
		 *\return		\p true if the shader object exists
		 *\~french
		 *\brief		Dit si le programme a un shader du type donné
		 *\param[in]	p_eTarget	Le type du shader object
		 *\return		\p true si le shader object existe
		 */
		bool HasObject( eSHADER_TYPE p_eTarget )const;
		/**
		 *\~english
		 *\brief		Retrieves the given object type status
		 *\return		The status
		 *\~french
		 *\brief		Récupère le statut du type d'objet donné
		 *\return		Le statut
		 */
		eSHADER_STATUS GetObjectStatus( eSHADER_TYPE p_eTarget )const;
		/**
		 *\~english
		 *\brief		Creates a variable
		 *\param[in]	p_strName	The variable name
		 *\param[in]	p_eType		The shader type
		 *\param[in]	p_iNbOcc	The array dimension
		 *\return		The created variable, nullptr if failed
		 *\~french
		 *\brief		Crée une variable
		 *\param[in]	p_strName	Le nom de la variable
		 *\param[in]	p_eType		Le type du shader
		 *\param[in]	p_iNbOcc	Les dimensions du tableau
		 *\return		La variable créée, nullptr en cas d'échec
		 */
		OneTextureFrameVariableSPtr CreateFrameVariable( Castor::String const & p_strName, eSHADER_TYPE p_eType, int p_iNbOcc = 1 );
		/**
		 *\~english
		 *\brief		Looks for a variable
		 *\param[in]	p_strName	The variable name
		 *\param[in]	p_eType		The shader type
		 *\return		The found variable, nullptr if failed
		 *\~french
		 *\brief		Cherche une variable
		 *\param[in]	p_strName	Le nom de la variable
		 *\param[in]	p_eType		Le type du shader
		 *\return		La variable trouvé, nullptr en cas d'échec
		 */
		OneTextureFrameVariableSPtr FindFrameVariable( Castor::String const & p_strName, eSHADER_TYPE p_eType )const;
		/**
		 *\~english
		 *\brief		Finds a variable
		 *\return		The found variable, nullptr if failed
		 *\~french
		 *\brief		Trouve une variable
		 *\return		La variable trouvé, nullptr en cas d'échec
		 */
		FrameVariableBufferSPtr FindFrameVariableBuffer( Castor::String const & p_strName )const;
		/**
		 *\~english
		 *\brief		Retrieves the frame variable buffers bound to one shader type
		 *\param[in]	p_type	The shader type
		 *\return		The list
		 *\~french
		 *\brief		Récupère les tampons de variables de frame liés à un type de shader particulier
		 *\param[in]	p_type	The shader type
		 *\return		La liste
		 */
		inline FrameVariableBufferPtrList & GetFrameVariableBuffers( eSHADER_TYPE p_type )
		{
			return m_frameVariableBuffers[p_type];
		}
		/**
		 *\~english
		 *\brief		Retrieves the frame variable buffers bound to one shader type
		 *\param[in]	p_type	The shader type
		 *\return		The list
		 *\~french
		 *\brief		Récupère les tampons de variable de frames liés à un type de shader particulier
		 *\param[in]	p_type	The shader type
		 *\return		La liste
		 */
		inline FrameVariableBufferPtrList const & GetFrameVariableBuffers( eSHADER_TYPE p_type )const
		{
			return m_frameVariableBuffers[p_type];
		}
		/**
		 *\~english
		 *\brief		Retrieves the frame variable buffers
		 *\return		The list
		 *\~french
		 *\brief		Récupère les tampons de variables de frame
		 *\return		La liste
		 */
		inline FrameVariableBufferPtrList & GetFrameVariableBuffers()
		{
			return m_listFrameVariableBuffers;
		}
		/**
		 *\~english
		 *\brief		Retrieves the frame variable buffers
		 *\return		The list
		 *\~french
		 *\brief		Récupère les tampons de variable de frames
		 *\return		La liste
		 */
		inline FrameVariableBufferPtrList const & GetFrameVariableBuffers()const
		{
			return m_listFrameVariableBuffers;
		}
		/**
		 *\~english
		 *\brief		Retrieves the program link status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de link du programme
		 *\return		La valeur
		 */
		inline ePROGRAM_STATUS GetStatus()const
		{
			return m_eStatus;
		}
		/**
		 *\~english
		 *\brief		Retrieves the shader language
		 *\return		The value
		 *\~french
		 *\brief		Récupère le language du shader
		 *\return		La valeur
		 */
		inline eSHADER_LANGUAGE GetLanguage()const
		{
			return m_eLanguage;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RenderSystem
		 *\return		The value
		 *\~french
		 *\brief		Récupère le RenderSystem
		 *\return		La valeur
		 */
		inline RenderSystem * GetRenderSystem()const
		{
			return m_pRenderSystem;
		}

	protected:
		/**
		 *\~english
		 *\brief		Creates the wanted shader object
		 *\param[in]	p_eType		The shader object concerned
		 *\~french
		 *\brief		Crée le shader object voulu
		 *\param[in]	p_eType		Le shader object concerné
		 */
		virtual ShaderObjectBaseSPtr DoCreateObject( eSHADER_TYPE p_eType ) = 0;
		/**
		 *\~english
		 *\brief		Creates a texture frame variable
		 *\param[in]	p_iNbOcc	The array dimension
		 *\return		The created variable, nullptr if failed
		 *\~french
		 *\brief		Crée une variable de frame texture
		 *\param[in]	p_iNbOcc	Les dimensions du tableau
		 *\return		La variable créée, nullptr en cas d'échec
		 */
		virtual OneTextureFrameVariableSPtr DoCreateTextureVariable( int p_iNbOcc ) = 0;

	protected:
		//!<\~english The program status	\~french Le statut du programme
		ePROGRAM_STATUS m_eStatus;
		//!<\~english The program language	\~french Le langage du programme
		eSHADER_LANGUAGE m_eLanguage;
		//!<\~english Tells whether the shader is enabled or not	\~french Dit si le shader est activé ou pas
		bool m_bEnabled;
		//!\~english The shaders array	\~french Le tableau de shaders
		std::array< ShaderObjectBaseSPtr, eSHADER_TYPE_COUNT > m_pShaders;
		//!\~english The active shaders array	\~french Le tableau de shaders actifs
		std::vector< ShaderObjectBaseSPtr > m_activeShaders;
		//!\~english Array of files path, sorted by shader model	\~french Tableau des chemins de fichiers, triés par modèle de shader
		std::array< Castor::Path, eSHADER_MODEL_COUNT > m_arrayFiles;
		//!\~english The RenderSystem instance	\~french L'instance du RenderSystem
		RenderSystem * m_pRenderSystem;
		//!\~english The frame variable buffers map, ordered by name	\~french La liste des buffers de variable de frame, triés par nom
		FrameVariableBufferPtrStrMap m_frameVariableBuffersByName;
		//!\~english The frame variable buffers map, ordered by shader type	\~french La liste des buffers de variable de frame, triés par type de shader
		std::array< FrameVariableBufferPtrList, eSHADER_TYPE_COUNT > m_frameVariableBuffers;
		//!\~english The frame variable buffers map	\~french La liste des buffer de variables de frame
		FrameVariableBufferPtrList m_listFrameVariableBuffers;
	};
}

#pragma warning( pop )

#endif
