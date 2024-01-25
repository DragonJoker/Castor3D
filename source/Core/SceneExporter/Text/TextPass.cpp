#include "TextPass.hpp"

#include "TextSubsurfaceScattering.hpp"
#include "TextTextureUnit.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextRgbaColour.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< Pass >::TextWriter( String const & tabs
		, Path const & folder
		, String const & subfolder )
		: TextWriterT< Pass >{ tabs }
		, m_folder{ folder }
		, m_subfolder{ subfolder }
	{
	}

	bool TextWriter< Pass >::operator()( Pass const & pass
		, StringStream & file )
	{
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "pass" ) ) } )
		{
			result = pass.writeText( tabs(), m_folder, m_subfolder, file );

			if ( result )
			{
				for ( auto unit : pass )
				{
					result = result
						&& writeSub( file
							, *unit
							, m_folder
							, m_subfolder );
				}
			}
		}

		return result;
	}
}
