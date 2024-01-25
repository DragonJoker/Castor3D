#include "TextSkeleton.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< Skeleton >::TextWriter( String const & tabs
		, String const & subfolder )
		: TextWriterT< Skeleton >{ tabs }
		, m_subfolder{ subfolder }
	{
	}

	bool TextWriter< Skeleton >::operator()( Skeleton const & object
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing Skeleton " ) << object.getName() << std::endl;
		bool result{ false };

		if ( auto block{ beginBlock( file, cuT( "skeleton" ), object.getName() ) } )
		{
			if ( !m_subfolder.empty() )
			{
				result = writeName( file, cuT( "import" ), cuT( "Skeletons/" ) + m_subfolder + cuT( "/" ) + object.getName() + cuT( ".cskl" ) );
			}
			else
			{
				result = writeName( file, cuT( "import" ), cuT( "Skeletons/" ) + object.getName() + cuT( ".cskl" ) );
			}

			for ( auto & animation : object.getAnimations() )
			{
				if ( !m_subfolder.empty() )
				{
					result = result && writeName( file, cuT( "import_anim" ), cuT( "Skeletons/" ) + m_subfolder + cuT( "/" ) + object.getName() + cuT( "-" ) + animation.first + cuT( ".cska" ) );
				}
				else
				{
					result = result && writeName( file, cuT( "import_anim" ), cuT( "Skeletons/" ) + object.getName() + cuT( "-" ) + animation.first + cuT( ".cska" ) );
				}
			}
		}

		return result;
	}
}
