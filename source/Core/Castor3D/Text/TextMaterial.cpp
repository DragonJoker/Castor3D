#include "Castor3D/Text/TextMaterial.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"
#include "Castor3D/Text/TextPass.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< Material >::TextWriter( String const & tabs
		, Path const & folder
		, String const & subfolder )
		: TextWriterT< Material >{ tabs }
		, m_folder{ folder }
		, m_subfolder{ subfolder }
	{
	}

	bool TextWriter< Material >::operator()( Material const & material
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing Material " ) << material.getName() << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "material" ), material.getName() ) } )
		{
			result = true;

			if ( material.getRenderPassInfo() )
			{
				result = writeName( file, "render_pass", material.getRenderPassInfo()->name );
			}

			for ( auto pass : material )
			{
				result = result
					&& writeSub( file, *pass, m_folder, m_subfolder );
			}
		}

		return result;
	}
}
