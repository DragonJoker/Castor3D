#include "GlShaderSource.hpp"

#ifdef max
#	undef max
#	undef min
#	undef abs
#endif

#ifdef OUT
#	undef OUT
#endif

#ifdef IN
#	undef IN
#endif

namespace GlRender
{
	namespace GLSL
	{
		//*****************************************************************************************

		Expr::Expr()
			: m_writer( NULL )
		{
		}

		Expr::Expr( int p_value )
			: m_writer( NULL )
		{
			m_value << p_value;
		}

		Expr::Expr( float p_value )
			: m_writer( NULL )
		{
			m_value << p_value;
		}

		Expr::Expr( double p_value )
			: m_writer( NULL )
		{
			m_value << p_value;
		}

		Expr::Expr( GlslWriter * p_writer )
			: m_writer( p_writer )
		{
		}

		Expr::Expr( GlslWriter * p_writer, Castor::String const & p_init )
			: m_writer( p_writer )
		{
			m_value << p_init;
		}

		Expr::Expr( Expr const & p_rhs )
			: m_writer( p_rhs.m_writer )
		{
			m_value << p_rhs.m_value.rdbuf();
		}

		Expr & Expr::operator=( Expr const & p_rhs )
		{
			if ( !m_writer )
			{
				m_writer = p_rhs.m_writer;
			}

			return *this;
		}

		//*****************************************************************************************

		Type::Type( Castor::String const & p_type )
			: Expr()
			, m_type( p_type )
		{
		}

		Type::Type( Castor::String const & p_type, GlslWriter * p_writer, Castor::String const & p_name )
			: Expr( p_writer )
			, m_name( p_name )
			, m_type( p_type )
		{
		}

		Type::Type( Type const & p_rhs )
			: Expr( p_rhs )
			, m_name( p_rhs.m_name )
			, m_type( p_rhs.m_type )
		{
		}

		Type & Type::operator=( Type const & p_rhs )
		{
			Expr::operator=( p_rhs );

			if ( m_name.empty() )
			{
				m_name = p_rhs.m_name;
			}

			return *this;
		}

		Type::operator Castor::String()const
		{
			Castor::String l_return = m_value.str();

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
				m_value.str( Castor::String() );
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

		GlslBool operator==( Type const & p_a, Type const & p_b )
		{
			GlslBool l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " == " ) << Castor::String( p_b );
			return l_return;
		}

		GlslBool operator!=( Type const & p_a, Type const & p_b )
		{
			GlslBool l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " != " ) << Castor::String( p_b );
			return l_return;
		}

