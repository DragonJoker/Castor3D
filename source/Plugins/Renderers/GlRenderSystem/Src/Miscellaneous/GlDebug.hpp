/*
See LICENSE file in root folder
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

			explicit Binding( uint32_t p_name )
				: m_name{ p_name }
			{
				castor::StringStream stream;
				stream << castor::Debug::Backtrace{};
				m_stack = stream.str();
			}

			uint32_t m_name;
			castor::String m_stack;
		};

		struct TextureUnitState
		{
			Binding m_texture;
			Binding m_sampler;
		};

	public:
		explicit GlDebug( GlRenderSystem & renderSystem );
		~GlDebug();
		void initialise();
		void cleanup();
		bool checkError( std::string const & p_strText )const;
		bool checkError( std::wstring const & p_strText )const;
		void filterMessage( uint32_t p_id );
		bool isFiltered( uint32_t p_id )const;
		void BindTexture( uint32_t p_name, uint32_t p_index )const;
		void bindSampler( uint32_t p_name, uint32_t p_index )const;
		void checkTextureUnits()const;

		static void CALLBACK debugLog( GlDebugSource source
			, GlDebugType type
			, uint32_t id
			, GlDebugSeverity severity
			, int length
			, const char * message
			, void * userParam );
		static void CALLBACK debugLogAMD( uint32_t id
			, GlDebugCategory category
			, GlDebugSeverity severity
			, int length
			, const char * message
			, void * userParam );

#if !defined( NDEBUG )

		C3D_Gl_API bool track( void * p_object
			, std::string const & p_name
			, std::string const & p_file
			, int line )const;
		C3D_Gl_API bool untrack( void * p_object )const;

		template< typename CreationFunction, typename DestructionFunction >
		inline bool track( Object< CreationFunction, DestructionFunction > * p_object
			, std::string const & p_name
			, std::string const & p_file
			, int p_line )const
		{
			return track( reinterpret_cast< void * >( p_object )
				, p_name + cuT( " (OpenGL Name: " ) + castor::string::toString( p_object->getGlName() ) + cuT( ")" )
				, p_file
				, p_line );
		}

		template< typename CreateFunction
			, typename DestroyFunction
			, typename BindFunction
			, typename UnbindFunction >
		inline bool track( Bindable< CreateFunction, DestroyFunction, BindFunction, UnbindFunction > * p_object
			, std::string const & p_name
			, std::string const & p_file
			, int p_line )const
		{
			return track( reinterpret_cast< void * >( p_object )
				, p_name + cuT( " (OpenGL Name: " ) + castor::string::toString( p_object->getGlName() ) + cuT( ")" )
				, p_file
				, p_line );
		}

#endif

	private:
		bool doGlCheckError( castor::String const & p_strText )const;
		void doDebugLog( GlDebugSource source
			, GlDebugType type
			, uint32_t id
			, GlDebugSeverity severity
			, int length
			, const char * message )const;
		void doDebugLogAMD( uint32_t id
			, GlDebugCategory category
			, GlDebugSeverity severity
			, int length
			, const char * message )const;
		void doUpdateTextureUnits()const;

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
