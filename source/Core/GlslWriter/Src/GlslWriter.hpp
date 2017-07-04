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

#include "GlslFunction.hpp"
#include "GlslIndentBlock.hpp"
#include "GlslInputLayout.hpp"
#include "GlslOutputLayout.hpp"
#include "GlslShader.hpp"
#include "GlslStruct.hpp"

#include <Design/OwnedBy.hpp>

#include <map>

namespace GLSL
{
	struct GlslWriterConfig
	{
		uint32_t m_shaderLanguageVersion;
		bool m_hasConstantsBuffers;
		bool m_hasTextureBuffers;
	};

	class GlslWriter
	{
		friend struct IndentBlock;
		friend class Ubo;
		friend class Ssbo;

	public:
		GlslWriter_API GlslWriter( GlslWriterConfig const & p_config );
		GlslWriter_API GlslWriter( GlslWriter const & p_rhs );
		GlslWriter_API GlslWriter & operator=( GlslWriter const & p_rhs );
		GlslWriter_API void RegisterName( Castor::String const & p_name, TypeName p_type );
		GlslWriter_API void CheckNameExists( Castor::String const & p_name, TypeName p_type );

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

		inline void RegisterSsbo( Castor::String const & p_name, Ssbo::Info const & p_info )
		{
			m_shader.RegisterSsbo( p_name, p_info );
		}

		inline void RegisterUbo( Castor::String const & p_name, Ubo::Info const & p_info )
		{
			m_shader.RegisterUbo( p_name, p_info );
		}

		inline void RegisterUniform( Castor::String const & p_name, TypeName p_type )
		{
			RegisterName( p_name, p_type );
			m_shader.RegisterUniform( p_name, p_type );
		}

		inline void RegisterAttribute( Castor::String const & p_name, TypeName p_type )
		{
			RegisterName( p_name, p_type );
			m_shader.RegisterAttribute( p_name, p_type );
		}

		inline void RegisterInput( Castor::String const & p_name, TypeName p_type )
		{
			RegisterName( p_name, p_type );
			m_shader.RegisterInput( p_name, p_type );
		}

		inline void RegisterOutput( Castor::String const & p_name, TypeName p_type )
		{
			RegisterName( p_name, p_type );
			m_shader.RegisterOutput( p_name, p_type );
		}

		GlslWriter_API std::unique_ptr< LightingModel > CreateLightingModel( Castor::String const & p_name, ShadowType p_shadows );
		GlslWriter_API std::unique_ptr< LightingModel > CreateDirectionalLightingModel( Castor::String const & p_name, ShadowType p_shadows );
		GlslWriter_API std::unique_ptr< LightingModel > CreatePointLightingModel( Castor::String const & p_name, ShadowType p_shadows );
		GlslWriter_API std::unique_ptr< LightingModel > CreateSpotLightingModel( Castor::String const & p_name, ShadowType p_shadows );

