#include "Common/OpenGl.hpp"

namespace GlRender
{
	template<> struct OneVariableBinder< int >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< int, 1 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< int, 2 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform2v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< int, 3 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform3v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< int, 4 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform4v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct OneVariableBinder< uint32_t >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< uint32_t, 1 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< uint32_t, 2 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform2v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< uint32_t, 3 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform3v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< uint32_t, 4 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform4v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct OneVariableBinder< float >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< float, 1 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< float, 2 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform2v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< float, 3 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform3v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< float, 4 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform4v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct OneVariableBinder< double >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< double, 1 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform1v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< double, 2 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform2v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< double, 3 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform3v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct PointVariableBinder< double, 4 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniform4v( p_glName, p_occurences, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 2, 2 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix2x2v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 2, 3 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix2x3v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 2, 4 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix2x4v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 3, 2 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix3x2v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 3, 3 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix3x3v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 3, 4 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix3x4v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 4, 2 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix4x2v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 4, 3 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix4x3v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< float, 4, 4 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix4x4v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 2, 2 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix2x2v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 2, 3 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix2x3v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 2, 4 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix2x4v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 3, 2 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix3x2v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 3, 3 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix3x3v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 3, 4>
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix3x4v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 4, 2 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix4x2v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 4, 3 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix4x3v( p_glName, p_occurences, false, p_value );
		}
	};

	template<> struct MatrixVariableBinder< double, 4, 4 >
	{
		inline bool operator()( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
		{
			return p_gl.SetUniformMatrix4x4v( p_glName, p_occurences, false, p_value );
		}
	};

	template< typename T > struct OneVariableBinder
	{
		inline bool operator()( OpenGl const & CU_PARAM_UNUSED( p_gl ), int CU_PARAM_UNUSED( p_glName ), T const * CU_PARAM_UNUSED( p_value ), uint32_t CU_PARAM_UNUSED( p_occurences ) )
		{
			CASTOR_EXCEPTION( "OneVariableBinder - Unsupported arguments" );
		}
	};

	template< typename T, uint32_t Count > struct PointVariableBinder
	{
		inline bool operator()( OpenGl const & CU_PARAM_UNUSED( p_gl ), int CU_PARAM_UNUSED( p_glName ), T const * CU_PARAM_UNUSED( p_value ), uint32_t CU_PARAM_UNUSED( p_occurences ) )
		{
			CASTOR_EXCEPTION( "PointVariableBinder - Unsupported arguments" );
		}
	};

	template< typename T, uint32_t Rows, uint32_t Columns > struct MatrixVariableBinder
	{
		inline bool operator()( OpenGl const & CU_PARAM_UNUSED( p_gl ), int CU_PARAM_UNUSED( p_glName ), T const * CU_PARAM_UNUSED( p_value ), uint32_t CU_PARAM_UNUSED( p_occurences ) )
		{
			CASTOR_EXCEPTION( "MatrixVariableBinder - Unsupported arguments" );
		}
	};

	template< typename Type >
	inline bool GlFrameVariableBase::DoBind( Type const * p_value, uint32_t p_occurences )const
	{
		bool l_return = true;

		if ( m_glName != GlInvalidIndex )
		{
			l_return = OneVariableBinder< Type>()( GetOpenGl(), m_glName, p_value, p_occurences );
		}

		return l_return;
	}
	template< typename Type, int Count >
	inline bool GlFrameVariableBase::DoBind( Type const * p_value, uint32_t p_occurences )const
	{
		bool l_return = true;

		if ( m_glName != GlInvalidIndex )
		{
			l_return = PointVariableBinder< Type, Count>()( GetOpenGl(), m_glName, p_value, p_occurences );
		}

		return l_return;
	}
	template< typename Type, int Rows, int Columns >
	inline bool GlFrameVariableBase::DoBind( Type const * p_value, uint32_t p_occurences )const
	{
		bool l_return = true;

		if ( m_glName != GlInvalidIndex )
		{
			l_return = MatrixVariableBinder< Type, Rows, Columns>()( GetOpenGl(), m_glName, p_value, p_occurences );
		}

		return l_return;
	}
}
