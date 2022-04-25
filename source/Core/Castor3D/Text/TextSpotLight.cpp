#include "Castor3D/Text/TextSpotLight.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Text/TextShadow.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< SpotLight >::TextWriter( String const & tabs )
		: TextWriterT< SpotLight >{ tabs }
	{
	}

	bool TextWriter< SpotLight >::operator()( SpotLight const & light
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing SpotLight " ) << light.getLight().getName() << std::endl;
		bool result{ false };

		if ( auto block{ beginBlock( file, "light", light.getLight().getName() ) } )
		{
			result = writeName( file, "parent", light.getLight().getParent()->getName() )
				&& write( file, cuT( "type" ), castor3d::getName( light.getLightType() ) )
				&& writeNamedSub( file, cuT( "colour" ), light.getColour() )
				&& writeNamedSub( file, cuT( "intensity" ), light.getIntensity() )
				&& writeNamedSub( file, cuT( "attenuation" ), light.getAttenuation() )
				&& write( file, cuT( "exponent" ), light.getExponent() )
				&& write( file, cuT( "inner_cut_off" ), light.getInnerCutOff().degrees() )
				&& write( file, cuT( "outer_cut_off" ), light.getOuterCutOff().degrees() )
				&& write( file, cuT( "shadow_producer" ), light.getLight().isShadowProducer() )
				&& writeSub( file, light.getShadowConfig() );
		}

		return result;
	}
}
