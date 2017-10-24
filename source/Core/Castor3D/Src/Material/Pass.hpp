/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PASS_H___
#define ___C3D_PASS_H___

#include "SubsurfaceScattering.hpp"

#include <Design/OwnedBy.hpp>
#include <Design/Signal.hpp>

namespace castor3d
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
		: public castor::OwnedBy< Material >
	{
	public:
		using Changed = std::function< void( Pass const & ) >;
		using OnChanged = castor::Signal< Changed >;
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
			: public castor::TextWriter< Pass >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief			Writes a Pass into a text file.
			 *\param[in]		pass	The Pass to write.
			 *\param[in,out]	file	The file where to write the Pass.
			 *\~french
			 *\brief			Ecrit une Pass dans un fichier texte.
			 *\param[in]		pass	La Pass à écrire.
			 *\param[in,out]	file	Le file où écrire la Pass.
			 */
			C3D_API bool operator()( Pass const & pass, castor::TextFile & file )override;
		};

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	parent	The parent material.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parent	Le matériau parent.
		 */
		C3D_API explicit Pass( Material & parent );
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
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Cleans up the pass and all it's dependencies.
		 *\~french
		 *\brief		Nettoie la passe et toutes ses dépendances.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Binds the pass' textures.
		 *\~french
		 *\brief		Active les textures de la passe.
		 */
		C3D_API void bindTextures();
		/**
		 *\~english
		 *\brief		Unbinds the pass' textures.
		 *\~french
		 *\brief		Désactive les textures de la passe.
		 */
		C3D_API void unbindTextures();
		/**
		 *\~english
		 *\brief		adds a texture unit.
		 *\param[in]	p_unit	The texture unit.
		 *\~french
		 *\brief		Ajoute une unité de texture.
		 *\param[in]	p_unit	L'unité de texture.
		 */
		C3D_API void addTextureUnit( TextureUnitSPtr p_unit );
		/**
		 *\~english
		 *\brief		Retrieves the TextureUnit at wanted channel.
		 *\remarks		If more than one TextureUnits are found at given channel, the first one is returned.
		 *\param[in]	channel	The channel.
		 *\return		\p nullptr if no TextureUnit at wanted channel.
		 *\~french
		 *\brief		Récupère la TextureUnit au canal demandé.
		 *\remarks		Si plus d'une TextureUnit est trouvée pour le canal demandé, la première est retournée.
		 *\param[in]	channel	Le canal.
		 *\return		\p nullptr si pas de TextureUnit au canal voulu.
		 */
		C3D_API TextureUnitSPtr getTextureUnit( TextureChannel channel )const;
		/**
		 *\~english
		 *\brief		Destroys a TextureUnit at the given index.
		 *\param[in]	index	the index of the TextureUnit to destroy.
		 *\~french
		 *\brief		Détruit la TextureUnit à l'index donné.
		 *\param[in]	index	L'index de la TextureUnit à détruire.
		 */
		C3D_API void destroyTextureUnit( uint32_t index );
		/**
		 *\~english
		 *\brief		Retrieves the TextureUnit at the given index.
		 *\param[in]	index	The index of the TextureUnit to retrieve.
		 *\return		The retrieved TextureUnit, nullptr if none.
		 *\~french
		 *\brief		Récupère la TextureUnit à l'index donné.
		 *\param[in]	index	L'index voulu.
		 *\return		La TextureUnit récupérée, nullptr si index était hors bornes.
		 */
		C3D_API TextureUnitSPtr getTextureUnit( uint32_t index )const;
		/**
		 *\~english
		 *\brief		Tells if the pass needs alpha blending.
		 *\return		\p true if at least one texture unit has an alpha channel.
		 *\~french
		 *\brief		Dit si la passe a besoin de mélange d'alpha.
		 *\return		\p true si au moins une unité de texture a un canal alpha.
		 */
		C3D_API bool hasAlphaBlending()const;
		/**
		 *\~english
		 *\brief		Reduces the textures.
		 *\~french
		 *\brief		Réduit les textures.
		 */
		C3D_API void prepareTextures();
		/**
		 *\~english
		 *\return		The material type.
		 *\~french
		 *\return		Le type de matériau.
		 */
		C3D_API MaterialType getType()const;
		/**
		 *\~english
		 *\brief		Sets the global alpha value.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la valeur alpha globale.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setOpacity( float value );
		/**
		 *\~english
		 *\return		The pass flags combination.
		 *\~french
		 *\return		La combinaison d'indicateurs de passe.
		 */
		C3D_API PassFlags getPassFlags()const;
		/**
		 *\~english
		 *\remarks	Passes are aligned on float[4], so the size of a pass
		 *			is the number of float[4] needed to contain it.
		 *\~french
		 *\remarks	Les passes sont alignées sur 4 flottants, donc la taille d'une passe
		 *			correspond aux nombres de float[4] qu'il faut pour la contenir.
		 */
		C3D_API virtual uint32_t getPassSize()const = 0;
		/**
		 *\~english
		 *\brief		Fills shader variables of given render node.
		 *\~french
		 *\brief		Remplit les variables de shader du noeud de rendu donné.
		 */
		C3D_API virtual void accept( PassBuffer & buffer )const = 0;
		/**
		 *\~english
		 *\return		The texture channels flags combination.
		 *\~french
		 *\return		La combinaison d'indicateurs de canal de texture.
		 */
		inline TextureChannels const & getTextureFlags()const
		{
			return m_textureFlags;
		}
		/**
		 *\~english
		 *\return		\p true if the pass shader is automatically generated.
		 *\~french
		 *\return		\p true si le shader de la passe est généré automatiquement.
		 */
		inline bool hasAutomaticShader()const
		{
			return m_automaticShader;
		}
		/**
		 *\~english
		 *\brief		Sets the two sided status.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le statut d'application aux deux faces.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setTwoSided( bool value )
		{
			m_twoSided = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the emissive factor.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le facteur d'émission.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setEmissive( float const & value )
		{
			m_emissive = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the refraction ratio.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le ratio de réfraction.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setRefractionRatio( float value )
		{
			m_refractionRatio = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the parallax occlusion mapping usage.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'utilisation du parallax occlusion mapping.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setParallaxOcclusion( bool value )
		{
			m_parallaxOcclusion = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the alpha blend mode.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le mode de mélange alpha.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setAlphaBlendMode( BlendMode value )
		{
			m_alphaBlendMode = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Sets the colour blend mode.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le mode de mélange couleur.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setColourBlendMode( BlendMode value )
		{
			m_colourBlendMode = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\return		The texture units count.
		 *\~french
		 *\return		Le nombre d'unités de texture.
		 */
		inline uint32_t getTextureUnitsCount()const
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
		inline bool hasEnvironmentMapping()const
		{
			return checkFlag( getTextureFlags(), TextureChannel::eReflection )
				|| checkFlag( getTextureFlags(), TextureChannel::eRefraction );
		}
		/**
		 *\~english
		 *\return		The global opacity value.
		 *\~french
		 *\return		La valeur globale d'opacité.
		 */
		inline float getOpacity()const
		{
			return m_opacity;
		}
		/**
		 *\~english
		 *\return		The emissive factor.
		 *\~french
		 *\return		Le facteur émission.
		 */
		inline float getEmissive()const
		{
			return m_emissive;
		}
		/**
		 *\~english
		 *\return		The refraction ratio.
		 *\~french
		 *\return		Le ration de réfraction.
		 */
		inline float getRefractionRatio()const
		{
			return m_refractionRatio;
		}
		/**
		 *\~english
		 *\return		The alpha blend mode.
		 *\~french
		 *\return		Le mode de mélange alpha.
		 */
		inline BlendMode getAlphaBlendMode()const
		{
			return m_alphaBlendMode;
		}
		/**
		 *\~english
		 *\return		The colour blend mode.
		 *\~french
		 *\return		Le mode de mélange couleur.
		 */
		inline BlendMode getColourBlendMode()const
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
		inline bool needsGammaCorrection()const
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
		inline uint32_t getId()const
		{
			return m_id;
		}
		/**
		 *\~english
		 *\brief		Sets the pass ID.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief
		 *\brief		Définit l'ID de la passe
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setId( uint32_t value )
		{
			m_id = value;
		}
		/**
		 *\~english
		 *\return		The alpha function.
		 *\~french
		 *\return		La fonction d'alpha.
		 */
		inline castor3d::ComparisonFunc getAlphaFunc()const
		{
			return m_alphaFunc;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha function.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la fonction d'alpha.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setAlphaFunc( castor3d::ComparisonFunc value )
		{
			m_alphaFunc = value;
		}
		/**
		 *\~english
		 *\return		The alpha reference value.
		 *\~french
		 *\return		La valeur de référence pour l'alpha.
		 */
		inline float getAlphaValue()const
		{
			return m_alphaValue;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha reference value.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la valeur de référence pour l'alpha.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setAlphaValue( float value )
		{
			m_alphaValue = value;
		}
		/**
		 *\~english
		 *\return		\p true if the pass has subsurface scattering extended informations.
		 *\~french
		 *\return		\p true si la passe a des informations étendues pour le subsurface scattering.
		 */
		inline bool hasSubsurfaceScattering()const
		{
			return m_subsurfaceScattering != nullptr;
		}
		/**
		 *\~english
		 *\return		\p true if the pass uses parallax occlusion mapping.
		 *\~french
		 *\return		\p true si la passe utilise le parallax occlusion mapping.
		 */
		inline bool hasParallaxOcclusion()const
		{
			return m_parallaxOcclusion;
		}
		/**
		 *\~english
		 *\return		The subsurface scattering extended informations.
		 *\~french
		 *\return		Les informations étendues pour le subsurface scattering.
		 */
		inline SubsurfaceScattering const & getSubsurfaceScattering()const
		{
			REQUIRE( m_subsurfaceScattering );
			return *m_subsurfaceScattering;
		}
		/**
		 *\~english
		 *\brief		Sets the subsurface scattering extended informations.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit les informations étendues pour le subsurface scattering.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setSubsurfaceScattering( SubsurfaceScatteringUPtr && value )
		{
			m_subsurfaceScattering = std::move( value );
		}

	protected:
		/**
		 *\~english
		 *\brief			Prepares a texture to be integrated to the pass.
		 *\remarks			Removes alpha channel if any, stores it in opacity if it is empty.
		 *\param[in]		channel			The texture channel.
		 *\param[in,out]	index			The texture index.
		 *\param[in,out]	opacitySource	Receives the texture unit if opacity is modified.
		 *\param[in,out]	opacity			Receives the alpha channel of the texture.
		 *\return			\p true if there were an alpha channel in the texture.
		 *\~french
		 *\brief			Prépare une texture à être intégrée à la passe.
		 *\remarks			Enlève le canal alpha s'il y en avait un, il est stocké dans opacity si celui-ci est vide.
		 *\param[in]		channel			Le canal de texture.
		 *\param[in,out]	index			L'index de la texture.
		 *\param[in,out]	opacitySource	Reçoit l'unité de texture si opacity est modifié.
		 *\param[in,out]	opacity			Reçoit le canal alpha de la texture.
		 *\return			\p true Si la texture possédait un canal alpha.
		 */
		bool doPrepareTexture( TextureChannel channel
			, uint32_t & index
			, TextureUnitSPtr & opacitySource
			, castor::PxBufferBaseSPtr & opacity );
		/**
		 *\~english
		 *\brief			Prepares a texture to be integrated to the pass.
		 *\remarks			Removes alpha channel if any.
		 *\param[in]		channel	The texture channel.
		 *\param[in,out]	index	The texture index.
		 *\return			The original texture's alpha channel.
		 *\~french
		 *\brief			Prépare une texture à être intégrée à la passe.
		 *\remarks			Enlève le canal alpha s'il y en avait un.
		 *\param[in]		channel	Le canal de texture.
		 *\param[in,out]	index	L'index de la texture.
		 *\return			Le canal alpha de la texture originale.
		 */
		castor::PxBufferBaseSPtr doPrepareTexture( TextureChannel channel
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Prepares the opacity channel.
		 *\param[in]		opacitySource	The texture unit from which opacity comes.
		 *\param[in]		opacityImage	The alpha channel from opacitySource.
		 *\param[in,out]	index			The texture index.
		 *\~french
		 *\brief			Prépare le canal d'opacité.
		 *\param[in]		opacitySource	L'unité de texture depuis laquelle l'opacité provient.
		 *\param[in]		opacityImage	Le canal alpha de opacitySource.
		 *\param[in,out]	index			L'index de la texture.
		 */
		void doPrepareOpacity( TextureUnitSPtr opacitySource
			, castor::PxBufferBaseSPtr opacityImage
			, uint32_t & index );
		/**
		 *\~english
		 *\brief		Updates the texture flags depending on the texture units.
		 *\~french
		 *\brief		Met à jour les indicateurs de texture en fonction des unités de texture.
		 */
		void doUpdateFlags();
		/**
		 *\~english
		 *\brief		Reduces the texture at given channel to given pixel format.
		 *\param[in]	channel	The texture channel.
		 *\param[in]	format	The wanted pixel format.
		 *\~french
		 *\brief		Réduit la texture au canal donné dans le format de pixels donné.
		 *\param[in]	channel	Le canal de texture.
		 *\param[in]	format	Le format de pixels voulu.
		 */
		void doReduceTexture( TextureChannel channel, castor::PixelFormat format );

	private:
		/**
		 *\~english
		 *\brief		Initialises the pass and all it's dependencies.
		 *\~french
		 *\brief		Initialise la passe et toutes ses dépendances.
		 */
		virtual void doInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans up the pass and all it's dependencies.
		 *\~french
		 *\brief		Nettoie la passe et toutes ses dépendances.
		 */
		virtual void doCleanup() = 0;
		/**
		 *\~english
		 *\brief		Sets the global alpha value.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la valeur alpha globale.
		 *\param[in]	value	La nouvelle valeur.
		 */
		virtual void doSetOpacity( float value ) = 0;

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
		//!\~english	Tells if the pass uses parallax occlusion mapping.
		//!\~french		Dit si la passe utilise le parallax occlusion mapping.
		bool m_parallaxOcclusion{ false };
		//!\~english	The subsurface scattering extended informations.
		//!\~french		Les informations étendus pour le subsurface scattering.
		SubsurfaceScatteringUPtr m_subsurfaceScattering;
	};
}

#endif
