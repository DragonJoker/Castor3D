#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Generator.hpp"

#ifdef _WIN32
#	include <Windows.h>
#endif

using namespace Castor3D;

int Castor3D :: GetCPUCount()
{
	int l_iReturn = 1;

#ifndef _WIN32
#	ifdef _GNUG
	FILE * fp;
	char res[128];
	fp = popen("/bin/cat /proc/cpuinfo |grep -c '^processor'","r");
	fread(res, 1, sizeof(res)-1, fp);
	fclose(fp);
	std::cout << "number of core: " << res[0] << endl;
#	endif
#else
	SYSTEM_INFO sysinfo;
	GetSystemInfo( & sysinfo);

	l_iReturn = sysinfo.dwNumberOfProcessors;
#endif

	return l_iReturn;
}

//*************************************************************************************************

Generator::Thread :: Thread( Generator * p_pParent, size_t p_uiIndex, int iWidth, int iTop, int iBottom, int iTotalHeight, UbPixel const & p_pxColour)
	:	m_pParent( p_pParent)
	,	m_uiIndex( p_uiIndex)
	,	m_iWidth( iWidth)
	,	m_iBottom( iBottom)
	,	m_iTop( iTop)
	,	m_iHeight( iTotalHeight)
	,	m_pxColour( p_pxColour)
	,	m_bEnded( true)
	,	m_bLaunched( false)
	,	m_bStopped( false)
{
}

Generator::Thread :: ~Thread()
{
	Kill();
}

void Generator::Thread :: Run()
{
	CreateThread( StEntry, this);
}

void Generator::Thread :: Wait()
{
	Stop();
	MultiThreading::Thread::Wait( 1000);
}

unsigned int Generator::Thread :: StEntry( void * p_pThis)
{
	return static_cast<Generator::Thread *>( p_pThis)->Entry();
}

int Generator::Thread :: Entry()
{
	while ( ! IsStopped())
	{
		if (m_bEnded && m_bLaunched && ! IsStopped())
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			m_bEnded = false;
			Step();
			m_bEnded = true;
			m_bLaunched = false;
		}

		Sleep( 1);
	}

	return 0;
}

//*************************************************************************************************

Generator :: Generator( int p_width, int p_height)
	:	m_iWidth			( p_width)
	,	m_iHeight			( p_height)
	,	m_ullStep			( 0)
	,	m_bInitialised		( false)
	,	m_bEnded			( true)
	,	m_frontBuffer		( Point<size_t, 2>( p_width, p_height))
	,	m_backBuffer		( Point<size_t, 2>( p_width, p_height))
{
	unsigned char l_tmp[] = { 255, 255, 255, 255 };
	m_pxColour.set<ePIXEL_FORMAT_A8R8G8B8>( l_tmp);
	Point2i l_size( p_width, p_height);
	m_uiThreadCount = GetCPUCount() * 2;
}

Generator :: ~Generator()
{
	_cleanup();
}

bool Generator :: Step()
{
	bool l_bReturn = false;

	if (m_bInitialised)
	{
		if (m_arraySlaveThreads.size() > 0)
		{
			if (m_bEnded)
			{
				m_bEnded = false;
				InitialiseStep();
			}
			else if (AllEnded())
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

void Generator :: SetRed( unsigned char val)
{
	m_pxColour[0] = val;

	for (size_t i = 0 ; i < m_arraySlaveThreads.size() ; i++)
	{
		if (m_arraySlaveThreads[i] != NULL)
		{
			m_arraySlaveThreads[i]->SetRed( val);
		}
	}
}

void Generator :: SetGreen( unsigned char val)
{
	m_pxColour[1] = val;

	for (size_t i = 0 ; i < m_arraySlaveThreads.size() ; i++)
	{
		if (m_arraySlaveThreads[i] != NULL)
		{
			m_arraySlaveThreads[i]->SetGreen( val);
		}
	}
}

void Generator :: SetBlue( unsigned char val)
{
	m_pxColour[2] = val;

	for (size_t i = 0 ; i < m_arraySlaveThreads.size() ; i++)
	{
		if (m_arraySlaveThreads[i] != NULL)
		{
			m_arraySlaveThreads[i]->SetBlue( val);
		}
	}
}

void Generator :: SwapBuffers()
{
	m_frontBuffer = m_backBuffer;
}

void Generator :: InitialiseStep()
{
	for (size_t i = 0 ; i < m_arraySlaveThreads.size() ; i++)
	{
		if (m_arraySlaveThreads[i] != NULL)
		{
			m_arraySlaveThreads[i]->Relaunch();
		}
	}
}

void Generator :: ClearAllThreads()
{
	for (size_t i = 0 ; i < m_arraySlaveThreads.size() ; i++)
	{
		Generator::Thread * l_pThread = m_arraySlaveThreads[i];
		m_arraySlaveThreads[i] = NULL;

		if (l_pThread != NULL)
		{
			l_pThread->Wait();
			delete l_pThread;
		}
	}

	m_arraySlaveThreads.clear();
}

bool Generator :: AllEnded()
{
	bool l_bReturn = true;

	for (size_t i = 0 ; i < m_arraySlaveThreads.size() && l_bReturn ; i++)
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

void Generator :: _cleanup()
{
	ClearAllThreads();

	m_frontBuffer.clear();
	m_backBuffer.clear();
}

Point2i Generator :: _loadImage( String const & p_strImagePath, Image & p_image)
{
	Collection<Image, String> l_imgCollection;
	ImagePtr l_pImage = l_imgCollection.GetElement( p_strImagePath);
	if ( ! l_pImage)
	{
		l_pImage = ImagePtr( new Image( p_strImagePath));
		l_imgCollection.AddElement( p_strImagePath, l_pImage);
	}
	Loader<Image>::Load( * l_pImage, p_strImagePath);
	return Point2i( l_pImage->GetWidth(), l_pImage->GetHeight());
}

//*************************************************************************************************
