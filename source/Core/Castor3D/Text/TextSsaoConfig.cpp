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
		, TextFile & file )
	{
		bool result{ false };
		log::info << tabs() << cuT( "Writing SsaoConfig" ) << std::endl;

		if ( auto block = beginBlock( "ssao", file ) )
		{
			result = write( cuT( "enabled" ), object.enabled, file )
				&& write( cuT( "high_quality" ), object.highQuality, file )
				&& write( cuT( "use_normals_buffer" ), object.useNormalsBuffer, file )
				&& write( cuT( "blur_high_quality" ), object.blurHighQuality, file )
				&& write( cuT( "intensity" ), object.intensity, file )
				&& write( cuT( "radius" ), object.radius, file )
				&& write( cuT( "bias" ), object.bias, file )
				&& write( cuT( "num_samples" ), object.numSamples, file )
				&& write( cuT( "edge_sharpness" ), object.edgeSharpness, file )
				&& write( cuT( "blur_step_size" ), object.blurStepSize, file )
				&& write( cuT( "blur_radius" ), object.blurRadius, file )
				&& write( cuT( "bend_step_count" ), object.bendStepCount, file )
				&& write( cuT( "bend_step_size" ), object.bendStepSize, file );
		}

		return result;
	}
}
