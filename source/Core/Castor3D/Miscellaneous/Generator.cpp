#include "Castor3D/Miscellaneous/Generator.hpp"
#include "Castor3D/Engine.hpp"

#include <CastorUtils/Graphics/Image.hpp>

using namespace castor3d;
using namespace castor;

//*************************************************************************************************

Generator::Thread::Thread( Generator * parent, uint32_t index, int iWidth, int iTop, int iBottom, int iTotalHeight, UbPixel const & pxColour )
	: m_parent( parent )
	, m_index( index )
	, m_iWidth( iWidth )
	, m_iBottom( iBottom )
	, m_iTop( iTop )
	, m_iHeight( iTotalHeight )
	, m_pxColour( pxColour )
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

void Generator::Thread::run()
{
	m_pThread = std::make_shared< std::thread >( StEntry, this );
}

void Generator::Thread::wait()
{
	stop();
	m_pThread->join();
}

uint32_t Generator::Thread::StEntry( Generator::Thread * pThis )
{
	return pThis->Entry();
}

int Generator::Thread::Entry()
{
	while ( !IsStopped() )
	{
		if ( m_bEnded && m_bLaunched && !IsStopped() )
		{
			auto lock = castor::makeUniqueLock( m_mutex );
			m_bEnded = false;
			Step();
			m_bEnded = true;
			m_bLaunched = false;
		}

		System::sleep( 1 );
	}

	return 0;
}

//*************************************************************************************************

Generator::Generator( Engine * engine, int width, int height )
	: m_iWidth( width )
	, m_iHeight( height )
	, m_frontBuffer( Size( width, height ) )
	, m_backBuffer( Size( width, height ) )
	, m_engine( engine )
{
	//uint8_t tmp[] = { 255, 255, 255, 255 };
	//m_pxColour.set<PixelFormat::eR8G8B8A8_UNORM>( tmp);
	m_uiThreadCount = engine->getCpuInformations().getCoreCount() * 2;
}

Generator::~Generator()
{
	doCleanup();
}

bool Generator::Step()
{
	bool result = false;

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
				swapBuffers();
				m_bEnded = true;
				result = true;
			}
		}
		else
		{
			result = false;
		}
	}

	return result;
}

void Generator::setRed( uint8_t val )
{
	m_pxColour[0] = val;
	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread * pThread )
	{
		if ( pThread )
		{
			pThread->setRed( val );
		}
	} );
}

void Generator::setGreen( uint8_t val )
{
	m_pxColour[1] = val;
	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread * pThread )
	{
		if ( pThread )
		{
			pThread->setGreen( val );
		}
	} );
}

void Generator::setBlue( uint8_t val )
{
	m_pxColour[2] = val;
	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread * pThread )
	{
		if ( pThread )
		{
			pThread->setBlue( val );
		}
	} );
}

void Generator::swapBuffers()
{
	m_frontBuffer = m_backBuffer;
}

void Generator::InitialiseStep()
{
	for ( auto & thread : m_arraySlaveThreads )
	{
		if ( thread )
		{
			thread->Relaunch();
		}
	}
}

void Generator::ClearAllThreads()
{
	for ( auto & thread : m_arraySlaveThreads )
	{
		Generator::Thread * pThread = thread;
		thread = nullptr;

		if ( pThread )
		{
			pThread->wait();
			delete pThread;
		}
	}

	m_arraySlaveThreads.clear();
}

bool Generator::AllEnded()
{
	bool result = true;
	uint32_t count = doGetThreadsCount();

	for ( uint32_t i = 0; i < count && result; i++ )
	{
		result &= m_arraySlaveThreads[i] == nullptr || m_arraySlaveThreads[i]->isEnded();
	}

	return result;
}

void Generator::Suspend()
{
	m_initialised = false;
	ClearAllThreads();
}

void Generator::doCleanup()
{
	ClearAllThreads();
	m_frontBuffer.clear();
	m_backBuffer.clear();
}

Point2i Generator::_loadImage( String const & strImagePath, Image & CU_UnusedParam( image ) )
{
	ImageSPtr pImage = m_engine->getImageCache().add( strImagePath, Path{ strImagePath }, false );
	return Point2i( pImage->getWidth(), pImage->getHeight() );
}

//*************************************************************************************************
