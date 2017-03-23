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
#ifndef ___GUICOMMON_FRAME_VARIABLE_BUFFER_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_FRAME_VARIABLE_BUFFER_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		10/09/2015
	\version	0.8.0
	\~english
	\brief		Frame variable buffer helper class to communicate between Scene objects or Materials lists and PropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesHolder, pour les tampons de variables de frame
	*/
	class FrameVariableBufferTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine	The engine, to post events to.
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_buffer	The target buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur, auquel on va poster les évènements
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_buffer	Le tampon cible
		 */
		FrameVariableBufferTreeItemProperty( Castor3D::Engine * p_engine, bool p_editable, Castor3D::UniformBufferSPtr p_buffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~FrameVariableBufferTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the camera
		 *\return		The value
		 *\~french
		 *\brief		Récupère la caméra
		 *\return		La valeur
		 */
		inline Castor3D::UniformBufferSPtr GetBuffer()
		{
			return m_buffer.lock();
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
		Castor3D::UniformBufferWPtr m_buffer;
	};
}

#endif
