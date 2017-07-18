#include "GlslKeywords.hpp"

#include "GlslWriter.hpp"

using namespace Castor;

namespace GLSL
{
	std::unique_ptr< KeywordsBase > KeywordsBase::Get( GlslWriterConfig const & p_rs )
	{
		std::unique_ptr< KeywordsBase > l_result;

		switch ( p_rs.m_shaderLanguageVersion )
		{
		case 110:
			l_result = std::make_unique< Keywords< 110 > >();
			break;

		case 120:
			l_result = std::make_unique< Keywords< 120 > >();
			break;

		case 130:
			if ( p_rs.m_hasConstantsBuffers )
			{
				l_result = std::make_unique< Keywords< 140 > >();
			}
			else
			{
				l_result = std::make_unique< Keywords< 130 > >();
			}
			break;

		case 140:
			l_result = std::make_unique< Keywords< 140 > >();
			break;

		case 150:
			l_result = std::make_unique< Keywords< 150 > >();
			break;

		case 330:
			l_result = std::make_unique< Keywords< 330 > >();
			break;

		case 400:
			l_result = std::make_unique< Keywords< 400 > >();
			break;

		case 410:
			l_result = std::make_unique< Keywords< 410 > >();
			break;

		case 420:
			l_result = std::make_unique< Keywords< 420 > >();
			break;

		case 430:
			l_result = std::make_unique< Keywords< 430 > >();
			break;

		case 440:
			l_result = std::make_unique< Keywords< 440 > >();
			break;

		case 450:
			l_result = std::make_unique< Keywords< 450 > >();
			break;

		default:
			if ( p_rs.m_shaderLanguageVersion > 450 )
			{
				l_result = std::make_unique< Keywords< 450 > >();
			}
			else
			{
				l_result = std::make_unique< Keywords< 110 > >();
			}
			break;
		}

		return l_result;
	}

	String KeywordsBase::GetAttribute( uint32_t p_index )const
	{
		return GetLayout( p_index ) + m_strAttribute;
	}

	String KeywordsBase::GetLayout( Ubo::Layout p_layout, uint32_t p_index )const
	{
		static std::map< Ubo::Layout, String > LayoutName
		{
			{ Ubo::Layout::eStd140, cuT( "std140" ) },
			{ Ubo::Layout::eShared, cuT( "shared" ) },
			{ Ubo::Layout::ePacked, cuT( "packed" ) },
		};

		String l_result;

		if ( !m_strUboLayout.empty() )
		{
			l_result = m_strUboLayout + cuT( "( " ) + LayoutName[p_layout];

			if ( !m_strUboBinding.empty() )
			{
				l_result += cuT( ", " ) + m_strUboBinding + cuT( " = " ) + string::to_string( p_index );
			}

			l_result += cuT( " ) " );
		}

		return l_result;
	}

	String KeywordsBase::GetLayout( Ssbo::Layout p_layout, uint32_t p_index )const
	{
		static std::map< Ssbo::Layout, String > LayoutName
		{
			{ Ssbo::Layout::eStd140, cuT( "std140" ) },
			{ Ssbo::Layout::eStd430, cuT( "std430" ) },
			{ Ssbo::Layout::eShared, cuT( "shared" ) },
			{ Ssbo::Layout::ePacked, cuT( "packed" ) },
		};

		String l_result;

		if ( !m_strSsboLayout.empty() )
		{
			l_result = m_strSsboLayout + cuT( "( " ) + LayoutName[p_layout];

			if ( !m_strSsboBinding.empty() )
			{
				l_result += cuT( ", " ) + m_strSsboBinding + cuT( " = " ) + string::to_string( p_index );
			}

			l_result += cuT( " ) " );
		}

		return l_result;
	}
}
