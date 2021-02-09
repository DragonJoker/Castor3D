#include "Castor3D/Text/TextGeometry.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Scene/SceneNode.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< Geometry >::TextWriter( String const & tabs )
		: TextWriterT< Geometry >{ tabs }
	{
	}

	bool TextWriter< Geometry >::operator()( Geometry const & geometry, TextFile & file )
	{
		bool result{ true };

		if ( geometry.getMesh() )
		{
			log::info << cuT( "Writing Geometry " ) << geometry.getName() << std::endl;
			result = false;

			if ( auto block = beginBlock( cuT( "object" ), geometry.getName(), file ) )
			{
				result = writeName( "parent", geometry.getParent()->getName(), file )
					&& write( cuT( "cast_shadows" ), geometry.isShadowCaster(), file )
					&& write( cuT( "receives_shadows" ), geometry.isShadowReceiver(), file )
					&& write( cuT( "mesh" ), geometry.getMesh()->getName(), file );

				if ( result )
				{
					auto it = std::find_if( geometry.getMesh()->begin()
						, geometry.getMesh()->end()
						, [&geometry]( SubmeshSPtr lookup )
						{
							return geometry.getMaterial( *lookup ) != lookup->getDefaultMaterial();
						} );

					if ( it != geometry.getMesh()->end() )
					{
						if ( geometry.getMesh()->getSubmeshCount() == 1 )
						{
							result = writeName( cuT( "material" ), geometry.getMaterial( *geometry.getMesh()->getSubmesh( 0u ) )->getName(), file );
						}
						else if ( auto matsBlock = beginBlock( "materials", file ) )
						{
							for ( auto submesh : *geometry.getMesh() )
							{
								auto material = geometry.getMaterial( *submesh );

								if ( material != submesh->getDefaultMaterial() )
								{
									result = result
										&& file.writeText( tabs() + cuT( "material " ) + string::toString( submesh->getId() ) + cuT( " \"" ) + material->getName() + cuT( "\"\n" ) ) > 0;
								}
							}
						}
					}
				}
			}
		}

		return result;
	}
}
