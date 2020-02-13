/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_RENDER_TARGET_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_RENDER_TARGET_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Render target helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les cibles de rendu
	*/
	class RenderTargetTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_target	The target object
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_target	L'objet cible
		 */
		RenderTargetTreeItemProperty( bool editable
			, castor3d::RenderTargetSPtr target );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~RenderTargetTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the object
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'objet
		 *\return		La valeur
		 */
		inline castor3d::RenderTargetSPtr getRenderTarget()
		{
			return m_target.lock();
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		virtual void doCreateProperties( wxPGEditor * editor
			, wxPropertyGrid * grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doPropertyChange
		 */
		virtual void doPropertyChange( wxPropertyGridEvent & event );

		void OnSsaoEnable( bool value );
		void OnSsaoHighQuality( bool value );
		void OnSsaoNormalsBuffer( bool value );
		void OnSsaoRadius( float value );
		void OnSsaoBias( float value );
		void OnSsaoIntensity( float value );
		void OnSsaoSamples( uint32_t value );
		void OnSsaoEdgeSharpness( float value );
		void OnSsaoBlurHighQuality( bool value );
		void OnSsaoBlurStepSize( uint32_t value );
		void OnSsaoBlurRadius( int32_t value );

	private:
		castor3d::RenderTargetWPtr m_target;
	};
}

#endif
