#include "IblTextures.hpp"

#include "Engine.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace
	{
		TextureUnit doCreateRadianceTexture( Engine & engine )
		{
			auto texture = engine.getRenderSystem()->createTexture( TextureType::eCube
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGB32F
				, Size{ 32u, 32u } );
			SamplerSPtr sampler;
			auto name = cuT( "IblTexturesRadiance" );

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().create( name );
				sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				engine.getSamplerCache().add( name, sampler );
			}

			TextureUnit result{ engine };
			result.setTexture( texture );
			result.setSampler( sampler );
			result.setIndex( MinTextureIndex + 6u );
			return result;
		}

		TextureUnit doCreatePrefilteredTexture( Engine & engine )
		{
			auto texture = engine.getRenderSystem()->createTexture( TextureType::eCube
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGB32F
				, Size{ 128u, 128u } );
			SamplerSPtr sampler;
			auto name = cuT( "IblTexturesPrefiltered" );

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().create( name );
				sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				sampler->setInterpolationMode( InterpolationFilter::eMip, InterpolationMode::eLinear );
				sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				engine.getSamplerCache().add( name, sampler );
			}

			TextureUnit result{ engine };
			result.setTexture( texture );
			result.setSampler( sampler );
			result.setIndex( MinTextureIndex + 7u );
			return result;
		}

		TextureUnit doCreatePrefilteredBrdf( Engine & engine )
		{
			auto texture = engine.getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eAL16F32F
				, Size{ 512u, 512u } );
			SamplerSPtr sampler;
			auto name = cuT( "IblTexturesBRDF" );

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().create( name );
				sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				engine.getSamplerCache().add( name, sampler );
			}

			TextureUnit result{ engine };
			result.setTexture( texture );
			result.setSampler( sampler );
			result.setIndex( MinTextureIndex + 8u );
			return result;
		}
	}

	//************************************************************************************************

	IblTextures::IblTextures( Scene & scene )
		: OwnedBy< Scene >{ scene }
		, m_radianceTexture{ doCreateRadianceTexture( *scene.getEngine() ) }
		, m_prefilteredEnvironment{ doCreatePrefilteredTexture( *scene.getEngine() ) }
		, m_prefilteredBrdf{ doCreatePrefilteredBrdf( *scene.getEngine() ) }
		, m_radianceComputer{ *scene.getEngine(), m_radianceTexture.getTexture()->getDimensions() }
		, m_environmentPrefilter{ *scene.getEngine(), m_prefilteredEnvironment.getTexture()->getDimensions() }
	{
		auto texture = m_radianceTexture.getTexture();
		texture->getImage( uint32_t( CubeMapFace::ePositiveX ) ).initialiseSource();
		texture->getImage( uint32_t( CubeMapFace::eNegativeX ) ).initialiseSource();
		texture->getImage( uint32_t( CubeMapFace::ePositiveY ) ).initialiseSource();
		texture->getImage( uint32_t( CubeMapFace::eNegativeY ) ).initialiseSource();
		texture->getImage( uint32_t( CubeMapFace::ePositiveZ ) ).initialiseSource();
		texture->getImage( uint32_t( CubeMapFace::eNegativeZ ) ).initialiseSource();
		m_radianceTexture.initialise();
		m_radianceTexture.getSampler()->initialise();

		texture = m_prefilteredEnvironment.getTexture();
		texture->getImage( uint32_t( CubeMapFace::ePositiveX ) ).initialiseSource();
		texture->getImage( uint32_t( CubeMapFace::eNegativeX ) ).initialiseSource();
		texture->getImage( uint32_t( CubeMapFace::ePositiveY ) ).initialiseSource();
		texture->getImage( uint32_t( CubeMapFace::eNegativeY ) ).initialiseSource();
		texture->getImage( uint32_t( CubeMapFace::ePositiveZ ) ).initialiseSource();
		texture->getImage( uint32_t( CubeMapFace::eNegativeZ ) ).initialiseSource();
		m_prefilteredEnvironment.initialise();
		m_prefilteredEnvironment.getSampler()->initialise();
		texture->bind( MinTextureIndex );
		texture->generateMipmaps();
		texture->unbind( MinTextureIndex );

		texture = m_prefilteredBrdf.getTexture();
		texture->getImage().initialiseSource();
		m_prefilteredBrdf.initialise();
		m_prefilteredBrdf.getSampler()->initialise();

		BrdfPrefilter filter{ *scene.getEngine(), m_prefilteredBrdf.getTexture()->getDimensions() };
		filter.render( m_prefilteredBrdf.getTexture() );
	}

	IblTextures::~IblTextures()
	{
		m_radianceTexture.cleanup();
		m_prefilteredEnvironment.cleanup();
		m_prefilteredBrdf.cleanup();
	}

	void IblTextures::update( TextureLayout const & source )
	{
		m_radianceComputer.render( source, m_radianceTexture.getTexture() );
		m_environmentPrefilter.render( source, m_prefilteredEnvironment.getTexture() );
	}

	void IblTextures::debugDisplay( Size const & renderSize )const
	{
		int width = int( m_prefilteredBrdf.getTexture()->getWidth() );
		int height = int( m_prefilteredBrdf.getTexture()->getHeight() );
		int left = 0u;
		int top = renderSize.getHeight() - height;
		auto size = Size( width, height );
		auto & context = *getScene()->getEngine()->getRenderSystem()->getCurrentContext();
		context.renderTexture( Position{ left, top }
			, size
			, *m_prefilteredBrdf.getTexture() );
		left += 512;
		context.renderTextureCube( Position{ left, top }
			, Size( width / 4, height / 4u )
			, *m_prefilteredEnvironment.getTexture() );
		left += 512;
		context.renderTextureCube( Position{ left, top }
			, Size( width / 4, height / 4u )
			, *m_radianceTexture.getTexture() );
	}
}
