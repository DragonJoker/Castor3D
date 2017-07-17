#include "GlslWriter.hpp"

#include "GlslVec.hpp"
#include "GlslLighting.hpp"
#include "GlslPhongLighting.hpp"
#include "GlslMetallicBrdfLighting.hpp"
#include "GlslSpecularBrdfLighting.hpp"

#include <Log/Logger.hpp>

namespace GLSL
{
	GlslWriter::GlslWriter( GlslWriterConfig const & p_config )
		: m_keywords( GLSL::KeywordsBase::Get( p_config ) )
		, m_uniform( cuT( "uniform " ) )
		, m_config( p_config )
	{
		m_lightingFactory.Register( PhongLightingModel::Name, &PhongLightingModel::Create );
		m_lightingFactory.Register( MetallicBrdfLightingModel::Name, &MetallicBrdfLightingModel::Create );
		m_lightingFactory.Register( SpecularBrdfLightingModel::Name, &SpecularBrdfLightingModel::Create );
		*this << GLSL::Version() << Endl();
	}

	GlslWriter::GlslWriter( GlslWriter const & p_rhs )
		: m_keywords( GLSL::KeywordsBase::Get( p_rhs.m_config ) )
		, m_attributeIndex( p_rhs.m_attributeIndex )
		, m_layoutIndex( p_rhs.m_layoutIndex )
		, m_uniform( p_rhs.m_uniform )
		, m_config( p_rhs.m_config )
	{
		m_stream << p_rhs.m_stream.str();
	}

	GlslWriter & GlslWriter::operator=( GlslWriter const & p_rhs )
	{
		m_attributeIndex = p_rhs.m_attributeIndex;
		m_layoutIndex = p_rhs.m_layoutIndex;
		m_uniform = p_rhs.m_uniform;
		m_config = p_rhs.m_config;
		m_stream << p_rhs.m_stream.str();
		return *this;
	}

	void GlslWriter::RegisterName( Castor::String const & p_name, TypeName p_type )
	{
		auto l_it = m_registered.find( p_name );

		//if ( l_it != m_registered.end() )
		//{
		//	std::stringstream l_text;
		//	l_text << "A variable with the name [" << p_name << "] is already registered.";
		//	Castor::Logger::LogWarning( l_text );
		//}

		m_registered.emplace( p_name, p_type );
	}

	void GlslWriter::CheckNameExists( Castor::String const & p_name, TypeName p_type )
	{
		auto l_it = m_registered.find( p_name );

		if ( l_it == m_registered.end() )
		{
			std::stringstream l_text;
			l_text << "No registered variable with the name [" << p_name << "].";
			CASTOR_EXCEPTION( l_text.str() );
		}
	}

	std::shared_ptr< LightingModel > GlslWriter::CreateLightingModel( Castor::String const & p_name, ShadowType p_shadows )
	{
		std::shared_ptr< LightingModel > l_lighting = m_lightingFactory.Create( p_name, p_shadows, *this );
		l_lighting->DeclareModel();
		return l_lighting;
	}

	std::shared_ptr< LightingModel > GlslWriter::CreateDirectionalLightingModel( Castor::String const & p_name, ShadowType p_shadows )
	{
		std::shared_ptr< LightingModel > l_lighting = m_lightingFactory.Create( p_name, p_shadows, *this );
		l_lighting->DeclareDirectionalModel();
		return l_lighting;
	}

	std::shared_ptr< LightingModel > GlslWriter::CreatePointLightingModel( Castor::String const & p_name, ShadowType p_shadows )
	{
		std::shared_ptr< LightingModel > l_lighting = m_lightingFactory.Create( p_name, p_shadows, *this );
		l_lighting->DeclarePointModel();
		return l_lighting;
	}

	std::shared_ptr< LightingModel > GlslWriter::CreateSpotLightingModel( Castor::String const & p_name, ShadowType p_shadows )
	{
		std::shared_ptr< LightingModel > l_lighting = m_lightingFactory.Create( p_name, p_shadows, *this );
		l_lighting->DeclareSpotModel();
		return l_lighting;
	}

	Shader GlslWriter::Finalise()
	{
		m_shader.SetSource( m_stream.str() );
		return m_shader;
	}

	void GlslWriter::WriteAssign( Type const & p_lhs, Type const & p_rhs )
	{
		m_stream << Castor::String( p_lhs ) << cuT( " = " ) << Castor::String( p_rhs ) << cuT( ";" ) << std::endl;
	}

	void GlslWriter::WriteAssign( Type const & p_lhs, int const & p_rhs )
	{
		m_stream << Castor::String( p_lhs ) << cuT( " = " ) << Castor::string::to_string( p_rhs ) << cuT( ";" ) << std::endl;
	}

