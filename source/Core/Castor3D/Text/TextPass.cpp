#include "Castor3D/Text/TextPass.hpp"

#include "Castor3D/Text/TextSubsurfaceScattering.hpp"
#include "Castor3D/Text/TextTextureUnit.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>

using namespace castor3d;

namespace castor
{	
	TextWriter< Pass >::TextWriter( String const & tabs )
		: TextWriterT< Pass >{ tabs }
	{
	}

	bool TextWriter< Pass >::operator()( Pass const & pass
		, TextFile & file )
	{
		static std::map< VkCompareOp, String > strAlphaFuncs
		{
			{ VK_COMPARE_OP_ALWAYS, cuT( "always" ) },
			{ VK_COMPARE_OP_LESS, cuT( "less" ) },
			{ VK_COMPARE_OP_LESS_OR_EQUAL, cuT( "less_or_equal" ) },
			{ VK_COMPARE_OP_EQUAL, cuT( "equal" ) },
			{ VK_COMPARE_OP_NOT_EQUAL, cuT( "not_equal" ) },
			{ VK_COMPARE_OP_GREATER_OR_EQUAL, cuT( "greater_or_equal" ) },
			{ VK_COMPARE_OP_GREATER, cuT( "greater" ) },
			{ VK_COMPARE_OP_NEVER, cuT( "never" ) },
		};
		static const String StrBlendModes[uint32_t( BlendMode::eCount )] =
		{
			cuT( "none" ),
			cuT( "additive" ),
			cuT( "multiplicative" ),
			cuT( "interpolative" ),
			cuT( "a_buffer" ),
			cuT( "depth_peeling" ),
		};

		bool result = true;

		if ( pass.getOpacity() < 1 )
		{
			result = write( cuT( "alpha" ), pass.getOpacity(), file )
				&& write( cuT( "bw_accumulation" ), uint32_t( pass.getBWAccumulationOperator() ), file );
		}

		if ( result )
		{
			result = writeOpt( cuT( "emissive" ), pass.getEmissive(), 0.0f, file )
				&& writeOpt( cuT( "transmission" ), pass.getTransmission(), castor::Point3f{ 1.0f, 1.0f, 1.0f }, file )
				&& writeOpt( cuT( "two_sided" ), pass.isTwoSided(), true, file )
				&& writeOpt( cuT( "colour_blend_mode" ), StrBlendModes[uint32_t( pass.getColourBlendMode() )], StrBlendModes[uint32_t( BlendMode::eNoBlend )], file );
		}

		if ( result )
		{
			if ( pass.hasAlphaTest() )
			{
				result = write( cuT( "alpha_func" ), strAlphaFuncs[pass.getAlphaFunc()], file );
			}
			else if ( pass.hasAlphaBlending()
				&& pass.getAlphaBlendMode() != BlendMode::eNoBlend )
			{
				result = write( cuT( "alpha_blend_mode" ), StrBlendModes[uint32_t( pass.getAlphaBlendMode() )], file );
			}
		}

		if ( result )
		{
			result = writeOpt( cuT( "parallax_occlusion" ), pass.hasParallaxOcclusion(), file )
				&& writeOpt( cuT( "parallax_occlusion" ), pass.hasEnvironmentMapping(), file );
		}

		if ( result )
		{
			for ( auto unit : pass )
			{
				result = result && TextWriter< TextureUnit >{ tabs(), pass.getType() }( *unit, file );
			}
		}

		if ( result && pass.hasSubsurfaceScattering() )
		{
			result = TextWriter< SubsurfaceScattering >{ tabs() }( pass.getSubsurfaceScattering(), file );
		}

		return result;
	}
}
