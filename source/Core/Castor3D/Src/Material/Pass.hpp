/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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

#include <OwnedBy.hpp>

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
	class Pass
		: public Castor::OwnedBy< Engine >
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
		class TextWriter
			: public Castor::TextWriter< Pass >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
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
			C3D_API bool operator()( Pass const & p_pass, Castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remarks		Used by Material, don't use it.
		 *\param[in]	p_engine	The core engine
		 *\param[in]	p_parent	The parent material
		 *\~french
		 *\brief		Constructeur
		 *\remarks		A ne pas utiliser autrement que via la classe Material
		 *\param[in]	p_engine	Le moteur
		 *\param[in]	p_parent	Le matériau parent
		 */
		C3D_API Pass( Engine & p_engine, MaterialSPtr p_parent = nullptr );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Pass();
		/**
		 *\~english
		 *\brief		Initialises the pass and all it's dependencies
		 *\~french
		 *\brief		Initialise la passe et toutes ses dépendances
		 */
		C3D_API void Initialise();
		/**
		 *\~english
		 *\brief		Cleans up the pass and all it's dependencies
		 *\~french
		 *\brief		Nettoie la passe et toutes ses dépendances
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Binds this pass to given render node.
		 *\param[in]	p_node	The render node.
		 *\~french
		 *\brief		Lie cette passe au noeud de rendu donné.
		 *\param[in]	p_node	Le noeud de rendu.
		 *\return
		 */
		C3D_API void BindToNode( RenderNode & p_node );
		/**
		 *\~english
		 *\brief		Binds this pass to given render node.
		 *\param[in]	p_node	The render node.
		 *\~french
		 *\brief		Lie cette passe au noeud de rendu donné.
		 *\param[in]	p_node	Le noeud de rendu.
		 *\return
		 */
		C3D_API void BindToNode( SceneRenderNode & p_node );
		/**
		 *\~english
		 *\brief		Applies the pass
		 *\~french
		 *\brief		Applique la passe
		 */
		C3D_API void Render();
		/**
		 *\~english
		 *\brief		Applies the pass for 2D render
		 *\~french
		 *\brief		Applique la passe pour un rendu 2D
		 */
		C3D_API void Render2D();
		/**
		 *\~english
		 *\brief		Removes the pass (to avoid it from interfering with other passes)
		 *\~french
		 *\brief		Retire la passe du rendu courant
		 */
		C3D_API void EndRender();
		/**
		 *\~english
		 *\brief		Adds a texture unit.
		 *\param[in]	p_unit	The texture unit.
		 *\~french
		 *\brief		Ajoute une unité de texture.
		 *\param[in]	p_unit	L'unité de texture.
		 */
		C3D_API void AddTextureUnit( TextureUnitSPtr p_unit );
		/**
		 *\~english
		 *\brief		Retrieves the TextureUnit at wanted channel
		 *\remarks		If more than one TextureUnits are found at given channel, the first one is returned
		 *\param[in]	p_channel	The channel
		 *\return		\p nullptr if no TextureUnit at wanted channel
		 *\~french
		 *\brief		Récupère la TextureUnit au canal demandé
		 *\remarks		Si plus d'une TextureUnit est trouvée pour le canal demandé, la première est retournée
		 *\param[in]	p_channel	Le canal
		 *\return		\p nullptr si pas de TextureUnit au canal voulu
		 */
		C3D_API TextureUnitSPtr GetTextureUnit( TextureChannel p_channel );
		/**
		 *\~english
		 *\brief		Destroys a TextureUnit at the given index
		 *\param[in]	p_index	the index of the TextureUnit to destroy
		 *\return		\p false if the index was out of bounds
		 *\~french
		 *\brief		Détruit la TextureUnit à l'index donné
		 *\param[in]	p_index	L'index de la TextureUnit à détruire
		 *\return		\p false si l'index était hors bornes
		 */
		C3D_API bool DestroyTextureUnit( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Retrieves the TextureUnit at the given index
		 *\param[in]	p_index	The index of the TextureUnit to retrieve
		 *\return		The retrieved TextureUnit, nullptr if none
		 *\~french
		 *\brief		Récupère la TextureUnit à l'index donné
		 *\param[in]	p_index	L'index voulu
		 *\return		La TextureUnit récupérée, nullptr si p_index était hors bornes
		 */
		C3D_API TextureUnitSPtr GetTextureUnit( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Tells if the pass needs alpha blending
		 *\return		\p true if at least one texture unit has an alpha channel
		 *\~french
		 *\brief		Dit si la passe a besoin de mélange d'alpha
		 *\return		\p true si au moins une unité de texture a un canal alpha
		 */
		C3D_API bool HasAlphaBlending()const;
		/**
		 *\~english
		 *\brief		Binds the program and the textures.
		 *\remarks		The frame variable buffers are not bound.
						A call to FillShaderVariables must be done before the draw call.
		 *\~french
		 *\brief		Lie le programme et les textures.
		 *\remarks		Les tampons de variables de frame ne seront as liés.
						Un appel à FillShaderVariables doit être fait avant le draw call.
		 */
		C3D_API void Bind();
		/**
		 *\~english
		 *\brief		Fills shader variables.
		 *\~french
		 *\brief		Remplit les variables de shader.
		 */
		C3D_API void FillShaderVariables( RenderNode & p_node )const;
		/**
		 *\~english
		 *\brief		Reduces the textures.
		 *\~french
		 *\brief		Réduit les textures.
		 */
		C3D_API void PrepareTextures();
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
			return m_textureFlags;
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
		inline void SetAlphaBlendMode( BlendMode p_value )
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
		inline void SetColourBlendMode( BlendMode p_value )
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
		 *\remarks		Dit si la passe est double face
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
			return m_parent.lock();
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
		inline BlendMode GetAlphaBlendMode()const
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
		inline BlendMode GetColourBlendMode()const
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
		inline auto begin()const
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
		inline auto begin()
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
		inline auto end()const
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
		inline auto end()
		{
			return m_arrayTextureUnits.end();
		}

	private:
		/**
		 *\~english
		 *\brief		Makes the link between an existing texture bound to a channel, and the matching shader variable in given program
		 *\param[in]	p_channel	The texture channel
		 *\param[in]	p_name		The shader variable name
		 *\param[in]	p_program	The shader program
		 *\param[in,out]p_variable	Receives the shader variable
		 *\~french
		 *\brief		Fait le lien entre une texture affectée à un canal, et la variable shader correspondante dans le programme donné
		 *\param[in]	p_channel	Le canal de la texture
		 *\param[in]	p_name		Le nom de la variable shader
		 *\param[in]	p_program	Le programme
		 *\param[in,out]p_variable	Reçoit la variable shader
		 */
		C3D_API void DoGetTexture( TextureChannel p_channel, Castor::String const & p_name, RenderNode & p_node );
		/**
		 *\~english
		 *\brief		Retrieves the channeled textures shader variables
		 *\~french
		 *\brief		Récupère les variables associées aux texture affectées à un canal
		 */
		C3D_API void DoGetTextures( RenderNode & p_node );
		/**
		 *\~english
		 *\brief		Retrieves the pass, scene, and matrix buffers, and needed variables
		 *\~french
		 *\brief		Récupère les tampons de variables de passe, scène et matrices, ainsi que les variables nécessaires
		 */
		C3D_API void DoGetBuffers( RenderNode & p_node );
		/**
		 *\~english
		 *\brief		Retrieves the pass, scene, and matrix buffers, and needed variables
		 *\~french
		 *\brief		Récupère les tampons de variables de passe, scène et matrices, ainsi que les variables nécessaires
		 */
		C3D_API void DoGetBuffers( SceneRenderNode & p_node );
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
		C3D_API bool DoPrepareTexture( TextureChannel p_channel, TextureUnitSPtr p_unit, uint32_t & p_index, TextureUnitSPtr & p_opacitySource, Castor::PxBufferBaseSPtr & p_opacity );
		/**
		 *\~english
		 *\brief		Prepares a texture to be integrated to the pass.
		 *\remarks		Removes alpha channel if any.
		 *\param[in]	p_channel		The texture channel
		 *\param[in]	p_unit			The texture unit
		 *\param[in,out]p_index			The texture index
		 *\return		The original texture's alpha channel.
		 *\~french
		 *\brief		Prépare une texture à être intégrée à la passe.
		 *\remarks		Enlève le canal alpha s'il y en avait un.
		 *\param[in]	p_channel		Le canal de texture
		 *\param[in]	p_unit			L'unité de texture
		 *\param[in,out]p_index			L'index de la texture
		 *\return		Le canal alpha de la texture originale.
		 */
		C3D_API Castor::PxBufferBaseSPtr DoPrepareTexture( TextureChannel p_channel, TextureUnitSPtr p_unit, uint32_t & p_index );
		/**
		 *\~english
		 *\brief		Applies the pass
		 *\~french
		 *\brief		Applique la passe
		 */
		C3D_API void DoRender()const;
		/**
		 *\~english
		 *\brief		Unapplies the pass.
		 *\~french
		 *\brief		Désapplique la passe.
		 */
		C3D_API void DoEndRender()const;
		/**
		 *\~english
		 *\brief		Updates the texture flags depending on the texture units.
		 *\~french
		 *\brief		Met à jour les indicateurs de texture en fonction des unités de texture.
		 */
		C3D_API void DoUpdateFlags();

	protected:
		typedef std::pair< TextureUnitWPtr, OneIntFrameVariableWPtr > UnitVariablePair;
		DECLARE_MAP( TextureChannel, UnitVariablePair, UnitVariableChannel );
		friend class Material;
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
		//!\~english Texture units	\~french Les textures
		TextureUnitPtrArray m_arrayTextureUnits;
		//!\~english The parent material	\~french Le materiau parent
		MaterialWPtr m_parent;
		//!\~english The current shader variables and associated texture units, ordered by channel	\~french Les variables de shader avec les unités de texture associées, triées par canal
		UnitVariableChannelMap m_mapUnits;
		//!\~english Blend states	\~french Etats de blend
		BlendStateSPtr m_pBlendState;
		//!\~english Bitwise ORed TextureChannel	\~french Combinaison des TextureChannel affectés à une texture pour cette passe
		uint32_t m_textureFlags;
		//!\~english Tells the pass shader is an automatically generated one	\~french Dit que le shader de la passe a été généré automatiquement
		bool m_bAutomaticShader;
		//!\~english The alpha blend mode \~french Le mode de mélange alpha
		BlendMode m_alphaBlendMode;
		//!\~english The colour blend mode \~french Le mode de mélange couleur
		BlendMode m_colourBlendMode;
		//!\~english Tells if the pass' textures are reduced. \~french Dit si les textures de la passe sont réduites.
		bool m_texturesReduced;
	};
}

#endif
