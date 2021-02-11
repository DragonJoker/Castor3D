#include "Castor3D/Text/TextViewport.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< Viewport >::TextWriter( String const & tabs )
		: TextWriterT< Viewport >{ tabs }
	{
	}

	bool TextWriter< Viewport >::operator()( Viewport const & viewport
		, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing Viewport" ) << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, "viewport" ) } )
		{
			result = write( file, "type", Viewport::TypeName[size_t( viewport.getType() )] );

			if ( result )
			{
				if ( viewport.getType() == ViewportType::eOrtho || viewport.getType() == ViewportType::eFrustum )
				{
					result = write( file, cuT( "near" ), viewport.getNear() )
						&& write( file, cuT( "far" ), viewport.getFar() )
						&& write( file, cuT( "left" ), viewport.getLeft() )
						&& write( file, cuT( "right" ), viewport.getRight() )
						&& write( file, cuT( "top" ), viewport.getTop() )
						&& write( file, cuT( "bottom" ), viewport.getBottom() );
				}
				else
				{
					result = write( file, cuT( "near" ), viewport.getNear() )
						&& write( file, cuT( "far" ), viewport.getFar() )
						&& write( file, cuT( "aspect_ratio" ), viewport.getRatio() )
						&& write( file, cuT( "fov_y" ), viewport.getFovY().degrees() );
				}
			}
		}

		return result;
	}
}
