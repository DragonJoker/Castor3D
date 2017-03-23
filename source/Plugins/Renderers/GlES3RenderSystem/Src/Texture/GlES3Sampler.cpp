#include "Texture/GlES3Sampler.hpp"

#include "Common/OpenGlES3.hpp"
#include "Render/GlES3RenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3Sampler::GlES3Sampler( OpenGlES3 & p_gl, GlES3RenderSystem * p_renderSystem, Castor::String const & p_name )
		: Sampler( *p_renderSystem->GetEngine(), p_name )
		, ObjectType( p_gl,
					  "GlES3Sampler",
					  std::bind( &OpenGlES3::GenSamplers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGlES3::DeleteSamplers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGlES3::IsSampler, std::ref( p_gl ), std::placeholders::_1 )
					)
	{
	}

	GlES3Sampler::~GlES3Sampler()
	{
	}

	bool GlES3Sampler::Initialise()
	{
		bool l_return = IsValid();

		if ( !l_return )
		{
			l_return = ObjectType::Create();

			if ( l_return )
			{
				GlES3InterpolationMode l_minMode = GetOpenGlES3().Get( GetInterpolationMode( InterpolationFilter::eMin ) );
				GlES3InterpolationMode l_mipMode = GetOpenGlES3().Get( GetInterpolationMode( InterpolationFilter::eMip ) );
				DoAdjustMinMipModes( l_minMode, l_mipMode );

				GetOpenGlES3().SetSamplerParameter( GetGlES3Name(), GlES3SamplerParameter::eLODBias, float( GetLodBias() ) );
				GetOpenGlES3().SetSamplerParameter( GetGlES3Name(), GlES3SamplerParameter::eUWrap, int( GetOpenGlES3().Get( GetWrappingMode( TextureUVW::eU ) ) ) );
				GetOpenGlES3().SetSamplerParameter( GetGlES3Name(), GlES3SamplerParameter::eVWrap, int( GetOpenGlES3().Get( GetWrappingMode( TextureUVW::eV ) ) ) );
				GetOpenGlES3().SetSamplerParameter( GetGlES3Name(), GlES3SamplerParameter::eWWrap, int( GetOpenGlES3().Get( GetWrappingMode( TextureUVW::eW ) ) ) );
				GetOpenGlES3().SetSamplerParameter( GetGlES3Name(), GlES3SamplerParameter::eMinFilter, int( l_minMode ) );
				GetOpenGlES3().SetSamplerParameter( GetGlES3Name(), GlES3SamplerParameter::eMagFilter, int( GetOpenGlES3().Get( GetInterpolationMode( InterpolationFilter::eMag ) ) ) );
				GetOpenGlES3().SetSamplerParameter( GetGlES3Name(), GlES3SamplerParameter::eBorderColour, GetBorderColour().const_ptr() );
				GetOpenGlES3().SetSamplerParameter( GetGlES3Name(), GlES3SamplerParameter::eMaxAnisotropy, float( GetMaxAnisotropy() ) );
				GetOpenGlES3().SetSamplerParameter( GetGlES3Name(), GlES3SamplerParameter::eCompareMode, int( GetOpenGlES3().Get( GetComparisonMode() ) ) );
				GetOpenGlES3().SetSamplerParameter( GetGlES3Name(), GlES3SamplerParameter::eCompareFunc, int( GetOpenGlES3().Get( GetComparisonFunc() ) ) );
			}
		}

		return l_return;
	}

	void GlES3Sampler::Cleanup()
	{
		ObjectType::Destroy();
	}

	void GlES3Sampler::Bind( uint32_t p_index )const
	{
		glTrackSampler( GetGlES3Name(), p_index );
		GetOpenGlES3().BindSampler( p_index, GetGlES3Name() );
	}

	void GlES3Sampler::Unbind( uint32_t p_index )const
	{
		GetOpenGlES3().BindSampler( p_index, 0u );
		glTrackSampler( 0u, p_index );
	}

	void GlES3Sampler::DoAdjustMinMipModes( GlES3InterpolationMode & p_min, GlES3InterpolationMode & p_mip )
	{
		if ( p_mip != GlES3InterpolationMode::eNearest )
		{
			if ( p_min == GlES3InterpolationMode::eLinear )
			{
				if ( p_mip == GlES3InterpolationMode::eLinear )
				{
					p_min = GlES3InterpolationMode::eLinearMipmapLinear;
				}
				else
				{
					p_min = GlES3InterpolationMode::eLinearMipmapNearest;
				}
			}
			else
			{
				if ( p_mip == GlES3InterpolationMode::eLinear )
				{
					p_min = GlES3InterpolationMode::eNearestMipmapLinear;
				}
				else
				{
					p_min = GlES3InterpolationMode::eNearestMipmapNearest;
				}
			}
		}
	}
}
