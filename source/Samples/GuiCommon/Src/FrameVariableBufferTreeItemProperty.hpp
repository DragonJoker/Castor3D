/*
See LICENSE file in root folder
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
		 *\param[in]	engine	The engine, to post events to.
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_buffer	The target buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur, auquel on va poster les évènements
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_buffer	Le tampon cible
		 */
		FrameVariableBufferTreeItemProperty( castor3d::Engine * engine
			, bool p_editable
			, castor3d::UniformBuffer & p_buffer );
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
		inline castor3d::UniformBuffer const & getBuffer()const
		{
			return m_buffer;
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
		castor3d::UniformBuffer & m_buffer;
	};
}

#endif
