#include "GlSampler.hpp"

#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlSampler::GlSampler( OpenGl & p_gl, GlRenderSystem * p_renderSystem, Castor::String const & p_name )
		: Sampler( *p_renderSystem->GetEngine(), p_name )
		, ObjectType( p_gl,
					  "GlSampler",
					  std::bind( &OpenGl::GenSamplers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteSamplers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsSampler, std::ref( p_gl ), std::placeholders::_1 )
					)
	{
	}

	GlSampler::~GlSampler()
	{
	}

	bool GlSampler::Initialise()
	{
		bool l_return = IsValid();

		if ( !l_return )
		{
			l_return = ObjectType::Create();

			if ( l_return )
			{
				eGL_INTERPOLATION_MODE l_minMode = GetOpenGl().Get( GetInterpolationMode( InterpolationFilter::Min ) );
				eGL_INTERPOLATION_MODE l_mipMode = GetOpenGl().Get( GetInterpolationMode( InterpolationFilter::Mip ) );
				DoAdjustMinMipModes( l_minMode, l_mipMode );

				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_LODBIAS, float( GetLodBias() ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_UWRAP, GetOpenGl().Get( GetWrappingMode( TextureUVW::U ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_VWRAP, GetOpenGl().Get( GetWrappingMode( TextureUVW::V ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_WWRAP, GetOpenGl().Get( GetWrappingMode( TextureUVW::W ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_MINFILTER, l_minMode );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_MAGFILTER, GetOpenGl().Get( GetInterpolationMode( InterpolationFilter::Mag ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_BORDERCOLOUR, GetBorderColour().const_ptr() );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_MAXANISOTROPY, float( GetMaxAnisotropy() ) );
			}
		}

		return l_return;
	}

	void GlSampler::Cleanup()
	{
		ObjectType::Destroy();
	}

	bool GlSampler::Bind( uint32_t p_index )const
	{
		return GetOpenGl().BindSampler( p_index, GetGlName() );
	}

	void GlSampler::Unbind( uint32_t p_index )const
	{
	}

	void GlSampler::DoAdjustMinMipModes( eGL_INTERPOLATION_MODE & p_min, eGL_INTERPOLATION_MODE & p_mip )
	{
		if ( p_mip != eGL_INTERPOLATION_MODE_NEAREST )
		{
			if ( p_min == eGL_INTERPOLATION_MODE_LINEAR )
			{
				if ( p_mip == eGL_INTERPOLATION_MODE_LINEAR )
				{
					p_min = eGL_INTERPOLATION_MODE_LINEAR_MIPMAP_LINEAR;
				}
				else
				{
					p_min = eGL_INTERPOLATION_MODE_LINEAR_MIPMAP_NEAREST;
				}
			}
			else
			{
				if ( p_mip == eGL_INTERPOLATION_MODE_LINEAR )
				{
					p_min = eGL_INTERPOLATION_MODE_NEAREST_MIPMAP_LINEAR;
				}
				else
				{
					p_min = eGL_INTERPOLATION_MODE_NEAREST_MIPMAP_NEAREST;
				}
			}
		}
	}
}
