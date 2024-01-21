/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuObjectTracker_H___
#define ___C3D_GpuObjectTracker_H___

#ifndef C3D_TRACE_OBJECTS
#	ifndef NDEBUG
#		define C3D_TRACE_OBJECTS 1
#	else
#		define C3D_TRACE_OBJECTS 0
#	endif
#endif

#if C3D_TRACE_OBJECTS

#include "MiscellaneousModule.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <list>

namespace castor3d
{
	class GpuObjectTracker
	{
	public:
		struct ObjectDeclaration
		{
			ObjectDeclaration( uint32_t id
				, std::string name
				, void * object
				, std::string file
				, int line
				, std::string stack )
				: m_id{ id }
				, m_name{ std::move( name ) }
				, m_object{ object }
				, m_file{ std::move( file ) }
				, m_line{ line }
				, m_stack{ std::move( stack ) }
			{
			}

			uint32_t m_id;
			std::string m_name;
			void * m_object;
			std::string m_file;
			int m_line;
			std::string m_stack;
		};

	public:
		C3D_API bool track( void * object, std::string const & type, std::string const & file, int line, std::string & name );
		C3D_API bool track( castor::Named * object, std::string const & type, std::string const & file, int line, std::string & name );
		C3D_API bool untrack( void * object, ObjectDeclaration & declaration );
		C3D_API void reportTracked()const;

	private:
		uint32_t m_id = 0;
		std::list< ObjectDeclaration > m_allocated;
	};
}

#endif

#endif
