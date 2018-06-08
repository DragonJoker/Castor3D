#include "GlslKeywords.hpp"

#include "GlslWriter.hpp"

using namespace castor;

namespace glsl
{
	std::unique_ptr< KeywordsBase > KeywordsBase::get( GlslWriterConfig const & config )
	{
		std::unique_ptr< KeywordsBase > result;

		if ( config.m_hasPushConstants )
		{
			switch ( config.m_shaderLanguageVersion )
			{
			case 140:
				result = std::make_unique< Keywords< 140, true > >();
				break;

			case 150:
				result = std::make_unique< Keywords< 150, true > >();
				break;

			case 330:
				result = std::make_unique< Keywords< 330, true > >();
				break;

			case 400:
				result = std::make_unique< Keywords< 400, true > >();
				break;

			case 410:
				result = std::make_unique< Keywords< 410, true > >();
				break;

			case 420:
				result = std::make_unique< Keywords< 420, true > >();
				break;

			case 430:
				result = std::make_unique< Keywords< 430, true > >();
				break;

			case 440:
				result = std::make_unique< Keywords< 440, true > >();
				break;

			case 450:
				result = std::make_unique< Keywords< 450, true > >();
				break;

			default:
				if ( config.m_shaderLanguageVersion > 450 )
				{
					result = std::make_unique< Keywords< 450, true > >();
				}
				else
				{
					result = std::make_unique< Keywords< 140, true > >();
				}
				break;
			}
		}
		else
		{
			switch ( config.m_shaderLanguageVersion )
			{
			case 110:
				result = std::make_unique< Keywords< 110, false > >();
				break;

			case 120:
				result = std::make_unique< Keywords< 120, false > >();
				break;

			case 130:
				if ( config.m_hasConstantsBuffers )
				{
					result = std::make_unique< Keywords< 140, false > >();
				}
				else
				{
					result = std::make_unique< Keywords< 130, false > >();
				}
				break;

			case 140:
				result = std::make_unique< Keywords< 140, false > >();
				break;

			case 150:
				result = std::make_unique< Keywords< 150, false > >();
				break;

			case 330:
				result = std::make_unique< Keywords< 330, false > >();
				break;

			case 400:
				result = std::make_unique< Keywords< 400, false > >();
				break;

			case 410:
				result = std::make_unique< Keywords< 410, false > >();
				break;

			case 420:
				result = std::make_unique< Keywords< 420, false > >();
				break;

			case 430:
				result = std::make_unique< Keywords< 430, false > >();
				break;

			case 440:
				result = std::make_unique< Keywords< 440, false > >();
				break;

			case 450:
				result = std::make_unique< Keywords< 450, false > >();
				break;

			default:
				if ( config.m_shaderLanguageVersion > 450 )
				{
					result = std::make_unique< Keywords< 450, false > >();
				}
				else
				{
					result = std::make_unique< Keywords< 110, false > >();
				}
				break;
			}
		}

		return result;
	}

	String KeywordsBase::getAttribute( uint32_t index )const
	{
		return m_attribute;
	}

	String KeywordsBase::getUboLayout( Ubo::Layout layout, uint32_t index, uint32_t set )const
	{
		static std::map< Ubo::Layout, String > LayoutName
		{
			{ Ubo::Layout::eStd140, cuT( "std140" ) },
			{ Ubo::Layout::eShared, cuT( "shared" ) },
			{ Ubo::Layout::ePacked, cuT( "packed" ) },
		};

		StringStream result;
		result.imbue( Expr::getLocale() );

		if ( !m_uboLayout.empty() )
		{
			result << m_uboLayout << cuT( "( " ) << LayoutName[layout];

			if ( !m_uboBinding.empty() )
			{
				result << cuT( ", " ) << m_uboBinding << cuT( " = " ) << index;
			}

			if ( !m_uboSet.empty() )
			{
				result << cuT( ", " ) << m_uboSet << cuT( " = " ) << set;
			}

			result << cuT( " ) " );
		}

		return result.str();
	}

	String KeywordsBase::getSsboLayout( Ssbo::Layout layout, uint32_t index, uint32_t set )const
	{
		static std::map< Ssbo::Layout, String > LayoutName
		{
			{ Ssbo::Layout::eStd140, cuT( "std140" ) },
			{ Ssbo::Layout::eStd430, cuT( "std430" ) },
			{ Ssbo::Layout::eShared, cuT( "shared" ) },
			{ Ssbo::Layout::ePacked, cuT( "packed" ) },
		};

		StringStream result;
		result.imbue( Expr::getLocale() );

		if ( !m_ssboLayout.empty() )
		{
			result << m_ssboLayout << cuT( "( " ) << LayoutName[layout];

			if ( !m_ssboBinding.empty() )
			{
				result << cuT( ", " ) << m_ssboBinding << cuT( " = " ) << index;
			}

			if ( !m_ssboSet.empty() )
			{
				result << cuT( ", " ) << m_ssboSet << cuT( " = " ) << set;
			}

			result << cuT( " ) " );
		}

		return result.str();
	}

	String KeywordsBase::getTextureLayout( uint32_t index, uint32_t set )const
	{
		StringStream result;
		result.imbue( Expr::getLocale() );

		if ( !m_textureLayout.empty() )
		{
			REQUIRE( !m_textureBinding.empty() );
			result << m_textureLayout;
			result << cuT( "( " ) << m_textureBinding << cuT( " = " ) << index;

			if ( !m_textureSet.empty() )
			{
				result << cuT( ", " ) << m_textureSet << cuT( " = " ) << set;
			}

			result << cuT( " ) " );
		}

		return result.str();
	}

	String KeywordsBase::getUniformLayout( uint32_t location )const
	{
		StringStream result;
		result.imbue( Expr::getLocale() );

		if ( !m_uniformLayout.empty() )
		{
			result << m_uniformLayout << cuT( "( location = " ) << location << cuT( " ) " );
		}

		return result.str();
	}

	String KeywordsBase::getInputLayout( uint32_t location )const
	{
		StringStream result;
		result.imbue( Expr::getLocale() );

		if ( !m_inputLayout.empty() )
		{
			result << m_inputLayout << cuT( "( location = " ) << location << cuT( " ) " );
		}

		return result.str();
	}

	String KeywordsBase::getOutputLayout( uint32_t location )const
	{
		StringStream result;
		result.imbue( Expr::getLocale() );

		if ( !m_outputLayout.empty() )
		{
			result << m_outputLayout << cuT( "( location = " ) << location << cuT( " ) " );
		}

		return result.str();
	}
}
