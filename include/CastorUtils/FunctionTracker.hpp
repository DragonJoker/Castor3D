#ifndef ___CU_FunctionTracker___
#define ___CU_FunctionTracker___

#include <iostream>

namespace Castor
{ namespace Utils
{
	/*!
	Helper class, used to track entering and leaving a function
	Call the macro CASTOR_TRACK at the beginning of a function to have a console output when entering or leaving that function
	As it's dependant from scope, can also work to print scope entering and leaving
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class FunctionTracker
	{
		char const			*	m_szFile;
		char const			*	m_szFunction;
		unsigned int const		m_uiLine;

	public:
		FunctionTracker( char const * p_szFunction, char const * p_szFile, unsigned int p_uiLine)
			:	m_szFunction( p_szFunction)
			,	m_szFile( p_szFile)
			,	m_uiLine( p_uiLine)
		{
			std::cout << "Entered Function : " << m_szFunction << " in " << m_szFile << ", line " << m_uiLine << std::endl; 
		}
		~FunctionTracker()
		{
			std::cout << "Exited Function : " << m_szFunction << " in " << m_szFile << ", line " << m_uiLine << std::endl; 
		}
	};
}
}

#ifndef CASTOR_USE_TRACK
#	ifndef NDEBUG
#		define CASTOR_USE_TRACK		1
#	else
#		define CASTOR_USE_TRACK		0
#	endif
#endif

#ifndef CASTOR_USE_LOG_TRACK
#	define CASTOR_USE_LOG_TRACK	0
#endif

#if CASTOR_USE_TRACK
#	if CASTOR_USE_LOG_TRACK
#		define CASTOR_TRACK Logger::Log( eLOG_TYPE_MESSAGE) << cuT( "Entered Function : ") << __FUNCTION__ << cuT( " in ") << __FILE__ << cuT( ", line ") << __LINE__ << cuT( "\n")
#	else
#		define CASTOR_TRACK Castor::Utils::FunctionTracker l_tracker##__LINE__( __FUNCTION__, __FILE__, __LINE__)
#	endif
#else
#	define CASTOR_TRACK
#endif

#endif
