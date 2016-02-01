#include "GlSampler.hpp"

#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	//************************************************************************************************

	GlSamplerImpl::GlSamplerImpl( GlSampler & p_sampler )
		: Castor::OwnedBy< GlSampler >( p_sampler )
	{
	}

	GlSamplerImpl::~GlSamplerImpl()
	{
	}

	void GlSamplerImpl::DoAdjustMinMipModes( GlSampler const & p_sampler, OpenGl const & p_gl, eGL_INTERPOLATION_MODE & p_min, eGL_INTERPOLATION_MODE & p_mip )
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

	//************************************************************************************************

	class GlSamplerStd
		: public GlSamplerImpl
		, public Holder
	{
	public:
		GlSamplerStd( GlSampler & p_sampler, OpenGl & p_gl )
			: GlSamplerImpl( p_sampler )
			, Holder( p_gl )
		{
		}

		~GlSamplerStd()
		{
		}

		bool Bind( eGL_TEXDIM p_dimension, uint32_t p_index )
		{
			bool l_return = true;
			eGL_INTERPOLATION_MODE l_minMode = GetOpenGl().Get( GetOwner()->GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) );
			eGL_INTERPOLATION_MODE l_mipMode = GetOpenGl().Get( GetOwner()->GetInterpolationMode( eINTERPOLATION_FILTER_MIP ) );
			DoAdjustMinMipModes( *GetOwner(), GetOpenGl(), l_minMode, l_mipMode );

			l_return &= GetOpenGl().TexParameter( p_dimension, eGL_TEXTURE_PARAMETER_MINLOD, float( GetOwner()->GetMinLod() ) );
			l_return &= GetOpenGl().TexParameter( p_dimension, eGL_TEXTURE_PARAMETER_MAXLOD, float( GetOwner()->GetMaxLod() ) );
			l_return &= GetOpenGl().TexParameter( p_dimension, eGL_TEXTURE_PARAMETER_LODBIAS, float( GetOwner()->GetLodBias() ) );
			l_return &= GetOpenGl().TexParameter( p_dimension, eGL_TEXTURE_PARAMETER_WRAP_S, GetOpenGl().Get( GetOwner()->GetWrappingMode( eTEXTURE_UVW_U ) ) );
			l_return &= GetOpenGl().TexParameter( p_dimension, eGL_TEXTURE_PARAMETER_WRAP_T, GetOpenGl().Get( GetOwner()->GetWrappingMode( eTEXTURE_UVW_V ) ) );
			l_return &= GetOpenGl().TexParameter( p_dimension, eGL_TEXTURE_PARAMETER_WRAP_R, GetOpenGl().Get( GetOwner()->GetWrappingMode( eTEXTURE_UVW_W ) ) );
			l_return &= GetOpenGl().TexParameter( p_dimension, eGL_TEXTURE_PARAMETER_MIN_FILTER, l_minMode );
			l_return &= GetOpenGl().TexParameter( p_dimension, eGL_TEXTURE_PARAMETER_MAG_FILTER, GetOpenGl().Get( GetOwner()->GetInterpolationMode( eINTERPOLATION_FILTER_MAG ) ) );
			//l_return &= GetOpenGl().TexParameter( p_dimension, eGL_TEXTURE_PARAMETER_BORDERCOLOUR, GetBorderColour().const_ptr() );
			l_return &= GetOpenGl().TexParameter( p_dimension, eGL_TEXTURE_PARAMETER_MAX_ANISOTROPY, float( GetOwner()->GetMaxAnisotropy() ) );
			return l_return;
		}

		void Unbind()
		{
		}
	};

	//************************************************************************************************

	class GlSamplerObject
		: public GlSamplerImpl
		, private Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >
	{
		using ObjectType = Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >;

	public:
		GlSamplerObject( GlSampler & p_sampler, OpenGl & p_gl )
			: GlSamplerImpl( p_sampler )
			, ObjectType( p_gl,
						  "GlSampler",
						  std::bind( &OpenGl::GenSamplers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						  std::bind( &OpenGl::DeleteSamplers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						  std::bind( &OpenGl::IsSampler, std::ref( p_gl ), std::placeholders::_1 )
						)
		{
			if ( ObjectType::Create() )
			{
				eGL_INTERPOLATION_MODE l_minMode = GetOpenGl().Get( GetOwner()->GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) );
				eGL_INTERPOLATION_MODE l_mipMode = GetOpenGl().Get( GetOwner()->GetInterpolationMode( eINTERPOLATION_FILTER_MIP ) );
				DoAdjustMinMipModes( *GetOwner(), GetOpenGl(), l_minMode, l_mipMode );

				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_LODBIAS, float( GetOwner()->GetLodBias() ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_UWRAP, GetOpenGl().Get( GetOwner()->GetWrappingMode( eTEXTURE_UVW_U ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_VWRAP, GetOpenGl().Get( GetOwner()->GetWrappingMode( eTEXTURE_UVW_V ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_WWRAP, GetOpenGl().Get( GetOwner()->GetWrappingMode( eTEXTURE_UVW_W ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_MINFILTER, l_minMode );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_MAGFILTER, GetOpenGl().Get( GetOwner()->GetInterpolationMode( eINTERPOLATION_FILTER_MAG ) ) );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_BORDERCOLOUR, GetOwner()->GetBorderColour().const_ptr() );
				GetOpenGl().SetSamplerParameter( GetGlName(), eGL_SAMPLER_PARAMETER_MAXANISOTROPY, float( GetOwner()->GetMaxAnisotropy() ) );
			}
		}

		~GlSamplerObject()
		{
			Object::Destroy();
		}

		virtual bool Bind( eGL_TEXDIM p_dimension, uint32_t p_index )
		{
			return GetOpenGl().BindSampler( p_index, GetGlName() );
		}

		virtual void Unbind()
		{
		}
	};

	//************************************************************************************************

	GlSampler::GlSampler( OpenGl & p_gl, GlRenderSystem * p_renderSystem, Castor::String const & p_name )
		: Sampler( *p_renderSystem->GetEngine(), p_name )
		, Holder( p_gl )
	{
	}

	GlSampler::~GlSampler()
	{
	}

	bool GlSampler::Initialise()
	{
		bool l_return = bool( m_impl );

		if ( !l_return )
		{
			if ( GetOpenGl().HasSpl() )
			{
				m_impl = std::make_unique< GlSamplerObject >( *this, GetOpenGl() );
			}
			else
			{
				m_impl = std::make_unique< GlSamplerStd >( *this, GetOpenGl() );
			}
		}

		return l_return;
	}

	void GlSampler::Cleanup()
	{
		m_impl.reset();
	}

	bool GlSampler::Bind( eTEXTURE_TYPE p_dimension, uint32_t p_index )
	{
		m_glDimension = GetOpenGl().Get( p_dimension );
		bool l_return = true;

		if ( m_glDimension != eGL_TEXDIM_BUFFER )
		{
			REQUIRE( m_impl );
			l_return = m_impl->Bind( GetOpenGl().Get( p_dimension ), p_index );
		}

		return l_return;
	}

	void GlSampler::Unbind()
	{
		if ( m_glDimension != eGL_TEXDIM_BUFFER )
		{
			REQUIRE( m_impl );
			m_impl->Unbind();
		}
	}
}
