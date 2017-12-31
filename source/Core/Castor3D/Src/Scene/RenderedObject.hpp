/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderedObject_H___
#define ___C3D_RenderedObject_H___

#include "Castor3DPrerequisites.hpp"

#include "MovableObject.hpp"
#include "RenderedObject.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Handles object's visibility, and behavior with shadows.
	\~french
	\brief		Gère la visibilité de l'objet, ainsi que son comportement avec les ombres.
	*/
	class RenderedObject
		: public std::enable_shared_from_this< RenderedObject >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Geometry loader
		\~french
		\brief		Loader de Geometry
		*/
		class TextWriter
			: public castor::TextWriter< RenderedObject >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a RenderedObject into a text file.
			 *\param[in]	p_file		The file to save the cameras in.
			 *\param[in]	p_object	The RenderedObject to save.
			 *\~french
			 *\brief		Ecrit une RenderedObject dans un fichier texte.
			 *\param[in]	p_file		Le fichier.
			 *\param[in]	p_object	Le RenderedObject.
			 */
			C3D_API bool operator()( RenderedObject const & p_object, castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\return		The visibility status.
		 *\~french
		 *\return		Le statut de visibilité.
		 */
		inline bool isVisible()const
		{
			return m_visible;
		}
		/**
		 *\~english
		 *\brief		Defines the visibility status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut de visibilité.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setVisible( bool p_value )
		{
			m_visible = p_value;
		}
		/**
		 *\~english
		 *\return		The shadow casting status.
		 *\~french
		 *\return		Le statut de projection d'ombres.
		 */
		inline bool isShadowCaster()const
		{
			return m_castShadows;
		}
		/**
		 *\~english
		 *\brief		Defines the shadow caster status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut de projection d'ombres.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setShadowCaster( bool p_value )
		{
			m_castShadows = p_value;
		}
		/**
		 *\~english
		 *\return		The shadow receiving status.
		 *\~french
		 *\return		Le statut de réception d'ombres.
		 */
		inline bool isShadowReceiver()const
		{
			return m_receivesShadows;
		}
		/**
		 *\~english
		 *\brief		Defines the shadow receiving status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut de réception d'ombres.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setShadowReceiver( bool p_value )
		{
			m_receivesShadows = p_value;
		}

	private:
		//!\~english	Tells if the object is visible.
		//!\~french		Dit si l'objet est visible.
		bool m_visible{ true };
		//!\~english	Tells if the object casts shadows.
		//!\~french		Dit si l'objet projette des ombres.
		bool m_castShadows{ true };
		//!\~english	Tells if the object receives shadows.
		//!\~french		Dit si l'objet reçoit les ombres.
		bool m_receivesShadows{ true };
	};
}

#endif
