/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PASS_H___
#define ___C3D_PASS_H___

#include "PassModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Material/Pass/Component/PassComponent.hpp"
#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"

#include "Castor3D/Material/Pass/SubsurfaceScattering.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"

#include <CastorUtils/Design/FlagCombination.hpp>
#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Math/RangedValue.hpp>
#include <CastorUtils/Multithreading/SpinMutex.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	class Pass
		: public castor::OwnedBy< Material >
	{
		friend struct PassComponent;

	public:
		using UnitArray = std::vector< TextureUnitRPtr >;
		using PassTextureSource = std::pair< TextureSourceInfo, PassTextureConfig >;
		using TextureSourceArray = std::vector< PassTextureSource >;
		using UnitDataSources = std::map< TextureUnitDataRPtr, std::vector< TextureSourceInfo > >;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	parent			The parent material.
		 *\param[in]	lightingModelId	The material's lighting model ID.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parent			Le matériau parent.
		 *\param[in]	lightingModelId	L'ID du modèle d'éclairage du matériau.
		 */
		C3D_API explicit Pass( Material & parent
			, LightingModelID lightingModelId );
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\param[in]	parent	The parent material.
		 *\param[in]	rhs		The pass top copy.
		 *\~french
		 *\brief		Constructeur par copie.
		 *\param[in]	parent	Le matériau parent.
		 *\param[in]	rhs		La passe à copier.
		 */
		C3D_API Pass( Material & parent
			, Pass const & rhs );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Pass()noexcept;
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
		 *\brief			Updates the render pass, CPU wise.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 */
		C3D_API void update();
		/**
		*\~english
		*\name
		*	Components handling.
		*\~french
		*\name
		*	Gestion des composants.
		*/
		/**@{*/
		C3D_API void addComponent( PassComponentUPtr component );
		C3D_API bool hasComponent( castor::String const & name )const;
		C3D_API PassComponent * getComponent( castor::String const & name )const;
		C3D_API std::vector< PassComponentUPtr > removeComponent( castor::String const & name );
		C3D_API shader::PassMaterialShader * getMaterialShader( castor::String const & componentType )const;
		C3D_API PassComponentID getComponentId( castor::String const & componentType )const;
		C3D_API PassComponentPlugin const & getComponentPlugin( PassComponentID componentId )const;
		C3D_API PassComponentCombineID getComponentCombineID()const;
		C3D_API TextureCombineID getTextureCombineID()const;

		uint32_t getHash()const noexcept
		{
			return ( uint32_t( getComponentCombineID() ) << 16u )
				| uint32_t( getTextureCombineID() );
		}

		PassComponentPlugin const & getComponentPlugin( castor::String const & componentType )const
		{
			return getComponentPlugin( getComponentId( componentType ) );
		}

		template< typename ComponentT >
		PassComponentPlugin const & getComponentPlugin()const
		{
			return getComponentPlugin( getComponentId( ComponentT::TypeName ) );
		}

		template< typename ComponentT >
		void removeComponent()
		{
			removeComponent( ComponentT::TypeName );
		}

		template< typename ComponentT, typename ... ParamsT >
		ComponentT * createComponent( ParamsT && ... params )
		{
			auto result = getComponent< ComponentT >();

			if ( !result )
			{
				auto component = castor::makeUnique< ComponentT >( *this
					, std::forward< ParamsT >( params )... );
				result = component.get();
				this->addComponent( castor::ptrRefCast< PassComponent >( component ) );
			}

			return result;
		}

		template< typename ComponentT >
		bool hasComponent()const
		{
			return this->hasComponent( ComponentT::TypeName );
		}

		template< typename ComponentT >
		ComponentT * getComponent()const
		{
			return this->hasComponent< ComponentT >()
				? &static_cast< ComponentT & >( *this->getComponent( ComponentT::TypeName ) )
				: nullptr;
		}

		PassComponentMap const & getComponents()const
		{
			return m_components;
		}
		/**@}*/
		/**
		 *\~english
		 *\brief		Retrieves the TextureUnit at the given index.
		 *\param[in]	index	The index of the TextureUnit to retrieve.
		 *\return		\p nullptr if index was out of bounds.
		 *\~french
		 *\brief		Récupère la TextureUnit à l'index donné.
		 *\param[in]	index	L'index voulu.
		 *\return		\p nullptr si index était hors bornes.
		 */
		C3D_API TextureUnitRPtr getTextureUnit( uint32_t index )const;
		/**
		 *\~english
		 *\brief		Adds a texture.
		 *\param[in]	sourceInfo		The texture source.
		 *\param[in]	configuration	The texture configuration.
		 *\~french
		 *\brief		Ajoute une texture.
		 *\param[in]	sourceInfo		La source de la texture.
		 *\param[in]	configuration	La configuration de la texture.
		 */
		C3D_API void registerTexture( TextureSourceInfo sourceInfo
			, PassTextureConfig configuration );
		/**
		 *\~english
		 *\brief		Adds an animated texture.
		 *\param[in]	sourceInfo		The texture source.
		 *\param[in]	configuration	The texture configuration.
		 *\param[in]	animation		The texture animation.
		 *\~french
		 *\brief		Ajoute une texture animée.
		 *\param[in]	sourceInfo		La source de la texture.
		 *\param[in]	configuration	La configuration de la texture.
		 *\param[in]	animation		L'animation de la texture.
		 */
		C3D_API void registerTexture( TextureSourceInfo sourceInfo
			, PassTextureConfig configuration
			, TextureAnimationUPtr animation );
		/**
		 *\~english
		 *\brief		Removes a texture unit.
		 *\param[in]	sourceInfo	The texture source.
		 *\~french
		 *\brief		Supprime une unité de texture.
		 *\param[in]	sourceInfo	La source de la texture.
		 */
		C3D_API void unregisterTexture( TextureSourceInfo sourceInfo );
		/**
		 *\~english
		 *\brief		Replaces a texture source.
		 *\param[in]	srcSourceInfo	The original texture source.
		 *\param[in]	dstSourceInfo	The replacement texture source.
		 *\~french
		 *\brief		Remplace la source d'une texture.
		 *\param[in]	srcSourceInfo	La source d'origine de la texture.
		 *\param[in]	dstSourceInfo	La source de remplacement de la texture.
		 */
		C3D_API void resetTexture( TextureSourceInfo const & srcSourceInfo
			, TextureSourceInfo dstSourceInfo );
		/**
		 *\~english
		 *\brief		Updates the configuration for a texture.
		 *\param[in]	sourceInfo		The texture source.
		 *\param[in]	configuration	The new texture configuration.
		 *\~french
		 *\brief		Met à jour la configuration d'une texture.
		 *\param[in]	sourceInfo		La source de la texture.
		 *\param[in]	configuration	La nouvelle configuration de la texture.
		 */
		C3D_API void updateConfig( TextureSourceInfo const & sourceInfo
			, TextureConfiguration configuration );
		/**
		 *\~english
		 *\brief		Reduces the textures.
		 *\~french
		 *\brief		Réduit les textures.
		 */
		C3D_API void prepareTextures();
		/**
		 *\~english
		 *\brief		Sets the basic pass colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur basique de la passe.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setColour( castor::HdrRgbColour const & value );
		/**
		 *\~english
		 *\return		The basic pass colour.
		 *\~french
		 *\return		La couleur basique de la passe.
		 */
		C3D_API castor::HdrRgbColour const & getColour()const;
		/**
		 *\~english
		 *\return		The pass flags combination.
		 *\~french
		 *\return		La combinaison d'indicateurs de passe.
		 */
		C3D_API PassComponentCombine getPassFlags()const;
		/**
		*\~english
		*\brief
		*	ConfigurationVisitorBase acceptance function.
		*\param vis
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de ConfigurationVisitorBase.
		*\param vis
		*	Le ... visiteur.
		*/
		C3D_API void accept( ConfigurationVisitorBase & vis );
		/**
		 *\~english
		 *\brief			Fills the pass buffer with this pass data.
		 *\param[in,out]	buffer			The pass buffer.
		 *\param[in]		passTypeIndex	The pass type index.
		 *\~french
		 *\brief			Remplit le pass buffer aves les données de cette passe.
		 *\param[in,out]	buffer			Le pass buffer.
		 *\param[in]		passTypeIndex	L'indice du type de passe.
		 */
		C3D_API void fillBuffer( PassBuffer & buffer
			, uint16_t passTypeIndex )const;
		/**
		 *\~english
		 *\brief			Writes the component content to text.
		 *\param[in]		tabs		The current tabulation level.
		 *\param[in]		folder		The resources folder.
		 *\param[in]		subfolder	The resources subfolder.
		 *\param[in,out]	file		The output file.
		 *\~french
		 *\brief			Ecrit le contenu du composant en texte.
		 *\param[in]		tabs		Le niveau actuel de tabulation.
		 *\param[in]		folder		Le dossier de ressources.
		 *\param[in]		subfolder	Le sous-dossier de ressources.
		 *\param[in,out]	file		Le fichier de sortie.
		 */
		C3D_API bool writeText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const;
		/**
		*\~english
		*\brief
		*	ConfigurationVisitorBase acceptance function, for a specific texture configuration.
		*\param config
		*	The texture configuration.
		*\param vis
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de ConfigurationVisitorBase, pour une configuration de texture.
		*\param config
		*	La configuration de texture.
		*\param vis
		*	Le ... visiteur.
		*/
		C3D_API void fillConfig( TextureConfiguration & config
			, ConfigurationVisitorBase & vis );

		C3D_API static void addParsers( castor::AttributeParsers & result
			, castor::UInt32StrMap const & textureChannels );

		C3D_API static float computeRoughnessFromGlossiness( float glossiness );
		C3D_API static float computeGlossinessFromRoughness( float roughness );
		C3D_API static float computeGlossinessFromShininess( float shininess );
		C3D_API static float computeShininessFromGlossiness( float glossiness );

		static float computeRoughnessFromShininess( float const & shininess )
		{
			return computeRoughnessFromGlossiness( computeGlossinessFromShininess( shininess ) );
		}

		static float computeShininessFromRoughness( float const & roughness )
		{
			return computeShininessFromGlossiness( computeGlossinessFromRoughness( roughness ) );
		}

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
		C3D_API BlendMode getAlphaBlendMode()const;
		C3D_API BlendMode getColourBlendMode()const;
		C3D_API VkCompareOp getAlphaFunc()const;
		C3D_API VkCompareOp getBlendAlphaFunc()const;
		C3D_API bool hasEnvironmentMapping()const;
		C3D_API bool hasSubsurfaceScattering()const;
		C3D_API bool isTwoSided()const;
		C3D_API UnitArray getTextureUnits()const;
		C3D_API uint32_t getTextureUnitsCount()const;
		C3D_API TextureCombine getTexturesMask()const;
		C3D_API bool hasLighting()const;
		C3D_API PassComponentRegister & getPassComponentsRegister()const;
		C3D_API castor::String getTextureFlagsName( PassComponentTextureFlag flags )const;
		C3D_API LightingModelID getLightingModelId()const;

		bool isInitialised()const
		{
			return m_initialised;
		}

		bool isInitialising()const
		{
			return m_initialising;
		}

		bool hasAutomaticShader()const
		{
			return m_automaticShader;
		}

		uint32_t getId()const
		{
			return m_id;
		}

		bool isImplicit()const
		{
			return m_implicit;
		}

		RenderPassRegisterInfo * getRenderPassInfo()const
		{
			return m_renderPassInfo;
		}

		uint32_t getIndex()const
		{
			return m_index;
		}

		uint32_t getMaxTexCoordSet()const
		{
			return m_maxTexcoordSet;
		}

		PassComponentTextureFlag getColourMapFlags()const
		{
			return m_colourMapFlag;
		}

		PassComponentTextureFlag getOpacityMapFlags()const
		{
			return m_opacityMapFlag;
		}

		PassComponentTextureFlag getNormalMapFlags()const
		{
			return m_normalMapFlag;
		}

		PassComponentTextureFlag getHeightMapFlags()const
		{
			return m_heightMapFlag;
		}

		PassComponentTextureFlag getOcclusionMapFlags()const
		{
			return m_occlusionMapFlag;
		}

		PassComponentTextureFlag getReflRefrFlags()const
		{
			return m_reflRefrFlag;
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
		C3D_API void enableLighting( bool value );
		C3D_API void enablePicking( bool value );

		void setId( uint32_t value )
		{
			m_id = value;
		}

		void setImplicit( bool value = true )
		{
			m_implicit = value;
		}

		void setColour( castor::RgbColour const & v
			, float gamma = 2.2f )
		{
			setColour( castor::HdrRgbColour{ v, gamma } );
		}

		void setColour( castor::Coords3f const & v )
		{
			setColour( castor::HdrRgbColour{ v[0u], v[1u], v[2u] } );
		}

		void setColour( castor::Point3f const & v )
		{
			setColour( castor::HdrRgbColour{ v[0u], v[1u], v[2u] } );
		}

		void reset()const
		{
			m_dirty = false;
		}
		/**@}*/
		/**
		*name
		*	Signals.
		*/
		/**@{*/
		OnPassChanged onChanged;
		/**@}*/

	protected:
		mutable std::atomic_bool m_dirty{ true };

	private:
		void onSssChanged( SubsurfaceScattering const & sss );
		void doPrepareImage( PassTextureSource cfg );
		void doAddUnit( TextureUnitData & unitData
			, TextureUnitRPtr unit
			, UnitArray & result );
		void doUpdateTextureFlags();
		std::vector< PassComponentUPtr > doRemoveDependencies( castor::String const & name );
		void doRemoveConfiguration( PassComponentTextureFlag flag );

	private:
		PassComponentCombine m_componentCombine;
		TextureCombine m_textureCombine;
		uint32_t m_index;
		PassComponentMap m_components;
		TextureSourceArray m_sources;
		TextureUnitDataRefs m_prepared;
		std::unordered_map< TextureSourceInfo, TextureAnimationUPtr, TextureSourceInfoHasher > m_animations;
		uint32_t m_maxTexcoordSet{};
		std::atomic_bool m_texturesReduced{ false };
		UnitArray m_textureUnits;
		uint32_t m_id{ 0u };
		bool m_implicit{ false };
		bool m_automaticShader{ true };
		RenderPassRegisterInfo * m_renderPassInfo{};
		bool m_initialised{ false };
		std::atomic_bool m_initialising{ false };
		PassComponentTextureFlag m_colourMapFlag{};
		PassComponentTextureFlag m_opacityMapFlag{};
		PassComponentTextureFlag m_normalMapFlag{};
		PassComponentTextureFlag m_heightMapFlag{};
		PassComponentTextureFlag m_occlusionMapFlag{};
		PassComponentFlag m_reflRefrFlag{};
	};
}

#endif
