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
#ifndef ___GUICOMMON_PASS_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_PASS_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

#include <Material/Material.hpp>
#include <Material/Pass.hpp>

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
	class PassTreeItemProperty
		: public TreeItemProperty
		, private wxEvtHandler
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_editable	Tells if the properties are modifiable.
		 *\param[in]	p_pass		The target pass.
		 *\param[in]	p_scene		The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables.
		 *\param[in]	p_pass		La passe cible.
		 *\param[in]	p_scene		La scène.
		 */
		PassTreeItemProperty( bool p_editable, Castor3D::PassSPtr p_pass, Castor3D::Scene & p_scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~PassTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the geometry
		 *\return		The value
		 *\~french
		 *\brief		Récupère la gàomàtrie
		 *\return		La valeur
		 */
		inline Castor3D::PassSPtr GetPass()
		{
			return m_pass.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the geometry
		 *\return		The value
		 *\~french
		 *\brief		Récupère la gàomàtrie
		 *\return		La valeur
		 */
		template< Castor3D::MaterialType Type >
		inline std::shared_ptr< typename Castor3D::PassTyper< Type >::Type > GetTypedPass()
		{
			auto pass = m_pass.lock();
			REQUIRE( pass && pass->GetType() == Type );
			return std::static_pointer_cast< typename Castor3D::PassTyper< Type >::Type >( pass );
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
		void OnDiffuseColourChange( Castor::Colour const & p_value );
		void OnSpecularColourChange( Castor::Colour const & p_value );
		void OnAmbientChange( double p_value );
		void OnEmissiveChange( double p_value );
		void OnExponentChange( double p_value );
		void OnTwoSidedChange( bool p_value );
		void OnOpacityChange( double p_value );
		void OnRefractionRatioChange( double p_value );
		void OnAlbedoChange( Castor::Colour const & p_value );
		void OnRoughnessChange( double p_value );
		void OnMetallicChange( double p_value );
		void OnGlossinessChange( double p_value );
		bool OnEditShader( wxPGProperty * p_property );

	private:
		Castor3D::PassWPtr m_pass;
		Castor3D::Scene & m_scene;
	};
}

#endif
