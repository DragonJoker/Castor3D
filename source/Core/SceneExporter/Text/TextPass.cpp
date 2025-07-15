#include "TextPass.hpp"

#include "TextSubsurfaceScattering.hpp"
#include "TextTextureUnit.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextRgbaColour.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

#include <Castor3D/Material/Texture/TextureUnit.hpp>

namespace c3d
{
	namespace txtpass
	{
		static bool isSerialisable( TextureUnit const & unit )
		{
			return unit.getData().base->sourceInfo.isSerialisable();
		}
	}

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
					if ( txtpass::isSerialisable( *unit ) )
					{
						result = result
							&& writeSub( file
								, *unit
								, m_folder
								, m_subfolder );
					}
				}
			}
		}

		return result;
	}
}
