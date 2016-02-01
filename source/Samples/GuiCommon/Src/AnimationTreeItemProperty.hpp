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
#ifndef ___GUICOMMON_ANIMATION_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_ANIMATION_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Animation helper class to communicate between Scene objects or Materials lists and PropertiesHolder.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesHolder, pour les Animation.
	*/
	class AnimationTreeItemProperty
		: public TreeItemProperty
		, private wxEvtHandler
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_editable	Tells if the properties are modifiable.
		 *\param[in]	p_animation	The target Animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables.
		 *\param[in]	p_animation	L'Animation cible.
		 */
		AnimationTreeItemProperty( Castor3D::Engine * p_engine, bool p_editable, Castor3D::AnimatedObjectGroupSPtr p_group, Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~AnimationTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the Animation.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère l'Animation.
		 *\return		La valeur.
		 */
		inline Castor3D::AnimatedObjectGroupSPtr GetGroup()
		{
			return m_group.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation name.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le de nom de l'animation.
		 *\return		La valeur.
		 */
		inline Castor::String GetName()
		{
			return m_name;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::DoCreateProperties
		 */
		virtual void DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::DoPropertyChange
		 */
		virtual void DoPropertyChange( wxPropertyGridEvent & p_event );

	private:
		void OnSpeedChange( double p_value );
		bool OnStateChange( wxPGProperty * p_property );

	private:
		Castor3D::AnimatedObjectGroupWPtr m_group;
		Castor::String m_name;
	};
}

#endif
