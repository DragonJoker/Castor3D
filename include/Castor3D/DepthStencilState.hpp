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
#ifndef ___C3D_DepthStencilState___
#define ___C3D_DepthStencilState___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
		\brief		Fonction et opérations de stencil
		*/
		struct stSTENCIL
		{
			//!\~english	The function	\~french	La fonction
			eSTENCIL_FUNC m_eFunc;
			//!\~english	The operation on stencil test fail	\~french	Opération faite en cas d'échec du stencil test
			eSTENCIL_OP m_eFailOp;
			//!\~english	The operation on deph test fail	\~french	Opération faite en cas d'échec du test de profondeur
			eSTENCIL_OP m_eDepthFailOp;
			//!\~english	The operation on depth and stencil tests success	\~french	Opération faite en cas de réussite des tests stencil et profondeur
			eSTENCIL_OP m_ePassOp;
		};
		//!\~english	Tells it has changed	\~french	Dit que l'état a changé
		bool m_bChanged;
		//!\~english	Tells whether the depth test is activated or not	\~french	Dit si oui on non le test de profondeur est activé
		bool m_bDepthTest;
		//!\~english	The depth test function	\~french	La fonction du test de profondeur
		eDEPTH_FUNC m_eDepthFunc;
		//!\~english	The near plane for depth test	\~french	Le plan proche pour le test de profondeur
		double m_dDepthNear;
		//!\~english	The far plane for depth test	\~french	Le plan lointain pour le test de profondeur
		double m_dDepthFar;
		//!\~english	Depth test mask	\~french	Masque du test de profondeur
		eDEPTH_MASK m_eDepthMask;
		//!\~english	Tells whether the stencil test is activated or not	\~french	Dit si le test stencil est activé ou non
		bool m_bStencilTest;
		//!\~english	The stencil read mask	\~french	Le masque de lecture du stencil
		uint32_t m_uiStencilReadMask;
		//!\~english	The stencil write mask	\~french	Le masque d'écriture du stencil
		uint32_t m_uiStencilWriteMask;
		//!\~english	Front buffer stencil function	\~french	Fonction stencil du tampon de premier plan
		stSTENCIL m_stStencilFront;
		//!\~english	Back buffer stencil function	\~french	Fonction stencil du tampon d'arrière plan
		stSTENCIL m_stStencilBack;

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
		 *\brief		Initialise les états
		 */
		 virtual bool Initialise()=0;
		/**
		 *\~english
		 *\brief		Cleans the states
		 *\~french
		 *\brief		Nettoie les états
		 */
		 virtual void Cleanup()=0;
		/**
		 *\~english
		 *\brief		Applies the states
		 *\~french
		 *\brief		Applique les états
		 */
		 virtual bool Apply()=0;
		/**
		 *\~english
		 *\brief		Defines the depth test status
		 *\param[in]	p_bEnable The new status
		 *\~french
		 *\brief		Définit le statut du test de profondeur
		 *\param[in]	p_bEnable	Le nouveau statut
		 */
		inline void SetDepthTest( bool p_bEnable ) { m_bChanged |= m_bDepthTest != p_bEnable;m_bDepthTest = p_bEnable; }
		/**
		 *\~english
		 *\brief		Retrieves the depth test status
		 *\return		The status
		 *\~french
		 *\brief		Récupère le statut du test de profondeur
		 *\return		Le statut
		 */
		inline bool GetDepthTest() { return m_bDepthTest; }
		/**
		 *\~english
		 *\brief		Defines the depth function
		 *\param[in]	p_eFunc The function
		 *\~french
		 *\brief		Définit la fonction de profondeur
		 *\param[in]	p_eFunc	La fonction
		 */
		inline void SetDepthFunc( eDEPTH_FUNC p_eFunc ) { m_bChanged |= m_eDepthFunc != p_eFunc;m_eDepthFunc = p_eFunc; }
		/**
		 *\~english
		 *\brief		Retrieves the depth function
		 *\return		The function
		 *\~french
		 *\brief		Récupère la fonction de profondeur
		 *\return		La fonction
		 */
		inline eDEPTH_FUNC GetDepthFunc() { return m_eDepthFunc; }
		/**
		 *\~english
		 *\brief		Defines the depth function
		 *\param[in]	p_eFunc The function
		 *\~french
		 *\brief		Définit la fonction de profondeur
		 *\param[in]	p_eFunc	La fonction
		 */
		inline void SetDepthRange( double p_dNear, double p_dFar ) { m_bChanged |= p_dNear != m_dDepthNear || p_dFar != m_dDepthFar;m_dDepthNear = p_dNear;m_dDepthFar = p_dFar; }
		/**
		 *\~english
		 *\brief		Retrieves the depth near value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur proche pour la profondeur
		 *\return		La valeur
		 */
		inline double GetDepthNear() { return m_dDepthNear; }
		/**
		 *\~english
		 *\brief		Retrieves the depth far value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur lointaine pour la profondeur
		 *\return		La valeur
		 */
		inline double GetDepthFar() { return m_dDepthFar; }
		/**
		 *\~english
		 *\brief		Defines the depth mask
		 *\param[in]	p_eMask The mask
		 *\~french
		 *\brief		Définit la masque de profondeur
		 *\param[in]	p_eMask	Le masque
		 */
		inline void SetDepthMask( eDEPTH_MASK p_eMask ) { m_bChanged |= m_eDepthMask != p_eMask;m_eDepthMask = p_eMask; }
		/**
		 *\~english
		 *\brief		Retrieves the depth mask
		 *\return		The mask
		 *\~french
		 *\brief		Récupère le masque de profondeur
		 *\return		Le masque
		 */
		inline eDEPTH_MASK GetDepthMask() { return m_eDepthMask; }
		/**
		 *\~english
		 *\brief		Defines the stencil test status
		 *\param[in]	p_bEnable The new status
		 *\~french
		 *\brief		Définit le statut du test de stencil
		 *\param[in]	p_bEnable	Le nouveau statut
		 */
		inline void SetStencilTest( bool p_bEnable ) { m_bChanged |= true;m_bStencilTest = p_bEnable; }
		/**
		 *\~english
		 *\brief		Retrieves the stencil test status
		 *\return		The status
		 *\~french
		 *\brief		Récupère le statut du test de stencil
		 *\return		Le statut
		 */
		inline bool GetStencilTest() { return m_bStencilTest; }
		/**
		 *\~english
		 *\brief		Defines the stencil read mask
		 *\param[in]	p_uiMask The mask
		 *\~french
		 *\brief		Définit le masque de lecture du stencil
		 *\param[in]	p_uiMask	Le masque
		 */
		inline void SetStencilReadMask( uint32_t p_uiMask ) { m_bChanged |= m_uiStencilReadMask != p_uiMask;m_uiStencilReadMask = p_uiMask; }
		/**
		 *\~english
		 *\brief		Retrieves the stencil read mask
		 *\return		The mask
		 *\~french
		 *\brief		Récupère le masque de lecture du stencil
		 *\return		Le masque
		 */
		inline uint32_t GetStencilReadMask() { return m_uiStencilReadMask; }
		/**
		 *\~english
		 *\brief		Defines the stencil write mask
		 *\param[in]	p_uiMask The mask
		 *\~french
		 *\brief		Définit le masque d'écriture du stencil
		 *\param[in]	p_uiMask	Le masque
		 */
		inline void SetStencilWriteMask( uint32_t p_uiMask ) { m_bChanged |= m_uiStencilWriteMask != p_uiMask;m_uiStencilWriteMask = p_uiMask; }
		/**
		 *\~english
		 *\brief		Retrieves the stencil write mask
		 *\return		The mask
		 *\~french
		 *\brief		Récupère le masque d'écriture du stencil
		 *\return		Le masque
		 */
		inline uint32_t GetStencilWriteMask() { return m_uiStencilWriteMask; }
		/**
		 *\~english
		 *\brief		Defines the stencil function for front faces
		 *\param[in]	p_eFunc The function
		 *\~french
		 *\brief		Définit la fonction de stencil pour les faces avant
		 *\param[in]	p_eFunc	La fonction
		 */
		inline void SetStencilFrontFunc( eSTENCIL_FUNC p_eFunc ) { m_bChanged |= m_stStencilFront.m_eFunc != p_eFunc;m_stStencilFront.m_eFunc = p_eFunc; }
		/**
		 *\~english
		 *\brief		Retrieves the stencil function for front faces
		 *\return		The function
		 *\~french
		 *\brief		Récupère la fonction de stencil pour les faces avant
		 *\return		La fonction
		 */
		inline eSTENCIL_FUNC GetStencilFrontFunc() { return m_stStencilFront.m_eFunc; }
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test failed, for front faces
		 *\param[in]	p_eOp The operation
		 *\~french
		 *\brief		Définit l'opération lors de l'échec du test de stencil, pour les faces avant
		 *\param[in]	p_eOp	L'opération
		 */
		inline void SetStencilFrontFailOp( eSTENCIL_OP p_eOp ) { m_bChanged |= m_stStencilFront.m_eFailOp != p_eOp;m_stStencilFront.m_eFailOp = p_eOp; }
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test failed, for front faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de l'échec du test de stencil, pour les faces avant
		 *\return		L'opération
		 */
		inline eSTENCIL_OP GetStencilFrontFailOp() { return m_stStencilFront.m_eFailOp; }
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test passes but depth test failed, for front faces
		 *\param[in]	p_eOp The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur, pour les faces avant
		 *\param[in]	p_eOp	L'opération
		 */
		inline void SetStencilFrontDepthFailOp( eSTENCIL_OP p_eOp ) { m_bChanged |= m_stStencilFront.m_eDepthFailOp != p_eOp;m_stStencilFront.m_eDepthFailOp = p_eOp; }
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test passes but depth test failed, for front faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur, pour les faces avant
		 *\return		L'opération
		 */
		inline eSTENCIL_OP GetStencilFrontDepthFailOp() { return m_stStencilFront.m_eDepthFailOp; }
		/**
		 *\~english
		 *\brief		Defines the operation when stencil stencil and depth tests passed, for front faces
		 *\param[in]	p_eOp The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite des tests de stencil et de profondeur, pour les faces avant
		 *\param[in]	p_eOp	L'opération
		 */
		inline void SetStencilFrontPassOp( eSTENCIL_OP p_eOp ) { m_bChanged |= m_stStencilFront.m_ePassOp != p_eOp;m_stStencilFront.m_ePassOp = p_eOp; }
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil and depth tests passed, for front faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de la réussite des tests de stencil et de profondeur, pour les faces avant
		 *\return		L'opération
		 */
		inline eSTENCIL_OP GetStencilFrontPassOp() { return m_stStencilFront.m_ePassOp; }
		/**
		 *\~english
		 *\brief		Defines the stencil function for front back
		 *\param[in]	p_eFunc The function
		 *\~french
		 *\brief		Définit la fonction de stencil pour les faces arrières
		 *\param[in]	p_eFunc	La fonction
		 */
		inline void SetStencilBackFunc( eSTENCIL_FUNC p_eFunc ) { m_bChanged |= m_stStencilBack.m_eFunc != p_eFunc;m_stStencilBack.m_eFunc = p_eFunc; }
		/**
		 *\~english
		 *\brief		Retrieves the stencil function for back faces
		 *\return		The function
		 *\~french
		 *\brief		Récupère la fonction de stencil pour les faces arrières
		 *\return		La fonction
		 */
		inline eSTENCIL_FUNC GetStencilBackFunc() { return m_stStencilBack.m_eFunc; }
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test failed, for back faces
		 *\param[in]	p_eOp The operation
		 *\~french
		 *\brief		Définit l'opération lors de l'échec du test de stencil, pour les faces arrières
		 *\param[in]	p_eOp	L'opération
		 */
		inline void SetStencilBackFailOp( eSTENCIL_OP p_eOp ) { m_bChanged |= m_stStencilBack.m_eFailOp != p_eOp;m_stStencilBack.m_eFailOp = p_eOp; }
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test failed, for back faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de l'échec du test de stencil, pour les faces arrières
		 *\return		L'opération
		 */
		inline eSTENCIL_OP GetStencilBackFailOp() { return m_stStencilBack.m_eFailOp; }
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test passes but depth test failed, for back faces
		 *\param[in]	p_eOp The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur, pour les faces arrières
		 *\param[in]	p_eOp	L'opération
		 */
		inline void SetStencilBackDepthFailOp( eSTENCIL_OP p_eOp ) { m_bChanged |= m_stStencilBack.m_eDepthFailOp != p_eOp;m_stStencilBack.m_eDepthFailOp = p_eOp; }
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test passes but depth test failed, for back faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur, pour les faces arrières
		 *\return		L'opération
		 */
		inline eSTENCIL_OP GetStencilBackDepthFailOp() { return m_stStencilBack.m_eDepthFailOp; }
		/**
		 *\~english
		 *\brief		Defines the operation when stencil stencil and depth tests passed, for back faces
		 *\param[in]	p_eOp The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite des tests de stencil et de profondeur, pour les faces arrières
		 *\param[in]	p_eOp	L'opération
		 */
		inline void SetStencilBackPassOp( eSTENCIL_OP p_eOp ) { m_bChanged |= m_stStencilBack.m_ePassOp != p_eOp;m_stStencilBack.m_ePassOp = p_eOp; }
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil and depth tests passed, for back faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de la réussite des tests de stencil et de profondeur, pour les faces arrières
		 *\return		L'opération
		 */
		inline eSTENCIL_OP GetStencilBackPassOp() { return m_stStencilBack.m_ePassOp; }
	};
}

#pragma warning( pop )

#endif