	void GlslWriter::WriteAssign( Type const & p_lhs, unsigned int const & p_rhs )
	{
		m_stream << Castor::String( p_lhs ) << cuT( " = " ) << Castor::string::to_string( p_rhs ) << cuT( ";" ) << std::endl;
	}

	void GlslWriter::WriteAssign( Type const & p_lhs, float const & p_rhs )
	{
		m_stream << Castor::String( p_lhs ) << cuT( " = " ) << Castor::string::to_string( p_rhs ) << cuT( ";" ) << std::endl;
	}

	void GlslWriter::For( Type && p_init, Expr const & p_cond, Expr const & p_incr, std::function< void() > p_function )
	{
		m_stream << std::endl;
		m_stream << cuT( "for ( " ) << Castor::String( p_init ) << cuT( "; " ) << p_cond.m_value.rdbuf() << cuT( "; " ) << p_incr.m_value.rdbuf() << cuT( " )" ) << std::endl;
		{
			IndentBlock l_block( *this );
			p_function();
		}
		m_stream << std::endl;
	}

	void GlslWriter::While( Expr const & p_cond, std::function< void() > p_function )
	{
		m_stream << std::endl;
		m_stream << cuT( "while ( " ) << p_cond.m_value.rdbuf() << cuT( " )" ) << std::endl;
		{
			IndentBlock l_block( *this );
			p_function();
		}
		m_stream << std::endl;
	}

	GlslWriter & GlslWriter::If( Expr const & p_cond, std::function< void() > p_function )
	{
		m_stream << std::endl;
		m_stream << cuT( "if ( " ) << p_cond.m_value.rdbuf() << cuT( " )" ) << std::endl;
		{
			IndentBlock l_block( *this );
			p_function();
		}
		m_stream << std::endl;
		return *this;
	}

