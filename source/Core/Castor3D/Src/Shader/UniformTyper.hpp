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
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	*/
	template< typename T >
	struct UniformTyper;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for OneUniform< bool >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour OneUniform< bool >.
	\remarks
	*/
	template<>
	struct UniformTyper< OneUniform< bool > >
	{
		static UniformType const value = UniformType::eBool;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< bool, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< bool, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< bool, 2 > >
	{
		static UniformType const value = UniformType::eVec2b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< bool, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< bool, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< bool, 3 > >
	{
		static UniformType const value = UniformType::eVec3b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< bool, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< bool, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< bool, 4 > >
	{
		static UniformType const value = UniformType::eVec4b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< bool, 2, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< bool, 2, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< bool, 2, 2 > >
	{
		static UniformType const value = UniformType::eMat2x2b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< bool, 2, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< bool, 2, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< bool, 2, 3 > >
	{
		static UniformType const value = UniformType::eMat2x3b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< bool, 2, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< bool, 2, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< bool, 2, 4 > >
	{
		static UniformType const value = UniformType::eMat2x4b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< bool, 3, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< bool, 3, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< bool, 3, 2 > >
	{
		static UniformType const value = UniformType::eMat3x2b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< bool, 3, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< bool, 3, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< bool, 3, 3 > >
	{
		static UniformType const value = UniformType::eMat3x3b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< bool, 3, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< bool, 3, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< bool, 3, 4 > >
	{
		static UniformType const value = UniformType::eMat3x4b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< bool, 4, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< bool, 4, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< bool, 4, 2 > >
	{
		static UniformType const value = UniformType::eMat4x2b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< bool, 4, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< bool, 4, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< bool, 4, 3 > >
	{
		static UniformType const value = UniformType::eMat4x3b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< bool, 4, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< bool, 4, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< bool, 4, 4 > >
	{
		static UniformType const value = UniformType::eMat4x4b;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for Uniform1i.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour Uniform1i.
	\remarks
	*/
	template<>
	struct UniformTyper< Uniform1i >
	{
		static UniformType const value = UniformType::eInt;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< int, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< int, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< int, 2 > >
	{
		static UniformType const value = UniformType::eVec2i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< int, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< int, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< int, 3 > >
	{
		static UniformType const value = UniformType::eVec3i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< int, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< int, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< int, 4 > >
	{
		static UniformType const value = UniformType::eVec4i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< int, 2, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< int, 2, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< int, 2, 2 > >
	{
		static UniformType const value = UniformType::eMat2x2i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< int, 2, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< int, 2, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< int, 2, 3 > >
	{
		static UniformType const value = UniformType::eMat2x3i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< int, 2, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< int, 2, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< int, 2, 4 > >
	{
		static UniformType const value = UniformType::eMat2x4i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< int, 3, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< int, 3, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< int, 3, 2 > >
	{
		static UniformType const value = UniformType::eMat3x2i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< int, 3, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< int, 3, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< int, 3, 3 > >
	{
		static UniformType const value = UniformType::eMat3x3i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< int, 3, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< int, 3, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< int, 3, 4 > >
	{
		static UniformType const value = UniformType::eMat3x4i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< int, 4, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< int, 4, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< int, 4, 2 > >
	{
		static UniformType const value = UniformType::eMat4x2i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< int, 4, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< int, 4, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< int, 4, 3 > >
	{
		static UniformType const value = UniformType::eMat4x3i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< int, 4, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< int, 4, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< int, 4, 4 > >
	{
		static UniformType const value = UniformType::eMat4x4i;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for OneUniform< unsigned int >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour OneUniform< unsigned int >.
	\remarks
	*/
	template<>
	struct UniformTyper< OneUniform< unsigned int > >
	{
		static UniformType const value = UniformType::eUInt;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< unsigned int, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< unsigned int, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< unsigned int, 2 > >
	{
		static UniformType const value = UniformType::eVec2ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< unsigned int, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< unsigned int, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< unsigned int, 3 > >
	{
		static UniformType const value = UniformType::eVec3ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< unsigned int, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< unsigned int, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< unsigned int, 4 > >
	{
		static UniformType const value = UniformType::eVec4ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< unsigned int, 2, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< unsigned int, 2, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< unsigned int, 2, 2 > >
	{
		static UniformType const value = UniformType::eMat2x2ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< unsigned int, 2, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< unsigned int, 2, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< unsigned int, 2, 3 > >
	{
		static UniformType const value = UniformType::eMat2x3ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< unsigned int, 2, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< unsigned int, 2, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< unsigned int, 2, 4 > >
	{
		static UniformType const value = UniformType::eMat2x4ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< unsigned int, 3, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< unsigned int, 3, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< unsigned int, 3, 2 > >
	{
		static UniformType const value = UniformType::eMat3x2ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< unsigned int, 3, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< unsigned int, 3, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< unsigned int, 3, 3 > >
	{
		static UniformType const value = UniformType::eMat3x3ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< unsigned int, 3, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< unsigned int, 3, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< unsigned int, 3, 4 > >
	{
		static UniformType const value = UniformType::eMat3x4ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< unsigned int, 4, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< unsigned int, 4, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< unsigned int, 4, 2 > >
	{
		static UniformType const value = UniformType::eMat4x2ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< unsigned int, 4, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< unsigned int, 4, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< unsigned int, 4, 3 > >
	{
		static UniformType const value = UniformType::eMat4x3ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< unsigned int, 4, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< unsigned int, 4, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< unsigned int, 4, 4 > >
	{
		static UniformType const value = UniformType::eMat4x4ui;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for OneUniform< float >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour OneUniform< float >.
	\remarks
	*/
	template<>
	struct UniformTyper< OneUniform< float > >
	{
		static UniformType const value = UniformType::eFloat;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< float, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< float, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< float, 2 > >
	{
		static UniformType const value = UniformType::eVec2f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< float, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< float, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< float, 3 > >
	{
		static UniformType const value = UniformType::eVec3f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< float, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< float, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< float, 4 > >
	{
		static UniformType const value = UniformType::eVec4f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< float, 2, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< float, 2, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< float, 2, 2 > >
	{
		static UniformType const value = UniformType::eMat2x2f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< float, 2, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< float, 2, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< float, 2, 3 > >
	{
		static UniformType const value = UniformType::eMat2x3f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< float, 2, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< float, 2, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< float, 2, 4 > >
	{
		static UniformType const value = UniformType::eMat2x4f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< float, 3, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< float, 3, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< float, 3, 2 > >
	{
		static UniformType const value = UniformType::eMat3x2f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< float, 3, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< float, 3, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< float, 3, 3 > >
	{
		static UniformType const value = UniformType::eMat3x3f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< float, 3, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< float, 3, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< float, 3, 4 > >
	{
		static UniformType const value = UniformType::eMat3x4f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< float, 4, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< float, 4, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< float, 4, 2 > >
	{
		static UniformType const value = UniformType::eMat4x2f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< float, 4, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< float, 4, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< float, 4, 3 > >
	{
		static UniformType const value = UniformType::eMat4x3f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< float, 4, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< float, 4, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< float, 4, 4 > >
	{
		static UniformType const value = UniformType::eMat4x4f;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for OneUniform< double >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour OneUniform< double >.
	\remarks
	*/
	template<>
	struct UniformTyper< OneUniform< double > >
	{
		static UniformType const value = UniformType::eDouble;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< double, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< double, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< double, 2 > >
	{
		static UniformType const value = UniformType::eVec2d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< double, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< double, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< double, 3 > >
	{
		static UniformType const value = UniformType::eVec3d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for PointUniform< double, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour PointUniform< double, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< PointUniform< double, 4 > >
	{
		static UniformType const value = UniformType::eVec4d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< double, 2, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< double, 2, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< double, 2, 2 > >
	{
		static UniformType const value = UniformType::eMat2x2d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< double, 2, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< double, 2, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< double, 2, 3 > >
	{
		static UniformType const value = UniformType::eMat2x3d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< double, 2, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< double, 2, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< double, 2, 4 > >
	{
		static UniformType const value = UniformType::eMat2x4d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< double, 3, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< double, 3, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< double, 3, 2 > >
	{
		static UniformType const value = UniformType::eMat3x2d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< double, 3, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< double, 3, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< double, 3, 3 > >
	{
		static UniformType const value = UniformType::eMat3x3d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< double, 3, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< double, 3, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< double, 3, 4 > >
	{
		static UniformType const value = UniformType::eMat3x4d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< double, 4, 2 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< double, 4, 2 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< double, 4, 2 > >
	{
		static UniformType const value = UniformType::eMat4x2d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< double, 4, 3 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< double, 4, 3 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< double, 4, 3 > >
	{
		static UniformType const value = UniformType::eMat4x3d;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a UniformType from a frame variable type.
	\remarks	Specialisation for MatrixUniform< double, 4, 4 >.
	\~french
	\brief		Structure d'aide permettant de récupérer un UniformType depuis un type de frame variable.
	\remarks	Spécialisation pour MatrixUniform< double, 4, 4 >.
	\remarks
	*/
	template<>
	struct UniformTyper< MatrixUniform< double, 4, 4 > >
	{
		static UniformType const value = UniformType::eMat4x4d;
	};
}

#endif
