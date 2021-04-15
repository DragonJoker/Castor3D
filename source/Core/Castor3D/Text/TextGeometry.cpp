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

	bool TextWriter< Geometry >::operator()( Geometry const & geometry
		, StringStream & file )
	{
		bool result{ true };

		if ( geometry.getMesh() )
		{
			log::info << tabs() << cuT( "Writing Geometry " ) << geometry.getName() << std::endl;
			result = false;

			if ( auto block{ beginBlock( file, cuT( "object" ), geometry.getName() ) } )
			{
				result = writeName( file, "parent", geometry.getParent()->getName() )
					&& writeOpt( file, cuT( "cast_shadows" ), geometry.isShadowCaster(), true )
					&& writeOpt( file, cuT( "receive_shadows" ), geometry.isShadowReceiver(), true )
					&& writeName( file, cuT( "mesh" ), geometry.getMesh()->getName() );

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
							result = writeName( file, cuT( "material" ), geometry.getMaterial( *geometry.getMesh()->getSubmesh( 0u ) )->getName() );
						}
						else if ( auto matsBlock{ beginBlock( file, "materials" ) } )
						{
							for ( auto submesh : *geometry.getMesh() )
							{
								auto material = geometry.getMaterial( *submesh );

								if ( material != submesh->getDefaultMaterial() )
								{
									result = result
										&& writeText( file, tabs() + cuT( "material " ) + string::toString( submesh->getId() ) + cuT( " \"" ) + material->getName() + cuT( "\"\n" ) );
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
