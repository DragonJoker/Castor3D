#include "Castor3D/Text/TextPass.hpp"

#include "Castor3D/Text/TextSubsurfaceScattering.hpp"
#include "Castor3D/Text/TextTextureUnit.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>

using namespace castor3d;

namespace castor
{
	TextWriter< Pass >::TextWriter( String const & tabs
		, String const & subfolder )
		: TextWriterT< Pass >{ tabs }
		, m_subfolder{ subfolder }
	{
	}

	bool TextWriter< Pass >::operator()( Pass const & pass
		, TextFile & file )
	{
		static std::map< VkCompareOp, String > strAlphaFuncs
		{
			{ VK_COMPARE_OP_ALWAYS, cuT( "always" ) },
			{ VK_COMPARE_OP_LESS, cuT( "less" ) },
			{ VK_COMPARE_OP_LESS_OR_EQUAL, cuT( "less_equal" ) },
			{ VK_COMPARE_OP_EQUAL, cuT( "equal" ) },
			{ VK_COMPARE_OP_NOT_EQUAL, cuT( "not_equal" ) },
			{ VK_COMPARE_OP_GREATER_OR_EQUAL, cuT( "greater_equal" ) },
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
			result = write( file, cuT( "alpha" ), pass.getOpacity() )
				&& write( file, cuT( "bw_accumulation" ), uint32_t( pass.getBWAccumulationOperator() ) );
		}

		if ( result )
		{
			result = writeOpt( file, cuT( "emissive" ), pass.getEmissive(), 0.0f )
				&& writeNamedSubOpt( file, cuT( "transmission" ), pass.getTransmission(), castor::Point3f{ 1.0f, 1.0f, 1.0f } )
				&& writeOpt( file, cuT( "two_sided" ), pass.isTwoSided(), true )
				&& writeOpt( file, cuT( "colour_blend_mode" ), StrBlendModes[uint32_t( pass.getColourBlendMode() )], StrBlendModes[uint32_t( BlendMode::eNoBlend )] );
		}

		if ( result && pass.hasAlphaTest() )
		{
			result = write( file, cuT( "alpha_func" ), strAlphaFuncs[pass.getAlphaFunc()], pass.getAlphaValue() );
		}
			
		if ( result
			&& pass.hasAlphaBlending() )
		{
			if ( pass.hasBlendAlphaTest() )
			{
				result = write( file, cuT( "blend_alpha_func" ), strAlphaFuncs[pass.getBlendAlphaFunc()], pass.getAlphaValue() );
			}

			if ( result )
			{
				result = write( file, cuT( "alpha_blend_mode" ), StrBlendModes[uint32_t( pass.getAlphaBlendMode() )] );
			}
		}

		if ( result )
		{
			result = writeOpt( file, cuT( "parallax_occlusion" ), pass.hasParallaxOcclusion() )
				&& writeOpt( file, cuT( "reflections" ), pass.hasReflections() )
				&& writeOpt( file, cuT( "refractions" ), pass.hasRefraction() )
				&& writeOpt( file, cuT( "refraction_ratio" ), pass.getRefractionRatio(), 0.0f );
		}

		if ( result )
		{
			for ( auto unit : pass )
			{
				result = result
					&& writeSub( file, *unit, pass.getType(), m_subfolder );
			}
		}

		if ( result && pass.hasSubsurfaceScattering() )
		{
			result = writeSub( file, pass.getSubsurfaceScattering() );
		}

		return result;
	}
}
