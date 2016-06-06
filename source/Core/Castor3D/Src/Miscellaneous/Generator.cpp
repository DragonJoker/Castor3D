#include "Generator.hpp"
#include "Engine.hpp"

#include <Utils.hpp>
#include <Image.hpp>

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

Generator::Thread::Thread( Generator * p_parent, uint32_t p_index, int iWidth, int iTop, int iBottom, int iTotalHeight, UbPixel const & p_pxColour )
	: m_parent( p_parent )
	, m_index( p_index )
	, m_iWidth( iWidth )
	, m_iBottom( iBottom )
	, m_iTop( iTop )
	, m_iHeight( iTotalHeight )
	, m_pxColour( p_pxColour )
	, m_pThread()
{
}

Generator::Thread::~Thread()
{
	if ( m_pThread )
	{
		m_pThread->detach();
	}
}

void Generator::Thread::Run()
{
	m_pThread = std::make_shared< std::thread >( StEntry, this );
}

void Generator::Thread::Wait()
{
	Stop();
	m_pThread->join();
}

uint32_t Generator::Thread::StEntry( Generator::Thread * p_pThis )
{
	return p_pThis->Entry();
}

int Generator::Thread::Entry()
{
	while ( !IsStopped() )
	{
		if ( m_bEnded && m_bLaunched && !IsStopped() )
		{
			auto l_lock = Castor::make_unique_lock( m_mutex );
			m_bEnded = false;
			Step();
			m_bEnded = true;
			m_bLaunched = false;
		}

		System::Sleep( 1 );
	}

	return 0;
}

//*************************************************************************************************

Generator::Generator( Engine * p_engine, int p_width, int p_height )
	: m_iWidth( p_width )
	, m_iHeight( p_height )
	, m_frontBuffer( Size( p_width, p_height ) )
	, m_backBuffer( Size( p_width, p_height ) )
	, m_engine( p_engine )
{
	//uint8_t l_tmp[] = { 255, 255, 255, 255 };
	//m_pxColour.set<ePIXEL_FORMAT_A8R8G8B8>( l_tmp);
	m_uiThreadCount = System::GetCPUCount() * 2;
}

Generator::~Generator()
{
	DoCleanup();
}

bool Generator::Step()
{
	bool l_return = false;

	if ( m_initialised )
	{
		if ( m_arraySlaveThreads.size() > 0 )
		{
			if ( m_bEnded )
			{
				m_bEnded = false;
				InitialiseStep();
			}
			else if ( AllEnded() )
			{
				SwapBuffers();
				m_bEnded = true;
				l_return = true;
			}
		}
		else
		{
			l_return = false;
		}
	}

	return l_return;
}

void Generator::SetRed( uint8_t val )
{
	m_pxColour[0] = val;
	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread * p_pThread )
	{
		if ( p_pThread )
		{
			p_pThread->SetRed( val );
		}
	} );
}

void Generator::SetGreen( uint8_t val )
{
	m_pxColour[1] = val;
	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread * p_pThread )
	{
		if ( p_pThread )
		{
			p_pThread->SetGreen( val );
		}
	} );
}

void Generator::SetBlue( uint8_t val )
{
	m_pxColour[2] = val;
	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread * p_pThread )
	{
		if ( p_pThread )
		{
			p_pThread->SetBlue( val );
		}
	} );
}

void Generator::SwapBuffers()
{
	m_frontBuffer = m_backBuffer;
}

void Generator::InitialiseStep()
{
	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread * p_pThread )
	{
		if ( p_pThread )
		{
			p_pThread->Relaunch();
		}
	} );
}

void Generator::ClearAllThreads()
{
	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread *& p_pThread )
	{
		Generator::Thread * l_pThread = p_pThread;
		p_pThread = nullptr;

		if ( l_pThread )
		{
			l_pThread->Wait();
			delete l_pThread;
		}
	} );
	m_arraySlaveThreads.clear();
}

bool Generator::AllEnded()
{
	bool l_return = true;
	uint32_t l_count = DoGetThreadsCount();

	for ( uint32_t i = 0; i < l_count && l_return; i++ )
	{
		l_return &= m_arraySlaveThreads[i] == nullptr || m_arraySlaveThreads[i]->IsEnded();
	}

	return l_return;
}

void Generator::Suspend()
{
	m_initialised = false;
	ClearAllThreads();
}

void Generator::DoCleanup()
{
	ClearAllThreads();
	m_frontBuffer.clear();
	m_backBuffer.clear();
}

Point2i Generator::_loadImage( String const & p_strImagePath, Image & CU_PARAM_UNUSED( p_image ) )
{
	ImageSPtr l_pImage = m_engine->GetImageManager().create( p_strImagePath, Path{ p_strImagePath } );
	return Point2i( l_pImage->GetWidth(), l_pImage->GetHeight() );
}

//*************************************************************************************************
