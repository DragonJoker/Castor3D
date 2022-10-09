/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TwoSidedComponent_H___
#define ___C3D_TwoSidedComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct TwoSidedComponent
		: public BaseDataPassComponentT< castor::AtomicGroupChangeTracked< bool > >
	{
		C3D_API explicit TwoSidedComponent( Pass & pass );

		C3D_API static void createParsers( castor::AttributeParsers & parsers
			, ChannelFillers & channelFillers );

		C3D_API static bool isComponentNeeded( TextureFlags const & textures
			, ComponentModeFlags const & filter )
		{
			// Component is never need in shader.
			return false;
		}

		C3D_API void accept( PassVisitorBase & vis )override;

		bool isTwoSided()const
		{
			return getData();
		}

		void setTwoSided( bool v )
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
