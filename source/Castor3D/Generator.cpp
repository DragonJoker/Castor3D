#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Generator.hpp"
#include "Castor3D/Engine.hpp"
#include <CastorUtils/Utils.hpp>

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

Generator::Thread :: Thread( Generator * p_pParent, uint32_t p_uiIndex, int iWidth, int iTop, int iBottom, int iTotalHeight, UbPixel const & p_pxColour )
	:	m_pParent	( p_pParent		)
	,	m_uiIndex	( p_uiIndex		)
	,	m_iWidth	( iWidth		)
	,	m_iBottom	( iBottom		)
	,	m_iTop		( iTop			)
	,	m_iHeight	( iTotalHeight	)
	,	m_pxColour	( p_pxColour	)
	,	m_bEnded	( true			)
	,	m_bLaunched	( false			)
	,	m_bStopped	( false			)
	,	m_pThread	(				)
{
}

Generator::Thread :: ~Thread()
{
	if( m_pThread )
	{
		m_pThread->detach();
	}
}

void Generator::Thread :: Run()
{
	m_pThread = std::make_shared< std::thread >( StEntry, this );
}

void Generator::Thread :: Wait()
{
	Stop();
	m_pThread->join();
}

uint32_t Generator::Thread :: StEntry( Generator::Thread * p_pThis )
{
	return p_pThis->Entry();
}

int Generator::Thread :: Entry()
{
	while( !IsStopped() )
	{
		if( m_bEnded && m_bLaunched && !IsStopped() )
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			m_bEnded = false;
			Step();
			m_bEnded = true;
			m_bLaunched = false;
		}

		Castor::Sleep( 1 );
	}

	return 0;
}

//*************************************************************************************************

Generator :: Generator( Engine * p_pEngine, int p_width, int p_height )
	:	m_iWidth			( p_width					)
	,	m_iHeight			( p_height					)
	,	m_ullStep			( 0							)
	,	m_bInitialised		( false						)
	,	m_bEnded			( true						)
	,	m_frontBuffer		( Size( p_width, p_height )	)
	,	m_backBuffer		( Size( p_width, p_height )	)
	,	m_pEngine			( p_pEngine					)
{
//	uint8_t l_tmp[] = { 255, 255, 255, 255 };
//	m_pxColour.set<ePIXEL_FORMAT_A8R8G8B8>( l_tmp);
	m_uiThreadCount = Castor::GetCPUCount() * 2;
}

Generator :: ~Generator()
{
	DoCleanup();
}

bool Generator :: Step()
{
	bool l_bReturn = false;

	if( m_bInitialised )
	{
		if( m_arraySlaveThreads.size() > 0 )
		{
			if( m_bEnded )
			{
				m_bEnded = false;
				InitialiseStep();
			}
			else if( AllEnded() )
			{
				SwapBuffers();
				m_bEnded = true;
				l_bReturn = true;
			}
		}
		else
		{
			l_bReturn = false;
		}
	}

	return l_bReturn;
}

void Generator :: SetRed( uint8_t val )
{
	m_pxColour[0] = val;

	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread * p_pThread )
	{
		if( p_pThread )
		{
			p_pThread->SetRed( val );
		}
	} );
}

void Generator :: SetGreen( uint8_t val )
{
	m_pxColour[1] = val;

	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread * p_pThread )
	{
		if( p_pThread )
		{
			p_pThread->SetGreen( val );
		}
	} );
}

void Generator :: SetBlue( uint8_t val )
{
	m_pxColour[2] = val;

	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread * p_pThread )
	{
		if( p_pThread )
		{
			p_pThread->SetBlue( val );
		}
	} );
}

void Generator :: SwapBuffers()
{
	m_frontBuffer = m_backBuffer;
}

void Generator :: InitialiseStep()
{
	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread * p_pThread )
	{
		if( p_pThread )
		{
			p_pThread->Relaunch();
		}
	} );
}

void Generator :: ClearAllThreads()
{
	std::for_each( m_arraySlaveThreads.begin(), m_arraySlaveThreads.end(), [&]( Thread *& p_pThread )
	{
		Generator::Thread * l_pThread = p_pThread;
		p_pThread = NULL;

		if( l_pThread )
		{
			l_pThread->Wait();
			delete l_pThread;
		}
	} );

	m_arraySlaveThreads.clear();
}

bool Generator :: AllEnded()
{
	bool l_bReturn = true;
	uint32_t l_uiCount = DoGetThreadsCount();

	for( uint32_t i = 0; i < l_uiCount && l_bReturn; i++ )
	{
		l_bReturn &= m_arraySlaveThreads[i] == NULL || m_arraySlaveThreads[i]->IsEnded();
	}

	return l_bReturn;
}

void Generator :: Suspend()
{
	m_bInitialised = false;
	ClearAllThreads();
}

void Generator :: DoCleanup()
{
	ClearAllThreads();

	m_frontBuffer.clear();
	m_backBuffer.clear();
}

Point2i Generator :: _loadImage( String const & p_strImagePath, Image & CU_PARAM_UNUSED( p_image ) )
{
	ImageCollection & l_imgCollection = m_pEngine->GetImageManager();
	ImageSPtr l_pImage = l_imgCollection.find( p_strImagePath );

	if( !l_pImage )
	{
		l_pImage = std::make_shared< Image >( p_strImagePath, p_strImagePath );
		l_imgCollection.insert( p_strImagePath, l_pImage );
	}

	return Point2i( l_pImage->GetWidth(), l_pImage->GetHeight() );
}

//*************************************************************************************************
