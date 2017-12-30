/*
See LICENSE file in root folder
*/
#ifndef ___C3DGL_GlGetFunction___
#define ___C3DGL_GlGetFunction___

#include "Common/OpenGl.hpp"

namespace GlRender
{
	namespace gl_api
	{
#if defined( CASTOR_PLATFORM_WINDOWS )
		using GlProc = INT_PTR( WINAPI * )( );
#elif defined( CASTOR_PLATFORM_LINUX )
		using GlProc = void( * )();
#endif

		GlProc getProcAddress( castor::String const & p_name );

		template< typename Func >
		bool getFunction( castor::String const & p_name, Func & p_func )
		{
			p_func = reinterpret_cast< Func >( getProcAddress( p_name ) );
			return p_func != nullptr;
		}

		template< typename Ret, typename ... Arguments >
		bool getFunction( castor::String const & p_name, GlFunction< Ret, Arguments... > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( Arguments... );
			PFNType pfnResult = nullptr;

			if ( getFunction( p_name, pfnResult ) )
			{
				p_func = pfnResult;
			}

			return pfnResult != nullptr;
		}

		template< typename Ret, typename ... Arguments >
		bool getFunction( castor::String const & p_name, std::function< Ret( Arguments... ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( Arguments... );
			PFNType pfnResult = nullptr;

			if ( getFunction( p_name, pfnResult ) )
			{
				p_func = pfnResult;
			}

			return pfnResult != nullptr;
		}

		template< typename T >
		inline void getFunction( T & p_function, castor::String const & p_name, castor::String const & p_extension )
		{
			if ( !gl_api::getFunction( p_name, p_function ) )
			{
				if ( !gl_api::getFunction( p_name + p_extension, p_function ) )
				{
					castor::Logger::logWarning( cuT( "Unable to retrieve function " ) + p_name );
				}
			}
		}
	}
}

#endif
