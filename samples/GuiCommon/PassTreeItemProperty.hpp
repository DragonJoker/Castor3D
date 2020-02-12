/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_PASS_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_PASS_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/TreeItemProperty.hpp"

#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Geometry helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les gàomàtries
	*/
	class PassTreeItemProperty
		: public TreeItemProperty
		, private wxEvtHandler
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	editable	Tells if the properties are modifiable.
		 *\param[in]	pass		The target pass.
		 *\param[in]	scene		The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	pass		La passe cible.
		 *\param[in]	scene		La scène.
		 */
		PassTreeItemProperty( bool editable
			, castor3d::PassSPtr pass
			, castor3d::Scene & scene
			, wxWindow * parent );
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
		inline castor3d::PassSPtr getPass()
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
		template< castor3d::MaterialType Type >
		inline std::shared_ptr< typename castor3d::PassTyper< Type >::Type > getTypedPass()
		{
			auto pass = m_pass.lock();
			CU_Require( pass && pass->getType() == Type );
			return std::static_pointer_cast< typename castor3d::PassTyper< Type >::Type >( pass );
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
		void OnDiffuseColourChange( castor::RgbColour const & p_value );
		void OnSpecularColourChange( castor::RgbColour const & p_value );
		void OnAmbientChange( double p_value );
		void OnEmissiveChange( double p_value );
		void OnExponentChange( double p_value );
		void OnTwoSidedChange( bool p_value );
		void OnOpacityChange( double p_value );
		void OnBWAccumulatorChange( long p_value );
		void OnRefractionRatioChange( double p_value );
		void OnAlbedoChange( castor::RgbColour const & p_value );
		void OnRoughnessChange( double p_value );
		void OnMetallicChange( double p_value );
		void OnGlossinessChange( double p_value );
		bool OnEditShader( wxPGProperty * p_property );

	private:
		castor3d::PassWPtr m_pass;
		castor3d::Scene & m_scene;
		wxWindow * m_parent;
	};
}

#endif
