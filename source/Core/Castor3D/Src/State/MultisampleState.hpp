/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MULTISAMPLE_STATE_H___
#define ___C3D_MULTISAMPLE_STATE_H___

#include "Castor3DPrerequisites.hpp"

#include <Graphics/Colour.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		15/08/2016
	\~english
	\brief		Multisample configurations grouping class.
	\~french
	\brief		Classe regroupant les configurations de multi-échantillonnage.
	*/
	class MultisampleState
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves the multisample activation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'activation du multisample
		 *\return		La valeur
		 */
		inline bool getMultisample()const
		{
			return m_bMultisampled;
		}
		/**
		 *\~english
		 *\brief		Sets the multisample activation status
		 *\param[in]	p_enable	The new value
		 *\~french
		 *\brief		Définit le statut d'activation du multisample
		 *\param[in]	p_enable	La nouvelle valeur
		 */
		inline void setMultisample( bool p_enable )
		{
			m_bMultisampled = p_enable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha to coverage activation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'activation de l'alpha to coverage
		 *\return		La valeur
		 */
		inline bool isAlphaToCoverageEnabled()const
		{
			return m_bEnableAlphaToCoverage;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha to coverage activation status
		 *\param[in]	p_enable	The new value
		 *\~french
		 *\brief		Définit le statut d'activation de l'alpha to coverage
		 *\param[in]	p_enable	La nouvelle valeur
		 */
		inline void enableAlphaToCoverage( bool p_enable )
		{
			m_bEnableAlphaToCoverage = p_enable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the sample coverage mask
		 *\return		The value
		 *\~french
		 *\brief		Récupère le masque de couverture de samples
		 *\return		La valeur
		 */
		inline uint32_t getSampleCoverageMask()const
		{
			return m_uiSampleMask;
		}
		/**
		 *\~english
		 *\brief		Sets the sample coverage mask
		 *\param[in]	p_uiMask	The new value
		 *\~french
		 *\brief		Définit le masque de couverture de samples
		 *\param[in]	p_uiMask	La nouvelle valeur
		 */
		inline void setSampleCoverageMask( uint32_t p_uiMask )
		{
			m_uiSampleMask = p_uiMask;
		}

	protected:
		//!\~english	Tells if the alpha to coverage is enabled.
		//!\~french		Dit si l'alpha to coverage est activé.
		bool m_bEnableAlphaToCoverage{ false };
		//!\~english	The sample mask.
		//!\~french		Le masque d'échantillonage.
		uint32_t m_uiSampleMask{ 0xFFFFFFFF };
		//!\~english	The multisampling activation status.
		//!\~french		Le statut d'activation du multié-échantillonnage.
		bool m_bMultisampled{ false };
	};
}

#endif
