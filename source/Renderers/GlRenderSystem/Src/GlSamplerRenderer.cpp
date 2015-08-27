#include "GlSamplerRenderer.hpp"
#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlSamplerRenderer::GlSamplerRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
		:	SamplerRenderer( p_pRenderSystem )
		,	m_uiSamplerId( eGL_INVALID_INDEX )
		,	m_gl( p_gl )
	{
	}

	GlSamplerRenderer::~GlSamplerRenderer()
	{
	}

	bool GlSamplerRenderer::Initialise()
	{
		if ( !m_pfnBind )
		{
			if ( m_gl.HasSpl() && m_uiSamplerId == eGL_INVALID_INDEX )
			{
				m_gl.GenSamplers( 1, &m_uiSamplerId );
			}

			if ( m_uiSamplerId != eGL_INVALID_INDEX )
			{
				eGL_INTERPOLATION_MODE l_eMinMode = m_gl.Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIN	) );

				if ( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIP ) != eINTERPOLATION_MODE_UNDEFINED )
				{
					eGL_INTERPOLATION_MODE l_eMipMode = m_gl.Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIP	) );

					if ( l_eMinMode == eGL_INTERPOLATION_MODE_LINEAR )
					{
						if ( l_eMipMode == eGL_INTERPOLATION_MODE_LINEAR )
						{
							l_eMinMode = eGL_INTERPOLATION_MODE_LINEAR_MIPMAP_LINEAR;
						}
						else
						{
							l_eMinMode = eGL_INTERPOLATION_MODE_LINEAR_MIPMAP_NEAREST;
						}
					}
					else
					{
						if ( l_eMipMode == eGL_INTERPOLATION_MODE_LINEAR )
						{
							l_eMinMode = eGL_INTERPOLATION_MODE_NEAREST_MIPMAP_LINEAR;
						}
						else
						{
							l_eMinMode = eGL_INTERPOLATION_MODE_NEAREST_MIPMAP_NEAREST;
						}
					}
				}

				if ( m_target->GetMinLod() != -1000 )
				{
					m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MINLOD,		float( m_target->GetMinLod() )	);
				}

				if ( m_target->GetMaxLod() != 1000 )
				{
					m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MAXLOD,		float( m_target->GetMaxLod() )	);
				}

				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_LODBIAS,			float( m_target->GetLodBias() )	);
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_UWRAP,			m_gl.Get( m_target->GetWrappingMode( eTEXTURE_UVW_U	) )	);
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_VWRAP,			m_gl.Get( m_target->GetWrappingMode( eTEXTURE_UVW_V	) )	);
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_WWRAP,			m_gl.Get( m_target->GetWrappingMode( eTEXTURE_UVW_W	) )	);
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MINFILTER,		l_eMinMode	);
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MAGFILTER,		m_gl.Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MAG	) )	);
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_BORDERCOLOUR,	m_target->GetBorderColour().const_ptr()	);
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MAXANISOTROPY,	float( m_target->GetMaxAnisotropy() )	);
				m_pfnBind = PBindFunction( [&]( eGL_TEXDIM CU_PARAM_UNUSED( p_eDimension ), uint32_t p_uiIndex )
				{
					return m_gl.BindSampler( p_uiIndex, m_uiSamplerId );
				} );
				m_pfnUnbind = PUnbindFunction( [&]() {} );
			}
			else
			{
				m_pfnBind = PBindFunction( [&]( eGL_TEXDIM p_eDimension, uint32_t CU_PARAM_UNUSED( p_uiIndex ) )
				{
					bool l_bReturn = true;
					eGL_INTERPOLATION_MODE l_eMinMode = m_gl.Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIN	) );

					if ( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIP ) != eINTERPOLATION_MODE_UNDEFINED )
					{
						eGL_INTERPOLATION_MODE l_eMipMode = m_gl.Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIP	) );

						if ( l_eMinMode == eGL_INTERPOLATION_MODE_LINEAR )
						{
							if ( l_eMipMode == eGL_INTERPOLATION_MODE_LINEAR )
							{
								l_eMinMode = eGL_INTERPOLATION_MODE_LINEAR_MIPMAP_LINEAR;
							}
							else
							{
								l_eMinMode = eGL_INTERPOLATION_MODE_LINEAR_MIPMAP_NEAREST;
							}
						}
						else
						{
							if ( l_eMipMode == eGL_INTERPOLATION_MODE_LINEAR )
							{
								l_eMinMode = eGL_INTERPOLATION_MODE_NEAREST_MIPMAP_LINEAR;
							}
							else
							{
								l_eMinMode = eGL_INTERPOLATION_MODE_NEAREST_MIPMAP_NEAREST;
							}
						}
					}

					l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MINLOD,			float( m_target->GetMinLod() )	);
					l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MAXLOD,			float( m_target->GetMaxLod() )	);
					l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_LODBIAS,		float( m_target->GetLodBias() )	);
					l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_WRAP_S,			m_gl.Get( m_target->GetWrappingMode( eTEXTURE_UVW_U	) )	);
					l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_WRAP_T,			m_gl.Get( m_target->GetWrappingMode( eTEXTURE_UVW_V	) )	);
					l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_WRAP_R,			m_gl.Get( m_target->GetWrappingMode( eTEXTURE_UVW_W	) )	);
					l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MIN_FILTER,		l_eMinMode	);
					l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MAG_FILTER,		m_gl.Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MAG	) )	);
//					l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_BORDERCOLOUR,	m_target->GetBorderColour().const_ptr()									);
					l_bReturn &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MAX_ANISOTROPY,	float( m_target->GetMaxAnisotropy() )	);
					return l_bReturn;
				} );
				m_pfnUnbind = PUnbindFunction( [&]() {} );
			}
		}

		return true;
	}

	void GlSamplerRenderer::Cleanup()
	{
		if ( m_uiSamplerId != eGL_INVALID_INDEX )
		{
			m_gl.DeleteSamplers( 1, &m_uiSamplerId );
			m_uiSamplerId = eGL_INVALID_INDEX;
		}

		m_pfnBind = nullptr;
		m_pfnUnbind = nullptr;
	}

	bool GlSamplerRenderer::Bind( eTEXTURE_DIMENSION p_eDimension, uint32_t p_uiIndex )
	{
		return m_pfnBind( m_gl.Get( p_eDimension ), p_uiIndex );
	}

	void GlSamplerRenderer::Unbind()
	{
		m_pfnUnbind();
	}
}
