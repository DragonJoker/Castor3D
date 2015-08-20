#include "GlShaderSource.hpp"

namespace GlRender
{
	namespace GLSL
	{
		//*****************************************************************************************

		Lighting::Lighting()
		{
			m_strLightsDeclaration =
				cuT( "struct Light\n" )
				cuT( "{\n" )
				cuT( "	vec4 m_v4Ambient;\n" )
				cuT( "	vec4 m_v4Diffuse;\n" )
				cuT( "	vec4 m_v4Specular;\n" )
				cuT( "	vec4 m_v4Position;\n" )
				cuT( "	int m_iType;\n" )
				cuT( "	vec3 m_v3Attenuation;\n" )
				cuT( "	mat4 m_mtx4Orientation;\n" )
				cuT( "	float m_fExponent;\n" )
				cuT( "	float m_fCutOff;\n" )
				cuT( "};\n" )
				cuT( "Light GetLight( int p_iIndex )\n" )
				cuT( "{\n" )
				cuT( "	float l_fFactor = (float( p_iIndex ) * 0.01);\n" )
				cuT( "	Light l_lightReturn;\n" )
				cuT( "	l_lightReturn.m_v4Ambient = <texture1D>( c3d_sLights, l_fFactor + (0 * 0.001) + 0.0005 );\n" )
				cuT( "	l_lightReturn.m_v4Diffuse = <texture1D>( c3d_sLights, l_fFactor + (1 * 0.001) + 0.0005 );\n" )
				cuT( "	l_lightReturn.m_v4Specular = <texture1D>( c3d_sLights, l_fFactor + (2 * 0.001) + 0.0005 );\n" )
				cuT( "	vec4 l_v4Position = <texture1D>( c3d_sLights, l_fFactor + (3 * 0.001) + 0.0005 );\n" )
				cuT( "	l_lightReturn.m_v3Attenuation = <texture1D>( c3d_sLights, l_fFactor + (4 * 0.001) + 0.0005 ).xyz;\n" )
				cuT( "	vec4 l_v4A = <texture1D>( c3d_sLights, l_fFactor + (5 * 0.001) + 0.0005 );\n" )
				cuT( "	vec4 l_v4B = <texture1D>( c3d_sLights, l_fFactor + (6 * 0.001) + 0.0005 );\n" )
				cuT( "	vec4 l_v4C = <texture1D>( c3d_sLights, l_fFactor + (7 * 0.001) + 0.0005 );\n" )
				cuT( "	vec4 l_v4D = <texture1D>( c3d_sLights, l_fFactor + (8 * 0.001) + 0.0005 );\n" )
				cuT( "	vec2 l_v2Spot = <texture1D>( c3d_sLights, l_fFactor + (9 * 0.001) + 0.0005 ).xy;\n" )
				cuT( "	l_lightReturn.m_v4Position = vec4( l_v4Position.z, l_v4Position.y, l_v4Position.x, 0.0 );\n" )
				cuT( "	l_lightReturn.m_iType = int( l_v4Position.w );\n" )
				cuT( "	l_lightReturn.m_mtx4Orientation = mat4( l_v4A, l_v4B, l_v4C, l_v4D );\n" )
				cuT( "	l_lightReturn.m_fExponent = l_v2Spot.x;\n" )
				cuT( "	l_lightReturn.m_fCutOff = l_v2Spot.x;\n" )
				cuT( "	return l_lightReturn;\n" )
				cuT( "}\n" );
		}

		//*****************************************************************************************