	GlslWriter & GlslWriter::ElseIf( Expr const & p_cond, std::function< void() > p_function )
	{
		m_stream << cuT( "else if ( " ) << p_cond.m_value.rdbuf() << cuT( " )" ) << std::endl;
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

	void GlslWriter::EmitVertex()
	{
		m_stream << cuT( "EmitVertex();" ) << std::endl;
	}

	void GlslWriter::EndPrimitive()
	{
		m_stream << cuT( "EndPrimitive();" ) << std::endl;
	}

	void GlslWriter::Discard()
	{
		m_stream << cuT( "discard;" ) << std::endl;
	}

	void GlslWriter::InputGeometryLayout( Castor::String const & p_layout )
	{
		m_stream << cuT( "layout( " ) << p_layout << cuT( " ) in;" ) << std::endl;
	}

	void GlslWriter::OutputGeometryLayout( Castor::String const & p_layout )
	{
		m_stream << cuT( "layout( " ) << p_layout << cuT( " ) out;" ) << std::endl;
	}

	void GlslWriter::OutputGeometryLayout( Castor::String const & p_layout, uint32_t p_count )
	{
		m_stream << cuT( "layout( " ) << p_layout << cuT( ", max_vertices = " ) << p_count << cuT( " ) out;" ) << std::endl;
	}

	void GlslWriter::OutputVertexCount( uint32_t p_count )
	{
		m_stream << cuT( "layout( max_vertices = " ) << p_count << cuT( " ) out;" ) << std::endl;
	}

	Vec4 GlslWriter::Texture( Sampler1D const & p_sampler, Float const & p_value )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture1D(), p_sampler, p_value );
	}

	Vec4 GlslWriter::Texture( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture1DLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::Texture( Sampler2D const & p_sampler, Vec2 const & p_value )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture2D(), p_sampler, p_value );
	}

	Vec4 GlslWriter::Texture( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture2DLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::Texture( Sampler3D const & p_sampler, Vec3 const & p_value )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture3D(), p_sampler, p_value );
	}

	Vec4 GlslWriter::Texture( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture3DLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::Texture( SamplerCube const & p_sampler, Vec3 const & p_value )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTextureCube(), p_sampler, p_value );
	}

	Vec4 GlslWriter::Texture( SamplerCube const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTextureCubeLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::TextureOffset( Sampler1D const & p_sampler, Float const & p_value, Int const & p_offset )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture1DOffset(), p_sampler, p_value, p_offset );
	}

	Vec4 GlslWriter::TextureOffset( Sampler1D const & p_sampler, Float const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture1DOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 GlslWriter::TextureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture2DOffset(), p_sampler, p_value, p_offset );
	}

	Vec4 GlslWriter::TextureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture2DOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 GlslWriter::TextureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture3DOffset(), p_sampler, p_value, p_offset );
	}

	Vec4 GlslWriter::TextureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset, Float const & p_lod )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture3DOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 GlslWriter::TextureLodOffset( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture1DLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 GlslWriter::TextureLodOffset( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture2DLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 GlslWriter::TextureLodOffset( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const & p_offset )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture3DLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 GlslWriter::TexelFetch( SamplerBuffer const & p_sampler, Type const & p_value )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexelFetchBuffer(), p_sampler, p_value );
	}

	Vec4 GlslWriter::TexelFetch( Sampler1D const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexelFetch1D(), p_sampler, p_value, p_modif );
	}

	Vec4 GlslWriter::TexelFetch( Sampler2D const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexelFetch2D(), p_sampler, p_value, p_modif );
	}

	Vec4 GlslWriter::TexelFetch( Sampler3D const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexelFetch3D(), p_sampler, p_value, p_modif );
	}

	Vec4 GlslWriter::Texture( Sampler1DArray const & p_sampler, Vec2 const & p_value )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture1DArray(), p_sampler, p_value );
	}

	Vec4 GlslWriter::Texture( Sampler1DArray const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture1DArrayLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::Texture( Sampler2DArray const & p_sampler, Vec3 const & p_value )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture2DArray(), p_sampler, p_value );
	}

	Vec4 GlslWriter::Texture( Sampler2DArray const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture2DArrayLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::Texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTextureCubeArray(), p_sampler, p_value );
	}

	Vec4 GlslWriter::Texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTextureCubeArrayLod(), p_sampler, p_value, p_lod );
	}

	Vec4 GlslWriter::TextureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const & p_offset )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture1DArrayOffset(), p_sampler, p_value, p_offset );
	}

	Vec4 GlslWriter::TextureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture1DArrayOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 GlslWriter::TextureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture2DArrayOffset(), p_sampler, p_value, p_offset );
	}

	Vec4 GlslWriter::TextureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture2DArrayOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Vec4 GlslWriter::TextureLodOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture1DArrayLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Vec4 GlslWriter::TextureLodOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return WriteFunctionCall< Vec4 >( this, m_keywords->GetTexture2DArrayLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Float GlslWriter::Texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture1DShadow(), p_sampler, p_value );
	}

	Float GlslWriter::Texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture1DShadowLod(), p_sampler, p_value, p_lod );
	}

	Float GlslWriter::Texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture2DShadow(), p_sampler, p_value );
	}

	Float GlslWriter::Texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture2DShadowLod(), p_sampler, p_value, p_lod );
	}

	Float GlslWriter::Texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTextureCubeShadow(), p_sampler, p_value );
	}

	Float GlslWriter::Texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTextureCubeShadowLod(), p_sampler, p_value, p_lod );
	}

	Float GlslWriter::TextureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const & p_offset )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture1DShadowOffset(), p_sampler, p_value, p_offset );
	}

	Float GlslWriter::TextureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture1DShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Float GlslWriter::TextureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture2DShadowOffset(), p_sampler, p_value, p_offset );
	}

	Float GlslWriter::TextureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture2DShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Float GlslWriter::TextureLodOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture1DShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Float GlslWriter::TextureLodOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture2DShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Float GlslWriter::Texture( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture1DArrayShadow(), p_sampler, p_value );
	}

	Float GlslWriter::Texture( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture1DArrayShadowLod(), p_sampler, p_value, p_lod );
	}

	Float GlslWriter::Texture( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture2DArrayShadow(), p_sampler, p_value );
	}

	Float GlslWriter::Texture( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture2DArrayShadowLod(), p_sampler, p_value, p_lod );
	}

	Float GlslWriter::Texture( SamplerCubeArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_layer )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTextureCubeArrayShadow(), p_sampler, p_value, p_layer );
	}

	Float GlslWriter::Texture( SamplerCubeArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTextureCubeArrayShadowLod(), p_sampler, p_value, p_layer, p_lod );
	}

	Float GlslWriter::TextureOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Int const & p_offset )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture1DArrayShadowOffset(), p_sampler, p_value, p_offset );
	}

	Float GlslWriter::TextureOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture1DArrayShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Float GlslWriter::TextureOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture2DArrayShadowOffset(), p_sampler, p_value, p_offset );
	}

	Float GlslWriter::TextureOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture2DArrayShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Float GlslWriter::TextureLodOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture1DArrayShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Float GlslWriter::TextureLodOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return WriteFunctionCall< Float >( this, m_keywords->GetTexture2DArrayShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< Sampler1D > const & p_sampler, Float const & p_value )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture1D(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture1DLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture2D(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture2DLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture3D(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture3DLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTextureCube(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTextureCubeLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::TextureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const & p_offset )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture1DOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > GlslWriter::TextureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture1DOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > GlslWriter::TextureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture2DOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > GlslWriter::TextureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture2DOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > GlslWriter::TextureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture3DOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > GlslWriter::TextureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture3DOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > GlslWriter::TextureLodOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture1DLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > GlslWriter::TextureLodOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture2DLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > GlslWriter::TextureLodOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const & p_offset )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture3DLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > GlslWriter::TexelFetch( Optional< SamplerBuffer > const & p_sampler, Type const & p_value )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexelFetchBuffer(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::TexelFetch( Optional< Sampler1D > const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexelFetch1D(), p_sampler, p_value, p_modif );
	}

	Optional< Vec4 > GlslWriter::TexelFetch( Optional< Sampler2D > const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexelFetch2D(), p_sampler, p_value, p_modif );
	}

	Optional< Vec4 > GlslWriter::TexelFetch( Optional< Sampler3D > const & p_sampler, Type const & p_value, Int const & p_modif )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexelFetch3D(), p_sampler, p_value, p_modif );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture1DArray(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture1DArrayLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture2DArray(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture2DArrayLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTextureCubeArray(), p_sampler, p_value );
	}

	Optional< Vec4 > GlslWriter::Texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTextureCubeArrayLod(), p_sampler, p_value, p_lod );
	}

	Optional< Vec4 > GlslWriter::TextureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const & p_offset )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture1DArrayOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > GlslWriter::TextureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture1DArrayOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > GlslWriter::TextureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture2DArrayOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Vec4 > GlslWriter::TextureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture2DArrayOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Vec4 > GlslWriter::TextureLodOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture1DArrayLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Vec4 > GlslWriter::TextureLodOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return WriteOptionalFunctionCall< Vec4 >( this, m_keywords->GetTexture2DArrayLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > GlslWriter::Texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture1DShadow(), p_sampler, p_value );
	}

	Optional< Float > GlslWriter::Texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture1DShadowLod(), p_sampler, p_value, p_lod );
	}

	Optional< Float > GlslWriter::Texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture2DShadow(), p_sampler, p_value );
	}

	Optional< Float > GlslWriter::Texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture2DShadowLod(), p_sampler, p_value, p_lod );
	}

	Optional< Float > GlslWriter::Texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTextureCubeShadow(), p_sampler, p_value );
	}

	Optional< Float > GlslWriter::Texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTextureCubeShadowLod(), p_sampler, p_value, p_lod );
	}

	Optional< Float > GlslWriter::TextureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const & p_offset )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture1DShadowOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Float > GlslWriter::TextureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture1DShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > GlslWriter::TextureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture2DShadowOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Float > GlslWriter::TextureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture2DShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > GlslWriter::TextureLodOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture1DShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > GlslWriter::TextureLodOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture2DShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > GlslWriter::Texture( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture1DArrayShadow(), p_sampler, p_value );
	}

	Optional< Float > GlslWriter::Texture( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture1DArrayShadowLod(), p_sampler, p_value, p_lod );
	}

	Optional< Float > GlslWriter::Texture( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture2DArrayShadow(), p_sampler, p_value );
	}

	Optional< Float > GlslWriter::Texture( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture2DArrayShadowLod(), p_sampler, p_value, p_lod );
	}

	Optional< Float > GlslWriter::Texture( Optional< SamplerCubeArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_layer )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTextureCubeArrayShadow(), p_sampler, p_value, p_layer );
	}

	Optional< Float > GlslWriter::Texture( Optional< SamplerCubeArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTextureCubeArrayShadowLod(), p_sampler, p_value, p_layer, p_lod );
	}

	Optional< Float > GlslWriter::TextureOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Int const & p_offset )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture1DArrayShadowOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Float > GlslWriter::TextureOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Int const & p_offset, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture1DArrayShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > GlslWriter::TextureOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture2DArrayShadowOffset(), p_sampler, p_value, p_offset );
	}

	Optional< Float > GlslWriter::TextureOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset, Float const & p_lod )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture2DArrayShadowOffsetLod(), p_sampler, p_value, p_offset, p_lod );
	}

	Optional< Float > GlslWriter::TextureLodOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const & p_offset )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture1DArrayShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
	}

	Optional< Float > GlslWriter::TextureLodOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const & p_offset )
	{
		return WriteOptionalFunctionCall< Float >( this, m_keywords->GetTexture2DArrayShadowLodOffset(), p_sampler, p_value, p_lod, p_offset );
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

	GlslWriter & GlslWriter::operator<<( Layout const & p_rhs )
	{
		m_stream << m_keywords->GetLayout( p_rhs.m_index );
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
		static std::map< InputLayout::Kind, Castor::String > const Names
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
		static std::map< OutputLayout::Kind, Castor::String > const Names
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
