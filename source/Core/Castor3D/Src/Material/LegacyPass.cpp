#include "LegacyPass.hpp"

#include "Engine.hpp"
#include "Cache/CameraCache.hpp"
#include "Cache/MaterialCache.hpp"
#include "Cache/ShaderCache.hpp"

#include "Render/RenderPipeline.hpp"
#include "Render/RenderNode.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/BlendState.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Log/Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	//*********************************************************************************************

	LegacyPass::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< LegacyPass >{ p_tabs }
	{
	}

	bool LegacyPass::TextWriter::operator()( LegacyPass const & p_pass, TextFile & p_file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing LegacyPass " ) );
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tambient " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_pass.GetAmbient(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< LegacyPass >::CheckError( l_return, "LegacyPass ambient colour" );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tdiffuse " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_pass.GetDiffuse(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< LegacyPass >::CheckError( l_return, "LegacyPass diffuse colour" );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\temissive " ), m_tabs.c_str() ) > 0
					   && HdrColour::TextWriter( String() )( p_pass.GetEmissive(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< LegacyPass >::CheckError( l_return, "LegacyPass emissive colour" );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tspecular " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_pass.GetSpecular(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< LegacyPass >::CheckError( l_return, "LegacyPass specular colour" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tshininess " ) + string::to_string( p_pass.GetShininess() ) + cuT( "\n" ) ) > 0;
			Castor::TextWriter< LegacyPass >::CheckError( l_return, "LegacyPass shininess" );
		}

		if ( l_return )
		{
			l_return = Castor::TextWriter< Pass >{ m_tabs }( p_pass, p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*********************************************************************************************

	LegacyPass::LegacyPass( Material & p_parent )
		: Pass{ p_parent }
		, m_shininess{ 50.0 }
		, m_diffuse{ Colour::from_rgba( 0xFFFFFFFF ) }
		, m_ambient{ Colour::from_rgba( 0x000000FF ) }
		, m_specular{ Colour::from_rgba( 0xFFFFFFFF ) }
		, m_emissive{ HdrColour::from_rgba( 0x000000FF ) }
	{
	}

	LegacyPass::~LegacyPass()
	{
	}

	void LegacyPass::DoInitialise()
	{
	}

	void LegacyPass::DoCleanup()
	{
	}

	void LegacyPass::DoUpdateRenderNode( PassRenderNode & p_node )const
	{
		p_node.m_ambient.SetValue( rgba_float( GetAmbient() ) );
		p_node.m_diffuse.SetValue( rgba_float( GetDiffuse() ) );
		p_node.m_specular.SetValue( rgba_float( GetSpecular() ) );
		p_node.m_emissive.SetValue( rgba_float( GetEmissive() ) );
		p_node.m_shininess.SetValue( GetShininess() );
	}

	void LegacyPass::DoSetOpacity( float p_value )
	{
		m_diffuse.alpha() = p_value;
		m_ambient.alpha() = p_value;
		m_specular.alpha() = p_value;
		m_emissive.alpha() = p_value;
	}
}
