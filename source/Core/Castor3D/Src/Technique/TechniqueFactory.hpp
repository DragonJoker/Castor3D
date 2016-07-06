/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_TECHNIQUE_FACTORY_H___
#define ___C3D_TECHNIQUE_FACTORY_H___

#include "RenderTechnique.hpp"

#include <Factory.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		24/11/2014
	\~english
	\brief		Render technique factory
	\~french
	\brief		La fabrique de techniques de rendu
	*/
	class TechniqueFactory
		: public Castor::Factory< RenderTechnique, Castor::String, std::shared_ptr< RenderTechnique >, std::function< RenderTechniqueSPtr( RenderTarget &, RenderSystem &, Parameters const & ) > >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API TechniqueFactory();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~TechniqueFactory();
		/**
		 *\~english
		 *\brief		Registers all objects types
		 *\~french
		 *\brief		Enregistre tous les types d'objets
		 */
		C3D_API virtual void Initialise();
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/07/2016
	\version	0.9.0
	\~english
	\brief		Helper structure to create an element.
	\~french
	\brief		Structure permettant de créer un élément.
	*/
	template<>
	struct ElementProducer< RenderTechnique, Castor::String, Castor::String, RenderTarget, RenderSystem, Parameters >
	{
		using ElementPtr = std::shared_ptr< RenderTechnique >;

		ElementProducer() = default;

		ElementPtr operator()( Castor::String const & p_key, Castor::String const & p_type, RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_parameters )
		{
			return m_factory.Create( p_type, p_renderTarget, p_renderSystem, p_parameters );
		}
		//!\~english	The RenderTechnique factory.
		//!\~french		La fabrique de RenderTechnique.
		TechniqueFactory m_factory;
	};
}

#endif
