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
	typedef enum eSHADER_MODEL
		: uint8_t
	{
		//!\~english Vertex shaders \~french Vertex shaders
		eSHADER_MODEL_1,
		//!\~english Pixel shaders \~french Pixel shaders
		eSHADER_MODEL_2,
		//!\~english Geometry shaders \~french Geometry shaders
		eSHADER_MODEL_3,
		//!\~english Tessellation shaders \~french Tessellation shaders
		eSHADER_MODEL_4,
		//!\~english Compute shaders \~french Compute shaders
		eSHADER_MODEL_5,
		CASTOR_ENUM_BOUNDS( eSHADER_MODEL, eSHADER_MODEL_1 )
	}	eSHADER_MODEL;
	/*!
	\author 	Sylvain DOREMUS
	\date 		22/08/2012
	\version	0.7.0.0
	\~english
	\brief		Shader status enueration
	\~french
	\brief		Enumération des statuts de shader
	*/
	typedef enum eSHADER_STATUS
		: uint8_t
	{
		eSHADER_STATUS_DONTEXIST,
		eSHADER_STATUS_NOTCOMPILED,
		eSHADER_STATUS_ERROR,
		eSHADER_STATUS_COMPILED,
		CASTOR_ENUM_BOUNDS( eSHADER_STATUS, eSHADER_STATUS_DONTEXIST )
	}	eSHADER_STATUS;
	/*!
	\author 	Sylvain DOREMUS
	\date 		22/08/2012
	\version	0.7.0.0
	\~english
	\brief		Program status enumeration
	\~french
	\brief		Enumération des statuts de programme
	*/
	typedef enum ePROGRAM_STATUS
		: uint8_t
	{
		ePROGRAM_STATUS_NOTLINKED,
		ePROGRAM_STATUS_ERROR,
		ePROGRAM_STATUS_LINKED,
		CASTOR_ENUM_BOUNDS( ePROGRAM_STATUS, ePROGRAM_STATUS_NOTLINKED )
	}	ePROGRAM_STATUS;
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
		None = -1,
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Compute,
		CASTOR_ENUM_CLASS_BOUNDS( None )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Shader object types masks
	\~french
	\brief		Masques pour les types de shader object
	*/
	static const uint64_t MASK_SHADER_TYPE_VERTEX = uint64_t( 0x1 ) << int( ShaderType::Vertex );
	static const uint64_t MASK_SHADER_TYPE_HULL = uint64_t( 0x1 ) << int( ShaderType::Hull );
	static const uint64_t MASK_SHADER_TYPE_DOMAIN = uint64_t( 0x1 ) << int( ShaderType::Domain );
	static const uint64_t MASK_SHADER_TYPE_GEOMETRY = uint64_t( 0x1 ) << int( ShaderType::Geometry );
	static const uint64_t MASK_SHADER_TYPE_PIXEL = uint64_t( 0x1 ) << int( ShaderType::Pixel );
	static const uint64_t MASK_SHADER_TYPE_COMPUTE = uint64_t( 0x1 ) << int( ShaderType::Compute );
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
		: uint8_t
	{
		//!\~english	Program using instanciation.
		//!\~french		Programme utilisant l'instanciation.
		Instantiation = 0x01,
		//!\~english	Program using skeleton animations.
		//!\~french		Programme utilisant les animations par squelette.
		Skinning = 0x02,
		//!\~english	Program used by billboards.
		//!\~french		Programme utilisé par des billboards.
		Billboards = 0x04,
		//!\~english	Program using per-vertex animations.
		//!\~french		Programme utilisant les animations par sommet.
		Morphing = 0x08,
		//!\~english	Program used in transparent pipeline.
		//!\~french		Programme utilisé dans le pipeline des objets transparents.
		AlphaBlending = 0x10,
		//!\~english	Picking pass program.
		//\~french		Programme de passe de picking.
		Picking = 0x20,
	};
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
		One,
		//!\~english	1 variable array.
		//!\~french		Tableau de 1 variable.
		Vec1,
		//!\~english	2 variables array.
		//!\~french		Tableau de 2 variables.
		Vec2,
		//!\~english	3 variables array.
		//!\~french		Tableau de 3 variables.
		Vec3,
		//!\~english	4 variables array.
		//!\~french		Tableau de 4 variables.
		Vec4,
		//!\~english	1x1 matrix.
		//!\~french		Matrice 1x1.
		Mat1,
		//!\~english	2x2 matrix.
		//!\~french		Matrice 2x2.
		Mat2x2,
		//!\~english	2x3 matrix.
		//!\~french		Matrice 2x3.
		Mat2x3,
		//!\~english	2x4 matrix.
		//!\~french		Matrice 2x4.
		Mat2x4,
		//!\~english	3x2 matrix.
		//!\~french		Matrice 3x2.
		Mat3x2,
		//!\~english	3x3 matrix.
		//!\~french		Matrice 3x3.
		Mat3x3,
		//!\~english	3x4 matrix.
		//!\~french		Matrice 3x4.
		Mat3x4,
		//!\~english	4x2 matrix.
		//!\~french		Matrice 4x2.
		Mat4x2,
		//!\~english	4x3 matrix.
		//!\~french		Matrice 4x3
		Mat4x3,
		//!\~english	4x4 matrix.
		//!\~french		Matrice 4x4.
		Mat4x4,
		CASTOR_ENUM_CLASS_BOUNDS( One )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Supported variables full type enumeration
	\~french
	\brief		Enumération des types complets de variable supportées
	*/
	enum class FrameVariableType
		: uint8_t
	{
		Bool,
		Int,
		UInt,
		Float,
		Double,
		Sampler,
		Vec2b,
		Vec3b,
		Vec4b,
		Vec2i,
		Vec3i,
		Vec4i,
		Vec2ui,
		Vec3ui,
		Vec4ui,
		Vec2f,
		Vec3f,
		Vec4f,
		Vec2d,
		Vec3d,
		Vec4d,
		Mat2x2b,
		Mat2x3b,
		Mat2x4b,
		Mat3x2b,
		Mat3x3b,
		Mat3x4b,
		Mat4x2b,
		Mat4x3b,
		Mat4x4b,
		Mat2x2i,
		Mat2x3i,
		Mat2x4i,
		Mat3x2i,
		Mat3x3i,
		Mat3x4i,
		Mat4x2i,
		Mat4x3i,
		Mat4x4i,
		Mat2x2ui,
		Mat2x3ui,
		Mat2x4ui,
		Mat3x2ui,
		Mat3x3ui,
		Mat3x4ui,
		Mat4x2ui,
		Mat4x3ui,
		Mat4x4ui,
		Mat2x2f,
		Mat2x3f,
		Mat2x4f,
		Mat3x2f,
		Mat3x3f,
		Mat3x4f,
		Mat4x2f,
		Mat4x3f,
		Mat4x4f,
		Mat2x2d,
		Mat2x3d,
		Mat2x4d,
		Mat3x2d,
		Mat3x3d,
		Mat3x4d,
		Mat4x2d,
		Mat4x3d,
		Mat4x4d,
#if CASTOR_USE_DOUBLE
		Real = Double,
		Vec2r = Vec2d,
		Vec3r = Vec3d,
		Vec4r = Vec4d,
		Mat2x2r = Mat2x2d,
		Mat2x3r = Mat2x3d,
		Mat2x4r = Mat2x4d,
		Mat3x2r = Mat3x2d,
		Mat3x3r = Mat3x3d,
		Mat3x4r = Mat3x4d,
		Mat4x2r = Mat4x2d,
		Mat4x3r = Mat4x3d,
		Mat4x4r = Mat4x4d,
#else
		Real = Float,
		Vec2r = Vec2f,
		Vec3r = Vec3f,
		Vec4r = Vec4f,
		Mat2x2r = Mat2x2f,
		Mat2x3r = Mat2x3f,
		Mat2x4r = Mat2x4f,
		Mat3x2r = Mat3x2f,
		Mat3x3r = Mat3x3f,
		Mat3x4r = Mat3x4f,
		Mat4x2r = Mat4x2f,
		Mat4x3r = Mat4x3f,
		Mat4x4r = Mat4x4f,
#endif
		CASTOR_ENUM_CLASS_BOUNDS( Bool )
	};

	class ShaderProgram;
	class ShaderObject;
	class ProgramInputLayout;
	class FrameVariable;
	class FrameVariableBuffer;
	class FrameVariableLink;
	class ProgramLinks;
	template < typename T > class OneFrameVariable;
	template < typename T, uint32_t Count > class PointFrameVariable;
	template < typename T, uint32_t Rows, uint32_t Columns > class MatrixFrameVariable;

	typedef OneFrameVariable< bool > OneBoolFrameVariable;
	typedef OneFrameVariable< int > OneIntFrameVariable;
	typedef OneFrameVariable< uint32_t > OneUIntFrameVariable;
	typedef OneFrameVariable< float > OneFloatFrameVariable;
	typedef OneFrameVariable< double > OneDoubleFrameVariable;
	typedef OneFrameVariable< real > OneRealFrameVariable;
	typedef PointFrameVariable< bool, 2 > Point2bFrameVariable;
	typedef PointFrameVariable< bool, 3 > Point3bFrameVariable;
	typedef PointFrameVariable< bool, 4 > Point4bFrameVariable;
	typedef PointFrameVariable< int, 2 > Point2iFrameVariable;
	typedef PointFrameVariable< int, 3 > Point3iFrameVariable;
	typedef PointFrameVariable< int, 4 > Point4iFrameVariable;
	typedef PointFrameVariable< uint32_t, 2 > Point2uiFrameVariable;
	typedef PointFrameVariable< uint32_t, 3 > Point3uiFrameVariable;
	typedef PointFrameVariable< uint32_t, 4 > Point4uiFrameVariable;
	typedef PointFrameVariable< float, 2 > Point2fFrameVariable;
	typedef PointFrameVariable< float, 3 > Point3fFrameVariable;
	typedef PointFrameVariable< float, 4 > Point4fFrameVariable;
	typedef PointFrameVariable< double, 2 > Point2dFrameVariable;
	typedef PointFrameVariable< double, 3 > Point3dFrameVariable;
	typedef PointFrameVariable< double, 4 > Point4dFrameVariable;
	typedef PointFrameVariable< real, 2 > Point2rFrameVariable;
	typedef PointFrameVariable< real, 3 > Point3rFrameVariable;
	typedef PointFrameVariable< real, 4 > Point4rFrameVariable;
	typedef MatrixFrameVariable< bool, 2, 2 > Matrix2x2bFrameVariable;
	typedef MatrixFrameVariable< bool, 2, 3 > Matrix2x3bFrameVariable;
	typedef MatrixFrameVariable< bool, 2, 4 > Matrix2x4bFrameVariable;
	typedef MatrixFrameVariable< bool, 3, 2 > Matrix3x2bFrameVariable;
	typedef MatrixFrameVariable< bool, 3, 3 > Matrix3x3bFrameVariable;
	typedef MatrixFrameVariable< bool, 3, 4 > Matrix3x4bFrameVariable;
	typedef MatrixFrameVariable< bool, 4, 2 > Matrix4x2bFrameVariable;
	typedef MatrixFrameVariable< bool, 4, 3 > Matrix4x3bFrameVariable;
	typedef MatrixFrameVariable< bool, 4, 4 > Matrix4x4bFrameVariable;
	typedef MatrixFrameVariable< int, 2, 2 > Matrix2x2iFrameVariable;
	typedef MatrixFrameVariable< int, 2, 3 > Matrix2x3iFrameVariable;
	typedef MatrixFrameVariable< int, 2, 4 > Matrix2x4iFrameVariable;
	typedef MatrixFrameVariable< int, 3, 2 > Matrix3x2iFrameVariable;
	typedef MatrixFrameVariable< int, 3, 3 > Matrix3x3iFrameVariable;
	typedef MatrixFrameVariable< int, 3, 4 > Matrix3x4iFrameVariable;
	typedef MatrixFrameVariable< int, 4, 2 > Matrix4x2iFrameVariable;
	typedef MatrixFrameVariable< int, 4, 3 > Matrix4x3iFrameVariable;
	typedef MatrixFrameVariable< int, 4, 4 > Matrix4x4iFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 2, 2 > Matrix2x2uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 2, 3 > Matrix2x3uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 2, 4 > Matrix2x4uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 3, 2 > Matrix3x2uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 3, 3 > Matrix3x3uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 3, 4 > Matrix3x4uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 4, 2 > Matrix4x2uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 4, 3 > Matrix4x3uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 4, 4 > Matrix4x4uiFrameVariable;
	typedef MatrixFrameVariable< float, 2, 2 > Matrix2x2fFrameVariable;
	typedef MatrixFrameVariable< float, 2, 3 > Matrix2x3fFrameVariable;
	typedef MatrixFrameVariable< float, 2, 4 > Matrix2x4fFrameVariable;
	typedef MatrixFrameVariable< float, 3, 2 > Matrix3x2fFrameVariable;
	typedef MatrixFrameVariable< float, 3, 3 > Matrix3x3fFrameVariable;
	typedef MatrixFrameVariable< float, 3, 4 > Matrix3x4fFrameVariable;
	typedef MatrixFrameVariable< float, 4, 2 > Matrix4x2fFrameVariable;
	typedef MatrixFrameVariable< float, 4, 3 > Matrix4x3fFrameVariable;
	typedef MatrixFrameVariable< float, 4, 4 > Matrix4x4fFrameVariable;
	typedef MatrixFrameVariable< double, 2, 2 > Matrix2x2dFrameVariable;
	typedef MatrixFrameVariable< double, 2, 3 > Matrix2x3dFrameVariable;
	typedef MatrixFrameVariable< double, 2, 4 > Matrix2x4dFrameVariable;
	typedef MatrixFrameVariable< double, 3, 2 > Matrix3x2dFrameVariable;
	typedef MatrixFrameVariable< double, 3, 3 > Matrix3x3dFrameVariable;
	typedef MatrixFrameVariable< double, 3, 4 > Matrix3x4dFrameVariable;
	typedef MatrixFrameVariable< double, 4, 2 > Matrix4x2dFrameVariable;
	typedef MatrixFrameVariable< double, 4, 3 > Matrix4x3dFrameVariable;
	typedef MatrixFrameVariable< double, 4, 4 > Matrix4x4dFrameVariable;
	typedef MatrixFrameVariable< real, 2, 2 > Matrix2x2rFrameVariable;
	typedef MatrixFrameVariable< real, 2, 3 > Matrix2x3rFrameVariable;
	typedef MatrixFrameVariable< real, 2, 4 > Matrix2x4rFrameVariable;
	typedef MatrixFrameVariable< real, 3, 2 > Matrix3x2rFrameVariable;
	typedef MatrixFrameVariable< real, 3, 3 > Matrix3x3rFrameVariable;
	typedef MatrixFrameVariable< real, 3, 4 > Matrix3x4rFrameVariable;
	typedef MatrixFrameVariable< real, 4, 2 > Matrix4x2rFrameVariable;
	typedef MatrixFrameVariable< real, 4, 3 > Matrix4x3rFrameVariable;
	typedef MatrixFrameVariable< real, 4, 4 > Matrix4x4rFrameVariable;

	DECLARE_SMART_PTR( OneBoolFrameVariable );
	DECLARE_SMART_PTR( OneIntFrameVariable );
	DECLARE_SMART_PTR( OneUIntFrameVariable );
	DECLARE_SMART_PTR( OneFloatFrameVariable );
	DECLARE_SMART_PTR( OneDoubleFrameVariable );
	DECLARE_SMART_PTR( OneRealFrameVariable );
	DECLARE_SMART_PTR( Point2bFrameVariable );
	DECLARE_SMART_PTR( Point3bFrameVariable );
	DECLARE_SMART_PTR( Point4bFrameVariable );
	DECLARE_SMART_PTR( Point2iFrameVariable );
	DECLARE_SMART_PTR( Point3iFrameVariable );
	DECLARE_SMART_PTR( Point4iFrameVariable );
	DECLARE_SMART_PTR( Point2uiFrameVariable );
	DECLARE_SMART_PTR( Point3uiFrameVariable );
	DECLARE_SMART_PTR( Point4uiFrameVariable );
	DECLARE_SMART_PTR( Point2fFrameVariable );
	DECLARE_SMART_PTR( Point3fFrameVariable );
	DECLARE_SMART_PTR( Point4fFrameVariable );
	DECLARE_SMART_PTR( Point2dFrameVariable );
	DECLARE_SMART_PTR( Point3dFrameVariable );
	DECLARE_SMART_PTR( Point4dFrameVariable );
	DECLARE_SMART_PTR( Point2rFrameVariable );
	DECLARE_SMART_PTR( Point3rFrameVariable );
	DECLARE_SMART_PTR( Point4rFrameVariable );
	DECLARE_SMART_PTR( Matrix2x2bFrameVariable );
	DECLARE_SMART_PTR( Matrix2x3bFrameVariable );
	DECLARE_SMART_PTR( Matrix2x4bFrameVariable );
	DECLARE_SMART_PTR( Matrix3x2bFrameVariable );
	DECLARE_SMART_PTR( Matrix3x3bFrameVariable );
	DECLARE_SMART_PTR( Matrix3x4bFrameVariable );
	DECLARE_SMART_PTR( Matrix4x2bFrameVariable );
	DECLARE_SMART_PTR( Matrix4x3bFrameVariable );
	DECLARE_SMART_PTR( Matrix4x4bFrameVariable );
	DECLARE_SMART_PTR( Matrix2x2iFrameVariable );
	DECLARE_SMART_PTR( Matrix2x3iFrameVariable );
	DECLARE_SMART_PTR( Matrix2x4iFrameVariable );
	DECLARE_SMART_PTR( Matrix3x2iFrameVariable );
	DECLARE_SMART_PTR( Matrix3x3iFrameVariable );
	DECLARE_SMART_PTR( Matrix3x4iFrameVariable );
	DECLARE_SMART_PTR( Matrix4x2iFrameVariable );
	DECLARE_SMART_PTR( Matrix4x3iFrameVariable );
	DECLARE_SMART_PTR( Matrix4x4iFrameVariable );
	DECLARE_SMART_PTR( Matrix2x2uiFrameVariable );
	DECLARE_SMART_PTR( Matrix2x3uiFrameVariable );
	DECLARE_SMART_PTR( Matrix2x4uiFrameVariable );
	DECLARE_SMART_PTR( Matrix3x2uiFrameVariable );
	DECLARE_SMART_PTR( Matrix3x3uiFrameVariable );
	DECLARE_SMART_PTR( Matrix3x4uiFrameVariable );
	DECLARE_SMART_PTR( Matrix4x2uiFrameVariable );
	DECLARE_SMART_PTR( Matrix4x3uiFrameVariable );
	DECLARE_SMART_PTR( Matrix4x4uiFrameVariable );
	DECLARE_SMART_PTR( Matrix2x2fFrameVariable );
	DECLARE_SMART_PTR( Matrix2x3fFrameVariable );
	DECLARE_SMART_PTR( Matrix2x4fFrameVariable );
	DECLARE_SMART_PTR( Matrix3x2fFrameVariable );
	DECLARE_SMART_PTR( Matrix3x3fFrameVariable );
	DECLARE_SMART_PTR( Matrix3x4fFrameVariable );
	DECLARE_SMART_PTR( Matrix4x2fFrameVariable );
	DECLARE_SMART_PTR( Matrix4x3fFrameVariable );
	DECLARE_SMART_PTR( Matrix4x4fFrameVariable );
	DECLARE_SMART_PTR( Matrix2x2dFrameVariable );
	DECLARE_SMART_PTR( Matrix2x3dFrameVariable );
	DECLARE_SMART_PTR( Matrix2x4dFrameVariable );
	DECLARE_SMART_PTR( Matrix3x2dFrameVariable );
	DECLARE_SMART_PTR( Matrix3x3dFrameVariable );
	DECLARE_SMART_PTR( Matrix3x4dFrameVariable );
	DECLARE_SMART_PTR( Matrix4x2dFrameVariable );
	DECLARE_SMART_PTR( Matrix4x3dFrameVariable );
	DECLARE_SMART_PTR( Matrix4x4dFrameVariable );
	DECLARE_SMART_PTR( Matrix2x2rFrameVariable );
	DECLARE_SMART_PTR( Matrix2x3rFrameVariable );
	DECLARE_SMART_PTR( Matrix2x4rFrameVariable );
	DECLARE_SMART_PTR( Matrix3x2rFrameVariable );
	DECLARE_SMART_PTR( Matrix3x3rFrameVariable );
	DECLARE_SMART_PTR( Matrix3x4rFrameVariable );
	DECLARE_SMART_PTR( Matrix4x2rFrameVariable );
	DECLARE_SMART_PTR( Matrix4x3rFrameVariable );
	DECLARE_SMART_PTR( Matrix4x4rFrameVariable );

	DECLARE_SMART_PTR( FrameVariableLink );
	DECLARE_SMART_PTR( ProgramLinks );
	DECLARE_SMART_PTR( FrameVariableBuffer );
	DECLARE_SMART_PTR( FrameVariable );
	DECLARE_SMART_PTR( ShaderObject );
	DECLARE_SMART_PTR( ShaderProgram );

	DECLARE_LIST( FrameVariableLinkSPtr, VariableLinkPtr );
	DECLARE_MAP( ShaderProgram *, ProgramLinksSPtr, LinksPtrListProgram );
	DECLARE_VECTOR( ShaderProgramSPtr, ShaderProgramPtr );
	DECLARE_VECTOR( ShaderObjectSPtr, ShaderObjectPtr );
	DECLARE_LIST( FrameVariableSPtr, FrameVariablePtr );
	DECLARE_LIST( FrameVariableBufferSPtr, FrameVariableBufferPtr );
	DECLARE_MAP( Castor::String, FrameVariableWPtr, FrameVariablePtrStr );
	DECLARE_MAP( Castor::String, FrameVariableBufferWPtr, FrameVariableBufferPtrStr );
	DECLARE_MAP( ShaderType, FrameVariableBufferWPtr, FrameVariableBufferPtrShader );

	//@}

