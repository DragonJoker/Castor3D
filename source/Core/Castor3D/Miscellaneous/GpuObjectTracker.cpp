#include "Castor3D/Miscellaneous/GpuObjectTracker.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

#if C3D_TRACE_OBJECTS

namespace c3d
{
	bool GpuObjectTracker::track( void * object, String const & type, String const & file, int line, String & name )
	{
		auto it = std::find_if( m_allocated.begin()
			, m_allocated.end()
			, [object]( ObjectDeclaration const & lookup )
			{
				return object == lookup.m_object;
			} );

		bool result = it == m_allocated.end();

		StringStream ptrStream;
		ptrStream.width( 16 );
		ptrStream.fill( '0' );
		ptrStream << std::hex << std::right << uint64_t( object );
		StringStream typeStream;
		typeStream.width( 20 );
		typeStream << std::left << type;

		if ( result )
		{
			StringStream stream;
			stream << debug::Backtrace();
			++m_id;
			m_allocated.emplace_back( m_id, type, object, file, line, stream.str() );
			StringStream nameStream;
			nameStream << "(" << m_id << ") " << typeStream.str() << " [0x" << ptrStream.str() << "]";
			log::debug << nameStream.str() << std::endl;
			name = nameStream.str();
		}
		else
		{
			StringStream nameStream;
			nameStream << "(" << it->m_id << ") " << typeStream.str() << " [0x" << ptrStream.str() << "]";
			log::debug << "Rereferencing object: " << nameStream.str() << std::endl;
		}

		return result;
	}

	bool GpuObjectTracker::track( Named * object, String const & type, String const & file, int line, String & name )
	{
		return track( reinterpret_cast< void * >( object ), type + cuT( ": " ) + object->getName(), file, line, name );
	}

	bool GpuObjectTracker::untrack( void * object, ObjectDeclaration & declaration )
	{
		auto it = std::find_if( m_allocated.begin()
			, m_allocated.end()
			, [&object]( ObjectDeclaration const & lookup )
			{
				return object == lookup.m_object;
			} );

		bool result = false;
		StringStream ptrStream;
		ptrStream.width( 16 );
		ptrStream.fill( '0' );
		ptrStream << std::hex << std::right << uint64_t( object );

		if ( it != m_allocated.end() )
		{
			StringStream typeStream;
			typeStream.width( 20 );
			typeStream << std::left << it->m_name;
			declaration = *it;
			result = true;
			log::warn << "Released " << typeStream.str() << " [0x" << ptrStream.str() << "]" << std::endl;
			m_allocated.erase( it );
		}
		else
		{
			log::warn << "Untracked [0x" << ptrStream.str() << cuT( "]" ) << std::endl;
		}

		return result;
	}

	void GpuObjectTracker::reportTracked()const
	{
		for ( auto const & decl : m_allocated )
		{
			StringStream stream;
			stream << "Leaked 0x" << std::hex << decl.m_object << std::dec << " (" << decl.m_name << "), from file " << decl.m_file << ", line " << decl.m_line << std::endl;
			stream << decl.m_stack << std::endl;
			log::error << stream.str() << std::endl;
		}
	}
}

#endif
