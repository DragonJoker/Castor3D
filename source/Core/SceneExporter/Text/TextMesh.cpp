#include "TextMesh.hpp"

#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< Mesh >::TextWriter( String const & tabs
		, String const & subfolder )
		: TextWriterT< Mesh >{ tabs }
		, m_subfolder{ subfolder }
	{
	}

	bool TextWriter< Mesh >::operator()( Mesh const & object
		, StringStream & file )
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

			for ( auto & animation : object.getAnimations() )
			{
				if ( !m_subfolder.empty() )
				{
					result = result && writeName( file, "import_anim", "Meshes/" + m_subfolder + "/" + object.getName() + "-" + animation.first + ".cmsa" );
				}
				else
				{
					result = result && writeName( file, "import_anim", "Meshes/" + object.getName() + "-" + animation.first + ".cmsa" );
				}
			}

			if ( auto skeleton = object.getSkeleton() )
			{
				result = result && writeName( file, "skeleton", skeleton->getName() );
			}

			auto it = std::find_if( object.begin()
				, object.end()
				, []( SubmeshUPtr const & lookup )
				{
					return lookup->getDefaultMaterial() != nullptr;
				} );

			if ( result && it != object.end() )
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
							result = writeText( file, tabs() + cuT( "material " ) + string::toString( submesh->getId() ) + cuT( " \"" ) + submesh->getDefaultMaterial()->getName() + cuT( "\"\n" ) );
						}
					}
				}
			}
		}

		return result;
	}
}
