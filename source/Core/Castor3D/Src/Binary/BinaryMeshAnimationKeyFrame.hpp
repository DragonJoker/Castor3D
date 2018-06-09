/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinaryMeshAnimationKeyFrame_H___
#define ___C3D_BinaryMeshAnimationKeyFrame_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date 		07/12/2017
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for MeshAnimationKeyFrame.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour MeshAnimationKeyFrame.
	*/
	template<>
	struct ChunkTyper< MeshAnimationKeyFrame >
	{
		static ChunkType const Value = ChunkType::eMeshAnimationKeyFrame;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date 		07/12/2017
	\~english
	\brief		MeshAnimationKeyFrame binary loader.
	\~english
	\brief		Loader binaire de MeshAnimationKeyFrame.
	*/
	template<>
	class BinaryWriter< MeshAnimationKeyFrame >
		: public BinaryWriterBase< MeshAnimationKeyFrame >
	{
	protected:
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
		C3D_API bool doWrite( MeshAnimationKeyFrame const & obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date 		07/12/2017
	\~english
	\brief		MeshAnimationKeyFrame binary loader.
	\~english
	\brief		Loader binaire de MeshAnimationKeyFrame.
	*/
	template<>
	class BinaryParser< MeshAnimationKeyFrame >
		: public BinaryParserBase< MeshAnimationKeyFrame >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk.
		 *\param[out]	obj	The object to read.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk.
		 *\param[out]	obj	L'objet à lire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doParse( MeshAnimationKeyFrame & obj )override;
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk.
		 *\param[out]	obj	The object to read.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk.
		 *\param[out]	obj	L'objet à lire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doParse_v1_3( MeshAnimationKeyFrame & obj )override;
	};
}

#endif

