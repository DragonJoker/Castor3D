/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_DEPTH_STENCIL_STATE_H___
#define ___C3D_DEPTH_STENCIL_STATE_H___

#include "Castor3DPrerequisites.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/07/2016
	\version	0.9.0
	\~english
	\brief		Helper structure to create an element.
	\~french
	\brief		Structure permettant de créer un élément.
	*/
	template<>
	struct ElementProducer< DepthStencilState, Castor::String >
	{
		using ElementPtr = std::shared_ptr< DepthStencilState >;

		ElementProducer( Engine & p_engine )
			: m_engine{ p_engine }
		{
		}

		ElementPtr operator()( Castor::String const & p_key );

		Engine & m_engine;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Depth and stencil buffers configurations class
	\~french
	\brief		Classe de configuration des buffers de profondeur et stencil
	*/
	class DepthStencilState
		: public Castor::OwnedBy< Engine >
	{
	protected:
		/*!
		\author 	Sylvain DOREMUS
		\version	0.7.0
		\date 		27/11/2013
		\~english
		\brief		Stencil function and operations description
		\~french
		\brief		Fonction et opérations de stencil
		*/
		struct stSTENCIL
		{
			//!\~english The function	\~french La fonction
			eSTENCIL_FUNC m_eFunc;
			//!\~english The reference value for the function	\~french La valeur de référence pour la fonction
			uint32_t m_ref;
			//!\~english The operation on stencil test fail	\~french Opération faite en cas d'échec du stencil test
			eSTENCIL_OP m_eFailOp;
			//!\~english The operation on deph test fail	\~french Opération faite en cas d'échec du test de profondeur
			eSTENCIL_OP m_eDepthFailOp;
			//!\~english The operation on depth and stencil tests success	\~french Opération faite en cas de réussite des tests stencil et profondeur
			eSTENCIL_OP m_ePassOp;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API explicit DepthStencilState( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~DepthStencilState();
		/**
		 *\~english
		 *\brief		Initialises the states
		 *\~french
		 *\brief		Initialise les états
		 */
		C3D_API virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans the states
		 *\~french
		 *\brief		Nettoie les états
		 */
		C3D_API virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Applies the states
		 *\~french
		 *\brief		Applique les états
		 */
		C3D_API virtual bool Apply() = 0;
		/**
		 *\~english
		 *\brief		Defines the depth test status
		 *\param[in]	p_enable The new status
		 *\~french
		 *\brief		Définit le statut du test de profondeur
		 *\param[in]	p_enable	Le nouveau statut
		 */
		inline void SetDepthTest( bool p_enable )
		{
			m_changed |= m_bDepthTest != p_enable;
			m_bDepthTest = p_enable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the depth test status
		 *\return		The status
		 *\~french
		 *\brief		Récupère le statut du test de profondeur
		 *\return		Le statut
		 */
		inline bool GetDepthTest()const
		{
			return m_bDepthTest;
		}
		/**
		 *\~english
		 *\brief		Defines the depth function
		 *\param[in]	p_func The function
		 *\~french
		 *\brief		Définit la fonction de profondeur
		 *\param[in]	p_func	La fonction
		 */
		inline void SetDepthFunc( eDEPTH_FUNC p_func )
		{
			m_changed |= m_eDepthFunc != p_func;
			m_eDepthFunc = p_func;
		}
		/**
		 *\~english
		 *\brief		Retrieves the depth function
		 *\return		The function
		 *\~french
		 *\brief		Récupère la fonction de profondeur
		 *\return		La fonction
		 */
		inline eDEPTH_FUNC GetDepthFunc()const
		{
			return m_eDepthFunc;
		}
		/**
		 *\~english
		 *\brief		Defines the depth range
		 *\param[in]	p_near	The near bound
		 *\param[in]	p_far	The far bound
		 *\~french
		 *\brief		Définit l'intervalle profondeur
		 *\param[in]	p_near	La borne inférieure
		 *\param[in]	p_far	La borne supérieure
		 */
		inline void SetDepthRange( double p_near, double p_far )
		{
			m_changed |= p_near != m_dDepthNear || p_far != m_dDepthFar;
			m_dDepthNear = p_near;
			m_dDepthFar = p_far;
		}
		/**
		 *\~english
		 *\brief		Retrieves the depth near value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur proche pour la profondeur
		 *\return		La valeur
		 */
		inline double GetDepthNear()const
		{
			return m_dDepthNear;
		}
		/**
		 *\~english
		 *\brief		Retrieves the depth far value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur lointaine pour la profondeur
		 *\return		La valeur
		 */
		inline double GetDepthFar()const
		{
			return m_dDepthFar;
		}
		/**
		 *\~english
		 *\brief		Defines the depth writing mask
		 *\param[in]	p_eMask The mask
		 *\~french
		 *\brief		Définit le masque d'écriture de la profondeur
		 *\param[in]	p_eMask	Le masque
		 */
		inline void SetDepthMask( eWRITING_MASK p_eMask )
		{
			m_changed |= m_eDepthMask != p_eMask;
			m_eDepthMask = p_eMask;
		}
		/**
		 *\~english
		 *\brief		Retrieves the depth writing mask
		 *\return		The mask
		 *\~french
		 *\brief		Récupère le masque d'écriture de la profondeur
		 *\return		Le masque
		 */
		inline eWRITING_MASK GetDepthMask()const
		{
			return m_eDepthMask;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil test status
		 *\param[in]	p_enable The new status
		 *\~french
		 *\brief		Définit le statut du test de stencil
		 *\param[in]	p_enable	Le nouveau statut
		 */
		inline void SetStencilTest( bool p_enable )
		{
			m_changed = true;
			m_bStencilTest = p_enable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil test status
		 *\return		The status
		 *\~french
		 *\brief		Récupère le statut du test de stencil
		 *\return		Le statut
		 */
		inline bool GetStencilTest()const
		{
			return m_bStencilTest;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil read mask
		 *\param[in]	p_uiMask The mask
		 *\~french
		 *\brief		Définit le masque de lecture du stencil
		 *\param[in]	p_uiMask	Le masque
		 */
		inline void SetStencilReadMask( uint32_t p_uiMask )
		{
			m_changed |= m_uiStencilReadMask != p_uiMask;
			m_uiStencilReadMask = p_uiMask;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil read mask
		 *\return		The mask
		 *\~french
		 *\brief		Récupère le masque de lecture du stencil
		 *\return		Le masque
		 */
		inline uint32_t GetStencilReadMask()const
		{
			return m_uiStencilReadMask;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil write mask
		 *\param[in]	p_uiMask The mask
		 *\~french
		 *\brief		Définit le masque d'écriture du stencil
		 *\param[in]	p_uiMask	Le masque
		 */
		inline void SetStencilWriteMask( uint32_t p_uiMask )
		{
			m_changed |= m_uiStencilWriteMask != p_uiMask;
			m_uiStencilWriteMask = p_uiMask;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil write mask
		 *\return		The mask
		 *\~french
		 *\brief		Récupère le masque d'écriture du stencil
		 *\return		Le masque
		 */
		inline uint32_t GetStencilWriteMask()const
		{
			return m_uiStencilWriteMask;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil reference value for front faces
		 *\param[in]	p_ref The reference value
		 *\~french
		 *\brief		Définit la valeur de référence de stencil pour les faces avant
		 *\param[in]	p_ref	La valeur de référence
		 */
		inline void SetStencilFrontRef( uint32_t p_ref )
		{
			m_changed |= m_stStencilFront.m_ref != p_ref;
			m_stStencilFront.m_ref = p_ref;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil reference value for front faces
		 *\return		The reference value
		 *\~french
		 *\brief		Récupère la valeur de référence de stencil pour les faces avant
		 *\return		La valeur de référence
		 */
		inline uint32_t GetStencilFrontRef()const
		{
			return m_stStencilFront.m_ref;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil function for front faces
		 *\param[in]	p_func The function
		 *\~french
		 *\brief		Définit la fonction de stencil pour les faces avant
		 *\param[in]	p_func	La fonction
		 */
		inline void SetStencilFrontFunc( eSTENCIL_FUNC p_func )
		{
			m_changed |= m_stStencilFront.m_eFunc != p_func;
			m_stStencilFront.m_eFunc = p_func;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil function for front faces
		 *\return		The function
		 *\~french
		 *\brief		Récupère la fonction de stencil pour les faces avant
		 *\return		La fonction
		 */
		inline eSTENCIL_FUNC GetStencilFrontFunc()const
		{
			return m_stStencilFront.m_eFunc;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test failed, for front faces
		 *\param[in]	p_eOp The operation
		 *\~french
		 *\brief		Définit l'opération lors de l'échec du test de stencil, pour les faces avant
		 *\param[in]	p_eOp	L'opération
		 */
		inline void SetStencilFrontFailOp( eSTENCIL_OP p_eOp )
		{
			m_changed |= m_stStencilFront.m_eFailOp != p_eOp;
			m_stStencilFront.m_eFailOp = p_eOp;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test failed, for front faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de l'échec du test de stencil, pour les faces avant
		 *\return		L'opération
		 */
		inline eSTENCIL_OP GetStencilFrontFailOp()const
		{
			return m_stStencilFront.m_eFailOp;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test passes but depth test failed, for front faces
		 *\param[in]	p_eOp The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur, pour les faces avant
		 *\param[in]	p_eOp	L'opération
		 */
		inline void SetStencilFrontDepthFailOp( eSTENCIL_OP p_eOp )
		{
			m_changed |= m_stStencilFront.m_eDepthFailOp != p_eOp;
			m_stStencilFront.m_eDepthFailOp = p_eOp;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test passes but depth test failed, for front faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur, pour les faces avant
		 *\return		L'opération
		 */
		inline eSTENCIL_OP GetStencilFrontDepthFailOp()const
		{
			return m_stStencilFront.m_eDepthFailOp;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil stencil and depth tests passed, for front faces
		 *\param[in]	p_eOp The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite des tests de stencil et de profondeur, pour les faces avant
		 *\param[in]	p_eOp	L'opération
		 */
		inline void SetStencilFrontPassOp( eSTENCIL_OP p_eOp )
		{
			m_changed |= m_stStencilFront.m_ePassOp != p_eOp;
			m_stStencilFront.m_ePassOp = p_eOp;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil and depth tests passed, for front faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de la réussite des tests de stencil et de profondeur, pour les faces avant
		 *\return		L'opération
		 */
		inline eSTENCIL_OP GetStencilFrontPassOp()const
		{
			return m_stStencilFront.m_ePassOp;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil reference value for back faces
		 *\param[in]	p_ref The reference value
		 *\~french
		 *\brief		Définit la valeur de référence de stencil pour les faces arrières
		 *\param[in]	p_ref	La valeur de référence
		 */
		inline void SetStencilBackRef( uint32_t p_ref )
		{
			m_changed |= m_stStencilBack.m_ref != p_ref;
			m_stStencilBack.m_ref = p_ref;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil reference value for back faces
		 *\return		The reference value
		 *\~french
		 *\brief		Récupère la valeur de référence de stencil pour les faces arrières
		 *\return		La valeur de référence
		 */
		inline uint32_t GetStencilBackRef()const
		{
			return m_stStencilBack.m_ref;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil function for front back
		 *\param[in]	p_func The function
		 *\~french
		 *\brief		Définit la fonction de stencil pour les faces arrières
		 *\param[in]	p_func	La fonction
		 */
		inline void SetStencilBackFunc( eSTENCIL_FUNC p_func )
		{
			m_changed |= m_stStencilBack.m_eFunc != p_func;
			m_stStencilBack.m_eFunc = p_func;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil function for back faces
		 *\return		The function
		 *\~french
		 *\brief		Récupère la fonction de stencil pour les faces arrières
		 *\return		La fonction
		 */
		inline eSTENCIL_FUNC GetStencilBackFunc()const
		{
			return m_stStencilBack.m_eFunc;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test failed, for back faces
		 *\param[in]	p_eOp The operation
		 *\~french
		 *\brief		Définit l'opération lors de l'échec du test de stencil, pour les faces arrières
		 *\param[in]	p_eOp	L'opération
		 */
		inline void SetStencilBackFailOp( eSTENCIL_OP p_eOp )
		{
			m_changed |= m_stStencilBack.m_eFailOp != p_eOp;
			m_stStencilBack.m_eFailOp = p_eOp;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test failed, for back faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de l'échec du test de stencil, pour les faces arrières
		 *\return		L'opération
		 */
		inline eSTENCIL_OP GetStencilBackFailOp()const
		{
			return m_stStencilBack.m_eFailOp;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test passes but depth test failed, for back faces
		 *\param[in]	p_eOp The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur, pour les faces arrières
		 *\param[in]	p_eOp	L'opération
		 */
		inline void SetStencilBackDepthFailOp( eSTENCIL_OP p_eOp )
		{
			m_changed |= m_stStencilBack.m_eDepthFailOp != p_eOp;
			m_stStencilBack.m_eDepthFailOp = p_eOp;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test passes but depth test failed, for back faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur, pour les faces arrières
		 *\return		L'opération
		 */
		inline eSTENCIL_OP GetStencilBackDepthFailOp()const
		{
			return m_stStencilBack.m_eDepthFailOp;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil stencil and depth tests passed, for back faces
		 *\param[in]	p_eOp The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite des tests de stencil et de profondeur, pour les faces arrières
		 *\param[in]	p_eOp	L'opération
		 */
		inline void SetStencilBackPassOp( eSTENCIL_OP p_eOp )
		{
			m_changed |= m_stStencilBack.m_ePassOp != p_eOp;
			m_stStencilBack.m_ePassOp = p_eOp;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil and depth tests passed, for back faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de la réussite des tests de stencil et de profondeur, pour les faces arrières
		 *\return		L'opération
		 */
		inline eSTENCIL_OP GetStencilBackPassOp()const
		{
			return m_stStencilBack.m_ePassOp;
		}

	protected:
		/**
		 *\~english
		 *\brief		Creates the state used to save the current state
		 *\~french
		 *\brief		Crée l'état utilisé pour stocker l'état courant
		 */
		C3D_API void CreateCurrent();
		/**
		 *\~english
		 *\brief		Destroys the state used to save the current state
		 *\~french
		 *\brief		Détruit l'état utilisé pour stocker l'état courant
		 */
		C3D_API void DestroyCurrent();
		/**
		 *\~english
		 *\brief		Creates the state used to save the current state
		 *\~french
		 *\brief		Crée l'état utilisé pour stocker l'état courant
		 */
		C3D_API virtual DepthStencilStateSPtr DoCreateCurrent() = 0;

	protected:
		//!\~english Tells it has changed	\~french Dit que l'état a changé
		bool m_changed;
		//!\~english Tells whether the depth test is activated or not	\~french Dit si oui on non le test de profondeur est activé
		bool m_bDepthTest;
		//!\~english The depth test function	\~french La fonction du test de profondeur
		eDEPTH_FUNC m_eDepthFunc;
		//!\~english The near plane for depth test	\~french Le plan proche pour le test de profondeur
		double m_dDepthNear;
		//!\~english The far plane for depth test	\~french Le plan lointain pour le test de profondeur
		double m_dDepthFar;
		//!\~english Depth writing mask	\~french Masque d'écriture de la profondeur
		eWRITING_MASK m_eDepthMask;
		//!\~english Tells whether the stencil test is activated or not	\~french Dit si le test stencil est activé ou non
		bool m_bStencilTest;
		//!\~english The stencil read mask	\~french Le masque de lecture du stencil
		uint32_t m_uiStencilReadMask;
		//!\~english The stencil write mask	\~french Le masque d'écriture du stencil
		uint32_t m_uiStencilWriteMask;
		//!\~english Front buffer stencil function	\~french Fonction stencil du tampon de premier plan
		stSTENCIL m_stStencilFront;
		//!\~english Back buffer stencil function	\~french Fonction stencil du tampon d'arrière plan
		stSTENCIL m_stStencilBack;
		//!\~english	The internal global state used to commit only the changed states.	\~french	Etat interne global, utilisé pour n'appliquer que les changements d'état.
		static DepthStencilStateWPtr m_wCurrentState;
		//!\~english	Shared_pointer to the internal global state, to use reference counting for this static member.	\~french	Pointeur partag2 sur l'état interne global, utilisé pour avoir le comptage de références pour ce membre statique.
		DepthStencilStateSPtr m_currentState;
	};
}

#endif
