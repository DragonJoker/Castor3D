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

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			std::shared_ptr< GlOneFrameVariable< value_type > > l_variable = std::static_pointer_cast< GlOneFrameVariable< value_type > >( p_variable );

			if ( l_variable->GetOccCount() <= 1 && l_variable->GetValue() )
			{
				p_gl.SetUniform1v( p_index, l_variable->GetOccCount(), ( int * )&l_variable->GetValue()->GetIndex() );
			}
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_INT > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform1v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_UINT > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform1v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_FLOAT > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform1v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_DOUBLE > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform1v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC2I > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform2v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC3I > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform3v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC4I > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform4v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC2UI > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform2v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC3UI > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform3v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC4UI > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform4v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC2F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform2v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC3F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform3v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC4F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform4v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC2D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform2v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC3D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform3v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_VEC4D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform4v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X2F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X3F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X4F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X2F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X3F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X4F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X2F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X3F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X4F > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X2D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X3D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT2X4D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X2D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X3D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT3X4D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X2D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X3D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X4D > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template< eFRAME_VARIABLE_TYPE Type > struct GlVariableApplyer
	{
		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			CASTOR_EXCEPTION( "VariableApplyer - Unsupported arguments" );
		}
	};

	namespace
	{
		template< eFRAME_VARIABLE_TYPE Type > FrameVariableSPtr GlFrameVariableCreator( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences );
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_INT >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< int > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_UINT >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< uint32_t > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_FLOAT >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< float > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_DOUBLE >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< double > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_SAMPLER >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< TextureBaseRPtr > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2I >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< int, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3I >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< int, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4I >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< int, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2UI >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< uint32_t, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3UI >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< uint32_t, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4UI >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< uint32_t, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2F >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< float, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3F >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< float, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4F >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< float, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2D >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< double, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3D >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< double, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4D >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< double, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2F >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 2, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3F >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 2, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4F >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 2, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2F >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 3, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3F >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 3, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4F >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 3, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2F >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 4, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3F >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 4, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4F >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 4, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2D >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 2, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3D >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 2, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4D >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 2, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2D >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 3, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3D >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 3, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4D >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 3, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2D >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 4, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3D >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 4, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4D >( OpenGl & p_gl, GlShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 4, 4 > >( p_gl, p_occurences, p_program );
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
		, Holder( p_gl )
		, m_glBuffer( p_gl, eGL_BUFFER_TARGET_UNIFORM )
		, m_uniformBlockIndex( eGL_INVALID_INDEX )
		, m_uniformBlockSize( 0 )
	{
	}

	GlFrameVariableBuffer::~GlFrameVariableBuffer()
	{
	}

	FrameVariableSPtr GlFrameVariableBuffer::DoCreateVariable( ShaderProgramBase * p_program, eFRAME_VARIABLE_TYPE p_type, Castor::String const & p_name, uint32_t p_occurences )
	{
		FrameVariableSPtr l_return;
		GlVariableApplyerBaseSPtr l_pApplyer;
		GlShaderProgram * l_program = static_cast< GlShaderProgram * >( p_program );

		switch ( p_type )
		{
		case eFRAME_VARIABLE_TYPE_INT:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_INT >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_INT >();
			break;

		case eFRAME_VARIABLE_TYPE_UINT:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_UINT >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_UINT >();
			break;

		case eFRAME_VARIABLE_TYPE_FLOAT:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_FLOAT >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_FLOAT >();
			break;

		case eFRAME_VARIABLE_TYPE_DOUBLE:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_DOUBLE >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_DOUBLE >();
			break;

		case eFRAME_VARIABLE_TYPE_SAMPLER:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_SAMPLER >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_SAMPLER >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC2I:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2I >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2I >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC3I:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3I >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3I >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC4I:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4I >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4I >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC2UI:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2UI >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2UI >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC3UI:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3UI >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3UI >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC4UI:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4UI >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4UI >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC2F:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2F >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2F >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC3F:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3F >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3F >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC4F:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4F >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4F >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC2D:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2D >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2D >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC3D:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3D >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3D >();
			break;

		case eFRAME_VARIABLE_TYPE_VEC4D:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4D >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X2F:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2F >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X2F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X3F:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3F >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X3F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X4F:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4F >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X4F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X2F:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2F >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X2F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X3F:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3F >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X3F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X4F:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4F >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X4F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X2F:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2F >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X2F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X3F:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3F >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X3F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X4F:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4F >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X4F >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X2D:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2D >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X2D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X3D:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3D >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X3D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X4D:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4D >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT2X4D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X2D:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2D >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X2D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X3D:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3D >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X3D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X4D:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4D >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT3X4D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X2D:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2D >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X2D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X3D:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3D >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X3D >();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X4D:
			l_return = GlFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4D >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X4D >();
			break;
		}

		if ( l_return )
		{
			l_return->SetName( p_name );
		}

		return l_return;
	}

	bool GlFrameVariableBuffer::DoInitialise( ShaderProgramBase * p_program )
	{
		uint32_t l_index = m_glBuffer.GetGlName();
		bool l_return = false;
		GlShaderProgram * l_program = static_cast< GlShaderProgram * >( p_program );
		int l_max = 0;
		GetOpenGl().GetIntegerv( eGL_MAX_UNIFORM_BUFFER_BINDINGS, &l_max );

		if ( int( m_index ) < l_max )
		{
			if ( GetOpenGl().HasUbo() && l_index == eGL_INVALID_INDEX )
			{
				GetOpenGl().UseProgram( l_program->GetGlName() );
				m_uniformBlockIndex = GetOpenGl().GetUniformBlockIndex( l_program->GetGlName(), string::string_cast< char >( m_name ).c_str() );
				uint32_t l_totalSize = 0;

				if ( m_uniformBlockIndex != eGL_INVALID_INDEX )
				{
					GetOpenGl().GetActiveUniformBlockiv( l_program->GetGlName(), m_uniformBlockIndex, eGL_UNIFORM_BLOCK_DATA_SIZE, &m_uniformBlockSize );
					m_glBuffer.Create();
					m_glBuffer.Fill( NULL, m_uniformBlockSize, eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
					m_glBuffer.Bind();
					GetOpenGl().BindBufferBase( eGL_BUFFER_TARGET_UNIFORM, m_index, m_glBuffer.GetGlName() );
					GetOpenGl().UniformBlockBinding( l_program->GetGlName(), m_uniformBlockIndex, m_index );
					m_buffer.resize( m_uniformBlockSize );
					std::vector< const char * > l_arrayNames;

					for ( auto && l_variable : m_listVariables )
					{
						char * l_szChar = new char[l_variable->GetName().size() + 1];
						l_szChar[l_variable->GetName().size()] = 0;
#if defined( _MSC_VER )
						strncpy_s( l_szChar, l_variable->GetName().size() + 1, string::string_cast< char >( l_variable->GetName() ).c_str(), l_variable->GetName().size() );
#else
						strncpy( l_szChar, string::string_cast< char >( l_variable->GetName() ).c_str(), l_variable->GetName().size() );
#endif
						l_arrayNames.push_back( l_szChar );
					}

					UIntArray l_arrayIndices( l_arrayNames.size() );
					GetOpenGl().GetUniformIndices( l_program->GetGlName(), int( l_arrayNames.size() ), &l_arrayNames[0], &l_arrayIndices[0] );
					clear_container( l_arrayNames );
					int i = 0;

					for ( auto && l_variable : m_listVariables )
					{
						uint32_t l_index = l_arrayIndices[i++];
						int l_offset, l_size;

						if ( l_index != eGL_INVALID_INDEX )
						{
							GetOpenGl().GetActiveUniformsiv( l_program->GetGlName(), 1, &l_index, eGL_UNIFORM_OFFSET, &l_offset	);
							GetOpenGl().GetActiveUniformsiv( l_program->GetGlName(), 1, &l_index, eGL_UNIFORM_SIZE, &l_size	);

							if ( l_size == 1 )
							{
								l_size = int( l_variable->size() );
							}

							l_totalSize += l_size;
							l_variable->link( &m_buffer[l_offset] );
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
		m_uniformBlockIndex = eGL_INVALID_INDEX;
		m_uniformBlockSize = 0;
		m_glBuffer.Cleanup();
		m_glBuffer.Destroy();
	}

	bool GlFrameVariableBuffer::DoBind( uint32_t p_index )
	{
		if ( m_uniformBlockIndex != eGL_INVALID_INDEX )
		{
			m_glBuffer.Bind();
			m_glBuffer.Fill( &m_buffer[0], m_uniformBlockSize, eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
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
		if ( m_uniformBlockIndex != eGL_INVALID_INDEX )
		{
			//m_glBuffer.Unbind();
		}
	}
}
