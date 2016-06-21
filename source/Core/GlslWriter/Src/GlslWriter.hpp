/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GLSL_WRITER_H___
#define ___GLSL_WRITER_H___

#include "GlslOptionalArray.hpp"

#include <Factory.hpp>
#include <OwnedBy.hpp>

namespace GLSL
{
	struct Int;
	struct Vec4;
	struct SamplerBuffer;
	struct Sampler1D;
	struct Sampler2D;
	struct Sampler2DRect;
	struct Sampler3D;
	struct SamplerCube;

	class LightingModel;

	class LightingModelFactory
		: public Castor::Factory< LightingModel, Castor::String, std::unique_ptr< LightingModel >, std::function< std::unique_ptr< LightingModel >( uint32_t, GlslWriter & ) > >
	{
	public:
		GlslWriter_API virtual void Initialise();
	};

	struct Endl {};
	struct Endi {};

	struct IndentBlock
	{
		GlslWriter_API IndentBlock( GlslWriter & p_writter );
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
		IndentBlock * m_block;
		GlslWriter & m_writer;
		Castor::String m_name;
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

		GlslWriter_API std::unique_ptr< LightingModel > CreateLightingModel( Castor::String const & p_name, uint32_t p_flags );

		GlslWriter_API Castor::String Finalise();
		GlslWriter_API void WriteAssign( Type const & p_lhs, Type const & p_rhs );
		GlslWriter_API void WriteAssign( Type const & p_lhs, int const & p_rhs );
		GlslWriter_API void WriteAssign( Type const & p_lhs, float const & p_rhs );
		GlslWriter_API void For( Type && p_init, Expr const & p_cond, Expr const & p_incr, std::function< void() > p_function );
		GlslWriter_API GlslWriter & If( Expr const & p_cond, std::function< void() > p_function );
		GlslWriter_API GlslWriter & ElseIf( Expr const & p_cond, std::function< void() > p_function );
		GlslWriter_API void Else( std::function< void() > p_function );
		GlslWriter_API Struct GetStruct( Castor::String const & p_name );
		GlslWriter_API Ubo GetUbo( Castor::String const & p_name );
		GlslWriter_API void EmitVertex();
		GlslWriter_API void EndPrimitive();
		GlslWriter_API void Discard();
		GlslWriter_API void InputGeometryLayout( Castor::String const & p_layout );
		GlslWriter_API void OutputGeometryLayout( Castor::String const & p_layout );
		GlslWriter_API void OutputVertexCount( uint32_t p_count );
		GlslWriter_API Vec4 Texture1D( Sampler1D const & p_sampler, Type const & p_value );
		GlslWriter_API Vec4 Texture2D( Sampler2D const & p_sampler, Type const & p_value );
		GlslWriter_API Vec4 Texture3D( Sampler3D const & p_sampler, Type const & p_value );
		GlslWriter_API Vec4 TextureCube( SamplerCube const & p_sampler, Type const & p_value );
		GlslWriter_API Vec4 TexelFetch( SamplerBuffer const & p_sampler, Type const & p_value );
		GlslWriter_API Vec4 TexelFetch( Sampler1D const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Vec4 TexelFetch( Sampler2D const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Vec4 TexelFetch( Sampler3D const & p_sampler, Type const & p_value, Int const & p_modif );
		GlslWriter_API Optional< Vec4 > Texture1D( Optional< Sampler1D > const & p_sampler, Type const & p_value );
		GlslWriter_API Optional< Vec4 > Texture2D( Optional< Sampler2D > const & p_sampler, Type const & p_value );
		GlslWriter_API Optional< Vec4 > Texture3D( Optional< Sampler3D > const & p_sampler, Type const & p_value );
		GlslWriter_API Optional< Vec4 > TextureCube( Optional< SamplerCube > const & p_sampler, Type const & p_value );
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
		VariablesBase * m_variables;
		ConstantsBase * m_constants;
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

#define ELSEIF()\
	).ElseIf( [&]()

#define FI\
	);

#define LOCALE_ASSIGN( Writer, Type, Name, Assign )\
	auto Name = ( Writer ).GetLocale< Type >( cuT( #Name ), Assign )

#define LOCALE_ASSIGN_ARRAY( Writer, Type, Name, Dimension, Assign )\
	auto Name = ( Writer ).GetLocale< Type >( cuT( #Name ), Dimension, Assign )

#define UBO_MATRIX( Writer )\
	Ubo l_matrices = l_writer.GetUbo( cuT( "Matrices" ) );\
	auto c3d_mtxProjection = l_matrices.GetUniform< Mat4 >( cuT( "c3d_mtxProjection" ) );\
	auto c3d_mtxModel = l_matrices.GetUniform< Mat4 >( cuT( "c3d_mtxModel" ) );\
	auto c3d_mtxView = l_matrices.GetUniform< Mat4 >( cuT( "c3d_mtxView" ) );\
	auto c3d_mtxNormal = l_matrices.GetUniform< Mat4 >( cuT( "c3d_mtxNormal" ) );\
	auto c3d_mtxTexture0 = l_matrices.GetUniform< Mat4 >( cuT( "c3d_mtxTexture0" ) );\
	auto c3d_mtxTexture1 = l_matrices.GetUniform< Mat4 >( cuT( "c3d_mtxTexture1" ) );\
	auto c3d_mtxTexture2 = l_matrices.GetUniform< Mat4 >( cuT( "c3d_mtxTexture2" ) );\
	auto c3d_mtxTexture3 = l_matrices.GetUniform< Mat4 >( cuT( "c3d_mtxTexture3" ) );\
	auto c3d_mtxBones = l_matrices.GetUniform< Mat4 >( cuT( "c3d_mtxBones" ), 400 );\
	l_matrices.End()

#define UBO_PASS( Writer )\
	Ubo l_pass = l_writer.GetUbo( cuT( "Pass" ) );\
	auto c3d_v4MatAmbient = l_pass.GetUniform< Vec4 >( cuT( "c3d_v4MatAmbient" ) );\
	auto c3d_v4MatDiffuse = l_pass.GetUniform< Vec4 >( cuT( "c3d_v4MatDiffuse" ) );\
	auto c3d_v4MatEmissive = l_pass.GetUniform< Vec4 >( cuT( "c3d_v4MatEmissive" ) );\
	auto c3d_v4MatSpecular = l_pass.GetUniform< Vec4 >( cuT( "c3d_v4MatSpecular" ) );\
	auto c3d_fMatShininess = l_pass.GetUniform< Float >( cuT( "c3d_fMatShininess" ) );\
	auto c3d_fMatOpacity = l_pass.GetUniform< Float >( cuT( "c3d_fMatOpacity" ) );\
	l_pass.End()

#define UBO_SCENE( Writer )\
	Ubo l_scene = l_writer.GetUbo( cuT( "Scene" ) );\
	auto c3d_v4AmbientLight = l_scene.GetUniform< Vec4 >( cuT( "c3d_v4AmbientLight" ) );\
	auto c3d_v4BackgroundColour = l_scene.GetUniform< Vec4 >( cuT( "c3d_v4BackgroundColour" ) );\
	auto c3d_iLightsCount = l_scene.GetUniform< IVec4 >( cuT( "c3d_iLightsCount" ) );\
	auto c3d_v3CameraPosition = l_scene.GetUniform< Vec3 >( cuT( "c3d_v3CameraPosition" ) );\
	l_scene.End()

#define UBO_BILLBOARD( Writer )\
	Ubo l_billboard = l_writer.GetUbo( cuT( "Billboard" ) );\
	auto c3d_v2iDimensions = l_billboard.GetUniform< IVec2 >( cuT( "c3d_v2iDimensions" ) );\
	l_billboard.End();
}

#define TERNARY( Writer, ExprType, Condition, Left, Right )\
	Writer.Ternary< ExprType >( Condition, Left, Right )

#include "GlslWriter.inl"

#endif
