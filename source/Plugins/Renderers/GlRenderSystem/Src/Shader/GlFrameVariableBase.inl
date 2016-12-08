#include "Common/OpenGl.hpp"

namespace GlRender
{
	template<> struct OneVariableBinder< int >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< int, 1 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< int, 2 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform2v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< int, 3 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform3v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< int, 4 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform4v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct OneVariableBinder< uint32_t >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< uint32_t, 1 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< uint32_t, 2 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform2v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< uint32_t, 3 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform3v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< uint32_t, 4 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform4v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct OneVariableBinder< float >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< float, 1 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< float, 2 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform2v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< float, 3 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform3v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< float, 4 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform4v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct OneVariableBinder< double >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< double, 1 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< double, 2 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform2v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< double, 3 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform3v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< double, 4 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniform4v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 2, 2 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix2x2v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 2, 3 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix2x3v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 2, 4 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix2x4v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 3, 2 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix3x2v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 3, 3 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix3x3v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 3, 4 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix3x4v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 4, 2 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix4x2v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 4, 3 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix4x3v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 4, 4 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix4x4v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 2, 2 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix2x2v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 2, 3 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix2x3v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 2, 4 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix2x4v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 3, 2 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix3x2v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 3, 3 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix3x3v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 3, 4>
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix3x4v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 4, 2 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix4x2v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 4, 3 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix4x3v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 4, 4 >
	{
		inline void operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			p_gl.SetUniformMatrix4x4v( p_glName, p_occurences, false, p_value );
		}
	};

	template< typename T > struct OneVariableBinder
	{
		inline void operator()( OpenGl const & CU_PARAM_UNUSED( p_gl ), int CU_PARAM_UNUSED( p_glName ), T const * CU_PARAM_UNUSED( p_value ), uint32_t CU_PARAM_UNUSED( p_occurences ) )
		{
			CASTOR_EXCEPTION( "OneVariableBinder - Unsupported arguments" );
		}
	};

	template< typename T, uint32_t Count > struct PointVariableBinder
	{
		inline void operator()( OpenGl const & CU_PARAM_UNUSED( p_gl ), int CU_PARAM_UNUSED( p_glName ), T const * CU_PARAM_UNUSED( p_value ), uint32_t CU_PARAM_UNUSED( p_occurences ) )
		{
			CASTOR_EXCEPTION( "PointVariableBinder - Unsupported arguments" );
		}
	};

	template< typename T, uint32_t Rows, uint32_t Columns > struct MatrixVariableBinder
	{
		inline void operator()( OpenGl const & CU_PARAM_UNUSED( p_gl ), int CU_PARAM_UNUSED( p_glName ), T const * CU_PARAM_UNUSED( p_value ), uint32_t CU_PARAM_UNUSED( p_occurences ) )
		{
			CASTOR_EXCEPTION( "MatrixVariableBinder - Unsupported arguments" );
		}
	};

	template< typename Type >
	inline void GlFrameVariableBase::DoBind( Type const * p_value, uint32_t p_occurences )const
	{
		REQUIRE( m_glName != GlInvalidIndex );
		OneVariableBinder< Type>()( GetOpenGl(), m_glName, p_value, p_occurences );
	}
	template< typename Type, int Count >
	inline void GlFrameVariableBase::DoBind( Type const * p_value, uint32_t p_occurences )const
	{
		REQUIRE( m_glName != GlInvalidIndex );
		PointVariableBinder< Type, Count>()( GetOpenGl(), m_glName, p_value, p_occurences );
	}
	template< typename Type, int Rows, int Columns >
	inline void GlFrameVariableBase::DoBind( Type const * p_value, uint32_t p_occurences )const
	{
		REQUIRE( m_glName != GlInvalidIndex );
		MatrixVariableBinder< Type, Rows, Columns>()( GetOpenGl(), m_glName, p_value, p_occurences );
	}
}
