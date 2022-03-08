/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinarySceneNodeAnimationKeyFrame_H___
#define ___C3D_BinarySceneNodeAnimationKeyFrame_H___

#include "Castor3D/Binary/BinaryParser.hpp"
#include "Castor3D/Binary/BinaryWriter.hpp"

#include "Castor3D/Scene/Animation/AnimationModule.hpp"

namespace castor3d
{
	/**
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for SceneNodeAnimationKeyFrame.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour SceneNodeAnimationKeyFrame.
	*/
	template<>
	struct ChunkTyper< SceneNodeAnimationKeyFrame >
	{
		static ChunkType const Value = ChunkType::eSceneNodeAnimationKeyFrame;
	};
	/**
	\~english
	\brief		SceneNodeAnimationKeyFrame binary loader.
	\~english
	\brief		Loader binaire de SceneNodeAnimationKeyFrame.
	*/
	template<>
	class BinaryWriter< SceneNodeAnimationKeyFrame >
		: public BinaryWriterBase< SceneNodeAnimationKeyFrame >
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
		C3D_API bool doWrite( SceneNodeAnimationKeyFrame const & obj )override;
	};
	/**
	\~english
	\brief		SceneNodeAnimationKeyFrame binary loader.
	\~english
	\brief		Loader binaire de SceneNodeAnimationKeyFrame.
	*/
	template<>
	class BinaryParser< SceneNodeAnimationKeyFrame >
		: public BinaryParserBase< SceneNodeAnimationKeyFrame >
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
		C3D_API bool doParse( SceneNodeAnimationKeyFrame & obj )override;
	};
}

#endif

