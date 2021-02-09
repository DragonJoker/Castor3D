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
			log::info << cuT( "Writing Sampler " ) << sampler.getName() << std::endl;

			if ( auto block = beginBlock( "sampler", sampler.getName(), file ) )
			{
				result = write( "min_filter", ashes::getName( sampler.getMinFilter() ), file )
					&& write( "mag_filter", ashes::getName( sampler.getMagFilter() ), file )
					&& write( "mip_filter", ashes::getName( sampler.getMipFilter() ), file )
					&& write( "u_wrap_mode", ashes::getName( sampler.getWrapS() ), file )
					&& write( "v_wrap_mode", ashes::getName( sampler.getWrapT() ), file )
					&& write( "w_wrap_mode", ashes::getName( sampler.getWrapR() ), file )
					&& write( "min_lod", sampler.getMinLod(), file )
					&& write( "max_lod", sampler.getMaxLod(), file )
					&& write( "lod_bias", sampler.getLodBias(), file )
					&& write( "border_colour", ashes::getName( sampler.getBorderColour() ), file )
					&& write( "max_anisotropy", sampler.getMaxAnisotropy(), file );

				if ( result
					&& sampler.getCompareOp() != VK_COMPARE_OP_NEVER
					&& sampler.getCompareOp() != VK_COMPARE_OP_ALWAYS )
				{
					result = write( "comparison_mode", String{ "ref_to_texture" }, file )
						&& write( "comparison_func", ashes::getName( sampler.getCompareOp() ), file );
				}
			}
		}

		return result;
	}
}
