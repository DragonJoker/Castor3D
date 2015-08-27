#include "OpenGl.hpp"

namespace GlRender
{
	template <> struct OneVariableApplyer< Castor3D::TextureBaseRPtr >
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, Castor3D::TextureBaseRPtr const * p_pValue, uint32_t p_uiOcc )
		{
			if ( p_uiOcc <= 1 && *p_pValue )
			{
				p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, ( int * ) & ( *p_pValue )->GetIndex() );
			}
		}
	};

	template <> struct OneVariableApplyer<int>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, int const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<int, 1>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, int const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<int, 2>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, int const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform2v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<int, 3>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, int const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform3v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<int, 4>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, int const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform4v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct OneVariableApplyer<uint32_t>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, uint32_t const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<uint32_t, 1>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, uint32_t const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<uint32_t, 2>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, uint32_t const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform2v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<uint32_t, 3>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, uint32_t const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform3v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<uint32_t, 4>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, uint32_t const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform4v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct OneVariableApplyer<float>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<float, 1>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<float, 2>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform2v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<float, 3>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform3v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<float, 4>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform4v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct OneVariableApplyer<double>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<double, 1>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform1v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<double, 2>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform2v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<double, 3>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform3v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct PointVariableApplyer<double, 4>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniform4v( p_iGlIndex, p_uiOcc, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<float, 2, 2>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix2x2v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<float, 2, 3>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix2x3v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<float, 2, 4>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix2x4v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<float, 3, 2>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix3x2v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<float, 3, 3>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix3x3v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<float, 3, 4>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix3x4v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<float, 4, 2>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix4x2v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<float, 4, 3>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix4x3v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<float, 4, 4>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, float const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix4x4v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<double, 2, 2>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix2x2v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<double, 2, 3>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix2x3v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<double, 2, 4>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix2x4v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<double, 3, 2>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix3x2v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<double, 3, 3>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix3x3v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<double, 3, 4>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix3x4v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<double, 4, 2>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix4x2v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<double, 4, 3>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix4x3v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template <> struct MatrixVariableApplyer<double, 4, 4>
	{
		inline void operator()( OpenGl & p_gl, int p_iGlIndex, double const * p_pValue, uint32_t p_uiOcc )
		{
			p_gl.SetUniformMatrix4x4v( p_iGlIndex, p_uiOcc, false, p_pValue );
		}
	};

	template< typename T > struct OneVariableApplyer
	{
		inline void operator()( OpenGl & CU_PARAM_UNUSED( p_gl ), int CU_PARAM_UNUSED( p_iGlIndex ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "OneVariableApplyer - Unsupported arguments" );
		}
	};

	template <typename T, uint32_t Count> struct PointVariableApplyer
	{
		inline void operator()( OpenGl & CU_PARAM_UNUSED( p_gl ), int CU_PARAM_UNUSED( p_iGlIndex ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "PointVariableApplyer - Unsupported arguments" );
		}
	};

	template <typename T, uint32_t Rows, uint32_t Columns> struct MatrixVariableApplyer
	{
		inline void operator()( OpenGl & CU_PARAM_UNUSED( p_gl ), int CU_PARAM_UNUSED( p_iGlIndex ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "MatrixVariableApplyer - Unsupported arguments" );
		}
	};

	template <typename Type>
	inline void GlFrameVariableBase::DoApply( Type const * p_pValue, uint32_t p_uiOcc )
	{
		if ( m_iGlIndex != eGL_INVALID_INDEX )
		{
			OneVariableApplyer<Type>()( m_gl, m_iGlIndex, p_pValue, p_uiOcc );
		}
	}
	template <typename Type, int Count>
	inline void GlFrameVariableBase::DoApply( Type const * p_pValue, uint32_t p_uiOcc )
	{
		if ( m_iGlIndex != eGL_INVALID_INDEX )
		{
			PointVariableApplyer<Type, Count>()( m_gl, m_iGlIndex, p_pValue, p_uiOcc );
		}
	}
	template <typename Type, int Rows, int Columns>
	inline void GlFrameVariableBase::DoApply( Type const * p_pValue, uint32_t p_uiOcc )
	{
		if ( m_iGlIndex != eGL_INVALID_INDEX )
		{
			MatrixVariableApplyer<Type, Rows, Columns>()( m_gl, m_iGlIndex, p_pValue, p_uiOcc );
		}
	}
}
