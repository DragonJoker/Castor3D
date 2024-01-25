#include "TextRenderTarget.hpp"

#include "TextClustersConfig.hpp"
#include "TextSsaoConfig.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/Clustered/FrustumClusters.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>
#include <Castor3D/Render/ToneMapping/ToneMapping.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <CastorUtils/Graphics/PixelFormat.hpp>
#include <CastorUtils/Data/Text/TextSize.hpp>

namespace castor
{
	using namespace castor3d;

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

		if ( auto block{ beginBlock( file, cuT( "render_target" ) ) } )
		{
			result = writeNamedSub( file, cuT( "size" ), target.getSize() )
				&& write( file, cuT( "format" ), getFormatName( convert( target.getPixelFormat() ) ) )
				&& writeName( file, cuT( "tone_mapping" ), target.getToneMapping()->getName() );

			if ( result && target.getScene() )
			{
				result = writeName( file, cuT( "scene" ), target.getScene()->getName() );
			}

			if ( result && target.getCamera() )
			{
				result = writeName( file, cuT( "camera" ), target.getCamera()->getName() );
			}

			if ( result )
			{
				for ( auto const & effect : target.getHDRPostEffects() )
				{
					if ( effect->isEnabled() )
					{
						result = effect->writeInto( file, tabs() );
					}
				}
			}

			if ( result )
			{
				for ( auto const & effect : target.getSRGBPostEffects() )
				{
					if ( effect->isEnabled() )
					{
						result = effect->writeInto( file, tabs() );
					}
				}
			}

			if ( result )
			{
				result = writeSubOpt( file, target.getSsaoConfig(), SsaoConfig{} );
			}

			if ( result && target.getFrustumClusters() )
			{
				result = writeSubOpt( file, target.getFrustumClusters()->getConfig(), ClustersConfig{} );
			}
		}

		return result;
	}
}
