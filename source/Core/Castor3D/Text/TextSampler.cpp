#include "Castor3D/Text/TextSampler.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

using namespace castor3d;

namespace castor
{
	TextWriter< Sampler >::TextWriter( castor::String const & tabs )
		: TextWriterT< Sampler >{ tabs }
	{
	}

	bool TextWriter< Sampler >::operator()( Sampler const & sampler
		, castor::TextFile & file )
	{
		bool result = true;

		if ( sampler.getName() != cuT( "LightsSampler" ) && sampler.getName() != RenderTarget::DefaultSamplerName )
		{
			result = false;
			log::info << tabs() << cuT( "Writing Sampler " ) << sampler.getName() << std::endl;

			if ( auto block = beginBlock( file, "sampler", sampler.getName() ) )
			{
				result = write( file, "min_filter", ashes::getName( sampler.getMinFilter() ) )
					&& write( file, "mag_filter", ashes::getName( sampler.getMagFilter() ) )
					&& write( file, "mip_filter", ashes::getName( sampler.getMipFilter() ) )
					&& write( file, "u_wrap_mode", ashes::getName( sampler.getWrapS() ) )
					&& write( file, "v_wrap_mode", ashes::getName( sampler.getWrapT() ) )
					&& write( file, "w_wrap_mode", ashes::getName( sampler.getWrapR() ) )
					&& write( file, "min_lod", sampler.getMinLod() )
					&& write( file, "max_lod", sampler.getMaxLod() )
					&& write( file, "lod_bias", sampler.getLodBias() )
					&& write( file, "border_colour", ashes::getName( sampler.getBorderColour() ) )
					&& write( file, "max_anisotropy", sampler.getMaxAnisotropy() );

				if ( result
					&& sampler.getCompareOp() != VK_COMPARE_OP_NEVER
					&& sampler.getCompareOp() != VK_COMPARE_OP_ALWAYS )
				{
					result = write( file, "comparison_mode", String{ "ref_to_texture" } )
						&& write( file, "comparison_func", ashes::getName( sampler.getCompareOp() ) );
				}
			}
		}

		return result;
	}
}
