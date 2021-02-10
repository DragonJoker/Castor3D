#include "Castor3D/Text/TextBillboardList.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< BillboardList >::TextWriter( String const & tabs )
		: TextWriterT< BillboardList >{ tabs }
	{
	}

	bool TextWriter< BillboardList >::operator()( BillboardList const & obj, castor::TextFile & file )
	{
		log::info << tabs() << cuT( "Writing BillboardList " ) << obj.getName() << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, "billboard", obj.getName() ) } )
		{
			result = writeName( file, "parent", obj.getParent()->getName() )
				&& write( file, cuT( "cast_shadows" ), obj.isShadowCaster() )
				&& write( file, cuT( "receive_shadows" ), obj.isShadowReceiver() )
				&& writeName( file, "material", obj.getMaterial()->getName() )
				&& write( file, "dimensions", obj.getDimensions() );

			if ( result && obj.getCount() )
			{
				if ( auto posBlock{ beginBlock( file, "positions" ) } )
				{
					for ( auto const & point : obj )
					{
						result = result && write( file, "pos", point );
					}
				}
			}
		}

		return result;
	}
}
