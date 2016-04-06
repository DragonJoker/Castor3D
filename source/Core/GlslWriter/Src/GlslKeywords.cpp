#include "GlslKeywords.hpp"

#include "GlslWriter.hpp"

using namespace Castor;

namespace GLSL
{
	std::unique_ptr< KeywordsBase > KeywordsBase::Get( GlslWriterConfig const & p_rs )
	{
		std::unique_ptr< KeywordsBase > l_return;

		switch ( p_rs.m_shaderLanguageVersion )
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

	String KeywordsBase::GetAttribute( uint32_t p_index )const
	{
		return GetLayout( p_index ) + m_strAttribute;
	}
}
