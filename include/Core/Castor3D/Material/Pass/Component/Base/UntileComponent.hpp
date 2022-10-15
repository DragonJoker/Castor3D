/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UntileComponent_H___
#define ___C3D_UntileComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct UntileComponent
		: public BaseDataPassComponentT< castor::AtomicGroupChangeTracked< bool > >
	{
		struct ComponentsShader
			: shader::PassComponentsShader
		{
			explicit ComponentsShader( PassComponentPlugin const & plugin )
				: shader::PassComponentsShader{ plugin }
			{
			}

			C3D_API sdw::Vec4 sampleMap( sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec3 const & texCoords )const override;
			C3D_API sdw::Vec4 sampleMap( sdw::CombinedImage2DRgba32 const & map
				, shader::DerivTex const & texCoords )const override;

			bool isMapSampling( PipelineFlags const & flags )const override
			{
				return checkFlag( flags.m_passFlags
					, PassFlag::eUntile );
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
		};

		static PassComponentPluginUPtr createPlugin()
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >();
		}

		C3D_API explicit UntileComponent( Pass & pass );

		C3D_API void accept( PassVisitorBase & vis )override;

		C3D_API PassFlags getPassFlags()const override
		{
			return m_value.value()
				? PassFlag::eUntile
				: PassFlag::eNone;
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
