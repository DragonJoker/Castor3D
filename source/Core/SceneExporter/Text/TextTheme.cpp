#include "TextTheme.hpp"

#include "TextStylesHolder.hpp"

namespace c3d
{
	//*********************************************************************************************

	namespace stlhdrwrtr
	{
		static bool filter( Theme const & theme
			, Scene const * scene )
		{
			if ( !theme.hasScene() && !scene )
			{
				return true;
			}

			return theme.hasScene()
				&& scene == &theme.getScene();
		}
	}

	//*********************************************************************************************

	TextWriter< Theme >::TextWriter( String const & tabs
		, Scene const * scene )
		: TextWriterT< Theme >{ tabs }
		, m_scene{ scene }
	{
	}

	bool TextWriter< Theme >::operator()( Theme const & theme
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing Theme " ) << theme.getName() << std::endl;

		if ( !stlhdrwrtr::filter( theme, m_scene ) )
		{
			return true;
		}

		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "theme" ), theme.getName() ) } )
		{
			result = TextWriter< StylesHolder >{ tabs(), m_scene, cuT( "" ) }( theme, file );
		}

		return result;
	}

	//*********************************************************************************************
}
