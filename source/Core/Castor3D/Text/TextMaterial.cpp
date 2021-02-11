#include "Castor3D/Text/TextMaterial.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Text/TextPhongPass.hpp"
#include "Castor3D/Text/TextMetallicRoughnessPbrPass.hpp"
#include "Castor3D/Text/TextSpecularGlossinessPbrPass.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< Material >::TextWriter( String const & tabs
		, String const & subfolder )
		: TextWriterT< Material >{ tabs }
		, m_subfolder{ subfolder }
	{
	}

	bool TextWriter< Material >::operator()( Material const & material, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing Material " ) << material.getName() << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "material" ), material.getName() ) } )
		{
			result = true;

			switch ( material.getType() )
			{
			case MaterialType::ePhong:
				for ( auto pass : material )
				{
					result = result
						&& writeSub( file, *std::static_pointer_cast< PhongPass >( pass ), m_subfolder );
				}
				break;

			case MaterialType::eMetallicRoughness:
				for ( auto pass : material )
				{
					result = result
						&& writeSub( file, *std::static_pointer_cast< MetallicRoughnessPbrPass >( pass ), m_subfolder );
				}
				break;

			case MaterialType::eSpecularGlossiness:
				for ( auto pass : material )
				{
					result = result
						&& writeSub( file, *std::static_pointer_cast< SpecularGlossinessPbrPass >( pass ), m_subfolder );
				}
				break;

			default:
				CU_Failure( cuT( "Unsupported pass type" ) );
				break;
			}
		}

		return result;
	}
}
