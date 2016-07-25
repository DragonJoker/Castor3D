/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GUICOMMON_ANIMATED_OBJECT_GROUP_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_ANIMATED_OBJECT_GROUP_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		01/02/2016
	\version	0.8.0
	\~english
	\brief		AnimatedObjectGroup helper class to communicate between Scene objects or Materials lists and PropertiesHolder.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de sc�ne, ou la liste de mat�riaux, et PropertiesHolder, pour les AnimatedObjectGroup.
	*/
	class AnimatedObjectGroupTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_group		The target AnimatedObjectGroup
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propri�t�s sont modifiables
		 *\param[in]	p_group		L"AnimatedObjectGroup cible
		 */
		AnimatedObjectGroupTreeItemProperty( bool p_editable, Castor3D::AnimatedObjectGroupSPtr p_group );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~AnimatedObjectGroupTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the AnimatedObjectGroup
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re l'AnimatedObjectGroup
		 *\return		La valeur
		 */
		inline Castor3D::AnimatedObjectGroupSPtr GetGroup()
		{
			return m_group.lock();
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
		Castor3D::AnimatedObjectGroupWPtr m_group;
	};
}

#endif
