/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassMapComponent_H___
#define ___C3D_PassMapComponent_H___

#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Material/Pass/Component/PassComponent.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"

#include <CastorUtils/Graphics/ImageCache.hpp>

namespace c3d
{
	using TextureSourceSet = HashSet< TextureSourceInfo, TextureSourceInfoHasher >;

	namespace shader
	{
		struct PassMapMaterialShader
			: shader::PassMaterialShader
		{
			C3D_API explicit PassMapMaterialShader( String const & mapMemberName )
				: shader::PassMaterialShader{ sizeof( uint32_t ) }
				, m_mapMemberName{ toUtf8( mapMemberName + cuT( "MapAndMask" ) ) }
			{
			}

			C3D_API void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;

		private:
			MbString m_mapMemberName;
		};

		struct PassMapComponentsShader
			: public PassComponentsShader
		{
		public:
			using PassComponentsShader::PassComponentsShader;

			C3D_API virtual void applyTexture( PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const = 0;

		protected:
			C3D_API sdw::Float loadFloatComponent( String const & mapName
				, String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const;
			C3D_API sdw::Vec2 loadVec2Component( String const & mapName
				, String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const;
			C3D_API sdw::Vec3 loadVec3Component( String const & mapName
				, String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const;
			C3D_API sdw::Vec4 loadVec4Component( String const & mapName
				, String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const;

			sdw::Float loadFloatComponent( String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const
			{
				return loadFloatComponent( valueName
					, valueName
					, passShaders
					, textureConfigs
					, textureAnims
					, material
					, components
					, sampleTexture );
			}

			sdw::Vec2 loadVec2Component( String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const
			{
				return loadVec2Component( valueName
					, valueName
					, passShaders
					, textureConfigs
					, textureAnims
					, material
					, components
					, sampleTexture );
			}

			sdw::Vec3 loadVec3Component( String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const
			{
				return loadVec3Component( valueName
					, valueName
					, passShaders
					, textureConfigs
					, textureAnims
					, material
					, components
					, sampleTexture );
			}

			sdw::Vec4 loadVec4Component( String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const
			{
				return loadVec4Component( valueName
					, valueName
					, passShaders
					, textureConfigs
					, textureAnims
					, material
					, components
					, sampleTexture );
			}

			C3D_API void applyFloatComponent( String const & mapName
				, String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const;
			C3D_API void applyVec2Component( String const & mapName
				, String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const;
			C3D_API void applyVec3Component( String const & mapName
				, String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const;
			C3D_API void applyVec4Component( String const & mapName
				, String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const;

			void applyFloatComponent( String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const
			{
				applyFloatComponent( valueName
					, valueName
					, passShaders
					, textureConfigs
					, textureAnims
					, material
					, components
					, sampleTexture );
			}

			void applyVec2Component( String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const
			{
				applyVec2Component( valueName
					, valueName
					, passShaders
					, textureConfigs
					, textureAnims
					, material
					, components
					, sampleTexture );
			}

			void applyVec3Component( String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const
			{
				applyVec3Component( valueName
					, valueName
					, passShaders
					, textureConfigs
					, textureAnims
					, material
					, components
					, sampleTexture );
			}

			void applyVec4Component( String const & valueName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const
			{
				applyVec4Component( valueName
					, valueName
					, passShaders
					, textureConfigs
					, textureAnims
					, material
					, components
					, sampleTexture );
			}

			sdw::Float doLoadFloatComponent( MbString const & mbMapName
				, MbString const & mbValueName
				, MbString const & textureName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const;
			sdw::Vec2 doLoadVec2Component( MbString const & mbMapName
				, MbString const & mbValueName
				, MbString const & textureName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const;
			sdw::Vec3 doLoadVec3Component( MbString const & mbMapName
				, MbString const & mbValueName
				, MbString const & textureName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const;
			sdw::Vec4 doLoadVec4Component( MbString const & mbMapName
				, MbString const & mbValueName
				, MbString const & textureName
				, PassShaders const & passShaders
				, TextureConfigurations const & textureConfigs
				, TextureAnimations const & textureAnims
				, Material const & material
				, BlendComponents & components
				, SampleTexture const & sampleTexture )const;
		};
	}

	class PassMapComponentPlugin
		: public PassComponentPlugin
	{
	public:
		C3D_API explicit PassMapComponentPlugin( PassComponentRegister const & passComponent
			, UpdateComponent const & pupdateComponent = nullptr )
			: PassComponentPlugin{ passComponent, pupdateComponent }
		{
		}

		C3D_API void zeroBuffer( Pass const & pass
			, shader::PassMaterialShader const & materialShader
			, PassBuffer & buffer )const override;
		C3D_API bool writeTextureConfig( TextureConfiguration const & configuration
			, String const & tabs
			, StringStream & file )const override;

	private:
		virtual bool doWriteTextureConfig( TextureConfiguration const & configuration
			, uint32_t mask
			, String const & tabs
			, StringStream & file )const
		{
			return true;
		}
	};

	struct PassMapComponent
		: public PassComponent
	{
		/**
		*\name
		*	Construction / Destruction.
		*/
		/**@{*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass			The parent pass.
		 *\param[in]	type			The component type.
		 *\param[in]	textureFlags	The texture flags.
		 *\param[in]	deps			The components this one depends on.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass			Le sous-maillage pass.
		 *\param[in]	type			Le type de composant.
		 *\param[in]	textureFlags	Les indicateurs de texture.
		 *\param[in]	deps			Les composants dont celui-ci dépend.
		 */
		C3D_API PassMapComponent( Pass & pass
			, String type
			, TextureFlags textureFlags
			, StringArray deps = {} );
		/**@}*/
		/**
		*\~english
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		PassComponentTextureFlag getTextureFlags()const noexcept
		{
			return m_textureFlags;
		}

		C3D_API void fillConfig( TextureConfiguration & config
			, ConfigurationVisitorBase & vis )const override;
		C3D_API void createDefaultTexture( Pass & pass
			, String name
			, TextureConfiguration config
			, ImageCreateParams imageParams );
		/**@}*/

	private:
		C3D_API void doFillBuffer( PassBuffer & buffer )const override;
		C3D_API virtual void doFillConfig( TextureConfiguration & configuration
			, ConfigurationVisitorBase & vis )const
		{
		}

	private:
		PassComponentTextureFlag m_textureFlags;
	};

	struct PassMapDefaultImageParams
	{
		String name;
		ImageCreateParams image;
	};

	template< typename ComponentT >
	void createDefaultTextureComponent( Pass & pass )
	{
		ComponentT * component = createPassComponent< ComponentT >( pass );
		PassMapDefaultImageParams imageParams = component->createDefaultImage();
		const PassComponentPlugin & plugin = component->getPlugin();
		TextureConfiguration config;
		plugin.fillTextureConfiguration( config, 0U );
		component->createDefaultTexture( pass
			, std::move( imageParams.name )
			, std::move( config )
			, std::move( imageParams.image ) );
	}
}

#endif
