#include "Castor3D/Text/TextMesh.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< Mesh >::TextWriter( String const & tabs
		, String const & subfolder )
		: TextWriterT< Mesh >{ tabs }
		, m_subfolder{ subfolder }
	{
	}

	bool TextWriter< Mesh >::operator()( Mesh const & object
		, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing Mesh " ) << object.getName() << std::endl;
		bool result{ false };

		if ( auto block{ beginBlock( file, "mesh", object.getName() ) } )
		{
			if ( !m_subfolder.empty() )
			{
				result = writeName( file, "import", "Meshes/" + m_subfolder + "/" + object.getName() + ".cmsh" );
			}
			else
			{
				result = writeName( file, "import", "Meshes/" + object.getName() + ".cmsh" );
			}

			auto it = std::find_if( object.begin()
				, object.end()
				, []( SubmeshSPtr lookup )
				{
					return lookup->getDefaultMaterial() != nullptr;
				} );

			if ( it != object.end() )
			{
				if ( object.getSubmeshCount() == 1 )
				{
					result = writeName( file, cuT( "default_material" ), object.getSubmesh( 0u )->getDefaultMaterial()->getName() );
				}
				else if ( auto matsBlock{ beginBlock( file, "default_materials" ) } )
				{
					for ( auto & submesh : object )
					{
						if ( submesh->getDefaultMaterial() )
						{
							result = file.writeText( tabs() + cuT( "material " ) + string::toString( submesh->getId() ) + cuT( " \"" ) + submesh->getDefaultMaterial()->getName() + cuT( "\"\n" ) ) > 0;
						}
					}
				}
			}
		}

		return result;
	}
}
