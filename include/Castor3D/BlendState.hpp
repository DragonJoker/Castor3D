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
#ifndef ___C3D_BlendState___
#define ___C3D_BlendState___

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
	\brief		Blending configurations grouping class
	\~french
	\brief		Classe regroupant les configurations de blend
	*/
	class C3D_API BlendState
	{
	protected:
		struct C3D_API stRT_BLEND_STATE
		{
			bool		m_bEnableBlend;
			eBLEND		m_eRgbSrcBlend;
			eBLEND		m_eRgbDstBlend;
			eBLEND_OP	m_eRgbBlendOp;
			eBLEND		m_eAlphaSrcBlend;
			eBLEND		m_eAlphaDstBlend;
			eBLEND_OP	m_eAlphaBlendOp;
			uint8_t		m_uiWriteMask;
		};


		bool								m_bChanged;
		bool								m_bEnableAlphaToCoverage;
		bool								m_bIndependantBlend;
		Castor::Colour						m_blendFactors;
		uint32_t							m_uiSampleMask;
		std::array< stRT_BLEND_STATE, 8 >	m_rtStates;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		BlendState();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~BlendState();
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
		 *\brief		Retrieves the alpha to coverage activation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'activation de l'alpha to coverage
		 *\return		La valeur
		 */
		inline bool IsAlphaToCoverageEnabled()const { return m_bEnableAlphaToCoverage; }
		/**
		 *\~english
		 *\brief		Sets the alpha to coverage activation status
		 *\param[in]	p_bEnable	The new value
		 *\~french
		 *\brief		Définit le statut d'activation de l'alpha to coverage
		 *\param[in]	p_bEnable	La nouvelle valeur
		 */
		inline void EnableAlphaToCoverage( bool p_bEnable ) { m_bChanged |= m_bEnableAlphaToCoverage != p_bEnable;m_bEnableAlphaToCoverage = p_bEnable; }
		/**
		 *\~english
		 *\brief		Retrieves the independant blending activation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'activation du blending indépendant
		 *\return		La valeur
		 */
		inline bool IsIndependantBlendEnabled()const { return m_bIndependantBlend; }
		/**
		 *\~english
		 *\brief		Sets the independant blending activation status
		 *\param[in]	p_bEnable	The new value
		 *\~french
		 *\brief		Définit le statut d'activation du blending indépendant
		 *\param[in]	p_bEnable	La nouvelle valeur
		 */
		inline void EnableIndependantBlend( bool p_bEnable ) { m_bChanged |= m_bIndependantBlend != p_bEnable;m_bIndependantBlend = p_bEnable; }
		/**
		 *\~english
		 *\brief		Retrieves the blend factors
		 *\return		The value
		 *\~french
		 *\brief		Récupère les facteurs de blend
		 *\return		La valeur
		 */
		inline const Castor::Colour & GetBlendFactors()const { return m_blendFactors; }
		/**
		 *\~english
		 *\brief		Sets the blend factors
		 *\param[in]	p_clFactors	The new value
		 *\~french
		 *\brief		Définit les facteurs de blend
		 *\param[in]	p_clFactors	La nouvelle valeur
		 */
		inline void SetBlendFactors( const Castor::Colour & p_clFactors ) { m_bChanged |= m_blendFactors != p_clFactors;m_blendFactors = p_clFactors; }
		/**
		 *\~english
		 *\brief		Retrieves the sample coverage mask
		 *\return		The value
		 *\~french
		 *\brief		Récupère le masque de couverture de samples
		 *\return		La valeur
		 */
		inline uint32_t GetSampleCoverageMask()const { return m_uiSampleMask; }
		/**
		 *\~english
		 *\brief		Sets the sample coverage mask
		 *\param[in]	p_uiMask	The new value
		 *\~french
		 *\brief		Définit le masque de couverture de samples
		 *\param[in]	p_uiMask	La nouvelle valeur
		 */
		inline void SetSampleCoverageMask( uint32_t p_uiMask ) { m_bChanged |= m_uiSampleMask != p_uiMask;m_uiSampleMask = p_uiMask; }
		/**
		 *\~english
		 *\brief		Retrieves the blending activation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'activation du blending
		 *\return		La valeur
		 */
		inline bool IsBlendEnabled( uint8_t p_uiIndex=0 )const { return m_rtStates[p_uiIndex].m_bEnableBlend; }
		/**
		 *\~english
		 *\brief		Sets the blending activation status
		 *\param[in]	p_bEnable	The new value
		 *\~french
		 *\brief		Définit le statut d'activation du blending
		 *\param[in]	p_bEnable	La nouvelle valeur
		 */
		inline void EnableBlend( bool p_bEnable, uint8_t p_uiIndex=0 ) { m_bChanged |= m_rtStates[p_uiIndex].m_bEnableBlend != p_bEnable;m_rtStates[p_uiIndex].m_bEnableBlend = p_bEnable; }
		/**
		 *\~english
		 *\brief		Retrieves the RGB source blending factor
		 *\return		The value
		 *\~french
		 *\brief		Récupère le facteur source RGB
		 *\return		La valeur
		 */
		inline eBLEND GetRgbSrcBlend( uint8_t p_uiIndex=0 )const { return m_rtStates[p_uiIndex].m_eRgbSrcBlend; }
		/**
		 *\~english
		 *\brief		Sets the RGB source blending factor
		 *\param[in]	p_eValue	The new value
		 *\~french
		 *\brief		Définit le facteur source RGB
		 *\param[in]	p_eValue	La nouvelle valeur
		 */
		inline void SetRgbSrcBlend( eBLEND p_eValue, uint8_t p_uiIndex=0 ) { m_bChanged |= m_rtStates[p_uiIndex].m_eRgbSrcBlend != p_eValue;m_rtStates[p_uiIndex].m_eRgbSrcBlend = p_eValue; }
		/**
		 *\~english
		 *\brief		Retrieves the RGB destination blending factor
		 *\return		The value
		 *\~french
		 *\brief		Récupère le facteur destination RGB
		 *\return		La valeur
		 */
		inline eBLEND GetRgbDstBlend( uint8_t p_uiIndex=0 )const { return m_rtStates[p_uiIndex].m_eRgbDstBlend; }
		/**
		 *\~english
		 *\brief		Sets the RGB destination blending factor
		 *\param[in]	p_eValue	The new value
		 *\~french
		 *\brief		Définit le facteur destination RGB
		 *\param[in]	p_eValue	La nouvelle valeur
		 */
		inline void SetRgbDstBlend( eBLEND p_eValue, uint8_t p_uiIndex=0 ) { m_bChanged |= m_rtStates[p_uiIndex].m_eRgbDstBlend != p_eValue;m_rtStates[p_uiIndex].m_eRgbDstBlend = p_eValue; }
		/**
		 *\~english
		 *\brief		Retrieves the RGB blending operation
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'opération de blend RGB
		 *\return		La valeur
		 */
		inline eBLEND_OP GetRgbBlendOp( uint8_t p_uiIndex=0 )const { return m_rtStates[p_uiIndex].m_eRgbBlendOp; }
		/**
		 *\~english
		 *\brief		Sets the RGB blending operation
		 *\param[in]	p_eValue	The new value
		 *\~french
		 *\brief		Définit l'opération de blend RGB
		 *\param[in]	p_eValue	La nouvelle valeur
		 */
		inline void SetRgbBlendOp( eBLEND_OP p_eValue, uint8_t p_uiIndex=0 ) { m_bChanged |= m_rtStates[p_uiIndex].m_eRgbBlendOp != p_eValue;m_rtStates[p_uiIndex].m_eRgbBlendOp = p_eValue; }
		/**
		 *\~english
		 *\brief		Retrieves the alpha source blending factor
		 *\return		The value
		 *\~french
		 *\brief		Récupère le facteur source alpha
		 *\return		La valeur
		 */
		inline eBLEND GetAlphaSrcBlend( uint8_t p_uiIndex=0 )const { return m_rtStates[p_uiIndex].m_eAlphaSrcBlend; }
		/**
		 *\~english
		 *\brief		Sets the alpha source blending factor
		 *\param[in]	p_eValue	The new value
		 *\~french
		 *\brief		Définit le facteur source alpha
		 *\param[in]	p_eValue	La nouvelle valeur
		 */
		inline void SetAlphaSrcBlend( eBLEND p_eValue, uint8_t p_uiIndex=0 ) { m_bChanged |= m_rtStates[p_uiIndex].m_eAlphaSrcBlend != p_eValue;m_rtStates[p_uiIndex].m_eAlphaSrcBlend = p_eValue; }
		/**
		 *\~english
		 *\brief		Retrieves the alpha destination blending factor
		 *\return		The value
		 *\~french
		 *\brief		Récupère le facteur destination alpha
		 *\return		La valeur
		 */
		inline eBLEND GetAlphaDstBlend( uint8_t p_uiIndex=0 )const { return m_rtStates[p_uiIndex].m_eAlphaDstBlend; }
		/**
		 *\~english
		 *\brief		Sets the alpha destination blending factor
		 *\param[in]	p_eValue	The new value
		 *\~french
		 *\brief		Définit le facteur destination alpha
		 *\param[in]	p_eValue	La nouvelle valeur
		 */
		inline void SetAlphaDstBlend( eBLEND p_eValue, uint8_t p_uiIndex=0 ) { m_bChanged |= m_rtStates[p_uiIndex].m_eAlphaDstBlend != p_eValue;m_rtStates[p_uiIndex].m_eAlphaDstBlend = p_eValue; }
		/**
		 *\~english
		 *\brief		Retrieves the RGB blending operation
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'opération de blend RGB
		 *\return		La valeur
		 */
		inline eBLEND_OP GetAlphaBlendOp( uint8_t p_uiIndex=0 )const { return m_rtStates[p_uiIndex].m_eAlphaBlendOp; }
		/**
		 *\~english
		 *\brief		Sets the RGB blending operation
		 *\param[in]	p_eValue	The new value
		 *\~french
		 *\brief		Définit l'opération de blend RGB
		 *\param[in]	p_eValue	La nouvelle valeur
		 */
		inline void SetAlphaBlendOp( eBLEND_OP p_eValue, uint8_t p_uiIndex=0 ) { m_bChanged |= m_rtStates[p_uiIndex].m_eAlphaBlendOp != p_eValue;m_rtStates[p_uiIndex].m_eAlphaBlendOp = p_eValue; }
		/**
		 *\~english
		 *\brief		Retrieves the render target write mask
		 *\return		The value
		 *\~french
		 *\brief		Récupère le masque d'écriture de la cible de rendu
		 *\return		La valeur
		 */
		inline uint8_t GetWriteMask( uint8_t p_uiIndex=0 )const { return m_rtStates[p_uiIndex].m_uiWriteMask; }
		/**
		 *\~english
		 *\brief		Sets the render target write mask
		 *\param[in]	p_byMask	The new value
		 *\~french
		 *\brief		Définit le masque d'écriture de la cible de rendu
		 *\param[in]	p_byMask	La nouvelle valeur
		 */
		inline void SetWriteMask( uint8_t p_byMask, uint8_t p_uiIndex=0 ) { m_bChanged |= m_rtStates[p_uiIndex].m_uiWriteMask != p_byMask;m_rtStates[p_uiIndex].m_uiWriteMask = p_byMask; }
	};
}

#pragma warning( pop )

#endif
