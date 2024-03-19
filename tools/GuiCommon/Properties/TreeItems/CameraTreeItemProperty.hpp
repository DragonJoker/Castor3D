/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_CAMERA_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_CAMERA_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Camera helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les camàras
	*/
	class CameraTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable
		 *\param[in]	camera	The target camera
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables
		 *\param[in]	camera	La caméra cible
		 */
		CameraTreeItemProperty( bool editable, castor3d::Camera & camera );
		/**
		 *\~english
		 *\brief		Retrieves the camera
		 *\return		The value
		 *\~french
		 *\brief		Récupère la caméra
		 *\return		La valeur
		 */
		inline castor3d::Camera & getCamera()
		{
			return m_camera;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		castor3d::Camera & m_camera;
	};
}

#endif
