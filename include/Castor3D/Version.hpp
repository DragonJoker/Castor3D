#ifndef ___C3D_Version___
#define ___C3D_Version___

#include "Prerequisites.hpp"

namespace Castor3D
{
	//! Version management class C3D_API
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
		int m_iAnnex;
		int m_iCorrect;
		int m_iPoint;

	public:
		Version( int p_iMajor=CASTOR_VERSION_MAJOR, int p_iAnnex=CASTOR_VERSION_ANNEX, int p_iCorrect=CASTOR_VERSION_CORRECT, int p_iPoint=CASTOR_VERSION_POINT);
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

	std::basic_ostream<xchar> & operator <<( std::basic_ostream<xchar> & p_stream, const Version & p_version);

	class C3D_API VersionException : public Castor::Utils::Exception
	{
	public:
		VersionException( const Version & p_given, const Version & p_expected, String const & p_strFile, String const & p_strFunction, unsigned int p_uiLine);
		virtual ~VersionException() throw() {}
	};

#	define CASTOR_VERSION_EXCEPTION( p_given, p_expected) throw Castor3D::VersionException( p_given, p_expected, __FILE__, __FUNCTION__, __LINE__)
}

#endif