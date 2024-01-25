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

		if ( sampler.getName() != cuT( "C3D_Lights" ) && sampler.getName() != RenderTarget::DefaultSamplerName )
		{
			result = false;
			log::info << tabs() << cuT( "Writing Sampler " ) << sampler.getName() << std::endl;

			if ( auto block{ beginBlock( file, cuT( "sampler" ), sampler.getName() ) } )
			{
				result = write( file, cuT( "min_filter" ), ashes::getName( sampler.getMinFilter() ) )
					&& write( file, cuT( "mag_filter" ), ashes::getName( sampler.getMagFilter() ) )
					&& write( file, cuT( "mip_filter" ), ashes::getName( sampler.getMipFilter() ) )
					&& write( file, cuT( "u_wrap_mode" ), ashes::getName( sampler.getWrapS() ) )
					&& write( file, cuT( "v_wrap_mode" ), ashes::getName( sampler.getWrapT() ) )
					&& write( file, cuT( "w_wrap_mode" ), ashes::getName( sampler.getWrapR() ) )
					&& write( file, cuT( "min_lod" ), sampler.getMinLod() )
					&& write( file, cuT( "max_lod" ), sampler.getMaxLod() )
					&& write( file, cuT( "lod_bias" ), sampler.getLodBias() )
					&& write( file, cuT( "border_colour" ), ashes::getName( sampler.getBorderColour() ) )
					&& write( file, cuT( "max_anisotropy" ), sampler.getMaxAnisotropy() );

				if ( result
					&& sampler.getCompareOp() != VK_COMPARE_OP_NEVER
					&& sampler.getCompareOp() != VK_COMPARE_OP_ALWAYS )
				{
					result = write( file, cuT( "comparison_mode" ), String{ cuT( "ref_to_texture" ) } )
						&& write( file, cuT( "comparison_func" ), ashes::getName( sampler.getCompareOp() ) );
				}
			}
		}

		return result;
	}

	TextWriter< ashes::Sampler >::TextWriter( castor::String const & tabs )
		: TextWriterT< ashes::Sampler >{ tabs }
	{
	}

	bool TextWriter< ashes::Sampler >::operator()( ashes::Sampler const & sampler
		, castor::StringStream & file )
	{
		bool result = true;

		if ( auto sName = castor::makeString( sampler.getName() );
			sName != cuT( "C3D_Lights" ) && sName != RenderTarget::DefaultSamplerName )
		{
			result = false;
			log::info << tabs() << cuT( "Writing Sampler " ) << sName << std::endl;

			if ( auto block{ beginBlock( file, cuT( "sampler" ), sName ) } )
			{
				result = write( file, cuT( "min_filter" ), ashes::getName( sampler.getMinFilter() ) )
					&& write( file, cuT( "mag_filter" ), ashes::getName( sampler.getMagFilter() ) )
					&& write( file, cuT( "mip_filter" ), ashes::getName( sampler.getMipFilter() ) )
					&& write( file, cuT( "u_wrap_mode" ), ashes::getName( sampler.getWrapS() ) )
					&& write( file, cuT( "v_wrap_mode" ), ashes::getName( sampler.getWrapT() ) )
					&& write( file, cuT( "w_wrap_mode" ), ashes::getName( sampler.getWrapR() ) )
					&& write( file, cuT( "min_lod" ), sampler.getMinLod() )
					&& write( file, cuT( "max_lod" ), sampler.getMaxLod() )
					&& write( file, cuT( "lod_bias" ), sampler.getLodBias() )
					&& write( file, cuT( "border_colour" ), ashes::getName( sampler.getBorderColour() ) )
					&& write( file, cuT( "max_anisotropy" ), sampler.getMaxAnisotropy() );

				if ( result
					&& sampler.getCompareOp() != VK_COMPARE_OP_NEVER
					&& sampler.getCompareOp() != VK_COMPARE_OP_ALWAYS )
				{
					result = write( file, cuT( "comparison_mode" ), String{ cuT( "ref_to_texture" ) } )
						&& write( file, cuT( "comparison_func" ), ashes::getName( sampler.getCompareOp() ) );
				}
			}
		}

		return result;
	}
}
