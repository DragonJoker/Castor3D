#include "Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	MetallicRoughnessPbrPass::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< MetallicRoughnessPbrPass >{ tabs }
	{
	}

	bool MetallicRoughnessPbrPass::TextWriter::operator()( MetallicRoughnessPbrPass const & pass, TextFile & file )
	{
		log::info << m_tabs << cuT( "Writing MetallicRoughnessPbrPass " ) << std::endl;
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		
		if ( result )
		{
			result = file.print( 256, cuT( "%s\talbedo " ), m_tabs.c_str() ) > 0
				&& RgbColour::TextWriter( String() )( pass.getAlbedo(), file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< MetallicRoughnessPbrPass >::checkError( result, "MetallicRoughnessPbrPass albedo" );
		}

		if ( result && pass.getRoughness() != 1 )
		{
			result = file.writeText( m_tabs + cuT( "\troughness " )
				+ string::toString( pass.getRoughness(), std::locale{ "C" } )
				+ cuT( "\n" ) ) > 0;
			castor::TextWriter< MetallicRoughnessPbrPass >::checkError( result, "MetallicRoughnessPbrPass roughness" );
		}

		if ( result && pass.getMetallic() != 0 )
		{
			result = file.writeText( m_tabs + cuT( "\tmetallic " )
				+ string::toString( pass.getMetallic(), std::locale{ "C" } )
				+ cuT( "\n" ) ) > 0;
			castor::TextWriter< MetallicRoughnessPbrPass >::checkError( result, "MetallicRoughnessPbrPass reflectance" );
		}

		if ( result )
		{
			result = Pass::TextWriter{ m_tabs }( pass, file );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*********************************************************************************************

	MetallicRoughnessPbrPass::MetallicRoughnessPbrPass( Material & parent )
		: Pass{ parent }
		, m_albedo{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
	{
	}

	MetallicRoughnessPbrPass::~MetallicRoughnessPbrPass()
	{
	}

	void MetallicRoughnessPbrPass::accept( PassBuffer & buffer )const
	{
		buffer.visit( *this );
	}

	void MetallicRoughnessPbrPass::doInitialise()
	{
	}

	void MetallicRoughnessPbrPass::doCleanup()
	{
	}

	void MetallicRoughnessPbrPass::doSetOpacity( float value )
	{
	}
}
