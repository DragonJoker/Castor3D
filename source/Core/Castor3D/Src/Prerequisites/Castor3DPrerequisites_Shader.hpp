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
#ifndef ___C3D_PREREQUISITES_SHADER_H___
#define ___C3D_PREREQUISITES_SHADER_H___

namespace Castor3D
{
	/**@name Shader */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Enumeration of existing shader models
	\~french
	\brief		Enumération des modèles de shader existants
	\remark
	*/
	enum class ShaderModel
		: uint8_t
	{
		//!\~english	Vertex shaders.
		//!\~french		Vertex shaders.
		eModel1,
		//!\~english	Pixel shaders.
		//!\~french		Pixel shaders.
		eModel2,
		//!\~english	Geometry shaders.
		//!\~french		Geometry shaders.
		eModel3,
		//!\~english	Tessellation shaders.
		//!\~french		Tessellation shaders.
		eModel4,
		//!\~english	Compute shaders.
		//!\~french		Compute shaders.
		eModel5,
		CASTOR_SCOPED_ENUM_BOUNDS( eModel1 )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		22/08/2012
	\version	0.7.0.0
	\~english
	\brief		Shader status enueration
	\~french
	\brief		Enumération des statuts de shader
	*/
	enum class ShaderStatus
		: uint8_t
	{
		eDontExist,
		eNotCompiled,
		eError,
		eCompiled,
		CASTOR_SCOPED_ENUM_BOUNDS( eDontExist )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		22/08/2012
	\version	0.7.0.0
	\~english
	\brief		Program status enumeration
	\~french
	\brief		Enumération des statuts de programme
	*/
	enum class ProgramStatus
		: uint8_t
	{
		eNotLinked,
		eError,
		eLinked,
		CASTOR_SCOPED_ENUM_BOUNDS( eNotLinked )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Shader object types enumeration
	\~french
	\brief		Enumération des types de shader object
	*/
	enum class ShaderType
		: int8_t
	{
		eNone = -1,
		eVertex,
		eHull,
		eDomain,
		eGeometry,
		ePixel,
		eCompute,
		CASTOR_SCOPED_ENUM_BOUNDS( eNone )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Shader object types masks
	\~french
	\brief		Masques pour les types de shader object
	*/
	enum class ShaderTypeFlag
		: uint8_t
	{
		eVertex = uint8_t( 0x1 ) << int( ShaderType::eVertex ),
		eHull = uint8_t( 0x1 ) << int( ShaderType::eHull ),
		eDomain = uint8_t( 0x1 ) << int( ShaderType::eDomain ),
		eGeometry = uint8_t( 0x1 ) << int( ShaderType::eGeometry ),
		ePixel = uint8_t( 0x1 ) << int( ShaderType::ePixel ),
		eCompute = uint8_t( 0x1 ) << int( ShaderType::eCompute ),
	};
	IMPLEMENT_FLAGS( ShaderTypeFlag )
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/11/13
	\version	0.7.0.0
	\~english
	\brief		Flags to use when looking for an automatically generated program
	\~french
	\brief		Indicateurs à utiliser lors de la recherche de programme généré automatiquement
	*/
	enum class ProgramFlag
		: uint16_t
	{
		//!\~english	Program using instanciation.
		//!\~french		Programme utilisant l'instanciation.
		eInstantiation = 0x0001,
		//!\~english	Program using skeleton animations.
		//!\~french		Programme utilisant les animations par squelette.
		eSkinning = 0x0002,
		//!\~english	Program used by billboards.
		//!\~french		Programme utilisé par des billboards.
		eBillboards = 0x0004,
		//!\~english	Program using per-vertex animations.
		//!\~french		Programme utilisant les animations par sommet.
		eMorphing = 0x0008,
		//!\~english	Program used in transparent pipeline.
		//!\~french		Programme utilisé dans le pipeline des objets transparents.
		eAlphaBlending = 0x0010,
		//!\~english	Picking pass program.
		//\~french		Programme de passe de picking.
		ePicking = 0x0020,
		//!\~english	Shadow map is available as input.
		//\~french		La map d'ombres est disponible en entrée.
		eShadows = 0x0040,
		//!\~english	Shader used to render a shadow map.
		//\~french		Shader utilisé pour dessiner la shadow map.
		eShadowMap = 0x0080,
		//!\~english	Shader supporting lighting.
		//\~french		Shader supportant les éclairages.
		eLighting = 0x0100,
		//!\~english	Shader for spherical billboards.
		//\~french		Shader pour les billboards sphériques.
		eSpherical = 0x0200,
		//!\~english	Shader for fixed size billboards.
		//\~french		Shader pour les billboards à dimensions fixes.
		eFixedSize = 0x0400,
	};
	IMPLEMENT_FLAGS( ProgramFlag )
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Supported variables dimensions enumeration
	\~french
	\brief		Enumération des dimensions de variable supportées
	*/
	enum class VariableType
		: uint8_t
	{
		//!\~english	One variable.
		//!\~french		Une variable.
		eOne,
		//!\~english	1 variable array.
		//!\~french		Tableau de 1 variable.
		eVec1,
		//!\~english	2 variables array.
		//!\~french		Tableau de 2 variables.
		eVec2,
		//!\~english	3 variables array.
		//!\~french		Tableau de 3 variables.
		eVec3,
		//!\~english	4 variables array.
		//!\~french		Tableau de 4 variables.
		eVec4,
		//!\~english	1x1 matrix.
		//!\~french		Matrice 1x1.
		eMat1,
		//!\~english	2x2 matrix.
		//!\~french		Matrice 2x2.
		eMat2x2,
		//!\~english	2x3 matrix.
		//!\~french		Matrice 2x3.
		eMat2x3,
		//!\~english	2x4 matrix.
		//!\~french		Matrice 2x4.
		eMat2x4,
		//!\~english	3x2 matrix.
		//!\~french		Matrice 3x2.
		eMat3x2,
		//!\~english	3x3 matrix.
		//!\~french		Matrice 3x3.
		eMat3x3,
		//!\~english	3x4 matrix.
		//!\~french		Matrice 3x4.
		eMat3x4,
		//!\~english	4x2 matrix.
		//!\~french		Matrice 4x2.
		eMat4x2,
		//!\~english	4x3 matrix.
		//!\~french		Matrice 4x3
		eMat4x3,
		//!\~english	4x4 matrix.
		//!\~french		Matrice 4x4.
		eMat4x4,
		CASTOR_SCOPED_ENUM_BOUNDS( eOne )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Supported variables full type enumeration
	\~french
	\brief		Enumération des types complets de variable supportées
	*/
	enum class UniformType
		: uint8_t
	{
		eBool,
		eInt,
		eUInt,
		eFloat,
		eDouble,
		eSampler,
		eVec2b,
		eVec3b,
		eVec4b,
		eVec2i,
		eVec3i,
		eVec4i,
		eVec2ui,
		eVec3ui,
		eVec4ui,
		eVec2f,
		eVec3f,
		eVec4f,
		eVec2d,
		eVec3d,
		eVec4d,
		eMat2x2b,
		eMat2x3b,
		eMat2x4b,
		eMat3x2b,
		eMat3x3b,
		eMat3x4b,
		eMat4x2b,
		eMat4x3b,
		eMat4x4b,
		eMat2x2i,
		eMat2x3i,
		eMat2x4i,
		eMat3x2i,
		eMat3x3i,
		eMat3x4i,
		eMat4x2i,
		eMat4x3i,
		eMat4x4i,
		eMat2x2ui,
		eMat2x3ui,
		eMat2x4ui,
		eMat3x2ui,
		eMat3x3ui,
		eMat3x4ui,
		eMat4x2ui,
		eMat4x3ui,
		eMat4x4ui,
		eMat2x2f,
		eMat2x3f,
		eMat2x4f,
		eMat3x2f,
		eMat3x3f,
		eMat3x4f,
		eMat4x2f,
		eMat4x3f,
		eMat4x4f,
		eMat2x2d,
		eMat2x3d,
		eMat2x4d,
		eMat3x2d,
		eMat3x3d,
		eMat3x4d,
		eMat4x2d,
		eMat4x3d,
		eMat4x4d,
#if CASTOR_USE_DOUBLE
		eReal = eDouble,
		eVec2r = eVec2d,
		eVec3r = eVec3d,
		eVec4r = eVec4d,
		eMat2x2r = eMat2x2d,
		eMat2x3r = eMat2x3d,
		eMat2x4r = eMat2x4d,
		eMat3x2r = eMat3x2d,
		eMat3x3r = eMat3x3d,
		eMat3x4r = eMat3x4d,
		eMat4x2r = eMat4x2d,
		eMat4x3r = eMat4x3d,
		eMat4x4r = eMat4x4d,
#else
		eReal = eFloat,
		eVec2r = eVec2f,
		eVec3r = eVec3f,
		eVec4r = eVec4f,
		eMat2x2r = eMat2x2f,
		eMat2x3r = eMat2x3f,
		eMat2x4r = eMat2x4f,
		eMat3x2r = eMat3x2f,
		eMat3x3r = eMat3x3f,
		eMat3x4r = eMat3x4f,
		eMat4x2r = eMat4x2f,
		eMat4x3r = eMat4x3f,
		eMat4x4r = eMat4x4f,
#endif
		CASTOR_SCOPED_ENUM_BOUNDS( eBool )
	};

	class ShaderProgram;
	class ShaderObject;
	class ProgramInputLayout;
	class Uniform;
	class UniformBuffer;
	class ShaderStorageBuffer;
	class AtomicCounterBuffer;
	template < typename T > class OneUniform;
	template < typename T, uint32_t Count > class PointUniform;
	template < typename T, uint32_t Rows, uint32_t Columns > class MatrixUniform;

	using Uniform1b = OneUniform< bool >;
	using Uniform1i = OneUniform< int >;
	using Uniform1ui = OneUniform< uint32_t >;
	using Uniform1f = OneUniform< float >;
	using Uniform1d = OneUniform< double >;
	using Uniform1r = OneUniform< real >;
	using Uniform2b = PointUniform< bool, 2 >;
	using Uniform3b = PointUniform< bool, 3 >;
	using Uniform4b = PointUniform< bool, 4 >;
	using Uniform2i = PointUniform< int, 2 >;
	using Uniform3i = PointUniform< int, 3 >;
	using Uniform4i = PointUniform< int, 4 >;
	using Uniform2ui = PointUniform< uint32_t, 2 >;
	using Uniform3ui = PointUniform< uint32_t, 3 >;
	using Uniform4ui = PointUniform< uint32_t, 4 >;
	using Uniform2f = PointUniform< float, 2 >;
	using Uniform3f = PointUniform< float, 3 >;
	using Uniform4f = PointUniform< float, 4 >;
	using Uniform2d = PointUniform< double, 2 >;
	using Uniform3d = PointUniform< double, 3 >;
	using Uniform4d = PointUniform< double, 4 >;
	using Uniform2r = PointUniform< real, 2 >;
	using Uniform3r = PointUniform< real, 3 >;
	using Uniform4r = PointUniform< real, 4 >;
	using Uniform2x2b = MatrixUniform< bool, 2, 2 >;
	using Uniform2x3b = MatrixUniform< bool, 2, 3 >;
	using Uniform2x4b = MatrixUniform< bool, 2, 4 >;
	using Uniform3x2b = MatrixUniform< bool, 3, 2 >;
	using Uniform3x3b = MatrixUniform< bool, 3, 3 >;
	using Uniform3x4b = MatrixUniform< bool, 3, 4 >;
	using Uniform4x2b = MatrixUniform< bool, 4, 2 >;
	using Uniform4x3b = MatrixUniform< bool, 4, 3 >;
	using Uniform4x4b = MatrixUniform< bool, 4, 4 >;
	using Uniform2x2i = MatrixUniform< int, 2, 2 >;
	using Uniform2x3i = MatrixUniform< int, 2, 3 >;
	using Uniform2x4i = MatrixUniform< int, 2, 4 >;
	using Uniform3x2i = MatrixUniform< int, 3, 2 >;
	using Uniform3x3i = MatrixUniform< int, 3, 3 >;
	using Uniform3x4i = MatrixUniform< int, 3, 4 >;
	using Uniform4x2i = MatrixUniform< int, 4, 2 >;
	using Uniform4x3i = MatrixUniform< int, 4, 3 >;
	using Uniform4x4i = MatrixUniform< int, 4, 4 >;
	using Uniform2x2ui = MatrixUniform< unsigned  int, 2, 2 >;
	using Uniform2x3ui = MatrixUniform< unsigned  int, 2, 3 >;
	using Uniform2x4ui = MatrixUniform< unsigned  int, 2, 4 >;
	using Uniform3x2ui = MatrixUniform< unsigned  int, 3, 2 >;
	using Uniform3x3ui = MatrixUniform< unsigned  int, 3, 3 >;
	using Uniform3x4ui = MatrixUniform< unsigned  int, 3, 4 >;
	using Uniform4x2ui = MatrixUniform< unsigned  int, 4, 2 >;
	using Uniform4x3ui = MatrixUniform< unsigned  int, 4, 3 >;
	using Uniform4x4ui = MatrixUniform< unsigned  int, 4, 4 >;
	using Uniform2x2f = MatrixUniform< float, 2, 2 >;
	using Uniform2x3f = MatrixUniform< float, 2, 3 >;
	using Uniform2x4f = MatrixUniform< float, 2, 4 >;
	using Uniform3x2f = MatrixUniform< float, 3, 2 >;
	using Uniform3x3f = MatrixUniform< float, 3, 3 >;
	using Uniform3x4f = MatrixUniform< float, 3, 4 >;
	using Uniform4x2f = MatrixUniform< float, 4, 2 >;
	using Uniform4x3f = MatrixUniform< float, 4, 3 >;
	using Uniform4x4f = MatrixUniform< float, 4, 4 >;
	using Uniform2x2d = MatrixUniform< double, 2, 2 >;
	using Uniform2x3d = MatrixUniform< double, 2, 3 >;
	using Uniform2x4d = MatrixUniform< double, 2, 4 >;
	using Uniform3x2d = MatrixUniform< double, 3, 2 >;
	using Uniform3x3d = MatrixUniform< double, 3, 3 >;
	using Uniform3x4d = MatrixUniform< double, 3, 4 >;
	using Uniform4x2d = MatrixUniform< double, 4, 2 >;
	using Uniform4x3d = MatrixUniform< double, 4, 3 >;
	using Uniform4x4d = MatrixUniform< double, 4, 4 >;
	using Uniform2x2r = MatrixUniform< real, 2, 2 >;
	using Uniform2x3r = MatrixUniform< real, 2, 3 >;
	using Uniform2x4r = MatrixUniform< real, 2, 4 >;
	using Uniform3x2r = MatrixUniform< real, 3, 2 >;
	using Uniform3x3r = MatrixUniform< real, 3, 3 >;
	using Uniform3x4r = MatrixUniform< real, 3, 4 >;
	using Uniform4x2r = MatrixUniform< real, 4, 2 >;
	using Uniform4x3r = MatrixUniform< real, 4, 3 >;
	using Uniform4x4r = MatrixUniform< real, 4, 4 >;

	DECLARE_SMART_PTR( Uniform1b );
	DECLARE_SMART_PTR( Uniform1i );
	DECLARE_SMART_PTR( Uniform1ui );
	DECLARE_SMART_PTR( Uniform1f );
	DECLARE_SMART_PTR( Uniform1d );
	DECLARE_SMART_PTR( Uniform1r );
	DECLARE_SMART_PTR( Uniform2b );
	DECLARE_SMART_PTR( Uniform3b );
	DECLARE_SMART_PTR( Uniform4b );
	DECLARE_SMART_PTR( Uniform2i );
	DECLARE_SMART_PTR( Uniform3i );
	DECLARE_SMART_PTR( Uniform4i );
	DECLARE_SMART_PTR( Uniform2ui );
	DECLARE_SMART_PTR( Uniform3ui );
	DECLARE_SMART_PTR( Uniform4ui );
	DECLARE_SMART_PTR( Uniform2f );
	DECLARE_SMART_PTR( Uniform3f );
	DECLARE_SMART_PTR( Uniform4f );
	DECLARE_SMART_PTR( Uniform2d );
	DECLARE_SMART_PTR( Uniform3d );
	DECLARE_SMART_PTR( Uniform4d );
	DECLARE_SMART_PTR( Uniform2r );
	DECLARE_SMART_PTR( Uniform3r );
	DECLARE_SMART_PTR( Uniform4r );
	DECLARE_SMART_PTR( Uniform2x2b );
	DECLARE_SMART_PTR( Uniform2x3b );
	DECLARE_SMART_PTR( Uniform2x4b );
	DECLARE_SMART_PTR( Uniform3x2b );
	DECLARE_SMART_PTR( Uniform3x3b );
	DECLARE_SMART_PTR( Uniform3x4b );
	DECLARE_SMART_PTR( Uniform4x2b );
	DECLARE_SMART_PTR( Uniform4x3b );
	DECLARE_SMART_PTR( Uniform4x4b );
	DECLARE_SMART_PTR( Uniform2x2i );
	DECLARE_SMART_PTR( Uniform2x3i );
	DECLARE_SMART_PTR( Uniform2x4i );
	DECLARE_SMART_PTR( Uniform3x2i );
	DECLARE_SMART_PTR( Uniform3x3i );
	DECLARE_SMART_PTR( Uniform3x4i );
	DECLARE_SMART_PTR( Uniform4x2i );
	DECLARE_SMART_PTR( Uniform4x3i );
	DECLARE_SMART_PTR( Uniform4x4i );
	DECLARE_SMART_PTR( Uniform2x2ui );
	DECLARE_SMART_PTR( Uniform2x3ui );
	DECLARE_SMART_PTR( Uniform2x4ui );
	DECLARE_SMART_PTR( Uniform3x2ui );
	DECLARE_SMART_PTR( Uniform3x3ui );
	DECLARE_SMART_PTR( Uniform3x4ui );
	DECLARE_SMART_PTR( Uniform4x2ui );
	DECLARE_SMART_PTR( Uniform4x3ui );
	DECLARE_SMART_PTR( Uniform4x4ui );
	DECLARE_SMART_PTR( Uniform2x2f );
	DECLARE_SMART_PTR( Uniform2x3f );
	DECLARE_SMART_PTR( Uniform2x4f );
	DECLARE_SMART_PTR( Uniform3x2f );
	DECLARE_SMART_PTR( Uniform3x3f );
	DECLARE_SMART_PTR( Uniform3x4f );
	DECLARE_SMART_PTR( Uniform4x2f );
	DECLARE_SMART_PTR( Uniform4x3f );
	DECLARE_SMART_PTR( Uniform4x4f );
	DECLARE_SMART_PTR( Uniform2x2d );
	DECLARE_SMART_PTR( Uniform2x3d );
	DECLARE_SMART_PTR( Uniform2x4d );
	DECLARE_SMART_PTR( Uniform3x2d );
	DECLARE_SMART_PTR( Uniform3x3d );
	DECLARE_SMART_PTR( Uniform3x4d );
	DECLARE_SMART_PTR( Uniform4x2d );
	DECLARE_SMART_PTR( Uniform4x3d );
	DECLARE_SMART_PTR( Uniform4x4d );
	DECLARE_SMART_PTR( Uniform2x2r );
	DECLARE_SMART_PTR( Uniform2x3r );
	DECLARE_SMART_PTR( Uniform2x4r );
	DECLARE_SMART_PTR( Uniform3x2r );
	DECLARE_SMART_PTR( Uniform3x3r );
	DECLARE_SMART_PTR( Uniform3x4r );
	DECLARE_SMART_PTR( Uniform4x2r );
	DECLARE_SMART_PTR( Uniform4x3r );
	DECLARE_SMART_PTR( Uniform4x4r );

	DECLARE_SMART_PTR( UniformBuffer );
	DECLARE_SMART_PTR( Uniform );
	DECLARE_SMART_PTR( ShaderObject );
	DECLARE_SMART_PTR( ShaderProgram );
	DECLARE_SMART_PTR( ShaderStorageBuffer );
	DECLARE_SMART_PTR( AtomicCounterBuffer );

	DECLARE_VECTOR( ShaderProgramSPtr, ShaderProgramPtr );
	DECLARE_VECTOR( ShaderObjectSPtr, ShaderObjectPtr );
	DECLARE_LIST( UniformSPtr, UniformPtr );
	DECLARE_LIST( UniformBufferSPtr, UniformBufferPtr );
	DECLARE_LIST( ShaderStorageBufferSPtr, ShaderStorageBufferPtr );
	DECLARE_LIST( AtomicCounterBufferSPtr, AtomicCounterBufferPtr );
	DECLARE_MAP( Castor::String, UniformWPtr, UniformPtrStr );
	DECLARE_MAP( Castor::String, UniformBufferWPtr, UniformBufferPtrStr );
	DECLARE_MAP( Castor::String, ShaderStorageBufferWPtr, ShaderStorageBufferPtrStr );
	DECLARE_MAP( Castor::String, AtomicCounterBufferWPtr, AtomicCounterBufferPtrStr );
	DECLARE_MAP( ShaderType, UniformBufferWPtr, UniformBufferPtrShader );
	DECLARE_MAP( ShaderType, ShaderStorageBufferWPtr, ShaderStorageBufferPtrShader );
	DECLARE_MAP( ShaderType, AtomicCounterBufferWPtr, AtomicCounterBufferPtrShader );

	//@}

#define UBO_MATRIX( Writer )\
	GLSL::Ubo l_matrices{ l_writer, ShaderProgram::BufferMatrix };\
	auto c3d_mtxProjection = l_matrices.GetUniform< GLSL::Mat4 >( RenderPipeline::MtxProjection );\
	auto c3d_mtxModel = l_matrices.GetUniform< GLSL::Mat4 >( RenderPipeline::MtxModel );\
	auto c3d_mtxView = l_matrices.GetUniform< GLSL::Mat4 >( RenderPipeline::MtxView );\
	auto c3d_mtxNormal = l_matrices.GetUniform< GLSL::Mat4 >( RenderPipeline::MtxNormal );\
	auto c3d_mtxTexture0 = l_matrices.GetUniform< GLSL::Mat4 >( RenderPipeline::MtxTexture[0] );\
	auto c3d_mtxTexture1 = l_matrices.GetUniform< GLSL::Mat4 >( RenderPipeline::MtxTexture[1] );\
	auto c3d_mtxTexture2 = l_matrices.GetUniform< GLSL::Mat4 >( RenderPipeline::MtxTexture[2] );\
	auto c3d_mtxTexture3 = l_matrices.GetUniform< GLSL::Mat4 >( RenderPipeline::MtxTexture[3] );\
	l_matrices.End()

#define UBO_SCENE( Writer )\
	GLSL::Ubo l_scene{ l_writer, ShaderProgram::BufferScene };\
	auto c3d_v4AmbientLight = l_scene.GetUniform< GLSL::Vec4 >( ShaderProgram::AmbientLight );\
	auto c3d_v4BackgroundColour = l_scene.GetUniform< GLSL::Vec4 >( ShaderProgram::BackgroundColour );\
	auto c3d_iLightsCount = l_scene.GetUniform< GLSL::IVec4 >( ShaderProgram::LightsCount );\
	auto c3d_v3CameraPosition = l_scene.GetUniform< GLSL::Vec3 >( ShaderProgram::CameraPos );\
	auto c3d_iFogType = l_scene.GetUniform< GLSL::Int >( ShaderProgram::FogType );\
	auto c3d_fFogDensity = l_scene.GetUniform< GLSL::Float >( ShaderProgram::FogDensity );\
	l_scene.End()

#define UBO_PASS( Writer )\
	GLSL::Ubo l_pass{ l_writer, ShaderProgram::BufferPass };\
	auto c3d_v4MatAmbient = l_pass.GetUniform< GLSL::Vec4 >( ShaderProgram::MatAmbient );\
	auto c3d_v4MatDiffuse = l_pass.GetUniform< GLSL::Vec4 >( ShaderProgram::MatDiffuse );\
	auto c3d_v4MatEmissive = l_pass.GetUniform< GLSL::Vec4 >( ShaderProgram::MatEmissive );\
	auto c3d_v4MatSpecular = l_pass.GetUniform< GLSL::Vec4 >( ShaderProgram::MatSpecular );\
	auto c3d_fMatShininess = l_pass.GetUniform< GLSL::Float >( ShaderProgram::MatShininess );\
	auto c3d_fMatOpacity = l_pass.GetUniform< GLSL::Float >( ShaderProgram::MatOpacity );\
	l_pass.End()

#define UBO_MODEL( Writer )\
	GLSL::Ubo l_model{ l_writer, ShaderProgram::BufferModel };\
	auto c3d_iShadowReceiver = l_model.GetUniform< GLSL::Int >( ShaderProgram::ShadowReceiver );\
	l_model.End()

#define UBO_BILLBOARD( Writer )\
	GLSL::Ubo l_billboard{ l_writer, ShaderProgram::BufferBillboards };\
	auto c3d_v2iDimensions = l_billboard.GetUniform< IVec2 >( ShaderProgram::Dimensions );\
	auto c3d_v2iWindowSize = l_billboard.GetUniform< IVec2 >( ShaderProgram::WindowSize );\
	l_billboard.End()

#define UBO_ANIMATION( Writer, Flags )\
	GLSL::Ubo l_animation{ l_writer, ShaderProgram::BufferAnimation };\
	auto c3d_mtxBones = l_animation.GetUniform< GLSL::Mat4 >( ShaderProgram::Bones, 400, CheckFlag( Flags, ProgramFlag::eSkinning ) );\
	auto c3d_fTime = l_animation.GetUniform< GLSL::Float >( ShaderProgram::Time, CheckFlag( Flags, ProgramFlag::eMorphing ) );\
	l_animation.End()

#define STRUCT_VTX_OUTPUT( Writer )\
	GLSL::Struct VtxOutput{ l_writer, cuT( "VtxOutput" ) };\
	auto vtx_worldSpacePosition = VtxOutput.GetMember< GLSL::Vec3 >( cuT( "vtx_worldSpacePosition" ) );\
	auto vtx_normal = VtxOutput.GetMember< GLSL::Vec3 >( cuT( "vtx_normal" ) );\
	auto vtx_tangent = VtxOutput.GetMember< GLSL::Vec3 >( cuT( "vtx_tangent" ) );\
	auto vtx_bitangent = VtxOutput.GetMember< GLSL::Vec3 >( cuT( "vtx_bitangent" ) );\
	auto vtx_texture = VtxOutput.GetMember< GLSL::Vec3 >( cuT( "vtx_texture" ) );\
	auto vtx_instance = VtxOutput.GetMember< GLSL::Int >( cuT( "vtx_instance" ) );\
	VtxOutput.End()
}

#endif
