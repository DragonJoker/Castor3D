/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GLSL_WRITER_H___
#define ___GLSL_WRITER_H___

#include "GlslOptionalArray.hpp"

#include <Design/Factory.hpp>
#include <Design/OwnedBy.hpp>

namespace GLSL
{
	struct Int;
	struct UInt;
	struct Vec4;
	struct SamplerBuffer;
	struct Sampler1D;
	struct Sampler2D;
	struct Sampler2DRect;
	struct Sampler3D;
	struct SamplerCube;

	class LightingModel;
	using LightingModelFactory = Castor::Factory< LightingModel, Castor::String, std::unique_ptr< LightingModel >, std::function< std::unique_ptr< LightingModel >( bool, GlslWriter & ) > >;

	struct Endl {};
	struct Endi {};

	struct IndentBlock
	{
		GlslWriter_API IndentBlock( GlslWriter & p_writter );
		GlslWriter_API IndentBlock( Castor::StringStream & p_stream );
		GlslWriter_API ~IndentBlock();
		Castor::StringStream & m_stream;
		int m_indent;
	};

	struct Ubo
	{
		GlslWriter_API Ubo( GlslWriter & p_writer, Castor::String const & p_name );
		GlslWriter_API void End();
		template< typename T > inline T GetUniform( Castor::String const & p_name );
		template< typename T > inline Array< T > GetUniform( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Optional< T > GetUniform( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< Array< T > > GetUniform( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		IndentBlock * m_block;
		GlslWriter & m_writer;
		Castor::StringStream m_stream;
		Castor::String m_name;
		uint32_t m_count{ 0u };
	};

	struct Struct
	{
		GlslWriter_API Struct( GlslWriter & p_writer, Castor::String const & p_name );
		GlslWriter_API void End();
		template< typename T > inline T GetMember( Castor::String const & p_name );
		template< typename T > inline Array< T > GetMember( Castor::String const & p_name, uint32_t p_dimension );
		IndentBlock * m_block;
		GlslWriter & m_writer;
		Castor::String m_name;
	};

	struct GlslWriterConfig
	{
		uint32_t m_shaderLanguageVersion;
		bool m_hasConstantsBuffers;
		bool m_hasTextureBuffers;
	};

	class GlslWriter
	{
		friend struct IndentBlock;
		friend struct Ubo;

	public:
		GlslWriter_API GlslWriter( GlslWriterConfig const & p_config );
		GlslWriter_API GlslWriter( GlslWriter const & p_rhs );
		GlslWriter_API GlslWriter & operator=( GlslWriter const & p_rhs );

		inline uint32_t GetShaderLanguageVersion()const
		{
			return m_config.m_shaderLanguageVersion;
		}

		inline uint32_t HasConstantsBuffers()const
		{
			return m_config.m_hasConstantsBuffers;
		}

		inline uint32_t HasTextureBuffers()const
		{
			return m_config.m_hasTextureBuffers;
		}

		inline bool HasTexelFetch()const
		{
			return GetShaderLanguageVersion() >= 130;
		}

		GlslWriter_API std::unique_ptr< LightingModel > CreateLightingModel( Castor::String const & p_name, bool p_shadows );

		GlslWriter_API Castor::String Finalise();
		GlslWriter_API void WriteAssign( Type const & p_lhs, Type const & p_rhs );
		GlslWriter_API void WriteAssign( Type const & p_lhs, int const & p_rhs );
		GlslWriter_API void WriteAssign( Type const & p_lhs, unsigned int const & p_rhs );
		GlslWriter_API void WriteAssign( Type const & p_lhs, float const & p_rhs );
		GlslWriter_API void For( Type && p_init, Expr const & p_cond, Expr const & p_incr, std::function< void() > p_function );
		GlslWriter_API GlslWriter & If( Expr const & p_cond, std::function< void() > p_function );
		GlslWriter_API GlslWriter & ElseIf( Expr const & p_cond, std::function< void() > p_function );
		GlslWriter_API void Else( std::function< void() > p_function );
		GlslWriter_API Struct GetStruct( Castor::String const & p_name );
		GlslWriter_API void EmitVertex();
		GlslWriter_API void EndPrimitive();
		GlslWriter_API void Discard();
		GlslWriter_API void InputGeometryLayout( Castor::String const & p_layout );
		GlslWriter_API void OutputGeometryLayout( Castor::String const & p_layout );
		GlslWriter_API void OutputVertexCount( uint32_t p_count );
		GlslWriter_API Vec4 Texture( Sampler1D const & p_sampler, Float const & p_value );
		GlslWriter_API Vec4 Texture( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 Texture( Sampler2D const & p_sampler, Vec2 const & p_value );
		GlslWriter_API Vec4 Texture( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 Texture( Sampler3D const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Vec4 Texture( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 Texture( SamplerCube const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Vec4 Texture( SamplerCube const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 TextureOffset( Sampler1D const & p_sampler, Float const & p_value, Int const & p_offset );
		GlslWriter_API Vec4 TextureOffset( Sampler1D const & p_sampler, Float const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Vec4 TextureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Vec4 TextureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Vec4 TextureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset );
		GlslWriter_API Vec4 TextureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset, Float const & p_lod );
		GlslWriter_API Vec4 TextureLodOffset( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Vec4 TextureLodOffset( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Vec4 TextureLodOffset( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const & p_offset );
		GlslWriter_API Vec4 TexelFetch( SamplerBuffer const & p_sampler, Type const & p_value );
		GlslWriter_API Vec4 TexelFetch( Sampler1D const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Vec4 TexelFetch( Sampler2D const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Vec4 TexelFetch( Sampler3D const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Optional< Vec4 > Texture( Optional< Sampler1D > const & p_sampler, Float const & p_value );
		GlslWriter_API Optional< Vec4 > Texture( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > Texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value );
		GlslWriter_API Optional< Vec4 > Texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > Texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Optional< Vec4 > Texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > Texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Optional< Vec4 > Texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > TextureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const & p_offset );
		GlslWriter_API Optional< Vec4 > TextureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > TextureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Optional< Vec4 > TextureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > TextureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset );
		GlslWriter_API Optional< Vec4 > TextureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > TextureLodOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Optional< Vec4 > TextureLodOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Optional< Vec4 > TextureLodOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const & p_offset );
		GlslWriter_API Optional< Vec4 > TexelFetch( Optional< SamplerBuffer > const & p_sampler, Type const & p_value );
		GlslWriter_API Optional< Vec4 > TexelFetch( Optional< Sampler1D > const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Optional< Vec4 > TexelFetch( Optional< Sampler2D > const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Optional< Vec4 > TexelFetch( Optional< Sampler3D > const & p_sampler, Type const & p_value, Int const & p_modif );

		template< typename T > void WriteAssign( Type const & p_lhs, Optional< T > const & p_rhs );
		template< typename RetType, typename FuncType, typename ... Params > inline void ImplementFunction( Castor::String const & p_name, FuncType p_function, Params && ... p_params );
		template< typename RetType > void Return( RetType const & p_return );
		template< typename ExprType > ExprType Paren( ExprType const p_expr );
		template< typename ExprType > ExprType Ternary( Type const & p_condition, Type const & p_left, Type const & p_right );
		template< typename T > inline T Cast( Type const & p_from );
		template< typename T > inline T GetAttribute( Castor::String const & p_name );
		template< typename T > inline T GetOutput( Castor::String const & p_name );
		template< typename T > inline T GetInput( Castor::String const & p_name );
		template< typename T > inline T GetLocale( Castor::String const & p_name );
		template< typename T > inline T GetLocale( Castor::String const & p_name, Expr const & p_rhs );
		template< typename T > inline T GetLocale( Castor::String const & p_name, Type const & p_rhs );
		template< typename T > inline T GetBuiltin( Castor::String const & p_name );
		template< typename T > inline T GetUniform( Castor::String const & p_name );
		template< typename T > inline T GetFragData( Castor::String const & p_name, uint32_t p_index );
		template< typename T > inline Array< T > GetAttribute( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Array< T > GetOutput( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Array< T > GetInput( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Array< T > GetLocale( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Array< T > GetLocale( Castor::String const & p_name, uint32_t p_dimension, Expr const & p_rhs );
		template< typename T > inline Array< T > GetLocale( Castor::String const & p_name, uint32_t p_dimension, Type const & p_rhs );
		template< typename T > inline Array< T > GetBuiltin( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Array< T > GetUniform( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Optional< T > GetAttribute( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< T > GetOutput( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< T > GetInput( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< T > GetLocale( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< T > GetLocale( Castor::String const & p_name, bool p_enabled, Expr const & p_rhs );
		template< typename T > inline Optional< T > GetLocale( Castor::String const & p_name, bool p_enabled, Type const & p_rhs );
		template< typename T > inline Optional< T > GetBuiltin( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< T > GetUniform( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< Array< T > > GetAttribute( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > GetOutput( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > GetInput( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > GetLocale( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > GetLocale( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled, Expr const & p_rhs );
		template< typename T > inline Optional< Array< T > > GetLocale( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled, Type const & p_rhs );
		template< typename T > inline Optional< Array< T > > GetBuiltin( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > GetUniform( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );

		GlslWriter_API GlslWriter & operator<<( Version const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( Attribute const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( In const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( Out const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( StdLayout const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( Layout const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( Uniform const & p_rhs );


		GlslWriter_API GlslWriter & operator<<( Endl const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( Endi const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( Castor::String const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( uint32_t const & p_rhs );

	private:
		GlslWriterConfig m_config;
		Castor::String m_uniform;
		std::unique_ptr< KeywordsBase > m_keywords;
		Castor::StringStream m_stream;
		int m_attributeIndex;
		int m_layoutIndex;
		LightingModelFactory m_lightingFactory;
	};

#define FOR( Writer, Type, Name, Init, Cond, Incr )\
	{\
		Type Name( &Writer, cuT( #Name ) );\
		Name.m_value << Type().m_type << cuT( #Name ) << cuT( " = " ) << cuT( #Init );\
		( Writer ).For( std::move( Name ), Expr( &( Writer ), Castor::String( Cond ) ), Expr( &( Writer ), Castor::String( Incr ) ), [&]()

#define ROF\
	 );\
	}

#define IF( Writer, Condition )\
	( Writer ).If( Expr( &( Writer ), Castor::String( Condition ) ), [&]()

#define ELSE\
 ).Else( [&]()

#define ELSEIF( Writer, Condition )\
 ).ElseIf( Expr( &( Writer ), Castor::String( Condition ) ), [&]()

#define FI\
 );

#define LOCALE_ASSIGN( Writer, Type, Name, Assign )\
	auto Name = ( Writer ).GetLocale< Type >( cuT( #Name ), Assign )

#define LOCALE_ASSIGN_ARRAY( Writer, Type, Name, Dimension, Assign )\
	auto Name = ( Writer ).GetLocale< Type >( cuT( #Name ), Dimension, Assign )
}

#define TERNARY( Writer, ExprType, Condition, Left, Right )\
	Writer.Ternary< ExprType >( Condition, Left, Right )

#include "GlslWriter.inl"

#endif
