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
		bool result = false;

		if ( auto block = beginBlock( "viewport", file ) )
		{
			result = write( "type", Viewport::TypeName[size_t( viewport.getType() )], file );

			if ( result )
			{
				if ( viewport.getType() == ViewportType::eOrtho || viewport.getType() == ViewportType::eFrustum )
				{
					result = write( cuT( "near " ), viewport.getNear(), file )
						&& write( cuT( "far " ), viewport.getFar(), file )
						&& write( cuT( "left " ), viewport.getLeft(), file )
						&& write( cuT( "right " ), viewport.getRight(), file )
						&& write( cuT( "top " ), viewport.getTop(), file )
						&& write( cuT( "bottom " ), viewport.getBottom(), file );
				}
				else
				{
					result = write( cuT( "near " ), viewport.getNear(), file )
						&& write( cuT( "far " ), viewport.getFar(), file )
						&& write( cuT( "aspect_ratio " ), viewport.getRatio(), file )
						&& write( cuT( "fov_y " ), viewport.getFovY().degrees(), file );
				}
			}
		}

		return result;
	}
}
