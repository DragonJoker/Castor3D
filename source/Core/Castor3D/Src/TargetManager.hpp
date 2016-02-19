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
#ifndef ___C3D_TARGET_MANAGER_H___
#define ___C3D_TARGET_MANAGER_H___

#include "ResourceManager.hpp"

#include "RenderTarget.hpp"
#include "TechniqueFactory.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<> struct ManagedObjectNamer< RenderTarget >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		RenderTarget manager.
	\~french
	\brief		Gestionnaire de RenderTarget.
	*/
	class TargetManager
		: public ResourceManager< Castor::String, RenderTarget >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API TargetManager( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~TargetManager();
		/**
		 *\~english
		 *\brief		Creates a render target of given type
		 *\param[in]	p_type	The render target type
		 *\return		The render target
		 *\~french
		 *\brief		Crée une cible de rendu du type voulu
		 *\param[in]	p_type	Le type de cible de rendu
		 *\return		La cible de rendu
		 */
		C3D_API RenderTargetSPtr Create( eTARGET_TYPE p_type );
		/**
		 *\~english
		 *\brief		Removes a render target from the render loop
		 *\param[in]	p_pRenderTarget	The render target
		 *\~french
		 *\brief		Enlève une cible de rendu de la boucle de rendu
		 *\param[in]	p_pRenderTarget	La cible de rendu
		 */
		C3D_API void Remove( RenderTargetSPtr && p_pRenderTarget );
		/**
		 *\~english
		 *\brief			Renders into the render targets.
		 *\param[in]		p_time		The current frame time.
		 *\param[in,out]	p_vtxCount	Receives the total vertex count.
		 *\param[in]		p_fceCount	Receives the total faces count.
		 *\param[in]		p_objCount	Receives the total objects count.
		 *\~french
		 *\brief			Dessine dans les cibles de rendu.
		 *\param[in]		p_time		Le temps d dessin actuel.
		 *\param[in,out]	p_vtxCount	Reçoit le nombre de sommets dessinés.
		 *\param[in,out]	p_fceCount	Reçoit le nombre de faces dessinées.
		 *\param[in,out]	p_objCount	Reçoit le nombre d'objets dessinés.
		 */
		C3D_API void Render( double & p_time, uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount );

	private:
		using ResourceManager< Castor::String, RenderTarget >::Create;

	private:
		DECLARE_VECTOR( RenderTargetSPtr, RenderTarget );
		DECLARE_ARRAY( RenderTargetArray, eTARGET_TYPE_COUNT, TargetType );
		//!\~english The render targets sorted by target type	\~french Les cibles de rendu, triées par type de cible de rendu
		TargetTypeArray m_renderTargets;
	};
}

#endif
