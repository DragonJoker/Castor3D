/*
See LICENSE file in root folder
*/
#ifndef ___C3DAM_AnisotropyComponent_H___
#define ___C3DAM_AnisotropyComponent_H___

#include <Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp>

#ifndef CU_PlatformWindows
#	define C3D_AnisotropicMaterial_API
#else
#	ifdef AnisotropicMaterial_EXPORTS
#		define C3D_AnisotropicMaterial_API __declspec( dllexport )
#	else
#		define C3D_AnisotropicMaterial_API __declspec( dllimport )
#	endif
#endif

namespace anisotropy
{
	namespace c3d = castor3d::shader;

	struct AnisotropyData
	{
		explicit AnisotropyData( std::atomic_bool & dirty
			, float strength
			, float rotation )
			: strength{ dirty, castor::makeRangedValue( strength, 0.0f, 1.0f ) }
			, rotation{ dirty, castor::makeRangedValue( rotation, 0.0f, castor::PiMult2< float > ) }
		{
		}

		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > strength;
		castor::AtomicGroupChangeTracked< castor::RangedValue< float > > rotation;
	};

	struct AnisotropyComponent
		: public castor3d::BaseDataPassComponentT< AnisotropyData >
	{
		struct ReflRefrShader
			: public c3d::PassReflRefrShader
		{
			explicit ReflRefrShader( castor3d::PassComponentPlugin const & plugin )
				: PassReflRefrShader{ plugin }
			{
			}

			void computeReflRefr( c3d::ReflectionModel & reflections
				, c3d::BlendComponents & components
				, c3d::LightSurface const & lightSurface
				, sdw::Vec4 const & position
				, c3d::BackgroundModel & background
				, sdw::CombinedImage2DRgba32 const & mippedScene
				, c3d::CameraData const & camera
				, c3d::DirectLighting & lighting
				, c3d::IndirectLighting & indirect
				, sdw::Vec2 const & sceneUv
				, sdw::UInt const & envMapIndex
				, sdw::Vec3 const & incident
				, sdw::UInt const & hasReflection
				, sdw::UInt const & hasRefraction
				, sdw::Float const & refractionRatio
				, sdw::Vec3 & reflectedDiffuse
				, sdw::Vec3 & reflectedSpecular
				, sdw::Vec3 & refracted
				, sdw::Vec3 & coatReflected
				, sdw::Vec3 & sheenReflected
				, c3d::DebugOutput & debugOutput )const override;
			void computeReflRefr( c3d::ReflectionModel & reflections
				, c3d::BlendComponents & components
				, c3d::LightSurface const & lightSurface
				, c3d::BackgroundModel & background
				, c3d::CameraData const & camera
				, c3d::DirectLighting & lighting
				, c3d::IndirectLighting & indirect
				, sdw::Vec2 const & sceneUv
				, sdw::UInt const & envMapIndex
				, sdw::Vec3 const & incident
				, sdw::UInt const & hasReflection
				, sdw::UInt const & hasRefraction
				, sdw::Float const & refractionRatio
				, sdw::Vec3 & reflectedDiffuse
				, sdw::Vec3 & reflectedSpecular
				, sdw::Vec3 & refracted
				, sdw::Vec3 & coatReflected
				, sdw::Vec3 & sheenReflected
				, c3d::DebugOutput & debugOutput )const override;
		};

		struct ComponentsShader
			: c3d::PassComponentsShader
		{
			explicit ComponentsShader( castor3d::PassComponentPlugin const & plugin )
				: PassComponentsShader{ plugin }
			{
			}

			void fillComponents( castor3d::ComponentModeFlags componentsMask
				, sdw::type::BaseStruct & components
				, c3d::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			void fillComponentsInits( sdw::type::BaseStruct const & components
				, c3d::Materials const & materials
				, c3d::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			void blendComponents( c3d::Materials const & materials
				, sdw::Float const & passMultiplier
				, c3d::BlendComponents & res
				, c3d::BlendComponents const & src )const override;
			void updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, c3d::Material const & material
				, c3d::BlendComponents & components
				, bool isFrontCulled )const override;
		};

		struct MaterialShader
			: c3d::PassMaterialShader
		{
			MaterialShader();
			void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
		};

		class Plugin
			: public castor3d::PassComponentPlugin
		{
		public:
			using castor3d::PassComponentPlugin::PassComponentPlugin;

			castor3d::PassComponentUPtr createComponent( castor3d::Pass & pass )const override
			{
				return castor::makeUniqueDerived< castor3d::PassComponent, AnisotropyComponent >( pass );
			}

			void createParsers( castor::AttributeParsers & parsers
				, castor3d::ChannelFillers & channelFillers )const override;
			void zeroBuffer( castor3d::Pass const & pass
				, c3d::PassMaterialShader const & materialShader
				, castor3d::PassBuffer & buffer )const override;
			bool isComponentNeeded( castor3d::TextureCombine const & textures
				, castor3d::ComponentModeFlags const & filter )const override;

			c3d::PassComponentsShaderPtr createComponentsShader()const override
			{
				return castor::make_unique< ComponentsShader >( *this );
			}

			c3d::PassMaterialShaderPtr createMaterialShader()const override
			{
				return castor::make_unique< MaterialShader >();
			}

			c3d::PassReflRefrShaderPtr createReflRefrShader()const override
			{
				return castor::make_unique< ReflRefrShader >( *this );
			}

			bool isReflRefrComponent()const override
			{
				return true;
			}
		};

		static castor3d::PassComponentPluginUPtr createPlugin( castor3d::PassComponentRegister const & passComponents )
		{
			return castor::makeUniqueDerived< castor3d::PassComponentPlugin, Plugin >( passComponents );
		}

		C3D_AnisotropicMaterial_API explicit AnisotropyComponent( castor3d::Pass & pass );

		void accept( castor3d::ConfigurationVisitorBase & vis )override;

		void setStrength( float value )
		{
			*m_value.strength = value;
		}

		void setRotation( castor::Angle const & value )
		{
			*m_value.rotation = value.radians();
		}

		float getStrength()const
		{
			return m_value.strength.value().value();
		}

		castor::Angle getRotation()const
		{
			return castor::Angle::fromRadians( m_value.rotation->value() );
		}

		C3D_AnisotropicMaterial_API static castor::String const TypeName;
		static float constexpr DefaultStrength{ 0.0f };
		static float constexpr DefaultRotation{ 0.0f };

	private:
		castor3d::PassComponentUPtr doClone( castor3d::Pass & pass )const override;
		bool doWriteText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;
		void doFillBuffer( castor3d::PassBuffer & buffer )const override;
	};

	CU_DeclareSmartPtr( anisotropy, AnisotropyComponent, C3D_AnisotropicMaterial_API );
}

#endif
