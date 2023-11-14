/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UntileMappingComponent_H___
#define ___C3D_UntileMappingComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct UntileMappingComponent
		: public BaseDataPassComponentT< castor::AtomicGroupChangeTracked< bool > >
	{
		static constexpr PassFlag eUntile = PassFlag( 0x01u );

		struct ComponentsShader
			: shader::PassComponentsShader
		{
			explicit ComponentsShader( PassComponentPlugin const & plugin )
				: shader::PassComponentsShader{ plugin }
			{
			}

			C3D_API sdw::Vec4 sampleMap( sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec3 const & texCoords
				, shader::BlendComponents const & components )const override;
			C3D_API sdw::Vec4 sampleMap( sdw::CombinedImage2DRgba32 const & map
				, shader::DerivTex const & texCoords
				, shader::BlendComponents const & components )const override;

			bool isMapSampling( PipelineFlags const & flags )const override
			{
				return hasAny( flags.pass
					, makePassComponentFlag( getId(), eUntile ) );
			}

		private:
			sdw::RetVec4 sampleUntiled( sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec2 const & texCoords
				, sdw::Vec2 const & ddx
				, sdw::Vec2 const & ddy )const;

		private:
			mutable sdw::Function< sdw::Vec4
				, sdw::InVec2 > m_hash4;
			mutable sdw::Function< sdw::Vec4
				, sdw::InCombinedImage2DRgba32
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InVec2 > m_sampleUntiled;
		};

		class Plugin
			: public PassComponentPlugin
		{
		public:
			explicit Plugin( PassComponentRegister const & passComponent )
				: PassComponentPlugin{ passComponent }
			{
			}

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return castor::makeUniqueDerived< PassComponent, UntileMappingComponent >( pass );
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
				return makePassComponentFlag( getId(), eUntile );
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit UntileMappingComponent( Pass & pass );

		C3D_API void accept( ConfigurationVisitorBase & vis )override;

		C3D_API PassComponentFlag getPassFlags()const override
		{
			return makePassComponentFlag( getId()
				, ( isUntiling()
					? eUntile
					: PassFlag::eNone ) );
		}

		bool isUntiling()const
		{
			return getData();
		}

		void setUntiling( bool v )
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
