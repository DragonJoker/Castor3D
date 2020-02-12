/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinarySkeletonAnimationObject_H___
#define ___C3D_BinarySkeletonAnimationObject_H___

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
	\remarks	Specialisation for SkeletonAnimationObject.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour SkeletonAnimationObject.
	*/
	template<>
	struct ChunkTyper< SkeletonAnimationObject >
	{
		static ChunkType const Value = ChunkType::eAnimationObject;
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		SkeletonAnimationObject binary loader.
	\~english
	\brief		Loader binaire de SkeletonAnimationObject.
	*/
	template<>
	class BinaryWriter< SkeletonAnimationObject >
		: public BinaryWriterBase< SkeletonAnimationObject >
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
		C3D_API bool doWrite( SkeletonAnimationObject const & obj )override;
	};
	/**
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		SkeletonAnimationObject binary loader.
	\~english
	\brief		Loader binaire de SkeletonAnimationObject.
	*/
	template<>
	class BinaryParser< SkeletonAnimationObject >
		: public BinaryParserBase< SkeletonAnimationObject >
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
		C3D_API bool doParse( SkeletonAnimationObject & obj )override;
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk in version 1.1.
		 *\param[out]	obj	The object to read.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk en version 1.1.
		 *\param[out]	obj	L'objet à lire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doParse_v1_1( SkeletonAnimationObject & obj )override;
	};
}

#endif
