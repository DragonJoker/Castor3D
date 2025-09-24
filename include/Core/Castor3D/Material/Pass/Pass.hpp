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

namespace c3d
{
	class Pass
		: public OwnedBy< Material >
	{
	private:
		friend struct PassComponent;

		C3D_API Pass( Material & parent
			, LightingModelID lightingModelId
			, RenderPassRegisterInfo * renderPassInfo
			, bool implicit = false
			, bool automaticShader = true );

	public:
		using UnitArray = Vector< TextureUnitRPtr >;
		using PassTextureSource = Pair< TextureSourceInfo, PassTextureConfig >;
		using TextureSourceArray = Vector< PassTextureSource >;
		using UnitDataSources = Map< TextureUnitDataRPtr, Vector< TextureSourceInfo > >;
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
		C3D_API Pass( Material & parent
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
		C3D_API bool hasComponent( String const & name )const noexcept;
		C3D_API PassComponent * getComponent( String const & name )const;
		C3D_API Vector< PassComponentUPtr > removeComponent( String const & name );
		C3D_API shader::PassMaterialShader * getMaterialShader( String const & componentType )const;
		C3D_API PassComponentID getComponentId( String const & componentType )const;
		C3D_API PassComponentPlugin const & getComponentPlugin( PassComponentID componentId )const;
		C3D_API PassComponentCombineID getComponentCombineID()const;
		C3D_API TextureCombineID getTextureCombineID()const;
		C3D_API uint32_t getHash()const noexcept;
		C3D_API bool isVisible()const noexcept;

		PassComponentPlugin const & getComponentPlugin( String const & componentType )const
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
				result = new ComponentT{ *this, c3d::forward< ParamsT >( params )... };
				this->addComponent( PassComponentUPtr{ result } );
			}

			return result;
		}

		template< typename ComponentT >
		bool hasComponent()const noexcept
		{
			return this->hasComponent( ComponentT::TypeName );
		}

