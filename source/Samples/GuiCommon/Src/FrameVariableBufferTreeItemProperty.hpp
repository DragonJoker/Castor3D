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
	\brief		Frame variable buffer helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les tampons de variables de frame
	*/
	class FrameVariableBufferTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine		The engine, to post events to.
		 *\param[in]	editable	Tells if the properties are modifiable
		 *\param[in]	buffer		The target buffer
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine		Le moteur, auquel on va poster les évènements
		 *\param[in]	editable	Dit si les propriétés sont modifiables
		 *\param[in]	buffer		Le tampon cible
		 */
		FrameVariableBufferTreeItemProperty( castor3d::Engine * engine
			, bool editable
			, renderer::UniformBufferBase & buffer );
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
		inline renderer::UniformBufferBase const & getBuffer()const
		{
			return m_buffer;
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
		renderer::UniformBufferBase & m_buffer;
	};
}

#endif
