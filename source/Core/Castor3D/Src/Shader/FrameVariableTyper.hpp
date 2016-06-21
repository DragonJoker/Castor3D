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
#ifndef ___C3D_FRAME_VARIABLE_TYPER_H___
#define ___C3D_FRAME_VARIABLE_TYPER_H___

#include "Castor3DPrerequisites.hpp"

#include "Texture/TextureLayout.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	*/
	template< typename T >
	struct FrameVariableTyper;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for OneFrameVariable< bool >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour OneFrameVariable< bool >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< OneFrameVariable< bool > >
	{
		static FrameVariableType const value = FrameVariableType::Bool;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< bool, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< bool, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< bool, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec2b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< bool, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< bool, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< bool, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec3b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< bool, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< bool, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< bool, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec4b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< bool, 2, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< bool, 2, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< bool, 2, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x2b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< bool, 2, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< bool, 2, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< bool, 2, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x3b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< bool, 2, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< bool, 2, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< bool, 2, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x4b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< bool, 3, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< bool, 3, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< bool, 3, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x2b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< bool, 3, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< bool, 3, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< bool, 3, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x3b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< bool, 3, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< bool, 3, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< bool, 3, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x4b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< bool, 4, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< bool, 4, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< bool, 4, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x2b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< bool, 4, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< bool, 4, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< bool, 4, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x3b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< bool, 4, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< bool, 4, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< bool, 4, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x4b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for OneIntFrameVariable.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour OneIntFrameVariable.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< OneIntFrameVariable >
	{
		static FrameVariableType const value = FrameVariableType::Int;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< int, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< int, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< int, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec2i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< int, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< int, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< int, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec3i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< int, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< int, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< int, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec4i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< int, 2, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< int, 2, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< int, 2, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x2i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< int, 2, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< int, 2, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< int, 2, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x3i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< int, 2, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< int, 2, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< int, 2, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x4i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< int, 3, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< int, 3, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< int, 3, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x2i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< int, 3, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< int, 3, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< int, 3, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x3i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< int, 3, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< int, 3, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< int, 3, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x4i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< int, 4, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< int, 4, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< int, 4, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x2i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< int, 4, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< int, 4, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< int, 4, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x3i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< int, 4, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< int, 4, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< int, 4, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x4i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for OneFrameVariable< unsigned int >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour OneFrameVariable< unsigned int >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< OneFrameVariable< unsigned int > >
	{
		static FrameVariableType const value = FrameVariableType::UInt;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< unsigned int, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< unsigned int, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< unsigned int, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec2ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< unsigned int, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< unsigned int, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< unsigned int, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec3ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< unsigned int, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< unsigned int, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< unsigned int, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec4ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< unsigned int, 2, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< unsigned int, 2, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< unsigned int, 2, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x2ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< unsigned int, 2, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< unsigned int, 2, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< unsigned int, 2, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x3ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< unsigned int, 2, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< unsigned int, 2, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< unsigned int, 2, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x4ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< unsigned int, 3, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< unsigned int, 3, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< unsigned int, 3, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x2ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< unsigned int, 3, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< unsigned int, 3, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< unsigned int, 3, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x3ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< unsigned int, 3, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< unsigned int, 3, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< unsigned int, 3, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x4ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< unsigned int, 4, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< unsigned int, 4, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< unsigned int, 4, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x2ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< unsigned int, 4, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< unsigned int, 4, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< unsigned int, 4, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x3ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< unsigned int, 4, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< unsigned int, 4, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< unsigned int, 4, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x4ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for OneFrameVariable< float >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour OneFrameVariable< float >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< OneFrameVariable< float > >
	{
		static FrameVariableType const value = FrameVariableType::Float;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< float, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< float, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< float, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec2f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< float, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< float, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< float, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec3f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< float, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< float, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< float, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec4f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< float, 2, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< float, 2, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< float, 2, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x2f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< float, 2, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< float, 2, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< float, 2, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x3f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< float, 2, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< float, 2, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< float, 2, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x4f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< float, 3, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< float, 3, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< float, 3, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x2f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< float, 3, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< float, 3, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< float, 3, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x3f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< float, 3, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< float, 3, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< float, 3, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x4f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< float, 4, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< float, 4, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< float, 4, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x2f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< float, 4, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< float, 4, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< float, 4, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x3f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< float, 4, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< float, 4, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< float, 4, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x4f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for OneFrameVariable< double >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour OneFrameVariable< double >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< OneFrameVariable< double > >
	{
		static FrameVariableType const value = FrameVariableType::Double;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< double, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< double, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< double, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec2d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< double, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< double, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< double, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec3d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for PointFrameVariable< double, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour PointFrameVariable< double, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< PointFrameVariable< double, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Vec4d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< double, 2, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< double, 2, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< double, 2, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x2d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< double, 2, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< double, 2, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< double, 2, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x3d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< double, 2, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< double, 2, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< double, 2, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat2x4d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< double, 3, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< double, 3, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< double, 3, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x2d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< double, 3, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< double, 3, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< double, 3, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x3d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< double, 3, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< double, 3, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< double, 3, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat3x4d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< double, 4, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< double, 4, 2 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< double, 4, 2 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x2d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< double, 4, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< double, 4, 3 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< double, 4, 3 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x3d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a FrameVariableType from a frame variable type.
	\remarks	Specialisation for MatrixFrameVariable< double, 4, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un FrameVariableType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixFrameVariable< double, 4, 4 >.
	\remarks
	*/
	template<>
	struct FrameVariableTyper< MatrixFrameVariable< double, 4, 4 > >
	{
		static FrameVariableType const value = FrameVariableType::Mat4x4d;
	};
}

#endif
