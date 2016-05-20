#include "CubeMapTexture.hpp"

#include "Sampler.hpp"

#include "Render/RenderSystem.hpp"

#include <Logger.hpp>
#include <Image.hpp>

using namespace Castor;

namespace Castor3D
{
	CubeMapTexture::CubeMapTexture( RenderSystem & p_renderSystem, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: TextureLayout{ p_renderSystem, eTEXTURE_TYPE_CUBE, p_cpuAccess, p_gpuAccess }
	{
	}

	CubeMapTexture::~CubeMapTexture()
	{
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
}
