#include "GlslWriter.hpp"

#include "GlslVeci.hpp"
#include "GlslLighting.hpp"

namespace GLSL
{
	//*****************************************************************************************

	LightingModelFactory::LightingModelFactory()
		: Castor::Factory< LightingModel, Castor::String, std::unique_ptr< LightingModel >, std::function< std::unique_ptr< LightingModel >( uint32_t, GlslWriter & ) > >{}
	{
		Register( PhongLightingModel::Name, &PhongLightingModel::Create );
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

	IndentBlock::IndentBlock( Castor::StringStream & p_stream )
		: m_stream( p_stream )
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
		, m_block( nullptr )
	{
		if ( m_writer.HasConstantsBuffers() )
		{
			m_stream << std::endl;
			m_stream << m_writer.m_keywords->GetStdLayout( 140 ) << m_writer.m_uniform << p_name << std::endl;
			m_writer.m_uniform.clear();
			m_block = new IndentBlock( m_stream );
		}
	}

	void Ubo::End()
	{
		delete m_block;
		m_block = nullptr;
		m_writer.m_uniform = cuT( "uniform " );

		if ( m_writer.HasConstantsBuffers() )
		{
			m_stream << cuT( ";" );
		}

		m_stream << std::endl;

		if ( m_count )
		{
			m_writer.m_stream << m_stream.rdbuf();
		}
	}

	//*****************************************************************************************

	Struct::Struct( GlslWriter & p_writer, Castor::String const & p_name )
		: m_writer( p_writer )
		, m_name( p_name )
		, m_block( nullptr )
	{
		m_writer << Endl();
		m_writer << cuT( "struct " ) << p_name << Endl();
		m_block = new IndentBlock( m_writer );
	}

	void Struct::End()
	{
		delete m_block;
		m_block = nullptr;
		m_writer << cuT( ";" ) << Endl();
	}

	//*****************************************************************************************

	GlslWriter::GlslWriter( GlslWriterConfig const & p_config )
		: m_keywords( GLSL::KeywordsBase::Get( p_config ) )
		, m_attributeIndex( 0 )
		, m_layoutIndex( 0 )
		, m_uniform( cuT( "uniform " ) )
		, m_config( p_config )
	{
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

	std::unique_ptr< LightingModel > GlslWriter::CreateLightingModel( Castor::String const & p_name, uint32_t p_flags )
	{
		std::unique_ptr< LightingModel > l_lighting = m_lightingFactory.Create( p_name, p_flags, *this );
		l_lighting->DeclareModel();
		return l_lighting;
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
		m_stream << Castor::String( p_lhs ) << cuT( " = " ) << Castor::string::to_string( p_rhs ) << cuT( ";" ) << std::endl;
	}

	void GlslWriter::WriteAssign( Type const & p_lhs, float const & p_rhs )
	{
		m_stream << Castor::String( p_lhs ) << cuT( " = " ) << Castor::string::to_string( p_rhs ) << cuT( ";" ) << std::endl;
	}

	void GlslWriter::For( Type && p_init, Expr const & p_cond, Expr const & p_incr, std::function< void() > p_function )
	{
		m_stream << std::endl;
		m_stream << cuT( "for( " ) << Castor::String( p_init ) << cuT( "; " ) << p_cond.m_value.rdbuf() << cuT( "; " ) << p_incr.m_value.rdbuf() << cuT( " )" ) << std::endl;
		{
			IndentBlock l_block( *this );
			p_function();
		}
		m_stream << std::endl;
	}

	GlslWriter & GlslWriter::If( Expr const & p_cond, std::function< void() > p_function )
	{
		m_stream << std::endl;
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
