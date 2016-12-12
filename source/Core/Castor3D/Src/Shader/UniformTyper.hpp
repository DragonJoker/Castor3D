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

#include <cstdint>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	*/
	template< UniformType T >
	struct UniformTyper;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eSampler.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eSampler.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eSampler >
	{
		using type = Uniform1s;
		using value_type = int;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 1u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eOne;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eBool.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eBool.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eBool >
	{
		using type = Uniform1b;
		using value_type = bool;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 1u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eOne;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec2b.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec2b.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec2b >
	{
		using type = Uniform2b;
		using value_type = Castor::Point2b;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 2u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec3b.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec3b.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec3b >
	{
		using type = Uniform3b;
		using value_type = Castor::Point3b;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 3u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec4b.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec4b.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec4b >
	{
		using type = Uniform4b;
		using value_type = Castor::Point4b;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 4u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x2b.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x2b.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x2b >
	{
		using type = Uniform2x2b;
		using value_type = Castor::SquareMatrix< bool, 2 >;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 4u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x3b.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x3b.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x3b >
	{
		using type = Uniform2x3b;
		using value_type = Castor::Matrix< bool, 2, 3 >;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 6u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x4b.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x4b.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x4b >
	{
		using type = Uniform2x4b;
		using value_type = Castor::Matrix< bool, 2, 4 >;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 8u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x2b.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x2b.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x2b >
	{
		using type = Uniform3x2b;
		using value_type = Castor::Matrix< bool, 3, 2 >;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 6u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x3b.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x3b.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x3b >
	{
		using type = Uniform3x3b;
		using value_type = Castor::SquareMatrix< bool, 3 >;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 9u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x4b.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x4b.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x4b >
	{
		using type = Uniform3x4b;
		using value_type = Castor::Matrix< bool, 3, 4 >;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 12u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x2b.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x2b.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x2b >
	{
		using type = Uniform4x2b;
		using value_type = Castor::Matrix< bool, 4, 2 >;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 8u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x3b.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x3b.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x3b >
	{
		using type = Uniform4x3b;
		using value_type = Castor::Matrix< bool, 4, 3 >;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 12u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x4b.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x4b.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x4b >
	{
		using type = Uniform4x4b;
		using value_type = Castor::SquareMatrix< bool, 4 >;
		using value_sub_type = bool;
		static constexpr uint32_t value_sub_type_count = 16u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eInt.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eInt.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eInt >
	{
		using type = Uniform1i;
		using value_type = int;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 1u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eOne;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec2i.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec2i.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec2i >
	{
		using type = Uniform2i;
		using value_type = Castor::Point2i;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 2u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec3i.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec3i.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec3i >
	{
		using type = Uniform3i;
		using value_type = Castor::Point3i;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 3u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec4i.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec4i.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec4i >
	{
		using type = Uniform4i;
		using value_type = Castor::Point4i;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 4u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x2i.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x2i.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x2i >
	{
		using type = Uniform2x2i;
		using value_type = Castor::SquareMatrix< int, 2 >;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 4u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x3i.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x3i.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x3i >
	{
		using type = Uniform2x3i;
		using value_type = Castor::Matrix< int, 2, 3 >;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 6u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x4i.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x4i.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x4i >
	{
		using type = Uniform2x4i;
		using value_type = Castor::Matrix< int, 2, 4 >;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 8u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x2i.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x2i.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x2i >
	{
		using type = Uniform3x2i;
		using value_type = Castor::Matrix< int, 3, 2 >;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 6u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x3i.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x3i.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x3i >
	{
		using type = Uniform3x3i;
		using value_type = Castor::SquareMatrix< int, 3 >;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 9u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x4i.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x4i.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x4i >
	{
		using type = Uniform3x4i;
		using value_type = Castor::Matrix< int, 3, 4 >;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 12u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x2i.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x2i.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x2i >
	{
		using type = Uniform4x2i;
		using value_type = Castor::Matrix< int, 4, 2 >;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 8u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x3i.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x3i.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x3i >
	{
		using type = Uniform4x3i;
		using value_type = Castor::Matrix< int, 4, 3 >;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 12u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x4i.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x4i.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x4i >
	{
		using type = Uniform4x4i;
		using value_type = Castor::SquareMatrix< int, 4 >;
		using value_sub_type = int;
		static constexpr uint32_t value_sub_type_count = 16u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eUInt.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eUInt.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eUInt >
	{
		using type = Uniform1ui;
		using value_type = uint32_t;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 1u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eOne;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec2ui.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec2ui.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec2ui >
	{
		using type = Uniform2ui;
		using value_type = Castor::Point2ui;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 2u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec3ui.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec3ui.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec3ui >
	{
		using type = Uniform3ui;
		using value_type = Castor::Point3ui;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 3u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec4ui.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec4ui.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec4ui >
	{
		using type = Uniform4ui;
		using value_type = Castor::Point4ui;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 4u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x2ui.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x2ui.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x2ui >
	{
		using type = Uniform2x2ui;
		using value_type = Castor::SquareMatrix< uint32_t, 2 >;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 4u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x3ui.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x3ui.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x3ui >
	{
		using type = Uniform2x3ui;
		using value_type = Castor::Matrix< uint32_t, 2, 3 >;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 6u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x4ui.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x4ui.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x4ui >
	{
		using type = Uniform2x4ui;
		using value_type = Castor::Matrix< uint32_t, 2, 4 >;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 8u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x2ui.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x2ui.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x2ui >
	{
		using type = Uniform3x2ui;
		using value_type = Castor::Matrix< uint32_t, 3, 2 >;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 6u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x3ui.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x3ui.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x3ui >
	{
		using type = Uniform3x3ui;
		using value_type = Castor::SquareMatrix< uint32_t, 3 >;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 9u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x4ui.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x4ui.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x4ui >
	{
		using type = Uniform3x4ui;
		using value_type = Castor::Matrix< uint32_t, 3, 4 >;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 12u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x2ui.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x2ui.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x2ui >
	{
		using type = Uniform4x2ui;
		using value_type = Castor::Matrix< uint32_t, 4, 2 >;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 8u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x3ui.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x3ui.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x3ui >
	{
		using type = Uniform4x3ui;
		using value_type = Castor::Matrix< uint32_t, 4, 3 >;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 12u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x4ui.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x4ui.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x4ui >
	{
		using type = Uniform4x4ui;
		using value_type = Castor::SquareMatrix< uint32_t, 4 >;
		using value_sub_type = uint32_t;
		static constexpr uint32_t value_sub_type_count = 16u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eFloat.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eFloat.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eFloat >
	{
		using type = Uniform1f;
		using value_type = float;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 1u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eOne;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec2f.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec2f.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec2f >
	{
		using type = Uniform2f;
		using value_type = Castor::Point2f;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 2u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec3f.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec3f.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec3f >
	{
		using type = Uniform3f;
		using value_type = Castor::Point3f;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 3u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec4f.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec4f.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec4f >
	{
		using type = Uniform4f;
		using value_type = Castor::Point4f;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 4u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x2f.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x2f.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x2f >
	{
		using type = Uniform2x2f;
		using value_type = Castor::SquareMatrix< float, 2 >;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 4u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x3f.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x3f.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x3f >
	{
		using type = Uniform2x3f;
		using value_type = Castor::Matrix< float, 2, 3 >;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 6u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x4f.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x4f.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x4f >
	{
		using type = Uniform2x4f;
		using value_type = Castor::Matrix< float, 2, 4 >;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 8u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x2f.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x2f.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x2f >
	{
		using type = Uniform3x2f;
		using value_type = Castor::Matrix< float, 3, 2 >;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 6u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x3f.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x3f.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x3f >
	{
		using type = Uniform3x3f;
		using value_type = Castor::SquareMatrix< float, 3 >;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 9u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x4f.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x4f.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x4f >
	{
		using type = Uniform3x4f;
		using value_type = Castor::Matrix< float, 3, 4 >;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 12u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x2f.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x2f.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x2f >
	{
		using type = Uniform4x2f;
		using value_type = Castor::Matrix< float, 4, 2 >;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 8u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x3f.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x3f.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x3f >
	{
		using type = Uniform4x3f;
		using value_type = Castor::Matrix< float, 4, 3 >;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 12u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x4f.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x4f.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x4f >
	{
		using type = Uniform4x4f;
		using value_type = Castor::SquareMatrix< float, 4 >;
		using value_sub_type = float;
		static constexpr uint32_t value_sub_type_count = 16u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eDouble.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eDouble.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eDouble >
	{
		using type = Uniform1d;
		using value_type = double;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 1u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eOne;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec2d.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec2d.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec2d >
	{
		using type = Uniform2d;
		using value_type = Castor::Point2d;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 2u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec3d.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec3d.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec3d >
	{
		using type = Uniform3d;
		using value_type = Castor::Point3d;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 3u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eVec4d.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eVec4d.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eVec4d >
	{
		using type = Uniform4d;
		using value_type = Castor::Point4d;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 4u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eVec4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x2d.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x2d.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x2d >
	{
		using type = Uniform2x2d;
		using value_type = Castor::SquareMatrix< double, 2 >;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 4u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x3d.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x3d.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x3d >
	{
		using type = Uniform2x3d;
		using value_type = Castor::Matrix< double, 2, 3 >;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 6u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat2x4d.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat2x4d.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat2x4d >
	{
		using type = Uniform2x4d;
		using value_type = Castor::Matrix< double, 2, 4 >;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 8u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat2x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x2d.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x2d.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x2d >
	{
		using type = Uniform3x2d;
		using value_type = Castor::Matrix< double, 3, 2 >;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 6u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x3d.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x3d.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x3d >
	{
		using type = Uniform3x3d;
		using value_type = Castor::SquareMatrix< double, 3 >;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 9u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat3x4d.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat3x4d.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat3x4d >
	{
		using type = Uniform3x4d;
		using value_type = Castor::Matrix< double, 3, 4 >;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 12u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat3x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x2d.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x2d.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x2d >
	{
		using type = Uniform4x2d;
		using value_type = Castor::Matrix< double, 4, 2 >;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 8u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x2;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x3d.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x3d.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x3d >
	{
		using type = Uniform4x3d;
		using value_type = Castor::Matrix< double, 4, 3 >;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 12u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x3;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for UniformType::eMat4x4d.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eMat4x4d.
	\remarks
	*/
	template<>
	struct UniformTyper< UniformType::eMat4x4d >
	{
		using type = Uniform4x4d;
		using value_type = Castor::SquareMatrix< double, 4 >;
		using value_sub_type = double;
		static constexpr uint32_t value_sub_type_count = 16u;
		static constexpr uint32_t size = uint32_t( sizeof( value_sub_type ) * value_sub_type_count );
		static constexpr VariableType variable_type = VariableType::eMat4x4;
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
}

#endif
