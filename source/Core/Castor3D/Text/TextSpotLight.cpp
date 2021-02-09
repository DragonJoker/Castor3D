#include "Castor3D/Text/TextSpotLight.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Text/TextLpvConfig.hpp"
#include "Castor3D/Text/TextRsmConfig.hpp"
#include "Castor3D/Text/TextShadow.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< SpotLight >::TextWriter( String const & tabs )
		: TextWriterT< SpotLight >{ tabs }
	{
	}

	bool TextWriter< SpotLight >::operator()( SpotLight const & light
		, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing SpotLight " ) << light.getLight().getName() << std::endl;
		bool result{ false };

		if ( auto block = beginBlock( "light", light.getLight().getName(), file ) )
		{
			result = writeName( "parent", light.getLight().getParent()->getName(), file )
				&& write( cuT( "type" ), castor3d::getName( light.getLightType() ), file )
				&& write( cuT( "colour" ), light.getColour(), file )
				&& write( cuT( "intensity" ), light.getIntensity(), file )
				&& write( cuT( "attenuation" ), light.getAttenuation(), file )
				&& write( cuT( "exponent" ), light.getExponent(), file )
				&& write( cuT( "cut_off" ), light.getCutOff().degrees(), file )
				&& write( light.getShadowConfig(), file )
				&& write( light.getRsmConfig(), file )
				&& write( light.getLpvConfig(), file );
		}

		return result;
	}
}
