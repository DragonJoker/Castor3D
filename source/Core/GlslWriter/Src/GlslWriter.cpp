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
			*this << "#extension GL_ARB_texture_cube_map_array : enable" << endl;
		}
	}

	void GlslWriter::registerName( castor::String const & name, TypeName type )
	{
		auto it = m_registered.find( name );

		//if ( it != m_registered.end() )
		//{
		//	std::stringstream text;
		//	text << "A variable with the name [" << name << "] is already registered.";
		//	castor::Logger::logWarning( text );
		//}

		m_registered.emplace( name, type );
	}

	void GlslWriter::checkNameExists( castor::String const & name, TypeName type )
	{
		auto it = m_registered.find( name );

		if ( it == m_registered.end() )
		{
			std::stringstream text;
			text << "No registered variable with the name [" << name << "].";
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

	Vec2 GlslWriter::ashesBottomUpToTopDown( Vec2 const & texCoord )
	{
		return writeFunctionCall< Vec2 >( this
			, cuT( "ashesBottomUpToTopDown" )
			, texCoord );
	}

	Vec2 GlslWriter::ashesTopDownToBottomUp( Vec2 const & texCoord )
	{
		return writeFunctionCall< Vec2 >( this
			, cuT( "ashesTopDownToBottomUp" )
			, texCoord );
	}

	Vec3 GlslWriter::ashesBottomUpToTopDown( Vec3 const & texCoord )
	{
		return writeFunctionCall< Vec3 >( this
			, cuT( "ashesBottomUpToTopDown" )
			, texCoord );
	}

	Vec3 GlslWriter::ashesTopDownToBottomUp( Vec3 const & texCoord )
	{
		return writeFunctionCall< Vec3 >( this
			, cuT( "ashesTopDownToBottomUp" )
			, texCoord );
	}

	Shader GlslWriter::finalise()
	{
		m_shader.setSource( m_stream.str() );
		return m_shader;
	}

	void GlslWriter::writeAssign( Type const & lhs, Type const & rhs )
	{
		m_stream << castor::String( lhs ) << cuT( " = " ) << castor::String( rhs ) << cuT( ";" ) << std::endl;
	}

	void GlslWriter::writeAssign( Type const & lhs, int const & rhs )
	{
		m_stream << castor::String( lhs ) << cuT( " = " ) << castor::string::toString( rhs, 10, Expr::getLocale() ) << cuT( ";" ) << std::endl;
	}

	void GlslWriter::writeAssign( Type const & lhs, unsigned int const & rhs )
	{
		m_stream << castor::String( lhs ) << cuT( " = " ) << castor::string::toString( rhs, 10, Expr::getLocale() ) << cuT( "u;" ) << std::endl;
	}

	void GlslWriter::writeAssign( Type const & lhs, float const & rhs )
	{
		m_stream << castor::String( lhs ) << cuT( " = " ) << castor::string::toString( rhs, Expr::getLocale() ) << cuT( ";" ) << std::endl;
	}

	void GlslWriter::forStmt( Type && init, Expr const & cond, Expr const & incr, std::function< void() > function )
	{
		m_stream << std::endl;
		m_stream << cuT( "for ( " ) << castor::String( init ) << cuT( "; " ) << cond.m_value.rdbuf() << cuT( "; " ) << incr.m_value.rdbuf() << cuT( " )" ) << std::endl;
		{
			IndentBlock block( *this );
			function();
		}
		m_stream << std::endl;
	}

	void GlslWriter::whileStmt( Expr const & cond, std::function< void() > function )
	{
		m_stream << std::endl;
		m_stream << cuT( "while ( " ) << cond.m_value.rdbuf() << cuT( " )" ) << std::endl;
		{
			IndentBlock block( *this );
			function();
		}
		m_stream << std::endl;
	}

	GlslWriter & GlslWriter::ifStmt( Expr const & cond, std::function< void() > function )
	{
		m_stream << std::endl;
		m_stream << cuT( "if ( " ) << cond.m_value.rdbuf() << cuT( " )" ) << std::endl;
		{
			IndentBlock block( *this );
			function();
		}
		m_stream << std::endl;
		return *this;
	}

	GlslWriter & GlslWriter::elseIfStmt( Expr const & cond, std::function< void() > function )
	{
		m_stream << cuT( "else if ( " ) << cond.m_value.rdbuf() << cuT( " )" ) << std::endl;
		{
			IndentBlock block( *this );
			function();
		}
		m_stream << std::endl;
		return *this;
	}

	void GlslWriter::elseStmt( std::function< void() > function )
	{
		m_stream << cuT( "else" ) << std::endl;
		{
			IndentBlock block( *this );
			function();
		}
		m_stream << std::endl;
	}

	Struct GlslWriter::getStruct( castor::String const & name )
	{
		return Struct( *this, name );
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

	void GlslWriter::inputGeometryLayout( castor::String const & layout )
	{
		m_stream << cuT( "layout( " ) << layout << cuT( " ) in;" ) << std::endl;
	}

	void GlslWriter::outputGeometryLayout( castor::String const & layout )
	{
		m_stream << cuT( "layout( " ) << layout << cuT( " ) out;" ) << std::endl;
	}

	void GlslWriter::outputGeometryLayout( castor::String const & layout, uint32_t count )
	{
		m_stream << cuT( "layout( " ) << layout << cuT( ", max_vertices = " ) << count << cuT( " ) out;" ) << std::endl;
	}

	void GlslWriter::outputVertexCount( uint32_t count )
	{
		m_stream << cuT( "layout( max_vertices = " ) << count << cuT( " ) out;" ) << std::endl;
	}

	void GlslWriter::returnStmt()
	{
		m_stream << cuT( "return;" ) << std::endl;
	}

	Vec4 GlslWriter::texture( Sampler1D const & sampler, Float const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1D(), sampler, value );
	}

	Vec4 GlslWriter::texture( Sampler1D const & sampler, Float const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DLod(), sampler, value, lod );
	}

	Vec4 GlslWriter::texture( Sampler2D const & sampler, Vec2 const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2D(), sampler, value );
	}

	Vec4 GlslWriter::texture( Sampler2D const & sampler, Vec2 const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DLod(), sampler, value, lod );
	}

	Vec4 GlslWriter::texture( Sampler3D const & sampler, Vec3 const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3D(), sampler, value );
	}

	Vec4 GlslWriter::texture( Sampler3D const & sampler, Vec3 const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DLod(), sampler, value, lod );
	}

	Vec4 GlslWriter::texture( SamplerCube const & sampler, Vec3 const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCube(), sampler, value );
	}

	Vec4 GlslWriter::texture( SamplerCube const & sampler, Vec3 const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCubeLod(), sampler, value, lod );
	}

	Vec4 GlslWriter::textureOffset( Sampler1D const & sampler, Float const & value, Int const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DOffset(), sampler, value, offset );
	}

	Vec4 GlslWriter::textureOffset( Sampler1D const & sampler, Float const & value, Int const & offset, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DOffsetLod(), sampler, value, offset, lod );
	}

	Vec4 GlslWriter::textureOffset( Sampler2D const & sampler, Vec2 const & value, IVec2 const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DOffset(), sampler, value, offset );
	}

	Vec4 GlslWriter::textureOffset( Sampler2D const & sampler, Vec2 const & value, IVec2 const & offset, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DOffsetLod(), sampler, value, offset, lod );
	}

	Vec4 GlslWriter::textureOffset( Sampler3D const & sampler, Vec3 const & value, IVec3 const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DOffset(), sampler, value, offset );
	}

	Vec4 GlslWriter::textureOffset( Sampler3D const & sampler, Vec3 const & value, IVec3 const & offset, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DOffsetLod(), sampler, value, offset, lod );
	}

	Vec4 GlslWriter::textureLodOffset( Sampler1D const & sampler, Float const & value, Float const & lod, Int const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DLodOffset(), sampler, value, lod, offset );
	}

	Vec4 GlslWriter::textureLodOffset( Sampler2D const & sampler, Vec2 const & value, Float const & lod, IVec2 const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DLodOffset(), sampler, value, lod, offset );
	}

	Vec4 GlslWriter::textureLodOffset( Sampler3D const & sampler, Vec3 const & value, Float const & lod, IVec3 const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DLodOffset(), sampler, value, lod, offset );
	}

	Vec4 GlslWriter::texelFetch( SamplerBuffer const & sampler, Type const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetchBuffer(), sampler, value );
	}

	Vec4 GlslWriter::texelFetch( Sampler1D const & sampler, Type const & value, Int const & modif )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetch1D(), sampler, value, modif );
	}

	Vec4 GlslWriter::texelFetch( Sampler2D const & sampler, Type const & value, Int const & modif )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetch2D(), sampler, value, modif );
	}

	Vec4 GlslWriter::texelFetch( Sampler3D const & sampler, Type const & value, Int const & modif )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetch3D(), sampler, value, modif );
	}

	Vec4 GlslWriter::texture( Sampler1DArray const & sampler, Vec2 const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArray(), sampler, value );
	}

	Vec4 GlslWriter::texture( Sampler1DArray const & sampler, Vec2 const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayLod(), sampler, value, lod );
	}

	Vec4 GlslWriter::texture( Sampler2DArray const & sampler, Vec3 const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArray(), sampler, value );
	}

	Vec4 GlslWriter::texture( Sampler2DArray const & sampler, Vec3 const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayLod(), sampler, value, lod );
	}

	Vec4 GlslWriter::texture( SamplerCubeArray const & sampler, Vec4 const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCubeArray(), sampler, value );
	}

	Vec4 GlslWriter::texture( SamplerCubeArray const & sampler, Vec4 const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCubeArrayLod(), sampler, value, lod );
	}

	Vec4 GlslWriter::textureOffset( Sampler1DArray const & sampler, Vec2 const & value, Int const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayOffset(), sampler, value, offset );
	}

	Vec4 GlslWriter::textureOffset( Sampler1DArray const & sampler, Vec2 const & value, Int const & offset, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayOffsetLod(), sampler, value, offset, lod );
	}

	Vec4 GlslWriter::textureOffset( Sampler2DArray const & sampler, Vec3 const & value, IVec2 const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayOffset(), sampler, value, offset );
	}

	Vec4 GlslWriter::textureOffset( Sampler2DArray const & sampler, Vec3 const & value, IVec2 const & offset, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayOffsetLod(), sampler, value, offset, lod );
	}

	Vec4 GlslWriter::textureLodOffset( Sampler1DArray const & sampler, Vec2 const & value, Float const & lod, Int const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayLodOffset(), sampler, value, lod, offset );
	}

	Vec4 GlslWriter::textureLodOffset( Sampler2DArray const & sampler, Vec3 const & value, Float const & lod, IVec2 const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayLodOffset(), sampler, value, lod, offset );
	}

	Float GlslWriter::texture( Sampler1DShadow const & sampler, Vec2 const & value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadow(), sampler, value );
	}

	Float GlslWriter::texture( Sampler1DShadow const & sampler, Vec2 const & value, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowLod(), sampler, value, lod );
	}

	Float GlslWriter::texture( Sampler2DShadow const & sampler, Vec3 const & value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadow(), sampler, value );
	}

	Float GlslWriter::texture( Sampler2DShadow const & sampler, Vec3 const & value, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowLod(), sampler, value, lod );
	}

	Float GlslWriter::texture( SamplerCubeShadow const & sampler, Vec4 const & value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeShadow(), sampler, value );
	}

	Float GlslWriter::texture( SamplerCubeShadow const & sampler, Vec4 const & value, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeShadowLod(), sampler, value, lod );
	}

	Float GlslWriter::textureOffset( Sampler1DShadow const & sampler, Vec2 const & value, Int const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowOffset(), sampler, value, offset );
	}

	Float GlslWriter::textureOffset( Sampler1DShadow const & sampler, Vec2 const & value, Int const & offset, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowOffsetLod(), sampler, value, offset, lod );
	}

	Float GlslWriter::textureOffset( Sampler2DShadow const & sampler, Vec3 const & value, IVec2 const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowOffset(), sampler, value, offset );
	}

	Float GlslWriter::textureOffset( Sampler2DShadow const & sampler, Vec3 const & value, IVec2 const & offset, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowOffsetLod(), sampler, value, offset, lod );
	}

	Float GlslWriter::textureLodOffset( Sampler1DShadow const & sampler, Vec2 const & value, Float const & lod, Int const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowLodOffset(), sampler, value, lod, offset );
	}

	Float GlslWriter::textureLodOffset( Sampler2DShadow const & sampler, Vec3 const & value, Float const & lod, IVec2 const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowLodOffset(), sampler, value, lod, offset );
	}

	Float GlslWriter::texture( Sampler1DArrayShadow const & sampler, Vec3 const & value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadow(), sampler, value );
	}

	Float GlslWriter::texture( Sampler1DArrayShadow const & sampler, Vec3 const & value, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowLod(), sampler, value, lod );
	}

	Float GlslWriter::texture( Sampler2DArrayShadow const & sampler, Vec4 const & value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadow(), sampler, value );
	}

	Float GlslWriter::texture( Sampler2DArrayShadow const & sampler, Vec4 const & value, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowLod(), sampler, value, lod );
	}

	Float GlslWriter::texture( SamplerCubeArrayShadow const & sampler, Vec4 const & value, Float const & layer )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeArrayShadow(), sampler, value, layer );
	}

	Float GlslWriter::texture( SamplerCubeArrayShadow const & sampler, Vec4 const & value, Float const & layer, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeArrayShadowLod(), sampler, value, layer, lod );
	}

	Float GlslWriter::textureOffset( Sampler1DArrayShadow const & sampler, Vec3 const & value, Int const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowOffset(), sampler, value, offset );
	}

	Float GlslWriter::textureOffset( Sampler1DArrayShadow const & sampler, Vec3 const & value, Int const & offset, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowOffsetLod(), sampler, value, offset, lod );
	}

	Float GlslWriter::textureOffset( Sampler2DArrayShadow const & sampler, Vec4 const & value, IVec2 const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowOffset(), sampler, value, offset );
	}

	Float GlslWriter::textureOffset( Sampler2DArrayShadow const & sampler, Vec4 const & value, IVec2 const & offset, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowOffsetLod(), sampler, value, offset, lod );
	}

	Float GlslWriter::textureLodOffset( Sampler1DArrayShadow const & sampler, Vec3 const & value, Float const & lod, Int const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowLodOffset(), sampler, value, lod, offset );
	}

	Float GlslWriter::textureLodOffset( Sampler2DArrayShadow const & sampler, Vec4 const & value, Float const & lod, IVec2 const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowLodOffset(), sampler, value, lod, offset );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler1D > const & sampler, Float const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1D(), sampler, value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler1D > const & sampler, Float const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DLod(), sampler, value, lod );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler2D > const & sampler, Vec2 const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2D(), sampler, value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler2D > const & sampler, Vec2 const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DLod(), sampler, value, lod );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler3D > const & sampler, Vec3 const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3D(), sampler, value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler3D > const & sampler, Vec3 const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DLod(), sampler, value, lod );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< SamplerCube > const & sampler, Vec3 const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCube(), sampler, value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< SamplerCube > const & sampler, Vec3 const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCubeLod(), sampler, value, lod );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler1D > const & sampler, Float const & value, Int const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DOffset(), sampler, value, offset );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler1D > const & sampler, Float const & value, Int const & offset, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DOffsetLod(), sampler, value, offset, lod );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler2D > const & sampler, Vec2 const & value, IVec2 const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DOffset(), sampler, value, offset );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler2D > const & sampler, Vec2 const & value, IVec2 const & offset, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DOffsetLod(), sampler, value, offset, lod );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler3D > const & sampler, Vec3 const & value, IVec3 const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DOffset(), sampler, value, offset );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler3D > const & sampler, Vec3 const & value, IVec3 const & offset, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DOffsetLod(), sampler, value, offset, lod );
	}

	Optional< Vec4 > GlslWriter::textureLodOffset( Optional< Sampler1D > const & sampler, Float const & value, Float const & lod, Int const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DLodOffset(), sampler, value, lod, offset );
	}

	Optional< Vec4 > GlslWriter::textureLodOffset( Optional< Sampler2D > const & sampler, Vec2 const & value, Float const & lod, IVec2 const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DLodOffset(), sampler, value, lod, offset );
	}

	Optional< Vec4 > GlslWriter::textureLodOffset( Optional< Sampler3D > const & sampler, Vec3 const & value, Float const & lod, IVec3 const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture3DLodOffset(), sampler, value, lod, offset );
	}

	Optional< Vec4 > GlslWriter::texelFetch( Optional< SamplerBuffer > const & sampler, Type const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetchBuffer(), sampler, value );
	}

	Optional< Vec4 > GlslWriter::texelFetch( Optional< Sampler1D > const & sampler, Type const & value, Int const & modif )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetch1D(), sampler, value, modif );
	}

	Optional< Vec4 > GlslWriter::texelFetch( Optional< Sampler2D > const & sampler, Type const & value, Int const & modif )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetch2D(), sampler, value, modif );
	}

	Optional< Vec4 > GlslWriter::texelFetch( Optional< Sampler3D > const & sampler, Type const & value, Int const & modif )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexelFetch3D(), sampler, value, modif );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler1DArray > const & sampler, Vec2 const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArray(), sampler, value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler1DArray > const & sampler, Vec2 const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayLod(), sampler, value, lod );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler2DArray > const & sampler, Vec3 const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArray(), sampler, value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< Sampler2DArray > const & sampler, Vec3 const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayLod(), sampler, value, lod );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< SamplerCubeArray > const & sampler, Vec4 const & value )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCubeArray(), sampler, value );
	}

	Optional< Vec4 > GlslWriter::texture( Optional< SamplerCubeArray > const & sampler, Vec4 const & value, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTextureCubeArrayLod(), sampler, value, lod );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler1DArray > const & sampler, Vec2 const & value, Int const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayOffset(), sampler, value, offset );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler1DArray > const & sampler, Vec2 const & value, Int const & offset, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayOffsetLod(), sampler, value, offset, lod );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler2DArray > const & sampler, Vec3 const & value, IVec2 const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayOffset(), sampler, value, offset );
	}

	Optional< Vec4 > GlslWriter::textureOffset( Optional< Sampler2DArray > const & sampler, Vec3 const & value, IVec2 const & offset, Float const & lod )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayOffsetLod(), sampler, value, offset, lod );
	}

	Optional< Vec4 > GlslWriter::textureLodOffset( Optional< Sampler1DArray > const & sampler, Vec2 const & value, Float const & lod, Int const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture1DArrayLodOffset(), sampler, value, lod, offset );
	}

	Optional< Vec4 > GlslWriter::textureLodOffset( Optional< Sampler2DArray > const & sampler, Vec3 const & value, Float const & lod, IVec2 const & offset )
	{
		return writeFunctionCall< Vec4 >( this, m_keywords->getTexture2DArrayLodOffset(), sampler, value, lod, offset );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler1DShadow > const & sampler, Vec2 const & value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadow(), sampler, value );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler1DShadow > const & sampler, Vec2 const & value, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowLod(), sampler, value, lod );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler2DShadow > const & sampler, Vec3 const & value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadow(), sampler, value );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler2DShadow > const & sampler, Vec3 const & value, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowLod(), sampler, value, lod );
	}

	Optional< Float > GlslWriter::texture( Optional< SamplerCubeShadow > const & sampler, Vec4 const & value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeShadow(), sampler, value );
	}

	Optional< Float > GlslWriter::texture( Optional< SamplerCubeShadow > const & sampler, Vec4 const & value, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeShadowLod(), sampler, value, lod );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler1DShadow > const & sampler, Vec2 const & value, Int const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowOffset(), sampler, value, offset );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler1DShadow > const & sampler, Vec2 const & value, Int const & offset, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowOffsetLod(), sampler, value, offset, lod );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler2DShadow > const & sampler, Vec3 const & value, IVec2 const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowOffset(), sampler, value, offset );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler2DShadow > const & sampler, Vec3 const & value, IVec2 const & offset, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowOffsetLod(), sampler, value, offset, lod );
	}

	Optional< Float > GlslWriter::textureLodOffset( Optional< Sampler1DShadow > const & sampler, Vec2 const & value, Float const & lod, Int const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DShadowLodOffset(), sampler, value, lod, offset );
	}

	Optional< Float > GlslWriter::textureLodOffset( Optional< Sampler2DShadow > const & sampler, Vec3 const & value, Float const & lod, IVec2 const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DShadowLodOffset(), sampler, value, lod, offset );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler1DArrayShadow > const & sampler, Vec3 const & value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadow(), sampler, value );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler1DArrayShadow > const & sampler, Vec3 const & value, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowLod(), sampler, value, lod );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler2DArrayShadow > const & sampler, Vec4 const & value )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadow(), sampler, value );
	}

	Optional< Float > GlslWriter::texture( Optional< Sampler2DArrayShadow > const & sampler, Vec4 const & value, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowLod(), sampler, value, lod );
	}

	Optional< Float > GlslWriter::texture( Optional< SamplerCubeArrayShadow > const & sampler, Vec4 const & value, Float const & layer )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeArrayShadow(), sampler, value, layer );
	}

	Optional< Float > GlslWriter::texture( Optional< SamplerCubeArrayShadow > const & sampler, Vec4 const & value, Float const & layer, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTextureCubeArrayShadowLod(), sampler, value, layer, lod );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler1DArrayShadow > const & sampler, Vec3 const & value, Int const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowOffset(), sampler, value, offset );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler1DArrayShadow > const & sampler, Vec3 const & value, Int const & offset, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowOffsetLod(), sampler, value, offset, lod );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler2DArrayShadow > const & sampler, Vec4 const & value, IVec2 const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowOffset(), sampler, value, offset );
	}

	Optional< Float > GlslWriter::textureOffset( Optional< Sampler2DArrayShadow > const & sampler, Vec4 const & value, IVec2 const & offset, Float const & lod )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowOffsetLod(), sampler, value, offset, lod );
	}

	Optional< Float > GlslWriter::textureLodOffset( Optional< Sampler1DArrayShadow > const & sampler, Vec3 const & value, Float const & lod, Int const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture1DArrayShadowLodOffset(), sampler, value, lod, offset );
	}

	Optional< Float > GlslWriter::textureLodOffset( Optional< Sampler2DArrayShadow > const & sampler, Vec4 const & value, Float const & lod, IVec2 const & offset )
	{
		return writeFunctionCall< Float >( this, m_keywords->getTexture2DArrayShadowLodOffset(), sampler, value, lod, offset );
	}

	GlslWriter & GlslWriter::operator<<( Version const & rhs )
	{
		m_stream << m_keywords->getVersion();
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( Attribute const & rhs )
	{
		m_stream << m_keywords->getAttribute( m_attributeIndex++ ) << cuT( " " );
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( In const & rhs )
	{
		m_stream << m_keywords->getIn() << cuT( " " );
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( Out const & rhs )
	{
		m_stream << m_keywords->getOut() << cuT( " " );
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( Layout const & rhs )
	{
		m_stream << m_keywords->getLayout( rhs.m_location );
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( Uniform const & rhs )
	{
		if ( !m_uniform.empty() )
		{
			m_stream << m_uniform;
		}

		return *this;
	}

	GlslWriter & GlslWriter::operator<<( InputLayout const & rhs )
	{
		static std::map< InputLayout::Kind, castor::String > const Names
		{
			{ InputLayout::Kind::ePoints, cuT( "points" ) },
			{ InputLayout::Kind::eLines, cuT( "lines" ) },
			{ InputLayout::Kind::eLinesAdjacency, cuT( "lines_adjacency" ) },
			{ InputLayout::Kind::eTriangles, cuT( "triangles" ) },
			{ InputLayout::Kind::eTrianglesAdjacency, cuT( "triangles_adjacency" ) },
		};
		REQUIRE( rhs.m_kind >= InputLayout::Kind::ePoints && rhs.m_kind <= InputLayout::Kind::eTrianglesAdjacency );
		m_stream << cuT( "layout( " ) << Names.at( rhs.m_kind ) << cuT( " ) in;" ) << std::endl;
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( OutputLayout const & rhs )
	{
		static std::map< OutputLayout::Kind, castor::String > const Names
		{
			{ OutputLayout::Kind::ePoints, cuT( "points" ) },
			{ OutputLayout::Kind::eLineStrip, cuT( "line_strip" ) },
			{ OutputLayout::Kind::eTriangleStrip, cuT( "triangle_strip" ) },
		};
		REQUIRE( rhs.m_kind >= OutputLayout::Kind::ePoints && rhs.m_kind <= OutputLayout::Kind::eTriangleStrip );
		m_stream << cuT( "layout( " ) << Names.at( rhs.m_kind ) << cuT( ", max_vertices = " ) << rhs.m_count << cuT( " ) out;" ) << std::endl;
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( Endl const & rhs )
	{
		m_stream << std::endl;
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( Endi const & rhs )
	{
		m_stream << ";" << std::endl;
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( castor::String const & rhs )
	{
		m_stream << rhs;
		return *this;
	}

	GlslWriter & GlslWriter::operator<<( uint32_t const & rhs )
	{
		m_stream << rhs;
		return *this;
	}
}
