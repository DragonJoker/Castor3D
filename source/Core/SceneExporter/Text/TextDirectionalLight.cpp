#include "TextDirectionalLight.hpp"

#include "TextShadow.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextRgbColour.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< DirectionalLight >::TextWriter( String const & tabs )
		: TextWriterT< DirectionalLight >{ tabs }
	{
	}

	bool TextWriter< DirectionalLight >::operator()( DirectionalLight const & light
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing DirectionalLight " ) << light.getLight().getName() << std::endl;
		bool result{ false };

		if ( auto block{ beginBlock( file, cuT( "light" ), light.getLight().getName() ) } )
		{
			result = writeName( file, cuT( "parent" ), light.getLight().getParent()->getName() )
				&& write( file, cuT( "type" ), castor3d::getName( light.getLightType() ) )
				&& writeNamedSub( file, cuT( "colour" ), light.getColour() )
				&& writeNamedSub( file, cuT( "intensity" ), light.getIntensity() )
				&& writeSubOpt( file, light.getShadowConfig(), ShadowConfig{} );
		}

		return result;
	}
}
