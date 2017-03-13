/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_DEPTH_STENCIL_STATE_H___
#define ___C3D_DEPTH_STENCIL_STATE_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>

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
	class DepthStencilState
	{
	public:
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
		inline void SetDepthFunc( DepthFunc p_func )
		{
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
		inline DepthFunc GetDepthFunc()const
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
		inline void SetDepthMask( WritingMask p_eMask )
		{
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
		inline WritingMask GetDepthMask()const
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
		inline void SetStencilFrontFunc( StencilFunc p_func )
		{
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
		inline StencilFunc GetStencilFrontFunc()const
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
		inline void SetStencilFrontFailOp( StencilOp p_eOp )
		{
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
		inline StencilOp GetStencilFrontFailOp()const
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
		inline void SetStencilFrontDepthFailOp( StencilOp p_eOp )
		{
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
		inline StencilOp GetStencilFrontDepthFailOp()const
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
		inline void SetStencilFrontPassOp( StencilOp p_eOp )
		{
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
		inline StencilOp GetStencilFrontPassOp()const
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
		inline void SetStencilBackFunc( StencilFunc p_func )
		{
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
		inline StencilFunc GetStencilBackFunc()const
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
		inline void SetStencilBackFailOp( StencilOp p_eOp )
		{
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
		inline StencilOp GetStencilBackFailOp()const
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
		inline void SetStencilBackDepthFailOp( StencilOp p_eOp )
		{
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
		inline StencilOp GetStencilBackDepthFailOp()const
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
		inline void SetStencilBackPassOp( StencilOp p_eOp )
		{
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
		inline StencilOp GetStencilBackPassOp()const
		{
			return m_stStencilBack.m_ePassOp;
		}

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
		struct StencilConfiguration
		{
			//!\~english	The function.
			//!\~french		La fonction.
			StencilFunc m_eFunc{ StencilFunc::eAlways };
			//!\~english	The reference value for the function.
			//!\~french		La valeur de référence pour la fonction.
			uint32_t m_ref{ 0 };
			//!\~english	The operation on stencil test fail.
			//!\~french		Opération faite en cas d'échec du stencil test.
			StencilOp m_eFailOp{ StencilOp::eKeep };
			//!\~english	The operation on deph test fail.
			//!\~french		Opération faite en cas d'échec du test de profondeur.
			StencilOp m_eDepthFailOp{ StencilOp::eIncrement };
			//!\~english	The operation on depth and stencil tests success.
			//!\~french		Opération faite en cas de réussite des tests stencil et profondeur.
			StencilOp m_ePassOp{ StencilOp::eKeep };
		};

	protected:
		//!\~english	Tells whether the depth test is activated or not.
		//!\~french		Dit si oui on non le test de profondeur est activé.
		bool m_bDepthTest{ true };
		//!\~english	The depth test function.
		//!\~french		La fonction du test de profondeur.
		DepthFunc m_eDepthFunc{ DepthFunc::eLess };
		//!\~english	The near plane for depth test.
		//!\~french		Le plan proche pour le test de profondeur.
		double m_dDepthNear{ 0.0 };
		//!\~english	The far plane for depth test.
		//!\~french		Le plan lointain pour le test de profondeur.
		double m_dDepthFar{ 1.0 };
		//!\~english	Depth writing mask.
		//!\~french		Masque d'écriture de la profondeur.
		WritingMask m_eDepthMask{ WritingMask::eAll };
		//!\~english	Tells whether the stencil test is activated or not.
		//!\~french		Dit si le test stencil est activé ou non.
		bool m_bStencilTest{ false };
		//!\~english	The stencil read mask.
		//!\~french		Le masque de lecture du stencil.
		uint32_t m_uiStencilReadMask{ 0xFFFFFFFF };
		//!\~english	The stencil write mask.
		//!\~french		Le masque d'écriture du stencil.
		uint32_t m_uiStencilWriteMask{ 0xFFFFFFFF };
		//!\~english	Front buffer stencil function.
		//!\~french		Fonction stencil du tampon de premier plan.
		StencilConfiguration m_stStencilFront;
		//!\~english	Back buffer stencil function.
		//!\~french		Fonction stencil du tampon d'arrière plan.
		StencilConfiguration m_stStencilBack;
	};
}

#endif
