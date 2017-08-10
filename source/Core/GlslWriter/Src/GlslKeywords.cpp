#include "GlslKeywords.hpp"

#include "GlslWriter.hpp"

using namespace castor;

namespace GLSL
{
	std::unique_ptr< KeywordsBase > KeywordsBase::get( GlslWriterConfig const & p_rs )
	{
		std::unique_ptr< KeywordsBase > result;

		switch ( p_rs.m_shaderLanguageVersion )
		{
		case 110:
			result = std::make_unique< Keywords< 110 > >();
			break;

		case 120:
			result = std::make_unique< Keywords< 120 > >();
			break;

		case 130:
			if ( p_rs.m_hasConstantsBuffers )
			{
				result = std::make_unique< Keywords< 140 > >();
			}
			else
			{
				result = std::make_unique< Keywords< 130 > >();
			}
			break;

		case 140:
			result = std::make_unique< Keywords< 140 > >();
			break;

		case 150:
			result = std::make_unique< Keywords< 150 > >();
			break;

		case 330:
			result = std::make_unique< Keywords< 330 > >();
			break;

		case 400:
			result = std::make_unique< Keywords< 400 > >();
			break;

		case 410:
			result = std::make_unique< Keywords< 410 > >();
			break;

		case 420:
			result = std::make_unique< Keywords< 420 > >();
			break;

		case 430:
			result = std::make_unique< Keywords< 430 > >();
			break;

		case 440:
			result = std::make_unique< Keywords< 440 > >();
			break;

		case 450:
			result = std::make_unique< Keywords< 450 > >();
			break;

		default:
			if ( p_rs.m_shaderLanguageVersion > 450 )
			{
				result = std::make_unique< Keywords< 450 > >();
			}
			else
			{
				result = std::make_unique< Keywords< 110 > >();
			}
			break;
		}

		return result;
	}

	String KeywordsBase::getAttribute( uint32_t p_index )const
	{
		return getLayout( p_index ) + m_strAttribute;
	}

	String KeywordsBase::getLayout( Ubo::Layout p_layout, uint32_t p_index )const
	{
		static std::map< Ubo::Layout, String > LayoutName
		{
			{ Ubo::Layout::eStd140, cuT( "std140" ) },
			{ Ubo::Layout::eShared, cuT( "shared" ) },
			{ Ubo::Layout::ePacked, cuT( "packed" ) },
		};

		String result;

		if ( !m_strUboLayout.empty() )
		{
			result = m_strUboLayout + cuT( "( " ) + LayoutName[p_layout];

			if ( !m_strUboBinding.empty() )
			{
				result += cuT( ", " ) + m_strUboBinding + cuT( " = " ) + string::toString( p_index );
			}

			result += cuT( " ) " );
		}

		return result;
	}

	String KeywordsBase::getLayout( Ssbo::Layout p_layout, uint32_t p_index )const
	{
		static std::map< Ssbo::Layout, String > LayoutName
		{
			{ Ssbo::Layout::eStd140, cuT( "std140" ) },
			{ Ssbo::Layout::eStd430, cuT( "std430" ) },
			{ Ssbo::Layout::eShared, cuT( "shared" ) },
			{ Ssbo::Layout::ePacked, cuT( "packed" ) },
		};

		String result;

		if ( !m_strSsboLayout.empty() )
		{
			result = m_strSsboLayout + cuT( "( " ) + LayoutName[p_layout];

			if ( !m_strSsboBinding.empty() )
			{
				result += cuT( ", " ) + m_strSsboBinding + cuT( " = " ) + string::toString( p_index );
			}

			result += cuT( " ) " );
		}

		return result;
	}
}
