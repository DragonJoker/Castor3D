#include "Texture/GlSampler.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlSampler::GlSampler( OpenGl & p_gl, GlRenderSystem * renderSystem, castor::String const & p_name )
		: Sampler( *renderSystem->getEngine(), p_name )
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

	bool GlSampler::initialise()
	{
		bool result = isValid();

		if ( !result )
		{
			result = ObjectType::create();

			if ( result )
			{
				GlInterpolationMode minMode = getOpenGl().get( getInterpolationMode( InterpolationFilter::eMin ) );
				GlInterpolationMode mipMode = getOpenGl().get( getInterpolationMode( InterpolationFilter::eMip ) );
				doAdjustMinMipModes( minMode, mipMode );

				getOpenGl().SetSamplerParameter( getGlName(), GlSamplerParameter::eLODBias, float( getLodBias() ) );
				getOpenGl().SetSamplerParameter( getGlName(), GlSamplerParameter::eUWrap, int( getOpenGl().get( getWrappingMode( TextureUVW::eU ) ) ) );
				getOpenGl().SetSamplerParameter( getGlName(), GlSamplerParameter::eVWrap, int( getOpenGl().get( getWrappingMode( TextureUVW::eV ) ) ) );
				getOpenGl().SetSamplerParameter( getGlName(), GlSamplerParameter::eWWrap, int( getOpenGl().get( getWrappingMode( TextureUVW::eW ) ) ) );
				getOpenGl().SetSamplerParameter( getGlName(), GlSamplerParameter::eMinFilter, int( minMode ) );
				getOpenGl().SetSamplerParameter( getGlName(), GlSamplerParameter::eMagFilter, int( getOpenGl().get( getInterpolationMode( InterpolationFilter::eMag ) ) ) );
				getOpenGl().SetSamplerParameter( getGlName(), GlSamplerParameter::eBorderColour, getBorderColour().constPtr() );
				getOpenGl().SetSamplerParameter( getGlName(), GlSamplerParameter::eMaxAnisotropy, float( getMaxAnisotropy() ) );
				getOpenGl().SetSamplerParameter( getGlName(), GlSamplerParameter::eCompareMode, int( getOpenGl().get( getComparisonMode() ) ) );
				getOpenGl().SetSamplerParameter( getGlName(), GlSamplerParameter::eCompareFunc, int( getOpenGl().get( getComparisonFunc() ) ) );
			}
		}

		return result;
	}

	void GlSampler::cleanup()
	{
		ObjectType::destroy();
	}

	void GlSampler::bind( uint32_t p_index )const
	{
		glTrackSampler( getGlName(), p_index );
		getOpenGl().BindSampler( p_index, getGlName() );
	}

	void GlSampler::unbind( uint32_t p_index )const
	{
		getOpenGl().BindSampler( p_index, 0u );
		glTrackSampler( 0u, p_index );
	}

	void GlSampler::doAdjustMinMipModes( GlInterpolationMode & p_min, GlInterpolationMode & p_mip )
	{
		if ( p_mip != GlInterpolationMode::eNearest )
		{
			if ( p_min == GlInterpolationMode::eLinear )
			{
				if ( p_mip == GlInterpolationMode::eLinear )
				{
					p_min = GlInterpolationMode::eLinearMipmapLinear;
				}
				else
				{
					p_min = GlInterpolationMode::eLinearMipmapNearest;
				}
			}
			else
			{
				if ( p_mip == GlInterpolationMode::eLinear )
				{
					p_min = GlInterpolationMode::eNearestMipmapLinear;
				}
				else
				{
					p_min = GlInterpolationMode::eNearestMipmapNearest;
				}
			}
		}
	}
}
