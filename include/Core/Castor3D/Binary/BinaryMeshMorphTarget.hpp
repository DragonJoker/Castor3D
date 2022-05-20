/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinaryMeshMorphTarget_H___
#define ___C3D_BinaryMeshMorphTarget_H___

#include "Castor3D/Binary/BinaryParser.hpp"
#include "Castor3D/Binary/BinaryWriter.hpp"

#include "Castor3D/Model/Mesh/Animation/MeshAnimationModule.hpp"

namespace castor3d
{
	/**
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for MeshMorphTarget.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour MeshMorphTarget.
	*/
	template<>
	struct ChunkTyper< MeshMorphTarget >
	{
		static ChunkType const Value = ChunkType::eMeshMorphTarget;
	};
	/**
	\~english
	\brief		MeshMorphTarget binary loader.
	\~english
	\brief		Loader binaire de MeshMorphTarget.
	*/
	template<>
	class BinaryWriter< MeshMorphTarget >
		: public BinaryWriterBase< MeshMorphTarget >
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
		C3D_API bool doWrite( MeshMorphTarget const & obj )override;
	};
	/**
	\~english
	\brief		MeshMorphTarget binary loader.
	\~english
	\brief		Loader binaire de MeshMorphTarget.
	*/
	template<>
	class BinaryParser< MeshMorphTarget >
		: public BinaryParserBase< MeshMorphTarget >
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
		C3D_API bool doParse( MeshMorphTarget & obj )override;
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
		C3D_API bool doParse_v1_3( MeshMorphTarget & obj )override;
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
		C3D_API bool doParse_v1_5( MeshMorphTarget & obj )override;
	};
}

#endif

