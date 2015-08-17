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
#ifndef ___C3D_PASS_H___
#define ___C3D_PASS_H___

#include "Castor3DPrerequisites.hpp"
#include "BinaryParser.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Definition of a material pass
	\remark		A pass is composed of : base colours (ambient, diffuse, specular, emissive), shininess,
				<br />texture units. Shader programs can also be applied.
	\~french
	\brief		Définition d'une passe d'un matériau
	\remark		Une passe est composé de : couleurs (ambiante, diffuse, spéculaire, émissive), exposant, textures, programme de shader
	*/
	class C3D_API Pass
	{
	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 19/10/2011
		\~english
		\brief Pass loader
		\~french
		\brief Loader de Pass
		*/
		class C3D_API TextLoader
			: public Castor::Loader< Pass, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
			, public Castor::NonCopyable
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
			 *\brief			Writes a pass into a text file
			 *\param[in]		p_pass	The pass to write
			 *\param[in,out]	p_file	The file where to write the pass
			 *\~french
			 *\brief			Ecrit une passe dans un fichier texte
			 *\param[in]		p_pass	La passe à écrire
			 *\param[in,out]	p_file	Le file où écrire la passe
			 */
			virtual bool operator()( Pass const & p_pass, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		15/04/2013
		\~english
		\brief		Pass loader
		\~french
		\brief		Loader de Pass
		*/
		class C3D_API BinaryParser
			: public Castor3D::BinaryParser< Pass >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_pathFile	The current path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_pathFile	Le chemin courant
			 */
			BinaryParser( Castor::Path const & p_pathFile );
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
			virtual bool Fill( Pass const & p_obj, BinaryChunk & p_chunk )const;
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
			virtual bool Parse( Pass & p_obj, BinaryChunk & p_chunk )const;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		Used by Material, don't use it.
		 *\param[in]	p_pEngine	The core engine
		 *\param[in]	p_pParent	The parent material
		 *\~french
		 *\brief		Constructeur
		 *\remark		A ne pas utiliser autrement que via la classe Material
		 *\param[in]	p_pEngine	Le moteur
		 *\param[in]	p_pParent	Le matériau parent
		 */
		Pass( Engine * p_pEngine, MaterialSPtr p_pParent = nullptr );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Pass();
		/**
		 *\~english
		 *\brief		Initialises the pass and all it's dependencies
		 *\~french
		 *\brief		Initialise la passe et toutes ses dépendances
		 */
		void Initialise();
		/**
		 *\~english
		 *\brief		Cleans up the pass and all it's dependencies
		 *\~french
		 *\brief		Nettoie la passe et toutes ses dépendances
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Binds this pass to given program
		 *\remark		Retrieves uniform variables, sets their values.
						<br />Does nothing if the given program is the same as the currently bound one
		 *\param[in]	p_pProgram	The program
		 *\~french
		 *\brief		Lie cette passe au programme donné
		 *\remark		Récupère les variables uniformes, définit leur valeur.
						<br />Ne fait rien si le programme donné ets le même que celui actuellement lié
		 *\param[in]	p_pProgram	Le programme
		 *\return
		 */
		void BindToProgram( ShaderProgramBaseSPtr p_pProgram );
		/**
		 *\~english
		 *\brief		Applies the pass
		 *\param[in]	p_byIndex	The pass index
		 *\param[in]	p_byCount	The material passes count
		 *\~french
		 *\brief		Applique la passe
		 *\param[in]	p_byIndex	L'index de la passe
		 *\param[in]	p_byCount	Le compte des passes du material
		 */
		void Render( uint8_t p_byIndex, uint8_t p_byCount );
		/**
		 *\~english
		 *\brief		Applies the pass for 2D render
		 *\param[in]	p_byIndex	The pass index
		 *\param[in]	p_byCount	The material passes count
		 *\~french
		 *\brief		Applique la passe pour un rendu 2D
		 *\param[in]	p_byIndex	L'index de la passe
		 *\param[in]	p_byCount	Le compte des passes du material
		 */
		void Render2D( uint8_t p_byIndex, uint8_t p_byCount );
		/**
		 *\~english
		 *\brief		Removes the pass (to avoid it from interfering with other passes)
		 *\~french
		 *\brief		Retire la passe du rendu courant
		 */
		void EndRender();
		/**
		 *\~english
		 *\brief		Creates and adds a TextureUnit
		 *\~french
		 *\brief		Crée et ajoute une unité de texture
		 */
		TextureUnitSPtr AddTextureUnit();
		/**
		 *\~english
		 *\brief		Retrieves the TextureUnit at wanted channel
		 *\remark		If more than one TextureUnits are found at given channel, the first one is returned
		 *\param[in]	p_eChannel	The channel
		 *\return		\p nullptr if no TextureUnit at wanted channel
		 *\~french
		 *\brief		Récupère la TextureUnit au canal demandé
		 *\remark		Si plus d'une TextureUnit est trouvée pour le canal demandé, la première est retournée
		 *\param[in]	p_eChannel	Le canal
		 *\return		\p nullptr si pas de TextureUnit au canal voulu
		 */
		TextureUnitSPtr GetTextureUnit( eTEXTURE_CHANNEL p_eChannel );
		/**
		 *\~english
		 *\brief		Destroys a TextureUnit at the given index
		 *\param[in]	p_uiIndex	the index of the TextureUnit to destroy
		 *\return		\p false if the index was out of bounds
		 *\~french
		 *\brief		Détruit la TextureUnit à l'index donné
		 *\param[in]	p_uiIndex	L'index de la TextureUnit à détruire
		 *\return		\p false si l'index était hors bornes
		 */
		bool DestroyTextureUnit( uint32_t p_uiIndex );
		/**
		 *\~english
		 *\brief		Retrieves the TextureUnit at the given index
		 *\param[in]	p_uiIndex	The index of the TextureUnit to retrieve
		 *\return		The retrieved TextureUnit, nullptr if none
		 *\~french
		 *\brief		Récupère la TextureUnit à l'index donné
		 *\param[in]	p_uiIndex	L'index voulu
		 *\return		La TextureUnit récupérée, nullptr si p_uiIndex était hors bornes
		 */
		TextureUnitSPtr GetTextureUnit( uint32_t p_uiIndex )const;
		/**
		 *\~english
		 *\brief		Retrieves the image path of the TextureUnit at the given index
		 *\param[in]	p_uiIndex	The index of the TextureUnit we want the image path
		 *\return		The path, void if none
		 *\~french
		 *\brief		Récupère le chemin de l'image de la TextureUnit à l'index donné
		 *\param[in]	p_uiIndex	L'index voulu
		 *\return		Le chemin, vide si aucun
		 */
		Castor::String GetTexturePath( uint32_t p_uiIndex );
		/**
		 *\~english
		 *\brief		Defines the shader program
		 *\param[in]	p_pProgram	The shader program
		 *\~french
		 *\brief		Définit le shader
		 *\param[in]	p_pProgram	Le programme
		 */
		void SetShader( ShaderProgramBaseSPtr p_pProgram );
		/**
		 *\~english
		 *\brief		Tells if the pass needs alpha blending
		 *\return		\p true if at least one texture unit has an alpha channel
		 *\~french
		 *\brief		Dit si la passe a besoin de mélange d'alpha
		 *\return		\p true si au moins une unité de texture a un canal alpha
		 */
		bool HasAlphaBlending()const;
		/**
		 *\~english
		 *\brief		Gives the current shader program
		 *\return		The shader program, nullptr if none
		 *\~french
		 *\brief		Récupère le shader
		 *\return		\p nullptr si aucun
		 */
		template <typename T>
		std::shared_ptr<T> GetShader()const
		{
			std::shared_ptr<T> l_pReturn;

			if ( ! m_pShaderProgram.expired() )
			{
				l_pReturn = std::static_pointer_cast<T, ShaderProgramBase>( m_pShaderProgram.lock() );
			}

			return l_pReturn;
		}
		/**
		*\~english
		*\brief			Tells if the pass has a shader program
		*\return		\p true if the shader program has been set, \p false if not
		*\~french
		*\brief			Dit si la passe a un shader
		*\return		\p true si le programme a été défini, \p false sinon
		 */
		bool HasShader()const;
		/**
		 *\~english
		 *\brief		Retrieves the texture channels flags combination
		 *\return		The value
		 *\~french
		 *\brief		Récupère la combinaison d'indicateurs de canal de texture
		 *\return		La valeur
		 */
		inline uint32_t GetTextureFlags()const
		{
			return m_uiTextureFlags;
		}
		/**
		 *\~english
		 *\brief		Tells whether the pass shader is automatically generated or not
		 *\return		The value
		 *\~french
		 *\brief		Dit si le shader de la passe est généré automatiquement ou pas
		 *\return		La valeur
		 */
		inline bool HasAutomaticShader()const
		{
			return m_bAutomaticShader;
		}
		/**
		 *\~english
		 *\brief		Sets the diffuse colour
		 *\param[in]	p_clrColour	The new value
		 *\~french
		 *\brief		Définit la couleur diffuse
		 *\param[in]	p_clrColour	La nouvelle valeur
		 */
		inline void SetDiffuse( Castor::Colour const & p_clrColour )
		{
			m_clrDiffuse = p_clrColour;
		}
		/**
		 *\~english
		 *\brief		Sets the ambient colour
		 *\param[in]	p_clrColour	The new value
		 *\~french
		 *\brief		Définit la couleur ambiante
		 *\param[in]	p_clrColour	La nouvelle valeur
		 */
		inline void SetAmbient( Castor::Colour const & p_clrColour )
		{
			m_clrAmbient = p_clrColour;
		}
		/**
		 *\~english
		 *\brief		Sets the specular colour
		 *\param[in]	p_clrColour	The new value
		 *\~french
		 *\brief		Définit la couleur spéculaire
		 *\param[in]	p_clrColour	La nouvelle valeur
		 */
		inline void SetSpecular( Castor::Colour const & p_clrColour )
		{
			m_clrSpecular = p_clrColour;
		}
		/**
		 *\~english
		 *\brief		Sets the emissive colour
		 *\param[in]	p_clrColour	The new value
		 *\~french
		 *\brief		Définit la couleur émissive
		 *\param[in]	p_clrColour	La nouvelle valeur
		 */
		inline void SetEmissive( Castor::Colour const & p_clrColour )
		{
			m_clrEmissive = p_clrColour;
		}
		/**
		 *\~english
		 *\brief		Sets the shininess
		 *\param[in]	p_fShininess	The new value
		 *\~french
		 *\brief		Définit l'exposant
		 *\param[in]	p_fShininess	La nouvelle valeur
		 */
		inline void SetShininess( float p_fShininess )
		{
			m_fShininess = p_fShininess;
		}
		/**
		 *\~english
		 *\brief		Sets the two sided status
		 *\param[in]	p_bDouble	The new value
		 *\~french
		 *\brief		Définit le statut double face
		 *\param[in]	p_bDouble	La nouvelle valeur
		 */
		inline void SetTwoSided( bool p_bDouble )
		{
			m_bDoubleFace = p_bDouble;
		}
		/**
		 *\~english
		 *\brief		Sets the global alpha value
		 *\param[in]	p_fAlpha	The new value
		 *\~french
		 *\brief		Définit la valeur alpha globale
		 *\param[in]	p_fAlpha	La nouvelle valeur
		 */
		inline void SetAlpha( float p_fAlpha )
		{
			m_fAlpha = p_fAlpha;
			m_clrDiffuse.alpha() = p_fAlpha;
			m_clrAmbient.alpha() = p_fAlpha;
			m_clrSpecular.alpha() = p_fAlpha;
			m_clrEmissive.alpha() = p_fAlpha;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha blend mode
		 *\param[in]	p_value	The value
		 *\~french
		 *\brief		Définit le mode de mélange alpha
		 *\param[in]	p_value	La valeur
		 */
		inline void SetAlphaBlendMode( eBLEND_MODE p_value )
		{
			m_alphaBlendMode = p_value;
		}
		/**
		 *\~english
		 *\brief		Sets the colour blend mode
		 *\param[in]	p_value	The value
		 *\~french
		 *\brief		Définit le mode de mélange couleur
		 *\param[in]	p_value	La valeur
		 */
		inline void SetColourBlendMode( eBLEND_MODE p_value )
		{
			m_colourBlendMode = p_value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the blend state
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'état de mélange
		 *\return		La valeur
		 */
		inline BlendStateSPtr GetBlendState()const
		{
			return m_pBlendState;
		}
		/**
		 *\~english
		 *\brief		Retrieves the shniness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'exposant
		 *\return		La valeur
		 */
		inline float GetShininess()const
		{
			return m_fShininess;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture units count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre d'unités de texture
		 *\return		La valeur
		 */
		inline uint32_t GetTextureUnitsCount()const
		{
			return uint32_t( m_arrayTextureUnits.size() );
		}
		/**
		 *\~english
		 *\brief		Tells if the pass is two sided
		 *\~french
		 *\brief
		 *\remark		Dit si la passe est double face
		 */
		inline bool IsTwoSided()const
		{
			return m_bDoubleFace;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent material
		 *\return		The value
		 *\~french
		 *\brief		Récupère le matériau parent
		 *\return		La valeur
		 */
		inline MaterialSPtr GetParent()const
		{
			return m_pParent.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the global alpha value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur alpha globale
		 *\return		La valeur
		 */
		inline float GetAlpha()const
		{
			return m_fAlpha;
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha blend mode
		 *\return		The value
		 *\~french
		 *\brief		Récupère le mode de mélange alpha
		 *\return		La valeur
		 */
		inline eBLEND_MODE GetAlphaBlendMode()const
		{
			return m_alphaBlendMode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the colour blend mode
		 *\return		The value
		 *\~french
		 *\brief		Récupère le mode de mélange couleur
		 *\return		La valeur
		 */
		inline eBLEND_MODE GetColourBlendMode()const
		{
			return m_colourBlendMode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the diffuse colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur diffuse
		 *\return		La valeur
		 */
		inline Castor::Colour const & GetDiffuse()const
		{
			return m_clrDiffuse;
		}
		/**
		 *\~english
		 *\brief		Retrieves the ambient colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur ambiante
		 *\return		La valeur
		 */
		inline Castor::Colour const & GetAmbient()const
		{
			return m_clrAmbient;
		}
		/**
		 *\~english
		 *\brief		Retrieves the specular colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur spéculaire
		 *\return		La valeur
		 */
		inline Castor::Colour const & GetSpecular()const
		{
			return m_clrSpecular;
		}
		/**
		 *\~english
		 *\brief		Retrieves the emissive colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur émissive
		 *\return		La valeur
		 */
		inline Castor::Colour const & GetEmissive()const
		{
			return m_clrEmissive;
		}
		/**
		 *\~english
		 *\brief		Retrieves the diffuse colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur diffuse
		 *\return		La valeur
		 */
		inline Castor::Colour & GetDiffuse()
		{
			return m_clrDiffuse;
		}
		/**
		 *\~english
		 *\brief		Retrieves the ambient colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur ambiante
		 *\return		La valeur
		 */
		inline Castor::Colour & GetAmbient()
		{
			return m_clrAmbient;
		}
		/**
		 *\~english
		 *\brief		Retrieves the specular colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur spéculaire
		 *\return		La valeur
		 */
		inline Castor::Colour & GetSpecular()
		{
			return m_clrSpecular;
		}
		/**
		 *\~english
		 *\brief		Retrieves the emissive colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur émissive
		 *\return		La valeur
		 */
		inline Castor::Colour & GetEmissive()
		{
			return m_clrEmissive;
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator on the beginning of the textures array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le début du tableau de textures
		 *\return		L'itérateur
		 */
		inline TextureUnitPtrArrayConstIt begin()const
		{
			return m_arrayTextureUnits.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the beginning of the textures array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le début du tableau de textures
		 *\return		L'itérateur
		 */
		inline TextureUnitPtrArrayIt begin()
		{
			return m_arrayTextureUnits.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator on the end of the textures array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la fin du tableau de textures
		 *\return		L'itérateur
		 */
		inline TextureUnitPtrArrayConstIt end()const
		{
			return m_arrayTextureUnits.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the end of the textures array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau de textures
		 *\return		L'itérateur
		 */
		inline TextureUnitPtrArrayIt end()
		{
			return m_arrayTextureUnits.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the pass variable buffer
		 *\return		The variable buffer
		 *\~french
		 *\brief		Récupère le buffer de variables pour pass
		 *\return		Le buffer de variables
		 */
		inline FrameVariableBufferSPtr GetPassBuffer()const
		{
			return m_passBuffer.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the scene variable buffer
		 *\return		The variable buffer
		 *\~french
		 *\brief		Récupère le buffer de variables pour la scène
		 *\return		Le buffer de variables
		 */
		inline FrameVariableBufferSPtr GetSceneBuffer()const
		{
			return m_sceneBuffer.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the matrices variable buffer
		 *\return		The variable buffer
		 *\~french
		 *\brief		Récupère le buffer de variables pour les matrices
		 *\return		Le buffer de variables
		 */
		inline FrameVariableBufferSPtr GetMatrixBuffer()const
		{
			return m_matrixBuffer.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the Engine
		 *\~french
		 *\brief		Récupère l'Engine
		 */
		virtual Engine * GetEngine()const
		{
			return m_pEngine;
		}

	private:
		void DoBindTexture( eTEXTURE_CHANNEL p_channel, Castor::String const & p_name, ShaderProgramBase & p_program );
		void DoBindTextures();
		void DoBindBuffers();
		/**
		 *\~english
		 *\brief		Prepares a texture to be integrated to the pass.
		 *\remarks		Removes alpha channel if any, stores it in p_opacity if it is empty
		 *\param[in]	p_channel		The texture channel
		 *\param[in]	p_unit			The texture unit
		 *\param[in,out]p_index			The texture index
		 *\param[in,out]p_opacitySource	Receives the texture unit if p_opacity is modified
		 *\param[in,out]p_opacity		Receives the alpha channel of the texture
		 *\return		\p true if there were an alpha channel in the texture
		 *\~french
		 *\brief		Prépare une texture à être intégrée à la passe
		 *\remarks		Enlève le canal alpha s'il y en avait un, il est stocké dans p_opacity si celui-ci est vide
		 *\param[in]	p_channel		Le canal de texture
		 *\param[in]	p_unit			L'unité de texture
		 *\param[in,out]p_index			L'index de la texture
		 *\param[in,out]p_opacitySource	Reçoit l'unité de texture si p_opacity est modifié
		 *\param[in,out]p_opacity		Reçoit le canal alpha de la texture
		 *\return		\p true Si la texture possédait un canal alpha
		 */
		bool DoPrepareTexture( eTEXTURE_CHANNEL p_channel, TextureUnitSPtr p_unit, uint32_t & p_index, TextureUnitSPtr & p_opacitySource, Castor::PxBufferBaseSPtr & p_opacity );
		/**
		 *\~english
		 *\brief		Prepares a texture to be integrated to the pass.
		 *\remarks		Removes alpha channel if any.
		 *\param[in]	p_channel		The texture channel
		 *\param[in]	p_unit			The texture unit
		 *\param[in,out]p_index			The texture index
		 *\~french
		 *\brief		Prépare une texture à être intégrée à la passe.
		 *\remarks		Enlève le canal alpha s'il y en avait un.
		 *\param[in]	p_channel		Le canal de texture
		 *\param[in]	p_unit			L'unité de texture
		 *\param[in,out]p_index			L'index de la texture
		 */
		void DoPrepareTexture( eTEXTURE_CHANNEL p_channel, TextureUnitSPtr p_unit, uint32_t & p_index );
		/**
		 *\~english
		 *\brief		Applies the pass
		 *\param[in]	p_byIndex	The pass index
		 *\param[in]	p_byCount	The material passes count
		 *\~french
		 *\brief		Applique la passe
		 *\param[in]	p_byIndex	L'index de la passe
		 *\param[in]	p_byCount	Le compte des passes du material
		 */
		void DoRender( uint8_t p_byIndex, uint8_t p_byCount );

	protected:
		typedef std::pair< TextureUnitWPtr, OneTextureFrameVariableWPtr > UnitVariablePair;
		DECLARE_MAP( eTEXTURE_CHANNEL, UnitVariablePair, UnitVariableChannel );
		friend class Material;
		//!\~english The core engine	\~french Le moteur
		Engine * m_pEngine;
		//!\~english Diffuse material colour	\~french La couleur diffuse
		Castor::Colour m_clrDiffuse;
		//!\~english Ambient material colour	\~french La couleur ambiante
		Castor::Colour m_clrAmbient;
		//!\~english Specular material colour	\~french La couleur spéculaire
		Castor::Colour m_clrSpecular;
		//!\~english Emissive material colour	\~french La couleur émissive
		Castor::Colour m_clrEmissive;
		//!\~english The shininess value	\~french L'exposant
		float m_fShininess;
		//!\~english The alpha value	\~french L'alpha
		float m_fAlpha;
		//!\~english Tells if the pass is two sided	\~french Dit si la passe est sur 2 faces
		bool m_bDoubleFace;
		//!\~english The shader program, if any	\~french Le programme de shader
		ShaderProgramBaseWPtr m_pShaderProgram;
		//!\~english Texture units	\~french Les textures
		TextureUnitPtrArray m_arrayTextureUnits;
		//!\~english The parent material	\~french Le materiau parent
		MaterialWPtr m_pParent;
		//!\~english The current shader variables and associated texture units, ordered by channel	\~french Les variables de shader avec les unités de texture associées, triées par canal
		UnitVariableChannelMap m_mapUnits;
		//!\~english Blend states	\~french Etats de blend
		BlendStateSPtr m_pBlendState;
		//!\~english Bitwise ORed eTEXTURE_CHANNEL	\~french Combinaison des eTEXTURE_CHANNEL affectés à une texture pour cette passe
		uint32_t m_uiTextureFlags;
		//!\~english Tells the pass shader is an automatically generated one	\~french Dit que le shader de la passe a été généré automatiquement
		bool m_bAutomaticShader;
		//!\~english The alpha blend mode \~french Le mode de mélange alpha
		eBLEND_MODE m_alphaBlendMode;
		//!\~english The colour blend mode \~french Le mode de mélange couleur
		eBLEND_MODE m_colourBlendMode;
		//!\~english Holds The scene frame variables buffer	\~french Le buffer de variables, pour la scène
		FrameVariableBufferWPtr m_sceneBuffer;
		//!\~english Holds The pass frame variables buffer	\~french Le buffer de variables, pour la passe
		FrameVariableBufferWPtr m_passBuffer;
		//!\~english Holds The matrix frame variables buffer	\~french Le buffer de variables, pour les matrices
		FrameVariableBufferWPtr m_matrixBuffer;
		//! The ambient colour frame variable
		Point4fFrameVariableWPtr m_pAmbient;
		//! The diffuser colour frame variable
		Point4fFrameVariableWPtr m_pDiffuse;
		//! The specular colour frame variable
		Point4fFrameVariableWPtr m_pSpecular;
		//! The emissive colour frame variable
		Point4fFrameVariableWPtr m_pEmissive;
		//! The shininess value frame variable
		OneFloatFrameVariableWPtr m_pShininess;
		//! The opacity value frame variable
		OneFloatFrameVariableWPtr m_pOpacity;
		//! The camera position frame variable
		Point3rFrameVariableWPtr m_pCameraPos;
		//! The ambient colour value
		Castor::Point4f m_ptAmbient;
		//! The diffuse colour value
		Castor::Point4f m_ptDiffuse;
		//! The specular colour value
		Castor::Point4f m_ptSpecular;
		//! The emissive colour value
		Castor::Point4f m_ptEmissive;
		//! The camera position value
		Castor::Point3r m_ptCameraPos;
	};
}

#pragma warning( pop )

#endif
