#include "GlTextureRenderer.hpp"
#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTextureRenderer::GlTextureRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
		:	TextureRenderer( p_pRenderSystem )
		,	m_gl( p_gl )
#if C3DGL_USE_TBO
		,	m_uiGlName( eGL_INVALID_INDEX )
#endif
	{
	}

	GlTextureRenderer::~GlTextureRenderer()
	{
	}

	bool GlTextureRenderer::Render()
	{
		if ( !m_pfnRender )
		{
#if C3DGL_USE_TBO

			if ( m_gl.HasTbo() )
			{
				m_pfnRender		= &GlTextureRenderer::DoRenderTbo;
				m_pfnEndRender	= &GlTextureRenderer::DoEndRenderTbo;
			}
			else
#endif
				if ( static_cast< GlRenderSystem * >( m_pRenderSystem )->GetOpenGlMajor() >= 3 )
				{
					m_pfnRender = BoolFunction( [&]()
					{
						return true;
					} );
					m_pfnEndRender = VoidFunction( [&]() {} );
				}
				else
				{
					m_pfnRender = BoolFunction( [&]()
					{
						if ( m_pRenderSystem->UseMultiTexturing() )
						{
							if ( m_target->GetRgbFunction() != eRGB_BLEND_FUNC_NONE || m_target->GetAlpFunction() != eALPHA_BLEND_FUNC_NONE )
							{
								m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_TEXTURE_ENV_MODE, eGL_TEXENV_PARAM_COMBINE );

								if ( m_target->GetRgbFunction() != eRGB_BLEND_FUNC_NONE )
								{
									m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_COMBINE_RGB, m_gl.Get( m_target->GetRgbFunction() ) );
									m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_SOURCE0_RGB, m_gl.Get( m_target->GetRgbArgument( eBLEND_SRC_INDEX_0 ) ) );
									m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_OPERAND0_RGB, eGL_BLEND_FACTOR_SRC_COLOR );

									if ( m_target->GetRgbFunction() != eRGB_BLEND_FUNC_FIRST_ARG )
									{
										m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_SOURCE1_RGB, m_gl.Get( m_target->GetRgbArgument( eBLEND_SRC_INDEX_1 ) ) );
										m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_OPERAND1_RGB, eGL_BLEND_FACTOR_SRC_COLOR );
									}

									if ( m_target->GetRgbFunction() == eRGB_BLEND_FUNC_INTERPOLATE )
									{
										m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_SOURCE2_RGB, m_gl.Get( m_target->GetRgbArgument( eBLEND_SRC_INDEX_2 ) ) );
										m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_OPERAND2_RGB, eGL_BLEND_FACTOR_SRC_COLOR );
									}
								}

								if ( m_target->GetAlpFunction() != eALPHA_BLEND_FUNC_NONE )
								{
									m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_COMBINE_ALPHA, m_gl.Get( m_target->GetAlpFunction() ) );
									m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_SOURCE0_ALPHA, m_gl.Get( m_target->GetAlpArgument( eBLEND_SRC_INDEX_0 ) ) );
									m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_OPERAND0_ALPHA, eGL_BLEND_FACTOR_SRC_ALPHA );

									if ( m_target->GetAlpFunction() != eALPHA_BLEND_FUNC_FIRST_ARG )
									{
										m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_SOURCE1_ALPHA, m_gl.Get( m_target->GetAlpArgument( eBLEND_SRC_INDEX_1 ) ) );
										m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_OPERAND1_ALPHA, eGL_BLEND_FACTOR_SRC_ALPHA );
									}

									if ( m_target->GetAlpFunction() == eALPHA_BLEND_FUNC_INTERPOLATE )
									{
										m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_SOURCE2_ALPHA, m_gl.Get( m_target->GetAlpArgument( eBLEND_SRC_INDEX_2 ) ) );
										m_gl.TexEnvi( eGL_TEXENV_TARGET_TEXTURE_ENV, eGL_TEXENV_ARGNAME_OPERAND2_ALPHA, eGL_BLEND_FACTOR_SRC_ALPHA );
									}
								}
							}
						}

						m_gl.Enable( m_gl.Get( m_target->GetDimension() ) );

						if ( m_target->GetMappingMode() != eTEXTURE_MAP_MODE_NONE )
						{
							if ( m_target->GetMappingMode() == eTEXTURE_MAP_MODE_REFLECTION )
							{
								m_gl.TexGeni( eGL_TEXGEN_COORD_S, eGL_TEXGEN_PARAM_TEXTURE_GEN_MODE, eGL_TEXGEN_MODE_REFLECTION_MAP );
								m_gl.TexGeni( eGL_TEXGEN_COORD_T, eGL_TEXGEN_PARAM_TEXTURE_GEN_MODE, eGL_TEXGEN_MODE_REFLECTION_MAP );
							}
							else if ( m_target->GetMappingMode() == eTEXTURE_MAP_MODE_SPHERE )
							{
								m_gl.TexGeni( eGL_TEXGEN_COORD_S, eGL_TEXGEN_PARAM_TEXTURE_GEN_MODE, eGL_TEXGEN_MODE_SPHERE_MAP );
								m_gl.TexGeni( eGL_TEXGEN_COORD_T, eGL_TEXGEN_PARAM_TEXTURE_GEN_MODE, eGL_TEXGEN_MODE_SPHERE_MAP );
							}

							m_gl.Enable( eGL_TWEAK_TEXTURE_GEN_S );
							m_gl.Enable( eGL_TWEAK_TEXTURE_GEN_T );
						}

						if ( m_target->GetAlphaFunc() != eALPHA_FUNC_ALWAYS )
						{
							m_gl.Enable( eGL_TWEAK_ALPHA_TEST );
							m_gl.AlphaFunc( m_gl.Get( m_target->GetAlphaFunc() ), m_target->GetAlphaValue() );
						}

						return true;
					} );
					m_pfnEndRender = VoidFunction( [&]()
					{
						if ( m_target->GetAlphaFunc() != eALPHA_FUNC_ALWAYS )
						{
							m_gl.Disable( eGL_TWEAK_ALPHA_TEST );
						}

						if ( m_target->GetMappingMode() != eTEXTURE_MAP_MODE_NONE )
						{
							m_gl.Disable( eGL_TWEAK_TEXTURE_GEN_S );
							m_gl.Disable( eGL_TWEAK_TEXTURE_GEN_T );
						}
					} );
				}
		}

		return m_pfnRender();
	}

	void GlTextureRenderer::EndRender()
	{
		m_pfnEndRender();
	}
}
