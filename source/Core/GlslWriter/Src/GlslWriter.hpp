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

namespace glsl
{
	struct GlslWriterConfig
	{
		uint32_t m_shaderLanguageVersion;
		bool m_hasConstantsBuffers;
		bool m_hasTextureBuffers;
		bool m_hasShaderStorageBuffers;
	};

	class GlslWriter
	{
		friend struct IndentBlock;
		friend class Ubo;
		friend class Ssbo;

	public:
		GlslWriter_API GlslWriter( GlslWriterConfig const & p_config );
		GlslWriter_API GlslWriter( GlslWriter const & rhs );
		GlslWriter_API GlslWriter & operator=( GlslWriter const & rhs );
		GlslWriter_API void registerName( castor::String const & name, TypeName type );
		GlslWriter_API void checkNameExists( castor::String const & name, TypeName type );

		inline uint32_t getShaderLanguageVersion()const
		{
			return m_config.m_shaderLanguageVersion;
		}

		inline bool hasConstantsBuffers()const
		{
			return m_config.m_hasConstantsBuffers;
		}

		inline bool hasTextureBuffers()const
		{
			return m_config.m_hasTextureBuffers;
		}

		inline bool hasShaderStorageBuffers()const
		{
			return m_config.m_hasShaderStorageBuffers;
		}

		inline bool hasTexelFetch()const
		{
			return getShaderLanguageVersion() >= 130;
		}

		inline void registerSsbo( castor::String const & name
			, Ssbo::Info const & p_info )
		{
			m_shader.registerSsbo( name, p_info );
		}

		inline void registerUbo( castor::String const & name
			, Ubo::Info const & p_info )
		{
			m_shader.registerUbo( name, p_info );
		}

		inline void registerConstant( castor::String const & name
			, TypeName type )
		{
			registerName( name, type );
			m_shader.registerConstant( name, type );
		}

		inline void registerSampler( castor::String const & name
			, TypeName type
			, uint32_t binding
			, uint32_t count )
		{
			registerName( name, type );
			m_shader.registerSampler( name, type, binding, count );
		}

		inline void registerUniform( castor::String const & name
			, TypeName type
			, uint32_t count )
		{
			registerName( name, type );
			m_shader.registerUniform( name, type, count );
		}

		inline void registerAttribute( castor::String const & name
			, TypeName type )
		{
			registerName( name, type );
			m_shader.registerAttribute( name, type );
		}

		inline void registerInput( castor::String const & name
			, TypeName type )
		{
			registerName( name, type );
			m_shader.registerInput( name, type );
		}

		inline void registerOutput( castor::String const & name
			, TypeName type )
		{
			registerName( name, type );
			m_shader.registerOutput( name, type );
		}

