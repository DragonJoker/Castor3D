/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#include "GlslArray.hpp"

#include "GlHolder.hpp"
#include "OpenGl.hpp"

#include <Factory.hpp>

namespace GlRender
{
	namespace GLSL
	{
		struct Int;
		struct Vec4;
		struct SamplerBuffer;
		struct Sampler1D;
		struct Sampler2D;
		struct Sampler3D;

		class LightingModel;

		class LightingModelFactory
			: public Castor::Factory< LightingModel, Castor::String, std::unique_ptr< LightingModel >, std::function< std::unique_ptr< LightingModel >( uint32_t, GlslWriter & ) > >
		{
		public:
			C3D_Gl_API virtual void Initialise();
		};

		struct Endl {};
		struct Endi {};

		struct IndentBlock
		{
			C3D_Gl_API IndentBlock( GlslWriter & p_writter );
			C3D_Gl_API ~IndentBlock();
			Castor::StringStream & m_stream;
			int m_indent;
		};

		struct Ubo
		{
			C3D_Gl_API Ubo( GlslWriter & p_writer, Castor::String const & p_name );
			C3D_Gl_API void End();
			template< typename T > inline T GetUniform( Castor::String const & p_name );
			template< typename T > inline Array< T > GetUniform( Castor::String const & p_name, uint32_t p_dimension );
			IndentBlock * m_block;
			GlslWriter & m_writer;
			Castor::String m_name;
		};

		struct Struct
		{
			C3D_Gl_API Struct( GlslWriter & p_writer, Castor::String const & p_name );
			C3D_Gl_API void End();
			template< typename T > inline T GetMember( Castor::String const & p_name );
			template< typename T > inline Array< T > GetMember( Castor::String const & p_name, uint32_t p_dimension );
			IndentBlock * m_block;
			GlslWriter & m_writer;
			Castor::String m_name;
		};

