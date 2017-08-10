#include "Common/OpenGl.hpp"
#include "Shader/GlShaderProgram.hpp"

namespace GlRender
{
	namespace details
	{
		template< castor3d::UniformType Type >
		struct VariableBinder
		{
			template< typename T >
			static inline void update( OpenGl const & p_gl, int p_glName, T const * p_value, uint32_t p_occurences )
			{
				CASTOR_EXCEPTION( "VariableBinder - Unsupported arguments" );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eInt >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform1v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eSampler >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform1v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eVec2i >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eVec3i >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eVec4i >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eUInt >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform1v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eVec2ui >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eVec3ui >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eVec4ui >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eFloat >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform1v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eVec2f >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eVec3f >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eVec4f >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eDouble >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform1v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eVec2d >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eVec3d >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eVec4d >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat2x2f >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2x2v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat2x3f >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2x3v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat2x4f >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2x4v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat3x2f >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3x2v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat3x3f >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3x3v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat3x4f >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3x4v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat4x2f >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4x2v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat4x3f >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4x3v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat4x4f >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4x4v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat2x2d >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2x2v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat2x3d >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2x3v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat2x4d >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2x4v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat3x2d >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3x2v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat3x3d >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3x3v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat3x4d >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3x4v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat4x2d >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4x2v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat4x3d >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4x3v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< castor3d::UniformType::eMat4x4d >
		{
			static inline void update( OpenGl const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4x4v( p_glName, p_occurences, false, p_value );
			}
		};
	}

	template< castor3d::UniformType Type >
	GlPushUniform< Type >::GlPushUniform( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences )
		: castor3d::TPushUniform< Type >( p_program, p_occurences )
		, Holder( p_gl )
	{
	}

	template< castor3d::UniformType Type >
	GlPushUniform< Type >::~GlPushUniform()
	{
	}

	template< castor3d::UniformType Type >
	bool GlPushUniform< Type >::doInitialise()
	{
		m_glName = getOpenGl().GetUniformLocation( reinterpret_cast< GlShaderProgram & >( this->m_program ).getGlName()
			, castor::string::stringCast< char >( this->getUniform().getName() ).c_str() );
		return m_glName != GlInvalidIndex;
	}

	template< castor3d::UniformType Type >
	void GlPushUniform< Type >::doUpdate()const
	{
		details::VariableBinder< Type >::update( getOpenGl(), m_glName, this->getUniform().getValues(), this->getUniform().getOccCount() );
	}
}
