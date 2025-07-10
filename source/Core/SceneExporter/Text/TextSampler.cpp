#include "TextSampler.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderTarget.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< Sampler >::TextWriter( castor::String const & tabs )
		: TextWriterT< Sampler >{ tabs }
	{
	}

	bool TextWriter< Sampler >::operator()( Sampler const & sampler
		, castor::StringStream & file )
	{
		bool result = true;

		if ( sampler.isSerialisable() )
		{
			result = false;
			log::info << tabs() << cuT( "Writing Sampler " ) << sampler.getName() << std::endl;

			if ( auto block{ beginBlock( file, cuT( "sampler" ), sampler.getName() ) } )
			{
				result = write( file, cuT( "min_filter" ), castor::makeString( getName( sampler.getMinFilter() ) ) )
					&& write( file, cuT( "mag_filter" ), castor::makeString( getName( sampler.getMagFilter() ) ) )
					&& write( file, cuT( "mip_filter" ), castor::makeString( getName( sampler.getMipFilter() ) ) )
					&& write( file, cuT( "u_wrap_mode" ), castor::makeString( getName( sampler.getWrapS() ) ) )
					&& write( file, cuT( "v_wrap_mode" ), castor::makeString( getName( sampler.getWrapT() ) ) )
					&& write( file, cuT( "w_wrap_mode" ), castor::makeString( getName( sampler.getWrapR() ) ) )
					&& write( file, cuT( "min_lod" ), sampler.getMinLod() )
					&& write( file, cuT( "max_lod" ), sampler.getMaxLod() )
					&& write( file, cuT( "lod_bias" ), sampler.getLodBias() )
					&& write( file, cuT( "border_colour" ), getName( sampler.getBorderColour() ) )
					&& write( file, cuT( "max_anisotropy" ), sampler.getMaxAnisotropy() );

				if ( result
					&& sampler.getCompareOp() != ComparisonFunc::eNever
					&& sampler.getCompareOp() != ComparisonFunc::eAlways )
				{
					result = write( file, cuT( "comparison_mode" ), String{ cuT( "ref_to_texture" ) } )
						&& write( file, cuT( "comparison_func" ), getName( sampler.getCompareOp() ) );
				}
			}
		}

		return result;
	}
}
