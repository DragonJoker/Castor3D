/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinarySkeletonAnimationNode_H___
#define ___C3D_BinarySkeletonAnimationNode_H___

#include "Castor3D/Binary/BinaryParser.hpp"
#include "Castor3D/Binary/BinaryWriter.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationModule.hpp"

namespace castor3d
{
	/**
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for SkeletonAnimationNode.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour SkeletonAnimationNode.
	*/
	template<>
	struct ChunkTyper< SkeletonAnimationNode >
	{
		static ChunkType const Value = ChunkType::eSkeletonAnimationNode;
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MovingBone binary loader.
	\~english
	\brief		Loader binaire de MovingBone.
	*/
	template<>
	class BinaryWriter< SkeletonAnimationNode >
		: public BinaryWriterBase< SkeletonAnimationNode >
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
		C3D_API bool doWrite( SkeletonAnimationNode const & obj )override;
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MovingBone binary loader.
	\~english
	\brief		Loader binaire de MovingBone.
	*/
	template<>
	class BinaryParser< SkeletonAnimationNode >
		: public BinaryParserBase< SkeletonAnimationNode >
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
		C3D_API bool doParse( SkeletonAnimationNode & obj )override;
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
		C3D_API bool doParse_v1_5( SkeletonAnimationNode & obj )override;
	};
}

#endif
