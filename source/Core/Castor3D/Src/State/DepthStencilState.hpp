/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DEPTH_STENCIL_STATE_H___
#define ___C3D_DEPTH_STENCIL_STATE_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
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
		inline void setDepthTest( bool p_enable )
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
		inline bool getDepthTest()const
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
		inline void setDepthFunc( DepthFunc p_func )
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
		inline DepthFunc getDepthFunc()const
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
		inline void setDepthRange( double p_near, double p_far )
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
		inline double getDepthNear()const
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
		inline double getDepthFar()const
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
		inline void setDepthMask( WritingMask p_eMask )
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
		inline WritingMask getDepthMask()const
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
		inline void setStencilTest( bool p_enable )
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
		inline bool getStencilTest()const
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
		inline void setStencilReadMask( uint32_t p_uiMask )
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
		inline uint32_t getStencilReadMask()const
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
		inline void setStencilWriteMask( uint32_t p_uiMask )
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
		inline uint32_t getStencilWriteMask()const
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
		inline void setStencilFrontRef( uint32_t p_ref )
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
		inline uint32_t getStencilFrontRef()const
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
		inline void setStencilFrontFunc( StencilFunc p_func )
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
		inline StencilFunc getStencilFrontFunc()const
		{
			return m_stStencilFront.m_eFunc;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test failed, for front faces
		 *\param[in]	p_op The operation
		 *\~french
		 *\brief		Définit l'opération lors de l'échec du test de stencil, pour les faces avant
		 *\param[in]	p_op	L'opération
		 */
		inline void setStencilFrontFailOp( StencilOp p_op )
		{
			m_stStencilFront.m_failOp = p_op;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test failed, for front faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de l'échec du test de stencil, pour les faces avant
		 *\return		L'opération
		 */
		inline StencilOp getStencilFrontFailOp()const
		{
			return m_stStencilFront.m_failOp;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test passes but depth test failed, for front faces
		 *\param[in]	p_op The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur, pour les faces avant
		 *\param[in]	p_op	L'opération
		 */
		inline void setStencilFrontDepthFailOp( StencilOp p_op )
		{
			m_stStencilFront.m_depthFailOp = p_op;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test passes but depth test failed, for front faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur, pour les faces avant
		 *\return		L'opération
		 */
		inline StencilOp getStencilFrontDepthFailOp()const
		{
			return m_stStencilFront.m_depthFailOp;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil stencil and depth tests passed, for front faces
		 *\param[in]	p_op The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite des tests de stencil et de profondeur, pour les faces avant
		 *\param[in]	p_op	L'opération
		 */
		inline void setStencilFrontPassOp( StencilOp p_op )
		{
			m_stStencilFront.m_passOp = p_op;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil and depth tests passed, for front faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de la réussite des tests de stencil et de profondeur, pour les faces avant
		 *\return		L'opération
		 */
		inline StencilOp getStencilFrontPassOp()const
		{
			return m_stStencilFront.m_passOp;
		}
		/**
		 *\~english
		 *\brief		Defines the all the stencil test operations, for front faces
		 *\param[in]	p_fail		The stencil fail operation
		 *\param[in]	p_depthFail	The stencil pass, depth fail operation
		 *\param[in]	p_pass		The stencil pass, depth pass operation
		 *\~french
		 *\brief		Définit toutes les opération des tests de stencil, pour les faces avant
		 *\param[in]	p_fail		L'opération d'échec stencil
		 *\param[in]	p_depthFail	L'opération de réussite stencil, échec profondeur
		 *\param[in]	p_pass		L'opération de réussite stencil, réussite profondeur
		 */
		inline void setStencilFrontOps( StencilOp p_fail, StencilOp p_depthFail, StencilOp p_pass )
		{
			m_stStencilFront.m_failOp = p_fail;
			m_stStencilFront.m_depthFailOp = p_depthFail;
			m_stStencilFront.m_passOp = p_pass;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil reference value for back faces
		 *\param[in]	p_ref The reference value
		 *\~french
		 *\brief		Définit la valeur de référence de stencil pour les faces arrières
		 *\param[in]	p_ref	La valeur de référence
		 */
		inline void setStencilBackRef( uint32_t p_ref )
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
		inline uint32_t getStencilBackRef()const
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
		inline void setStencilBackFunc( StencilFunc p_func )
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
		inline StencilFunc getStencilBackFunc()const
		{
			return m_stStencilBack.m_eFunc;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test failed, for back faces
		 *\param[in]	p_op The operation
		 *\~french
		 *\brief		Définit l'opération lors de l'échec du test de stencil, pour les faces arrières
		 *\param[in]	p_op	L'opération
		 */
		inline void setStencilBackFailOp( StencilOp p_op )
		{
			m_stStencilBack.m_failOp = p_op;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test failed, for back faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de l'échec du test de stencil, pour les faces arrières
		 *\return		L'opération
		 */
		inline StencilOp getStencilBackFailOp()const
		{
			return m_stStencilBack.m_failOp;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test passes but depth test failed, for back faces
		 *\param[in]	p_op The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur, pour les faces arrières
		 *\param[in]	p_op	L'opération
		 */
		inline void setStencilBackDepthFailOp( StencilOp p_op )
		{
			m_stStencilBack.m_depthFailOp = p_op;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil test passes but depth test failed, for back faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur, pour les faces arrières
		 *\return		L'opération
		 */
		inline StencilOp getStencilBackDepthFailOp()const
		{
			return m_stStencilBack.m_depthFailOp;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil and depth tests passed, for back faces
		 *\param[in]	p_op The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite des tests de stencil et de profondeur, pour les faces arrières
		 *\param[in]	p_op	L'opération
		 */
		inline void setStencilBackPassOp( StencilOp p_op )
		{
			m_stStencilBack.m_passOp = p_op;
		}
		/**
		 *\~english
		 *\brief		Defines the all the stencil test operations, for back faces
		 *\param[in]	p_fail		The stencil fail operation
		 *\param[in]	p_depthFail	The stencil pass, depth fail operation
		 *\param[in]	p_pass		The stencil pass, depth pass operation
		 *\~french
		 *\brief		Définit toutes les opération des tests de stencil, pour les faces arrières
		 *\param[in]	p_fail		L'opération d'échec stencil
		 *\param[in]	p_depthFail	L'opération de réussite stencil, échec profondeur
		 *\param[in]	p_pass		L'opération de réussite stencil, réussite profondeur
		 */
		inline void setStencilBackOps( StencilOp p_fail, StencilOp p_depthFail, StencilOp p_pass )
		{
			m_stStencilBack.m_failOp = p_fail;
			m_stStencilBack.m_depthFailOp = p_depthFail;
			m_stStencilBack.m_passOp = p_pass;
		}
		/**
		 *\~english
		 *\brief		Retrieves the  operation when stencil and depth tests passed, for back faces
		 *\return		The operation
		 *\~french
		 *\brief		Récupère l'opération lors de la réussite des tests de stencil et de profondeur, pour les faces arrières
		 *\return		L'opération
		 */
		inline StencilOp getStencilBackPassOp()const
		{
			return m_stStencilBack.m_passOp;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil reference value
		 *\param[in]	p_ref The reference value
		 *\~french
		 *\brief		Définit la valeur de référence de stencil
		 *\param[in]	p_ref	La valeur de référence
		 */
		inline void setStencilRef( uint32_t p_ref )
		{
			m_stStencilFront.m_ref = p_ref;
			m_stStencilBack.m_ref = p_ref;
		}
		/**
		 *\~english
		 *\brief		Defines the stencil function for
		 *\param[in]	p_func The function
		 *\~french
		 *\brief		Définit la fonction de stencil
		 *\param[in]	p_func	La fonction
		 */
		inline void setStencilFunc( StencilFunc p_func )
		{
			m_stStencilFront.m_eFunc = p_func;
			m_stStencilBack.m_eFunc = p_func;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test failed
		 *\param[in]	p_op The operation
		 *\~french
		 *\brief		Définit l'opération lors de l'échec du test de stencil
		 *\param[in]	p_op	L'opération
		 */
		inline void setStencilFailOp( StencilOp p_op )
		{
			m_stStencilFront.m_failOp = p_op;
			m_stStencilBack.m_failOp = p_op;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil test passes but depth test failed
		 *\param[in]	p_op The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite du test de stencil mais de l'échec du test de profondeur
		 *\param[in]	p_op	L'opération
		 */
		inline void setStencilDepthFailOp( StencilOp p_op )
		{
			m_stStencilFront.m_depthFailOp = p_op;
			m_stStencilBack.m_depthFailOp = p_op;
		}
		/**
		 *\~english
		 *\brief		Defines the operation when stencil stencil and depth tests passed
		 *\param[in]	p_op The operation
		 *\~french
		 *\brief		Définit l'opération lors de la réussite des tests de stencil et de profondeur
		 *\param[in]	p_op	L'opération
		 */
		inline void setStencilPassOp( StencilOp p_op )
		{
			m_stStencilFront.m_passOp = p_op;
			m_stStencilBack.m_passOp = p_op;
		}
		/**
		 *\~english
		 *\brief		Defines the all the stencil test operations
		 *\param[in]	p_fail		The stencil fail operation
		 *\param[in]	p_depthFail	The stencil pass, depth fail operation
		 *\param[in]	p_pass		The stencil pass, depth pass operation
		 *\~french
		 *\brief		Définit toutes les opération des tests de stencil
		 *\param[in]	p_fail		L'opération d'échec stencil
		 *\param[in]	p_depthFail	L'opération de réussite stencil, échec profondeur
		 *\param[in]	p_pass		L'opération de réussite stencil, réussite profondeur
		 */
		inline void setStencilOps( StencilOp p_fail, StencilOp p_depthFail, StencilOp p_pass )
		{
			m_stStencilFront.m_failOp = p_fail;
			m_stStencilFront.m_depthFailOp = p_depthFail;
			m_stStencilFront.m_passOp = p_pass;
			m_stStencilBack.m_failOp = p_fail;
			m_stStencilBack.m_depthFailOp = p_depthFail;
			m_stStencilBack.m_passOp = p_pass;
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
			StencilOp m_failOp{ StencilOp::eKeep };
			//!\~english	The operation on deph test fail.
			//!\~french		Opération faite en cas d'échec du test de profondeur.
			StencilOp m_depthFailOp{ StencilOp::eKeep };
			//!\~english	The operation on depth and stencil tests success.
			//!\~french		Opération faite en cas de réussite des tests stencil et profondeur.
			StencilOp m_passOp{ StencilOp::eKeep };
		};

	protected:
		//!\~english	Tells whether the depth test is activated or not.
		//!\~french		Dit si oui on non le test de profondeur est activé.
		bool m_bDepthTest{ false };
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
