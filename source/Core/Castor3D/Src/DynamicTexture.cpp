﻿#include "DynamicTexture.hpp"

#include "RenderSystem.hpp"
#include "Sampler.hpp"

#include <Image.hpp>
#include <PixelBufferBase.hpp>

using namespace Castor;

namespace Castor3D
{
	DynamicTexture::DynamicTexture( RenderSystem & p_renderSystem )
		: TextureBase( eTEXTURE_BASE_TYPE_DYNAMIC, p_renderSystem )
		, m_iSamplesCount( 0 )
	{
	}

	DynamicTexture::~DynamicTexture()
	{
	}

	bool DynamicTexture::Initialise( uint32_t p_index, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		if ( !m_bInitialised )
		{
			if ( !GetOwner()->HasNonPowerOfTwoTextures() )
			{
				Castor::Image l_img( cuT( "Tmp" ), *m_pPixelBuffer );
				Castor::Size l_size = m_pPixelBuffer->dimensions();
				l_size.set( GetNext2Pow( l_size.width() ), GetNext2Pow( l_size.height() ) );
				m_pPixelBuffer = l_img.Resample( l_size ).GetPixels();
			}

			m_cpuAccess = p_cpuAccess;
			m_gpuAccess = p_gpuAccess;
			m_uiIndex = p_index;
			m_bInitialised = DoInitialise();

			if ( GetSampler() )
			{
				GetSampler()->Initialise();
			}
		}

		return m_bInitialised;
	}

	void DynamicTexture::Cleanup()
	{
		if ( m_bInitialised )
		{
			m_bInitialised = false;
		}
	}

	bool DynamicTexture::BindAt( uint32_t p_index )
	{
		bool l_return = false;

		if ( m_bInitialised )
		{
			l_return = DoBind( p_index );

			if ( l_return && GetSampler() )
			{
				l_return = GetSampler()->Bind( m_type, p_index );
			}
		}

		return l_return;
	}

	void DynamicTexture::UnbindFrom( uint32_t p_index )
	{
		if ( GetSampler() )
		{
			GetSampler()->Unbind();
		}

		DoUnbind( p_index );
	}

	void DynamicTexture::Resize( Castor::Size const & p_size )
	{
		m_uiDepth = 1;

		if ( m_pPixelBuffer && m_pPixelBuffer->dimensions() != p_size )
		{
			Castor::Size l_size = p_size;

			if ( !GetOwner()->HasNonPowerOfTwoTextures() )
			{
				l_size.set( GetNext2Pow( l_size.width() ), GetNext2Pow( l_size.height() ) );
			}

			m_pPixelBuffer = PxBufferBase::create( l_size, m_pPixelBuffer->format() );
			Cleanup();
			m_bInitialised = DoInitialise();
		}
	}

	void DynamicTexture::Resize( Castor::Point3ui const & p_size )
	{
		Size l_size;

		if ( !GetOwner()->HasNonPowerOfTwoTextures() )
		{
			m_uiDepth = GetNext2Pow( p_size[2] );
			l_size.set( GetNext2Pow( p_size[0] ), GetNext2Pow( p_size[1] ) * m_uiDepth );
		}
		else
		{
			m_uiDepth = p_size[2];
			l_size.set( p_size[0], p_size[1] * m_uiDepth );
		}

		if ( m_pPixelBuffer && m_pPixelBuffer->dimensions() != l_size )
		{
			m_pPixelBuffer = PxBufferBase::create( l_size, m_pPixelBuffer->format() );
			Cleanup();
			m_bInitialised = DoInitialise();
		}
	}

	void DynamicTexture::SetImage( Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_ePixelFormat )
	{
		m_uiDepth = 1;
		Castor::Size l_size = p_size;

		if ( !GetOwner()->HasNonPowerOfTwoTextures() )
		{
			l_size.set( GetNext2Pow( l_size.width() ), GetNext2Pow( l_size.height() ) );
		}

		m_pPixelBuffer = Castor::PxBufferBase::create( l_size, p_ePixelFormat );
	}

	void DynamicTexture::SetImage( Castor::Point3ui const & p_size, Castor::ePIXEL_FORMAT p_ePixelFormat )
	{
		Size l_size;

		if ( !GetOwner()->HasNonPowerOfTwoTextures() )
		{
			m_uiDepth = GetNext2Pow( p_size[2] );
			l_size.set( GetNext2Pow( p_size[0] ), GetNext2Pow( p_size[1] ) * m_uiDepth );
		}
		else
		{
			m_uiDepth = p_size[2];
			l_size.set( p_size[0], p_size[1] * m_uiDepth );
		}

		m_pPixelBuffer = Castor::PxBufferBase::create( l_size, p_ePixelFormat );
	}
}
