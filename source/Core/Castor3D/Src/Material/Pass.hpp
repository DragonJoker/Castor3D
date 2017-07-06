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
#ifndef ___C3D_PASS_H___
#define ___C3D_PASS_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>
#include <Design/Signal.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Base class of a material pass.
	\~french
	\brief		Classe de base d'une passe d'un matériau.
	*/
	class Pass
		: public Castor::OwnedBy< Material >
	{
	public:
		using Changed = std::function< void( Pass const & ) >;
		using OnChanged = Castor::Signal< Changed >;
		using OnChangedConnection = OnChanged::connection;

	public:
		/*!
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 19/10/2011
		\~english
		\brief Pass loader.
		\~french
		\brief Loader de Pass.
		*/
		class TextWriter
			: public Castor::TextWriter< Pass >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief			Writes a Pass into a text file.
			 *\param[in]		p_pass	The Pass to write.
			 *\param[in,out]	p_file	The file where to write the Pass.
			 *\~french
			 *\brief			Ecrit une Pass dans un fichier texte.
			 *\param[in]		p_pass	La Pass à écrire.
			 *\param[in,out]	p_file	Le file où écrire la Pass.
			 */
			C3D_API bool operator()( Pass const & p_pass, Castor::TextFile & p_file )override;
		};

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_parent	The parent material.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_parent	Le matériau parent.
		 */
		C3D_API Pass( Material & p_parent );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Pass();

	public:
		/**
		 *\~english
		 *\brief		Initialises the pass and all it's dependencies.
		 *\~french
		 *\brief		Initialise la passe et toutes ses dépendances.
		 */
		C3D_API void Initialise();
		/**
		 *\~english
		 *\brief		Cleans up the pass and all it's dependencies.
		 *\~french
		 *\brief		Nettoie la passe et toutes ses dépendances.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Fills shader variables of given render node.
		 *\~french
		 *\brief		Remplit les variables de shader du noeud de rendu donné.
		 */
		C3D_API void Update( PassBuffer & p_passes )const;
		/**
		 *\~english
		 *\brief		Binds the pass' textures.
		 *\~french
		 *\brief		Active les textures de la passe.
		 */
		C3D_API void BindTextures();
		/**
		 *\~english
		 *\brief		Unbinds the pass' textures.
		 *\~french
		 *\brief		Désactive les textures de la passe.
		 */
		C3D_API void UnbindTextures();
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
		 *\brief		Retrieves the TextureUnit at wanted channel.
		 *\remarks		If more than one TextureUnits are found at given channel, the first one is returned.
		 *\param[in]	p_channel	The channel.
		 *\return		\p nullptr if no TextureUnit at wanted channel.
		 *\~french
		 *\brief		Récupère la TextureUnit au canal demandé.
		 *\remarks		Si plus d'une TextureUnit est trouvée pour le canal demandé, la première est retournée.
		 *\param[in]	p_channel	Le canal.
		 *\return		\p nullptr si pas de TextureUnit au canal voulu.
		 */
		C3D_API TextureUnitSPtr GetTextureUnit( TextureChannel p_channel )const;
		/**
		 *\~english
		 *\brief		Destroys a TextureUnit at the given index.
		 *\param[in]	p_index	the index of the TextureUnit to destroy.
		 *\~french
		 *\brief		Détruit la TextureUnit à l'index donné.
		 *\param[in]	p_index	L'index de la TextureUnit à détruire.
		 */
		C3D_API void DestroyTextureUnit( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Retrieves the TextureUnit at the given index.
		 *\param[in]	p_index	The index of the TextureUnit to retrieve.
		 *\return		The retrieved TextureUnit, nullptr if none.
		 *\~french
		 *\brief		Récupère la TextureUnit à l'index donné.
		 *\param[in]	p_index	L'index voulu.
		 *\return		La TextureUnit récupérée, nullptr si p_index était hors bornes.
		 */
		C3D_API TextureUnitSPtr GetTextureUnit( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Tells if the pass needs alpha blending.
		 *\return		\p true if at least one texture unit has an alpha channel.
		 *\~french
		 *\brief		Dit si la passe a besoin de mélange d'alpha.
		 *\return		\p true si au moins une unité de texture a un canal alpha.
		 */
		C3D_API bool HasAlphaBlending()const;
		/**
		 *\~english
		 *\brief		Reduces the textures.
		 *\~french
		 *\brief		Réduit les textures.
		 */
		C3D_API void PrepareTextures();
		/**
		 *\~english
		 *\return		The material type.
		 *\~french
		 *\return		Le type de matériau.
		 */
		C3D_API MaterialType GetType()const;
		/**
		 *\~english
		 *\brief		Sets the global alpha value.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la valeur alpha globale.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		C3D_API void SetOpacity( float p_value );
		/**
		 *\~english
		 *\return		The program flags combination.
		 *\~french
		 *\return		La combinaison d'indicateurs de programme.
		 */
		C3D_API ProgramFlags GetProgramFlags()const;
		/**
		 *\~english
		 *\remarks	Passes are aligned on float[4], so the size of a pass
		 *			is the number of float[4] needed to contain it.
		 *\~french
		 *\remarks	Les passes sont alignées sur 4 flottants, donc la taille d'une passe
		 *			correspond aux nombres de float[4] qu'il faut pour la contenir.
		 */
		GlslWriter_API virtual uint32_t GetPassSize()const = 0;
		/**
		 *\~english
		 *\return		The texture channels flags combination.
		 *\~french
		 *\return		La combinaison d'indicateurs de canal de texture.
		 */
		inline TextureChannels const & GetTextureFlags()const
		{
			return m_textureFlags;
		}
		/**
		 *\~english
		 *\return		\p true if the pass shader is automatically generated.
		 *\~french
		 *\return		\p true si le shader de la passe est généré automatiquement.
		 */
		inline bool HasAutomaticShader()const
		{
			return m_automaticShader;
		}
		/**
		 *\~english
		 *\brief		Sets the two sided status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut d'application aux deux faces.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetTwoSided( bool p_value )
		{
			m_twoSided = p_value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the emissive factor.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le facteur d'émission.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetEmissive( float const & p_value )
		{
			m_emissive = p_value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the refraction ratio.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le ratio de réfraction.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetRefractionRatio( float p_value )
		{
			m_refractionRatio = p_value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the alpha blend mode.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le mode de mélange alpha.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetAlphaBlendMode( BlendMode p_value )
		{
			m_alphaBlendMode = p_value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the colour blend mode.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le mode de mélange couleur.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetColourBlendMode( BlendMode p_value )
		{
			m_colourBlendMode = p_value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\return		The texture units count.
		 *\~french
		 *\return		Le nombre d'unités de texture.
		 */
		inline uint32_t GetTextureUnitsCount()const
		{
			return uint32_t( m_textureUnits.size() );
		}
		/**
		 *\~english
		 *\brief		Tells if the pass is two sided.
		 *\~french
		 *\brief
		 *\remarks		Dit si la passe s'applique sur les deux faces.
		 */
		inline bool IsTwoSided()const
		{
			return m_twoSided;
		}
		/**
		 *\~english
		 *\return		\p true if environment mapping is enabled for this pass.
		 *\~french
		 *\return		\p true si l'environment mapping est activé sur cette passe.
		 */
		inline bool HasEnvironmentMapping()const
		{
			return CheckFlag( GetTextureFlags(), TextureChannel::eReflection )
				|| CheckFlag( GetTextureFlags(), TextureChannel::eRefraction );
		}
		/**
		 *\~english
		 *\return		The global opacity value.
		 *\~french
		 *\return		La valeur globale d'opacité.
		 */
		inline float GetOpacity()const
		{
			return m_opacity;
		}
		/**
		 *\~english
		 *\return		The emissive factor.
		 *\~french
		 *\return		Le facteur émission.
		 */
		inline float GetEmissive()const
		{
			return m_emissive;
		}
		/**
		 *\~english
		 *\return		The refraction ratio.
		 *\~french
		 *\return		Le ration de réfraction.
		 */
		inline float GetRefractionRatio()const
		{
			return m_refractionRatio;
		}
		/**
		 *\~english
		 *\return		The alpha blend mode.
		 *\~french
		 *\return		Le mode de mélange alpha.
		 */
		inline BlendMode GetAlphaBlendMode()const
		{
			return m_alphaBlendMode;
		}
		/**
		 *\~english
		 *\return		The colour blend mode.
		 *\~french
		 *\return		Le mode de mélange couleur.
		 */
		inline BlendMode GetColourBlendMode()const
		{
			return m_colourBlendMode;
		}
		/**
		 *\~english
		 *\return		A constant iterator on the beginning of the textures array.
		 *\~french
		 *\return		Un itérateur constant sur le début du tableau de textures.
		 */
		inline auto begin()const
		{
			return m_textureUnits.begin();
		}
		/**
		 *\~english
		 *\return		An iterator on the beginning of the textures array.
		 *\~french
		 *\return		Un itérateur sur le début du tableau de textures.
		 */
		inline auto begin()
		{
			return m_textureUnits.begin();
		}
		/**
		 *\~english
		 *\return		A constant iterator on the end of the textures array.
		 *\~french
		 *\return		Un itérateur constant sur la fin du tableau de textures.
		 */
		inline auto end()const
		{
			return m_textureUnits.end();
		}
		/**
		 *\~english
		 *\return		An iterator on the end of the textures array.
		 *\~french
		 *\return		Un itérateur sur la fin du tableau de textures.
		 */
		inline auto end()
		{
			return m_textureUnits.end();
		}
		/**
		 *\~english
		 *\return		\p true if gamma correction is needed for this pass.
		 *\~french
		 *\return		\pt true si la correction gamma doit être appliquée à cette passe.
		 */
		inline bool NeedsGammaCorrection()const
		{
			return m_needsGammaCorrection;
		}
		/**
		 *\~english
		 *\return		The pass ID.
		 *\~french
		 *\brief
		 *\return		L'ID de la passe
		 */
		inline uint32_t GetId()const
		{
			return m_id;
		}
		/**
		 *\~english
		 *\brief		Sets the pass ID.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief
		 *\brief		Définit l'ID de la passe
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetId( uint32_t p_value )
		{
			m_id = p_value;
		}
		/**
		 *\~english
		 *\return		The alpha function.
		 *\~french
		 *\return		La fonction d'alpha.
		 */
		inline Castor3D::ComparisonFunc GetAlphaFunc()const
		{
			return m_alphaFunc;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha function.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la fonction d'alpha.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetAlphaFunc( Castor3D::ComparisonFunc p_value )
		{
			m_alphaFunc = p_value;
		}
		/**
		 *\~english
		 *\return		The alpha reference value.
		 *\~french
		 *\return		La valeur de référence pour l'alpha.
		 */
		inline float GetAlphaValue()const
		{
			return m_alphaValue;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha reference value.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la valeur de référence pour l'alpha.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetAlphaValue( float p_value )
		{
			m_alphaValue = p_value;
		}

	protected:
		/**
		 *\~english
		 *\brief			Prepares a texture to be integrated to the pass.
		 *\remarks			Removes alpha channel if any, stores it in p_opacity if it is empty.
		 *\param[in]		p_channel		The texture channel.
		 *\param[in,out]	p_index			The texture index.
		 *\param[in,out]	p_opacitySource	Receives the texture unit if p_opacity is modified.
		 *\param[in,out]	p_opacity		Receives the alpha channel of the texture.
		 *\return			\p true if there were an alpha channel in the texture.
		 *\~french
		 *\brief			Prépare une texture à être intégrée à la passe.
		 *\remarks			Enlève le canal alpha s'il y en avait un, il est stocké dans p_opacity si celui-ci est vide.
		 *\param[in]		p_channel		Le canal de texture.
		 *\param[in,out]	p_index			L'index de la texture.
		 *\param[in,out]	p_opacitySource	Reçoit l'unité de texture si p_opacity est modifié.
		 *\param[in,out]	p_opacity		Reçoit le canal alpha de la texture.
		 *\return			\p true Si la texture possédait un canal alpha.
		 */
		C3D_API bool DoPrepareTexture( TextureChannel p_channel, uint32_t & p_index, TextureUnitSPtr & p_opacitySource, Castor::PxBufferBaseSPtr & p_opacity );
		/**
		 *\~english
		 *\brief			Prepares a texture to be integrated to the pass.
		 *\remarks			Removes alpha channel if any.
		 *\param[in]		p_channel	The texture channel.
		 *\param[in,out]	p_index		The texture index.
		 *\return			The original texture's alpha channel.
		 *\~french
		 *\brief			Prépare une texture à être intégrée à la passe.
		 *\remarks			Enlève le canal alpha s'il y en avait un.
		 *\param[in]		p_channel	Le canal de texture.
		 *\param[in,out]	p_index		L'index de la texture.
		 *\return			Le canal alpha de la texture originale.
		 */
		C3D_API Castor::PxBufferBaseSPtr DoPrepareTexture( TextureChannel p_channel, uint32_t & p_index );
		/**
		 *\~english
		 *\brief			Prepares the opacity channel.
		 *\param[in]		p_opacitySource	The texture unit from which opacity comes.
		 *\param[in]		p_opacityImage	The alpha channel from p_opacitySource.
		 *\param[in,out]	p_index			The texture index.
		 *\~french
		 *\brief			Prépare le canal d'opacité.
		 *\param[in]		p_opacitySource	L'unité de texture depuis laquelle l'opacité provient.
		 *\param[in]		p_opacityImage	Le canal alpha de p_opacitySource.
		 *\param[in,out]	p_index			L'index de la texture.
		 */
		C3D_API void DoPrepareOpacity( TextureUnitSPtr p_opacitySource, Castor::PxBufferBaseSPtr p_opacityImage, uint32_t & p_index );
		/**
		 *\~english
		 *\brief		Updates the texture flags depending on the texture units.
		 *\~french
		 *\brief		Met à jour les indicateurs de texture en fonction des unités de texture.
		 */
		C3D_API void DoUpdateFlags();

	private:
		/**
		 *\~english
		 *\brief		Initialises the pass and all it's dependencies.
		 *\~french
		 *\brief		Initialise la passe et toutes ses dépendances.
		 */
		virtual void DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans up the pass and all it's dependencies.
		 *\~french
		 *\brief		Nettoie la passe et toutes ses dépendances.
		 */
		virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Fills shader variables of given render node.
		 *\~french
		 *\brief		Remplit les variables de shader du noeud de rendu donné.
		 */
		virtual void DoUpdate( PassBuffer & p_buffer )const = 0;
		/**
		 *\~english
		 *\brief		Sets the global alpha value.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la valeur alpha globale.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		virtual void DoSetOpacity( float p_value ) = 0;

	public:
		static uint32_t constexpr PassBufferIndex = 0u;
		static uint32_t constexpr LightBufferIndex = 1u;
		static uint32_t constexpr MinTextureIndex = 2u;

	public:
		OnChanged onChanged;

	private:
		//!\~english	Texture units.
		//!\~french		Les textures.
		TextureUnitPtrArray m_textureUnits;
		//!\~english	Bitwise ORed TextureChannel.
		//!\~french		Combinaison des TextureChannel affectés à une texture pour cette passe.
		TextureChannels m_textureFlags;
		//!\~english	The pass ID.
		//!\~french		L'ID de la passe.
		uint32_t m_id{ 0u };
		//!\~english	The opacity value.
		//!\~french		La valeur d'opacité.
		float m_opacity{ 1.0f };
		//!\~english	The emission factor.
		//!\~french		Le facteur d'émission.
		float m_emissive{ 0.0f };
		//!\~english	The refraction ratio.
		//!\~french		Le ratio de réfraction.
		float m_refractionRatio{ 0.0f };
		//!\~english	Tells if the pass is two sided.
		//!\~french		Dit si la passe s'applique sur les deux faces.
		bool m_twoSided{ false };
		//!\~english	Tells the pass shader is an automatically generated one.
		//!\~french		Dit si le shader de la passe a été généré automatiquement.
		bool m_automaticShader{ true };
		//!\~english	The alpha blend mode.
		//!\~french		Le mode de mélange alpha.
		BlendMode m_alphaBlendMode{ BlendMode::eNoBlend };
		//!\~english	The colour blend mode.
		//!\~french		Le mode de mélange couleur.
		BlendMode m_colourBlendMode{ BlendMode::eNoBlend };
		//!\~english	The reference alpha value for alpha comparison.
		//!\~french		La valeur d'alpha de référence pour la comparaison d'alpha.
		float m_alphaValue{ 0.0f };
		//!\~english	The alpha function for alpha comparison.
		//!\~french		La fonction d'alpha utilisée lors de la comparaison d'alpha.
		ComparisonFunc m_alphaFunc{ ComparisonFunc::eAlways };
		//!\~english	Tells if the pass' textures are reduced.
		//!\~french		Dit si les textures de la passe sont réduites.
		bool m_texturesReduced{ false };
		//!\~english	Tells if the pass' diffuse needs gamma correction.
		//!\~french		Dit si la diffuse de la passe a besoin de correction gamma.
		bool m_needsGammaCorrection{ false };
	};
}

#endif
