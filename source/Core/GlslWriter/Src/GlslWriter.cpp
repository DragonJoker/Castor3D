#include "GlslWriter.hpp"

#include "GlslVec.hpp"
#include "GlslIntrinsics.hpp"

#include <Log/Logger.hpp>

namespace glsl
{
	GlslWriter::GlslWriter( GlslWriterConfig const & config )
		: m_keywords( glsl::KeywordsBase::get( config ) )
		, m_uniform( cuT( "uniform " ) )
		, m_config( config )
	{
		m_stream.imbue( Expr::getLocale() );
		*this << glsl::Version() << endl;

#if !defined( NDEBUG )
		*this << "#pragma optimize(off)" << endl;
		*this << "#pragma debug(on)" << endl;
#endif

		if ( m_config.m_shaderLanguageVersion < 430 )
		{
			*this << "#extension GL_ARB_explicit_attrib_location : enable" << endl;
			*this << "#extension GL_ARB_explicit_uniform_location : enable" << endl;
			*this << "#extension GL_ARB_separate_shader_objects : enable" << endl;
			*this << "#extension GL_ARB_shading_language_420pack : enable" << endl;
		}
	}

	void GlslWriter::registerName( castor::String const & p_name, TypeName p_type )
	{
		auto it = m_registered.find( p_name );

		//if ( it != m_registered.end() )
		//{
		//	std::stringstream text;
		//	text << "A variable with the name [" << p_name << "] is already registered.";
		//	castor::Logger::logWarning( text );
		//}

		m_registered.emplace( p_name, p_type );
	}

	void GlslWriter::checkNameExists( castor::String const & p_name, TypeName p_type )
	{
		auto it = m_registered.find( p_name );

		if ( it == m_registered.end() )
		{
			std::stringstream text;
			text << "No registered variable with the name [" << p_name << "].";
			CASTOR_EXCEPTION( text.str() );
		}
	}

	void GlslWriter::inlineComment( castor::String const & comment )
	{
		m_stream << cuT( "// " ) << comment << std::endl;
	}

	void GlslWriter::multilineComment( castor::String const & comment )
	{
		m_stream << cuT( "/*" ) << comment << "*/" << std::endl;
	}

	void GlslWriter::enableExtension( castor::String const & name, uint32_t inCoreVersion )
	{
		if ( getShaderLanguageVersion() < inCoreVersion
			|| !inCoreVersion )
		{
			m_stream << "#extension " << name << ": enable" << std::endl;
		}
	}

	castor::String GlslWriter::getInstanceID()
	{
		if ( m_config.m_hasInstanceIndex )
		{
			return cuT( "gl_InstanceIndex" );
		}
		else
		{
			return cuT( "gl_InstanceID" );
		}
	}

	castor::String GlslWriter::getVertexID()
	{
		if ( m_config.m_hasVertexIndex )
		{
			return cuT( "gl_VertexIndex" );
		}
		else
		{
			return cuT( "gl_VertexID" );
		}
	}

	Vec2 GlslWriter::adjustTexCoords( Vec2 const & texcoords )
	{
		if ( m_config.m_isTopDown )
		{
			return vec2( texcoords.x(), 1.0_f - texcoords.y() );
		}

		return texcoords;
	}

	Vec4 GlslWriter::rendererScalePosition( Vec4 const & position )
	{
		return writeFunctionCall< Vec4 >( this
			, cuT( "rendererScalePosition" )
			, position );
	}

	Shader GlslWriter::finalise()
	{
		m_shader.setSource( m_stream.str() );
		return m_shader;
	}

	void GlslWriter::writeAssign( Type const & p_lhs, Type const & p_rhs )
	{
		m_stream << castor::String( p_lhs ) << cuT( " = " ) << castor::String( p_rhs ) << cuT( ";" ) << std::endl;
	}

	void GlslWriter::writeAssign( Type const & p_lhs, int const & p_rhs )
	{
		m_stream << castor::String( p_lhs ) << cuT( " = " ) << castor::string::toString( p_rhs, 10, Expr::getLocale() ) << cuT( ";" ) << std::endl;
	}

