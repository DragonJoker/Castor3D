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

	template<> struct GlVariableApplyer< FrameVariableType::eSampler > : public GlVariableApplyerBase
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

	template<> struct GlVariableApplyer< FrameVariableType::eInt > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform1v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eUInt > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform1v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eFloat > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform1v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eDouble > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform1v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eVec2i > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform2v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eVec3i > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform3v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eVec4i > : public GlVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform4v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eVec2ui > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform2v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eVec3ui > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform3v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eVec4ui > : public GlVariableApplyerBase
	{
		typedef uint32_t	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform4v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eVec2f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform2v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eVec3f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform3v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eVec4f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform4v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eVec2d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform2v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eVec3d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform3v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eVec4d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniform4v( p_index, p_variable->GetOccCount(), ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat2x2f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat2x3f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat2x4f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat3x2f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat3x3f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat3x4f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat4x2f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat4x3f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat4x4f > : public GlVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat2x2d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat2x3d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat2x4d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix2x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat3x2d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat3x3d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat3x4d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix3x4v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat4x2d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x2v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat4x3d > : public GlVariableApplyerBase
	{
		typedef double	value_type;

		inline void operator()( OpenGl & p_gl, uint32_t p_index, FrameVariableSPtr p_variable )
		{
			p_gl.SetUniformMatrix4x3v( p_index, p_variable->GetOccCount(), false, ( value_type const * )p_variable->const_ptr() );
		}
	};

	template<> struct GlVariableApplyer< FrameVariableType::eMat4x4d > : public GlVariableApplyerBase
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
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eInt >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< int > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eUInt >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< uint32_t > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eFloat >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< float > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eDouble >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< double > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eSampler >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlOneFrameVariable< int > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eVec2i >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< int, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eVec3i >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< int, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eVec4i >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< int, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eVec2ui >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< uint32_t, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eVec3ui >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< uint32_t, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eVec4ui >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< uint32_t, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eVec2f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< float, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eVec3f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< float, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eVec4f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< float, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eVec2d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< double, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eVec3d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< double, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eVec4d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlPointFrameVariable< double, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat2x2f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 2, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat2x3f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 2, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat2x4f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 2, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat3x2f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 3, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat3x3f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 3, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat3x4f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 3, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat4x2f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 4, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat4x3f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 4, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat4x4f >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< float, 4, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat2x2d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 2, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat2x3d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 2, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat2x4d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 2, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat3x2d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 3, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat3x3d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 3, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat3x4d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 3, 4 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat4x2d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 4, 2 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat4x3d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 4, 3 > >( p_gl, p_occurences, p_program );
		}
		template<> FrameVariableSPtr GlFrameVariableCreator< FrameVariableType::eMat4x4d >( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< GlMatrixFrameVariable< double, 4, 4 > >( p_gl, p_occurences, p_program );
		}

		template< FrameVariableType Type > GlVariableApplyerBaseSPtr GlApplyerCreator();
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eInt >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eInt > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eUInt >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eUInt > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eFloat >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eFloat > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eDouble >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eDouble > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eSampler >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eSampler > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eVec2i >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eVec2i > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eVec3i >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eVec3i > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eVec4i >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eVec4i > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eVec2ui >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eVec2ui > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eVec3ui >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eVec3ui > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eVec4ui >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eVec4ui > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eVec2f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eVec2f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eVec3f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eVec3f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eVec4f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eVec4f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eVec2d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eVec2d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eVec3d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eVec3d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eVec4d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eVec4d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat2x2f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat2x2f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat2x3f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat2x3f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat2x4f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat2x4f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat3x2f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat3x2f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat3x3f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat3x3f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat3x4f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat3x4f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat4x2f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat4x2f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat4x3f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat4x3f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat4x4f >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat4x4f > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat2x2d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat2x2d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat2x3d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat2x3d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat2x4d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat2x4d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat3x2d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat3x2d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat3x3d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat3x3d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat3x4d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat3x4d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat4x2d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat4x2d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat4x3d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat4x3d > >();
		}
		template<> GlVariableApplyerBaseSPtr GlApplyerCreator< FrameVariableType::eMat4x4d >()
		{
			return std::make_shared< GlVariableApplyer< FrameVariableType::eMat4x4d > >();
		}
	}

	GlFrameVariableBuffer::GlFrameVariableBuffer(
		OpenGl & p_gl,
		String const & p_name,
		GlShaderProgram & p_program,
		FlagCombination< ShaderTypeFlag > const & p_flags,
		RenderSystem & p_renderSystem )
		: FrameVariableBuffer( p_name, p_program, p_flags, p_renderSystem )
		, Holder( p_gl )
		, m_glBuffer( p_gl, GlBufferTarget::eUniform )
		, m_uniformBlockIndex( int( GlInvalidIndex ) )
		, m_uniformBlockSize( 0 )
	{
	}

	GlFrameVariableBuffer::~GlFrameVariableBuffer()
	{
	}

	void GlFrameVariableBuffer::SetBindingPoint( uint32_t p_point )const
	{
		m_glBuffer.SetBindingPoint( p_point );
	}

	FrameVariableSPtr GlFrameVariableBuffer::DoCreateVariable( FrameVariableType p_type, Castor::String const & p_name, uint32_t p_occurences )
	{
		FrameVariableSPtr l_return;
		GlVariableApplyerBaseSPtr l_pApplyer;
		GlShaderProgram & l_program = static_cast< GlShaderProgram & >( m_program );

		switch ( p_type )
		{
		case FrameVariableType::eInt:
			l_return = GlFrameVariableCreator< FrameVariableType::eInt >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eInt >();
			break;

		case FrameVariableType::eUInt:
			l_return = GlFrameVariableCreator< FrameVariableType::eUInt >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eUInt >();
			break;

		case FrameVariableType::eFloat:
			l_return = GlFrameVariableCreator< FrameVariableType::eFloat >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eFloat >();
			break;

		case FrameVariableType::eDouble:
			l_return = GlFrameVariableCreator< FrameVariableType::eDouble >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eDouble >();
			break;

		case FrameVariableType::eSampler:
			l_return = GlFrameVariableCreator< FrameVariableType::eSampler >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eSampler >();
			break;

		case FrameVariableType::eVec2i:
			l_return = GlFrameVariableCreator< FrameVariableType::eVec2i >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eVec2i >();
			break;

		case FrameVariableType::eVec3i:
			l_return = GlFrameVariableCreator< FrameVariableType::eVec3i >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eVec3i >();
			break;

		case FrameVariableType::eVec4i:
			l_return = GlFrameVariableCreator< FrameVariableType::eVec4i >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eVec4i >();
			break;

		case FrameVariableType::eVec2ui:
			l_return = GlFrameVariableCreator< FrameVariableType::eVec2ui >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eVec2ui >();
			break;

		case FrameVariableType::eVec3ui:
			l_return = GlFrameVariableCreator< FrameVariableType::eVec3ui >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eVec3ui >();
			break;

		case FrameVariableType::eVec4ui:
			l_return = GlFrameVariableCreator< FrameVariableType::eVec4ui >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eVec4ui >();
			break;

		case FrameVariableType::eVec2f:
			l_return = GlFrameVariableCreator< FrameVariableType::eVec2f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eVec2f >();
			break;

		case FrameVariableType::eVec3f:
			l_return = GlFrameVariableCreator< FrameVariableType::eVec3f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eVec3f >();
			break;

		case FrameVariableType::eVec4f:
			l_return = GlFrameVariableCreator< FrameVariableType::eVec4f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eVec4f >();
			break;

		case FrameVariableType::eVec2d:
			l_return = GlFrameVariableCreator< FrameVariableType::eVec2d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eVec2d >();
			break;

		case FrameVariableType::eVec3d:
			l_return = GlFrameVariableCreator< FrameVariableType::eVec3d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eVec3d >();
			break;

		case FrameVariableType::eVec4d:
			l_return = GlFrameVariableCreator< FrameVariableType::eVec4d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eVec4d >();
			break;

		case FrameVariableType::eMat2x2f:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat2x2f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat2x2f >();
			break;

		case FrameVariableType::eMat2x3f:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat2x3f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat2x3f >();
			break;

		case FrameVariableType::eMat2x4f:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat2x4f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat2x4f >();
			break;

		case FrameVariableType::eMat3x2f:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat3x2f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat3x2f >();
			break;

		case FrameVariableType::eMat3x3f:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat3x3f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat3x3f >();
			break;

		case FrameVariableType::eMat3x4f:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat3x4f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat3x4f >();
			break;

		case FrameVariableType::eMat4x2f:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat4x2f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat4x2f >();
			break;

		case FrameVariableType::eMat4x3f:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat4x3f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat4x3f >();
			break;

		case FrameVariableType::eMat4x4f:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat4x4f >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat4x4f >();
			break;

		case FrameVariableType::eMat2x2d:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat2x2d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat2x2d >();
			break;

		case FrameVariableType::eMat2x3d:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat2x3d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat2x3d >();
			break;

		case FrameVariableType::eMat2x4d:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat2x4d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat2x4d >();
			break;

		case FrameVariableType::eMat3x2d:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat3x2d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat3x2d >();
			break;

		case FrameVariableType::eMat3x3d:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat3x3d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat3x3d >();
			break;

		case FrameVariableType::eMat3x4d:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat3x4d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat3x4d >();
			break;

		case FrameVariableType::eMat4x2d:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat4x2d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat4x2d >();
			break;

		case FrameVariableType::eMat4x3d:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat4x3d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat4x3d >();
			break;

		case FrameVariableType::eMat4x4d:
			l_return = GlFrameVariableCreator< FrameVariableType::eMat4x4d >( GetOpenGl(), l_program, p_occurences );
			l_pApplyer = GlApplyerCreator< FrameVariableType::eMat4x4d >();
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
		GetOpenGl().GetIntegerv( GlMax::eUniformBufferBindings, &l_max );

		if ( int( m_index ) < l_max )
		{
			if ( GetOpenGl().HasUbo() && l_index == GlInvalidIndex )
			{
				GetOpenGl().UseProgram( l_program.GetGlName() );
				m_uniformBlockIndex = GetOpenGl().GetUniformBlockIndex( l_program.GetGlName(), string::string_cast< char >( m_name ).c_str() );

				if ( m_uniformBlockIndex != int( GlInvalidIndex ) )
				{
					GetOpenGl().GetActiveUniformBlockiv( l_program.GetGlName(), m_uniformBlockIndex, GlUniformBlockValue::eDataSize, &m_uniformBlockSize );
					m_glBuffer.Create();
					m_glBuffer.InitialiseStorage( m_uniformBlockSize, BufferAccessType::eDynamic, BufferAccessNature::eDraw );
					m_glBuffer.Bind();
					GetOpenGl().BindBufferBase( GlBufferTarget::eUniform, m_index, m_glBuffer.GetGlName() );
					GetOpenGl().UniformBlockBinding( l_program.GetGlName(), m_uniformBlockIndex, m_index );
					m_buffer.resize( m_uniformBlockSize );
					std::vector< const char * > l_names;

					for ( auto l_variable : m_listVariables )
					{
						char * l_szChar = new char[l_variable->GetName().size() + 1];
						l_szChar[l_variable->GetName().size()] = 0;
#if defined( _MSC_VER )
						strncpy_s( l_szChar, l_variable->GetName().size() + 1, string::string_cast< char >( l_variable->GetName() ).c_str(), l_variable->GetName().size() );
#else
						strncpy( l_szChar, string::string_cast< char >( l_variable->GetName() ).c_str(), l_variable->GetName().size() );
#endif
						l_names.push_back( l_szChar );
					}

					UIntArray l_indices( l_names.size() );
					GetOpenGl().GetUniformIndices( l_program.GetGlName(), int( l_names.size() ), l_names.data(), l_indices.data() );

					for ( auto l_name : l_names )
					{
						delete [] l_name;
					}

					int i = 0;

					for ( auto l_variable : m_listVariables )
					{
						uint32_t l_index = l_indices[i++];
						int l_offset = 0;
						int l_stride = 0;

						if ( l_index != GlInvalidIndex )
						{
							GetOpenGl().GetActiveUniformsiv( l_program.GetGlName(), 1, &l_index, GlUniformValue::eOffset, &l_offset );
							GetOpenGl().GetActiveUniformsiv( l_program.GetGlName(), 1, &l_index, GlUniformValue::eArrayStride, &l_stride );
							l_variable->link( &m_buffer[l_offset], l_stride < 0 ? 0u : uint32_t( l_stride ) );
						}
					}

					m_glBuffer.Unbind();
					m_glBuffer.Upload( 0u, m_uniformBlockSize, m_buffer.data() );

					for ( auto & l_variable : m_listVariables )
					{
						l_variable->SetChanged( false );
					}

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
		m_uniformBlockIndex = int( GlInvalidIndex );
		m_uniformBlockSize = 0;
		m_glBuffer.Destroy();
	}

	void GlFrameVariableBuffer::DoBind( uint32_t p_index )
	{
		bool l_changed = m_listVariables.end() != std::find_if( m_listVariables.begin(), m_listVariables.end(), []( FrameVariableSPtr p_variable )
		{
			return p_variable->IsChanged();
		} );

		if ( l_changed )
		{
			if ( m_uniformBlockIndex != int( GlInvalidIndex ) )
			{
				m_glBuffer.Upload( 0u, m_uniformBlockSize, m_buffer.data() );

				for ( auto & l_variable : m_listVariables )
				{
					l_variable->SetChanged( false );
				}
			}
			else
			{
				for ( auto l_variable : m_listInitialised )
				{
					l_variable->Bind();
				}
			}
		}
	}

	void GlFrameVariableBuffer::DoUnbind( uint32_t p_index )
	{
		if ( m_uniformBlockIndex != int( GlInvalidIndex ) )
		{
			m_glBuffer.Unbind();
		}
	}
}
