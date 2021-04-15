#include "Castor3D/Text/TextSsaoConfig.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< SsaoConfig >::TextWriter( String const & tabs )
		: TextWriterT< SsaoConfig >{ tabs }
	{
	}

	bool TextWriter< SsaoConfig >::operator()( SsaoConfig const & object
		, StringStream & file )
	{
		bool result{ false };
		log::info << tabs() << cuT( "Writing SsaoConfig" ) << std::endl;

		if ( auto block{ beginBlock( file, "ssao" ) } )
		{
			result = write( file, cuT( "enabled" ), object.enabled )
				&& write( file, cuT( "high_quality" ), object.highQuality )
				&& write( file, cuT( "use_normals_buffer" ), object.useNormalsBuffer )
				&& write( file, cuT( "blur_high_quality" ), object.blurHighQuality )
				&& write( file, cuT( "intensity" ), object.intensity )
				&& write( file, cuT( "radius" ), object.radius )
				&& write( file, cuT( "bias" ), object.bias )
				&& write( file, cuT( "num_samples" ), object.numSamples )
				&& write( file, cuT( "edge_sharpness" ), object.edgeSharpness )
				&& write( file, cuT( "blur_step_size" ), object.blurStepSize )
				&& write( file, cuT( "blur_radius" ), object.blurRadius )
				&& write( file, cuT( "bend_step_count" ), object.bendStepCount )
				&& write( file, cuT( "bend_step_size" ), object.bendStepSize );
		}

		return result;
	}
}
