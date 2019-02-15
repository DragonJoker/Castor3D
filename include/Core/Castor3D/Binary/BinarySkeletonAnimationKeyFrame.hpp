/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinarySkeletonAnimationKeyFrame_H___
#define ___C3D_BinarySkeletonAnimationKeyFrame_H___

#include "Castor3D/Binary/BinaryParser.hpp"
#include "Castor3D/Binary/BinaryWriter.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date 		07/12/2017
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for SkeletonAnimationKeyFrame.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour SkeletonAnimationKeyFrame.
	*/
	template<>
	struct ChunkTyper< SkeletonAnimationKeyFrame >
	{
		static ChunkType const Value = ChunkType::eSkeletonAnimationKeyFrame;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date 		07/12/2017
	\~english
	\brief		SkeletonAnimationKeyFrame binary loader.
	\~english
	\brief		Loader binaire de SkeletonAnimationKeyFrame.
	*/
	template<>
	class BinaryWriter< SkeletonAnimationKeyFrame >
		: public BinaryWriterBase< SkeletonAnimationKeyFrame >
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
		C3D_API bool doWrite( SkeletonAnimationKeyFrame const & obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date 		07/12/2017
	\~english
	\brief		SkeletonAnimationKeyFrame binary loader.
	\~english
	\brief		Loader binaire de SkeletonAnimationKeyFrame.
	*/
	template<>
	class BinaryParser< SkeletonAnimationKeyFrame >
		: public BinaryParserBase< SkeletonAnimationKeyFrame >
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
		C3D_API bool doParse( SkeletonAnimationKeyFrame & obj )override;
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
		C3D_API bool doParse_v1_3( SkeletonAnimationKeyFrame & obj )override;
	};
}

#endif

