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
#ifndef ___C3D_MULTISAMPLE_STATE_H___
#define ___C3D_MULTISAMPLE_STATE_H___

#include "Castor3DPrerequisites.hpp"

#include <Graphics/Colour.hpp>
#include <Design/OwnedBy.hpp>

namespace Castor3D
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
		: public Castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API explicit MultisampleState( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~MultisampleState();
		/**
		 *\~english
		 *\brief		Retrieves the multisample activation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'acitvation du multisample
		 *\return		La valeur
		 */
		inline bool GetMultisample()const
		{
			return m_bMultisampled;
		}
		/**
		 *\~english
		 *\brief		Sets the multisample activation status
		 *\param[in]	p_enable	The new value
		 *\~french
		 *\brief		Définit le statut d'acitvation du multisample
		 *\param[in]	p_enable	La nouvelle valeur
		 */
		inline void SetMultisample( bool p_enable )
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
		inline bool IsAlphaToCoverageEnabled()const
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
		inline void EnableAlphaToCoverage( bool p_enable )
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
		inline uint32_t GetSampleCoverageMask()const
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
		inline void SetSampleCoverageMask( uint32_t p_uiMask )
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
