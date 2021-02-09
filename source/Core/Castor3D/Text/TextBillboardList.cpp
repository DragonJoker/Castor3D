#include "Castor3D/Text/TextBillboardList.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< BillboardList >::TextWriter( String const & tabs )
		: TextWriterT< BillboardList >{ tabs }
	{
	}

	bool TextWriter< BillboardList >::operator()( BillboardList const & obj, castor::TextFile & file )
	{
		log::info << m_tabs << cuT( "Writing BillboardList " ) << obj.getName() << std::endl;
		bool result = false;

		if ( beginBlock( "billboard", obj.getName(), file ) )
		{
			result = writeName( "parent", obj.getParent()->getName(), file )
				&& writeName( "material", obj.getMaterial()->getName(), file )
				&& write( "dimensions", obj.getDimensions(), file );

			if ( result && obj.getCount() )
			{
				if ( beginBlock( "positions", file ) )
				{
					for ( auto const & point : obj )
					{
						result = result && write( "pos", point, file );
					}
				}
			}
		}

		return result;
	}
}
