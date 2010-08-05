#include "PrecompiledHeader.h"

#include "main/Module_Main.h"

#include "main/Plugin.h"
#include "Log.h"
#ifdef WIN32
#define dlerror() GetLastError()
#endif

using namespace Castor3D;


Castor3D::Plugin :: Plugin( const String & p_fileName)
	:	m_pDLLRefCount			( NULL),
		m_pfnGetRequiredVersion	( 0),
		m_pfnRegisterPlugin		( 0),
		m_library				( new DynamicLibrary)
{

	Cout( C3D_T( "Plugin name [") << p_fileName << C3D_T( "]") << std::endl);

	if ( ! m_library->Open( p_fileName))
	{
		Cerr( C3D_T( "Error encountered while loading plugin : ") << std::hex << dlerror() << std::endl);
		throw;
	}

	m_pfnGetRequiredVersion = reinterpret_cast <fnGetRequiredVersion *> ( m_library->GetFunction( "GetRequiredVersion"));

	if (m_pfnGetRequiredVersion == NULL)
	{
		Cerr( C3D_T( "Error encountered while loading plugin factory : ") << dlerror() << std::endl);
		throw;
	}

	m_pfnRegisterPlugin = reinterpret_cast <fnRegisterPlugin *> ( m_library->GetFunction( "RegisterPlugin"));

	if (m_pfnRegisterPlugin == NULL)
	{
		Cerr( C3D_T( "Error encountered while loading plugin destroyer : ") << dlerror() << std::endl);
		throw;
	}

	m_pDLLRefCount = new size_t( 1);
}



Castor3D::Plugin :: Plugin( const Castor3D::Plugin & p_plugin)
	:	m_pDLLRefCount			( p_plugin.m_pDLLRefCount),
		m_pfnGetRequiredVersion	( p_plugin.m_pfnGetRequiredVersion),
		m_pfnRegisterPlugin		( p_plugin.m_pfnRegisterPlugin)
{
	++*m_pDLLRefCount;
}



Castor3D::Plugin & Castor3D::Plugin :: operator =( const Castor3D::Plugin & p_plugin)
{
	m_pfnGetRequiredVersion = p_plugin.m_pfnGetRequiredVersion;
	m_pfnRegisterPlugin = p_plugin.m_pfnRegisterPlugin;
	m_pDLLRefCount = p_plugin.m_pDLLRefCount;
	++*m_pDLLRefCount;
	return *this;
}



Castor3D::Plugin :: ~Plugin()
{
	if (--*m_pDLLRefCount == 0)
	{
		delete m_pDLLRefCount;
//		delete m_library; <= Hang on when trying to delete this...
	}
}


