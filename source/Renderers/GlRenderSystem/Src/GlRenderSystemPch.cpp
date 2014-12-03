#include "GlRenderSystemPch.hpp"

#include "OpenGl.hpp"

namespace GlRender
{
	namespace GLSL
	{
		GLSL::VariablesBase * GetVariables( OpenGl const & p_gl )
		{
			if ( p_gl.HasUbo() )
			{
				return &GLSL::variablesUbo;
			}

			return &GLSL::variablesStd;
		}

		GLSL::ConstantsBase * GetConstants( OpenGl const & p_gl )
		{
			if ( p_gl.HasUbo() )
			{
				return &GLSL::constantsUbo;
			}

			return &GLSL::constantsStd;
		}

		std::unique_ptr< GLSL::KeywordsBase > GetKeywords( OpenGl const & p_gl )
		{
			std::unique_ptr< GLSL::KeywordsBase > l_return;

			switch ( p_gl.GetGlslVersion() )
			{
			case 110:
				l_return = std::make_unique< GLSL::Keywords< 110 > >();
				break;

			case 120:
				l_return = std::make_unique< GLSL::Keywords< 120 > >();
				break;

			case 130:
				l_return = std::make_unique< GLSL::Keywords< 130 > >();
				break;

			case 140:
				l_return = std::make_unique< GLSL::Keywords< 140 > >();
				break;

			case 150:
				l_return = std::make_unique< GLSL::Keywords< 150 > >();
				break;

			case 330:
				l_return = std::make_unique< GLSL::Keywords< 330 > >();
				break;

			case 400:
				l_return = std::make_unique< GLSL::Keywords< 400 > >();
				break;

			case 410:
				l_return = std::make_unique< GLSL::Keywords< 410 > >();
				break;

			case 420:
				l_return = std::make_unique< GLSL::Keywords< 420 > >();
				break;

			case 430:
				l_return = std::make_unique< GLSL::Keywords< 430 > >();
				break;

			default:
				l_return = std::make_unique< GLSL::Keywords< 110 > >();
				break;
			}

			return l_return;
		}
	}
}
