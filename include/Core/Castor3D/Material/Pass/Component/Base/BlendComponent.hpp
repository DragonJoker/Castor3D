/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BlendComponent_H___
#define ___C3D_BlendComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct BlendData
	{
		explicit BlendData( std::atomic_bool & dirty )
			: colourBlendMode{ dirty, BlendMode::eNoBlend }
			, alphaBlendMode{ dirty, BlendMode::eNoBlend }
		{
		}

		castor::AtomicGroupChangeTracked< BlendMode > colourBlendMode;
		castor::AtomicGroupChangeTracked< BlendMode > alphaBlendMode;
	};

	struct BlendComponent
		: public BaseDataPassComponentT< BlendData >
	{
		static constexpr PassFlag eAlphaBlending = PassFlag( 0x01u );

		class Plugin
			: public PassComponentPlugin
		{
		public:
			using PassComponentPlugin::PassComponentPlugin;

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return castor::makeUniqueDerived< PassComponent, BlendComponent >( pass );
			}

			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;

			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override
			{
				// Component is never need in shader.
				return false;
			}

			PassComponentFlag getAlphaBlendingFlag()const override
			{
				return getComponentFlags();
			}

			PassComponentFlag getComponentFlags()const override
			{
				return makePassComponentFlag( getId(), eAlphaBlending );
			}

			void filterComponentFlags( ComponentModeFlags filter
				, PassComponentCombine & componentsFlags )const override
			{
				if ( !checkFlag( filter, ComponentModeFlag::eOpacity )
					|| !checkFlag( filter, ComponentModeFlag::eAlphaBlending ) )
				{
					remFlags( componentsFlags, getComponentFlags() );
				}
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit BlendComponent( Pass & pass );

		C3D_API void accept( ConfigurationVisitorBase & vis )override;

		C3D_API PassComponentFlag getPassFlags()const override
		{
			return makePassComponentFlag( getId()
				, ( hasAlphaBlending()
					? eAlphaBlending
					: PassFlag::eNone ) );
		}

		bool hasAlphaBlending()const
		{
			return getAlphaBlendMode() != BlendMode::eNoBlend;
		}

		BlendMode getAlphaBlendMode()const
		{
			return m_value.alphaBlendMode;
		}

		BlendMode getColourBlendMode()const
		{
			return m_value.colourBlendMode;
		}

		void setAlphaBlendMode( BlendMode value )
		{
			m_value.alphaBlendMode = value;
		}

		void setColourBlendMode( BlendMode value )
		{
			m_value.colourBlendMode = value;
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
