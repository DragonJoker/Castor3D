/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PickableComponent_H___
#define ___C3D_PickableComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct PickableComponent
		: public BaseDataPassComponentT< castor::AtomicGroupChangeTracked< bool > >
	{
		static constexpr PassFlag ePickable = PassFlag( 0x01u );

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
				return castor::makeUniqueDerived< PassComponent, PickableComponent >( pass );
			}

			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;

			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override
			{
				// Component is never need in shader.
				return false;
			}

			PassComponentFlag getComponentFlags()const override
			{
				return makePassComponentFlag( getId(), ePickable );
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit PickableComponent( Pass & pass
			, bool pickable = true );

		C3D_API void accept( ConfigurationVisitorBase & vis )override;

		C3D_API PassComponentFlag getPassFlags()const override
		{
			return makePassComponentFlag( getId()
				, ( isPickable()
					? ePickable
					: PassFlag::eNone ) );
		}

		bool isPickable()const
		{
			return getData();
		}

		void setPickable( bool v )
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
