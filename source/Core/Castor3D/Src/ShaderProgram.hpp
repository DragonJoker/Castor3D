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
		 *\brief		Retrieves the vertex shader source according to the flags
		 *\param[in]	p_uiProgramFlags	Bitwise ORed ePROGRAM_FLAG
		 *\param[in]	p_bLightPass		Tells we want the light pass shader
		 *\~french
		 *\brief		Récupère le source du vertex shader selon les flags donnés
		 *\param[in]	p_uiProgramFlags	Combinaison de ePROGRAM_FLAG
		 *\param[in]	p_bLightPass		Dit si on veut le shader de la passe de lumières
		 */
		Castor::String GetVertexShaderSource( uint32_t p_uiProgramFlags, bool p_bLightPass = false );
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source according to the flags
		 *\param[in]	p_uiFlags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader selon les flags donnés
		 *\param[in]	p_uiFlags	Une combinaison de eTEXTURE_CHANNEL
		 */
		Castor::String GetPixelShaderSource( uint32_t p_uiFlags );
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
		virtual void Begin( uint8_t p_byIndex, uint8_t p_byCount );
		/**
		 *\~english
		 *\brief		Deactivates the program
		 *\~french
		 *\brief		Désactive le programme
		 */
		virtual void End();
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
		 *\brief		Returns a free light index and assigns it
		 *\return		The assigned light index, -1 if no more available.
		 *\~french
		 *\brief		Assigne une lumière et retourne son index
		 *\return		L'index de la lumière assignée, -1 si plus aucune disponible.
		 */
		virtual int AssignLight();
		/**
		 *\~english
		 *\brief		Frees a given light index
		 *\param[in]	The light index
		 *\~french
		 *\brief		Libère une lumière
		 *\param[in]	L'index de la lumière
		 */
		virtual void FreeLight( int p_iIndex );
		/**
		 *\~english
		 *\brief		Sets ambient component of the light at given index
		 */
		virtual void SetLightAmbient( int p_iIndex, Castor::Colour const & p_crColour );
		/**
		 *\~english
		 *\brief		Sets diffuse component of the light at given index
		 */
		virtual void SetLightDiffuse( int p_iIndex, Castor::Colour const & p_crColour );
		/**
		 *\~english
		 *\brief		Sets specular component of the light at given index
		 */
		virtual void SetLightSpecular( int p_iIndex, Castor::Colour const & p_crColour );
		/**
		 *\~english
		 *\brief		Sets the position of the light at given index
		 */
		virtual void SetLightPosition( int p_iIndex, Castor::Point4f const & p_ptPosition );
		/**
		 *\~english
		 *\brief		Sets the orientation of the light at given index
		 */
		virtual void SetLightOrientation( int p_iIndex, Castor::Matrix4x4r const & p_mtxOrientation );
		/**
		 *\~english
		 *\brief		Sets the attenuation components of the light at given index (spot and point lights only)
		 */
		virtual void SetLightAttenuation( int p_iIndex, Castor::Point3f const & p_fAtt );
		/**
		 *\~english
		 *\brief		Sets the exponent of the light at given index (spotlight only)
		 */
		virtual void SetLightExponent( int p_iIndex, float p_fExp );
		/**
		 *\~english
		 *\brief		Sets the cutoff angle of the light at given index (spotlight only)
		 */
		virtual void SetLightCutOff( int p_iIndex, float p_fCut );
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
		 *\brief		Retrieves the matrices variables buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon de variables de matrices
		 *\return		Le tampon
		 */
		inline FrameVariableBufferSPtr GetMatrixBuffer()const
		{
			return m_pMatrixBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the pass variables buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon de variables de passe
		 *\return		Le tampon
		 */
		FrameVariableBufferSPtr GetPassBuffer()const
		{
			return m_pPassBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the scene variables buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon de variables de scène
		 *\return		Le tampon
		 */
		FrameVariableBufferSPtr GetSceneBuffer()const
		{
			return m_pSceneBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the constant variables buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon de variables constantes
		 *\return		Le tampon
		 */
		FrameVariableBufferSPtr GetUserBuffer()const
		{
			return m_pUserBuffer;
		}
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
		 *\remark		The loaded file will be the one of the highest supported profile
		 *\param[in]	p_eTarget	The shader object concerned
		 *\param[in]	p_eModel	The shader model
		 *\param[in]	p_pathFile	The file name
		 *\~french
		 *\brief		Définit le fichier du shader pour le modèle donné
		 *\remark		Le fichier chargé sera celui du plus haut profil supporté
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
		 *\brief		Sets the shader source for given model
		 *\remark		The loaded source will be the one of the highest supported profile
		 *\param[in]	p_eTarget	The shader object concerned
		 *\param[in]	p_eModel	The shader model
		 *\param[in]	p_strSource	The source code
		 *\~french
		 *\brief		Définit la source du shader pour le modèle donné
		 *\remark		La source chargée sera celle du plus haut profil supporté
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
		bool HasProgram( eSHADER_TYPE p_eTarget )const;
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
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source according to the flags
		 *\param[in]	p_uiProgramFlags	Bitwise ORed ePROGRAM_FLAG
		 *\~french
		 *\brief		Récupère le source du vertex shader selon les flags donnés
		 *\param[in]	p_uiProgramFlags	Une combinaison de ePROGRAM_FLAG
		 */
		virtual Castor::String DoGetVertexShaderSource( uint32_t p_uiProgramFlags ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source according to the flags
		 *\param[in]	p_uiFlags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader selon les flags donnés
		 *\param[in]	p_uiFlags	Une combinaison de eTEXTURE_CHANNEL
		 */
		virtual Castor::String DoGetPixelShaderSource( uint32_t p_uiFlags ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source for deferred shading's geometry pass according to the flags
		 *\param[in]	p_uiProgramFlags	Bitwise ORed ePROGRAM_FLAG
		 *\param[in]	p_bLightPass		Tells we want the light pass shader
		 *\~french
		 *\brief		Récupère le source du vertex shader pour la passe géométrique du deferred shading selon les flags donnés
		 *\param[in]	p_uiProgramFlags	Une combinaison de ePROGRAM_FLAG
		 *\param[in]	p_bLightPass		Dit si on veut le shader de la passe de lumières
		 */
		virtual Castor::String DoGetDeferredVertexShaderSource( uint32_t p_uiProgramFlags, bool p_bLightPass ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for deferred shading's geometry pass according to the flags
		 *\param[in]	p_uiFlags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader pour la passe géométrique du deferred shading selon les flags donnés
		 *\param[in]	p_uiFlags	Une combinaison de eTEXTURE_CHANNEL
		 */
		virtual Castor::String DoGetDeferredPixelShaderSource( uint32_t p_uiFlags ) = 0;

	protected:
		//!<\~english The program status	\~french Le statut du programme
		ePROGRAM_STATUS m_eStatus;
		//!<\~english The program language	\~french Le langage du programme
		eSHADER_LANGUAGE m_eLanguage;
		//!<\~english Tells whether the shader is enabled or not	\~french Dit si le shader est activé ou pas
		bool m_bEnabled;
		//!\~english The shaders array	\~french Le tableau de shaders
		ShaderObjectBaseSPtr m_pShaders[eSHADER_TYPE_COUNT];
		//!\~english Array of files path, sorted by shader model	\~french Tableau des chemins de fichiers, triés par modèle de shader
		std::array< Castor::Path, eSHADER_MODEL_COUNT > m_arrayFiles;
		//!\~english The RenderSystem instance	\~french L'instance du RenderSystem
		RenderSystem * m_pRenderSystem;
		//!\~english Tells if at least one light has changed since last frame	\~french Dit si une lumière au moins a changé depuis la dernière frame
		bool m_bLightsChanged;
		//!\~english Holds the number of active lights	\~french Contient le nombre de lumières actives
		OneIntFrameVariableSPtr m_pLightsCount;
		//!\~english Holds lights	\~french Contient les lumières
		OneTextureFrameVariableSPtr m_pLights;
		//!\~english Holds the camera position	\~french Contient la position de la caméra
		Point3rFrameVariableSPtr m_pCameraPos;
		//!\~english The image containing lights data	\~french L'image contenant les données des lumières
		Castor::PxBufferBaseSPtr m_pLightsData;
		//!\~english The lights texture	\~french La texture contenant les lumières
		TextureUnitSPtr m_pLightsTexture;
		//!\~english The set of available images	indices	\~french Le set contenant les indices des lumières disponibles
		std::set <int> m_setFreeLights;
		//!\~english The matrix variables buffer	\~french Le tampon de variables de matrices
		FrameVariableBufferSPtr m_pMatrixBuffer;
		//!\~english The pass variables buffer	\~french Le tampon de variables de passe
		FrameVariableBufferSPtr m_pPassBuffer;
		//!\~english The scene variables buffer	\~french Le tampon de variables de scène
		FrameVariableBufferSPtr m_pSceneBuffer;
		//!\~english The user variables buffer	\~french Le tampon de variables utilisateur
		FrameVariableBufferSPtr m_pUserBuffer;
	};
}

#pragma warning( pop )

#endif
