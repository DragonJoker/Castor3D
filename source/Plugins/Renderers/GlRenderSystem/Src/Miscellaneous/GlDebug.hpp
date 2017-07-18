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
#ifndef ___C3DGL_GlDebug_H___
#define ___C3DGL_GlDebug_H___

#include "Common/GlBindable.hpp"

#include <Miscellaneous/Debug.hpp>

#include <unordered_set>
#include <unordered_map>

#if !defined( CALLBACK )
#	if defined( CASTOR_PLATFORM_WINDOWS )
#		define CALLBACK __stdcall
#	else
#		define CALLBACK
#	endif
#endif

namespace GlRender
{
	class GlDebug
		: public Holder
	{
		using PFNGLDEBUGPROC = void ( CALLBACK * )( uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int length, const char * message, void * userParam );
		using PFNGLDEBUGAMDPROC = void ( CALLBACK * )( uint32_t id, uint32_t category, uint32_t severity, int length, const char * message, void * userParam );

		struct Binding
		{
			Binding()
				: m_name{ 0u }
			{
			}

			Binding( uint32_t p_name )
				: m_name{ p_name }
			{
				Castor::StringStream stream;
				stream << Castor::Debug::Backtrace{};
				m_stack = stream.str();
			}

			uint32_t m_name;
			Castor::String m_stack;
		};

		struct TextureUnitState
		{
			Binding m_texture;
			Binding m_sampler;
		};

	public:
		GlDebug( GlRenderSystem & p_renderSystem );
		~GlDebug();
		void Initialise();
		void Cleanup();
		bool GlCheckError( std::string const & p_strText )const;
		bool GlCheckError( std::wstring const & p_strText )const;
		void FilterMessage( uint32_t p_id );
		bool IsFiltered( uint32_t p_id )const;
		void BindTexture( uint32_t p_name, uint32_t p_index )const;
		void BindSampler( uint32_t p_name, uint32_t p_index )const;
		void CheckTextureUnits()const;

		static void CALLBACK StDebugLog( GlDebugSource source
			, GlDebugType type
			, uint32_t id
			, GlDebugSeverity severity
			, int length
			, const char * message
			, void * userParam );
		static void CALLBACK StDebugLogAMD( uint32_t id
			, GlDebugCategory category
			, GlDebugSeverity severity
			, int length
			, const char * message
			, void * userParam );

#if !defined( NDEBUG )

		C3D_Gl_API bool Track( void * p_object
			, std::string const & p_name
			, std::string const & p_file
			, int line )const;
		C3D_Gl_API bool UnTrack( void * p_object )const;

		template< typename CreationFunction, typename DestructionFunction >
		inline bool Track( Object< CreationFunction, DestructionFunction > * p_object
			, std::string const & p_name
			, std::string const & p_file
			, int p_line )const
		{
			return Track( reinterpret_cast< void * >( p_object )
				, p_name + cuT( " (OpenGL Name: " ) + Castor::string::to_string( p_object->GetGlName() ) + cuT( ")" )
				, p_file
				, p_line );
		}

		template< typename CreateFunction
			, typename DestroyFunction
			, typename BindFunction
			, typename UnbindFunction >
		inline bool Track( Bindable< CreateFunction, DestroyFunction, BindFunction, UnbindFunction > * p_object
			, std::string const & p_name
			, std::string const & p_file
			, int p_line )const
		{
			return Track( reinterpret_cast< void * >( p_object )
				, p_name + cuT( " (OpenGL Name: " ) + Castor::string::to_string( p_object->GetGlName() ) + cuT( ")" )
				, p_file
				, p_line );
		}

#endif

	private:
		bool DoGlCheckError( Castor::String const & p_strText )const;
		void DebugLog( GlDebugSource source
			, GlDebugType type
			, uint32_t id
			, GlDebugSeverity severity
			, int length
			, const char * message )const;
		void DebugLogAMD( uint32_t id
			, GlDebugCategory category
			, GlDebugSeverity severity
			, int length
			, const char * message )const;
		void DoUpdateTextureUnits()const;

	private:
		GlRenderSystem & m_renderSystem;
		std::function< uint32_t() > m_pfnGetError;
		std::function< void( PFNGLDEBUGPROC callback, void * userParam ) > m_pfnDebugMessageCallback;
		std::function< void( PFNGLDEBUGAMDPROC callback, void * userParam ) > m_pfnDebugMessageCallbackAMD;
		std::unordered_set< uint32_t > m_filteredOut;
		mutable std::unordered_map< uint32_t, TextureUnitState > m_textureUnits;
	};
}

#endif
