#include "Castor3D/Miscellaneous/GpuObjectTracker.hpp"

#if C3D_TRACE_OBJECTS

namespace castor3d
{
	bool GpuObjectTracker::track( void * object, std::string const & type, std::string const & file, int line, std::string & name )
	{
		auto it = std::find_if( m_allocated.begin()
			, m_allocated.end()
			, [object]( ObjectDeclaration const & lookup )
			{
				return object == lookup.m_object;
			} );

		bool result = it == m_allocated.end();

		std::stringstream ptrStream;
		ptrStream.width( 16 );
		ptrStream.fill( '0' );
		ptrStream << std::hex << std::right << uint64_t( object );
		std::stringstream typeStream;
		typeStream.width( 20 );
		typeStream << std::left << type;

		if ( result )
		{
			std::stringstream stream;
			stream << castor::Debug::Backtrace();
			m_allocated.push_back( { ++m_id, type, object, file, line, stream.str() } );
			std::stringstream nameStream;
			nameStream << "(" << m_id << ") " << typeStream.str() << " [0x" << ptrStream.str() << "]";
			castor::Logger::logDebug( nameStream );
			name = nameStream.str();
		}
		else
		{
			std::stringstream nameStream;
			nameStream << "(" << it->m_id << ") " << typeStream.str() << " [0x" << ptrStream.str() << "]";
			castor::Logger::logDebug( std::stringstream() << "Rereferencing object: " << nameStream.str() );
		}

		return result;
	}

	bool GpuObjectTracker::track( castor::Named * object, std::string const & type, std::string const & file, int line, std::string & name )
	{
		return track( reinterpret_cast< void * >( object ), type + ": " + castor::string::stringCast< char >( object->getName() ), file, line, name );
	}

	bool GpuObjectTracker::untrack( void * object, ObjectDeclaration & declaration )
	{
		auto it = std::find_if( m_allocated.begin()
			, m_allocated.end()
			, [&object]( ObjectDeclaration lookup )
			{
				return object == lookup.m_object;
			} );

		bool result = false;
		char szName[1024] = { 0 };
		std::stringstream ptrStream;
		ptrStream.width( 16 );
		ptrStream.fill( '0' );
		ptrStream << std::hex << std::right << uint64_t( object );

		if ( it != m_allocated.end() )
		{
			std::stringstream typeStream;
			typeStream.width( 20 );
			typeStream << std::left << it->m_name;
			declaration = *it;
			result = true;
			castor::Logger::logWarning( std::stringstream() << "Released " << typeStream.str() << " [0x" << ptrStream.str() << "]" );
			m_allocated.erase( it );
		}
		else
		{
			castor::Logger::logWarning( std::stringstream() << "Untracked [0x" << ptrStream.str() << cuT( "]" ) );
		}

		return result;
	}

	void GpuObjectTracker::reportTracked()
	{
		for ( auto const & decl : m_allocated )
		{
			std::stringstream stream;
			stream << "Leaked 0x" << std::hex << decl.m_object << std::dec << " (" << decl.m_name << "), from file " << decl.m_file << ", line " << decl.m_line << std::endl;
			stream << castor::string::stringCast< char >( decl.m_stack ) << std::endl;
			castor::Logger::logError( stream.str() );
		}
	}
}

#endif
