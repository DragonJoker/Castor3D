#include "GlSampler.hpp"
#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlSampler::GlSampler( OpenGl & p_gl, GlRenderSystem * p_renderSystem, Castor::String const & p_name )
		: Sampler( *p_renderSystem->GetOwner(), p_name )
		, m_uiSamplerId( eGL_INVALID_INDEX )
		, m_gl( p_gl )
	{
	}

	GlSampler::~GlSampler()
	{
	}

	bool GlSampler::Initialise()
	{
		if ( !m_pfnBind )
		{
			if ( m_gl.HasSpl() && m_uiSamplerId == eGL_INVALID_INDEX )
			{
				m_gl.GenSamplers( 1, &m_uiSamplerId );
				glTrack( m_gl, GlSampler, this );
			}

			if ( m_uiSamplerId != eGL_INVALID_INDEX )
			{
				eGL_INTERPOLATION_MODE l_eMinMode = m_gl.Get( GetInterpolationMode( eINTERPOLATION_FILTER_MIN	) );

				if ( GetInterpolationMode( eINTERPOLATION_FILTER_MIP ) != eINTERPOLATION_MODE_UNDEFINED )
				{
					eGL_INTERPOLATION_MODE l_eMipMode = m_gl.Get( GetInterpolationMode( eINTERPOLATION_FILTER_MIP	) );

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

				if ( GetMinLod() != -1000 )
				{
					m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MINLOD, float( GetMinLod() ) );
				}

				if ( GetMaxLod() != 1000 )
				{
					m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MAXLOD, float( GetMaxLod() ) );
				}

				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_LODBIAS, float( GetLodBias() ) );
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_UWRAP, m_gl.Get( GetWrappingMode( eTEXTURE_UVW_U ) ) );
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_VWRAP, m_gl.Get( GetWrappingMode( eTEXTURE_UVW_V ) ) );
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_WWRAP, m_gl.Get( GetWrappingMode( eTEXTURE_UVW_W ) ) );
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MINFILTER, l_eMinMode );
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MAGFILTER, m_gl.Get( GetInterpolationMode( eINTERPOLATION_FILTER_MAG ) ) );
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_BORDERCOLOUR, GetBorderColour().const_ptr() );
				m_gl.SetSamplerParameter( m_uiSamplerId, eGL_SAMPLER_PARAMETER_MAXANISOTROPY, float( GetMaxAnisotropy() ) );
				m_pfnBind = PBindFunction( [this]( eGL_TEXDIM CU_PARAM_UNUSED( p_eDimension ), uint32_t p_index )
				{
					return m_gl.BindSampler( p_index, m_uiSamplerId );
				} );
				m_pfnUnbind = PUnbindFunction( [&]() {} );
			}
			else
			{
				m_pfnBind = PBindFunction( [this]( eGL_TEXDIM p_eDimension, uint32_t CU_PARAM_UNUSED( p_index ) )
				{
					bool l_return = true;
					eGL_INTERPOLATION_MODE l_eMinMode = m_gl.Get( GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) );

					if ( GetInterpolationMode( eINTERPOLATION_FILTER_MIP ) != eINTERPOLATION_MODE_UNDEFINED )
					{
						eGL_INTERPOLATION_MODE l_eMipMode = m_gl.Get( GetInterpolationMode( eINTERPOLATION_FILTER_MIP ) );

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

					l_return &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MINLOD, float( GetMinLod() ) );
					l_return &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MAXLOD, float( GetMaxLod() ) );
					l_return &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_LODBIAS, float( GetLodBias() ) );
					l_return &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_WRAP_S, m_gl.Get( GetWrappingMode( eTEXTURE_UVW_U ) ) );
					l_return &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_WRAP_T, m_gl.Get( GetWrappingMode( eTEXTURE_UVW_V ) ) );
					l_return &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_WRAP_R, m_gl.Get( GetWrappingMode( eTEXTURE_UVW_W ) ) );
					l_return &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MIN_FILTER, l_eMinMode );
					l_return &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MAG_FILTER, m_gl.Get( GetInterpolationMode( eINTERPOLATION_FILTER_MAG ) ) );
					//l_return &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_BORDERCOLOUR, GetBorderColour().const_ptr() );
					l_return &= m_gl.TexParameter( p_eDimension, eGL_TEXTURE_PARAMETER_MAX_ANISOTROPY,	float( GetMaxAnisotropy() ) );
					return l_return;
				} );
				m_pfnUnbind = PUnbindFunction( [&]() {} );
			}
		}

		return true;
	}

	void GlSampler::Cleanup()
	{
		if ( m_uiSamplerId != eGL_INVALID_INDEX )
		{
			glUntrack( m_gl, this );
			m_gl.DeleteSamplers( 1, &m_uiSamplerId );
			m_uiSamplerId = eGL_INVALID_INDEX;
		}

		m_pfnBind = nullptr;
		m_pfnUnbind = nullptr;
	}

	bool GlSampler::Bind( eTEXTURE_TYPE p_eDimension, uint32_t p_index )
	{
		return m_pfnBind( m_gl.Get( p_eDimension ), p_index );
	}

	void GlSampler::Unbind()
	{
		m_pfnUnbind();
	}
}
