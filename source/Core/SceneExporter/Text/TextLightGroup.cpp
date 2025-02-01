#include "TextLightGroup.hpp"

#include "TextDirectionalLight.hpp"
#include "TextPointLight.hpp"
#include "TextShadow.hpp"
#include "TextSpotLight.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< LightGroup >::TextWriter( String const & tabs )
		: TextWriterT< LightGroup >{ tabs }
	{
	}

	bool TextWriter< LightGroup >::operator()( LightGroup const & lightGroup
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing LightGroup " ) << lightGroup.getName() << std::endl;
		bool result{ false };

		if ( auto block{ beginBlock( file, cuT( "light_group" ), lightGroup.getName() ) } )
		{
			result = write( file, cuT( "type" ), castor3d::getName( lightGroup.getLightType() ) )
				&& writeNamedSub( file, cuT( "colour" ), lightGroup.getColour() );

			if ( result )
			{
				switch ( lightGroup.getLightType() )
				{
				case LightType::eDirectional:
					result = writeSub( file, *lightGroup.getDirectionalLight() );
					break;
				case LightType::ePoint:
					result = writeSub( file, *lightGroup.getPointLight() );
					break;
				case LightType::eSpot:
					result = writeSub( file, *lightGroup.getSpotLight() );
					break;
				default:
					result = false;
					break;
				}
			}

			if ( result )
			{
				result = writeSubOpt( file, lightGroup.getShadowConfig(), ShadowConfig{} );
			}

			if ( result )
			{
				auto instances{ beginBlock( file, cuT( "instances" ) ) };

				for ( auto & instance : lightGroup )
				{
					result = result && writeName( file, cuT( "instance" ), instance->getName() );
				}
			}
		}

		return result;
	}
}
