#include "CubeMapTexture.hpp"

#include "Sampler.hpp"

#include "Render/RenderSystem.hpp"

#include <Logger.hpp>
#include <Image.hpp>

using namespace Castor;

namespace Castor3D
{
	CubeMapTexture::CubeMapTexture( RenderSystem & p_renderSystem, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: Texture{ eTEXTURE_BASE_TYPE_CUBE, p_renderSystem, p_cpuAccess, p_gpuAccess }
	{
		m_depth = 1;
	}

	CubeMapTexture::~CubeMapTexture()
	{
	}

	void CubeMapTexture::SetImages( PxBufferArray p_buffers )
	{
		//REQUIRE( p_buffers.size() == size_t( CubeMapFace::Count ) );
		//PxBufferArray l_buffers( 6 );
		//std::swap( m_pixelBuffers, l_buffers );
		//CubeMapFace l_face = CubeMapFace::PositiveX;

		//for ( auto & l_buffer : p_buffers )
		//{
		//	DoSetImage( l_buffer, l_face );
		//	l_face = CubeMapFace( uint8_t( l_face ) + 1u );
		//}
	}

	bool CubeMapTexture::Initialise()
	{
		//if ( !m_initialised )
		//{
		//	m_initialised = DoInitialise();

		//	if ( m_initialised && m_pixelBuffers[0] && m_pixelBuffers[0]->ptr() )
		//	{
		//		m_size = m_pixelBuffers[0]->dimensions();
		//		m_pixelFormat = m_pixelBuffers[0]->format();

		//		for ( auto & l_buffer : m_pixelBuffers )
		//		{
		//			l_buffer->clear();
		//		}

		//		m_pixelBuffers.clear();
		//	}
		//}
		//else
		//{
		//	Logger::LogWarning( cuT( "Calling CubeMapTexture::Initialise on an already initialised texture" ) );
		//}

		return m_initialised;
	}

	void CubeMapTexture::Cleanup()
	{
		m_initialised = false;
	}

	void CubeMapTexture::DoSetImage( Castor::PxBufferBaseSPtr p_buffer, CubeMapFace p_face )
	{
		//if ( !GetRenderSystem()->GetGpuInformations().HasNonPowerOfTwoTextures() )
		//{
		//	Size l_size( GetNext2Pow( p_buffer->dimensions().width() ), GetNext2Pow( p_buffer->dimensions().height() ) );
		//	Castor::Image l_img( cuT( "Tmp" ), *p_buffer );
		//	m_pixelBuffers[p_face] = l_img.Resample( l_size ).GetPixels();
		//}
		//else
		//{
		//	m_pixelBuffers[p_face] = p_buffer;
		//}
	}
}
