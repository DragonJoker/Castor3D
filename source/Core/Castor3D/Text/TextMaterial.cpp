#include "Castor3D/Text/TextMaterial.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Text/TextPhongPass.hpp"
#include "Castor3D/Text/TextMetallicRoughnessPbrPass.hpp"
#include "Castor3D/Text/TextSpecularGlossinessPbrPass.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< Material >::TextWriter( String const & tabs )
		: TextWriterT< Material >{ tabs }
	{
	}

	bool TextWriter< Material >::operator()( Material const & material, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing Material " ) << material.getName() << std::endl;
		bool result = false;

		if ( auto block = beginBlock( cuT( "material" ), material.getName(), file ) )
		{
			switch ( material.getType() )
			{
			case MaterialType::ePhong:
				for ( auto pass : material )
				{
					result = result && TextWriter< PhongPass >{ tabs() }( *std::static_pointer_cast< PhongPass >( pass ), file );
				}
				break;

			case MaterialType::eMetallicRoughness:
				for ( auto pass : material )
				{
					result = result && TextWriter< MetallicRoughnessPbrPass >{ tabs() }( *std::static_pointer_cast< MetallicRoughnessPbrPass >( pass ), file );
				}
				break;

			case MaterialType::eSpecularGlossiness:
				for ( auto pass : material )
				{
					result = result && TextWriter< SpecularGlossinessPbrPass >{ tabs() }( *std::static_pointer_cast< SpecularGlossinessPbrPass >( pass ), file );
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
