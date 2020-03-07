#include "Castor3D/Material/Pass/PhongPass.hpp"

#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	PhongPass::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< PhongPass >{ p_tabs }
	{
	}

	bool PhongPass::TextWriter::operator()( PhongPass const & p_pass, TextFile & p_file )
	{
		log::info << m_tabs << cuT( "Writing PhongPass " ) << std::endl;
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tdiffuse " ), m_tabs.c_str() ) > 0
				&& RgbColour::TextWriter( String() )( p_pass.getDiffuse(), p_file )
				&& p_file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< PhongPass >::checkError( result, "PhongPass diffuse colour" );
		}

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tspecular " ), m_tabs.c_str() ) > 0
				&& RgbColour::TextWriter( String() )( p_pass.getSpecular(), p_file )
				&& p_file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< PhongPass >::checkError( result, "PhongPass specular colour" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tambient " ) + string::toString( p_pass.getAmbient() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< PhongPass >::checkError( result, "PhongPass ambient" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tshininess " ) + string::toString( p_pass.getShininess() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< PhongPass >::checkError( result, "PhongPass shininess" );
		}

		if ( result )
		{
			result = Pass::TextWriter{ m_tabs }( p_pass, p_file );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*********************************************************************************************

	PhongPass::PhongPass( Material & p_parent )
		: Pass{ p_parent }
		, m_diffuse{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_specular{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
	{
	}

	PhongPass::~PhongPass()
	{
	}

	void PhongPass::accept( PassBuffer & p_buffer )const
	{
		p_buffer.visit( *this );
	}

	void PhongPass::doInitialise()
	{
	}

	void PhongPass::doCleanup()
	{
	}

	void PhongPass::doSetOpacity( float p_value )
	{
	}
}
