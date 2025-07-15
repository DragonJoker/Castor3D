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

namespace c3d
{
	class GpuObjectTracker
	{
	public:
		struct ObjectDeclaration
		{
			ObjectDeclaration( uint32_t id
				, String name
				, void * object
				, String file
				, int line
				, String stack )
				: m_id{ id }
				, m_name{ c3d::move( name ) }
				, m_object{ object }
				, m_file{ c3d::move( file ) }
				, m_line{ line }
				, m_stack{ c3d::move( stack ) }
			{
			}

			uint32_t m_id;
			String m_name;
			void * m_object;
			String m_file;
			int m_line;
			String m_stack;
		};

	public:
		C3D_API bool track( void * object, String const & type, String const & file, int line, String & name );
		C3D_API bool track( Named * object, String const & type, String const & file, int line, String & name );
		C3D_API bool untrack( void * object, ObjectDeclaration & declaration );
		C3D_API void reportTracked()const;

	private:
		uint32_t m_id = 0;
		List< ObjectDeclaration > m_allocated;
	};
}

#endif

#endif
