/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinarySkinComponent_H___
#define ___C3D_BinarySkinComponent_H___

#include "Castor3D/Binary/BinaryParser.hpp"
#include "Castor3D/Binary/BinaryWriter.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/ComponentModule.hpp"

namespace castor3d
{
	/**
	\author 	Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for SkinComponent.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour SkinComponent.
	*/
	template<>
	struct ChunkTyper< SkinComponent >
	{
		static ChunkType const Value = ChunkType::eBonesComponent;
	};
	/**
	\author		Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		SkinComponent loader.
	\~english
	\brief		Loader de SkinComponent.
	*/
	template<>
	class BinaryWriter< SkinComponent >
		: public BinaryWriterBase< SkinComponent >
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
		C3D_API bool doWrite( SkinComponent const & obj )override;
	};
	/**
	\author		Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		SkinComponent loader.
	\~english
	\brief		Loader de SkinComponent.
	*/
	template<>
	class BinaryParser< SkinComponent >
		: public BinaryParserBase< SkinComponent >
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
		C3D_API bool doParse( SkinComponent & obj )override;
	};
}

#endif
