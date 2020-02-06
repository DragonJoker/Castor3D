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
#ifndef ___C3D_BILLBOARD_LIST_H___
#define ___C3D_BILLBOARD_LIST_H___

#include "MovableObject.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0
	\date		04/11/2013
	\~english
	\brief		Billboards list
	\remark		All billboards from this list shares the same texture
	\~french
	\brief		Liste de billboards
	\remark		Tous les billboards de cette liste ont la m�me texture
	*/
	class C3D_API BillboardList
		:	public MovableObject
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Camera loader
		\~french
		\brief		Loader de Camera
		*/
		class C3D_API TextLoader
			:	public MovableObject::TextLoader
		{
		public:
			/**
			 *\~english
			 *\brief		Writes a billboards list into a text file
			 *\param[in]	p_file	The file to save the object in
			 *\param[in]	p_obj	The object to save
			 *\~french
			 *\brief		Ecrit une cam�ra dans un fichier texte
			 *\param[in]	p_file	Le fichier
			 *\param[in]	p_obj	L'objet
			 */
			virtual bool operator()( BillboardList const & p_obj, Castor::TextFile & p_file );
		};

		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		BillboardList loader
		\~english
		\brief		Loader de BillboardList
		*/
		class C3D_API BinaryParser
			:	public MovableObject::BinaryParser
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'acc�s au dossier courant
			 */
			BinaryParser( Castor::Path const & p_path );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilis�e afin de remplir le chunk de donn�es sp�cifiques
			 *\param[in]	p_obj	L'objet � �crire
			 *\param[out]	p_chunk	Le chunk � remplir
			 *\return		\p false si une erreur quelconque est arriv�e
			 */
			virtual bool Fill( BillboardList const & p_obj, BinaryChunk & p_chunk )const;
			/**
			 *\~english
			 *\brief		Function used to retrieve specific data from the chunk
			 *\param[out]	p_obj	The object to read
			 *\param[in]	p_chunk	The chunk containing data
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilis�e afin de r�cup�rer des donn�es sp�cifiques � partir d'un chunk
			 *\param[out]	p_obj	L'objet � lire
			 *\param[in]	p_chunk	Le chunk contenant les donn�es
			 *\return		\p false si une erreur quelconque est arriv�e
			 */
			virtual bool Parse( BillboardList & p_obj, BinaryChunk & p_chunk )const;
		};
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pScene		The parent scene
		 *\param[in]	p_pRenderSystem	The RenderSystem
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pScene		La sc�ne parente
		 *\param[in]	p_pRenderSystem	Le RenderSystem
		 */
		BillboardList( Scene * p_pScene, RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~BillboardList();
		/**
		 *\~english
		 *\brief		Initialises GPU side elements
		 *\return		\p true if all is OK
		 *\~french
		 *\brief		Initialise les �l�ments GPU
		 *\return		\p true si tout s'est bien pass�
		 */
		bool Initialise();
		/**
		 *\~english
		 *\brief		Cleans GPU side elements up
		 *\~french
		 *\brief		Nettoie les �l�ments GPU
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Gets a point from the list
		 *\param[in]	p_uiIndex	The point index
		 *\return		The point
		 *\~french
		 *\brief		R�cup�re un point de la liste
		 *\param[in]	p_uiIndex	L'index du point
		 *\return		Le point
		 */
		Castor::Point3r const & GetAt( uint32_t p_uiIndex )const
		{
			return m_arrayPositions[p_uiIndex];
		}
		/**
		 *\~english
		 *\brief		Gets the list size
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re la taille de la liste
		 *\return		La valeur
		 */
		uint32_t GetCount()const
		{
			return uint32_t( m_arrayPositions.size() );
		}
		/**
		 *\~english
		 *\brief		Sets a point in the list
		 *\param[in]	p_uiIndex		The point index
		 *\param[in]	p_ptPosition	The point
		 *\~french
		 *\brief		D�finit un point de la liste
		 *\param[in]	p_uiIndex		L'index du point
		 *\param[in]	p_ptPosition	Le point
		 */
		void SetAt( uint32_t p_uiIndex, Castor::Point3r const & p_ptPosition )
		{
			m_bNeedUpdate = true;
			m_arrayPositions[p_uiIndex] = p_ptPosition;
		}
		/**
		 *\~english
		 *\brief		Retrieves the material
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re le mat�riau
		 *\return		La valeur
		 */
		inline MaterialSPtr GetMaterial()const
		{
			return m_wpMaterial.lock();
		}
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_pMaterial	The new value
		 *\~french
		 *\brief		D�finit le materiau
		 *\param[in]	p_pMaterial	La nouvelle valeur
		 */
		void SetMaterial( MaterialSPtr p_pMaterial );
		/**
		 *\~english
		 *\brief		Removes a point from the list
		 *\param[in]	p_uiIndex	The point index
		 *\~french
		 *\brief		Retire un point de la liste
		 *\param[in]	p_uiIndex	L'index du point
		 */
		void RemovePoint( uint32_t p_uiIndex );
		/**
		 *\~english
		 *\brief		Adds a point to the list
		 *\param[in]	p_ptPosition	The point
		 *\~french
		 *\brief		Ajoute un point � la liste
		 *\param[in]	p_ptPosition	Le point
		 */
		void AddPoint( Castor::Point3r const & p_ptPosition );
		/**
		 *\~english
		 *\brief		Adds a points list to the list
		 *\param[in]	p_ptPositions	The points list
		 *\~french
		 *\brief		Ajoute une liste de points � la liste
		 *\param[in]	p_ptPositions	La liste de points
		 */
		void AddPoints( Castor::Point3rArray const & p_ptPositions );
		/**
		 *\~english
		 *\brief		Renders the billboards
		 *\~french
		 *\brief		Rend les billboards
		 */
		void Render();
		/**
		 *\~english
		 *\brief		Retrieves the billboards dimensions
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re les dimensios des billboards
		 *\return		La valeur
		 */
		inline Castor::Size const & GetDimensions()const
		{
			return m_dimensions;
		}
		/**
		 *\~english
		 *\brief		Sets the billboards dimensions
		 *\param[in]	p_dimensions	The new value
		 *\~french
		 *\brief		D�finit les dimensios des billboards
		 *\param[in]	p_dimensions	La nouvelle valeur
		 */
		void SetDimensions( Castor::Size const & p_dimensions );
		/**
		 *\~english
		 *\brief		Gets an iterator to the beginning of the list
		 *\return		The iterator
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but de la liste
		 *\return		L'it�rateur
		 */
		Castor::Point3rArrayIt Begin()
		{
			return m_arrayPositions.begin();
		}
		/**
		 *\~english
		 *\brief		Gets an iterator to the beginning of the list
		 *\return		The iterator
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le d�but de la liste
		 *\return		L'it�rateur
		 */
		Castor::Point3rArrayConstIt Begin()const
		{
			return m_arrayPositions.begin();
		}
		/**
		 *\~english
		 *\brief		Gets an iterator to the end of the list
		 *\return		The iterator
		 *\~french
		 *\brief		R�cup�re un it�rateur sur la fin de la liste
		 *\return		L'it�rateur
		 */
		Castor::Point3rArrayIt End()
		{
			return m_arrayPositions.end();
		}
		/**
		 *\~english
		 *\brief		Gets an iterator to the end of the list
		 *\return		The iterator
		 *\~french
		 *\brief		R�cup�re un it�rateur sur la fin de la liste
		 *\return		L'it�rateur
		 */
		Castor::Point3rArrayConstIt End()const
		{
			return m_arrayPositions.end();
		}

	protected:
		/**
		 *\~english
		 *\brief		Initialises the shader program
		 *\return		\p true if all is OK
		 *\~french
		 *\brief		Initialise le shader
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool DoInitialise() = 0;

	protected:
		//!\~english  The RenderSystem	\~french La RenderSystem
		RenderSystem * m_pRenderSystem;
		//!\~english The positions list	\~french La liste des positions
		Castor::Point3rArray m_arrayPositions;
		//!\~english The Vertex buffer's description	\~french La description du tampon de sommets
		BufferDeclarationSPtr m_pDeclaration;
		//!\~english Tells the positions have changed and needs to be sent again to GPU	\~french Dit que les positions ont chang� et doivent �tre renvoy�es au GPU
		bool m_bNeedUpdate;
		//!\~english The positions GPU buffers	\~french Les tampon GPU de positions
		GeometryBuffersSPtr m_pGeometryBuffers;
		//!\~english  The shader program used to draw the billboards	\~french Le shader utilis� pour rendre les billboards
		ShaderProgramBaseWPtr m_wpProgram;
		//!\~english The Material	\~french Le Material
		MaterialWPtr m_wpMaterial;
		//!\~english The billboards dimensions	\~french Les dimensions des billboards
		Castor::Size m_dimensions;
		//!\~english The dimensions uniform variable	\~french La variable uniforme des dimensions
		Point2iFrameVariableSPtr m_pDimensionsUniform;
	};
}

#pragma warning( pop )

#endif
