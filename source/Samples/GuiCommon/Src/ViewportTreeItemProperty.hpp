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
#ifndef ___GUICOMMON_VIEWPORT_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_VIEWPORT_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/08/2015
	\version	0.8.0
	\~english
	\brief		Viewport helper class to communicate between Scene objects or Materials lists and PropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesHolder, pour les viewports
	*/
	class ViewportTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	engine	The engine
		 *\param[in]	p_viewport	The target viewport
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	engine	Le moteur
		 *\param[in]	p_viewport	Le viewport cible
		 */
		ViewportTreeItemProperty( bool p_editable, Castor3D::Engine & engine, Castor3D::Viewport & p_viewport );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~ViewportTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the viewport
		 *\return		The value
		 *\~french
		 *\brief		Récupère le viewport
		 *\return		La valeur
		 */
		inline Castor3D::Viewport & GetViewport()
		{
			return m_viewport;
		}
		/**
		 *\~english
		 *\brief		Retrieves the viewport
		 *\return		The value
		 *\~french
		 *\brief		Récupère le viewport
		 *\return		La valeur
		 */
		inline Castor3D::Viewport const & GetViewport()const
		{
			return m_viewport;
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
		void OnTypeChange( Castor3D::ViewportType p_value );
		void OnSizeChange( Castor::Size const & p_value );
		void OnTopChange( double p_value );
		void OnBottomChange( double p_value );
		void OnLeftChange( double p_value );
		void OnRightChange( double p_value );
		void OnNearChange( double p_value );
		void OnFarChange( double p_value );
		void OnFovYChange( double p_value );
		void OnRatioChange( double p_value );

	private:
		Castor3D::Viewport & m_viewport;
	};
}

#endif