		BlinnPhong::BlinnPhong()
		{
			m_blinnPhongFresnelShit =
				// Computes BlinnPhong components : direction and attenuation
				cuT( "vec4 ComputeLightDirection( in Light p_light, in vec3 p_position, in <mat4> p_mtxModelView )\n" )
				cuT( "{\n" )
				cuT( "	vec4 l_v4Return;\n" )
				cuT( "	\n" )
				cuT( "	if( 0 != p_light.m_iType ) // non directional light?\n" )
				cuT( "	{\n" )
				cuT( "		vec3 l_direction = p_position - p_light.m_v4Position.xyz;\n" )
				cuT( "		float l_distance = length( l_direction );\n" )
				cuT( "		l_v4Return.xyz = normalize( l_direction );\n" )
				cuT( "		float l_attenuation = p_light.m_v3Attenuation.x\n" )
				cuT( "		                    + p_light.m_v3Attenuation.y * l_distance\n" )
				cuT( "		                    + p_light.m_v3Attenuation.z * l_distance * l_distance;\n" )
				cuT( "		l_v4Return.w = l_attenuation;\n" )
				cuT( "		\n" )
				cuT( "		if ( p_light.m_fCutOff <= 90.0 ) // spotlight?\n" )
				cuT( "		{\n" )
				cuT( "			float l_clampedCosine = max( 0.0, dot( -l_v4Return.xyz, ( vec4( 0, 0, 1, 0 ) * p_light.m_mtx4Orientation ).xyz ) );\n" )
				cuT( "			if ( l_clampedCosine < cos( radians( p_light.m_fCutOff ) ) ) // outside of spotlight cone?\n" )
				cuT( "			{\n" )
				cuT( "				l_v4Return.w = 0.0;\n" )
				cuT( "			}\n" )
				cuT( "			else\n" )
				cuT( "			{\n" )
				cuT( "				l_v4Return.w = l_v4Return.w * pow( l_clampedCosine, p_light.m_fExponent );\n" )
				cuT( "			}\n" )
				cuT( "		}\n" )
				cuT( "	}\n" )
				cuT( "	else\n" )
				cuT( "	{\n" )
				cuT( "		l_v4Return = vec4( p_light.m_v4Position.xyz, 1.0 );\n" )
				cuT( "	}\n" )
				cuT( "	\n" )
				cuT( "	return l_v4Return;\n" )
				cuT( "}\n" )
				// Computes Fresnel component
				cuT( "float ComputeFresnel( in float p_lambert, in vec3 p_direction, in vec3 p_normal, in vec3 p_eye, in float p_shininess, inout vec3 p_specular )\n" )
				cuT( "{\n" )
				cuT( "	vec3 l_lightReflect = normalize( reflect( p_direction, p_normal ) );\n" )
				cuT( "	float l_fresnel = dot( p_eye, l_lightReflect );\n" )
				cuT( "	l_fresnel = pow( l_fresnel, p_shininess );\n" )
				//cuT( "	float l_fresnel = pow( 1.0 - clamp( dot( normalize( p_direction + p_eye ), p_eye ), 0.0, 1.0 ), 5.0 );\n" )
				//cuT( "	p_specular = clamp( mix( vec3( c3d_v4MatSpecular ), vec3( 1.0 ), l_fresnel ), 0.0, 1.0 );\n" )
				//cuT( "	l_fresnel = pow( clamp( dot( p_eye, l_lightReflect ), 0.0, 1.0 ), p_shininess );\n" )
				////cuT( "	l_fFresnel = pow( max( 0.0, dot( l_lightReflect, p_eye ) ), p_shininess );\n" )
				cuT( "	return l_fresnel;\n" )
				cuT( "}\n" )
				// Computes normal mapping components
				cuT( "void Bump( in vec3 p_v3T, in vec3 p_v3B, in vec3 p_v3N, inout vec3 p_lightDir, inout float p_fAttenuation )\n" )
				cuT( "{\n" )
				cuT( "	float l_fInvRadius = 0.02;\n" )
				cuT( "	p_lightDir = vec3( dot( p_lightDir, p_v3T ), dot( p_lightDir, p_v3B ), dot( p_lightDir, p_v3N ) );\n" )
				cuT( "	float l_fSqrLength = dot( p_lightDir, p_lightDir );\n" )
				cuT( "	p_lightDir = p_lightDir * inversesqrt( l_fSqrLength );\n" )
				cuT( "	p_fAttenuation *= clamp( 1.0 - l_fInvRadius * sqrt( l_fSqrLength ), 0.0, 1.0 );\n" )
				cuT( "}\n" );
		}

		//*****************************************************************************************

		Type::Type( Castor::String const & p_type )
			: m_writer( NULL )
			, m_type( p_type )
		{
		}

		Type::Type( Castor::String const & p_type, GlslWriter * p_writer, Castor::String const & p_name )
			: m_name( p_name )
			, m_writer( p_writer )
			, m_type( p_type )
		{
		}

		Type::Type( Type const & p_rhs )
			: m_name( p_rhs.m_name )
			, m_writer( p_rhs.m_writer )
			, m_type( p_rhs.m_type )
		{
			m_result << p_rhs.m_result.rdbuf();
		}

		Type & Type::operator=( Type const & p_rhs )
		{
			if ( m_name.empty() )
			{
				m_name = p_rhs.m_name;
			}

			if ( !m_writer )
			{
				m_writer = p_rhs.m_writer;
			}

			m_type = p_rhs.m_type;
			return *this;
		}

		Type::operator Castor::String()const
		{
			Castor::String l_return = m_result.str();

			if ( l_return.empty() )
			{
				if ( m_name.empty() )
				{
					l_return = m_type;
				}
				else
				{
					l_return = m_name;
				}
			}
			else
			{
				std::swap( Castor::StringStream(), m_result );
				l_return = l_return;
			}

			return l_return;
		}

