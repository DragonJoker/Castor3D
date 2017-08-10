#include "GpuObjectTracker.hpp"

#if C3D_TRACE_OBJECTS

namespace castor3d
{
	bool GpuObjectTracker::track( void * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name )
	{
		auto it = std::find_if( m_allocated.begin(), m_allocated.end(), [p_object]( ObjectDeclaration const & object )
		{
			return p_object == object.m_object;
		} );

		bool result = it == m_allocated.end();

		std::stringstream ptr;
		ptr.width( 16 );
		ptr.fill( '0' );
		ptr << std::hex << std::right << uint64_t( p_object );
		std::stringstream type;
		type.width( 20 );
		type << std::left << p_type;

		if ( result )
		{
			std::stringstream stream;
			stream << castor::Debug::Backtrace();
			m_allocated.push_back( { ++m_id, p_type, p_object, p_file, p_line, stream.str() } );
			std::stringstream name;
			name << "(" << m_id << ") " << type.str() << " [0x" << ptr.str() << "]";
			castor::Logger::logDebug( name );
			p_name = name.str();
		}
		else
		{
			std::stringstream name;
			name << "(" << it->m_id << ") " << type.str() << " [0x" << ptr.str() << "]";
			castor::Logger::logDebug( std::stringstream() << "Rereferencing object: " << name.str() );
		}

		return result;
	}

	bool GpuObjectTracker::track( castor::Named * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name )
	{
		return track( reinterpret_cast< void * >( p_object ), p_type + ": " + castor::string::stringCast< char >( p_object->getName() ), p_file, p_line, p_name );
	}

	bool GpuObjectTracker::untrack( void * p_object, ObjectDeclaration & p_declaration )
	{
		auto it = std::find_if( m_allocated.begin(), m_allocated.end(), [&p_object]( ObjectDeclaration p_decl )
		{
			return p_object == p_decl.m_object;
		} );

		bool result = false;
		char szName[1024] = { 0 };
		std::stringstream ptr;
		ptr.width( 16 );
		ptr.fill( '0' );
		ptr << std::hex << std::right << uint64_t( p_object );

		if ( it != m_allocated.end() )
		{
			std::stringstream type;
			type.width( 20 );
			type << std::left << it->m_name;
			p_declaration = *it;
			result = true;
			castor::Logger::logWarning( std::stringstream() << "Released " << type.str() << " [0x" << ptr.str() << "]" );
			m_allocated.erase( it );
		}
		else
		{
			castor::Logger::logWarning( std::stringstream() << "Untracked [0x" << ptr.str() << cuT( "]" ) );
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