		class GlslWriter
			: public Holder
		{
			friend struct IndentBlock;
			friend struct Ubo;

		public:
			C3D_Gl_API GlslWriter( OpenGl & p_gl, Castor3D::eSHADER_TYPE p_type );

			inline bool HasTexelFetch()const
			{
				return GetOpenGl().GetGlslVersion() >= 130;
			}

			C3D_Gl_API std::unique_ptr< LightingModel > CreateLightingModel( Castor::String const & p_name, uint32_t p_flags );

			C3D_Gl_API Castor::String Finalise();
			C3D_Gl_API void WriteAssign( Type const & p_lhs, Type const & p_rhs );
			C3D_Gl_API void WriteAssign( Type const & p_lhs, int const & p_rhs );
			C3D_Gl_API void WriteAssign( Type const & p_lhs, float const & p_rhs );
			C3D_Gl_API void For( Type const & p_init, Expr const & p_cond, Expr const & p_incr, std::function< void( Type const & ) > p_function );
			C3D_Gl_API GlslWriter & If( Expr const & p_cond, std::function< void() > p_function );
			C3D_Gl_API GlslWriter & ElseIf( Expr const & p_cond, std::function< void() > p_function );
			C3D_Gl_API void Else( std::function< void() > p_function );
			C3D_Gl_API Struct GetStruct( Castor::String const & p_name );
			C3D_Gl_API Ubo GetUbo( Castor::String const & p_name );
			C3D_Gl_API void EmitVertex();
			C3D_Gl_API void EndPrimitive();
			C3D_Gl_API void Discard();
			C3D_Gl_API Vec4 Texture1D( Sampler1D const & p_sampler, Type const & p_value );
			C3D_Gl_API Vec4 Texture2D( Sampler2D const & p_sampler, Type const & p_value );
			C3D_Gl_API Vec4 Texture3D( Sampler3D const & p_sampler, Type const & p_value );
			C3D_Gl_API Vec4 TexelFetch( SamplerBuffer const & p_sampler, Type const & p_value );
			C3D_Gl_API Vec4 TexelFetch( Sampler1D const & p_sampler, Type const & p_value, Int const & p_modif );
			C3D_Gl_API Vec4 TexelFetch( Sampler2D const & p_sampler, Type const & p_value, Int const & p_modif );
			C3D_Gl_API Vec4 TexelFetch( Sampler3D const & p_sampler, Type const & p_value, Int const & p_modif );
			C3D_Gl_API Optional< Vec4 > Texture1D( Optional< Sampler1D > const & p_sampler, Type const & p_value );
			C3D_Gl_API Optional< Vec4 > Texture2D( Optional< Sampler2D > const & p_sampler, Type const & p_value );
			C3D_Gl_API Optional< Vec4 > Texture3D( Optional< Sampler3D > const & p_sampler, Type const & p_value );
			C3D_Gl_API Optional< Vec4 > TexelFetch( Optional< SamplerBuffer > const & p_sampler, Type const & p_value );
			C3D_Gl_API Optional< Vec4 > TexelFetch( Optional< Sampler1D > const & p_sampler, Type const & p_value, Int const & p_modif );
			C3D_Gl_API Optional< Vec4 > TexelFetch( Optional< Sampler2D > const & p_sampler, Type const & p_value, Int const & p_modif );
			C3D_Gl_API Optional< Vec4 > TexelFetch( Optional< Sampler3D > const & p_sampler, Type const & p_value, Int const & p_modif );

			template< typename T > void WriteAssign( Type const & p_lhs, Optional< T > const & p_rhs );
			template< typename RetType, typename FuncType, typename ... Params > inline void ImplementFunction( Castor::String const & p_name, FuncType p_function, Params && ... p_params );
			template< typename RetType > void Return( RetType const & p_return );
			template< typename ExprType > ExprType Paren( ExprType const p_expr );
			template< typename ExprType > ExprType Ternary( Type const & p_condition, Type const & p_left, Type const & p_right );
			template< typename T > inline T Cast( Type const & p_from );
			template< typename T > inline T GetAttribute( Castor::String const & p_name );
			template< typename T > inline T GetOut( Castor::String const & p_name );
			template< typename T > inline T GetIn( Castor::String const & p_name );
			template< typename T > inline T GetLocale( Castor::String const & p_name );
			template< typename T > inline T GetLocale( Castor::String const & p_name, Expr const & p_rhs );
			template< typename T > inline T GetLocale( Castor::String const & p_name, Type const & p_rhs );
			template< typename T > inline T GetBuiltin( Castor::String const & p_name );
			template< typename T > inline T GetUniform( Castor::String const & p_name );
			template< typename T > inline T GetFragData( Castor::String const & p_name, uint32_t p_index );
			template< typename T > inline Array< T > GetAttribute( Castor::String const & p_name, uint32_t p_dimension );
			template< typename T > inline Array< T > GetOut( Castor::String const & p_name, uint32_t p_dimension );
			template< typename T > inline Array< T > GetIn( Castor::String const & p_name, uint32_t p_dimension );
			template< typename T > inline Array< T > GetLocale( Castor::String const & p_name, uint32_t p_dimension );
			template< typename T > inline Array< T > GetLocale( Castor::String const & p_name, uint32_t p_dimension, Expr const & p_rhs );
			template< typename T > inline Array< T > GetLocale( Castor::String const & p_name, uint32_t p_dimension, Type const & p_rhs );
			template< typename T > inline Array< T > GetBuiltin( Castor::String const & p_name, uint32_t p_dimension );
			template< typename T > inline Array< T > GetUniform( Castor::String const & p_name, uint32_t p_dimension );
			template< typename T > inline Optional< T > GetAttribute( Castor::String const & p_name, bool p_enabled );
			template< typename T > inline Optional< T > GetOut( Castor::String const & p_name, bool p_enabled );
			template< typename T > inline Optional< T > GetIn( Castor::String const & p_name, bool p_enabled );
			template< typename T > inline Optional< T > GetLocale( Castor::String const & p_name, bool p_enabled );
			template< typename T > inline Optional< T > GetLocale( Castor::String const & p_name, bool p_enabled, Expr const & p_rhs );
			template< typename T > inline Optional< T > GetLocale( Castor::String const & p_name, bool p_enabled, Type const & p_rhs );
			template< typename T > inline Optional< T > GetBuiltin( Castor::String const & p_name, bool p_enabled );
			template< typename T > inline Optional< T > GetUniform( Castor::String const & p_name, bool p_enabled );

			C3D_Gl_API GlslWriter & operator<<( Version const & p_rhs );
			C3D_Gl_API GlslWriter & operator<<( Attribute const & p_rhs );
			C3D_Gl_API GlslWriter & operator<<( In const & p_rhs );
			C3D_Gl_API GlslWriter & operator<<( Out const & p_rhs );
			C3D_Gl_API GlslWriter & operator<<( StdLayout const & p_rhs );
			C3D_Gl_API GlslWriter & operator<<( Layout const & p_rhs );
			C3D_Gl_API GlslWriter & operator<<( Uniform const & p_rhs );

			C3D_Gl_API GlslWriter & operator<<( Legacy_MatrixOut const & p_rhs );
			C3D_Gl_API GlslWriter & operator<<( Legacy_MatrixCopy const & p_rhs );
			C3D_Gl_API GlslWriter & operator<<( Legacy_PixelModelView const & p_rhs );

			C3D_Gl_API GlslWriter & operator<<( Endl const & p_rhs );
			C3D_Gl_API GlslWriter & operator<<( Endi const & p_rhs );
			C3D_Gl_API GlslWriter & operator<<( Castor::String const & p_rhs );
			C3D_Gl_API GlslWriter & operator<<( uint32_t const & p_rhs );

		private:
			Castor::String m_uniform;
			VariablesBase * m_variables;
			ConstantsBase * m_constants;
			std::unique_ptr< KeywordsBase > m_keywords;
			Castor::StringStream m_stream;
			int m_attributeIndex;
			int m_layoutIndex;
			Castor3D::eSHADER_TYPE m_type;
			LightingModelFactory m_lightingFactory;
		};
	}

#define FOR( Writer, Type, Name, Init, Cond, Incr )\
	{\
		Type Name( &Writer, cuT( #Name ) );\
		Name.m_value << Type().m_type << cuT( #Name ) << cuT( " = " ) << cuT( #Init );\
		( Writer ).For( Name, Expr( &( Writer ), Castor::String( Cond ) ), Expr( &( Writer ), Castor::String( Incr ) ), [&]( Type const & i )

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

#define LOCALE( Writer, Type, Name )\
	Type Name = ( Writer ).GetLocale< Type >( cuT( #Name ) )

#define LOCALE_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetLocale< Type >( cuT( #Name ), Dimension )

#define LOCALE_ASSIGN( Writer, Type, Name, Assign )\
	Type Name = ( Writer ).GetLocale< Type >( cuT( #Name ), Assign )

#define LOCALE_ASSIGN_ARRAY( Writer, Type, Name, Dimension, Assign )\
	Array< Type > Name = ( Writer ).GetLocale< Type >( cuT( #Name ), Dimension, Assign )

#define BUILTIN( Writer, Type, Name )\
	Type Name = ( Writer ).GetBuiltin< Type >( cuT( #Name ) )

#define BUILTIN_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetBuiltin< Type >( cuT( #Name ), Dimension )

#define FRAG_OUTPUT( Writer, Type, Name, Index )\
	Type Name = ( Writer ).GetFragData< Type >( cuT( #Name ), Index )

#define FRAG_OUTPUT_ARRAY( Writer, Type, Name, Index, Dimension )\
	Type Name = ( Writer ).GetFragData< Type >( cuT( #Name ), Index, Dimension )

#define OUT( Writer, Type, Name )\
	Type Name = ( Writer ).GetOut< Type >( cuT( #Name ) )

#define OUT_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetOut< Type >( cuT( #Name ), Dimension )

#define IN( Writer, Type, Name )\
	Type Name = ( Writer ).GetIn< Type >( cuT( #Name ) )

#define IN_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetIn< Type >( cuT( #Name ), Dimension )

#define ATTRIBUTE( Writer, Type, Name )\
	Type Name = ( Writer ).GetAttribute< Type >( cuT( #Name ) )

#define ATTRIBUTE_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetAttribute< Type >( cuT( #Name ), Dimension )

#define UNIFORM( Writer, Type, Name )\
	Type Name = ( Writer ).GetUniform< Type >( cuT( #Name ) )

#define UNIFORM_ARRAY( Writer, Type, Name, Dimension )\
	Array< Type > Name = ( Writer ).GetUniform< Type >( cuT( #Name ), Dimension )

#define CAST( Writer, NewType, OldType )\
	( Writer ).Cast< NewType >( OldType )

#define UBO_MATRIX( Writer )\
	Ubo l_matrices = l_writer.GetUbo( cuT( "Matrices" ) );\
	UNIFORM( l_matrices, Mat4, c3d_mtxProjection );\
	UNIFORM( l_matrices, Mat4, c3d_mtxModel );\
	UNIFORM( l_matrices, Mat4, c3d_mtxView );\
	UNIFORM( l_matrices, Mat4, c3d_mtxModelView );\
	UNIFORM( l_matrices, Mat4, c3d_mtxProjectionView );\
	UNIFORM( l_matrices, Mat4, c3d_mtxProjectionModelView );\
	UNIFORM( l_matrices, Mat4, c3d_mtxNormal );\
	UNIFORM( l_matrices, Mat4, c3d_mtxTexture0 );\
	UNIFORM( l_matrices, Mat4, c3d_mtxTexture1 );\
	UNIFORM( l_matrices, Mat4, c3d_mtxTexture2 );\
	UNIFORM( l_matrices, Mat4, c3d_mtxTexture3 );\
	UNIFORM_ARRAY( l_matrices, Mat4, c3d_mtxBones, 100 );\
	l_matrices.End();

#define UBO_PASS( Writer )\
	Ubo l_pass = l_writer.GetUbo( cuT( "Pass" ) );\
	UNIFORM( l_pass, Vec4, c3d_v4MatAmbient );\
	UNIFORM( l_pass, Vec4, c3d_v4MatDiffuse );\
	UNIFORM( l_pass, Vec4, c3d_v4MatEmissive );\
	UNIFORM( l_pass, Vec4, c3d_v4MatSpecular );\
	UNIFORM( l_pass, Float, c3d_fMatShininess );\
	UNIFORM( l_pass, Float, c3d_fMatOpacity );\
	l_pass.End();

#define UBO_SCENE( Writer )\
	Ubo l_scene = l_writer.GetUbo( cuT( "Scene" ) );\
	UNIFORM( l_scene, Vec4, c3d_v4AmbientLight );\
	UNIFORM( l_scene, Vec4, c3d_v4BackgroundColour );\
	UNIFORM( l_scene, IVec4, c3d_iLightsCount );\
	UNIFORM( l_scene, Vec3, c3d_v3CameraPosition );\
	l_scene.End();

#define UBO_BILLBOARD( Writer )\
	Ubo l_billboard = l_writer.GetUbo( cuT( "Billboard" ) );\
	UNIFORM( l_billboard, IVec2, c3d_v2iDimensions );\
	l_billboard.End();
}

#define TERNARY( Writer, ExprType, Condition, Left, Right )\
	Writer.Ternary< ExprType >( Condition, Left, Right )

#include "GlslWriter.inl"

#endif
