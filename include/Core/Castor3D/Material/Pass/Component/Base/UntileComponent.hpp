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
		C3D_API explicit UntileComponent( Pass & pass );

		C3D_API static void createParsers( castor::AttributeParsers & parsers
			, ChannelFillers & channelFillers );

		C3D_API static bool isComponentNeeded( TextureFlags const & textures
			, ComponentModeFlags const & filter )
		{
			// Component is never need in shader.
			return false;
		}

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