	void GlslWriter::writeAssign( Type const & p_lhs, unsigned int const & p_rhs )
	{
		m_stream << castor::String( p_lhs ) << cuT( " = " ) << castor::string::toString( p_rhs, 10, Expr::getLocale() ) << cuT( "u;" ) << std::endl;
	}

	void GlslWriter::writeAssign( Type const & p_lhs, float const & p_rhs )
	{
		m_stream << castor::String( p_lhs ) << cuT( " = " ) << castor::string::toString( p_rhs, Expr::getLocale() ) << cuT( ";" ) << std::endl;
	}

	void GlslWriter::forStmt( Type && p_init, Expr const & p_cond, Expr const & p_incr, std::function< void() > p_function )
	{
		m_stream << std::endl;
		m_stream << cuT( "for ( " ) << castor::String( p_init ) << cuT( "; " ) << p_cond.m_value.rdbuf() << cuT( "; " ) << p_incr.m_value.rdbuf() << cuT( " )" ) << std::endl;
		{
			IndentBlock block( *this );
			p_function();
		}
		m_stream << std::endl;
	}

	void GlslWriter::whileStmt( Expr const & p_cond, std::function< void() > p_function )
	{
		m_stream << std::endl;
		m_stream << cuT( "while ( " ) << p_cond.m_value.rdbuf() << cuT( " )" ) << std::endl;
		{
			IndentBlock block( *this );
			p_function();
		}
		m_stream << std::endl;
	}

	GlslWriter & GlslWriter::ifStmt( Expr const & p_cond, std::function< void() > p_function )
	{
		m_stream << std::endl;
		m_stream << cuT( "if ( " ) << p_cond.m_value.rdbuf() << cuT( " )" ) << std::endl;
		{
			IndentBlock block( *this );
			p_function();
		}
		m_stream << std::endl;
		return *this;
	}

	GlslWriter & GlslWriter::elseIfStmt( Expr const & p_cond, std::function< void() > p_function )
	{
		m_stream << cuT( "else if ( " ) << p_cond.m_value.rdbuf() << cuT( " )" ) << std::endl;
		{
			IndentBlock block( *this );
			p_function();
		}
		m_stream << std::endl;
		return *this;
	}

	void GlslWriter::elseStmt( std::function< void() > p_function )
	{
		m_stream << cuT( "else" ) << std::endl;
		{
			IndentBlock block( *this );
			p_function();
		}
		m_stream << std::endl;
	}

	Struct GlslWriter::getStruct( castor::String const & p_name )
	{
		return Struct( *this, p_name );
	}

	void GlslWriter::emitVertex()
	{
		m_stream << cuT( "emitVertex();" ) << std::endl;
	}

	void GlslWriter::endPrimitive()
	{
		m_stream << cuT( "endPrimitive();" ) << std::endl;
	}

	void GlslWriter::discard()
	{
		m_stream << cuT( "discard;" ) << std::endl;
	}

	void GlslWriter::inputGeometryLayout( castor::String const & p_layout )
	{
		m_stream << cuT( "layout( " ) << p_layout << cuT( " ) in;" ) << std::endl;
	}

	void GlslWriter::outputGeometryLayout( castor::String const & p_layout )
	{
		m_stream << cuT( "layout( " ) << p_layout << cuT( " ) out;" ) << std::endl;
	}

	void GlslWriter::outputGeometryLayout( castor::String const & p_layout, uint32_t p_count )
	{
		m_stream << cuT( "layout( " ) << p_layout << cuT( ", max_vertices = " ) << p_count << cuT( " ) out;" ) << std::endl;
	}

	void GlslWriter::outputVertexCount( uint32_t p_count )
	{
		m_stream << cuT( "layout( max_vertices = " ) << p_count << cuT( " ) out;" ) << std::endl;
	}

	void GlslWriter::returnStmt()
	{
		m_stream << cuT( "return;" ) << std::endl;
	}

