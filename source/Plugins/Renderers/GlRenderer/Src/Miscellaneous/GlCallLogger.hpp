/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <iostream>

#define GL_LOG_CALLS 0

namespace gl_renderer
{
	template< typename T >
	struct Stringifier
	{
		static inline std::string toString( T const value )
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}
	};

	template< typename T >
	struct Stringifier< T * >
	{
		static inline std::string toString( T * const value )
		{
			if ( value )
			{
				std::stringstream stream;
				stream << std::hex << size_t( value );
				return stream.str();
			}

			return std::string( "nullptr" );
		}
	};

	template< typename T >
	struct Stringifier< T const * >
	{
		static inline std::string toString( T const * const value )
		{
			if ( value )
			{
				std::stringstream stream;
				stream << std::hex << size_t( value );
				return stream.str();
			}

			return std::string( "nullptr" );
		}
	};
	
	template< typename T >
	inline std::string toString( T const value )
	{
		return Stringifier< T >::toString( value );
	}

	inline std::string toString( std::nullptr_t )
	{
		return std::string( "nullptr" );
	}

	inline std::string toString( GlAccessType value )
	{
		return getName( value );
	}

	inline std::string toString( GlAttachmentPoint value )
	{
		return getName( value );
	}

	inline std::string toString( GlAttachmentType value )
	{
		return getName( value );
	}

	inline std::string toString( GlBaseType value )
	{
		return getName( value );
	}

	inline std::string toString( GlBlendFactor value )
	{
		return getName( value );
	}

	inline std::string toString( GlBlendOp value )
	{
		return getName( value );
	}

	inline std::string toString( GlBufferTarget value )
	{
		return getName( value );
	}

	inline std::string toString( GlClearTarget value )
	{
		return getName( value );
	}

	inline std::string toString( GlClipDepth value )
	{
		return getName( value );
	}

	inline std::string toString( GlClipOrigin value )
	{
		return getName( value );
	}

	inline std::string toString( GlCompareOp value )
	{
		return getName( value );
	}

	inline std::string toString( GlComponentSwizzle value )
	{
		return getName( value );
	}

	inline std::string toString( GlCullMode value )
	{
		return getName( value );
	}

	inline std::string toString( GlFenceWaitFlag value )
	{
		return getName( value );
	}

	inline std::string toString( GlFilter value )
	{
		return getName( value );
	}

	inline std::string toString( GlFrontFace value )
	{
		return getName( value );
	}

	inline std::string toString( GlFormat value )
	{
		return getName( value );
	}

	inline std::string toString( GlFrameBufferTarget value )
	{
		return getName( value );
	}

	inline std::string toString( GlGetParameter value )
	{
		return getName( value );
	}

	inline std::string toString( GlImageAspectFlags value )
	{
		return getName( value );
	}

	inline std::string toString( GlImageLayout value )
	{
		return getName( value );
	}

	inline std::string toString( GlImageTiling value )
	{
		return getName( value );
	}

	inline std::string toString( GlIndexType value )
	{
		return getName( value );
	}

	inline std::string toString( GlInternal value )
	{
		return getName( value );
	}

	inline std::string toString( GlLogicOp value )
	{
		return getName( value );
	}

	inline std::string toString( GlMemoryMapFlags value )
	{
		return getName( value );
	}

	inline std::string toString( GlMemoryPropertyFlags value )
	{
		return getName( value );
	}

	inline std::string toString( GlPolygonMode value )
	{
		return getName( value );
	}

	inline std::string toString( GlPrimitiveTopology value )
	{
		return getName( value );
	}

	inline std::string toString( GlQueryResultFlags value )
	{
		return getName( value );
	}

	inline std::string toString( GlQueryType value )
	{
		return getName( value );
	}

	inline std::string toString( GlSamplerParameter value )
	{
		return getName( value );
	}

	inline std::string toString( GlShaderInfo value )
	{
		return getName( value );
	}

	inline std::string toString( GlShaderStageFlags value )
	{
		return getName( value );
	}

	inline std::string toString( GlStencilOp value )
	{
		return getName( value );
	}

	inline std::string toString( GlSwizzle value )
	{
		return getName( value );
	}

	inline std::string toString( GlTexLevelParameter value )
	{
		return getName( value );
	}

	inline std::string toString( GlTexParameter value )
	{
		return getName( value );
	}

	inline std::string toString( GlTextureType value )
	{
		return getName( value );
	}

	inline std::string toString( GlTextureUnit value )
	{
		return getName( value );
	}

	inline std::string toString( GlTweak value )
	{
		return getName( value );
	}

	inline std::string toString( GlType value )
	{
		return getName( value );
	}

	inline std::string toString( GlWrapMode value )
	{
		return getName( value );
	}

	template< typename ... ParamsT >
	struct GlParamLoggerRec;

	template<>
	struct GlParamLoggerRec<>
	{
		static inline void log()
		{
		}
	};

	template< typename ParamT >
	struct GlParamLoggerRec< ParamT >
	{
		static inline void log( ParamT const & last )
		{
			std::clog << toString( last );
		}
	};

	template< typename ParamT, typename ... ParamsT >
	struct GlParamLoggerRec< ParamT, ParamsT... >
	{
		static inline void log( ParamT const & param
			, ParamsT ... params )
		{
			std::clog << toString( param ) << ", ";
			GlParamLoggerRec< ParamsT... >::log( std::forward< ParamsT >( params )... );
		}
	};

	template< typename ... ParamsT >
	void logParams( ParamsT ... params )
	{
		std::clog << "(";
		GlParamLoggerRec< ParamsT... >::log( std::forward< ParamsT >( params )... );
		std::clog << ")";
	}

	template< typename FuncT, typename ... ParamsT >
	struct GlFuncCaller
	{
		static inline auto call( FuncT function
			, char const * const name
			, ParamsT ... params )
		{
			std::clog << name;
			logParams( std::forward< ParamsT >( params )... );
			std::clog << std::endl;
			return function( std::forward< ParamsT >( params )... );
		}
	};

	template< typename FuncT >
	struct GlFuncCaller< FuncT, void >
	{
		static inline void call( FuncT function
			, char const * const name )
		{
			std::clog << name << "()" << std::endl;
			function();
		}
	};

	template< typename FuncT, typename ... ParamsT >
	inline auto executeFunction( FuncT function
		, char const * const name
		, ParamsT ... params )
	{
		return GlFuncCaller< FuncT, ParamsT... >::call( function
			, name
			, std::forward< ParamsT >( params )... );
	}

#if GL_LOG_CALLS
#	define glLogCall( Name, ... )\
	executeFunction( Name, #Name, __VA_ARGS__ )
#	define glLogCommand( Name )\
	std::cout << "Command: " << Name << std::endl
#elif defined( NDEBUG )
#	define glLogCall( Name, ... )\
	( Name( __VA_ARGS__ ) )
#	define glLogCommand( Name )
#	else
#	define glLogCall( Name, ... )\
	( Name( __VA_ARGS__ ) );\
	glCheckError( #Name )
#	define glLogCommand( Name )
#endif
}