		GlslWriter_API Shader finalise();
		GlslWriter_API void writeAssign( Type const & p_lhs, Type const & rhs );
		GlslWriter_API void writeAssign( Type const & p_lhs, int const & rhs );
		GlslWriter_API void writeAssign( Type const & p_lhs, unsigned int const & rhs );
		GlslWriter_API void writeAssign( Type const & p_lhs, float const & rhs );
		GlslWriter_API void forStmt( Type && p_init, Expr const & p_cond, Expr const & p_incr, std::function< void() > p_function );
		GlslWriter_API void whileStmt( Expr const & p_cond, std::function< void() > p_function );
		GlslWriter_API GlslWriter & ifStmt( Expr const & p_cond, std::function< void() > p_function );
		GlslWriter_API GlslWriter & elseIfStmt( Expr const & p_cond, std::function< void() > p_function );
		GlslWriter_API void elseStmt( std::function< void() > p_function );
		GlslWriter_API Struct getStruct( castor::String const & name );
		GlslWriter_API void emitVertex();
		GlslWriter_API void endPrimitive();
		GlslWriter_API void discard();
		GlslWriter_API void inputGeometryLayout( castor::String const & p_layout );
		GlslWriter_API void outputGeometryLayout( castor::String const & p_layout );
		GlslWriter_API void outputGeometryLayout( castor::String const & p_layout, uint32_t p_count );
		GlslWriter_API void outputVertexCount( uint32_t p_count );
		GlslWriter_API Vec4 texture( Sampler1D const & p_sampler, Float const & p_value );
		GlslWriter_API Vec4 texture( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 texture( Sampler2D const & p_sampler, Vec2 const & p_value );
		GlslWriter_API Vec4 texture( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 texture( Sampler3D const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Vec4 texture( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 texture( SamplerCube const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Vec4 texture( SamplerCube const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 textureOffset( Sampler1D const & p_sampler, Float const & p_value, Int const & p_offset );
		GlslWriter_API Vec4 textureOffset( Sampler1D const & p_sampler, Float const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Vec4 textureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Vec4 textureOffset( Sampler2D const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Vec4 textureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset );
		GlslWriter_API Vec4 textureOffset( Sampler3D const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset, Float const & p_lod );
		GlslWriter_API Vec4 textureLodOffset( Sampler1D const & p_sampler, Float const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Vec4 textureLodOffset( Sampler2D const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Vec4 textureLodOffset( Sampler3D const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const & p_offset );
		GlslWriter_API Vec4 texelFetch( SamplerBuffer const & p_sampler, Type const & p_value );
		GlslWriter_API Vec4 texelFetch( Sampler1D const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Vec4 texelFetch( Sampler2D const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Vec4 texelFetch( Sampler3D const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Vec4 texture( Sampler1DArray const & p_sampler, Vec2 const & p_value );
		GlslWriter_API Vec4 texture( Sampler1DArray const & p_sampler, Vec2 const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 texture( Sampler2DArray const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Vec4 texture( Sampler2DArray const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value );
		GlslWriter_API Vec4 texture( SamplerCubeArray const & p_sampler, Vec4 const & p_value, Float const & p_lod );
		GlslWriter_API Vec4 textureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const & p_offset );
		GlslWriter_API Vec4 textureOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Vec4 textureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Vec4 textureOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Vec4 textureLodOffset( Sampler1DArray const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Vec4 textureLodOffset( Sampler2DArray const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Float texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value );
		GlslWriter_API Float texture( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod );
		GlslWriter_API Float texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Float texture( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Float texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value );
		GlslWriter_API Float texture( SamplerCubeShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod );
		GlslWriter_API Float textureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const & p_offset );
		GlslWriter_API Float textureOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Float textureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Float textureOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Float textureLodOffset( Sampler1DShadow const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Float textureLodOffset( Sampler2DShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Float texture( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Float texture( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Float texture( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value );
		GlslWriter_API Float texture( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod );
		GlslWriter_API Float texture( SamplerCubeArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_layer );
		GlslWriter_API Float texture( SamplerCubeArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod );
		GlslWriter_API Float textureOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Int const & p_offset );
		GlslWriter_API Float textureOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Float textureOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Float textureOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Float textureLodOffset( Sampler1DArrayShadow const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Float textureLodOffset( Sampler2DArrayShadow const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Optional< Vec4 > texture( Optional< Sampler1D > const & p_sampler, Float const & p_value );
		GlslWriter_API Optional< Vec4 > texture( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value );
		GlslWriter_API Optional< Vec4 > texture( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Optional< Vec4 > texture( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Optional< Vec4 > texture( Optional< SamplerCube > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const & p_offset );
		GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset );
		GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, IVec3 const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > textureLodOffset( Optional< Sampler1D > const & p_sampler, Float const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Optional< Vec4 > textureLodOffset( Optional< Sampler2D > const & p_sampler, Vec2 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Optional< Vec4 > textureLodOffset( Optional< Sampler3D > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec3 const & p_offset );
		GlslWriter_API Optional< Vec4 > texelFetch( Optional< SamplerBuffer > const & p_sampler, Type const & p_value );
		GlslWriter_API Optional< Vec4 > texelFetch( Optional< Sampler1D > const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Optional< Vec4 > texelFetch( Optional< Sampler2D > const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Optional< Vec4 > texelFetch( Optional< Sampler3D > const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Optional< Vec4 > texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value );
		GlslWriter_API Optional< Vec4 > texture( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Optional< Vec4 > texture( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value );
		GlslWriter_API Optional< Vec4 > texture( Optional< SamplerCubeArray > const & p_sampler, Vec4 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const & p_offset );
		GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Optional< Vec4 > textureOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Vec4 > textureLodOffset( Optional< Sampler1DArray > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Optional< Vec4 > textureLodOffset( Optional< Sampler2DArray > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Optional< Float > texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value );
		GlslWriter_API Optional< Float > texture( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Float > texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Optional< Float > texture( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Float > texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value );
		GlslWriter_API Optional< Float > texture( Optional< SamplerCubeShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Float > textureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const & p_offset );
		GlslWriter_API Optional< Float > textureOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Float > textureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Optional< Float > textureOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Float > textureLodOffset( Optional< Sampler1DShadow > const & p_sampler, Vec2 const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Optional< Float > textureLodOffset( Optional< Sampler2DShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, IVec2 const & p_offset );
		GlslWriter_API Optional< Float > texture( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value );
		GlslWriter_API Optional< Float > texture( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Float > texture( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value );
		GlslWriter_API Optional< Float > texture( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod );
		GlslWriter_API Optional< Float > texture( Optional< SamplerCubeArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_layer );
		GlslWriter_API Optional< Float > texture( Optional< SamplerCubeArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_layer, Float const & p_lod );
		GlslWriter_API Optional< Float > textureOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Int const & p_offset );
		GlslWriter_API Optional< Float > textureOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Int const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Float > textureOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset );
		GlslWriter_API Optional< Float > textureOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, IVec2 const & p_offset, Float const & p_lod );
		GlslWriter_API Optional< Float > textureLodOffset( Optional< Sampler1DArrayShadow > const & p_sampler, Vec3 const & p_value, Float const & p_lod, Int const & p_offset );
		GlslWriter_API Optional< Float > textureLodOffset( Optional< Sampler2DArrayShadow > const & p_sampler, Vec4 const & p_value, Float const & p_lod, IVec2 const & p_offset );

		template< typename T > void writeAssign( Type const & p_lhs, Optional< T > const & rhs );
		template< typename RetType, typename FuncType, typename ... Params > inline Function< RetType, Params... > implementFunction( castor::String const & name, FuncType p_function, Params && ... p_params );
		template< typename RetType > void returnStmt( RetType const & p_return );
		template< typename ExprType > ExprType paren( ExprType const p_expr );
		template< typename ExprType > ExprType ternary( Type const & p_condition, ExprType const & p_left, ExprType const & p_right );
		template< typename T > inline T cast( Type const & p_from );
		template< typename T > inline T declConstant( castor::String const & name, T const & rhs );
		template< typename T > inline Optional< T > declConstant( castor::String const & name, T const & rhs, bool enabled );
		template< typename T > inline T declSampler( castor::String const & name, uint32_t binding );
		template< typename T > inline Optional< T > declSampler( castor::String const & name, uint32_t binding, bool enabled );
		template< typename T > inline Array< T > declSampler( castor::String const & name, uint32_t binding, uint32_t dimension );
		template< typename T > inline Optional< Array< T > > declSampler( castor::String const & name, uint32_t binding, uint32_t dimension, bool enabled );
		template< typename T > inline T declAttribute( castor::String const & name );
		template< typename T > inline T declOutput( castor::String const & name );
		template< typename T > inline T declInput( castor::String const & name );
		template< typename T > inline T declLocale( castor::String const & name );
		template< typename T > inline T declLocale( castor::String const & name, T const & rhs );
		template< typename T > inline T declBuiltin( castor::String const & name );
		template< typename T > inline T getBuiltin( castor::String const & name );
		template< typename T > inline T declUniform( castor::String const & name );
		template< typename T > inline T declUniform( castor::String const & name, T const & rhs );
		template< typename T > inline T declFragData( castor::String const & name, uint32_t p_index );
		template< typename T > inline Array< T > declAttribute( castor::String const & name, uint32_t dimension );
		template< typename T > inline Array< T > declOutput( castor::String const & name, uint32_t dimension );
		template< typename T > inline Array< T > declOutputArray( castor::String const & name );
		template< typename T > inline Array< T > declInput( castor::String const & name, uint32_t dimension );
		template< typename T > inline Array< T > declInputArray( castor::String const & name );
		template< typename T > inline Array< T > declLocale( castor::String const & name, uint32_t dimension );
		template< typename T > inline Array< T > declLocale( castor::String const & name, uint32_t dimension, T const & rhs );
		template< typename T > inline Array< T > declBuiltin( castor::String const & name, uint32_t dimension );
		template< typename T > inline Array< T > declBuiltinArray( castor::String const & name );
		template< typename T > inline Array< T > getBuiltin( castor::String const & name, uint32_t dimension );
		template< typename T > inline Array< T > getBuiltinArray( castor::String const & name );
		template< typename T > inline Array< T > declUniform( castor::String const & name, uint32_t dimension );
		template< typename T > inline Array< T > declUniformArray( castor::String const & name );
		template< typename T > inline Array< T > declUniform( castor::String const & name, uint32_t dimension, std::vector< T > const & rhs );
		template< typename T > inline Optional< T > declAttribute( castor::String const & name, bool enabled );
		template< typename T > inline Optional< T > declOutput( castor::String const & name, bool enabled );
		template< typename T > inline Optional< T > declInput( castor::String const & name, bool enabled );
		template< typename T > inline Optional< T > declLocale( castor::String const & name, bool enabled );
		template< typename T > inline Optional< T > declLocale( castor::String const & name, Optional< T > const & rhs );
		template< typename T > inline Optional< T > declLocale( castor::String const & name, bool enabled, T const & rhs );
		template< typename T > inline Optional< T > declBuiltin( castor::String const & name, bool enabled );
		template< typename T > inline Optional< T > getBuiltin( castor::String const & name, bool enabled );
		template< typename T > inline Optional< T > declUniform( castor::String const & name, bool enabled );
		template< typename T > inline Optional< T > declUniform( castor::String const & name, bool enabled, T const & rhs );
		template< typename T > inline Optional< Array< T > > declAttribute( castor::String const & name, uint32_t dimension, bool enabled );
		template< typename T > inline Optional< Array< T > > declOutput( castor::String const & name, uint32_t dimension, bool enabled );
		template< typename T > inline Optional< Array< T > > declOutputArray( castor::String const & name, bool enabled );
		template< typename T > inline Optional< Array< T > > declInput( castor::String const & name, uint32_t dimension, bool enabled );
		template< typename T > inline Optional< Array< T > > declInputArray( castor::String const & name, bool enabled );
		template< typename T > inline Optional< Array< T > > declLocale( castor::String const & name, uint32_t dimension, bool enabled );
		template< typename T > inline Optional< Array< T > > declLocale( castor::String const & name, uint32_t dimension, bool enabled, T const & rhs );
		template< typename T > inline Optional< Array< T > > declBuiltin( castor::String const & name, uint32_t dimension, bool enabled );
		template< typename T > inline Optional< Array< T > > declBuiltinArray( castor::String const & name, bool enabled );
		template< typename T > inline Optional< Array< T > > getBuiltin( castor::String const & name, uint32_t dimension, bool enabled );
		template< typename T > inline Optional< Array< T > > getBuiltinArray( castor::String const & name, bool enabled );
		template< typename T > inline Optional< Array< T > > declUniform( castor::String const & name, uint32_t dimension, bool enabled );
		template< typename T > inline Optional< Array< T > > declUniformArray( castor::String const & namedimension, bool enabled );
		template< typename T > inline Optional< Array< T > > declUniform( castor::String const & name, uint32_t dimension, bool enabled, std::vector< T > const & rhs );

		GlslWriter_API GlslWriter & operator<<( Version const & rhs );
		GlslWriter_API GlslWriter & operator<<( Attribute const & rhs );
		GlslWriter_API GlslWriter & operator<<( In const & rhs );
		GlslWriter_API GlslWriter & operator<<( Out const & rhs );
		GlslWriter_API GlslWriter & operator<<( Layout const & rhs );
		GlslWriter_API GlslWriter & operator<<( Uniform const & rhs );
		GlslWriter_API GlslWriter & operator<<( InputLayout const & rhs );
		GlslWriter_API GlslWriter & operator<<( OutputLayout const & rhs );

		GlslWriter_API GlslWriter & operator<<( Endl const & rhs );
		GlslWriter_API GlslWriter & operator<<( Endi const & rhs );
		GlslWriter_API GlslWriter & operator<<( castor::String const & rhs );
		GlslWriter_API GlslWriter & operator<<( uint32_t const & rhs );

	private:
		std::map< castor::String, TypeName > m_registered;
		GlslWriterConfig m_config;
		castor::String m_uniform;
		std::unique_ptr< KeywordsBase > m_keywords;
		castor::StringStream m_stream;
		uint32_t m_attributeIndex{ 0u };
		uint32_t m_layoutIndex{ 0u };
		Shader m_shader;
	};

#define FOR( Writer, Type, Name, Init, Cond, Incr )\
	{\
		Type Name( &Writer, cuT( #Name ) );\
		Name.m_value << Type().m_type << cuT( #Name ) << cuT( " = " ) << cuT( #Init );\
		( Writer ).forStmt( std::move( Name ), glsl::Expr( &( Writer ), castor::String( Cond ) ), glsl::Expr( &( Writer ), castor::String( Incr ) ), [&]()

#define ROF\
	 );\
	}

#define WHILE( Writer, Cond )\
	{\
		( Writer ).whileStmt( glsl::Expr( &( Writer ), castor::String( Cond ) ), [&]()

#define ELIHW\
	 );\
	}

#define IF( Writer, Condition )\
	( Writer ).ifStmt( glsl::Expr( &( Writer ), castor::String( Condition ) ), [&]()

#define ELSE\
 ).elseStmt( [&]()

#define ELSEIF( Writer, Condition )\
 ).elseIfStmt( glsl::Expr( &( Writer ), castor::String( Condition ) ), [&]()

#define FI\
 );
}

#define TERNARY( Writer, ExprType, Condition, Left, Right )\
	Writer.ternary< ExprType >( Condition, Left, Right )

#include "GlslWriter.inl"

#endif
