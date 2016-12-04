#include "GpuObjectTracker.hpp"

using namespace Castor;

#if C3D_TRACE_OBJECTS

namespace Castor3D
{
	bool GpuObjectTracker::Track( void * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name )
	{
		auto l_it = std::find_if( m_allocated.begin(), m_allocated.end(), [p_object]( ObjectDeclaration const & l_object )
		{
			return p_object == l_object.m_object;
		} );

		bool l_return = l_it == m_allocated.end();

		std::stringstream l_ptr;
		l_ptr.width( 16 );
		l_ptr.fill( '0' );
		l_ptr << std::hex << std::right << uint64_t( p_object );
		std::stringstream l_type;
		l_type.width( 20 );
		l_type << std::left << p_type;

		if ( l_return )
		{
			std::stringstream l_stream;
			l_stream << Castor::Debug::Backtrace();
			m_allocated.push_back( { ++m_id, p_type, p_object, p_file, p_line, l_stream.str() } );
			std::stringstream l_name;
			l_name << "(" << m_id << ") " << l_type.str() << " [0x" << l_ptr.str() << "]";
			Castor::Logger::LogDebug( l_name );
			p_name = l_name.str();
		}
		else
		{
			std::stringstream l_name;
			l_name << "(" << l_it->m_id << ") " << l_type.str() << " [0x" << l_ptr.str() << "]";
			Castor::Logger::LogDebug( std::stringstream() << "Rereferencing object: " << l_name.str() );
		}

		return l_return;
	}

	bool GpuObjectTracker::Track( Castor::Named * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name )
	{
		return Track( reinterpret_cast< void * >( p_object ), p_type + ": " + Castor::string::string_cast< char >( p_object->GetName() ), p_file, p_line, p_name );
	}

	bool GpuObjectTracker::Untrack( void * p_object, ObjectDeclaration & p_declaration )
	{
		auto l_it = std::find_if( m_allocated.begin(), m_allocated.end(), [&p_object]( ObjectDeclaration p_decl )
		{
			return p_object == p_decl.m_object;
		} );

		bool l_return = false;
		char l_szName[1024] = { 0 };
		std::stringstream l_ptr;
		l_ptr.width( 16 );
		l_ptr.fill( '0' );
		l_ptr << std::hex << std::right << uint64_t( p_object );

		if ( l_it != m_allocated.end() )
		{
			std::stringstream l_type;
			l_type.width( 20 );
			l_type << std::left << l_it->m_name;
			p_declaration = *l_it;
			l_return = true;
			Castor::Logger::LogWarning( std::stringstream() << "Released " << l_type.str() << " [0x" << l_ptr.str() << "]" );
			m_allocated.erase( l_it );
		}
		else
		{
			Castor::Logger::LogWarning( std::stringstream() << "Untracked [0x" << l_ptr.str() << cuT( "]" ) );
		}

		return l_return;
	}

	void GpuObjectTracker::ReportTracked()
	{
		for ( auto const & l_decl : m_allocated )
		{
			std::stringstream l_stream;
			l_stream << "Leaked 0x" << std::hex << l_decl.m_object << std::dec << " (" << l_decl.m_name << "), from file " << l_decl.m_file << ", line " << l_decl.m_line << std::endl;
			l_stream << string::string_cast< char >( l_decl.m_stack ) << std::endl;
			Castor::Logger::LogError( l_stream.str() );
		}
	}
}

#endif
