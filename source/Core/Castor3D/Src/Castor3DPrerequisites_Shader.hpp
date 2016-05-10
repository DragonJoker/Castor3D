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
	\~english
	\brief		Enumeration of existing shader lnaguages
	\~french
	\brief		Enumération des langages de shader existants
	\remark
	*/
	typedef enum eSHADER_LANGUAGE
		: uint8_t
	{
		eSHADER_LANGUAGE_GLSL,
		eSHADER_LANGUAGE_AUTO,
		CASTOR_ENUM_BOUNDS( eSHADER_LANGUAGE, eSHADER_LANGUAGE_GLSL )
	}	eSHADER_LANGUAGE;
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
	typedef enum eSHADER_TYPE
		: int8_t
	{
		eSHADER_TYPE_NONE = -1,
		eSHADER_TYPE_VERTEX,
		eSHADER_TYPE_HULL,
		eSHADER_TYPE_DOMAIN,
		eSHADER_TYPE_GEOMETRY,
		eSHADER_TYPE_PIXEL,
		eSHADER_TYPE_COMPUTE,
		CASTOR_ENUM_BOUNDS( eSHADER_TYPE, eSHADER_TYPE_NONE )
	}	eSHADER_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Shader object types masks
	\~french
	\brief		Masques pour les types de shader object
	*/
	static const uint64_t MASK_SHADER_TYPE_VERTEX = uint64_t( 0x1 ) << eSHADER_TYPE_VERTEX;
	static const uint64_t MASK_SHADER_TYPE_HULL = uint64_t( 0x1 ) << eSHADER_TYPE_HULL;
	static const uint64_t MASK_SHADER_TYPE_DOMAIN = uint64_t( 0x1 ) << eSHADER_TYPE_DOMAIN;
	static const uint64_t MASK_SHADER_TYPE_GEOMETRY = uint64_t( 0x1 ) << eSHADER_TYPE_GEOMETRY;
	static const uint64_t MASK_SHADER_TYPE_PIXEL = uint64_t( 0x1 ) << eSHADER_TYPE_PIXEL;
	static const uint64_t MASK_SHADER_TYPE_COMPUTE = uint64_t( 0x1 ) << eSHADER_TYPE_COMPUTE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/11/13
	\version	0.7.0.0
	\~english
	\brief		Flags to use when looking for an automatically generated program
	\~french
	\brief		Indicateurs à utiliser lors de la recherche de programme généré automatiquement
	*/
	typedef enum ePROGRAM_FLAG
		: uint32_t
	{
		//!\~english Program using instanciation	\~french Programme utilisant l'instanciation
		ePROGRAM_FLAG_INSTANCIATION = 0x01,
		//!\~english Program using skeleton animations	\~french Programme utilisant les animations par squelette
		ePROGRAM_FLAG_SKINNING = 0x02,
		//!\~english Program used by billboards	\~french Programme utilisé par des billboards
		ePROGRAM_FLAG_BILLBOARDS = 0x04,
	}	ePROGRAM_FLAG;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Supported variables dimensions enumeration
	\~french
	\brief		Enumération des dimensions de variable supportées
	*/
	typedef enum eVARIABLE_TYPE
		: uint8_t
	{
		//!\~english One variable	\~french Une variable
		eVARIABLE_TYPE_ONE,
		//!\~english 1 variable array	\~french Tableau de 1 variable
		eVARIABLE_TYPE_VEC1,
		//!\~english 2 variables array	\~french Tableau de 2 variables
		eVARIABLE_TYPE_VEC2,
		//!\~english 3 variables array	\~french Tableau de 3 variables
		eVARIABLE_TYPE_VEC3,
		//!\~english 4 variables array	\~french Tableau de 4 variables
		eVARIABLE_TYPE_VEC4,
		//!\~english 1x1 matrix	\~french Matrice 1x1
		eVARIABLE_TYPE_MAT1,
		//!\~english 2x2 matrix	\~french Matrice 2x2
		eVARIABLE_TYPE_MAT2X2,
		//!\~english 2x3 matrix	\~french Matrice 2x3
		eVARIABLE_TYPE_MAT2X3,
		//!\~english 2x4 matrix	\~french Matrice 2x4
		eVARIABLE_TYPE_MAT2X4,
		//!\~english 3x2 matrix	\~french Matrice 3x2
		eVARIABLE_TYPE_MAT3X2,
		//!\~english 3x3 matrix	\~french Matrice 3x3
		eVARIABLE_TYPE_MAT3X3,
		//!\~english 3x4 matrix	\~french Matrice 3x4
		eVARIABLE_TYPE_MAT3X4,
		//!\~english 4x2 matrix	\~french Matrice 4x2
		eVARIABLE_TYPE_MAT4X2,
		//!\~english 4x3 matrix	\~french Matrice 4x3
		eVARIABLE_TYPE_MAT4X3,
		//!\~english 4x4 matrix	\~french Matrice 4x4
		eVARIABLE_TYPE_MAT4X4,
		CASTOR_ENUM_BOUNDS( eVARIABLE_TYPE, eVARIABLE_TYPE_ONE )
	}	eVARIABLE_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Supported variables full type enumeration
	\~french
	\brief		Enumération des types complets de variable supportées
	*/
	typedef enum eFRAME_VARIABLE_TYPE
		: uint8_t
	{
		eFRAME_VARIABLE_TYPE_BOOL,
		eFRAME_VARIABLE_TYPE_INT,
		eFRAME_VARIABLE_TYPE_UINT,
		eFRAME_VARIABLE_TYPE_FLOAT,
		eFRAME_VARIABLE_TYPE_DOUBLE,
		eFRAME_VARIABLE_TYPE_SAMPLER,
		eFRAME_VARIABLE_TYPE_VEC2B,
		eFRAME_VARIABLE_TYPE_VEC3B,
		eFRAME_VARIABLE_TYPE_VEC4B,
		eFRAME_VARIABLE_TYPE_VEC2I,
		eFRAME_VARIABLE_TYPE_VEC3I,
		eFRAME_VARIABLE_TYPE_VEC4I,
		eFRAME_VARIABLE_TYPE_VEC2UI,
		eFRAME_VARIABLE_TYPE_VEC3UI,
		eFRAME_VARIABLE_TYPE_VEC4UI,
		eFRAME_VARIABLE_TYPE_VEC2F,
		eFRAME_VARIABLE_TYPE_VEC3F,
		eFRAME_VARIABLE_TYPE_VEC4F,
		eFRAME_VARIABLE_TYPE_VEC2D,
		eFRAME_VARIABLE_TYPE_VEC3D,
		eFRAME_VARIABLE_TYPE_VEC4D,
		eFRAME_VARIABLE_TYPE_MAT2X2B,
		eFRAME_VARIABLE_TYPE_MAT2X3B,
		eFRAME_VARIABLE_TYPE_MAT2X4B,
		eFRAME_VARIABLE_TYPE_MAT3X2B,
		eFRAME_VARIABLE_TYPE_MAT3X3B,
		eFRAME_VARIABLE_TYPE_MAT3X4B,
		eFRAME_VARIABLE_TYPE_MAT4X2B,
		eFRAME_VARIABLE_TYPE_MAT4X3B,
		eFRAME_VARIABLE_TYPE_MAT4X4B,
		eFRAME_VARIABLE_TYPE_MAT2X2I,
		eFRAME_VARIABLE_TYPE_MAT2X3I,
		eFRAME_VARIABLE_TYPE_MAT2X4I,
		eFRAME_VARIABLE_TYPE_MAT3X2I,
		eFRAME_VARIABLE_TYPE_MAT3X3I,
		eFRAME_VARIABLE_TYPE_MAT3X4I,
		eFRAME_VARIABLE_TYPE_MAT4X2I,
		eFRAME_VARIABLE_TYPE_MAT4X3I,
		eFRAME_VARIABLE_TYPE_MAT4X4I,
		eFRAME_VARIABLE_TYPE_MAT2X2UI,
		eFRAME_VARIABLE_TYPE_MAT2X3UI,
		eFRAME_VARIABLE_TYPE_MAT2X4UI,
		eFRAME_VARIABLE_TYPE_MAT3X2UI,
		eFRAME_VARIABLE_TYPE_MAT3X3UI,
		eFRAME_VARIABLE_TYPE_MAT3X4UI,
		eFRAME_VARIABLE_TYPE_MAT4X2UI,
		eFRAME_VARIABLE_TYPE_MAT4X3UI,
		eFRAME_VARIABLE_TYPE_MAT4X4UI,
		eFRAME_VARIABLE_TYPE_MAT2X2F,
		eFRAME_VARIABLE_TYPE_MAT2X3F,
		eFRAME_VARIABLE_TYPE_MAT2X4F,
		eFRAME_VARIABLE_TYPE_MAT3X2F,
		eFRAME_VARIABLE_TYPE_MAT3X3F,
		eFRAME_VARIABLE_TYPE_MAT3X4F,
		eFRAME_VARIABLE_TYPE_MAT4X2F,
		eFRAME_VARIABLE_TYPE_MAT4X3F,
		eFRAME_VARIABLE_TYPE_MAT4X4F,
		eFRAME_VARIABLE_TYPE_MAT2X2D,
		eFRAME_VARIABLE_TYPE_MAT2X3D,
		eFRAME_VARIABLE_TYPE_MAT2X4D,
		eFRAME_VARIABLE_TYPE_MAT3X2D,
		eFRAME_VARIABLE_TYPE_MAT3X3D,
		eFRAME_VARIABLE_TYPE_MAT3X4D,
		eFRAME_VARIABLE_TYPE_MAT4X2D,
		eFRAME_VARIABLE_TYPE_MAT4X3D,
		eFRAME_VARIABLE_TYPE_MAT4X4D,
		CASTOR_ENUM_BOUNDS( eFRAME_VARIABLE_TYPE, eFRAME_VARIABLE_TYPE_BOOL )
	}	eFRAME_VARIABLE_TYPE;

