#include "Texture/GlSampler.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

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
				GlInterpolationMode l_minMode = GetOpenGl().Get( GetInterpolationMode( InterpolationFilter::Min ) );
				GlInterpolationMode l_mipMode = GetOpenGl().Get( GetInterpolationMode( InterpolationFilter::Mip ) );
				DoAdjustMinMipModes( l_minMode, l_mipMode );

				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_LODBIAS, float( GetLodBias() ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_UWRAP, int( GetOpenGl().Get( GetWrappingMode( TextureUVW::U ) ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_VWRAP, int( GetOpenGl().Get( GetWrappingMode( TextureUVW::V ) ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_WWRAP, int( GetOpenGl().Get( GetWrappingMode( TextureUVW::W ) ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_MINFILTER, int( l_minMode ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_MAGFILTER, int( GetOpenGl().Get( GetInterpolationMode( InterpolationFilter::Mag ) ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_BORDERCOLOUR, GetBorderColour().const_ptr() );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_MAXANISOTROPY, float( GetMaxAnisotropy() ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_COMPAREMODE, int( GetOpenGl().Get( GetComparisonMode() ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_COMPAREFUNC, int( GetOpenGl().Get( GetComparisonFunc() ) ) );
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

	void GlSampler::DoAdjustMinMipModes( GlInterpolationMode & p_min, GlInterpolationMode & p_mip )
	{
		if ( p_mip != GlInterpolationMode::Nearest )
		{
			if ( p_min == GlInterpolationMode::Linear )
			{
				if ( p_mip == GlInterpolationMode::Linear )
				{
					p_min = GlInterpolationMode::LinearMipmapLinear;
				}
				else
				{
					p_min = GlInterpolationMode::LinearMipmapNearest;
				}
			}
			else
			{
				if ( p_mip == GlInterpolationMode::Linear )
				{
					p_min = GlInterpolationMode::NearestMipmapLinear;
				}
				else
				{
					p_min = GlInterpolationMode::NearestMipmapNearest;
				}
			}
		}
	}
}
