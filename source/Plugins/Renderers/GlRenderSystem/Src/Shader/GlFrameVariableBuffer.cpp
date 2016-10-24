#include "Shader/GlFrameVariableBuffer.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlOneFrameVariable.hpp"
#include "Shader/GlPointFrameVariable.hpp"
#include "Shader/GlMatrixFrameVariable.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	template< FrameVariableType > struct GlVariableApplyer;

	template<> struct GlVariableApplyer< FrameVariableType::Sampler > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			std::shared_ptr< GlOneFrameVariable< value_type > > l_variable = std::static_pointer_cast< GlOneFrameVariable< value_type > >( p_variable );

			if ( l_variable->GetOccCount() <= 1 && l_variable->GetValue() )
			{
				p_gl.SetUniform1v( p_index, l_variable->GetOccCount(), ( int * )&l_variable->GetValue() );
			}
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Int > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform1v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::UInt > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform1v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Float > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform1v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Double > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform1v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Vec2i > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform2v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Vec3i > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform3v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Vec4i > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform4v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Vec2ui > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform2v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Vec3ui > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform3v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Vec4ui > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform4v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Vec2f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform2v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Vec3f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform3v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Vec4f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform4v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Vec2d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform2v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Vec3d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform3v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Vec4d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform4v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat2x2f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat2x3f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat2x4f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat3x2f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat3x3f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat3x4f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat4x2f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat4x3f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat4x4f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat2x2d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat2x3d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat2x4d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat3x2d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat3x3d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat3x4d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat4x2d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat4x3d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::Mat4x4d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template< FrameVariableType Type > struct GlVariableApplyer
	{
		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			CASTOR_EXCEPTION( "VariableApplyer - Unsupported arguments" );
		}
	};

	namespace
	{
		template< FrameVariableType Type > FrameVariableSPtr GlFrameVariableCreator( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences );
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Int >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< int > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::UInt >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< uint32_t > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Float >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< float > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Double >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< double > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Sampler >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< int > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Vec2i >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< int, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Vec3i >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< int, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Vec4i >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< int, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Vec2ui >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< uint32_t, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Vec3ui >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< uint32_t, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Vec4ui >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< uint32_t, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Vec2f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< float, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Vec3f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< float, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Vec4f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< float, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Vec2d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< double, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Vec3d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< double, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Vec4d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< double, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat2x2f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 2, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat2x3f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 2, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat2x4f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 2, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat3x2f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 3, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat3x3f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 3, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat3x4f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 3, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat4x2f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 4, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat4x3f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 4, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat4x4f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 4, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat2x2d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 2, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat2x3d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 2, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat2x4d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 2, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat3x2d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 3, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat3x3d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 3, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat3x4d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 3, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat4x2d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 4, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat4x3d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 4, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::Mat4x4d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 4, 4 > >( p_gl, p_occurences, p_program );
		}

		template< FrameVariableType Type > GlVariableApplyerBaseSPtr GlApplyerCreator();
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Int >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Int > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::UInt >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::UInt > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Float >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Float > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Double >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Double > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Sampler >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Sampler > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Vec2i >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Vec2i > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Vec3i >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Vec3i > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Vec4i >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Vec4i > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Vec2ui >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Vec2ui > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Vec3ui >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Vec3ui > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Vec4ui >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Vec4ui > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Vec2f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Vec2f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Vec3f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Vec3f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Vec4f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Vec4f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Vec2d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Vec2d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Vec3d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Vec3d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Vec4d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Vec4d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat2x2f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat2x2f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat2x3f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat2x3f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat2x4f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat2x4f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat3x2f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat3x2f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat3x3f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat3x3f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat3x4f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat3x4f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat4x2f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat4x2f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat4x3f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat4x3f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat4x4f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat4x4f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat2x2d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat2x2d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat2x3d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat2x3d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat2x4d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat2x4d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat3x2d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat3x2d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat3x3d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat3x3d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat3x4d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat3x4d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat4x2d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat4x2d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat4x3d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat4x3d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::Mat4x4d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::Mat4x4d > >();
		}
	}

	GlFrameVariableBuffer::GlFrameVariableBuffer( OpenGl & p_gl, String const & p_name, GlShaderProgram & p_program, RenderSystem & p_renderSystem )
		: FrameVariableBuffer( p_name, p_program, p_renderSystem )
		, Holder( p_gl )
		, m_glBuffer( p_gl, GlBufferTarget::Uniform )
		, m_uniformBlockIndex( int( eGL_INVALID_INDEX ) )
		, m_uniformBlockSize( 0 )
	{
	}

	GlFrameVariableBuffer::~GlFrameVariableBuffer()
	{
	}

	FrameVariableSPtr GlFrameVariableBuffer::DoCreateVariable( FrameVariableType p_type, Castor::String const & p_name, uint32_t p_occurences )
	{
		FrameVariableSPtr l_return;
		GlVariableApplyerBaseSPtr l_pApplyer;
		GlShaderProgram & l_program = static_cast< GlShaderProgram & >( m_program );

		switch ( p_type )
		{
		case FrameVariableType::Int:
			l_return = GlFrameVariableCreator< FrameVariableType::Int >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Int >();
			break;

		case FrameVariableType::UInt:
			l_return = GlFrameVariableCreator< FrameVariableType::UInt >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::UInt >();
			break;

		case FrameVariableType::Float:
			l_return = GlFrameVariableCreator< FrameVariableType::Float >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Float >();
			break;

		case FrameVariableType::Double:
			l_return = GlFrameVariableCreator< FrameVariableType::Double >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Double >();
			break;

		case FrameVariableType::Sampler:
			l_return = GlFrameVariableCreator< FrameVariableType::Sampler >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Sampler >();
			break;

		case FrameVariableType::Vec2i:
			l_return = GlFrameVariableCreator< FrameVariableType::Vec2i >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Vec2i >();
			break;

		case FrameVariableType::Vec3i:
			l_return = GlFrameVariableCreator< FrameVariableType::Vec3i >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Vec3i >();
			break;

		case FrameVariableType::Vec4i:
			l_return = GlFrameVariableCreator< FrameVariableType::Vec4i >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Vec4i >();
			break;

		case FrameVariableType::Vec2ui:
			l_return = GlFrameVariableCreator< FrameVariableType::Vec2ui >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Vec2ui >();
			break;

		case FrameVariableType::Vec3ui:
			l_return = GlFrameVariableCreator< FrameVariableType::Vec3ui >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Vec3ui >();
			break;

		case FrameVariableType::Vec4ui:
			l_return = GlFrameVariableCreator< FrameVariableType::Vec4ui >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Vec4ui >();
			break;

		case FrameVariableType::Vec2f:
			l_return = GlFrameVariableCreator< FrameVariableType::Vec2f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Vec2f >();
			break;

		case FrameVariableType::Vec3f:
			l_return = GlFrameVariableCreator< FrameVariableType::Vec3f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Vec3f >();
			break;

		case FrameVariableType::Vec4f:
			l_return = GlFrameVariableCreator< FrameVariableType::Vec4f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Vec4f >();
			break;

		case FrameVariableType::Vec2d:
			l_return = GlFrameVariableCreator< FrameVariableType::Vec2d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Vec2d >();
			break;

		case FrameVariableType::Vec3d:
			l_return = GlFrameVariableCreator< FrameVariableType::Vec3d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Vec3d >();
			break;

		case FrameVariableType::Vec4d:
			l_return = GlFrameVariableCreator< FrameVariableType::Vec4d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Vec4d >();
			break;

		case FrameVariableType::Mat2x2f:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat2x2f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat2x2f >();
			break;

		case FrameVariableType::Mat2x3f:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat2x3f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat2x3f >();
			break;

		case FrameVariableType::Mat2x4f:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat2x4f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat2x4f >();
			break;

		case FrameVariableType::Mat3x2f:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat3x2f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat3x2f >();
			break;

		case FrameVariableType::Mat3x3f:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat3x3f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat3x3f >();
			break;

		case FrameVariableType::Mat3x4f:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat3x4f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat3x4f >();
			break;

		case FrameVariableType::Mat4x2f:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat4x2f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat4x2f >();
			break;

		case FrameVariableType::Mat4x3f:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat4x3f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat4x3f >();
			break;

		case FrameVariableType::Mat4x4f:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat4x4f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat4x4f >();
			break;

		case FrameVariableType::Mat2x2d:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat2x2d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat2x2d >();
			break;

		case FrameVariableType::Mat2x3d:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat2x3d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat2x3d >();
			break;

		case FrameVariableType::Mat2x4d:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat2x4d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat2x4d >();
			break;

		case FrameVariableType::Mat3x2d:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat3x2d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat3x2d >();
			break;

		case FrameVariableType::Mat3x3d:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat3x3d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat3x3d >();
			break;

		case FrameVariableType::Mat3x4d:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat3x4d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat3x4d >();
			break;

		case FrameVariableType::Mat4x2d:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat4x2d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat4x2d >();
			break;

		case FrameVariableType::Mat4x3d:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat4x3d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat4x3d >();
			break;

		case FrameVariableType::Mat4x4d:
			l_return = GlFrameVariableCreator< FrameVariableType::Mat4x4d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::Mat4x4d >();
			break;
		}

		if ( l_return )
		{
			l_return->SetName( p_name );
		}

		return l_return;
	}

	bool GlFrameVariableBuffer::DoInitialise()
	{
		uint32_t l_index = m_glBuffer.GetGlName();
		bool l_return = false;
		GlShaderProgram & l_program = static_cast< GlShaderProgram & >( m_program );
		int l_max = 0;
		GetOpenGl().GetIntegerv( GlMax::UniformBufferBindings, &l_max );

		if ( int( m_index ) < l_max )
		{
			if ( GetOpenGl().HasUbo() && l_index == eGL_INVALID_INDEX )
			{
				GetOpenGl().UseProgram( l_program.GetGlName() );
				m_uniformBlockIndex = GetOpenGl().GetUniformBlockIndex( l_program.GetGlName(), string::string_cast< char >( m_name ).c_str() );
				uint32_t l_totalSize = 0;

				if ( m_uniformBlockIndex != int( eGL_INVALID_INDEX ) )
				{
					GetOpenGl().GetActiveUniformBlockiv( l_program.GetGlName(), m_uniformBlockIndex, eGL_UNIFORM_BLOCK_DATA_SIZE, &m_uniformBlockSize );
					m_glBuffer.Create();
					m_glBuffer.Fill( nullptr, m_uniformBlockSize, BufferAccessType::Dynamic, BufferAccessNature::Draw );
					m_glBuffer.Bind();
					GetOpenGl().BindBufferBase( GlBufferTarget::Uniform, m_index, m_glBuffer.GetGlName() );
					GetOpenGl().UniformBlockBinding( l_program.GetGlName(), m_uniformBlockIndex, m_index );
					m_buffer.resize( m_uniformBlockSize );
					std::vector< const char * > l_arrayNames;

					for ( auto l_variable : m_listVariables )
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
					GetOpenGl().GetUniformIndices( l_program.GetGlName(), int( l_arrayNames.size() ), &l_arrayNames[0], &l_arrayIndices[0] );

					for ( auto l_name : l_arrayNames )
					{
						delete [] l_name;
					}

					int i = 0;

					for ( auto l_variable : m_listVariables )
					{
						uint32_t l_index = l_arrayIndices[i++];
						int l_offset, l_size;

						if ( l_index != eGL_INVALID_INDEX )
						{
							GetOpenGl().GetActiveUniformsiv( l_program.GetGlName(), 1, &l_index, eGL_UNIFORM_OFFSET, &l_offset );
							GetOpenGl().GetActiveUniformsiv( l_program.GetGlName(), 1, &l_index, eGL_UNIFORM_SIZE, &l_size );

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
		m_uniformBlockIndex = int( eGL_INVALID_INDEX );
		m_uniformBlockSize = 0;
		m_glBuffer.Destroy();
	}

	bool GlFrameVariableBuffer::DoBind( uint32_t p_index )
	{
		if ( m_uniformBlockIndex != int( eGL_INVALID_INDEX ) )
		{
			m_glBuffer.Fill( &m_buffer[0], m_uniformBlockSize, BufferAccessType::Dynamic, BufferAccessNature::Draw );
		}
		else
		{
			for ( auto l_variable : m_listInitialised )
			{
				l_variable->Bind();
			}
		}

		return true;
	}

	void GlFrameVariableBuffer::DoUnbind( uint32_t p_index )
	{
		if ( m_uniformBlockIndex != int( eGL_INVALID_INDEX ) )
		{
			m_glBuffer.Unbind();
		}
	}
}
