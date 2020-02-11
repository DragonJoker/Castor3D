/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PASS_H___
#define ___C3D_PASS_H___

#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Shader/PassBuffer/PassBufferModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	class Pass
		: public castor::OwnedBy< Material >
	{
	public:
		/**
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
			C3D_API bool operator()( Pass const & pass
				, castor::TextFile & file )override;
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
		 *\brief		adds a texture unit.
		 *\param[in]	unit	The texture unit.
		 *\~french
		 *\brief		Ajoute une unité de texture.
		 *\param[in]	unit	L'unité de texture.
		 */
		C3D_API void addTextureUnit( TextureUnitSPtr unit );
		/**
		 *\~english
		 *\brief		Removes the TextureUnit at the given index.
		 *\param[in]	index	the index of the TextureUnit to remove.
		 *\~french
		 *\brief		Retire la TextureUnit à l'index donné.
		 *\param[in]	index	L'index de la TextureUnit à retirer.
		 */
		C3D_API void removeTextureUnit( uint32_t index );
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
		 *\brief		Sets the subsurface scattering extended informations.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit les informations étendues pour le subsurface scattering.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setSubsurfaceScattering( SubsurfaceScatteringUPtr && value );
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
		 *\brief			Fills the pass buffer with this pass data.
		 *\param[in,out]	buffer	The pass buffer.
		 *\~french
		 *\brief			Remplit le pass buffer aves les données de cette passe.
		 *\param[in,out]	buffer	Le pass buffer.
		 */
		C3D_API virtual void accept( PassBuffer & buffer )const = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API bool needsGammaCorrection()const;
		C3D_API uint32_t getNonEnvTextureUnitsCount()const;

		inline TextureFlags const & getTextures()const
		{
			return m_textures;
		}

		inline bool hasAutomaticShader()const
		{
			return m_automaticShader;
		}

		inline uint32_t getTextureUnitsCount()const
		{
			return uint32_t( m_textureUnits.size() );
		}

		inline bool IsTwoSided()const
		{
			return m_twoSided;
		}

		inline bool hasEnvironmentMapping()const
		{
			return checkFlag( m_textures, TextureFlag::eReflection )
				|| checkFlag( m_textures, TextureFlag::eRefraction );
		}

		inline float getOpacity()const
		{
			return m_opacity;
		}

		inline uint32_t getBWAccumulationOperator()const
		{
			return m_bwAccumulationOperator.value();
		}

		inline float getEmissive()const
		{
			return m_emissive;
		}

		inline float getRefractionRatio()const
		{
			return m_refractionRatio;
		}

		inline BlendMode getAlphaBlendMode()const
		{
			return m_alphaBlendMode;
		}

		inline BlendMode getColourBlendMode()const
		{
			return m_colourBlendMode;
		}

		inline uint32_t getId()const
		{
			return m_id;
		}

		inline VkCompareOp getAlphaFunc()const
		{
			return m_alphaFunc;
		}

		inline float getAlphaValue()const
		{
			return m_alphaValue;
		}

		inline bool hasSubsurfaceScattering()const
		{
			return m_subsurfaceScattering != nullptr;
		}

		inline bool hasParallaxOcclusion()const
		{
			return m_parallaxOcclusion;
		}

		inline SubsurfaceScattering const & getSubsurfaceScattering()const
		{
			CU_Require( m_subsurfaceScattering );
			return *m_subsurfaceScattering;
		}

		inline bool isImplicit()const
		{
			return m_implicit;
		}

		inline uint32_t getHeightTextureIndex()const
		{
			return m_heightTextureIndex;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Textures iteration.
		*\~french
		*name
		*	Itération sur les textures.
		*/
		/**@{*/
		inline auto begin()const
		{
			return m_textureUnits.begin();
		}

		inline auto begin()
		{
			return m_textureUnits.begin();
		}

		inline auto end()const
		{
			return m_textureUnits.end();
		}

		inline auto end()
		{
			return m_textureUnits.end();
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		inline void setTwoSided( bool value )
		{
			m_twoSided = value;
			onChanged( *this );
		}

		inline void setEmissive( float const & value )
		{
			m_emissive = value;
			onChanged( *this );
		}

		inline void setRefractionRatio( float value )
		{
			m_refractionRatio = value;
			onChanged( *this );
		}

		inline void setParallaxOcclusion( bool value )
		{
			m_parallaxOcclusion = value;
			onChanged( *this );
		}

		inline void setAlphaBlendMode( BlendMode value )
		{
			m_alphaBlendMode = value;
			onChanged( *this );
		}

		inline void setColourBlendMode( BlendMode value )
		{
			m_colourBlendMode = value;
			onChanged( *this );
		}

		inline void setId( uint32_t value )
		{
			m_id = value;
		}

		inline void setAlphaFunc( VkCompareOp value )
		{
			m_alphaFunc = value;
			onChanged( *this );
		}

		inline void setAlphaValue( float value )
		{
			m_alphaValue = value;
			onChanged( *this );
		}

		inline void setBWAccumulationOperator( uint32_t value )
		{
			m_bwAccumulationOperator = value;
			onChanged( *this );
		}

		inline void setImplicit( bool value = true )
		{
			m_implicit = value;
		}
		/**@}*/

	private:
		void onSssChanged( SubsurfaceScattering const & sss );
		virtual void doInitialise() = 0;
		virtual void doCleanup() = 0;
		virtual void doSetOpacity( float value ) = 0;

	public:
		OnPassChanged onChanged;

	private:
		TextureUnitPtrArray m_textureUnits;
		TextureFlags m_textures;
		uint32_t m_id{ 0u };
		bool m_implicit{ false };
		float m_opacity{ 1.0f };
		castor::RangedValue< uint32_t > m_bwAccumulationOperator{ castor::makeRangedValue( 1u, 0u, 8u ) };
		float m_emissive{ 0.0f };
		float m_refractionRatio{ 0.0f };
		bool m_twoSided{ false };
		bool m_automaticShader{ true };
		BlendMode m_alphaBlendMode{ BlendMode::eNoBlend };
		BlendMode m_colourBlendMode{ BlendMode::eNoBlend };
		float m_alphaValue{ 0.0f };
		VkCompareOp m_alphaFunc{ VK_COMPARE_OP_ALWAYS };
		bool m_texturesReduced{ false };
		bool m_parallaxOcclusion{ false };
		SubsurfaceScatteringUPtr m_subsurfaceScattering;
		SubsurfaceScattering::OnChangedConnection m_sssConnection;
		uint32_t m_heightTextureIndex{ InvalidIndex };
	};
}

#endif
