#ifndef ___CU_FunctionTracker___
#define ___CU_FunctionTracker___

#include <iostream>

namespace Castor
{ namespace Utils
{
	class FunctionTracker
	{
		const char * m_szFile;
		const char * m_szFunction;
		const unsigned int m_uiLine;

	public:
		FunctionTracker( const char * p_szFunction, const char * p_szFile, unsigned int p_uiLine)
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

#define CASTOR_TRACK Castor::Utils::FunctionTracker l_tracker##__LINE__( __FUNCTION__, __FILE__, __LINE__)

#endif
