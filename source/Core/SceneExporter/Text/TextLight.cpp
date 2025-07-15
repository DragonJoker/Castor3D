#include "TextLight.hpp"

#include "TextDirectionalLight.hpp"
#include "TextPointLight.hpp"
#include "TextShadow.hpp"
#include "TextSpotLight.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace c3d
{
	TextWriter< Light >::TextWriter( String const & tabs )
		: TextWriterT< Light >{ tabs }
	{
	}

	bool TextWriter< Light >::operator()( Light const & light
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing Light " ) << light.getName() << std::endl;
		bool result{ false };

		if ( auto block{ beginBlock( file, cuT( "light" ), light.getName() ) } )
		{
			result = write( file, cuT( "type" ), getName( light.getLightType() ) )
				&& writeName( file, cuT( "parent" ), light.getParent()->getName() )
				&& writeNamedSub( file, cuT( "colour" ), light.getColour() );

			if ( result )
			{
				switch ( light.getLightType() )
				{
				case LightType::eDirectional:
					result = writeSub( file, *light.getDirectionalLight() );
					break;
				case LightType::ePoint:
					result = writeSub( file, *light.getPointLight() );
					break;
				case LightType::eSpot:
					result = writeSub( file, *light.getSpotLight() );
					break;
				default:
					result = false;
					break;
				}
			}

			if ( result )
			{
				result = writeSubOpt( file, light.getShadowConfig(), ShadowConfig{} );
			}
		}

		return result;
	}
}
