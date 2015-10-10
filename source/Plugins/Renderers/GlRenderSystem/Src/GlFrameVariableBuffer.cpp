#include "GlFrameVariableBuffer.hpp"
#include "GlOneFrameVariable.hpp"
#include "GlPointFrameVariable.hpp"
#include "GlMatrixFrameVariable.hpp"
#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	template< eFRAME_VARIABLE_TYPE > struct GlVariableApplyer;

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_SAMPLER > : public GlVariableApplyerBase
	{
		typedef TextureBaseRPtr	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			std::shared_ptr< GlOneFrameVariable< value_type > > l_pVariable = std::static_pointer_cast< GlOneFrameVariable< value_type > >( p_pVariable );

			if ( l_pVariable->GetOccCount() <= 1 && l_pVariable->GetValue() )
			{
				p_gl.SetUniform1v( p_index, l_pVariable->GetOccCount(), ( int * )&l_pVariable->GetValue()->GetIndex() );
			}
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_INT > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform1v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_UINT > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform1v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_FLOAT > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform1v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_DOUBLE > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform1v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC2I > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform2v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC3I > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform3v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC4I > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform4v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC2UI > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform2v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC3UI > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform3v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC4UI > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform4v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC2F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform2v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC3F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform3v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC4F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform4v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC2D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform2v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC3D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform3v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC4D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniform4v( p_index, p_pVariable->GetOccCount(), ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X2F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix2x2v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X3F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix2x3v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X4F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix2x4v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X2F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix3x2v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X3F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix3x3v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X4F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix3x4v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X2F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix4x2v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X3F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix4x3v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X4F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix4x4v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X2D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix2x2v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X3D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix2x3v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X4D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix2x4v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X2D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix3x2v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X3D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix3x3v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X4D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix3x4v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X2D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix4x2v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X3D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix4x3v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X4D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			p_gl.SetUniformMatrix4x4v( p_index, p_pVariable->GetOccCount(), false, ( value_type const * )p_pVariable->const_ptr() );
		}
	};

	template< eFRAME_VARIABLE_TYPE Type > struct GlVariableApplyer
	{
		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_pVariable )
		{
			CASTOR_EXCEPTION( "VariableApplyer - Unsupported arguments" );
		}
	};

	namespace
	{
		template< eFRAME_VARIABLE_TYPE Type > FrameVariableSPtr GlFrameVariableCreator( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc );
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_INT >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlOneFrameVariable< int > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_UINT >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlOneFrameVariable< uint32_t > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_FLOAT >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlOneFrameVariable< float > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_DOUBLE >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlOneFrameVariable< double > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_SAMPLER >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlOneFrameVariable< TextureBaseRPtr > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2I >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlPointFrameVariable< int, 2 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3I >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlPointFrameVariable< int, 3 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4I >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlPointFrameVariable< int, 4 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2UI >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlPointFrameVariable< uint32_t, 2 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3UI >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlPointFrameVariable< uint32_t, 3 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4UI >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlPointFrameVariable< uint32_t, 4 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2F >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlPointFrameVariable< float, 2 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3F >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlPointFrameVariable< float, 3 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4F >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlPointFrameVariable< float, 4 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2D >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlPointFrameVariable< double, 2 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3D >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlPointFrameVariable< double, 3 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4D >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlPointFrameVariable< double, 4 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2F >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 2, 2 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3F >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 2, 3 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4F >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 2, 4 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2F >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 3, 2 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3F >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 3, 3 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4F >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 3, 4 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2F >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 4, 2 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3F >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 4, 3 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4F >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 4, 4 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2D >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 2, 2 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3D >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 2, 3 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4D >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 2, 4 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2D >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 3, 2 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3D >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 3, 3 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4D >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 3, 4 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2D >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 4, 2 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3D >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 4, 3 > >( p_gl, p_uiNbOcc, p_pProgram );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4D >( OpenGl & p_gl, GlShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 4, 4 > >( p_gl, p_uiNbOcc, p_pProgram );
		}

		template< eFRAME_VARIABLE_TYPE Type > GlVariableApplyerBaseSPtr GlApplyerCreator();
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_INT >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_INT > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_UINT >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_UINT > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_FLOAT >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_FLOAT > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_DOUBLE >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_DOUBLE > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_SAMPLER >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_SAMPLER > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2I >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC2I > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3I >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC3I > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4I >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC4I > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2UI >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC2UI > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3UI >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC3UI > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4UI >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC4UI > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2F >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC2F > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3F >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC3F > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4F >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC4F > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2D >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC2D > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3D >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC3D > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4D >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC4D > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X2F >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X2F > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X3F >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X3F > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X4F >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X4F > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X2F >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X2F > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X3F >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X3F > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X4F >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X4F > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X2F >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X2F > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X3F >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X3F > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X4F >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X4F > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X2D >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X2D > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X3D >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X3D > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X4D >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X4D > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X2D >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X2D > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X3D >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X3D > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X4D >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X4D > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X2D >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X2D > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X3D >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X3D > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X4D >()
		{
			return std::make_shared< GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X4D > >();
		}
	}

	GlFrameVariableBuffer::GlFrameVariableBuffer( OpenGl & p_gl, String const & p_name, GlRenderSystem & p_renderSystem )
		: FrameVariableBuffer( p_name, p_renderSystem )
		, m_glBuffer( p_gl, eGL_BUFFER_TARGET_UNIFORM )
		, m_iUniformBlockIndex( eGL_INVALID_INDEX )
		, m_iUniformBlockSize( 0 )
		, m_gl( p_gl )
	{
	}

	GlFrameVariableBuffer::~GlFrameVariableBuffer()
	{
	}

	FrameVariableSPtr GlFrameVariableBuffer::DoCreateVariable( ShaderProgramBase * p_pProgram, eFRAME_VARIABLE_TYPE p_type, Castor::String const & p_name, uint32_t p_uiNbOcc )
	{
		FrameVariableSPtr l_pReturn;
		GlVariableApplyerBaseSPtr l_pApplyer;
		GlShaderProgram * l_pProgram = static_cast< GlShaderProgram * >( p_pProgram );

		switch ( p_type )
		{
		case eFRAME_VARIABLE_TYPE_INT:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_INT >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_INT >();
			break;

		case eFRAME_VARIABLE_TYPE_UINT:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_UINT >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_UINT >();
			break;

		case eFRAME_VARIABLE_TYPE_FLOAT:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_FLOAT >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_FLOAT >();
			break;

		case eFRAME_VARIABLE_TYPE_DOUBLE:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_DOUBLE >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_DOUBLE >();
			break;

		case eFRAME_VARIABLE_TYPE_SAMPLER:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_SAMPLER >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_SAMPLER >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC2I:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2I >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2I >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC3I:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3I >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3I >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC4I:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4I >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4I >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC2UI:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2UI >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2UI >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC3UI:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3UI >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3UI >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC4UI:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4UI >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4UI >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC2F:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2F >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2F >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC3F:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3F >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3F >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC4F:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4F >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4F >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC2D:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2D >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2D >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC3D:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3D >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3D >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC4D:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4D >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X2F:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2F >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X2F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X3F:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3F >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X3F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X4F:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4F >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X4F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X2F:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2F >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X2F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X3F:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3F >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X3F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X4F:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4F >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X4F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X2F:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2F >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X2F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X3F:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3F >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X3F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X4F:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4F >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X4F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X2D:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2D >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X2D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X3D:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3D >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X3D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X4D:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4D >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X4D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X2D:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2D >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X2D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X3D:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3D >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X3D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X4D:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4D >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X4D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X2D:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2D >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X2D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X3D:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3D >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X3D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X4D:
			l_pReturn = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4D >( m_gl, l_pProgram, p_uiNbOcc );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X4D >();
			break;
		}

		if ( l_pReturn )
		{
			l_pReturn->SetName( p_name );
		}

		return l_pReturn;
	}

	bool GlFrameVariableBuffer::DoInitialise( ShaderProgramBase * p_pProgram )
	{
		uint32_t l_uiIndex = m_glBuffer.GetGlIndex();
		bool l_return = false;
		GlShaderProgram * l_pProgram = static_cast< GlShaderProgram * >( p_pProgram );
		int l_max = 0;
		m_gl.GetIntegerv( eGL_MAX_UNIFORM_BUFFER_BINDINGS, &l_max );

		if ( int( m_uiIndex ) < l_max )
		{
			if ( m_gl.HasUbo() && l_uiIndex == eGL_INVALID_INDEX )
			{
				m_gl.UseProgram( l_pProgram->GetGlProgram() );
				m_iUniformBlockIndex = m_gl.GetUniformBlockIndex( l_pProgram->GetGlProgram(), string::string_cast< char >( m_strName ).c_str() );
				uint32_t l_uiTotalSize = 0;

				if ( m_iUniformBlockIndex != eGL_INVALID_INDEX )
				{
					m_gl.GetActiveUniformBlockiv( l_pProgram->GetGlProgram(), m_iUniformBlockIndex, eGL_UNIFORM_BLOCK_DATA_SIZE, &m_iUniformBlockSize );
					m_glBuffer.Create();
					m_glBuffer.Fill( NULL, m_iUniformBlockSize, eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
					m_glBuffer.Bind();
					m_gl.BindBufferBase( eGL_BUFFER_TARGET_UNIFORM, m_uiIndex, m_glBuffer.GetGlIndex() );
					m_gl.UniformBlockBinding( l_pProgram->GetGlProgram(), m_iUniformBlockIndex, m_uiIndex );
					m_buffer.resize( m_iUniformBlockSize );
					std::vector< const char * > l_arrayNames;

					for ( auto && l_pVariable : m_listVariables )
					{
						char * l_szChar = new char[l_pVariable->GetName().size() + 1];
						l_szChar[l_pVariable->GetName().size()] = 0;
#if defined( _MSC_VER )
						strncpy_s( l_szChar, l_pVariable->GetName().size() + 1, string::string_cast< char >( l_pVariable->GetName() ).c_str(), l_pVariable->GetName().size() );
#else
						strncpy( l_szChar, string::string_cast< char >( l_pVariable->GetName() ).c_str(), l_pVariable->GetName().size() );
#endif
						l_arrayNames.push_back( l_szChar );
					}

					UIntArray l_arrayIndices( l_arrayNames.size() );
					m_gl.GetUniformIndices( l_pProgram->GetGlProgram(), int( l_arrayNames.size() ), &l_arrayNames[0], &l_arrayIndices[0] );
					clear_container( l_arrayNames );
					int i = 0;

					for ( auto && l_pVariable : m_listVariables )
					{
						uint32_t l_uiIndex = l_arrayIndices[i++];
						int l_iOffset, l_iSize;

						if ( l_uiIndex != eGL_INVALID_INDEX )
						{
							m_gl.GetActiveUniformsiv( l_pProgram->GetGlProgram(), 1, &l_uiIndex, eGL_UNIFORM_OFFSET, &l_iOffset	);
							m_gl.GetActiveUniformsiv( l_pProgram->GetGlProgram(), 1, &l_uiIndex, eGL_UNIFORM_SIZE, &l_iSize	);

							if ( l_iSize == 1 )
							{
								l_iSize = int( l_pVariable->size() );
							}

							l_uiTotalSize += l_iSize;
							l_pVariable->link( &m_buffer[l_iOffset] );
						}
					}

					m_glBuffer.Unbind();
					l_return = true;
				}
			}
			else
			{
				l_return = false;
			}
		}
		else
		{
			DoCleanup();
			l_return = false;
		}

		return l_return;
	}

	void GlFrameVariableBuffer::DoCleanup()
	{
		m_iUniformBlockIndex = eGL_INVALID_INDEX;
		m_iUniformBlockSize = 0;
		m_glBuffer.Cleanup();
		m_glBuffer.Destroy();
	}

	bool GlFrameVariableBuffer::DoBind( uint32_t p_index )
	{
		if ( m_iUniformBlockIndex != eGL_INVALID_INDEX )
		{
			m_glBuffer.Bind();
			m_glBuffer.Fill( &m_buffer[0], m_iUniformBlockSize, eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
		}
		else
		{
			for ( auto && l_variable : m_listInitialised )
			{
				l_variable->Bind();
			}
		}

		return true;
	}

	void GlFrameVariableBuffer::DoUnbind( uint32_t p_index )
	{
		if ( m_iUniformBlockIndex != eGL_INVALID_INDEX )
		{
			//m_glBuffer.Unbind();
		}
	}
}
