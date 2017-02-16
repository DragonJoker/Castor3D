#include "Common/OpenGlES3.hpp"
#include "Shader/GlES3ShaderProgram.hpp"

namespace GlES3Render
{
	namespace details
	{
		template< Castor3D::UniformType Type >
		struct VariableBinder
		{
			template< typename T >
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, T const * p_value, uint32_t p_occurences )
			{
				CASTOR_EXCEPTION( "VariableBinder - Unsupported arguments" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eInt >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform1v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eSampler >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform1v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eVec2i >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eVec3i >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eVec4i >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, int const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eUInt >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform1v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eVec2ui >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eVec3ui >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eVec4ui >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, uint32_t const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eFloat >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform1v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eVec2f >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eVec3f >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eVec4f >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4v( p_glName, p_occurences, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eDouble >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eVec2d >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eVec3d >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eVec4d >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat2x2f >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2x2v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat2x3f >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2x3v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat2x4f >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform2x4v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat3x2f >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3x2v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat3x3f >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3x3v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat3x4f >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform3x4v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat4x2f >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4x2v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat4x3f >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4x3v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat4x4f >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, float const * p_value, uint32_t p_occurences )
			{
				p_gl.SetUniform4x4v( p_glName, p_occurences, false, p_value );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat2x2d >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat2x3d >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat2x4d >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat3x2d >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat3x3d >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat3x4d >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat4x2d >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat4x3d >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};

		template<> struct VariableBinder< Castor3D::UniformType::eMat4x4d >
		{
			static inline void Update( OpenGlES3 const & p_gl, int p_glName, double const * p_value, uint32_t p_occurences )
			{
				FAILURE( "OpenGL ES 3.X doesn't support double uniforms" );
			}
		};
	}

	template< Castor3D::UniformType Type >
	GlES3PushUniform< Type >::GlES3PushUniform( OpenGlES3 & p_gl, GlES3ShaderProgram & p_program, uint32_t p_occurences )
		: Castor3D::TPushUniform< Type >( p_program, p_occurences )
		, Holder( p_gl )
	{
	}

	template< Castor3D::UniformType Type >
	GlES3PushUniform< Type >::~GlES3PushUniform()
	{
	}

	template< Castor3D::UniformType Type >
	bool GlES3PushUniform< Type >::DoInitialise()
	{
		m_glName = GetOpenGlES3().GetUniformLocation( reinterpret_cast< GlES3ShaderProgram & >( this->m_program ).GetGlES3Name()
			, Castor::string::string_cast< char >( this->GetUniform().GetName() ).c_str() );
		return m_glName != GlES3InvalidIndex;
	}

	template< Castor3D::UniformType Type >
	void GlES3PushUniform< Type >::DoUpdate()const
	{
		details::VariableBinder< Type >::Update( GetOpenGlES3(), m_glName, this->GetUniform().GetValues(), this->GetUniform().GetOccCount() );
	}
}
