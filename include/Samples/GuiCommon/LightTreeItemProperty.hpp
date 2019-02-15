/*
See LICENSE file in root folder
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
	\brief		Geometry helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les gàomàtries
	*/
	class LightTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable
		 *\param[in]	light		The target light
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables
		 *\param[in]	light		La lumiàre cible
		 */
		LightTreeItemProperty( bool editable, castor3d::Light & light );
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
		virtual void doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doPropertyChange
		 */
		virtual void doPropertyChange( wxPropertyGridEvent & event );

	private:
		void doCreateDirectionalLightProperties( wxPropertyGrid * grid, castor3d::DirectionalLightSPtr light );
		void doCreatePointLightProperties( wxPropertyGrid * grid, castor3d::PointLightSPtr light );
		void doCreateSpotLightProperties( wxPropertyGrid * grid, castor3d::SpotLightSPtr light );
		void doCreateShadowProperties( wxPropertyGrid * grid );
		void onColourChange( castor::RgbColour const & value );
		void onIntensityChange( castor::Point2f const & value );
		void onAttenuationChange( castor::Point3f const & value );
		void onCutOffChange( double value );
		void onExponentChange( double value );
		void onShadowTypeChange( castor3d::ShadowType value );
		void onShadowMinOffsetChange( double value );
		void onShadowMaxSlopeOffsetChange( double value );
		void onShadowMaxVarianceChange( double value );
		void onShadowVarianceBiasChange( double value );
		void onShadowVolumetricStepsChange( long value );
		void onShadowVolumetricScatteringFactorChange( double value );

	private:
		castor3d::Light & m_light;
	};
}

#endif
