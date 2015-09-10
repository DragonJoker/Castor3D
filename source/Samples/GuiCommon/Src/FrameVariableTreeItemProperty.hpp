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
#ifndef ___GUICOMMON_FRAME_VARIABLE_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_FRAME_VARIABLE_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		10/09/2015
	\version	0.8.0
	\~english
	\brief		Frame variable helper class to communicate between Scene objects or Materials lists and PropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesHolder, pour les variables de frame
	*/
	class FrameVariableTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_variable	The target variable
		 *\param[in]	p_buffer	The variables buffer holding the one given
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_variable	La variable cible
		 *\param[in]	p_buffer	Le tampon de variables contenant celle donnée
		 */
		FrameVariableTreeItemProperty( bool p_editable, Castor3D::FrameVariableSPtr p_variable, Castor3D::FrameVariableBufferSPtr p_buffer );
		/**
		 *\~english
		 *\brief		Constructor, allows creation of sampler frame variables only
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_variable	The target variable
		 *\param[in]	p_type		The variables buffer holding the one given
		 *\~french
		 *\brief		Constructeur, ne permet la création que de variables de type sampler
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_variable	La variable cible
		 *\param[in]	p_type		Le tampon de variables contenant celle donnée
		 */
		FrameVariableTreeItemProperty( bool p_editable, Castor3D::FrameVariableSPtr p_variable, Castor3D::eSHADER_TYPE p_type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~FrameVariableTreeItemProperty();
		/**
		 *\~english
		 *\brief		Creates and fills the overlay properties, in the given wxPropertyGrid
		 *\param[in]	p_grid	The target wxPropertyGrid
		 *\~french
		 *\brief		Construit et remplit les propriétés de l'incrustation, dans la wxPropertyGrid donnée
		 *\param[in]	p_grid	La wxPropertyGrid cible
		 */
		virtual void CreateProperties( wxPropertyGrid * p_grid );
		/**
		 *\~english
		 *\brief		Call when a property grid property is changed
		 *\param[in]	p_event	The event
		 *\~french
		 *\brief		Appelée lorsqu'une propriété est changée
		 *\param[in]	p_event	L'évènement
		 */
		virtual void OnPropertyChange( wxPropertyGridEvent & p_event );
		/**
		 *\~english
		 *\brief		Retrieves the camera
		 *\return		The value
		 *\~french
		 *\brief		Récupère la caméra
		 *\return		La valeur
		 */
		inline Castor3D::FrameVariableSPtr GetVariable()
		{
			return m_variable.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the camera
		 *\return		The value
		 *\~french
		 *\brief		Récupère la caméra
		 *\return		La valeur
		 */
		inline Castor3D::FrameVariableBufferSPtr GetBuffer()
		{
			return m_buffer.lock();
		}

	private:
		void OnTypeChange( Castor3D::eFRAME_VARIABLE_TYPE p_value );

	private:
		Castor3D::FrameVariableWPtr m_variable;
		Castor3D::FrameVariableBufferWPtr m_buffer;
		Castor3D::eSHADER_TYPE m_type;
	};
}

#endif