#define UBO_MATRIX( Writer )\
	GLSL::Ubo l_matrices{ l_writer, ShaderProgram::BufferMatrix };\
	auto c3d_mtxProjection = l_matrices.GetUniform< GLSL::Mat4 >( Pipeline::MtxProjection );\
	auto c3d_mtxModel = l_matrices.GetUniform< GLSL::Mat4 >( Pipeline::MtxModel );\
	auto c3d_mtxView = l_matrices.GetUniform< GLSL::Mat4 >( Pipeline::MtxView );\
	auto c3d_mtxNormal = l_matrices.GetUniform< GLSL::Mat4 >( Pipeline::MtxNormal );\
	auto c3d_mtxTexture0 = l_matrices.GetUniform< GLSL::Mat4 >( Pipeline::MtxTexture[0] );\
	auto c3d_mtxTexture1 = l_matrices.GetUniform< GLSL::Mat4 >( Pipeline::MtxTexture[1] );\
	auto c3d_mtxTexture2 = l_matrices.GetUniform< GLSL::Mat4 >( Pipeline::MtxTexture[2] );\
	auto c3d_mtxTexture3 = l_matrices.GetUniform< GLSL::Mat4 >( Pipeline::MtxTexture[3] );\
	l_matrices.End()

#define UBO_PASS( Writer )\
	GLSL::Ubo l_pass{ l_writer, ShaderProgram::BufferPass };\
	auto c3d_v4MatAmbient = l_pass.GetUniform< GLSL::Vec4 >( ShaderProgram::MatAmbient );\
	auto c3d_v4MatDiffuse = l_pass.GetUniform< GLSL::Vec4 >( ShaderProgram::MatDiffuse );\
	auto c3d_v4MatEmissive = l_pass.GetUniform< GLSL::Vec4 >( ShaderProgram::MatEmissive );\
	auto c3d_v4MatSpecular = l_pass.GetUniform< GLSL::Vec4 >( ShaderProgram::MatSpecular );\
	auto c3d_fMatShininess = l_pass.GetUniform< GLSL::Float >( ShaderProgram::MatShininess );\
	auto c3d_fMatOpacity = l_pass.GetUniform< GLSL::Float >( ShaderProgram::MatOpacity );\
	l_pass.End()

