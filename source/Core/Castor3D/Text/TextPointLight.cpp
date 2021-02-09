#include "Castor3D/Text/TextPointLight.hpp"

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
	TextWriter< PointLight >::TextWriter( String const & tabs )
		: TextWriterT< PointLight >{ tabs }
	{
	}

	bool TextWriter< PointLight >::operator()( PointLight const & light
		, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing PointLight " ) << light.getLight().getName() << std::endl;
		bool result{ false };

		if ( auto block = beginBlock( "light", light.getLight().getName(), file ) )
		{
			result = writeName( "parent", light.getLight().getParent()->getName(), file )
				&& write( cuT( "type" ), castor3d::getName( light.getLightType() ), file )
				&& write( cuT( "colour" ), light.getColour(), file )
				&& write( cuT( "intensity" ), light.getIntensity(), file )
				&& write( cuT( "attenuation" ), light.getAttenuation(), file )
				&& write( light.getShadowConfig(), file )
				&& write( light.getRsmConfig(), file )
				&& write( light.getLpvConfig(), file );
		}

		return result;
	}
}
