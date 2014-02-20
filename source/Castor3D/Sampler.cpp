#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Sampler.hpp"

using namespace Castor3D;
using namespace Castor;

//*********************************************************************************************

SamplerRenderer :: SamplerRenderer( RenderSystem * p_pRenderSystem )
	:	Renderer< Sampler, SamplerRenderer >	( p_pRenderSystem	)
{
}

SamplerRenderer :: ~SamplerRenderer()
{
}

//*********************************************************************************************

Sampler :: Sampler( Engine * p_pEngine )
	:	Renderable< Sampler, SamplerRenderer >	( p_pEngine								)
	,	m_rMinLod								( -1000									)
	,	m_rMaxLod								( 1000									)
	,	m_clrBorderColour						( Colour::from_components( 0, 0, 0, 0 )	)
	,	m_rMaxAnisotropy						( 1.0									)
{
	m_eWrapModes[eTEXTURE_UVW_U] = eWRAP_MODE_REPEAT;
	m_eWrapModes[eTEXTURE_UVW_V] = eWRAP_MODE_REPEAT;
	m_eWrapModes[eTEXTURE_UVW_W] = eWRAP_MODE_REPEAT;

	m_eInterpolationModes[eINTERPOLATION_FILTER_MIN] = eINTERPOLATION_MODE_NEAREST;
	m_eInterpolationModes[eINTERPOLATION_FILTER_MAG] = eINTERPOLATION_MODE_NEAREST;
	m_eInterpolationModes[eINTERPOLATION_FILTER_MIP] = eINTERPOLATION_MODE_UNDEFINED;
}

Sampler :: ~Sampler()
{
}

bool Sampler :: Initialise()
{
	return GetRenderer()->Initialise();
}

void Sampler :: Cleanup()
{
	GetRenderer()->Cleanup();
}

bool Sampler :: Bind( eTEXTURE_DIMENSION p_eDimension, uint32_t p_uiIndex )
{
	return GetRenderer()->Bind( p_eDimension, p_uiIndex );
}

void Sampler :: Unbind()
{
	GetRenderer()->Unbind();
}

//*********************************************************************************************
