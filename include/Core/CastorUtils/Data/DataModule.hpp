/*
See LICENSE file in root folder
*/
#ifndef ___CU_DataModule_HPP___
#define ___CU_DataModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

namespace castor
{
	/**@name Data */
	//@{
	/**
	\~english
	\brief		castor::Loader supported file types enumeration
	\~french
	\brief		Enumération des types de fichier supportés par castor::Loader
	*/
	enum class FileType
		: uint8_t
	{
		//! Binary file
		eBinary,
		//! Text file
		eText,
		CU_ScopedEnumBounds( eBinary )
	};
	/**
	\~english
	\brief		Helper structure to find File type (BinaryFile or TextFile) from FileType.
	\remarks	Specialisation for FileType::eBinary.
	\~french
	\brief		Structure d'aide pour trouver le type de File (BinaryFile ou TextFile) à partir d'un FileType.
	\remarks	Spécialisation pour FileType::eBinary.
	*/
	template< FileType FileTypeT >
	struct FileTyper;
	/**
	\~english
	\brief		User friendly File class
	\remark		Adds some static functions to check file/directory validity, file/directory creation...
	\~french
	\brief		Classe de gestion de fichier
	\remark		Ajoute quelques fonctions statiques de vérification d'existence, suppression, ...
	*/
	class BinaryFile;
	/**
	\~english
	\brief		Text file class
	\~french
	\brief		Classe de gestion de fichier texte
	*/
	class TextFile;
	/*!
	\~english
	\brief		User friendly File class
	\remark		Adds some static functions to check file/directory validity, file/directory creation...
	\~french
	\brief		Classe de gestion de fichier
	\remark		Ajoute quelques fonctions statiques de vérification d'existence, suppression, ...
	*/
	class File;
	/**
	\~english
	\brief		Partial castor::Loader specialisation for binary files
	\~french
	\brief		Spécialisation partielle de castor::Loader, pour les fichiers binaires
	*/
	template< class T >
	class BinaryWriter;
	/**
	\~english
	\brief		Partial castor::Writer specialisation for text files
	\~french
	\brief		Spécialisation partielle de castor::Writer, pour les fichiers texte
	*/
	class TextWriterBase;
	/**
	\~english
	\brief		Partial castor::Writer specialisation for text files
	\~french
	\brief		Spécialisation partielle de castor::Writer, pour les fichiers texte
	*/
	template< class T >
	class TextWriterT;
	/**
	\~english
	\brief		Main resource writer class.
	\~french
	\brief		Classe de base pour les writers de ressource.
	*/
	template< class T, FileType FileTypeT >
	class Writer;
	/**
	\~english
	\brief		Partial castor::Loader specialisation for binary files
	\~french
	\brief		Spécialisation partielle de castor::Loader, pour les fichiers binaires
	*/
	template< class T >
	class BinaryLoader;
	/**
	\~english
	\brief		Main resource loader class.
	\remarks	Holds the two functions needed for a resource loader : Load and Save.
	\~french
	\brief		Classe de base pour les loaders de ressource.
	\remarks	Contient les 2 fonctions nécessaire a un loader : Load et Save.
	*/
	template< class T, FileType FileTypeT >
	class Loader;
	/**
	\~english
	\brief		Resource loading exception
	\remark		Thrown if the resource loader has encountered an error
	\~french
	\brief		Resource loading exception
	\remark		Lancée si un Loader rencontre un erreur
	*/
	class LoaderException;
	/**
	\~english
	\brief		Path management class
	\remark		Defines platform dependant paths.
	\~french
	\brief		Classe de gestion de chemin
	\remark		Définit un chemin de fichier.
	*/
	class Path;
	/**
	\~english
	\brief		Zip archive class
	\~french
	\brief		Classe de gestion d'archive zip
	*/
	class ZipArchive;

	CU_DeclareVector( Path, Path );
}

#endif
