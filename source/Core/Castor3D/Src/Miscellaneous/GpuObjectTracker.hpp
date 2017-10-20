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

#include <list>

#include <Design/Named.hpp>

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/12/2016
	\version	0.9.0
	\~english
	\brief		Tracks objects allocated on GPU.
	\~french
	\brief		Trace les objets alloués sur le GPU.
	*/
	class GpuObjectTracker
	{
	public:
		struct ObjectDeclaration
		{
			uint32_t m_id;
			std::string m_name;
			void * m_object;
			std::string m_file;
			int m_line;
			std::string m_stack;
		};

	public:
		C3D_API bool track( void * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name );
		C3D_API bool track( castor::Named * p_object, std::string const & p_type, std::string const & p_file, int p_line, std::string & p_name );
		C3D_API bool untrack( void * p_object, ObjectDeclaration & p_declaration );
		C3D_API void reportTracked();

	private:
		uint32_t m_id = 0;
		std::list< ObjectDeclaration > m_allocated;
	};
}

#endif

#endif
