#include "TextBillboardList.hpp"

#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< BillboardList >::TextWriter( String const & tabs )
		: TextWriterT< BillboardList >{ tabs }
	{
	}

	bool TextWriter< BillboardList >::operator()( BillboardList const & obj
		, castor::StringStream & file )
	{
		log::info << tabs() << cuT( "Writing BillboardList " ) << obj.getName() << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "billboard" ), obj.getName() ) } )
		{
			result = writeName( file, cuT( "parent" ), obj.getParent()->getName() )
				&& write( file, cuT( "cast_shadows" ), obj.isShadowCaster() )
				&& write( file, cuT( "receive_shadows" ), obj.isShadowReceiver() )
				&& writeName( file, cuT( "material" ), obj.getMaterial()->getName() )
				&& writeNamedSub( file, cuT( "dimensions" ), obj.getDimensions() );

			if ( result && obj.getCount() )
			{
				if ( auto posBlock{ beginBlock( file, cuT( "positions" ) ) } )
				{
					for ( auto const & point : obj )
					{
						result = result && writeNamedSub( file, cuT( "pos" ), point );
					}
				}
			}
		}

		return result;
	}
}