		Type operator+( Type const & p_a, Type const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " + " ) << Castor::String( p_b );
			return l_return;
		}

		Type operator-( Type const & p_a, Type const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " - " ) << Castor::String( p_b );
			return l_return;
		}

		Type operator*( Type const & p_a, Type const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " * " ) << Castor::String( p_b );
			return l_return;
		}

		Type operator/( Type const & p_a, Type const & p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " / " ) << Castor::String( p_b );
			return l_return;
		}

		Type operator+( Type const & p_a, float p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " + " ) << Castor::str_utils::to_string( p_b );
			return l_return;
		}

		Type operator-( Type const & p_a, float p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " - " ) << Castor::str_utils::to_string( p_b );
			return l_return;
		}

		Type operator*( Type const & p_a, float p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " * " ) << Castor::str_utils::to_string( p_b );
			return l_return;
		}

		Type operator/( Type const & p_a, float p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " / " ) << Castor::str_utils::to_string( p_b );
			return l_return;
		}

		Type operator+( Type const & p_a, int p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " + " ) << Castor::str_utils::to_string( p_b );
			return l_return;
		}

		Type operator-( Type const & p_a, int p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " - " ) << Castor::str_utils::to_string( p_b );
			return l_return;
		}

		Type operator*( Type const & p_a, int p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " * " ) << Castor::str_utils::to_string( p_b );
			return l_return;
		}

		Type operator/( Type const & p_a, int p_b )
		{
			Type l_return( p_a.m_type, p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " / " ) << Castor::str_utils::to_string( p_b );
			return l_return;
		}

		//*****************************************************************************************

		IndentBlock::IndentBlock( GlslWriter & p_writter )
			: m_stream( p_writter.m_stream )
		{
			using namespace Castor;
			m_stream << cuT( "{" );
			m_indent = format::get_indent( m_stream );
			m_stream << format::indent( m_indent + 4 );
			m_stream << std::endl;
		}

		IndentBlock::~IndentBlock()
		{
			using namespace Castor;
			m_stream << format::indent( m_indent );
			m_stream << cuT( "}" );
		}

		//*****************************************************************************************

		Ubo::Ubo( GlslWriter & p_writer, Castor::String const & p_name )
			: m_writer( p_writer )
			, m_name( p_name )
			, m_block( NULL )
		{
			if ( m_writer.m_gl.HasUbo() )
			{
				m_writer << StdLayout { 140 } << Uniform() << p_name << Endl();
				m_writer.m_uniform.clear();
				m_block = new IndentBlock( m_writer );
			}
		}

		void Ubo::End()
		{
			delete m_block;
			m_block = NULL;
			m_writer.m_uniform = cuT( "uniform " );

			if ( m_writer.m_gl.HasUbo() )
			{
				m_writer << cuT( ";" );
			}

			m_writer << Endl();
		}

		//*****************************************************************************************

		Struct::Struct( GlslWriter & p_writer, Castor::String const & p_name )
			: m_writer( p_writer )
			, m_name( p_name )
			, m_block( NULL )
		{
			m_writer << cuT( "struct " ) << p_name << Endl();
			m_block = new IndentBlock( m_writer );
		}

		void Struct::End()
		{
			delete m_block;
			m_block = NULL;
			m_writer << cuT( ";" ) << Endl();
		}

		//*****************************************************************************************

		GlslWriter::GlslWriter( OpenGl & p_gl, Castor3D::eSHADER_TYPE p_type )
			: m_variables( GLSL::VariablesBase::Get( p_gl ) )
			, m_constants( GLSL::ConstantsBase::Get( p_gl ) )
			, m_keywords( GLSL::KeywordsBase::Get( p_gl ) )
			, m_attributeIndex( 0 )
			, m_layoutIndex( 0 )
			, m_type( p_type )
			, m_gl( p_gl )
			, m_uniform( cuT( "uniform " ) )
		{
		}

		Castor::String GlslWriter::Finalise()
		{
			return m_stream.str();
		}

		void GlslWriter::WriteAssign( Type const & p_lhs, Type const & p_rhs )
		{
			m_stream << Castor::String( p_lhs ) << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
		}

		void GlslWriter::WriteAssign( Type const & p_lhs, int const & p_rhs )
		{
			m_stream << Castor::String( p_lhs ) << cuT( " = " ) << Castor::str_utils::to_string( p_rhs ) << cuT( ";" ) << std::endl;
		}

		void GlslWriter::WriteAssign( Type const & p_lhs, float const & p_rhs )
		{
			m_stream << Castor::String( p_lhs ) << cuT( " = " ) << Castor::str_utils::to_string( p_rhs ) << cuT( ";" ) << std::endl;
		}

		void GlslWriter::For( Type const & p_init, Expr const & p_cond, Expr const & p_incr, std::function< void( Type ) > p_function )
		{
			m_stream << cuT( "for( " ) << Castor::String( p_init ) << cuT( "; " ) << p_cond.m_value.rdbuf() << cuT( "; " ) << p_incr.m_value.rdbuf() << cuT( " )" ) << std::endl;
			{
				IndentBlock l_block( *this );
				p_function( p_init );
			}
			m_stream << std::endl;
		}

		GlslWriter & GlslWriter::If( Expr const & p_cond, std::function< void() > p_function )
		{
			m_stream << cuT( "if( " ) << p_cond.m_value.rdbuf() << cuT( " )" ) << std::endl;
			{
				IndentBlock l_block( *this );
				p_function();
			}
			m_stream << std::endl;
			return *this;
		}

		GlslWriter & GlslWriter::ElseIf( Expr const & p_cond, std::function< void() > p_function )
		{
			m_stream << cuT( "else if( " ) << p_cond.m_value.rdbuf() << cuT( " )" ) << std::endl;
			{
				IndentBlock l_block( *this );
				p_function();
			}
			m_stream << std::endl;
			return *this;
		}

		void GlslWriter::Else( std::function< void() > p_function )
		{
			m_stream << cuT( "else" ) << std::endl;
			{
				IndentBlock l_block( *this );
				p_function();
			}
			m_stream << std::endl;
		}

		Struct GlslWriter::GetStruct( Castor::String const & p_name )
		{
			return Struct( *this, p_name );
		}

		Ubo GlslWriter::GetUbo( Castor::String const & p_name )
		{
			return Ubo( *this, p_name );
		}

		void GlslWriter::EmitVertex()
		{
			m_stream << cuT( "EmitVertex();" ) << std::endl;
		}

		void GlslWriter::EndPrimitive()
		{
			m_stream << cuT( "EndPrimitive();" ) << std::endl;
		}

		Vec4 GlslWriter::Texture1D( Sampler1D const & p_sampler, Type const & p_value )
		{
			return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture1D(), p_sampler, p_value );
		}

		Vec4 GlslWriter::Texture2D( Sampler2D const & p_sampler, Type const & p_value )
		{
			return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture2D(), p_sampler, p_value );
		}

		Vec4 GlslWriter::Texture3D( Sampler3D const & p_sampler, Type const & p_value )
		{
			return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture3D(), p_sampler, p_value );
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

		GlslWriter & GlslWriter::operator<<( StdLayout const & p_rhs )
		{
			m_stream << m_keywords->GetStdLayout( p_rhs.m_index );
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
				m_stream << m_uniform;
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

		//*****************************************************************************************

		Vec4 operator*( Vec4 const & p_a, Mat4 const & p_b )
		{
			Vec4 l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " * " ) << Castor::String( p_b );
			return l_return;
		}

		Vec4 operator*( Mat4 const & p_a, Vec4 const & p_b )
		{
			Vec4 l_return( p_a.m_writer );
			l_return.m_value << Castor::String( p_a ) << cuT( " * " ) << Castor::String( p_b );
			return l_return;
		}

		Vec4 texture1D( Sampler1D const & p_sampler, Type const & p_value )
		{
			return p_sampler.m_writer->Texture1D( p_sampler, p_value );
		}

		Vec4 texture2D( Sampler2D const & p_sampler, Type const & p_value )
		{
			return p_sampler.m_writer->Texture2D( p_sampler, p_value );
		}

		Vec4 texture3D( Sampler3D const & p_sampler, Type const & p_value )
		{
			return p_sampler.m_writer->Texture3D( p_sampler, p_value );
		}

		Vec2 reflect( Vec2 const & p_a, Type const & p_b )
		{
			return WriteFunctionCall< Vec2 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
		}

		Vec3 reflect( Vec3 const & p_a, Type const & p_b )
		{
			return WriteFunctionCall< Vec3 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
		}

		Vec4 reflect( Vec4 const & p_a, Type const & p_b )
		{
			return WriteFunctionCall< Vec4 >( p_a.m_writer, cuT( "reflect" ), p_a, p_b );
		}

		inline Float length( Type const & p_value )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "length" ), p_value );
		}

		inline Float radians( Type const & p_value )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "radians" ), p_value );
		}

		inline Float cos( Type const & p_value )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "cos" ), p_value );
		}

		inline Float sin( Type const & p_value )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "sin" ), p_value );
		}

		inline Float tan( Type const & p_value )
		{
			return WriteFunctionCall< Float >( p_value.m_writer, cuT( "tan" ), p_value );
		}

		//*****************************************************************************************

		namespace BlinnPhong
		{
			Vec4 ComputeLightDirection( InParam< Light > const & p_light, InParam< Vec3 > const & p_position, InParam< Mat4 > const & p_mtxModelView )
			{
				GlslWriter * l_pWriter = p_light.m_writer;
				GlslWriter & l_writer = *l_pWriter;
				LOCALE( l_writer, Vec4, l_v4Return );

				IF( l_writer, p_light.m_iType() != Int( 0 ) )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_direction, p_position - p_light.m_v4Position().xyz() );
					LOCALE_ASSIGN( l_writer, Float, l_distance, length( l_direction ) );
					l_v4Return.xyz() = normalize( l_direction );
					LOCALE_ASSIGN( l_writer, Float, l_attenuation, p_light.m_v3Attenuation().x()
								   + p_light.m_v3Attenuation().y() * l_distance
								   + p_light.m_v3Attenuation().z() * l_distance * l_distance );
					l_v4Return.w() = l_attenuation;

					IF( l_writer, p_light.m_fCutOff() <= Float( 90.0f ) ) // spotlight?
					{
						LOCALE( l_writer, Float, l_clampedCosine ) = max( Float( 0.0f ), dot( neg( l_v4Return.xyz() ), l_writer.Paren( vec4( Float( 0.0f ), 0.0f, 1.0f, 0.0f ) * p_light.m_mtx4Orientation() ).xyz() ) );

						IF( l_writer, l_clampedCosine < cos( radians( p_light.m_fCutOff() ) ) )
						{
							l_v4Return.w() = Float( 0.0f );
						}
						ELSE
						{
							l_v4Return.w() = l_v4Return.w() * pow( l_clampedCosine, p_light.m_fExponent() );
						}
						FI
					}
					FI
				}
				ELSE
				{
					l_v4Return = vec4( p_light.m_v4Position().xyz(), 1.0 );
				}
				FI

				l_writer.Return( l_v4Return );
				return l_v4Return;
			}

			Void Bump( InParam< Vec3 > const & p_v3T, InParam< Vec3 > const & p_v3B, InParam< Vec3 > const & p_v3N, InOutParam< Vec3 > & p_lightDir, InOutParam< Float > & p_fAttenuation )
			{
				GlslWriter * l_pWriter = p_v3T.m_writer;
				GlslWriter & l_writer = *l_pWriter;
				LOCALE_ASSIGN( l_writer, Float, l_fInvRadius, Float( 0.02f ) );
				p_lightDir = vec3( dot( p_lightDir, p_v3T ), dot( p_lightDir, p_v3B ), dot( p_lightDir, p_v3N ) );
				LOCALE_ASSIGN( l_writer, Float, l_fSqrLength, dot( p_lightDir, p_lightDir ) );
				p_lightDir = p_lightDir * inversesqrt( l_fSqrLength );
				p_fAttenuation *= clamp( Float( 1.0f ) - l_fInvRadius * sqrt( l_fSqrLength ), 0.0, 1.0 );
				return Void();
			}

			Float ComputeFresnel( InParam< Float > const & p_lambert, InParam< Vec3 > const & p_direction, InParam< Vec3 > const & p_normal, InParam< Vec3 > const & p_eye, InParam< Float > const & p_shininess, InOutParam< Vec3 > & p_specular )
			{
				GlslWriter * l_pWriter = p_lambert.m_writer;
				GlslWriter & l_writer = *l_pWriter;
				LOCALE_ASSIGN( l_writer, Vec3, l_lightReflect, normalize( reflect( p_direction, p_normal ) ) );
				LOCALE_ASSIGN( l_writer, Float, l_fresnel, dot( p_eye, l_lightReflect ) );
				l_fresnel = pow( l_fresnel, p_shininess );
				//Float l_fresnel = pow( 1.0 - clamp( dot( normalize( p_direction + p_eye ), p_eye ), 0.0, 1.0 ), 5.0 );
				//p_specular = clamp( mix( vec3( c3d_v4MatSpecular ), vec3( 1.0 ), l_fresnel ), 0.0, 1.0 );
				//l_fresnel = pow( clamp( dot( p_eye, l_lightReflect ), 0.0, 1.0 ), p_shininess );
				////l_fFresnel = pow( max( 0.0, dot( l_lightReflect, p_eye ) ), p_shininess );
				l_writer.Return( l_fresnel );
				return l_fresnel;
			}
		}

		void BlinnPhongLightingModel::Declare_ComputeLightDirection( GlslWriter & p_writer )
		{
			p_writer.Implement_Function< Vec4 >( cuT( "ComputeLightDirection" ), &BlinnPhong::ComputeLightDirection,
												 InParam< Light >( &p_writer, cuT( "p_light" ) ),
												 InParam< Vec3 >( &p_writer, cuT( "p_position" ) ),
												 InParam< Mat4 >( &p_writer, cuT( "p_mtxModelView" ) ) );
		}

		void BlinnPhongLightingModel::Declare_Bump( GlslWriter & p_writer )
		{
			InOutParam< Vec3 > p_lightDir( &p_writer, cuT( "p_lightDir" ) );
			InOutParam< Float > p_fAttenuation( &p_writer, cuT( "p_fAttenuation" ) );
			p_writer.Implement_Function< Void >( cuT( "Bump" ), &BlinnPhong::Bump,
												 InParam< Vec3 >( &p_writer, cuT( "p_v3T" ) ),
												 InParam< Vec3 >( &p_writer, cuT( "p_v3B" ) ),
												 InParam< Vec3 >( &p_writer, cuT( "p_v3N" ) ),
												 p_lightDir,
												 p_fAttenuation );
		}

		void BlinnPhongLightingModel::Declare_ComputeFresnel( GlslWriter & p_writer )
		{
			InOutParam< Vec3 > p_specular( &p_writer, cuT( "p_specular" ) );
			p_writer.Implement_Function< Float >( cuT( "ComputeFresnel" ), &BlinnPhong::ComputeFresnel,
												  InParam< Float >( &p_writer, cuT( "p_lambert" ) ),
												  InParam< Vec3 >( &p_writer, cuT( "p_direction" ) ),
												  InParam< Vec3 >( &p_writer, cuT( "p_normal" ) ),
												  InParam< Vec3 >( &p_writer, cuT( "p_eye" ) ),
												  InParam< Float >( &p_writer, cuT( "p_shininess" ) ),
												  p_specular );
		}
	}
}