		Type & Type::operator+=( Type const & p_type )
		{
			*m_writer << m_name << cuT( " += " ) << Castor::String( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator-=( Type const & p_type )
		{
			*m_writer << m_name << cuT( " -= " ) << Castor::String( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator*=( Type const & p_type )
		{
			*m_writer << m_name << cuT( " *= " ) << Castor::String( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator/=( Type const & p_type )
		{
			*m_writer << m_name << cuT( " /= " ) << Castor::String( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator+=( float p_type )
		{
			*m_writer << m_name << cuT( " += " ) << Castor::str_utils::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator-=( float p_type )
		{
			*m_writer << m_name << cuT( " -= " ) << Castor::str_utils::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator*=( float p_type )
		{
			*m_writer << m_name << cuT( " *= " ) << Castor::str_utils::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator/=( float p_type )
		{
			*m_writer << m_name << cuT( " /= " ) << Castor::str_utils::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator+=( int p_type )
		{
			*m_writer << m_name << cuT( " += " ) << Castor::str_utils::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator-=( int p_type )
		{
			*m_writer << m_name << cuT( " -= " ) << Castor::str_utils::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator*=( int p_type )
		{
			*m_writer << m_name << cuT( " *= " ) << Castor::str_utils::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type & Type::operator/=( int p_type )
		{
			*m_writer << m_name << cuT( " /= " ) << Castor::str_utils::to_string( p_type ) << cuT( ";" ) << Endl();
			return *this;
		}

		Type const & operator+( Type const & p_a, Type const & p_b )
		{
			p_a.m_result << Castor::String( p_a ) << cuT( " + " ) << Castor::String( p_b );
			return p_a;
		}

		Type const & operator-( Type const & p_a, Type const & p_b )
		{
			p_a.m_result << Castor::String( p_a ) << cuT( " - " ) << Castor::String( p_b );
			return p_a;
		}

		Type const & operator*( Type const & p_a, Type const & p_b )
		{
			p_a.m_result << Castor::String( p_a ) << cuT( " * " ) << Castor::String( p_b );
			return p_a;
		}

		Type const & operator/( Type const & p_a, Type const & p_b )
		{
			p_a.m_result << Castor::String( p_a ) << cuT( " / " ) << Castor::String( p_b );
			return p_a;
		}

		Type const & operator+( Type const & p_a, float p_b )
		{
			p_a.m_result << Castor::String( p_a ) << cuT( " + " ) << Castor::str_utils::to_string( p_b );
			return p_a;
		}

		Type const & operator-( Type const & p_a, float p_b )
		{
			p_a.m_result << Castor::String( p_a ) << cuT( " - " ) << Castor::str_utils::to_string( p_b );
			return p_a;
		}

		Type const & operator*( Type const & p_a, float p_b )
		{
			p_a.m_result << Castor::String( p_a ) << cuT( " * " ) << Castor::str_utils::to_string( p_b );
			return p_a;
		}

		Type const & operator/( Type const & p_a, float p_b )
		{
			p_a.m_result << Castor::String( p_a ) << cuT( " / " ) << Castor::str_utils::to_string( p_b );
			return p_a;
		}

		Type const & operator+( Type const & p_a, int p_b )
		{
			p_a.m_result << Castor::String( p_a ) << cuT( " + " ) << Castor::str_utils::to_string( p_b );
			return p_a;
		}

		Type const & operator-( Type const & p_a, int p_b )
		{
			p_a.m_result << Castor::String( p_a ) << cuT( " - " ) << Castor::str_utils::to_string( p_b );
			return p_a;
		}

		Type const & operator*( Type const & p_a, int p_b )
		{
			p_a.m_result << Castor::String( p_a ) << cuT( " * " ) << Castor::str_utils::to_string( p_b );
			return p_a;
		}

		Type const & operator/( Type const & p_a, int p_b )
		{
			p_a.m_result << Castor::String( p_a ) << cuT( " / " ) << Castor::str_utils::to_string( p_b );
			return p_a;
		}

		//*****************************************************************************************

		Vec4 & Vec4::operator=( Vec4 const & p_rhs )
		{
			*m_writer << m_name << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << Endl();
			return *this;
		}

		//*****************************************************************************************

		IndentBlock::IndentBlock( GlslWriter & p_writter )
			: m_stream( p_writter.m_stream )
		{
			using namespace Castor;
			m_stream << cuT( "{" );
			m_indent = format::get_indent( m_stream );
			m_stream << format::indent( m_indent + 4 );
		}

		IndentBlock::~IndentBlock()
		{
			using namespace Castor;
			m_stream << format::indent( m_indent );
			m_stream << cuT( "}" ) << std::endl;
		}

		//*****************************************************************************************

		Ubo::Ubo( GlslWriter & p_writer, Castor::String const & p_name )
			: m_writer( p_writer )
			, m_name( p_name )
			, m_block( NULL )
		{
			if ( m_writer.m_gl.HasUbo() )
			{
				m_writer << StdLayout() << Uniform() << p_name << Endl();
				m_writer.m_uniform.clear();
				m_block = new IndentBlock( m_writer );
				m_writer << Endl();
			}
		}

		void Ubo::End()
		{
			delete m_block;
			m_block = NULL;
			m_writer.m_uniform = cuT( "uniform" );
			m_writer << Endl();
		}

		//*****************************************************************************************
		
		Function::Function( GlslWriter & p_writer, std::function< void() > p_function )
			: m_writer( p_writer )
		{
			IndentBlock l_block( m_writer );
			p_function();
		}

		//*****************************************************************************************

		GlslWriter::GlslWriter( OpenGl & p_gl, Castor3D::eSHADER_TYPE p_type )
			: m_variables( GLSL::GetVariables( p_gl ) )
			, m_constants( GLSL::GetConstants( p_gl ) )
			, m_keywords( GLSL::GetKeywords( p_gl ) )
			, m_attributeIndex( 0 )
			, m_layoutIndex( 0 )
			, m_type( p_type )
			, m_gl( p_gl )
		{
		}

		Castor::String GlslWriter::Finalise()
		{
			return m_stream.str();
		}
		
		Ubo GlslWriter::GetUbo( Castor::String const & p_name )
		{
			return Ubo( *this, p_name );
		}

		GlslWriter & GlslWriter::operator<<( Version const & p_rhs )
		{
			m_stream << m_keywords->GetVersion();
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Attribute const & p_rhs )
		{
			m_stream << m_keywords->GetAttribute( m_attributeIndex++ ) << cuT( " " );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( IVec2 const & p_rhs )
		{
			m_stream << cuT( "ivec2 " );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( IVec3 const & p_rhs )
		{
			m_stream << cuT( "ivec3 " );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( IVec4 const & p_rhs )
		{
			m_stream << cuT( "ivec4 " );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Vec2 const & p_rhs )
		{
#if CASTOR_USE_DOUBLE
			m_stream << cuT( "dvec2 " );
#else
			m_stream << cuT( "vec2 " );
#endif
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Vec3 const & p_rhs )
		{
#if CASTOR_USE_DOUBLE
			m_stream << cuT( "dvec3 " );
#else
			m_stream << cuT( "vec3 " );
#endif
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Vec4 const & p_rhs )
		{
#if CASTOR_USE_DOUBLE
			m_stream << cuT( "dvec4 " );
#else
			m_stream << cuT( "vec4 " );
#endif
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Mat3 const & p_rhs )
		{
#if CASTOR_USE_DOUBLE
			m_stream << cuT( "dmat3 " );
#else
			m_stream << cuT( "mat3 " );
#endif
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Mat4 const & p_rhs )
		{
#if CASTOR_USE_DOUBLE
			m_stream << cuT( "dmat4 " );
#else
			m_stream << cuT( "mat4 " );
#endif
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( In const & p_rhs )
		{
			m_stream << m_keywords->GetIn() << cuT( " " );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Out const & p_rhs )
		{
			m_stream << m_keywords->GetOut() << cuT( " " );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Void const & p_rhs )
		{
			m_stream << Castor::String( p_rhs );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Sampler1D const & p_rhs )
		{
			m_stream << Castor::String( p_rhs );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Sampler2D const & p_rhs )
		{
			m_stream << Castor::String( p_rhs );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Sampler3D const & p_rhs )
		{
			m_stream << Castor::String( p_rhs );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Int const & p_rhs )
		{
			m_stream << Castor::String( p_rhs );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Float const & p_rhs )
		{
			m_stream << Castor::String( p_rhs );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( StdLayout const & p_rhs )
		{
			m_stream << m_keywords->GetLayout();
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Layout const & p_rhs )
		{
			m_stream << m_keywords->GetLayout( m_layoutIndex );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Uniform const & p_rhs )
		{
			if ( !m_uniform.empty() )
			{
				m_stream << m_uniform << cuT( " " );
			}

			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Legacy_MatrixOut const & p_rhs )
		{
			m_stream << m_variables->GetVertexOutMatrices() << std::endl;
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Legacy_MatrixCopy const & p_rhs )
		{
			m_stream << m_variables->GetVertexMatrixCopy() << std::endl;
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Endl const & p_rhs )
		{
			m_stream << std::endl;
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Main const & p_rhs )
		{
			m_stream << cuT( "void main()" );
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( Castor::String const & p_rhs )
		{
			m_stream << p_rhs;
			return *this;
		}

		GlslWriter & GlslWriter::operator<<( uint32_t const & p_rhs )
		{
			m_stream << p_rhs;
			return *this;
		}
	}
}