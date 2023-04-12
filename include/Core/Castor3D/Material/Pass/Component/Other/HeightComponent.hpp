/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HeightComponent_H___
#define ___C3D_HeightComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct HeightComponent
		: public BaseDataPassComponentT< castor::AtomicGroupChangeTracked< ParallaxOcclusionMode > >
	{
		static constexpr PassFlag eParallaxOcclusionMappingOne = PassFlag( 0x01u );
		static constexpr PassFlag eParallaxOcclusionMappingRepeat = PassFlag( 0x02u );
		static constexpr PassFlag eAll = PassFlag( 0x03u );

		struct ComponentsShader
			: shader::PassComponentsShader
		{
			explicit ComponentsShader( PassComponentPlugin const & plugin )
				: shader::PassComponentsShader{ plugin }
			{
			}

			C3D_API void fillComponents( sdw::type::BaseStruct & components
				, shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			C3D_API void fillComponentsInits( sdw::type::BaseStruct const & components
				, shader::Materials const & materials
				, shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			C3D_API void blendComponents( shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, shader::BlendComponents & res
				, shader::BlendComponents const & src )const override;
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
				return castor::makeUniqueDerived< PassComponent, HeightComponent >( pass );
			}

			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override;

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return std::make_unique< ComponentsShader >( *this );
			}

			PassComponentFlag getParallaxOcclusionMappingOneFlag()const override
			{
				return makePassComponentFlag( getId(), eParallaxOcclusionMappingOne );
			}

			PassComponentFlag getParallaxOcclusionMappingRepeatFlag()const override
			{
				return makePassComponentFlag( getId(), eParallaxOcclusionMappingRepeat );
			}

			PassComponentFlag getComponentFlags()const override
			{
				return makePassComponentFlag( getId(), eAll );
			}

			void filterComponentFlags( ComponentModeFlags filter
				, PassComponentCombine & componentsFlags )const override
			{
				if ( !checkFlag( filter, ComponentModeFlag::eGeometry ) )
				{
					remFlags( componentsFlags, getComponentFlags() );
				}
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit HeightComponent( Pass & pass );

		C3D_API void accept( PassVisitorBase & vis )override;

		C3D_API PassComponentFlag getPassFlags()const override
		{
			return makePassComponentFlag( getId()
				, ( ( getParallaxOcclusion() == ParallaxOcclusionMode::eOne )
					? eParallaxOcclusionMappingOne
					: ( ( getParallaxOcclusion() == ParallaxOcclusionMode::eRepeat )
						? eParallaxOcclusionMappingRepeat
						: PassFlag::eNone ) ) );
		}

		void setParallaxOcclusion( ParallaxOcclusionMode v )
		{
			setData( v );
		}

		ParallaxOcclusionMode getParallaxOcclusion()const
		{
			return getData();
		}

		bool hasParallaxOcclusion()const
		{
			return getParallaxOcclusion() != ParallaxOcclusionMode::eNone;
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
