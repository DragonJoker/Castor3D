#include "Castor3D/Text/TextSkeleton.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

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

		if ( auto block{ beginBlock( file, "skeleton", object.getName() ) } )
		{
			if ( !m_subfolder.empty() )
			{
				result = writeName( file, "import", "Skeletons/" + m_subfolder + "/" + object.getName() + ".cskl" );
			}
			else
			{
				result = writeName( file, "import", "Skeletons/" + object.getName() + ".cskl" );
			}
		}

		return result;
	}
}