#if CASTOR_USE_DOUBLE
#	define eFRAME_VARIABLE_TYPE_REAL	eFRAME_VARIABLE_TYPE_DOUBLE
#	define eFRAME_VARIABLE_TYPE_VEC2R	eFRAME_VARIABLE_TYPE_VEC2D
#	define eFRAME_VARIABLE_TYPE_VEC3R	eFRAME_VARIABLE_TYPE_VEC3D
#	define eFRAME_VARIABLE_TYPE_VEC4R	eFRAME_VARIABLE_TYPE_VEC4D
#	define eFRAME_VARIABLE_TYPE_MAT2X2R	eFRAME_VARIABLE_TYPE_MAT2X2D
#	define eFRAME_VARIABLE_TYPE_MAT2X3R	eFRAME_VARIABLE_TYPE_MAT2X3D
#	define eFRAME_VARIABLE_TYPE_MAT2X4R	eFRAME_VARIABLE_TYPE_MAT2X4D
#	define eFRAME_VARIABLE_TYPE_MAT3X2R	eFRAME_VARIABLE_TYPE_MAT3X2D
#	define eFRAME_VARIABLE_TYPE_MAT3X3R	eFRAME_VARIABLE_TYPE_MAT3X3D
#	define eFRAME_VARIABLE_TYPE_MAT3X4R	eFRAME_VARIABLE_TYPE_MAT3X4D
#	define eFRAME_VARIABLE_TYPE_MAT4X2R	eFRAME_VARIABLE_TYPE_MAT4X2D
#	define eFRAME_VARIABLE_TYPE_MAT4X3R	eFRAME_VARIABLE_TYPE_MAT4X3D
#	define eFRAME_VARIABLE_TYPE_MAT4X4R	eFRAME_VARIABLE_TYPE_MAT4X4D
#else
#	define eFRAME_VARIABLE_TYPE_REAL	eFRAME_VARIABLE_TYPE_FLOAT
#	define eFRAME_VARIABLE_TYPE_VEC2R	eFRAME_VARIABLE_TYPE_VEC2F
#	define eFRAME_VARIABLE_TYPE_VEC3R	eFRAME_VARIABLE_TYPE_VEC3F
#	define eFRAME_VARIABLE_TYPE_VEC4R	eFRAME_VARIABLE_TYPE_VEC4F
#	define eFRAME_VARIABLE_TYPE_MAT2X2R	eFRAME_VARIABLE_TYPE_MAT2X2F
#	define eFRAME_VARIABLE_TYPE_MAT2X3R	eFRAME_VARIABLE_TYPE_MAT2X3F
#	define eFRAME_VARIABLE_TYPE_MAT2X4R	eFRAME_VARIABLE_TYPE_MAT2X4F
#	define eFRAME_VARIABLE_TYPE_MAT3X2R	eFRAME_VARIABLE_TYPE_MAT3X2F
#	define eFRAME_VARIABLE_TYPE_MAT3X3R	eFRAME_VARIABLE_TYPE_MAT3X3F
#	define eFRAME_VARIABLE_TYPE_MAT3X4R	eFRAME_VARIABLE_TYPE_MAT3X4F
#	define eFRAME_VARIABLE_TYPE_MAT4X2R	eFRAME_VARIABLE_TYPE_MAT4X2F
#	define eFRAME_VARIABLE_TYPE_MAT4X3R	eFRAME_VARIABLE_TYPE_MAT4X3F
#	define eFRAME_VARIABLE_TYPE_MAT4X4R	eFRAME_VARIABLE_TYPE_MAT4X4F
#endif

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
	DECLARE_MAP( eSHADER_TYPE, FrameVariableBufferWPtr, FrameVariableBufferPtrShader );

	//@}
}

#endif
