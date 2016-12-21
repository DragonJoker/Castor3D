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
		: std::integral_constant< bool, variable_type< Type >::is_matrix >
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
		: std::integral_constant< bool, variable_type< Type >::is_point >
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
		: std::integral_constant< bool, variable_type< Type >::is_one >
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
	\brief		Helper structure to retrieve a sub type from a UniformType.
	\~french
	\brief		Structure d'aide permettant de récupérer un sous type depuis un UniformType.
	*/
	template< UniformType Type, typename Enable = void >
	struct uniform_sub_type;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a sub type from a UniformType.
	\remarks	Specialisation for UniformType::eSampler.
	\~french
	\brief		Structure d'aide permettant de récupérer un sous type depuis un UniformType.
	\remarks	Spécialisation pour UniformType::eSampler.
	\remarks
	*/
	template< UniformType Type >
	struct uniform_sub_type < Type
		, typename std::enable_if< Type == UniformType::eSampler >::type >
	{
		using type = int;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for bool types.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour les types bool.
	\remarks
	*/
	template< UniformType Type >
	struct uniform_sub_type< Type
		, typename std::enable_if< Type == UniformType::eBool
								|| Type == UniformType::eVec2b
								|| Type == UniformType::eVec3b
								|| Type == UniformType::eVec4b
								|| Type == UniformType::eMat2x2b
								|| Type == UniformType::eMat2x3b
								|| Type == UniformType::eMat2x4b
								|| Type == UniformType::eMat3x2b
								|| Type == UniformType::eMat3x3b
								|| Type == UniformType::eMat3x4b
								|| Type == UniformType::eMat4x2b
								|| Type == UniformType::eMat4x3b
								|| Type == UniformType::eMat4x4b >::type >
	{
		using type = bool;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for int types.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour les types int.
	\remarks
	*/
	template< UniformType Type >
	struct uniform_sub_type< Type
		, typename std::enable_if< Type == UniformType::eInt
								|| Type == UniformType::eVec2i
								|| Type == UniformType::eVec3i
								|| Type == UniformType::eVec4i
								|| Type == UniformType::eMat2x2i
								|| Type == UniformType::eMat2x3i
								|| Type == UniformType::eMat2x4i
								|| Type == UniformType::eMat3x2i
								|| Type == UniformType::eMat3x3i
								|| Type == UniformType::eMat3x4i
								|| Type == UniformType::eMat4x2i
								|| Type == UniformType::eMat4x3i
								|| Type == UniformType::eMat4x4i >::type >
	{
		using type = int;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for uint types.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour les types uint.
	\remarks
	*/
	template< UniformType Type >
	struct uniform_sub_type< Type
		, typename std::enable_if< Type == UniformType::eUInt
								|| Type == UniformType::eVec2ui
								|| Type == UniformType::eVec3ui
								|| Type == UniformType::eVec4ui
								|| Type == UniformType::eMat2x2ui
								|| Type == UniformType::eMat2x3ui
								|| Type == UniformType::eMat2x4ui
								|| Type == UniformType::eMat3x2ui
								|| Type == UniformType::eMat3x3ui
								|| Type == UniformType::eMat3x4ui
								|| Type == UniformType::eMat4x2ui
								|| Type == UniformType::eMat4x3ui
								|| Type == UniformType::eMat4x4ui >::type >
	{
		using type = uint32_t;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for uint types.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour les types uint.
	\remarks
	*/
	template< UniformType Type >
	struct uniform_sub_type< Type
		, typename std::enable_if< Type == UniformType::eFloat
								|| Type == UniformType::eVec2f
								|| Type == UniformType::eVec3f
								|| Type == UniformType::eVec4f
								|| Type == UniformType::eMat2x2f
								|| Type == UniformType::eMat2x3f
								|| Type == UniformType::eMat2x4f
								|| Type == UniformType::eMat3x2f
								|| Type == UniformType::eMat3x3f
								|| Type == UniformType::eMat3x4f
								|| Type == UniformType::eMat4x2f
								|| Type == UniformType::eMat4x3f
								|| Type == UniformType::eMat4x4f >::type >
	{
		using type = float;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for uint types.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour les types uint.
	\remarks
	*/
	template< UniformType Type >
	struct uniform_sub_type< Type
		, typename std::enable_if< Type == UniformType::eDouble
								|| Type == UniformType::eVec2d
								|| Type == UniformType::eVec3d
								|| Type == UniformType::eVec4d
								|| Type == UniformType::eMat2x2d
								|| Type == UniformType::eMat2x3d
								|| Type == UniformType::eMat2x4d
								|| Type == UniformType::eMat3x2d
								|| Type == UniformType::eMat3x3d
								|| Type == UniformType::eMat3x4d
								|| Type == UniformType::eMat4x2d
								|| Type == UniformType::eMat4x3d
								|| Type == UniformType::eMat4x4d >::type >
	{
		using type = double;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a matrix type from a UniformType.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de matrice depuis un UniformType.
	*/
	template< typename T, size_t Columns, size_t Rows, typename Enable = void >
	struct uniform_matrix_type;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a matrix type from a UniformType.
	\remarks	Specialisation for non square matrices.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de matrice depuis un UniformType.
	\remarks	Spécialisation pour les matrices non carrées.
	*/
	template< typename T, size_t Columns, size_t Rows >
	struct uniform_matrix_type< T, Columns, Rows, typename std::enable_if< Columns != Rows >::type >
	{
		using type = typename Castor::Matrix< T, Columns, Rows >;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a matrix type from a UniformType.
	\remarks	Specialisation for square matrices.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de matrice depuis un UniformType.
	\remarks	Spécialisation pour les matrices carrées.
	*/
	template< typename T, size_t Columns, size_t Rows >
	struct uniform_matrix_type< T, Columns, Rows, typename std::enable_if< Columns == Rows >::type >
	{
		using type = typename Castor::SquareMatrix< T, Columns >;
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
	template< UniformType Type, typename Enable = void >
	struct uniform_type;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for matrix types.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour les types matrices.
	\remarks
	*/
	template< UniformType Type >
	struct uniform_type< Type, typename std::enable_if< is_matrix_type< Type >::value >::type >
	{
		using type = TUniform< Type >;
		using value_sub_type = typename uniform_sub_type< Type >::type;
		using value_type = typename uniform_matrix_type< value_sub_type, variable_type< Type >::cols, variable_type< Type >::rows >::type;
		using param_type = value_type;
		using return_type = value_type;
		using return_const_type = param_type;
		using typed_value_type = value_type;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for point types.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour les types points.
	\remarks
	*/
	template< UniformType Type >
	struct uniform_type< Type, typename std::enable_if< is_point_type< Type >::value >::type >
	{
		using type = TUniform< Type >;
		using value_sub_type = typename uniform_sub_type< Type >::type;
		using value_type = typename Castor::Coords< value_sub_type, variable_type< Type >::count >;
		using param_type = typename Castor::Point< value_sub_type, variable_type< Type >::count >;
		using return_type = value_type;
		using return_const_type = param_type;
		using typed_value_type = value_type;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/06/2016
	\~english
	\brief		Helper structure to retrieve a frame variable type from a UniformType.
	\remarks	Specialisation for single types.
	\~french
	\brief		Structure d'aide permettant de récupérer un type de frame variable depuis un UniformType.
	\remarks	Spécialisation pour les types simples.
	\remarks
	*/
	template< UniformType Type >
	struct uniform_type< Type, typename std::enable_if< is_one_type< Type >::value >::type >
	{
		using type = TUniform< Type >;
		using value_sub_type = typename uniform_sub_type< Type >::type;
		using value_type = value_sub_type;
		using param_type = value_type;
		using return_type = value_type;
		using return_const_type = param_type;
		using typed_value_type = value_type *;
	};
}

#endif
