/*
See LICENSE file in root folder
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
	\brief		Frame variable helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les variables de frame
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
		 *\param[in]	p_buffer	Le tampon de variables contenant celle donnàe
		 */
		FrameVariableTreeItemProperty( bool p_editable
			, castor3d::UniformSPtr p_variable
			, castor3d::UniformBuffer & p_buffer );
		/**
		 *\~english
		 *\brief		Constructor, allows creation of sampler frame variables only
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_variable	The target variable
		 *\param[in]	p_type		The variables buffer holding the one given
		 *\~french
		 *\brief		Constructeur, ne permet la cràation que de variables de type sampler
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_variable	La variable cible
		 *\param[in]	p_type		Le tampon de variables contenant celle donnàe
		 */
		FrameVariableTreeItemProperty( bool p_editable
			, castor3d::PushUniformSPtr p_variable
			, castor3d::ShaderType p_type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~FrameVariableTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the camera
		 *\return		The value
		 *\~french
		 *\brief		Récupère la caméra
		 *\return		La valeur
		 */
		inline castor3d::UniformSPtr getVariable()
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
		inline castor3d::UniformBufferRPtr getBuffer()
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
		void OnTypeChange( castor3d::UniformType p_value );
		void OnNameChange( castor::String const & p_value );
		void OnValueChange( wxVariant const & p_value );

	private:
		castor3d::UniformWPtr m_variable;
		castor3d::PushUniformWPtr m_pushVariable;
		castor3d::UniformBufferRPtr m_buffer{ nullptr };
		castor3d::ShaderType m_type;
	};
}

#endif