#define UBO_SCENE( Writer )\
	GLSL::Ubo l_scene{ l_writer, ShaderProgram::BufferScene };\
	auto c3d_v4AmbientLight = l_scene.GetUniform< GLSL::Vec4 >( ShaderProgram::AmbientLight );\
	auto c3d_v4BackgroundColour = l_scene.GetUniform< GLSL::Vec4 >( ShaderProgram::BackgroundColour );\
	auto c3d_iLightsCount = l_scene.GetUniform< GLSL::IVec4 >( ShaderProgram::LightsCount );\
	auto c3d_v3CameraPosition = l_scene.GetUniform< GLSL::Vec3 >( ShaderProgram::CameraPos );\
	l_scene.End()

#define UBO_BILLBOARD( Writer )\
	GLSL::Ubo l_billboard{ l_writer, ShaderProgram::BufferBillboards };\
	auto c3d_v2iDimensions = l_billboard.GetUniform< IVec2 >( ShaderProgram::Dimensions );\
	l_billboard.End()

#define UBO_ANIMATION( Writer, Flags )\
	GLSL::Ubo l_animation{ l_writer, ShaderProgram::BufferAnimation };\
	auto c3d_mtxBones = l_animation.GetUniform< GLSL::Mat4 >( ShaderProgram::Bones, 400, CheckFlag( Flags, ProgramFlag::Skinning ) );\
	auto c3d_fTime = l_animation.GetUniform< GLSL::Float >( ShaderProgram::Time, CheckFlag( Flags, ProgramFlag::Morphing ) );\
	l_animation.End()
}

#endif
