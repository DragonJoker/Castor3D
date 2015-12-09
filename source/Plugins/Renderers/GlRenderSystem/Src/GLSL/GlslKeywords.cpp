#include "GlslKeywords.hpp"

#include "OpenGl.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	namespace GLSL
	{
		std::unique_ptr< KeywordsBase > KeywordsBase::Get( OpenGl const & p_gl )
		{
			std::unique_ptr< KeywordsBase > l_return;

			switch ( p_gl.GetGlslVersion() )
			{
			case 110:
				l_return = std::make_unique< Keywords< 110 > >();
				break;

			case 120:
				l_return = std::make_unique< Keywords< 120 > >();
				break;

			case 130:
				l_return = std::make_unique< Keywords< 130 > >();
				break;

			case 140:
				l_return = std::make_unique< Keywords< 140 > >();
				break;

			case 150:
				l_return = std::make_unique< Keywords< 150 > >();
				break;

			case 330:
				l_return = std::make_unique< Keywords< 330 > >();
				break;

			case 400:
				l_return = std::make_unique< Keywords< 400 > >();
				break;

			case 410:
				l_return = std::make_unique< Keywords< 410 > >();
				break;

			case 420:
				l_return = std::make_unique< Keywords< 420 > >();
				break;

			case 430:
				l_return = std::make_unique< Keywords< 430 > >();
				break;

			default:
				l_return = std::make_unique< Keywords< 110 > >();
				break;
			}

			return l_return;
		}

		Castor::String KeywordsBase::GetAttribute( uint32_t p_index )const
		{
			return GetLayout( p_index ) + m_strAttribute;
		}
	}
}
