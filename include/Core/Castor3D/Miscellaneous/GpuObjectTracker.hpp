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
				, castor::String name
				, void * object
				, castor::String file
				, int line
				, castor::String stack )
				: m_id{ id }
				, m_name{ castor::move( name ) }
				, m_object{ object }
				, m_file{ castor::move( file ) }
				, m_line{ line }
				, m_stack{ castor::move( stack ) }
			{
			}

			uint32_t m_id;
			castor::String m_name;
			void * m_object;
			castor::String m_file;
			int m_line;
			castor::String m_stack;
		};

	public:
		C3D_API bool track( void * object, castor::String const & type, castor::String const & file, int line, castor::String & name );
		C3D_API bool track( castor::Named * object, castor::String const & type, castor::String const & file, int line, castor::String & name );
		C3D_API bool untrack( void * object, ObjectDeclaration & declaration );
		C3D_API void reportTracked()const;

	private:
		uint32_t m_id = 0;
		castor::List< ObjectDeclaration > m_allocated;
	};
}

#endif

#endif
