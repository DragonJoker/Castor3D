/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Depth and stencil buffers configurations class
	\~french
	\brief		Classe de configuration des buffers de profondeur et stencil
	*/
	class C3D_API DepthStencilState
	{
	protected:
		/*!
		\author 	Sylvain DOREMUS
		\version	0.7.0
		\date 		27/11/2013
		\~english
		\brief		Stencil function and operations description
		\~french
		\brief		Fonction et op�rations de stencil
		*/
		struct stSTENCIL
		{
			//!\~english The function	\~french La fonction
			eSTENCIL_FUNC m_eFunc;
			//!\~english The reference value for the function	\~french La valeur de r�f�rence pour la fonction
			uint32_t m_ref;
			//!\~english The operation on stencil test fail	\~french Op�ration faite en cas d'�chec du stencil test
			eSTENCIL_OP m_eFailOp;
			//!\~english The operation on deph test fail	\~french Op�ration faite en cas d'�chec du test de profondeur
			eSTENCIL_OP m_eDepthFailOp;
			//!\~english The operation on depth and stencil tests success	\~french Op�ration faite en cas de r�ussite des tests stencil et profondeur
			eSTENCIL_OP m_ePassOp;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		DepthStencilState();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~DepthStencilState();
		/**
		 *\~english
		 *\brief		Initialises the states
		 *\~french
		 *\brief		Initialise les �tats
		 */
		virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans the states
		 *\~french
		 *\brief		Nettoie les �tats
		 */
		virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Applies the states
		 *\~french
		 *\brief		Applique les �tats
		 */
		virtual bool Apply() = 0;
		/**
		 *\~english
		 *\brief		Defines the depth test status
		 *\param[in]	p_bEnable The new status
		 *\~french
		 *\brief		D�finit le statut du test de profondeur
		 *\param[in]	p_bEnable	Le nouveau statut
		 */
		inline void SetDepthTest( bool p_bEnable )
		{
			m_bChanged |= m_bDepthTest != p_bEnable;
			m_bDepthTest = p_bEnable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the depth test status
		 *\return		The status
		 *\~french
		 *\brief		R�cup�re le statut du test de profondeur
		 *\return		Le statut
		 */
		inline bool GetDepthTest()const
		{
			return m_bDepthTest;
		}
		/**
		 *\~english
		 *\brief		Defines the depth function
		 *\param[in]	p_eFunc The function
		 *\~french
		 *\brief		D�finit la fonction de profondeur
		 *\param[in]	p_eFunc	La fonction
		 */
		inline void SetDepthFunc( eDEPTH_FUNC p_eFunc )
		{
			m_bChanged |= m_eDepthFunc != p_eFunc;
			m_eDepthFunc = p_eFunc;
		}
		/**
		 *\~english
		 *\brief		Retrieves the depth function
		 *\return		The function
		 *\~french
		 *\brief		R�cup�re la fonction de profondeur
		 *\return		La fonction
		 */
		inline eDEPTH_FUNC GetDepthFunc()const
		{
			return m_eDepthFunc;
		}
		/**
		 *\~english
		 *\brief		Defines the depth function
		 *\param[in]	p_eFunc The function
		 *\~french
		 *\brief		D�finit la fonction de profondeur
		 *\param[in]	p_eFunc	La fonction
		 */
		inline void SetDepthRange( double p_dNear, double p_dFar )
		{
			m_bChanged |= p_dNear != m_dDepthNear || p_dFar != m_dDepthFar;
			m_dDepthNear = p_dNear;
			m_dDepthFar = p_dFar;
		}
		/**
		 *\~english
		 *\brief		Retrieves the depth near value
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re la valeur proche pour la profondeur
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
		 *\brief		R�cup�re la valeur lointaine pour la profondeur
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
		 *\brief		D�finit le masque d'�criture de la profondeur
		 *\param[in]	p_eMask	Le masque
		 */
		inline void SetDepthMask( eWRITING_MASK p_eMask )
		{
			m_bChanged |= m_eDepthMask != p_eMask;
			m_eDepthMask = p_eMask;
		}
		/**
		 *\~english
		 *\brief		Retrieves the depth writing mask
		 *\return		The mask
		 *\~french
		 *\brief		R�cup�re le masque d'�criture de la profondeur
		 *\return		Le masque
		 */
		inline eWRITING_MASK GetDepthMask()const
		{
			return m_eDepthMask;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil test status
		 *\param[in]	p_bEnable The new status
		 *\~french
		 *\brief		D�finit le statut du test de stencil
		 *\param[in]	p_bEnable	Le nouveau statut
		 */
		inline void SetStencilTest( bool p_bEnable )
		{
			m_bChanged |= true;
			m_bStencilTest = p_bEnable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil test status
		 *\return		The status
		 *\~french
		 *\brief		R�cup�re le statut du test de stencil
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
		 *\brief		D�finit le masque de lecture du stencil
		 *\param[in]	p_uiMask	Le masque
		 */
		inline void SetStencilReadMask( uint32_t p_uiMask )
		{
			m_bChanged |= m_uiStencilReadMask != p_uiMask;
			m_uiStencilReadMask = p_uiMask;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil read mask
		 *\return		The mask
		 *\~french
		 *\brief		R�cup�re le masque de lecture du stencil
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
		 *\brief		D�finit le masque d'�criture du stencil
		 *\param[in]	p_uiMask	Le masque
		 */
		inline void SetStencilWriteMask( uint32_t p_uiMask )
		{
			m_bChanged |= m_uiStencilWriteMask != p_uiMask;
			m_uiStencilWriteMask = p_uiMask;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil write mask
		 *\return		The mask
		 *\~french
		 *\brief		R�cup�re le masque d'�criture du stencil
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
		 *\brief		D�finit la valeur de r�f�rence de stencil pour les faces avant
		 *\param[in]	p_ref	La valeur de r�f�rence
		 */
		inline void SetStencilFrontRef( uint32_t p_ref )
		{
			m_bChanged |= m_stStencilFront.m_ref != p_ref;
			m_stStencilFront.m_ref = p_ref;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil reference value for front faces
		 *\return		The reference value
		 *\~french
		 *\brief		R�cup�re la valeur de r�f�rence de stencil pour les faces avant
		 *\return		La valeur de r�f�rence
		 */
		inline uint32_t GetStencilFrontRef()const
		{
			return m_stStencilFront.m_ref;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil function for front faces
		 *\param[in]	p_eFunc The function
		 *\~french
		 *\brief		D�finit la fonction de stencil pour les faces avant
		 *\param[in]	p_eFunc	La fonction
		 */
		inline void SetStencilFrontFunc( eSTENCIL_FUNC p_eFunc )
		{
			m_bChanged |= m_stStencilFront.m_eFunc != p_eFunc;
			m_stStencilFront.m_eFunc = p_eFunc;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil function for front faces
		 *\return		The function
		 *\~french
		 *\brief		R�cup�re la fonction de stencil pour les faces avant
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
		 *\brief		D�finit l'op�ration lors de l'�chec du test de stencil, pour les faces avant
		 *\param[in]	p_eOp	L'op�ration
		 */
		inline void SetStencilFrontFailOp( eSTENCIL_OP p_eOp )
		{
			m_bChanged |= m_stStencilFront.m_eFailOp != p_eOp;
			m_stStencilFront.m_eFailOp = p_eOp;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test failed, for front faces
		 *\return		The operation
		 *\~french
		 *\brief		R�cup�re l'op�ration lors de l'�chec du test de stencil, pour les faces avant
		 *\return		L'op�ration
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
		 *\brief		D�finit l'op�ration lors de la r�ussite du test de stencil mais de l'�chec du test de profondeur, pour les faces avant
		 *\param[in]	p_eOp	L'op�ration
		 */
		inline void SetStencilFrontDepthFailOp( eSTENCIL_OP p_eOp )
		{
			m_bChanged |= m_stStencilFront.m_eDepthFailOp != p_eOp;
			m_stStencilFront.m_eDepthFailOp = p_eOp;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test passes but depth test failed, for front faces
		 *\return		The operation
		 *\~french
		 *\brief		R�cup�re l'op�ration lors de la r�ussite du test de stencil mais de l'�chec du test de profondeur, pour les faces avant
		 *\return		L'op�ration
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
		 *\brief		D�finit l'op�ration lors de la r�ussite des tests de stencil et de profondeur, pour les faces avant
		 *\param[in]	p_eOp	L'op�ration
		 */
		inline void SetStencilFrontPassOp( eSTENCIL_OP p_eOp )
		{
			m_bChanged |= m_stStencilFront.m_ePassOp != p_eOp;
			m_stStencilFront.m_ePassOp = p_eOp;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil and depth tests passed, for front faces
		 *\return		The operation
		 *\~french
		 *\brief		R�cup�re l'op�ration lors de la r�ussite des tests de stencil et de profondeur, pour les faces avant
		 *\return		L'op�ration
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
		 *\brief		D�finit la valeur de r�f�rence de stencil pour les faces arri�res
		 *\param[in]	p_ref	La valeur de r�f�rence
		 */
		inline void SetStencilBackRef( uint32_t p_ref )
		{
			m_bChanged |= m_stStencilBack.m_ref != p_ref;
			m_stStencilBack.m_ref = p_ref;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil reference value for back faces
		 *\return		The reference value
		 *\~french
		 *\brief		R�cup�re la valeur de r�f�rence de stencil pour les faces arri�res
		 *\return		La valeur de r�f�rence
		 */
		inline uint32_t GetStencilBackRef()const
		{
			return m_stStencilBack.m_ref;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil function for front back
		 *\param[in]	p_eFunc The function
		 *\~french
		 *\brief		D�finit la fonction de stencil pour les faces arri�res
		 *\param[in]	p_eFunc	La fonction
		 */
		inline void SetStencilBackFunc( eSTENCIL_FUNC p_eFunc )
		{
			m_bChanged |= m_stStencilBack.m_eFunc != p_eFunc;
			m_stStencilBack.m_eFunc = p_eFunc;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stencil function for back faces
		 *\return		The function
		 *\~french
		 *\brief		R�cup�re la fonction de stencil pour les faces arri�res
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
		 *\brief		D�finit l'op�ration lors de l'�chec du test de stencil, pour les faces arri�res
		 *\param[in]	p_eOp	L'op�ration
		 */
		inline void SetStencilBackFailOp( eSTENCIL_OP p_eOp )
		{
			m_bChanged |= m_stStencilBack.m_eFailOp != p_eOp;
			m_stStencilBack.m_eFailOp = p_eOp;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test failed, for back faces
		 *\return		The operation
		 *\~french
		 *\brief		R�cup�re l'op�ration lors de l'�chec du test de stencil, pour les faces arri�res
		 *\return		L'op�ration
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
		 *\brief		D�finit l'op�ration lors de la r�ussite du test de stencil mais de l'�chec du test de profondeur, pour les faces arri�res
		 *\param[in]	p_eOp	L'op�ration
		 */
		inline void SetStencilBackDepthFailOp( eSTENCIL_OP p_eOp )
		{
			m_bChanged |= m_stStencilBack.m_eDepthFailOp != p_eOp;
			m_stStencilBack.m_eDepthFailOp = p_eOp;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test passes but depth test failed, for back faces
		 *\return		The operation
		 *\~french
		 *\brief		R�cup�re l'op�ration lors de la r�ussite du test de stencil mais de l'�chec du test de profondeur, pour les faces arri�res
		 *\return		L'op�ration
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
		 *\brief		D�finit l'op�ration lors de la r�ussite des tests de stencil et de profondeur, pour les faces arri�res
		 *\param[in]	p_eOp	L'op�ration
		 */
		inline void SetStencilBackPassOp( eSTENCIL_OP p_eOp )
		{
			m_bChanged |= m_stStencilBack.m_ePassOp != p_eOp;
			m_stStencilBack.m_ePassOp = p_eOp;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil and depth tests passed, for back faces
		 *\return		The operation
		 *\~french
		 *\brief		R�cup�re l'op�ration lors de la r�ussite des tests de stencil et de profondeur, pour les faces arri�res
		 *\return		L'op�ration
		 */
		inline eSTENCIL_OP GetStencilBackPassOp()const
		{
			return m_stStencilBack.m_ePassOp;
		}

	protected:
		//!\~english Tells it has changed	\~french Dit que l'�tat a chang�
		bool m_bChanged;
		//!\~english Tells whether the depth test is activated or not	\~french Dit si oui on non le test de profondeur est activ�
		bool m_bDepthTest;
		//!\~english The depth test function	\~french La fonction du test de profondeur
		eDEPTH_FUNC m_eDepthFunc;
		//!\~english The near plane for depth test	\~french Le plan proche pour le test de profondeur
		double m_dDepthNear;
		//!\~english The far plane for depth test	\~french Le plan lointain pour le test de profondeur
		double m_dDepthFar;
		//!\~english Depth writing mask	\~french Masque d'�criture de la profondeur
		eWRITING_MASK m_eDepthMask;
		//!\~english Tells whether the stencil test is activated or not	\~french Dit si le test stencil est activ� ou non
		bool m_bStencilTest;
		//!\~english The stencil read mask	\~french Le masque de lecture du stencil
		uint32_t m_uiStencilReadMask;
		//!\~english The stencil write mask	\~french Le masque d'�criture du stencil
		uint32_t m_uiStencilWriteMask;
		//!\~english Front buffer stencil function	\~french Fonction stencil du tampon de premier plan
		stSTENCIL m_stStencilFront;
		//!\~english Back buffer stencil function	\~french Fonction stencil du tampon d'arri�re plan
		stSTENCIL m_stStencilBack;
	};
}

#endif
