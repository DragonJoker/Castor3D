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
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	*/
	template< UniformType Type, typename Enable = void >
	struct variable_type;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for one value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une seule valeur.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eBool
								|| Type == UniformType::eInt
								|| Type == UniformType::eUInt
								|| Type == UniformType::eFloat
								|| Type == UniformType::eDouble
								|| Type == UniformType::eSampler >::type >
	{
		static constexpr VariableType value = VariableType::eOne;
		static constexpr uint32_t count = 1u;
		static constexpr bool is_matrix = false;
		static constexpr bool is_point = false;
		static constexpr bool is_one = true;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for vec2 value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une valeur vec2.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eVec2b
								|| Type == UniformType::eVec2i
								|| Type == UniformType::eVec2ui
								|| Type == UniformType::eVec2f
								|| Type == UniformType::eVec2d >::type >
	{
		static constexpr VariableType value = VariableType::eVec2;
		static constexpr uint32_t count = 2u;
		static constexpr bool is_matrix = false;
		static constexpr bool is_point = true;
		static constexpr bool is_one = false;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for vec3 value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une valeur vec3.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eVec3b
								|| Type == UniformType::eVec3i
								|| Type == UniformType::eVec3ui
								|| Type == UniformType::eVec3f
								|| Type == UniformType::eVec3d >::type >
	{
		static constexpr VariableType value = VariableType::eVec3;
		static constexpr uint32_t count = 3u;
		static constexpr bool is_matrix = false;
		static constexpr bool is_point = true;
		static constexpr bool is_one = false;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for vec4 value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une valeur vec4.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eVec4b
								|| Type == UniformType::eVec4i
								|| Type == UniformType::eVec4ui
								|| Type == UniformType::eVec4f
								|| Type == UniformType::eVec4d >::type >
	{
		static constexpr VariableType value = VariableType::eVec4;
		static constexpr uint32_t count = 4u;
		static constexpr bool is_matrix = false;
		static constexpr bool is_point = true;
		static constexpr bool is_one = false;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for mat2x2 value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une valeur mat2x2.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eMat2x2b
								|| Type == UniformType::eMat2x2i
								|| Type == UniformType::eMat2x2ui
								|| Type == UniformType::eMat2x2f
								|| Type == UniformType::eMat2x2d >::type >
	{
		static constexpr VariableType value = VariableType::eMat2x2;
		static constexpr uint32_t cols = 2u;
		static constexpr uint32_t rows = 4u;
		static constexpr uint32_t count = rows * cols;
		static constexpr bool is_matrix = true;
		static constexpr bool is_point = false;
		static constexpr bool is_one = false;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for mat2x3 value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une valeur mat2x3.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eMat2x3b
								|| Type == UniformType::eMat2x3i
								|| Type == UniformType::eMat2x3ui
								|| Type == UniformType::eMat2x3f
								|| Type == UniformType::eMat2x3d >::type >
	{
		static constexpr VariableType value = VariableType::eMat2x3;
		static constexpr uint32_t cols = 2u;
		static constexpr uint32_t rows = 3u;
		static constexpr uint32_t count = rows * cols;
		static constexpr bool is_matrix = true;
		static constexpr bool is_point = false;
		static constexpr bool is_one = false;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for mat2x4 value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une valeur mat2x4.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eMat2x4b
								|| Type == UniformType::eMat2x4i
								|| Type == UniformType::eMat2x4ui
								|| Type == UniformType::eMat2x4f
								|| Type == UniformType::eMat2x4d >::type >
	{
		static constexpr VariableType value = VariableType::eMat2x4;
		static constexpr uint32_t cols = 2u;
		static constexpr uint32_t rows = 4u;
		static constexpr uint32_t count = rows * cols;
		static constexpr bool is_matrix = true;
		static constexpr bool is_point = false;
		static constexpr bool is_one = false;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for mat3x2 value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une valeur mat3x2.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eMat3x2b
								|| Type == UniformType::eMat3x2i
								|| Type == UniformType::eMat3x2ui
								|| Type == UniformType::eMat3x2f
								|| Type == UniformType::eMat3x2d >::type >
	{
		static constexpr VariableType value = VariableType::eMat3x2;
		static constexpr uint32_t cols = 3u;
		static constexpr uint32_t rows = 2u;
		static constexpr uint32_t count = rows * cols;
		static constexpr bool is_matrix = true;
		static constexpr bool is_point = false;
		static constexpr bool is_one = false;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for mat3x3 value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une valeur mat3x3.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eMat3x3b
								|| Type == UniformType::eMat3x3i
								|| Type == UniformType::eMat3x3ui
								|| Type == UniformType::eMat3x3f
								|| Type == UniformType::eMat3x3d >::type >
	{
		static constexpr VariableType value = VariableType::eMat3x3;
		static constexpr uint32_t cols = 3u;
		static constexpr uint32_t rows = 3u;
		static constexpr uint32_t count = rows * cols;
		static constexpr bool is_matrix = true;
		static constexpr bool is_point = false;
		static constexpr bool is_one = false;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for mat3x4 value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une valeur mat3x4.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eMat3x4b
								|| Type == UniformType::eMat3x4i
								|| Type == UniformType::eMat3x4ui
								|| Type == UniformType::eMat3x4f
								|| Type == UniformType::eMat3x4d >::type >
	{
		static constexpr VariableType value = VariableType::eMat3x4;
		static constexpr uint32_t cols = 3u;
		static constexpr uint32_t rows = 4u;
		static constexpr uint32_t count = rows * cols;
		static constexpr bool is_matrix = true;
		static constexpr bool is_point = false;
		static constexpr bool is_one = false;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for mat4x2 value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une valeur mat4x2.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eMat4x2b
								|| Type == UniformType::eMat4x2i
								|| Type == UniformType::eMat4x2ui
								|| Type == UniformType::eMat4x2f
								|| Type == UniformType::eMat4x2d >::type >
	{
		static constexpr VariableType value = VariableType::eMat4x2;
		static constexpr uint32_t cols = 4u;
		static constexpr uint32_t rows = 2u;
		static constexpr uint32_t count = rows * cols;
		static constexpr bool is_matrix = true;
		static constexpr bool is_point = false;
		static constexpr bool is_one = false;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for mat4x3 value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une valeur mat4x3.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eMat4x3b
								|| Type == UniformType::eMat4x3i
								|| Type == UniformType::eMat4x3ui
								|| Type == UniformType::eMat4x3f
								|| Type == UniformType::eMat4x3d >::type >
	{
		static constexpr VariableType value = VariableType::eMat4x3;
		static constexpr uint32_t cols = 4u;
		static constexpr uint32_t rows = 3u;
		static constexpr uint32_t count = rows * cols;
		static constexpr bool is_matrix = true;
		static constexpr bool is_point = false;
		static constexpr bool is_one = false;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Retrieves the VariableType of a UniformType.
	\remarks	Specialisation for mat4x4 value types.
	\~french
	\brief		Récupère le VariableType d'un UniformType.
	\remarks	Spécialisation pour les types ayant une valeur mat4x4.
	*/
	template< UniformType Type >
	struct variable_type< Type
		, typename std::enable_if< Type == UniformType::eMat4x4b
								|| Type == UniformType::eMat4x4i
								|| Type == UniformType::eMat4x4ui
								|| Type == UniformType::eMat4x4f
								|| Type == UniformType::eMat4x4d >::type >
	{
		static constexpr VariableType value = VariableType::eMat4x4;
		static constexpr uint32_t cols = 4u;
		static constexpr uint32_t rows = 4u;
		static constexpr uint32_t count = rows * cols;
		static constexpr bool is_matrix = true;
		static constexpr bool is_point = false;
		static constexpr bool is_one = false;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Tells if a UniformType is a matrix type.
	\~french
	\brief		Dit si un UniformType est de type matrice.
	*/
	template< UniformType Type >
	struct is_matrix_type
		: std::bool_constant< variable_type< Type >::is_matrix >
	{
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Tells if a UniformType is a point type.
	\~french
	\brief		Dit si un UniformType est de type point.
	*/
	template< UniformType Type >
	struct is_point_type
		: std::bool_constant< variable_type< Type >::is_point >
	{
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		Tells if a UniformType is a single value type.
	\~french
	\brief		Dit si un UniformType est de type valeur simple.
	*/
	template< UniformType Type >
	struct is_one_type
		: std::bool_constant< variable_type< Type >::is_one >
	{
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Retrieves the name of a UniformType.
	\~french
	\brief		Récupère le nom d'un UniformType.
	*/
	template< UniformType Type >
	struct uniform_type_name
	{
		C3D_API static const Castor::String full_type_name;
		C3D_API static const Castor::String data_type_name;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	*/
	template< UniformType Type >
	struct uniform_type;
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
	struct uniform_type< UniformType::eSampler >
	{
		using type = Uniform1s;
		using value_type = int;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eBool >
	{
		using type = Uniform1b;
		using value_type = bool;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eVec2b >
	{
		using type = Uniform2b;
		using value_type = Castor::Coords2b;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eVec3b >
	{
		using type = Uniform3b;
		using value_type = Castor::Coords3b;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eVec4b >
	{
		using type = Uniform4b;
		using value_type = Castor::Coords4b;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eMat2x2b >
	{
		using type = Uniform2x2b;
		using value_type = Castor::Matrix2x2b;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eMat2x3b >
	{
		using type = Uniform2x3b;
		using value_type = Castor::Matrix2x3b;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eMat2x4b >
	{
		using type = Uniform2x4b;
		using value_type = Castor::Matrix2x4b;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eMat3x2b >
	{
		using type = Uniform3x2b;
		using value_type = Castor::Matrix3x2b;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eMat3x3b >
	{
		using type = Uniform3x3b;
		using value_type = Castor::Matrix3x3b;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eMat3x4b >
	{
		using type = Uniform3x4b;
		using value_type = Castor::Matrix3x4b;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eMat4x2b >
	{
		using type = Uniform4x2b;
		using value_type = Castor::Matrix4x2b;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eMat4x3b >
	{
		using type = Uniform4x3b;
		using value_type = Castor::Matrix4x3b;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eMat4x4b >
	{
		using type = Uniform4x4b;
		using value_type = Castor::Matrix4x4b;
		using value_sub_type = bool;
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
	struct uniform_type< UniformType::eInt >
	{
		using type = Uniform1i;
		using value_type = int;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eVec2i >
	{
		using type = Uniform2i;
		using value_type = Castor::Coords2i;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eVec3i >
	{
		using type = Uniform3i;
		using value_type = Castor::Coords3i;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eVec4i >
	{
		using type = Uniform4i;
		using value_type = Castor::Coords4i;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eMat2x2i >
	{
		using type = Uniform2x2i;
		using value_type = Castor::Matrix2x2i;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eMat2x3i >
	{
		using type = Uniform2x3i;
		using value_type = Castor::Matrix2x3i;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eMat2x4i >
	{
		using type = Uniform2x4i;
		using value_type = Castor::Matrix2x4i;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eMat3x2i >
	{
		using type = Uniform3x2i;
		using value_type = Castor::Matrix3x2i;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eMat3x3i >
	{
		using type = Uniform3x3i;
		using value_type = Castor::Matrix3x3i;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eMat3x4i >
	{
		using type = Uniform3x4i;
		using value_type = Castor::Matrix3x4i;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eMat4x2i >
	{
		using type = Uniform4x2i;
		using value_type = Castor::Matrix4x2i;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eMat4x3i >
	{
		using type = Uniform4x3i;
		using value_type = Castor::Matrix4x3i;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eMat4x4i >
	{
		using type = Uniform4x4i;
		using value_type = Castor::Matrix4x4i;
		using value_sub_type = int;
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
	struct uniform_type< UniformType::eUInt >
	{
		using type = Uniform1ui;
		using value_type = uint32_t;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eVec2ui >
	{
		using type = Uniform2ui;
		using value_type = Castor::Coords2ui;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eVec3ui >
	{
		using type = Uniform3ui;
		using value_type = Castor::Coords3ui;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eVec4ui >
	{
		using type = Uniform4ui;
		using value_type = Castor::Coords4ui;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eMat2x2ui >
	{
		using type = Uniform2x2ui;
		using value_type = Castor::Matrix2x2ui;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eMat2x3ui >
	{
		using type = Uniform2x3ui;
		using value_type = Castor::Matrix2x3ui;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eMat2x4ui >
	{
		using type = Uniform2x4ui;
		using value_type = Castor::Matrix2x4ui;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eMat3x2ui >
	{
		using type = Uniform3x2ui;
		using value_type = Castor::Matrix3x2ui;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eMat3x3ui >
	{
		using type = Uniform3x3ui;
		using value_type = Castor::Matrix3x3ui;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eMat3x4ui >
	{
		using type = Uniform3x4ui;
		using value_type = Castor::Matrix3x4ui;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eMat4x2ui >
	{
		using type = Uniform4x2ui;
		using value_type = Castor::Matrix4x2ui;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eMat4x3ui >
	{
		using type = Uniform4x3ui;
		using value_type = Castor::Matrix4x3ui;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eMat4x4ui >
	{
		using type = Uniform4x4ui;
		using value_type = Castor::Matrix4x4ui;
		using value_sub_type = uint32_t;
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
	struct uniform_type< UniformType::eFloat >
	{
		using type = Uniform1f;
		using value_type = float;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eVec2f >
	{
		using type = Uniform2f;
		using value_type = Castor::Coords2f;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eVec3f >
	{
		using type = Uniform3f;
		using value_type = Castor::Coords3f;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eVec4f >
	{
		using type = Uniform4f;
		using value_type = Castor::Coords4f;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eMat2x2f >
	{
		using type = Uniform2x2f;
		using value_type = Castor::Matrix2x2f;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eMat2x3f >
	{
		using type = Uniform2x3f;
		using value_type = Castor::Matrix2x3f;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eMat2x4f >
	{
		using type = Uniform2x4f;
		using value_type = Castor::Matrix2x4f;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eMat3x2f >
	{
		using type = Uniform3x2f;
		using value_type = Castor::Matrix3x2f;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eMat3x3f >
	{
		using type = Uniform3x3f;
		using value_type = Castor::Matrix3x3f;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eMat3x4f >
	{
		using type = Uniform3x4f;
		using value_type = Castor::Matrix3x4f;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eMat4x2f >
	{
		using type = Uniform4x2f;
		using value_type = Castor::Matrix4x2f;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eMat4x3f >
	{
		using type = Uniform4x3f;
		using value_type = Castor::Matrix4x3f;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eMat4x4f >
	{
		using type = Uniform4x4f;
		using value_type = Castor::Matrix4x4f;
		using value_sub_type = float;
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
	struct uniform_type< UniformType::eDouble >
	{
		using type = Uniform1d;
		using value_type = double;
		using value_sub_type = double;
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
	struct uniform_type< UniformType::eVec2d >
	{
		using type = Uniform2d;
		using value_type = Castor::Coords2d;
		using value_sub_type = double;
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
	struct uniform_type< UniformType::eVec3d >
	{
		using type = Uniform3d;
		using value_type = Castor::Coords3d;
		using value_sub_type = double;
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
	struct uniform_type< UniformType::eVec4d >
	{
		using type = Uniform4d;
		using value_type = Castor::Coords4d;
		using value_sub_type = double;
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
	struct uniform_type< UniformType::eMat2x2d >
	{
		using type = Uniform2x2d;
		using value_type = Castor::Matrix2x2d;
		using value_sub_type = double;
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
	struct uniform_type< UniformType::eMat2x3d >
	{
		using type = Uniform2x3d;
		using value_type = Castor::Matrix2x3d;
		using value_sub_type = double;
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
	struct uniform_type< UniformType::eMat2x4d >
	{
		using type = Uniform2x4d;
		using value_type = Castor::Matrix2x4d;
		using value_sub_type = double;
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
	struct uniform_type< UniformType::eMat3x2d >
	{
		using type = Uniform3x2d;
		using value_type = Castor::Matrix3x2d;
		using value_sub_type = double;
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
	struct uniform_type< UniformType::eMat3x3d >
	{
		using type = Uniform3x3d;
		using value_type = Castor::Matrix3x3d;
		using value_sub_type = double;
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
	struct uniform_type< UniformType::eMat3x4d >
	{
		using type = Uniform3x4d;
		using value_type = Castor::Matrix3x4d;
		using value_sub_type = double;
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
	struct uniform_type< UniformType::eMat4x2d >
	{
		using type = Uniform4x2d;
		using value_type = Castor::Matrix4x2d;
		using value_sub_type = double;
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
	struct uniform_type< UniformType::eMat4x3d >
	{
		using type = Uniform4x3d;
		using value_type = Castor::Matrix4x3d;
		using value_sub_type = double;
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
	struct uniform_type< UniformType::eMat4x4d >
	{
		using type = Uniform4x4d;
		using value_type = Castor::Matrix4x4d;
		using value_sub_type = double;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Retrieves the parameter or return type of a SetValue and GetValue functions.
	\~french
	\brief		Récupère le type de paramètre ou de retour pour les fonctions SetValue et GetValue.
	*/
	template< UniformType Type, typename Enable = void >
	struct uniform_param_type;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Retrieves the parameter or return type of a SetValue and GetValue functions.
	\remarks	Specialisation for non point types.
	\~french
	\brief		Récupère le type de paramètre ou de retour pour les fonctions SetValue et GetValue.
	\remarks	Spécialisation pour les types non point.
	*/
	template< UniformType Type >
	struct uniform_param_type< Type, typename std::enable_if< !is_point_type< Type >::value >::type >
	{
		using type = typename uniform_type< Type >::value_type;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Retrieves the parameter or return type of a SetValue and GetValue functions.
	\remarks	Specialisation for point types.
	\~french
	\brief		Récupère le type de paramètre ou de retour pour les fonctions SetValue et GetValue.
	\remarks	Spécialisation pour les types point.
	*/
	template< UniformType Type >
	struct uniform_param_type< Type, typename std::enable_if< is_point_type< Type >::value >::type >
	{
		using type = typename Castor::Point< typename uniform_type< Type >::value_sub_type, variable_type< Type >::count >;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Retrieves the parameter or return type of a SetValue and GetValue functions.
	\~french
	\brief		Récupère le type de paramètre ou de retour pour les fonctions SetValue et GetValue.
	*/
	template< UniformType Type, typename Enable = void >
	struct uniform_return_type;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Retrieves the parameter or return type of a SetValue and GetValue functions.
	\remarks	Specialisation for non point types.
	\~french
	\brief		Récupère le type de paramètre ou de retour pour les fonctions SetValue et GetValue.
	\remarks	Spécialisation pour les types non point.
	*/
	template< UniformType Type >
	struct uniform_return_type< Type, typename std::enable_if< !is_point_type< Type >::value >::type >
	{
		using type = typename uniform_type< Type >::value_type;
		using const_type = type;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Retrieves the parameter or return type of a SetValue and GetValue functions.
	\remarks	Specialisation for point types.
	\~french
	\brief		Récupère le type de paramètre ou de retour pour les fonctions SetValue et GetValue.
	\remarks	Spécialisation pour les types point.
	*/
	template< UniformType Type >
	struct uniform_return_type< Type, typename std::enable_if< is_point_type< Type >::value >::type >
	{
		using type = typename Castor::Coords< typename uniform_type< Type >::value_sub_type, variable_type< Type >::count >;
		using const_type = typename Castor::Point< typename uniform_type< Type >::value_sub_type, variable_type< Type >::count >;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Retrieves the type of the typed values.
	\~french
	\brief		Récupère le type des valeurs typées.
	*/
	template< UniformType Type, typename Enable = void >
	struct uniform_typed_value;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Retrieves the parameter or return type of a SetValue and GetValue functions.
	\remarks	Specialisation for non single types.
	\~french
	\brief		Récupère le type de paramètre ou de retour pour les fonctions SetValue et GetValue.
	\remarks	Spécialisation pour les types non simples.
	*/
	template< UniformType Type >
	struct uniform_typed_value< Type, typename std::enable_if< !is_one_type< Type >::value >::type >
	{
		using type = typename uniform_type< Type >::value_type;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Retrieves the parameter or return type of a SetValue and GetValue functions.
	\remarks	Specialisation for point types.
	\~french
	\brief		Récupère le type de paramètre ou de retour pour les fonctions SetValue et GetValue.
	\remarks	Spécialisation pour les types point.
	*/
	template< UniformType Type >
	struct uniform_typed_value< Type, typename std::enable_if< is_one_type< Type >::value >::type >
	{
		using type = typename uniform_type< Type >::value_type *;
	};
}

#endif
