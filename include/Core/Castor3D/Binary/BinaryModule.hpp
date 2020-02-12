/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BinaryModule_H___
#define ___C3D_BinaryModule_H___

#include "Castor3D/Castor3DModule.hpp"

namespace castor3d
{
	/**@name Binary */
	//@{

	/**
	*\~english
	*\brief
	*	Binary data chunk base class
	*\~french
	*\brief
	*	Classe de base d'un chunk de données binaires
	*/
	class BinaryChunk;
	/**
	*\~english
	*\brief
	*	Binary data chunk reader base class
	*\~french
	*\brief
	*	Classe de base de lecture d'un chunk de données binaires
	*/
	template< class TParsed >
	class BinaryParserBase;
	/**
	*\~english
	*\brief
	*	Specialisable binary data chunk reader class.
	*\~french
	*\brief
	*	Classe spécialisable de lecture d'un chunk de données binaires.
	*/
	template< class TParsed >
	class BinaryParser;
	/**
	*\~english
	*\brief
	*	Binary data chunk writer base class.
	*\~french
	*\brief
	*	Classe de base d'écriture d'un chunk de données binaires.
	*/
	template< class TWritten >
	class BinaryWriterBase;
	/**
	*\~english
	*\brief
	*	Specialisable binary data chunk writer class.
	*\~french
	*\brief
	*	Classe spécialisable d'écriture d'un chunk de données binaires.
	*/
	template< class TWritten >
	class BinaryWriter;
	/**
	*\~english
	*\brief
	*	Chunk parser
	*\~french
	*\brief
	*	Analyseur de chunk
	*/
	class ChunkParserBase;
	/**
	*\~english
	*\brief
	*	Helper structure to find ChunkType from a type.
	*\~french
	*\brief
	*	Classe d'aide pour récupéer un ChunkType depuis un type.
	*/
	template< class Type >
	struct ChunkTyper;
	/**
	*\~english
	*\brief
	*	Chunk writer
	*\~french
	*\brief
	*	Remplisseur de chunk
	*/
	class ChunkWriterBase;
	/**
	*\~english
	*\brief
	*	Used to import meshes from cmsh files.
	*\~french
	*\brief
	*	Utilisé pour importer les maillages depuis des fichiers cmsh.
	*/
	class CmshImporter;

	/**@name Version <= 1.3 */
	//@{

	template< typename T >
	struct OldInterleavedVertexT;

	//@}
}

#endif
