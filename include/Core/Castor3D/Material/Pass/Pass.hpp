/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PASS_H___
#define ___C3D_PASS_H___

#include "PassModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Shader/PassBuffer/PassBufferModule.hpp"

#include <CastorUtils/Design/FlagCombination.hpp>
#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	class Pass
		: public castor::OwnedBy< Material >
	{
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
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise la passe et toutes ses dépendances.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void initialise( RenderDevice const & device );
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
		C3D_API void setSubsurfaceScattering( SubsurfaceScatteringUPtr value );
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
		C3D_API bool needsAlphaProcessing()const;
		C3D_API bool hasAlphaBlending()const;
		C3D_API bool hasOnlyAlphaBlending()const;
		C3D_API bool hasAlphaTest()const;
		C3D_API bool hasBlendAlphaTest()const;
		C3D_API bool needsGammaCorrection()const;
		C3D_API TextureUnitPtrArray getTextureUnits( TextureFlags mask = TextureFlag::eAll )const;
		C3D_API uint32_t getTextureUnitsCount( TextureFlags mask = TextureFlag::eAll )const;
		C3D_API TextureFlagsArray getTextures( TextureFlags mask )const;

		TextureFlags const & getTextures()const
		{
			return m_textures;
		}

		bool hasAutomaticShader()const
		{
			return m_automaticShader;
		}

		bool isTwoSided()const
		{
			return m_twoSided;
		}

		bool hasEnvironmentMapping()const
		{
			return checkFlag( m_flags, PassFlag::eReflection )
				|| checkFlag( m_flags, PassFlag::eRefraction );
		}

		float getOpacity()const
		{
			return m_opacity;
		}

		uint32_t getBWAccumulationOperator()const
		{
			return m_bwAccumulationOperator.value();
		}

		float getEmissive()const
		{
			return m_emissive;
		}

		float getRefractionRatio()const
		{
			return m_refractionRatio;
		}

		BlendMode getAlphaBlendMode()const
		{
			return m_alphaBlendMode;
		}

		BlendMode getColourBlendMode()const
		{
			return m_colourBlendMode;
		}

		uint32_t getId()const
		{
			return m_id;
		}

		VkCompareOp getAlphaFunc()const
		{
			return m_alphaFunc;
		}

		float getAlphaValue()const
		{
			return m_alphaValue;
		}

		VkCompareOp getBlendAlphaFunc()const
		{
			return m_blendAlphaFunc;
		}

		castor::Point3f getTransmission()const
		{
			return m_transmission;
		}

		bool hasSubsurfaceScattering()const
		{
			return checkFlag( m_flags, PassFlag::eSubsurfaceScattering )
				&& m_subsurfaceScattering != nullptr;
		}

		ParallaxOcclusionMode getParallaxOcclusion()const
		{
			return m_parallaxOcclusionMode;
		}

		bool hasParallaxOcclusion()const
		{
			return m_parallaxOcclusionMode != ParallaxOcclusionMode::eNone;
		}

		SubsurfaceScattering const & getSubsurfaceScattering()const
		{
			CU_Require( m_subsurfaceScattering );
			return *m_subsurfaceScattering;
		}

		bool isImplicit()const
		{
			return m_implicit;
		}

		bool hasReflections()const
		{
			return checkFlag( m_flags, PassFlag::eReflection );
		}

		bool hasRefraction()const
		{
			return checkFlag( m_flags, PassFlag::eRefraction );
		}

		uint32_t getHeightTextureIndex()const
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
		auto begin()const
		{
			return m_textureUnits.begin();
		}

		auto begin()
		{
			return m_textureUnits.begin();
		}

		auto end()const
		{
			return m_textureUnits.end();
		}

		auto end()
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
		void setTwoSided( bool value )
		{
			m_twoSided = value;
			onChanged( *this );
		}

		void setEmissive( float const & value )
		{
			m_emissive = value;
			onChanged( *this );
		}

		void setRefractionRatio( float value )
		{
			m_refractionRatio = value;
			onChanged( *this );
		}

		void setTransmission( castor::Point3f value )
		{
			m_transmission = std::move( value );
			onChanged( *this );
		}

		void setParallaxOcclusion( ParallaxOcclusionMode value )
		{
			m_parallaxOcclusionMode = value;
			updateFlag( PassFlag::eParallaxOcclusionMappingOne
				, m_parallaxOcclusionMode == ParallaxOcclusionMode::eOne );
			updateFlag( PassFlag::eParallaxOcclusionMappingRepeat
				, m_parallaxOcclusionMode == ParallaxOcclusionMode::eRepeat );
			onChanged( *this );
		}

		void setAlphaBlendMode( BlendMode value )
		{
			m_alphaBlendMode = value;
			updateFlag( PassFlag::eAlphaBlending, hasAlphaBlending() );
			onChanged( *this );
		}

		void setColourBlendMode( BlendMode value )
		{
			m_colourBlendMode = value;
			onChanged( *this );
		}

		void setId( uint32_t value )
		{
			m_id = value;
		}

		void setAlphaFunc( VkCompareOp value )
		{
			m_alphaFunc = value;
			updateFlag( PassFlag::eAlphaTest, hasAlphaTest() );
		}

		void setAlphaValue( float value )
		{
			m_alphaValue = value;
			onChanged( *this );
		}

		void setBlendAlphaFunc( VkCompareOp value )
		{
			m_blendAlphaFunc = value;
			updateFlag( PassFlag::eBlendAlphaTest, hasBlendAlphaTest() );
		}

		void setBWAccumulationOperator( uint32_t value )
		{
			m_bwAccumulationOperator = value;
			onChanged( *this );
		}

		void enableReflections( bool value = true )
		{
			updateFlag( PassFlag::eReflection, value );
		}

		void enableRefractions( bool value = true )
		{
			updateFlag( PassFlag::eRefraction, value );
		}

		void setImplicit( bool value = true )
		{
			m_implicit = value;
		}
		/**@}*/

	private:
		void onSssChanged( SubsurfaceScattering const & sss );
		virtual void doInitialise() = 0;
		virtual void doCleanup() = 0;
		virtual void doSetOpacity( float value ) = 0;

		void updateFlag( PassFlag flag
			, bool value )
		{
			if ( value )
			{
				addFlag( m_flags, flag );
			}
			else
			{
				remFlag( m_flags, flag );
			}

			onChanged( *this );
		}

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
		castor::Point3f m_transmission{ 1.0f, 1.0f, 1.0f };
		VkCompareOp m_alphaFunc{ VK_COMPARE_OP_ALWAYS };
		VkCompareOp m_blendAlphaFunc{ VK_COMPARE_OP_ALWAYS };
		bool m_texturesReduced{ false };
		SubsurfaceScatteringUPtr m_subsurfaceScattering;
		SubsurfaceScattering::OnChangedConnection m_sssConnection;
		uint32_t m_heightTextureIndex{ InvalidIndex };
		PassFlags m_flags;
		ParallaxOcclusionMode m_parallaxOcclusionMode{ ParallaxOcclusionMode::eNone };
	};
}

#endif