		template< typename ComponentT >
		ComponentT * getComponent()const noexcept
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
		C3D_API void unregisterTexture( TextureSourceInfo const & sourceInfo )noexcept;
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
		C3D_API void setColour( HdrRgbColour const & value )const;
		/**
		 *\~english
		 *\return		The basic pass colour.
		 *\~french
		 *\return		La couleur basique de la passe.
		 */
		C3D_API HdrRgbColour const & getColour()const;
		/**
		 *\~english
		 *\return		The pass flags combination.
		 *\~french
		 *\return		La combinaison d'indicateurs de passe.
		 */
		C3D_API PassComponentCombine getPassFlags()const noexcept;
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
		C3D_API void accept( ConfigurationVisitorBase & vis )const;
		/**
		 *\~english
		 *\brief			Fills the pass buffer with this pass data.
		 *\param[in,out]	buffer	The pass buffer.
		 *\~french
		 *\brief			Remplit le pass buffer aves les données de cette passe.
		 *\param[in,out]	buffer	Le pass buffer.
		 */
		C3D_API void fillBuffer( PassBuffer & buffer )const;
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
		C3D_API bool writeText( String const & tabs
			, Path const & folder
			, String const & subfolder
			, StringStream & file )const;
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
			, ConfigurationVisitorBase & vis )const;

		C3D_API static void addParsers( AttributeParsers & result
			, UInt32StrMap const & textureChannels );

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
		C3D_API ComparisonFunc getAlphaFunc()const;
		C3D_API ComparisonFunc getBlendAlphaFunc()const;
		C3D_API bool hasEnvironmentMapping()const;
		C3D_API bool hasSubsurfaceScattering()const;
		C3D_API bool isTwoSided()const;
		C3D_API UnitArray getTextureUnits()const;
		C3D_API uint32_t getTextureUnitsCount()const;
		C3D_API TextureCombine getTexturesMask()const noexcept;
		C3D_API bool hasLighting()const;
		C3D_API PassComponentRegister & getPassComponentsRegister()const;
		C3D_API String getTextureFlagsName( PassComponentTextureFlag flags )const;
		C3D_API LightingModelID getLightingModelId()const;
		C3D_API RenderPassTypeID getRenderPassTypeId()const;

		bool isInitialised()const noexcept
		{
			return m_initialised;
		}

		bool isInitialising()const noexcept
		{
			return m_initialising;
		}

		bool hasAutomaticShader()const noexcept
		{
			return m_automaticShader;
		}

		uint32_t getId()const noexcept
		{
			return m_id;
		}

		bool isImplicit()const noexcept
		{
			return m_implicit;
		}

		RenderPassRegisterInfo * getRenderPassInfo()const noexcept
		{
			return m_renderPassInfo;
		}

		uint32_t getIndex()const noexcept
		{
			return m_index;
		}

		uint32_t getMaxTexCoordSet()const noexcept
		{
			return m_maxTexcoordSet;
		}

		PassComponentTextureFlag getColourMapFlags()const noexcept
		{
			return m_colourMapFlag;
		}

		PassComponentTextureFlag getOpacityMapFlags()const noexcept
		{
			return m_opacityMapFlag;
		}

		PassComponentTextureFlag getNormalMapFlags()const noexcept
		{
			return m_normalMapFlag;
		}

		PassComponentTextureFlag getHeightMapFlags()const noexcept
		{
			return m_heightMapFlag;
		}

		PassComponentTextureFlag getOcclusionMapFlags()const noexcept
		{
			return m_occlusionMapFlag;
		}

		PassComponentTextureFlag getReflRefrFlags()const noexcept
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
		auto begin()const noexcept
		{
			return m_textureUnits.begin();
		}

		auto begin()noexcept
		{
			return m_textureUnits.begin();
		}

		auto end()const noexcept
		{
			return m_textureUnits.end();
		}

		auto end()noexcept
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
		C3D_API void enableLighting( bool value )const;
		C3D_API void enablePicking( bool value )const;

		void setId( uint32_t value )noexcept
		{
			m_id = value;
		}

		void setImplicit( bool value = true )noexcept
		{
			m_implicit = value;
		}

		void setColour( RgbColour const & v
			, float gamma = 2.2f )const
		{
			setColour( HdrRgbColour{ v, gamma } );
		}

		void setColour( PointView3f const & v )const
		{
			setColour( HdrRgbColour{ v[0u], v[1u], v[2u] } );
		}

		void setColour( Point3f const & v )const
		{
			setColour( HdrRgbColour{ v[0u], v[1u], v[2u] } );
		}

		void reset()const noexcept
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
		void doPrepareImage( PassTextureSource const & cfg );
		void doUpdateTextureFlags();
		Vector< PassComponentUPtr > doRemoveDependencies( String const & name );
		void doRemoveConfiguration( PassComponentTextureFlag flag );

	private:
		PassComponentCombine m_componentCombine;
		TextureCombine m_textureCombine;
		uint32_t m_index;
		PassComponentMap m_components;
		TextureSourceArray m_sources;
		TextureUnitDataRefs m_prepared;
		HashMap< TextureSourceInfo, TextureAnimationUPtr, TextureSourceInfoHasher > m_animations;
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

	struct SceneContext;
	struct MaterialContext;

	struct PassContext
	{
		MaterialContext * material{};
		Pass * pass{};
		PassComponent * passComponent{};
		bool createPass{ true };
		uint32_t unitIndex{};
	};

	C3D_API String getPrefix( PassContext const & context );
	C3D_API Engine * getEngine( PassContext const & context );

	template< typename ComponentT, typename ... ParamsT >
	ComponentT * createPassComponent( Pass & pass, ParamsT && ... params )
	{
		return pass.template createComponent< ComponentT >( std::forward< ParamsT >( params )... );
	}

	template< typename ComponentT, typename ... ParamsT >
	ComponentT & getPassComponent( PassContext & context, ParamsT && ... params )
	{
		if ( !context.passComponent
			|| getComponentPass( *context.passComponent ) != context.pass
			|| getPassComponentType( *context.passComponent ) != ComponentT::TypeName )
		{
			if ( context.pass->template hasComponent< ComponentT >() )
			{
				context.passComponent = context.pass->template getComponent< ComponentT >();
			}
			else
			{
				context.passComponent = createPassComponent< ComponentT >( *context.pass, std::forward< ParamsT >( params )... );
			}
		}

		return static_cast< ComponentT & >( *context.passComponent );
	}
}

namespace c3d
{
	template<>
	struct ParserEnumTraits< BlendMode >
	{
		static inline xchar const * const Name = cuT( "BlendMode" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = getEnumMapT< BlendMode >();
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< ParallaxOcclusionMode >
	{
		static inline xchar const * const Name = cuT( "ParallaxOcclusionMode" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = getEnumMapT< ParallaxOcclusionMode >();
				return result;
			}( );
	};
}

#endif
