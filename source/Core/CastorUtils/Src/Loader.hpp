﻿/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CASTOR_LOADER_H___
#define ___CASTOR_LOADER_H___

#include "Exception.hpp"
#include "Resource.hpp"
#include "TextFile.hpp"
#include "BinaryFile.hpp"
#include "LoaderException.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Main resource loader class
	\remark		Holds the two functions needed for a resource loader : Load and Save
	\~french
	\brief		Classe de base pour les loaders de ressource
	\remark		Contient les 2 fonctions nécessaire a un loader : Load et Save
	*/
	template< class T, eFILE_TYPE FT, class TFile >
	class Loader
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eMode	The file open mode
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eMode	Le mode d'ouverture du fichier
		 */
		Loader( File::eOPEN_MODE p_eMode, File::eENCODING_MODE p_eEncodingMode = File::eENCODING_MODE_ASCII )
			:	m_eOpenMode( p_eMode	)
			,	m_eEncodingMode( p_eEncodingMode	)
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Loader()
		{
		}
		/**
		 *\~english
		 *\brief			Reads a resource from a file
		 *\param[in,out]	p_object	The object to read
		 *\param[in]		p_pathFile	The path of file where to read the object
		 *\~french
		 *\brief			Lit une ressource à partir d'un fichier
		 *\param[in,out]	p_object	L'objet à lire
		 *\param[in]		p_pathFile	Le chemin du fichier où lire l'objet
		 */
		virtual bool operator()( T & p_object, Path const & p_pathFile )
		{
			TFile l_file( p_pathFile, File::eOPEN_MODE_READ | m_eOpenMode, m_eEncodingMode );
			return operator()( p_object, l_file );
		}
		/**
		 *\~english
		 *\brief			Reads a resource from a file
		 *\param[in,out]	p_object	The object to read
		 *\param[in,out]	p_file		The file where to read the object
		 *\~french
		 *\brief			Lit une ressource à partir d'un fichier
		 *\param[in,out]	p_object	L'objet à lire
		 *\param[in,out]	p_file		Le fichier où lire l'objet
		 */
		virtual bool operator()( T & CU_PARAM_UNUSED( p_object ), TFile & CU_PARAM_UNUSED( p_file ) )
		{
			LOADER_ERROR( "Import not supported by the loader registered for this type" );
		}
		/**
		 *\~english
		 *\brief		Writes a resource to a file
		 *\param[in]	p_object	The object to write
		 *\param[in]	p_pathFile	The path of file where to write the object
		 *\~french
		 *\brief		Ecrit une ressource dans un fichier
		 *\param[in]	p_object	L'objet à écrire
		 *\param[in]	p_pathFile	Le chemin du fichier où écrire l'objet
		 */
		virtual bool operator()( T const & p_object, Path const & p_pathFile )
		{
			TFile l_file( p_pathFile, File::eOPEN_MODE_WRITE | m_eOpenMode, m_eEncodingMode );
			return operator()( p_object, l_file );
		}
		/**
		 *\~english
		 *\brief			Writes a resource to a file
		 *\param[in]		p_object	The object to write
		 *\param[in,out]	p_file		The file where to write the object
		 *\~french
		 *\brief			Ecrit une ressource dans un fichier
		 *\param[in]		p_object	L'objet à écrire
		 *\param[in,out]	p_file		Le fichier où écrire l'objet
		 */
		virtual bool operator()( T const & CU_PARAM_UNUSED( p_object ), TFile & CU_PARAM_UNUSED( p_file ) )
		{
			LOADER_ERROR( "Export not supported by the loader registered for this type" );
		}

	private:
		File::eOPEN_MODE m_eOpenMode;
		File::eENCODING_MODE m_eEncodingMode;
	};
}

#endif
