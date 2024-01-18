/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FractalMappingComponent_H___
#define ___C3D_FractalMappingComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct FractalMappingComponent
		: public BaseDataPassComponentT< castor::AtomicGroupChangeTracked< bool > >
	{
		static constexpr PassFlag eFractalMapping = PassFlag( 0x01u );

		struct ComponentsShader
			: shader::PassComponentsShader
		{
			using shader::PassComponentsShader::PassComponentsShader;

			C3D_API void fillComponents( ComponentModeFlags componentsMask
				, sdw::type::BaseStruct & components
				, shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			C3D_API void fillComponentsInits( sdw::type::BaseStruct const & components
				, shader::Materials const & materials
				, shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			C3D_API sdw::Vec4 sampleMap( sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec3 const & texCoords
				, shader::BlendComponents const & components )const override;
			C3D_API sdw::Vec4 sampleMap( sdw::CombinedImage2DRgba32 const & map
				, shader::DerivTex const & texCoords
				, shader::BlendComponents const & components )const override;

			bool isMapSampling( PipelineFlags const & flags )const override
			{
				return hasAny( flags.pass
					, makePassComponentFlag( getId(), eFractalMapping ) );
			}

		private:
			sdw::RetVec4 sampleFractal( sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec2 const & texCoords
				, sdw::Vec2 const & ddx
				, sdw::Vec2 const & ddy
				, sdw::Float const & depth )const;

		private:
			mutable sdw::Function< sdw::Vec4
				, sdw::InCombinedImage2DRgba32
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InFloat > m_sampleFractal;
		};

		class Plugin
			: public PassComponentPlugin
		{
		public:
			using PassComponentPlugin::PassComponentPlugin;

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return castor::makeUniqueDerived< PassComponent, FractalMappingComponent >( pass );
			}

			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;

			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override
			{
				// Component is always needed in shader, when present.
				return true;
			}

			bool replacesMapSampling()const override
			{
				return true;
			}

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return std::make_unique< ComponentsShader >( *this );
			}

			PassComponentFlag getComponentFlags()const override
			{
				return makePassComponentFlag( getId(), eFractalMapping );
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit FractalMappingComponent( Pass & pass );

		C3D_API void accept( ConfigurationVisitorBase & vis )override;

		C3D_API PassComponentFlag getPassFlags()const override
		{
			return makePassComponentFlag( getId()
				, ( isFractal()
					? eFractalMapping
					: PassFlag::eNone ) );
		}

		bool isFractal()const
		{
			return getData();
		}

		void setFractal( bool v )
		{
			setData( v );
		}

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		bool doWriteText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;
	};
}

#endif
