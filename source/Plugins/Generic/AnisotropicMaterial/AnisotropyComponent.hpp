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
	namespace c3ds = c3d::shader;

	struct AnisotropyData
	{
		explicit AnisotropyData( std::atomic_bool & dirty
			, float strength
			, float rotation )
			: strength{ dirty, c3d::makeRangedValue( strength, 0.0f, 1.0f ) }
			, rotation{ dirty, c3d::makeRangedValue( rotation, 0.0f, c3d::PiMult2< float > ) }
		{
		}

		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > strength;
		c3d::AtomicGroupChangeTracked< c3d::RangedValue< float > > rotation;
	};

	struct AnisotropyComponent
		: public c3d::BaseDataPassComponentT< AnisotropyData >
	{
		struct ReflRefrShader
			: public c3ds::PassReflRefrShader
		{
			using c3ds::PassReflRefrShader::PassReflRefrShader;

			void computeWithTransmission( c3ds::ReflectionModel & reflections
				, c3ds::BlendComponents & components
				, c3ds::LightSurface const & lightSurface
				, c3ds::BackgroundModel & background
				, sdw::CombinedImage2DRgba32 const & mippedScene
				, c3ds::CameraData const & camera
				, c3ds::RenderData const & render
				, c3ds::DirectLighting & lighting
				, c3ds::IndirectLighting & indirect
				, sdw::Vec2 const & sceneUv
				, sdw::UInt const & envMapIndex
				, sdw::Vec3 const & incident
				, c3ds::ReflectionRefraction & output
				, c3ds::DebugOutputCategory const & debugOutput )const override;
			void computeWithoutTransmission( c3ds::ReflectionModel & reflections
				, c3ds::BlendComponents & components
				, c3ds::LightSurface const & lightSurface
				, c3ds::BackgroundModel & background
				, c3ds::CameraData const & camera
				, c3ds::RenderData const & render
				, c3ds::DirectLighting & lighting
				, c3ds::IndirectLighting & indirect
				, sdw::Vec2 const & sceneUv
				, sdw::UInt const & envMapIndex
				, sdw::Vec3 const & incident
				, c3ds::ReflectionRefraction & output
				, c3ds::DebugOutputCategory const & debugOutput )const override;
		};

		struct MaterialShader
			: c3ds::PassMaterialShader
		{
			MaterialShader();
			void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
		};

		struct ComponentsShader
			: c3ds::PassComponentsShader
		{
			explicit ComponentsShader( c3d::PassComponentPlugin const & plugin )
				: PassComponentsShader{ plugin }
			{
			}

			void fillComponents( c3d::ComponentModeFlags componentsMask
				, sdw::type::BaseStruct & components
				, c3ds::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			void fillComponentsInits( sdw::type::BaseStruct const & components
				, c3ds::Materials const & materials
				, c3ds::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			void blendComponents( c3ds::Materials const & materials
				, sdw::Float const & passMultiplier
				, c3ds::BlendComponents & res
				, c3ds::BlendComponents const & src )const override;
			void updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, c3ds::Material const & material
				, c3ds::BlendComponents & components
				, bool isFrontCulled )const override;
		};

		class Plugin
			: public c3d::PassComponentPlugin
		{
		public:
			using c3d::PassComponentPlugin::PassComponentPlugin;

			c3d::PassComponentUPtr createComponent( c3d::Pass & pass )const override
			{
				return c3d::makeUniqueDerived< c3d::PassComponent, AnisotropyComponent >( pass );
			}

			void createParsers( c3d::AttributeParsers & parsers
				, c3d::ChannelFillers & channelFillers )const override;
			void zeroBuffer( c3d::Pass const & pass
				, c3ds::PassMaterialShader const & materialShader
				, c3d::PassBuffer & buffer )const override;
			bool isComponentNeeded( c3d::TextureCombine const & textures
				, c3d::ComponentModeFlags const & filter )const override;

			c3ds::PassComponentsShaderPtr createComponentsShader()const override
			{
				return c3d::makeRawUnique< ComponentsShader >( *this );
			}

			c3ds::PassMaterialShaderPtr createMaterialShader()const override
			{
				return c3d::makeRawUnique< MaterialShader >();
			}

			c3ds::PassReflRefrShaderPtr createReflRefrShader()const override
			{
				return c3d::makeRawUnique< ReflRefrShader >( *this );
			}

			bool isReflRefrComponent()const override
			{
				return true;
			}
		};

		static c3d::PassComponentPluginUPtr createPlugin( c3d::PassComponentRegister const & passComponents )
		{
			return c3d::makeUniqueDerived< c3d::PassComponentPlugin, Plugin >( passComponents );
		}

		C3D_AnisotropicMaterial_API explicit AnisotropyComponent( c3d::Pass & pass );

		void accept( c3d::ConfigurationVisitorBase & vis )override;

		void setStrength( float value )
		{
			*m_value.strength = value;
		}

		void setRotation( c3d::Angle const & value )
		{
			*m_value.rotation = value.radians();
		}

		float getStrength()const
		{
			return m_value.strength.value().value();
		}

		c3d::Angle getRotation()const
		{
			return c3d::Angle::fromRadians( m_value.rotation->value() );
		}

		C3D_AnisotropicMaterial_API static c3d::String const TypeName;
		static float constexpr DefaultStrength{ 0.0f };
		static float constexpr DefaultRotation{ 0.0f };

	private:
		c3d::PassComponentUPtr doClone( c3d::Pass & pass )const override;
		bool doWriteText( c3d::String const & tabs
			, c3d::Path const & folder
			, c3d::String const & subfolder
			, c3d::StringStream & file )const override;
		void doFillBuffer( c3d::PassBuffer & buffer )const override;
	};

	CU_DeclareSmartPtr( anisotropy, AnisotropyComponent, C3D_AnisotropicMaterial_API );
}

#endif
