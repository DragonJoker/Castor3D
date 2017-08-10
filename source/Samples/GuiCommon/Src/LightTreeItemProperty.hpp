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
#ifndef ___GUICOMMON_LIGHT_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_LIGHT_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Geometry helper class to communicate between Scene objects or Materials lists and PropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesHolder, pour les gàomàtries
	*/
	class LightTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_light		The target light
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_light		La lumiàre cible
		 */
		LightTreeItemProperty( bool p_editable, castor3d::Light & p_light );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~LightTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the light source
		 *\return		The value
		 *\~french
		 *\brief		Récupère la source lumineuse
		 *\return		La valeur
		 */
		inline castor3d::Light & getLight()
		{
			return m_light;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		virtual void doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doPropertyChange
		 */
		virtual void doPropertyChange( wxPropertyGridEvent & p_event );

	private:
		void doCreateDirectionalLightProperties( wxPropertyGrid * p_grid, castor3d::DirectionalLightSPtr p_light );
		void doCreatePointLightProperties( wxPropertyGrid * p_grid, castor3d::PointLightSPtr p_light );
		void doCreateSpotLightProperties( wxPropertyGrid * p_grid, castor3d::SpotLightSPtr p_light );
		void OnColourChange( castor::Colour const & p_value );
		void OnIntensityChange( castor::Point2f const & p_value );
		void OnAttenuationChange( castor::Point3f const & p_value );
		void OnCutOffChange( double p_value );
		void OnExponentChange( double p_value );

	private:
		castor3d::Light & m_light;
	};
}

#endif
