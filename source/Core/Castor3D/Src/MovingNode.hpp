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
#ifndef ___C3D_MOVING_NODE_H___
#define ___C3D_MOVING_NODE_H___

#include "MovingObjectBase.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of MovingObjectBase for abstract nodes
	\remark		Used to decompose the model and place intermediate animations
	\~french
	\brief		Impl�mentation de MovingObjectBase pour des noeuds abstraits.
	\remark		Utilis� afin de d�composer le mod�le et ajouter des animatiobns interm�diaires
	*/
	class C3D_API MovingNode
		: public MovingObjectBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		MovingNode();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~MovingNode();
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		R�cup�re le nom de l'objet
		 *\return		Le nom
		 */
		virtual Castor::String const & GetName()const;

	private:
		virtual void DoApply();
		virtual MovingObjectBaseSPtr DoClone();

	private:
		static const Castor::String StrVoidString;
	};
}

#pragma warning( pop )

#endif
