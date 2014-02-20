/*
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
#ifndef ___C3D_Face___
#define ___C3D_Face___

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Face implementation
	\remark		A face is constituted from 3 vertices
	\~french
	\brief		Implémentation d'un face
	\remark		Une face est constituée de 3 vertices
	*/
	class C3D_API Face
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Face loader
		\~french
		\brief		Loader de Face
		*/
		class C3D_API TextLoader : public Castor::Loader< Face, Castor::eFILE_TYPE_TEXT, Castor::TextFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode=Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief		Writes a face into a text file
			 *\param[in]	p_face	The face to write
			 *\param[out]	p_file	The file where to write the face
			 *\~french
			 *\brief		Ecrit une face dans un fichier texte
			 *\param[in]	p_face	La face à écrire
			 *\param[out]	p_file	Le fichier où écrire la face
			 */
			virtual bool operator ()( Face const & p_face, Castor::TextFile & p_file );
		};

	private:
		uint32_t	m_pIndex[3];

	public:
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	a, b, c	The three vertices indices
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	a, b, c	Les indices des 3 vertices
		 */
		Face( uint32_t a, uint32_t b, uint32_t c );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_object	L'objet à copier
		 */
		 Face( Face const & p_object );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_object	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 */
		 Face( Face && p_object );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_object	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_object	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		 Face & operator =( Face const & p_object );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_object	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		 Face & operator =( Face && p_object );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Face();
		/**
		 *\~english
		 *\brief		Retrieves the vertex index
		 *\param[in]	p_uiIndex	The index of the concerned vertex
		 *\return		The vertex index
		 *\~french
		 *\brief		Récupère l'indice du vertex voulu
		 *\param[in]	p_uiIndex	L'index du vertex concerné
		 *\return		L'indice
		 */
		inline uint32_t GetVertexIndex( uint32_t p_uiIndex )const { CASTOR_ASSERT( p_uiIndex < 3 ); return m_pIndex[p_uiIndex]; }
	};
}

#pragma warning( pop )

#endif
