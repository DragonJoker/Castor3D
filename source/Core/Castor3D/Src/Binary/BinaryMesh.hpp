/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinaryMesh_H___
#define ___C3D_BinaryMesh_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for Mesh.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour Mesh.
	*/
	template<>
	struct ChunkTyper< Mesh >
	{
		static ChunkType const Value = ChunkType::eMesh;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		MovableObject loader
	\~english
	\brief		Loader de MovableObject
	*/
	template<>
	class BinaryWriter< Mesh >
		: public BinaryWriterBase< Mesh >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to fill the chunk from specific data.
		 *\param[in]	obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doWrite( Mesh const & obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		MovableObject loader
	\~english
	\brief		Loader de MovableObject
	*/
	template<>
	class BinaryParser< Mesh >
		: public BinaryParserBase< Mesh >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk
		 *\param[out]	obj	The object to read
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
		 *\param[out]	obj	L'objet à lire
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool doParse( Mesh & obj )override;
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk
		 *\param[out]	obj	The object to read
		 *\return		\p false if any error occured
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
		 *\param[out]	obj	L'objet à lire
		 *\return		\p false si une erreur quelconque est arrivée
		 */
		C3D_API bool doParse_v1_2( Mesh & obj )override;
	};
}

#endif
