#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Pass.hpp"
#include "Castor3D/TextureUnit.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/SceneFileParser.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/Material.hpp"

using namespace Castor3D;

//*********************************************************************************************

bool Loader<Pass> :: Load( Pass & p_pass, File & p_file)
{
	p_pass.Cleanup();
	bool l_bReturn = Loader<Colour>::Load( p_pass.GetAmbient(), p_file);

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Load( p_pass.GetDiffuse(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Load( p_pass.GetSpecular(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Load( p_pass.GetEmissive(), p_file);
	}

	if (l_bReturn)
	{
		real l_rTmp;
		l_bReturn = (p_file.Read( l_rTmp) == sizeof( real));
		p_pass.SetShininess( l_rTmp);
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Load( p_pass.GetTexBaseColour(), p_file);
	}

	if (l_bReturn)
	{
		bool l_bTmp;
		l_bReturn = (p_file.Read( l_bTmp) == sizeof( bool));
		p_pass.SetDoubleFace( l_bTmp);
	}

	if (l_bReturn)
	{
		size_t l_uiNbTextureUnits = 0;
		l_bReturn = (p_file.Read( l_uiNbTextureUnits) == sizeof( size_t));

		for (size_t i = 0 ; i < l_uiNbTextureUnits && l_bReturn ; i++)
		{
			TextureUnitPtr l_pTextureUnit = p_pass.AddTextureUnit();
			l_bReturn = Loader<TextureUnit>::Load( * l_pTextureUnit, p_file);

			if ( ! l_bReturn)
			{
				p_pass.DestroyTextureUnit( l_pTextureUnit->GetIndex());
			}
		}
	}

	return l_bReturn;
}

bool Loader<Pass> :: Save( const Pass & p_pass, File & p_file)
{
	bool l_bReturn = Loader<Colour>::Save( p_pass.GetAmbient(), p_file);

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Save( p_pass.GetDiffuse(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Save( p_pass.GetSpecular(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Save( p_pass.GetEmissive(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( p_pass.GetShininess()) == sizeof( real));
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Save( p_pass.GetTexBaseColour(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = (p_file.Write( p_pass.IsDoubleFace()) == sizeof( bool));
	}

	if (l_bReturn)
	{
		size_t l_uiNbTextureUnits = 0;
		l_bReturn = (p_file.Write( p_pass.GetNbTexUnits()) == sizeof( size_t));

		for (size_t i = 0 ; i < p_pass.GetNbTexUnits() && l_bReturn ; i++)
		{
			l_bReturn = Loader<TextureUnit>::Save( * p_pass.GetTextureUnit( i), p_file);
		}
	}

	return l_bReturn;
}

bool Loader<Pass> :: Write( const Pass & p_pass, File & p_file)
{
	bool l_bReturn = p_file.WriteLine( cuT( "\tpass\n\t{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\t\tambient ")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Write( p_pass.GetAmbient(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\n\t\tdiffuse ")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Write( p_pass.GetDiffuse(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\n\t\temissive ")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Write( p_pass.GetEmissive(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\n\t\tspecular ")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Write( p_pass.GetSpecular(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\n\t\tshininess %f\n"), p_pass.GetShininess()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\t\ttex_base ")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Loader<Colour>::Write( p_pass.GetTexBaseColour(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( cuT( "\n\t\tdouble_face ") + String( p_pass.IsDoubleFace() ? cuT( "true") : cuT( "false")) + cuT( "\n")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( cuT( "\t\tblend_func ") + Pass::StringSrcBlendFactors[p_pass.GetSrcBlendFactor()] + cuT( " ") + Pass::StringDstBlendFactors[p_pass.GetDstBlendFactor()] + cuT( "\n")) > 0;
	}

	if (l_bReturn)
	{
		size_t l_uiNbTextureUnits = p_pass.GetNbTexUnits();
		bool l_bFirst = true;

		for (size_t i = 0 ; i < l_uiNbTextureUnits && l_bReturn ; i++)
		{
			if (l_bFirst)
			{
				l_bFirst = false;
			}
			else
			{
				p_file.WriteLine( "\n");
			}

			l_bReturn = Loader<TextureUnit>::Write( * p_pass.GetTextureUnit( i), p_file);
		}
	}

	if (l_bReturn && p_pass.HasShader())
	{
		l_bReturn = Loader<ShaderProgramBase>::Write( * p_pass.GetShader<ShaderProgramBase>(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "\t}\n") > 0;
	}

	return l_bReturn;
}

//*********************************************************************************************

std::map<String, Pass::eSRC_BLEND> Pass::MapSrcBlendFactors;
std::map<String, Pass::eDST_BLEND> Pass::MapDstBlendFactors;
const String Pass::StringSrcBlendFactors[eSRC_BLEND_COUNT]	= { cuT( "none"), cuT( "zero"), cuT( "one"), cuT( "dst_colour"), cuT( "one_minus_dst_colour"), cuT( "src_alpha"), cuT( "one_minus_src_alpha"), cuT( "dst_alpha"), cuT( "one_minus_dst_alpha"), cuT( "src_alpha_saturate") };
const String Pass::StringDstBlendFactors[eDST_BLEND_COUNT]	= { cuT( "none"), cuT( "zero"), cuT( "one"), cuT( "src_colour"), cuT( "one_minus_src_colour"), cuT( "src_alpha"), cuT( "one_minus_src_alpha"), cuT( "dst_alpha"), cuT( "one_minus_dst_alpha") };

Pass :: Pass( Material * p_parent)
	:	m_fShininess		( 50.0)
	,	m_bDoubleFace		( false)
	,	m_pParent			( p_parent)
	,	m_clrDiffuse		( Colour::FromABGR( 0xFFFFFFFF))
	,	m_clrAmbient		( Colour::FromABGR( 0x000000FF))
	,	m_clrSpecular		( Colour::FromABGR( 0xFFFFFFFF))
	,	m_clrEmissive		( Colour::FromABGR( 0x000000FF))
	,	m_clrTexBaseColour	( Colour::FromABGR( 0x000000FF))
	,	m_fAlpha			( 1.0f)
	,	m_eSrcBlendFactor	( eSRC_BLEND_NONE)
	,	m_eDstBlendFactor	( eDST_BLEND_NONE)
{
	_initialiseMaps();
}

Pass :: Pass( const Pass & p_copy)
	:	m_fShininess		( p_copy.m_fShininess)
	,	m_bDoubleFace		( p_copy.m_bDoubleFace)
	,	m_pParent			( p_copy.m_pParent)
	,	m_clrDiffuse		( p_copy.m_clrDiffuse)
	,	m_clrAmbient		( p_copy.m_clrAmbient)
	,	m_clrSpecular		( p_copy.m_clrSpecular)
	,	m_clrEmissive		( p_copy.m_clrEmissive)
	,	m_clrTexBaseColour	( p_copy.m_clrTexBaseColour)
	,	m_fAlpha			( p_copy.m_fAlpha)
	,	m_eSrcBlendFactor	( p_copy.m_eSrcBlendFactor)
	,	m_eDstBlendFactor	( p_copy.m_eDstBlendFactor)
	,	m_pShaderProgram	( p_copy.m_pShaderProgram)
{
	for (size_t i = 0 ; i < p_copy.m_arrayTextureUnits.size() ; i++)
	{
		m_arrayTextureUnits[i] = p_copy.m_arrayTextureUnits[i];
	}
}

Pass & Pass :: operator =( const Pass & p_copy)
{
	m_fShininess		= p_copy.m_fShininess;
	m_bDoubleFace		= p_copy.m_bDoubleFace;
	m_pParent			= p_copy.m_pParent;
	m_clrDiffuse		= p_copy.m_clrDiffuse;
	m_clrAmbient		= p_copy.m_clrAmbient;
	m_clrSpecular		= p_copy.m_clrSpecular;
	m_clrEmissive		= p_copy.m_clrEmissive;
	m_clrTexBaseColour	= p_copy.m_clrTexBaseColour;
	m_fAlpha			= p_copy.m_fAlpha;
	m_eSrcBlendFactor	= p_copy.m_eSrcBlendFactor;
	m_eDstBlendFactor	= p_copy.m_eDstBlendFactor;
	m_pShaderProgram	= p_copy.m_pShaderProgram;

	for (size_t i = 0 ; i < p_copy.m_arrayTextureUnits.size() ; i++)
	{
		m_arrayTextureUnits[i] = p_copy.m_arrayTextureUnits[i];
	}

	return * this;
}

Pass :: ~Pass()
{
	m_arrayTextureUnits.clear();
}

void Pass :: Initialise()
{
	CASTOR_TRACK;
	for (size_t i = 0 ; i < m_arrayTextureUnits.size() ; i++)
	{
		m_arrayTextureUnits[i]->Initialise();
	}

	m_pRenderer->Initialise();
}

void Pass :: Cleanup()
{
	CASTOR_TRACK;
	for (size_t i = 0 ; i < m_arrayTextureUnits.size() ; i++)
	{
		m_arrayTextureUnits[i]->Cleanup();
	}

	m_arrayTextureUnits.clear();
}

void Pass :: Render( ePRIMITIVE_TYPE p_eDisplayMode)
{
	CASTOR_TRACK;
	if ( ! m_pShaderProgram.expired())
	{
		ShaderProgramPtr l_pShader = m_pShaderProgram.lock();
		l_pShader->Initialise();
		l_pShader->Begin();
	}

	m_pRenderer->Render( p_eDisplayMode);

	for (size_t i = 0 ; i < m_arrayTextureUnits.size() ; i++)
	{
		m_arrayTextureUnits[i]->Render( p_eDisplayMode);
	}
}

void Pass :: Render2D()
{
	CASTOR_TRACK;
	if ( ! m_pShaderProgram.expired())
	{
		ShaderProgramPtr l_pShader = m_pShaderProgram.lock();
		l_pShader->Initialise();
		l_pShader->Begin();
	}

	m_pRenderer->Render2D();

	for (size_t i = 0 ; i < m_arrayTextureUnits.size() ; i++)
	{
		m_arrayTextureUnits[i]->Render2D();
	}
}

void Pass :: EndRender()
{
	CASTOR_TRACK;
	for (size_t i = m_arrayTextureUnits.size() - 1 ; i < m_arrayTextureUnits.size() ; i--)
	{
		m_arrayTextureUnits[i]->EndRender();
	}

	m_pRenderer->EndRender();

	if ( ! m_pShaderProgram.expired())
	{
		ShaderProgramPtr l_pShader = m_pShaderProgram.lock();
		l_pShader->End();
	}
}

void Pass :: SetTexBaseColour( float p_fRed, float p_fGreen, float p_fBlue, float p_fAlphA)
{
	m_clrTexBaseColour = Colour::FromRGBA( Point4f( p_fRed, p_fGreen, p_fBlue, p_fAlphA));

	for (size_t i = 0 ; i < m_arrayTextureUnits.size() ; i++)
	{
		m_arrayTextureUnits[i]->SetPrimaryColour( m_clrTexBaseColour);
	}
}

void Pass :: SetTexBaseColour( Colour const & p_clrColour)
{
	m_clrTexBaseColour = p_clrColour;

	for (size_t i = 0 ; i < m_arrayTextureUnits.size() ; i++)
	{
		m_arrayTextureUnits[i]->SetPrimaryColour( m_clrTexBaseColour);
	}
}

TextureUnitPtr Pass :: AddTextureUnit()
{
	TextureUnitPtr l_pReturn( new TextureUnit);
	size_t l_uiID = m_arrayTextureUnits.size();
	l_pReturn->SetIndex( l_uiID);
	l_pReturn->SetPrimaryColour( m_clrTexBaseColour);
	m_arrayTextureUnits.push_back( l_pReturn);
	return l_pReturn;
}

bool Pass :: DestroyTextureUnit( size_t p_uiIndex)
{
	CASTOR_ASSERT( p_uiIndex < m_arrayTextureUnits.size());
	bool l_bReturn = false;
	Logger::LogMessage( cuT( "Destroying TextureUnit %d"), p_uiIndex);

	TextureUnitPtrArray::iterator l_it = m_arrayTextureUnits.begin();
	m_arrayTextureUnits.erase( l_it + p_uiIndex);
	size_t i = p_uiIndex;

	for (l_it = m_arrayTextureUnits.begin() + p_uiIndex ; l_it != m_arrayTextureUnits.end() ; ++l_it)
	{
		(* l_it)->SetIndex( i++);
	}

	l_bReturn = true;

	return l_bReturn;
}

TextureUnitPtr Pass :: GetTextureUnit( size_t p_uiIndex)const
{
	CASTOR_ASSERT( p_uiIndex < m_arrayTextureUnits.size());
	return m_arrayTextureUnits[p_uiIndex];
}

String Pass :: GetTexturePath( size_t p_uiIndex)
{
	CASTOR_ASSERT( p_uiIndex < m_arrayTextureUnits.size());
	return m_arrayTextureUnits[p_uiIndex]->GetTexturePath();
}

void Pass :: SetShader( ShaderProgramPtr p_pProgram)
{
	m_pShaderProgram = p_pProgram;
	m_pRenderer->Initialise();
}

bool Pass :: HasShader()const
{
	return ! m_pShaderProgram.expired();
}

void Pass :: _initialiseMaps()
{
	MapSrcBlendFactors.clear();
	MapSrcBlendFactors[cuT( "none")]					= eSRC_BLEND_NONE;
	MapSrcBlendFactors[cuT( "zero")]					= eSRC_BLEND_ZERO;
	MapSrcBlendFactors[cuT( "one")]						= eSRC_BLEND_ONE;
	MapSrcBlendFactors[cuT( "dst_colour")]				= eSRC_BLEND_DST_COLOUR;
	MapSrcBlendFactors[cuT( "one_minus_dst_colour")]	= eSRC_BLEND_ONE_MINUS_DST_COLOUR;
	MapSrcBlendFactors[cuT( "src_alpha")]				= eSRC_BLEND_SRC_ALPHA;
	MapSrcBlendFactors[cuT( "one_minus_src_alpha")]		= eSRC_BLEND_ONE_MINUS_SRC_ALPHA;
	MapSrcBlendFactors[cuT( "dst_alpha")]				= eSRC_BLEND_DST_ALPHA;
	MapSrcBlendFactors[cuT( "one_minus_dst_alpha")]		= eSRC_BLEND_ONE_MINUS_DST_ALPHA;
	MapSrcBlendFactors[cuT( "src_alpha_saturate")]		= eSRC_BLEND_SRC_ALPHA_SATURATE;

	MapDstBlendFactors.clear();
	MapDstBlendFactors[cuT( "none")]					= eDST_BLEND_NONE;
	MapDstBlendFactors[cuT( "zero")]					= eDST_BLEND_ZERO;
	MapDstBlendFactors[cuT( "one")]						= eDST_BLEND_ONE;
	MapDstBlendFactors[cuT( "src_colour")]				= eDST_BLEND_SRC_COLOUR; 
	MapDstBlendFactors[cuT( "one_minus_src_colour")]	= eDST_BLEND_ONE_MINUS_SRC_COLOUR; 
	MapDstBlendFactors[cuT( "src_alpha")]				= eDST_BLEND_SRC_ALPHA; 
	MapDstBlendFactors[cuT( "one_minus_src_alpha")]		= eDST_BLEND_ONE_MINUS_SRC_ALPHA; 
	MapDstBlendFactors[cuT( "dst_alpha")]				= eDST_BLEND_DST_ALPHA; 
	MapDstBlendFactors[cuT( "one_minus_dst_alpha")]		= eDST_BLEND_ONE_MINUS_DST_ALPHA;
}

//*********************************************************************************************