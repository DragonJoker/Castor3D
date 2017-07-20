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
			auto texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eCube
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGB32F
				, Size{ 32u, 32u } );
			SamplerSPtr sampler;
			auto name = cuT( "IblTexturesRadiance" );

			if ( p_engine.GetSamplerCache().Has( name ) )
			{
				sampler = p_engine.GetSamplerCache().Find( name );
			}
			else
			{
				sampler = p_engine.GetSamplerCache().Create( name );
				sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				p_engine.GetSamplerCache().Add( name, sampler );
			}

			TextureUnit result{ p_engine };
			result.SetTexture( texture );
			result.SetSampler( sampler );
			result.SetIndex( 7u );
			return result;
		}

		TextureUnit DoCreatePrefilteredTexture( Engine & p_engine )
		{
			auto texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eCube
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGB32F
				, Size{ 128u, 128u } );
			SamplerSPtr sampler;
			auto name = cuT( "IblTexturesPrefiltered" );

			if ( p_engine.GetSamplerCache().Has( name ) )
			{
				sampler = p_engine.GetSamplerCache().Find( name );
			}
			else
			{
				sampler = p_engine.GetSamplerCache().Create( name );
				sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				sampler->SetInterpolationMode( InterpolationFilter::eMip, InterpolationMode::eLinear );
				sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				p_engine.GetSamplerCache().Add( name, sampler );
			}

			TextureUnit result{ p_engine };
			result.SetTexture( texture );
			result.SetSampler( sampler );
			result.SetIndex( 8u );
			return result;
		}

		TextureUnit DoCreatePrefilteredBrdf( Engine & p_engine )
		{
			auto texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eAL16F32F
				, Size{ 512u, 512u } );
			SamplerSPtr sampler;
			auto name = cuT( "IblTexturesBRDF" );

			if ( p_engine.GetSamplerCache().Has( name ) )
			{
				sampler = p_engine.GetSamplerCache().Find( name );
			}
			else
			{
				sampler = p_engine.GetSamplerCache().Create( name );
				sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				p_engine.GetSamplerCache().Add( name, sampler );
			}

			TextureUnit result{ p_engine };
			result.SetTexture( texture );
			result.SetSampler( sampler );
			result.SetIndex( 9u );
			return result;
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
		auto texture = m_radianceTexture.GetTexture();
		texture->GetImage( uint32_t( CubeMapFace::ePositiveX ) ).InitialiseSource();
		texture->GetImage( uint32_t( CubeMapFace::eNegativeX ) ).InitialiseSource();
		texture->GetImage( uint32_t( CubeMapFace::ePositiveY ) ).InitialiseSource();
		texture->GetImage( uint32_t( CubeMapFace::eNegativeY ) ).InitialiseSource();
		texture->GetImage( uint32_t( CubeMapFace::ePositiveZ ) ).InitialiseSource();
		texture->GetImage( uint32_t( CubeMapFace::eNegativeZ ) ).InitialiseSource();
		m_radianceTexture.Initialise();
		m_radianceTexture.GetSampler()->Initialise();

		texture = m_prefilteredEnvironment.GetTexture();
		texture->GetImage( uint32_t( CubeMapFace::ePositiveX ) ).InitialiseSource();
		texture->GetImage( uint32_t( CubeMapFace::eNegativeX ) ).InitialiseSource();
		texture->GetImage( uint32_t( CubeMapFace::ePositiveY ) ).InitialiseSource();
		texture->GetImage( uint32_t( CubeMapFace::eNegativeY ) ).InitialiseSource();
		texture->GetImage( uint32_t( CubeMapFace::ePositiveZ ) ).InitialiseSource();
		texture->GetImage( uint32_t( CubeMapFace::eNegativeZ ) ).InitialiseSource();
		m_prefilteredEnvironment.Initialise();
		m_prefilteredEnvironment.GetSampler()->Initialise();
		texture->Bind( 0u );
		texture->GenerateMipmaps();
		texture->Unbind( 0u );

		texture = m_prefilteredBrdf.GetTexture();
		texture->GetImage().InitialiseSource();
		m_prefilteredBrdf.Initialise();
		m_prefilteredBrdf.GetSampler()->Initialise();

		BrdfPrefilter filter{ *p_scene.GetEngine(), m_prefilteredBrdf.GetTexture()->GetDimensions() };
		filter.Render( m_prefilteredBrdf.GetTexture() );
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
		int width = int( m_prefilteredBrdf.GetTexture()->GetWidth() );
		int height = int( m_prefilteredBrdf.GetTexture()->GetHeight() );
		int left = 0u;
		int top = p_camera.GetHeight() - height;
		auto size = Size( width, height );
		auto & context = *GetScene()->GetEngine()->GetRenderSystem()->GetCurrentContext();
		p_camera.Apply();
		context.RenderTexture( Position{ left, top }
			, size
			, *m_prefilteredBrdf.GetTexture() );
		left += 512;
		context.RenderTextureCube( Position{ left, top }
			, Size( width / 4, height / 4u )
			, *m_prefilteredEnvironment.GetTexture() );
		left += 512;
		context.RenderTextureCube( Position{ left, top }
			, Size( width / 4, height / 4u )
			, *m_radianceTexture.GetTexture() );
	}
}
