/**
*\file
*	RenderLibPrerequisites.h
*\author
*	Sylvain Doremus
*/
#pragma once

#include <RendererPrerequisites.hpp>

#include "Miscellaneous/OpenGLFunctions.hpp"
#include "GlPixelFormat.hpp"

#include "Enum/GlAccessFlag.hpp"
#include "Enum/GlAttributeFormat.hpp"
#include "Enum/GlBlendFactor.hpp"
#include "Enum/GlBlendOp.hpp"
#include "Enum/GlBorderColour.hpp"
#include "Enum/GlBufferTarget.hpp"
#include "Enum/GlColourComponentFlag.hpp"
#include "Enum/GlCompareOp.hpp"
#include "Enum/GlCullModeFlag.hpp"
#include "Enum/GlFilter.hpp"
#include "Enum/GlFrontFace.hpp"
#include "Enum/GlImageAspectFlag.hpp"
#include "Enum/GlImageLayout.hpp"
#include "Enum/GlImageTiling.hpp"
#include "Enum/GlIndexType.hpp"
#include "Enum/GlLogicOp.hpp"
#include "Enum/GlMemoryMapFlag.hpp"
#include "Enum/GlMemoryPropertyFlag.hpp"
#include "Enum/GlMipmapMode.hpp"
#include "Enum/GlPolygonMode.hpp"
#include "Enum/GlPrimitiveTopology.hpp"
#include "Enum/GlQueryResultFlag.hpp"
#include "Enum/GlQueryType.hpp"
#include "Enum/GlSampleCountFlag.hpp"
#include "Enum/GlShaderStageFlag.hpp"
#include "Enum/GlStencilOp.hpp"
#include "Enum/GlTextureType.hpp"
#include "Enum/GlWrapMode.hpp"

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
#else
#	define glLogCall( Name, ... )\
	( Name( __VA_ARGS__ ) )
#endif

	class Buffer;
	class BufferView;
	class CommandBase;
	class Context;
	class DescriptorSet;
	class Device;
	class FrameBuffer;
	class GeometryBuffers;
	class Pipeline;
	class PipelineLayout;
	class QueryPool;
	class RenderPass;
	class ShaderProgram;
	class Texture;
	class TextureView;

	using ContextPtr = std::unique_ptr< Context >;
	using CommandPtr = std::unique_ptr< CommandBase >;

	using CommandArray = std::vector< CommandPtr >;
}
