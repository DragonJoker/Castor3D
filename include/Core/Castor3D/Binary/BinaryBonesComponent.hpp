/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinaryBonesComponent_H___
#define ___C3D_BinaryBonesComponent_H___

#include "Castor3D/Binary/BinaryParser.hpp"
#include "Castor3D/Binary/BinaryWriter.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for BonesComponent.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour BonesComponent.
	*/
	template<>
	struct ChunkTyper< BonesComponent >
	{
		static ChunkType const Value = ChunkType::eBonesComponent;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		BonesComponent loader.
	\~english
	\brief		Loader de BonesComponent.
	*/
	template<>
	class BinaryWriter< BonesComponent >
		: public BinaryWriterBase< BonesComponent >
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
		C3D_API bool doWrite( BonesComponent const & obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		09/11/2017
	\~english
	\brief		BonesComponent loader.
	\~english
	\brief		Loader de BonesComponent.
	*/
	template<>
	class BinaryParser< BonesComponent >
		: public BinaryParserBase< BonesComponent >
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
		C3D_API bool doParse( BonesComponent & obj )override;
	};
}

#endif
