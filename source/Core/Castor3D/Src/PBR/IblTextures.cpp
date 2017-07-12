#include "IblTextures.hpp"

#include "Engine.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace Castor;
using namespace GLSL;

namespace Castor3D
{
	namespace
	{
		TextureUnit DoCreateRadianceTexture( Engine & p_engine )
		{
			auto l_texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eCube
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGB32F
				, Size{ 32u, 32u } );
			SamplerSPtr l_sampler;
			auto l_name = cuT( "IblTexturesRadiance" );

			if ( p_engine.GetSamplerCache().Has( l_name ) )
			{
				l_sampler = p_engine.GetSamplerCache().Find( l_name );
			}
			else
			{
				l_sampler = p_engine.GetSamplerCache().Create( l_name );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				p_engine.GetSamplerCache().Add( l_name, l_sampler );
			}

			TextureUnit l_result{ p_engine };
			l_result.SetTexture( l_texture );
			l_result.SetSampler( l_sampler );
			l_result.SetIndex( 7u );
			return l_result;
		}

		TextureUnit DoCreatePrefilteredTexture( Engine & p_engine )
		{
			auto l_texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eCube
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGB32F
				, Size{ 128u, 128u } );
			SamplerSPtr l_sampler;
			auto l_name = cuT( "IblTexturesPrefiltered" );

			if ( p_engine.GetSamplerCache().Has( l_name ) )
			{
				l_sampler = p_engine.GetSamplerCache().Find( l_name );
			}
			else
			{
				l_sampler = p_engine.GetSamplerCache().Create( l_name );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMip, InterpolationMode::eLinear );
				l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				p_engine.GetSamplerCache().Add( l_name, l_sampler );
			}

			TextureUnit l_result{ p_engine };
			l_result.SetTexture( l_texture );
			l_result.SetSampler( l_sampler );
			l_result.SetIndex( 8u );
			return l_result;
		}

		TextureUnit DoCreatePrefilteredBrdf( Engine & p_engine )
		{
			auto l_texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eAL16F32F
				, Size{ 512u, 512u } );
			SamplerSPtr l_sampler;
			auto l_name = cuT( "IblTexturesBRDF" );

			if ( p_engine.GetSamplerCache().Has( l_name ) )
			{
				l_sampler = p_engine.GetSamplerCache().Find( l_name );
			}
			else
			{
				l_sampler = p_engine.GetSamplerCache().Create( l_name );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				p_engine.GetSamplerCache().Add( l_name, l_sampler );
			}

			TextureUnit l_result{ p_engine };
			l_result.SetTexture( l_texture );
			l_result.SetSampler( l_sampler );
			l_result.SetIndex( 9u );
			return l_result;
		}
	}

	//************************************************************************************************

	IblTextures::IblTextures( Scene & p_scene )
		: OwnedBy< Scene >{ p_scene }
		, m_radianceTexture{ DoCreateRadianceTexture( *p_scene.GetEngine() ) }
		, m_prefilteredEnvironment{ DoCreatePrefilteredTexture( *p_scene.GetEngine() ) }
		, m_prefilteredBrdf{ DoCreatePrefilteredBrdf( *p_scene.GetEngine() ) }
		, m_radianceComputer{ *p_scene.GetEngine(), m_radianceTexture.GetTexture()->GetDimensions() }
		, m_environmentPrefilter{ *p_scene.GetEngine(), m_prefilteredEnvironment.GetTexture()->GetDimensions() }
	{
		auto l_texture = m_radianceTexture.GetTexture();
		l_texture->GetImage( uint32_t( CubeMapFace::ePositiveX ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::eNegativeX ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::ePositiveY ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::eNegativeY ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::ePositiveZ ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::eNegativeZ ) ).InitialiseSource();
		m_radianceTexture.Initialise();
		m_radianceTexture.GetSampler()->Initialise();

		l_texture = m_prefilteredEnvironment.GetTexture();
		l_texture->GetImage( uint32_t( CubeMapFace::ePositiveX ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::eNegativeX ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::ePositiveY ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::eNegativeY ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::ePositiveZ ) ).InitialiseSource();
		l_texture->GetImage( uint32_t( CubeMapFace::eNegativeZ ) ).InitialiseSource();
		m_prefilteredEnvironment.Initialise();
		m_prefilteredEnvironment.GetSampler()->Initialise();
		l_texture->Bind( 0u );
		l_texture->GenerateMipmaps();
		l_texture->Unbind( 0u );

		l_texture = m_prefilteredBrdf.GetTexture();
		l_texture->GetImage().InitialiseSource();
		m_prefilteredBrdf.Initialise();
		m_prefilteredBrdf.GetSampler()->Initialise();

		BrdfPrefilter l_filter{ *p_scene.GetEngine(), m_prefilteredBrdf.GetTexture()->GetDimensions() };
		l_filter.Render( m_prefilteredBrdf.GetTexture() );
	}

	IblTextures::~IblTextures()
	{
		m_radianceTexture.Cleanup();
		m_prefilteredEnvironment.Cleanup();
		m_prefilteredBrdf.Cleanup();
	}

	void IblTextures::Update( TextureLayout const & p_source )
	{
		m_radianceComputer.Render( p_source, m_radianceTexture.GetTexture() );
		m_environmentPrefilter.Render( p_source, m_prefilteredEnvironment.GetTexture() );
	}

	void IblTextures::Debug( Camera const & p_camera )const
	{
		int l_width = int( m_prefilteredBrdf.GetTexture()->GetWidth() );
		int l_height = int( m_prefilteredBrdf.GetTexture()->GetHeight() );
		int l_left = 0u;
		int l_top = p_camera.GetHeight() - l_height;
		auto l_size = Size( l_width, l_height );
		auto & l_context = *GetScene()->GetEngine()->GetRenderSystem()->GetCurrentContext();
		p_camera.Apply();
		l_context.RenderTexture( Position{ l_left, l_top }
			, l_size
			, *m_prefilteredBrdf.GetTexture() );
		l_left += 512;
		l_context.RenderTextureCube( Position{ l_left, l_top }
			, Size( l_width / 4, l_height / 4u )
			, *m_prefilteredEnvironment.GetTexture() );
		l_left += 512;
		l_context.RenderTextureCube( Position{ l_left, l_top }
			, Size( l_width / 4, l_height / 4u )
			, *m_radianceTexture.GetTexture() );
	}
}
