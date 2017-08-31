/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