	Vec4 GlslWriter::texture( Sampler1D const & p_sampler, Float const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1D(), p_sampler, p_value );
	}

	Vec4 GlslWriter::texture( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::texture( Sampler2D const & p_sampler, Vec2 const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2D(), p_sampler, p_value );
	}

	Vec4 GlslWriter::texture( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::texture( Sampler3D const & p_sampler, Vec3 const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3D(), p_sampler, p_value );
	}

	Vec4 GlslWriter::texture( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::texture( SamplerCube const & p_sampler, Vec3 const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCube(), p_sampler, p_value );
	}

	Vec4 GlslWriter::texture( SamplerCube const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCubeLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::textureOffset( Sampler1D const & p_sampler, Float const & p_value, Int const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DOffset(), p_sampler, p_value, p_offset );
	}

	Vec4 GlslWriter::textureOffset( Sampler1D const & p_sampler, Float const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 GlslWriter::textureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DOffset(), p_sampler, p_value, p_offset );
	}

	Vec4 GlslWriter::textureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 GlslWriter::textureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DOffset(), p_sampler, p_value, p_offset );
	}

	Vec4 GlslWriter::textureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 GlslWriter::textureLodOffset( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 GlslWriter::textureLodOffset( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 GlslWriter::textureLodOffset( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 GlslWriter::texelFetch( SamplerBuffer const & p_sampler, Type const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetchBuffer(), p_sampler, p_value );
	}

	Vec4 GlslWriter::texelFetch( Sampler1D const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetch1D(), p_sampler, p_value, p_modif );
	}

	Vec4 GlslWriter::texelFetch( Sampler2D const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetch2D(), p_sampler, p_value, p_modif );
	}

	Vec4 GlslWriter::texelFetch( Sampler3D const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetch3D(), p_sampler, p_value, p_modif );
	}

	Vec4 GlslWriter::texture( Sampler1DArray const & p_sampler, Vec2 const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArray(), p_sampler, p_value );
	}

	Vec4 GlslWriter::texture( Sampler1DArray const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::texture( Sampler2DArray const & p_sampler, Vec3 const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArray(), p_sampler, p_value );
	}

	Vec4 GlslWriter::texture( Sampler2DArray const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCubeArray(), p_sampler, p_value );
	}

	Vec4 GlslWriter::texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCubeArrayLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::textureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayOffset(), p_sampler, p_value, p_offset );
	}

	Vec4 GlslWriter::textureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 GlslWriter::textureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayOffset(), p_sampler, p_value, p_offset );
	}

	Vec4 GlslWriter::textureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 GlslWriter::textureLodOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 GlslWriter::textureLodOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Float GlslWriter::texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadow(), p_sampler, p_value );
	}

	Float GlslWriter::texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowLod(), p_sampler, p_value, p_lod );
	}

	Float GlslWriter::texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadow(), p_sampler, p_value );
	}

	Float GlslWriter::texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowLod(), p_sampler, p_value, p_lod );
	}

	Float GlslWriter::texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeShadow(), p_sampler, p_value );
	}

	Float GlslWriter::texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeShadowLod(), p_sampler, p_value, p_lod );
	}

	Float GlslWriter::textureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowOffset(), p_sampler, p_value, p_offset );
	}

	Float GlslWriter::textureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Float GlslWriter::textureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowOffset(), p_sampler, p_value, p_offset );
	}

	Float GlslWriter::textureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Float GlslWriter::textureLodOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Float GlslWriter::textureLodOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Float GlslWriter::texture( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadow(), p_sampler, p_value );
	}

	Float GlslWriter::texture( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowLod(), p_sampler, p_value, p_lod );
	}

	Float GlslWriter::texture( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadow(), p_sampler, p_value );
	}

	Float GlslWriter::texture( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowLod(), p_sampler, p_value, p_lod );
	}

	Float GlslWriter::texture( SamplerCubeArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_layer )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeArrayShadow(), p_sampler, p_value, p_layer );
	}

	Float GlslWriter::texture( SamplerCubeArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeArrayShadowLod(), p_sampler, p_value, p_layer, p_lod );
	}

	Float GlslWriter::textureOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Int const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowOffset(), p_sampler, p_value, p_offset );
	}

	Float GlslWriter::textureOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Float GlslWriter::textureOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowOffset(), p_sampler, p_value, p_offset );
	}

	Float GlslWriter::textureOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Float GlslWriter::textureLodOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Float GlslWriter::textureLodOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler1D > const & p_sampler, Float const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1D(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2D(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3D(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCube(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCubeLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > GlslWriter::textureLodOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > GlslWriter::textureLodOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > GlslWriter::textureLodOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > GlslWriter::texelFetch( Optional< SamplerBuffer > const & p_sampler, Type const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetchBuffer(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::texelFetch( Optional< Sampler1D > const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetch1D(), p_sampler, p_value, p_modif );
	}

	Optional< Vec4 > GlslWriter::texelFetch( Optional< Sampler2D > const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetch2D(), p_sampler, p_value, p_modif );
	}

	Optional< Vec4 > GlslWriter::texelFetch( Optional< Sampler3D > const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetch3D(), p_sampler, p_value, p_modif );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArray(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArray(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCubeArray(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCubeArrayLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > GlslWriter::textureLodOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > GlslWriter::textureLodOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadow(), p_sampler, p_value );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowLod(), p_sampler, p_value, p_lod );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadow(), p_sampler, p_value );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowLod(), p_sampler, p_value, p_lod );
	}

	Optional< Float > GlslWriter::texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeShadow(), p_sampler, p_value );
	}

	Optional< Float > GlslWriter::texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeShadowLod(), p_sampler, p_value, p_lod );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > GlslWriter::textureLodOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > GlslWriter::textureLodOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadow(), p_sampler, p_value );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowLod(), p_sampler, p_value, p_lod );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadow(), p_sampler, p_value );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowLod(), p_sampler, p_value, p_lod );
	}

	Optional< Float > GlslWriter::texture( Optional< SamplerCubeArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_layer )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeArrayShadow(), p_sampler, p_value, p_layer );
	}

	Optional< Float > GlslWriter::texture( Optional< SamplerCubeArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeArrayShadowLod(), p_sampler, p_value, p_layer, p_lod );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Int const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > GlslWriter::textureLodOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > GlslWriter::textureLodOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	GlslWriter & GlslWriter::operator<<( Version const & p_rhs )
	{
		m_stream << m_keywords->getVersion();
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( Attribute const & p_rhs )
	{
		m_stream << m_keywords->getAttribute( m_attributeIndex++ ) << cuT( " " );
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( In const & p_rhs )
	{
		m_stream << m_keywords->getIn() << cuT( " " );
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( Out const & p_rhs )
	{
		m_stream << m_keywords->getOut() << cuT( " " );
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( Layout const & p_rhs )
	{
		m_stream << m_keywords->getLayout( p_rhs.m_location );
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

	GlslWriter & GlslWriter::operator<<( InputLayout const & p_rhs )
	{
		static std::map< InputLayout::Kind, castor::String > const Names
		{
			{ InputLayout::Kind::ePoints, cuT( "points" ) },
			{ InputLayout::Kind::eLines, cuT( "lines" ) },
			{ InputLayout::Kind::eLinesAdjacency, cuT( "lines_adjacency" ) },
			{ InputLayout::Kind::eTriangles, cuT( "triangles" ) },
			{ InputLayout::Kind::eTrianglesAdjacency, cuT( "triangles_adjacency" ) },
		};
		REQUIRE( p_rhs.m_kind >= InputLayout::Kind::ePoints && p_rhs.m_kind <= InputLayout::Kind::eTrianglesAdjacency );
		m_stream << cuT( "layout( " ) << Names.at( p_rhs.m_kind ) << cuT( " ) in;" ) << std::endl;
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( OutputLayout const & p_rhs )
	{
		static std::map< OutputLayout::Kind, castor::String > const Names
		{
			{ OutputLayout::Kind::ePoints, cuT( "points" ) },
			{ OutputLayout::Kind::eLineStrip, cuT( "line_strip" ) },
			{ OutputLayout::Kind::eTriangleStrip, cuT( "triangle_strip" ) },
		};
		REQUIRE( p_rhs.m_kind >= OutputLayout::Kind::ePoints && p_rhs.m_kind <= OutputLayout::Kind::eTriangleStrip );
		m_stream << cuT( "layout( " ) << Names.at( p_rhs.m_kind ) << cuT( ", max_vertices = " ) << p_rhs.m_count << cuT( " ) out;" ) << std::endl;
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( Endl const & p_rhs )
	{
		m_stream << std::endl;
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( Endi const & p_rhs )
	{
		m_stream << ";" << std::endl;
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( castor::String const & p_rhs )
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
