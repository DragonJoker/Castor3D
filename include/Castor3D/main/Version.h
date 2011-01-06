#ifndef ___C3D_Version___
#define ___C3D_Version___

#include "../Prerequisites.h"

namespace Castor3D
{
	//! Version management class
	/*!
	Class used to manage versions and versions dependencies for plugins
	It is 
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 14/01/2011
	*/
	class C3D_API Version
	{
	public:
		int m_iMajor;
		int m_iMinor;

	public:
		Version( int p_iMajor=CASTOR_VERSION_MAJOR, int p_iMinor=CASTOR_VERSION_MINOR);
		Version( const Version & p_version);
		~Version();

		Version & operator =( const Version & p_version);

		bool operator ==( const Version & p_version);
		bool operator !=( const Version & p_version);
		bool operator < ( const Version & p_version);
		bool operator > ( const Version & p_version);
		bool operator <=( const Version & p_version);
		bool operator >=( const Version & p_version);
	};

	class VersionException : public Castor::Utils::Exception
	{
	public:
		VersionException( const Version & p_given, const Version & p_expected, const String & p_strFile, const String & p_strFunction, unsigned int p_uiLine);
		virtual ~VersionException() throw() {}
	};

#	define CASTOR_VERSION_EXCEPTION( p_given, p_expected) throw Castor3D::VersionException( p_given, p_expected, __FILE__, __FUNCTION__, __LINE__)
}

#endif