		GlslWriter_API Shader Finalise();
		GlslWriter_API void WriteAssign( Type const & p_lhs, Type const & p_rhs );
		GlslWriter_API void WriteAssign( Type const & p_lhs, int const & p_rhs );
		GlslWriter_API void WriteAssign( Type const & p_lhs, unsigned int const & p_rhs );
		GlslWriter_API void WriteAssign( Type const & p_lhs, float const & p_rhs );
		GlslWriter_API void For( Type && p_init, Expr const & p_cond, Expr const & p_incr, std::function< void() > p_function );
		GlslWriter_API void While( Expr const & p_cond, std::function< void() > p_function );
		GlslWriter_API GlslWriter & If( Expr const & p_cond, std::function< void() > p_function );
		GlslWriter_API GlslWriter & ElseIf( Expr const & p_cond, std::function< void() > p_function );
		GlslWriter_API void Else( std::function< void() > p_function );
		GlslWriter_API Struct GetStruct( Castor::String const & p_name );
		GlslWriter_API void EmitVertex();
		GlslWriter_API void EndPrimitive();
		GlslWriter_API void Discard();
		GlslWriter_API void InputGeometryLayout( Castor::String const & p_layout );
		GlslWriter_API void OutputGeometryLayout( Castor::String const & p_layout );
		GlslWriter_API void OutputGeometryLayout( Castor::String const & p_layout, uint32_t p_count );
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
		GlslWriter_API Vec4 Texture( Sampler1DArray const & p_sampler, Vec2 const & p_value );
		GlslWriter_API Vec4 Texture( Sampler1DArray const & p_sampler, Vec2 const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 Texture( Sampler2DArray const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Vec4 Texture( Sampler2DArray const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 Texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value );
		GlslWriter_API Vec4 Texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 TextureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const & p_offset );
		GlslWriter_API Vec4 TextureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Vec4 TextureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Vec4 TextureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Vec4 TextureLodOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Vec4 TextureLodOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Float Texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value );
		GlslWriter_API Float Texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod );
		GlslWriter_API Float Texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Float Texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Float Texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value );
		GlslWriter_API Float Texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod );
		GlslWriter_API Float TextureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const & p_offset );
		GlslWriter_API Float TextureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Float TextureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Float TextureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Float TextureLodOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Float TextureLodOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Float Texture( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Float Texture( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Float Texture( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value );
		GlslWriter_API Float Texture( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod );
		GlslWriter_API Float Texture( SamplerCubeArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_layer );
		GlslWriter_API Float Texture( SamplerCubeArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod );
		GlslWriter_API Float TextureOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Int const & p_offset );
		GlslWriter_API Float TextureOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Float TextureOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Float TextureOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Float TextureLodOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Float TextureLodOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const & p_offset );
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
		GlslWriter_API Optional< Vec4 > Texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value );
		GlslWriter_API Optional< Vec4 > Texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > Texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Optional< Vec4 > Texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > Texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value );
		GlslWriter_API Optional< Vec4 > Texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > TextureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const & p_offset );
		GlslWriter_API Optional< Vec4 > TextureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > TextureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Optional< Vec4 > TextureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > TextureLodOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Optional< Vec4 > TextureLodOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Optional< Float > Texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value );
		GlslWriter_API Optional< Float > Texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Float > Texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Optional< Float > Texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Float > Texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value );
		GlslWriter_API Optional< Float > Texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Float > TextureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const & p_offset );
		GlslWriter_API Optional< Float > TextureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Float > TextureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Optional< Float > TextureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Float > TextureLodOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Optional< Float > TextureLodOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Optional< Float > Texture( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Optional< Float > Texture( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Float > Texture( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value );
		GlslWriter_API Optional< Float > Texture( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Float > Texture( Optional< SamplerCubeArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_layer );
		GlslWriter_API Optional< Float > Texture( Optional< SamplerCubeArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod );
		GlslWriter_API Optional< Float > TextureOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Int const & p_offset );
		GlslWriter_API Optional< Float > TextureOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Float > TextureOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Optional< Float > TextureOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Float > TextureLodOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Optional< Float > TextureLodOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const & p_offset );

		template< typename T > void WriteAssign( Type const & p_lhs, Optional< T > const & p_rhs );
		template< typename RetType, typename FuncType, typename ... Params > inline Function< RetType, Params... > ImplementFunction( Castor::String const & p_name, FuncType p_function, Params && ... p_params );
		template< typename RetType > void Return( RetType const & p_return );
		template< typename ExprType > ExprType Paren( ExprType const p_expr );
		template< typename ExprType > ExprType Ternary( Type const & p_condition, ExprType const & p_left, ExprType const & p_right );
		template< typename T > inline T Cast( Type const & p_from );
		template< typename T > inline T DeclAttribute( Castor::String const & p_name );
		template< typename T > inline T DeclOutput( Castor::String const & p_name );
		template< typename T > inline T DeclInput( Castor::String const & p_name );
		template< typename T > inline T DeclLocale( Castor::String const & p_name );
		template< typename T > inline T DeclLocale( Castor::String const & p_name, T const & p_rhs );
		template< typename T > inline T DeclBuiltin( Castor::String const & p_name );
		template< typename T > inline T GetBuiltin( Castor::String const & p_name );
		template< typename T > inline T DeclUniform( Castor::String const & p_name );
		template< typename T > inline T DeclUniform( Castor::String const & p_name, T const & p_rhs );
		template< typename T > inline T DeclFragData( Castor::String const & p_name, uint32_t p_index );
		template< typename T > inline Array< T > DeclAttribute( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Array< T > DeclOutput( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Array< T > DeclOutputArray( Castor::String const & p_name );
		template< typename T > inline Array< T > DeclInput( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Array< T > DeclInputArray( Castor::String const & p_name );
		template< typename T > inline Array< T > DeclLocale( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Array< T > DeclLocale( Castor::String const & p_name, uint32_t p_dimension, T const & p_rhs );
		template< typename T > inline Array< T > DeclBuiltin( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Array< T > DeclBuiltinArray( Castor::String const & p_name );
		template< typename T > inline Array< T > GetBuiltin( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Array< T > GetBuiltinArray( Castor::String const & p_name );
		template< typename T > inline Array< T > DeclUniform( Castor::String const & p_name, uint32_t p_dimension );
		template< typename T > inline Array< T > DeclUniformArray( Castor::String const & p_name );
		template< typename T > inline Array< T > DeclUniform( Castor::String const & p_name, uint32_t p_dimension, std::vector< T > const & p_rhs );
		template< typename T > inline Optional< T > DeclAttribute( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< T > DeclOutput( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< T > DeclInput( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< T > DeclLocale( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< T > DeclLocale( Castor::String const & p_name, bool p_enabled, T const & p_rhs );
		template< typename T > inline Optional< T > DeclBuiltin( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< T > GetBuiltin( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< T > DeclUniform( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< T > DeclUniform( Castor::String const & p_name, bool p_enabled, T const & p_rhs );
		template< typename T > inline Optional< Array< T > > DeclAttribute( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > DeclOutput( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > DeclOutputArray( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< Array< T > > DeclInput( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > DeclInputArray( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< Array< T > > DeclLocale( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > DeclLocale( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled, T const & p_rhs );
		template< typename T > inline Optional< Array< T > > DeclBuiltin( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > DeclBuiltinArray( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< Array< T > > GetBuiltin( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > GetBuiltinArray( Castor::String const & p_name, bool p_enabled );
		template< typename T > inline Optional< Array< T > > DeclUniform( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > DeclUniformArray( Castor::String const & p_namep_dimension, bool p_enabled );
		template< typename T > inline Optional< Array< T > > DeclUniform( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled, std::vector< T > const & p_rhs );

		GlslWriter_API GlslWriter & operator<<( Version const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( Attribute const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( In const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( Out const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( Layout const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( Uniform const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( InputLayout const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( OutputLayout const & p_rhs );

		GlslWriter_API GlslWriter & operator<<( Endl const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( Endi const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( Castor::String const & p_rhs );
		GlslWriter_API GlslWriter & operator<<( uint32_t const & p_rhs );

	private:
		std::map< Castor::String, TypeName > m_registered;
		GlslWriterConfig m_config;
		Castor::String m_uniform;
		std::unique_ptr< KeywordsBase > m_keywords;
		Castor::StringStream m_stream;
		uint32_t m_attributeIndex{ 0u };
		uint32_t m_layoutIndex{ 0u };
		LightingModelFactory m_lightingFactory;
		Shader m_shader;
	};

#define FOR( Writer, Type, Name, Init, Cond, Incr )\
	{\
		Type Name( &Writer, cuT( #Name ) );\
		Name.m_value << Type().m_type << cuT( #Name ) << cuT( " = " ) << cuT( #Init );\
		( Writer ).For( std::move( Name ), Expr( &( Writer ), Castor::String( Cond ) ), Expr( &( Writer ), Castor::String( Incr ) ), [&]()

#define ROF\
	 );\
	}

#define WHILE( Writer, Cond )\
	{\
		( Writer ).While( Expr( &( Writer ), Castor::String( Cond ) ), [&]()

#define ELIHW\
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
}

#define TERNARY( Writer, ExprType, Condition, Left, Right )\
	Writer.Ternary< ExprType >( Condition, Left, Right )

#include "GlslWriter.inl"

#endif
