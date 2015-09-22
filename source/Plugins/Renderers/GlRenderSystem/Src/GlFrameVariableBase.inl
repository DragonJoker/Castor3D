#include "OpenGl.hpp"

namespace GlRender
{
	template<> struct OneVariableBinder< Castor3D::TextureBaseRPtr >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, Castor3D::TextureBaseRPtr const * p_pValue, uint32_t p_uiOcc )
		{
			if ( p_uiOcc <= 1 && *p_pValue )
			{
				p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, ( int * ) & ( *p_pValue )->GetIndex() );
			}
		}
	};

	template<> struct OneVariableBinder< int >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, int const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< int, 1 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, int const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< int, 2 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, int const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform2v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< int, 3 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, int const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform3v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< int, 4 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, int const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform4v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct OneVariableBinder< uint32_t >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, uint32_t const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< uint32_t, 1 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, uint32_t const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< uint32_t, 2 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, uint32_t const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform2v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< uint32_t, 3 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, uint32_t const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform3v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< uint32_t, 4 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, uint32_t const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform4v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct OneVariableBinder< float >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< float, 1 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< float, 2 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform2v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< float, 3 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform3v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< float, 4 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform4v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct OneVariableBinder< double >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< double, 1 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< double, 2 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform2v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< double, 3 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform3v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct PointVariableBinder< double, 4 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform4v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< float, 2, 2 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix2x2v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< float, 2, 3 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix2x3v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< float, 2, 4 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix2x4v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< float, 3, 2 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix3x2v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< float, 3, 3 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix3x3v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< float, 3, 4 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix3x4v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< float, 4, 2 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix4x2v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< float, 4, 3 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix4x3v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< float, 4, 4 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix4x4v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< double, 2, 2 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix2x2v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< double, 2, 3 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix2x3v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< double, 2, 4 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix2x4v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< double, 3, 2 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix3x2v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< double, 3, 3 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix3x3v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< double, 3, 4>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix3x4v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< double, 4, 2 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix4x2v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< double, 4, 3 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix4x3v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template<> struct MatrixVariableBinder< double, 4, 4 >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix4x4v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template< typename T > struct OneVariableBinder
	{
		inline void operator()( OpenGl & CU_PARAM_UNUSED( p_gl ), int CU_PARAM_UNUSED( p_iGlIndex ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "OneVariableBinder - Unsupported arguments" );
		}
	};

	template< typename T, uint32_t Count > struct PointVariableBinder
	{
		inline void operator()( OpenGl & CU_PARAM_UNUSED( p_gl ), int CU_PARAM_UNUSED( p_iGlIndex ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "PointVariableBinder - Unsupported arguments" );
		}
	};

	template< typename T, uint32_t Rows, uint32_t Columns > struct MatrixVariableBinder
	{
		inline void operator()( OpenGl & CU_PARAM_UNUSED( p_gl ), int CU_PARAM_UNUSED( p_iGlIndex ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "MatrixVariableBinder - Unsupported arguments" );
		}
	};

	template< typename Type >
	inline void GlFrameVariableBase::DoBind( Type const * p_pValue, uint32_t p_uiOcc )
	{
		if ( m_iGlIndex != eGL_INVALID_INDEX )
		{
			OneVariableBinder< Type>()( m_gl, m_iGlIndex, p_pValue, p_uiOcc );
		}
	}
	template< typename Type, int Count >
	inline void GlFrameVariableBase::DoBind( Type const * p_pValue, uint32_t p_uiOcc )
	{
		if ( m_iGlIndex != eGL_INVALID_INDEX )
		{
			PointVariableBinder< Type, Count>()( m_gl, m_iGlIndex, p_pValue, p_uiOcc );
		}
	}
	template< typename Type, int Rows, int Columns >
	inline void GlFrameVariableBase::DoBind( Type const * p_pValue, uint32_t p_uiOcc )
	{
		if ( m_iGlIndex != eGL_INVALID_INDEX )
		{
			MatrixVariableBinder< Type, Rows, Columns>()( m_gl, m_iGlIndex, p_pValue, p_uiOcc );
		}
	}
}
