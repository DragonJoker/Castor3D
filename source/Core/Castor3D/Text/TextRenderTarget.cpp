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

	bool TextWriter< RenderTarget >::operator()( RenderTarget const & target, TextFile & file )
	{
		log::info << cuT( "Writing RenderTarget" ) << std::endl;
		bool result = false;

		if ( auto block = beginBlock( "render_target", file ) )
		{
			if ( target.getScene() )
			{
				result = writeName( cuT( "scene" ), target.getScene()->getName(), file );
			}

			if ( result && target.getCamera() )
			{
				result = writeName( cuT( "camera" ), target.getCamera()->getName(), file );
			}

			if ( result )
			{
				result = write( cuT( "size" ), target.getSize(), file )
					&& write( cuT( "format" ), PF::getFormatName( convert( target.getPixelFormat() ) ), file )
					&& writeName( cuT( "tone_mapping" ), target.getToneMapping()->getName(), file );
			}

			if ( result )
			{
				for ( auto const & effect : target.getHDRPostEffects() )
				{
					result = effect->writeInto( file, tabs() ) && file.writeText( cuT( "\n" ) ) > 0;
				}
			}

			if ( result )
			{
				for ( auto const & effect : target.getSRGBPostEffects() )
				{
					result = effect->writeInto( file, tabs() ) && file.writeText( cuT( "\n" ) ) > 0;
				}
			}

			if ( result )
			{
				result = TextWriter< SsaoConfig >{ tabs() }( target.getSsaoConfig(), file );
			}
		}

		return result;
	}
}
