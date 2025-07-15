/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TwoSidedComponent_H___
#define ___C3D_TwoSidedComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace c3d
{
	struct TwoSidedComponent
		: public BaseDataPassComponentT< AtomicGroupChangeTracked< bool > >
	{
		class Plugin
			: public PassComponentPlugin
		{
		public:
			using PassComponentPlugin::PassComponentPlugin;

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return makeUniqueDerived< PassComponent, TwoSidedComponent >( pass );
			}

			void createParsers( AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;

			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override
			{
				// Component is never need in shader.
				return false;
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit TwoSidedComponent( Pass & pass );

		C3D_API void accept( ConfigurationVisitorBase & vis )override;

		bool isTwoSided()const
		{
			return getData();
		}

		void setTwoSided( bool v )
		{
			setData( v );
		}

		C3D_API static String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		bool doWriteText( String const & tabs
			, Path const & folder
			, String const & subfolder
			, StringStream & file )const override;
	};
}

#endif
