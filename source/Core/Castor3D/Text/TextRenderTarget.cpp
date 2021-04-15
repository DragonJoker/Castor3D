#include "Castor3D/Text/TextRenderTarget.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/PostEffect/PostEffect.hpp"
#include "Castor3D/Render/ToneMapping/ToneMapping.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Text/TextSsaoConfig.hpp"

#include <CastorUtils/Graphics/PixelFormat.hpp>
#include <CastorUtils/Data/Text/TextSize.hpp>

using namespace castor3d;

namespace castor
{
	//*************************************************************************************************

	TextWriter< RenderTarget >::TextWriter( String const & tabs )
		: TextWriterT< RenderTarget >{ tabs }
	{
	}

	bool TextWriter< RenderTarget >::operator()( RenderTarget const & target
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing RenderTarget" ) << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, "render_target" ) } )
		{
			if ( target.getScene() )
			{
				result = writeName( file, cuT( "scene" ), target.getScene()->getName() );
			}

			if ( result && target.getCamera() )
			{
				result = writeName( file, cuT( "camera" ), target.getCamera()->getName() );
			}

			if ( result )
			{
				result = writeNamedSub( file, cuT( "size" ), target.getSize() )
					&& write( file, cuT( "format" ), getFormatName( convert( target.getPixelFormat() ) ) )
					&& writeName( file, cuT( "tone_mapping" ), target.getToneMapping()->getName() );
			}

			if ( result )
			{
				for ( auto const & effect : target.getHDRPostEffects() )
				{
					result = effect->writeInto( file, tabs() ) && writeText( file, cuT( "\n" ) );
				}
			}

			if ( result )
			{
				for ( auto const & effect : target.getSRGBPostEffects() )
				{
					result = effect->writeInto( file, tabs() ) && writeText( file, cuT( "\n" ) );
				}
			}

			if ( result )
			{
				result = writeSub( file, target.getSsaoConfig() );
			}
		}

		return result;
	}
}
