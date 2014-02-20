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
#ifndef ___C3D_BillboardList___
#define ___C3D_BillboardList___

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
	\remark		Tous les billboards de cette liste ont la même texture
	*/
	class C3D_API BillboardList : public MovableObject
	{
	protected:
		//!\~english	 The RenderSystem	\~french	La RenderSystem
		RenderSystem * m_pRenderSystem;
		//!\~english	The positions list	\~french	La liste des positions
		Castor::Point3rArray m_arrayPositions;
		//!\~english	The Vertex buffer's description	\~french	La description du tampon de sommets
		BufferDeclarationSPtr m_pDeclaration;
		//!\~english	Tells the positions have changed and needs to be sent again to GPU	\~french	Dit que les positions ont changé et doivent être renvoyées au GPU
		bool m_bNeedUpdate;
		//!\~english	The positions GPU buffers	\~french	Les tampon GPU de positions
		GeometryBuffersSPtr m_pGeometryBuffers;
		//!\~english	 The shader program used to draw the billboards	\~french	Le shader utilisé pour rendre les billboards
		ShaderProgramBaseWPtr m_wpProgram;
		//!\~english	The Material	\~french	Le Material
		MaterialWPtr m_wpMaterial;
		//!\~english	The billboards dimensions	\~french	Les dimensions des billboards
		Castor::Size m_dimensions;
		//!\~english	The dimensions uniform variable	\~french	La variable uniforme des dimensions
		Point2iFrameVariableSPtr m_pDimensionsUniform;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pScene		The parent scene
		 *\param[in]	p_pRenderSystem	The RenderSystem
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pScene		La scène parente
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
		 *\brief		Initialise les éléments GPU
		 *\return		\p true si tout s'est bien passé
		 */
		bool Initialise();
		/**
		 *\~english
		 *\brief		Cleans GPU side elements up
		 *\~french
		 *\brief		Nettoie les éléments GPU
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Gets a point from the list
		 *\param[in]	p_uiIndex	The point index
		 *\return		The point
		 *\~french
		 *\brief		Récupère un point de la liste
		 *\param[in]	p_uiIndex	L'index du point
		 *\return		Le point
		 */
		Castor::Point3r const & GetAt( uint32_t p_uiIndex )const { return m_arrayPositions[p_uiIndex]; }
		/**
		 *\~english
		 *\brief		Sets a point in the list
		 *\param[in]	p_uiIndex		The point index
		 *\param[in]	p_ptPosition	The point
		 *\~french
		 *\brief		Définit un point de la liste
		 *\param[in]	p_uiIndex		L'index du point
		 *\param[in]	p_ptPosition	Le point
		 */
		void SetAt( uint32_t p_uiIndex, Castor::Point3r const & p_ptPosition ) { m_bNeedUpdate = true;m_arrayPositions[p_uiIndex] = p_ptPosition; }
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_pSampler	The new value
		 *\~french
		 *\brief		Définit le material
		 *\param[in]	p_pSampler	La nouvelle valeur
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
		 *\brief		Ajoute un point à la liste
		 *\param[in]	p_ptPosition	Le point
		 */
		void AddPoint( Castor::Point3r const & p_ptPosition );
		/**
		 *\~english
		 *\brief		Adds a points list to the list
		 *\param[in]	p_ptPositions	The points list
		 *\~french
		 *\brief		Ajoute une liste de points à la liste
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
		 *\brief		Récupère les dimensios des billboards
		 *\return		La valeur
		 */
		inline Castor::Size const & GetDimensions()const { return m_dimensions; }
		/**
		 *\~english
		 *\brief		Sets the billboards dimensions
		 *\param[in]	p_dimensions	The new value
		 *\~french
		 *\brief		Définit les dimensios des billboards
		 *\param[in]	p_dimensions	La nouvelle valeur
		 */
		void SetDimensions( Castor::Size const & p_dimensions );

	protected:
		/**
		 *\~english
		 *\brief		Initialises the shader program
		 *\return		\p true if all is OK
		 *\~french
		 *\brief		Initialise le shader
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoInitialise()=0;
	};
}

#pragma warning( pop )

#endif
