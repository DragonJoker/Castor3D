#include <Engine.hpp>
#include <RenderSystem.hpp>

#include <Logger.hpp>

#include "CastorGuiPrerequisites.hpp"

using namespace Castor3D;
using namespace Castor;

static const Castor::String NAME = cuT( "CastorGui" );

namespace
{
	void ParseError( String const & p_strError )
	{
		StringStream l_strError;
		l_strError << cuT( "Error, : " ) << p_strError;
		Logger::LogError( l_strError.str() );
	}

	void AddParser( FileParser::AttributeParsersBySection & p_parsers, uint32_t p_section, String const & p_name, PParserFunction p_function, uint32_t p_count, ... )
	{
		auto && l_sectionIt = p_parsers.find( p_section );

		if ( l_sectionIt != p_parsers.end() && l_sectionIt->second.find( p_name ) != l_sectionIt->second.end() )
		{
			ParseError( cuT( "Parser " ) + p_name + cuT( " for section " ) + string::to_string( p_section ) + cuT( " already exists." ) );
		}
		else
		{
			ParserParameterArray l_params;
			va_list l_valist;
			va_start( l_valist, p_count );

			for ( uint32_t i = 0; i < p_count; ++i )
			{
				ePARAMETER_TYPE l_eParamType =  ePARAMETER_TYPE( va_arg( l_valist, int ) );

				switch ( l_eParamType )
				{
				case ePARAMETER_TYPE_NAME:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_NAME > >( p_name ) );
					break;

				case ePARAMETER_TYPE_TEXT:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_TEXT > >( p_name ) );
					break;

				case ePARAMETER_TYPE_PATH:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_PATH > >( p_name ) );
					break;

				case ePARAMETER_TYPE_CHECKED_TEXT:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT > >( p_name, *reinterpret_cast< UIntStrMap * >( va_arg( l_valist, void * ) ) ) );
					break;

				case ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT > >( p_name, *reinterpret_cast< UIntStrMap * >( va_arg( l_valist, void * ) ) ) );
					break;

				case ePARAMETER_TYPE_BOOL:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_BOOL > >( p_name ) );
					break;

				case ePARAMETER_TYPE_INT8:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT8 > >( p_name ) );
					break;

				case ePARAMETER_TYPE_INT16:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT16 > >( p_name ) );
					break;

				case ePARAMETER_TYPE_INT32:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT32 > >( p_name ) );
					break;

				case ePARAMETER_TYPE_INT64:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT64 > >( p_name ) );
					break;

				case ePARAMETER_TYPE_UINT8:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT8 > >( p_name ) );
					break;

				case ePARAMETER_TYPE_UINT16:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT16 > >( p_name ) );
					break;

				case ePARAMETER_TYPE_UINT32:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT32 > >( p_name ) );
					break;

				case ePARAMETER_TYPE_UINT64:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT64 > >( p_name ) );
					break;

				case ePARAMETER_TYPE_FLOAT:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_FLOAT > >( p_name ) );
					break;

				case ePARAMETER_TYPE_DOUBLE:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_DOUBLE > >( p_name ) );
					break;

				case ePARAMETER_TYPE_LONGDOUBLE:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_LONGDOUBLE > >( p_name ) );
					break;

				case ePARAMETER_TYPE_PIXELFORMAT:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_PIXELFORMAT > >( p_name ) );
					break;

				case ePARAMETER_TYPE_POINT2I:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT2I > >( p_name ) );
					break;

				case ePARAMETER_TYPE_POINT3I:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT3I > >( p_name ) );
					break;

				case ePARAMETER_TYPE_POINT4I:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT4I > >( p_name ) );
					break;

				case ePARAMETER_TYPE_POINT2F:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT2F > >( p_name ) );
					break;

				case ePARAMETER_TYPE_POINT3F:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT3F > >( p_name ) );
					break;

				case ePARAMETER_TYPE_POINT4F:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT4F > >( p_name ) );
					break;

				case ePARAMETER_TYPE_POINT2D:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT2D > >( p_name ) );
					break;

				case ePARAMETER_TYPE_POINT3D:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT3D > >( p_name ) );
					break;

				case ePARAMETER_TYPE_POINT4D:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT4D > >( p_name ) );
					break;

				case ePARAMETER_TYPE_SIZE:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_SIZE > >( p_name ) );
					break;

				case ePARAMETER_TYPE_POSITION:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POSITION > >( p_name ) );
					break;

				case ePARAMETER_TYPE_RECTANGLE:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_RECTANGLE > >( p_name ) );
					break;

				case ePARAMETER_TYPE_COLOUR:
					l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_COLOUR > >( p_name ) );
					break;
				}
			}

			va_end( l_valist );
			p_parsers[p_section][p_name] = std::make_pair( p_function, l_params );
		}
	}
}

C3D_CGui_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_CGui_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_GENERIC;
}

C3D_CGui_API String GetName()
{
	return cuT( "Castor GUI" );
}

C3D_CGui_API void OnLoad( Castor3D::Engine * p_engine )
{
	FileParser::AttributeParsersBySection l_parsers;
	p_engine->RegisterParsers( NAME, std::move( l_parsers ) );
}

C3D_CGui_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->UnregisterParsers( NAME );
}
