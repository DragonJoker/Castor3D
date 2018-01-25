/**
*\file
*	ObjLoader.h
*\author
*	Sylvain Doremus
*/
#ifndef ___ObjLoader_HPP___
#define ___ObjLoader_HPP___
#pragma once

#include "Prerequisites.hpp"

namespace common
{
	/**
	*\~french
	*\brief
	*	Les données d'un sommet texturé.
	*\~english
	*\brief
	*	The data for one textured vertex.
	*/
	struct TexturedVertexData
	{
		renderer::Vec4 position;
		renderer::Vec2 uv;
	};
	/**
	*\brief
	*	Charge un objet depuis un fichier OBJ.
	*\param[in] name
	*	Le nom de l'objet.
	*\param[in] fileContent
	*	Le contenu du fichier OBJ.
	*\param[in,out] materialsList
	*	Reçoit les matériaux chargés.
	*\param[in,out] texturesList
	*	Reçoit les textures chargées.
	*\param[in,out] meshesList
	*	Reçoit les maillages chargés.
	*\return
	*	L'objet chargé.
	*/
	void loadObjFile( std::string const & fileContent
		, std::vector< TexturedVertexData > & vboData
		, renderer::UInt16Array & iboData );
}

#endif
