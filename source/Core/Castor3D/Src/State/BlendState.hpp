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
#ifndef ___C3D_BLEND_STATE_H___
#define ___C3D_BLEND_STATE_H___

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Colour.hpp>
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
	struct ElementProducer< BlendState, Castor::String >
	{
		using ElemPtr = std::shared_ptr< BlendState >;

		ElementProducer( Engine & p_engine )
			: m_engine{ p_engine }
		{
		}

		ElemPtr operator()( Castor::String const & p_key )
		{
			return m_engine.GetRenderSystem()->CreateBlendState();
		}

		Engine & m_engine;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Blending configurations grouping class
	\~french
	\brief		Classe regroupant les configurations de blend
	*/
	class BlendState
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
		C3D_API explicit BlendState( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~BlendState();
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
			m_changed |= m_bEnableAlphaToCoverage != p_enable;
			m_bEnableAlphaToCoverage = p_enable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the independant blending activation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'activation du blending indépendant
		 *\return		La valeur
		 */
		inline bool IsIndependantBlendEnabled()const
		{
			return m_bIndependantBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the independant blending activation status
		 *\param[in]	p_enable	The new value
		 *\~french
		 *\brief		Définit le statut d'activation du blending indépendant
		 *\param[in]	p_enable	La nouvelle valeur
		 */
		inline void EnableIndependantBlend( bool p_enable )
		{
			m_changed |= m_bIndependantBlend != p_enable;
			m_bIndependantBlend = p_enable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the blend factors
		 *\return		The value
		 *\~french
		 *\brief		Récupère les facteurs de blend
		 *\return		La valeur
		 */
		inline const Castor::Colour & GetBlendFactors()const
		{
			return m_blendFactors;
		}
		/**
		 *\~english
		 *\brief		Sets the blend factors
		 *\param[in]	p_clFactors	The new value
		 *\~french
		 *\brief		Définit les facteurs de blend
		 *\param[in]	p_clFactors	La nouvelle valeur
		 */
		inline void SetBlendFactors( const Castor::Colour & p_clFactors )
		{
			m_changed |= m_blendFactors != p_clFactors;
			m_blendFactors = p_clFactors;
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
			m_changed |= m_uiSampleMask != p_uiMask;
			m_uiSampleMask = p_uiMask;
		}
		/**
		 *\~english
		 *\brief		Retrieves the blending activation status
		 *\param[in]	p_index		The value index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'activation du blending
		 *\param[in]	p_index		L'indice de la valeur
		 *\return		La valeur
		 */
		inline bool IsBlendEnabled( uint8_t p_index = 0 )const
		{
			return m_rtStates[p_index].m_bEnableBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the blending activation status
		 *\param[in]	p_enable	The new value
		 *\param[in]	p_index		The value index
		 *\~french
		 *\brief		Définit le statut d'activation du blending
		 *\param[in]	p_enable	La nouvelle valeur
		 *\param[in]	p_index		L'indice de la valeur
		 */
		inline void EnableBlend( bool p_enable, uint8_t p_index = 0 )
		{
			m_changed |= m_rtStates[p_index].m_bEnableBlend != p_enable;
			m_rtStates[p_index].m_bEnableBlend = p_enable;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RGB source blending factor
		 *\param[in]	p_index		The value index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le facteur source RGB
		 *\param[in]	p_index		L'indice de la valeur
		 *\return		La valeur
		 */
		inline BlendOperand GetRgbSrcBlend( uint8_t p_index = 0 )const
		{
			return m_rtStates[p_index].m_eRgbSrcBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the RGB source blending factor
		 *\param[in]	p_eValue	The new value
		 *\param[in]	p_index		The value index
		 *\~french
		 *\brief		Définit le facteur source RGB
		 *\param[in]	p_eValue	La nouvelle valeur
		 *\param[in]	p_index		L'indice de la valeur
		 */
		inline void SetRgbSrcBlend( BlendOperand p_eValue, uint8_t p_index = 0 )
		{
			m_changed |= m_rtStates[p_index].m_eRgbSrcBlend != p_eValue;
			m_rtStates[p_index].m_eRgbSrcBlend = p_eValue;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RGB destination blending factor
		 *\param[in]	p_index		The value index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le facteur destination RGB
		 *\param[in]	p_index		L'indice de la valeur
		 *\return		La valeur
		 */
		inline BlendOperand GetRgbDstBlend( uint8_t p_index = 0 )const
		{
			return m_rtStates[p_index].m_eRgbDstBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the RGB destination blending factor
		 *\param[in]	p_eValue	The new value
		 *\param[in]	p_index		The value index
		 *\~french
		 *\brief		Définit le facteur destination RGB
		 *\param[in]	p_eValue	La nouvelle valeur
		 *\param[in]	p_index		L'indice de la valeur
		 */
		inline void SetRgbDstBlend( BlendOperand p_eValue, uint8_t p_index = 0 )
		{
			m_changed |= m_rtStates[p_index].m_eRgbDstBlend != p_eValue;
			m_rtStates[p_index].m_eRgbDstBlend = p_eValue;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RGB blending operation
		 *\param[in]	p_index		The value index
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'opération de blend RGB
		 *\param[in]	p_index		L'indice de la valeur
		 *\return		La valeur
		 */
		inline BlendOperation GetRgbBlendOp( uint8_t p_index = 0 )const
		{
			return m_rtStates[p_index].m_eRgbBlendOp;
		}
		/**
		 *\~english
		 *\brief		Sets the RGB blending operation
		 *\param[in]	p_eValue	The new value
		 *\param[in]	p_index		The value index
		 *\~french
		 *\brief		Définit l'opération de blend RGB
		 *\param[in]	p_eValue	La nouvelle valeur
		 *\param[in]	p_index		L'indice de la valeur
		 */
		inline void SetRgbBlendOp( BlendOperation p_eValue, uint8_t p_index = 0 )
		{
			m_changed |= m_rtStates[p_index].m_eRgbBlendOp != p_eValue;
			m_rtStates[p_index].m_eRgbBlendOp = p_eValue;
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha source blending factor
		 *\param[in]	p_index		The value index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le facteur source alpha
		 *\param[in]	p_index		L'indice de la valeur
		 *\return		La valeur
		 */
		inline BlendOperand GetAlphaSrcBlend( uint8_t p_index = 0 )const
		{
			return m_rtStates[p_index].m_eAlphaSrcBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha source blending factor
		 *\param[in]	p_eValue	The new value
		 *\param[in]	p_index		The value index
		 *\~french
		 *\brief		Définit le facteur source alpha
		 *\param[in]	p_eValue	La nouvelle valeur
		 *\param[in]	p_index		L'indice de la valeur
		 */
		inline void SetAlphaSrcBlend( BlendOperand p_eValue, uint8_t p_index = 0 )
		{
			m_changed |= m_rtStates[p_index].m_eAlphaSrcBlend != p_eValue;
			m_rtStates[p_index].m_eAlphaSrcBlend = p_eValue;
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha destination blending factor
		 *\param[in]	p_index		The value index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le facteur destination alpha
		 *\param[in]	p_index		L'indice de la valeur
		 *\return		La valeur
		 */
		inline BlendOperand GetAlphaDstBlend( uint8_t p_index = 0 )const
		{
			return m_rtStates[p_index].m_eAlphaDstBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha destination blending factor
		 *\param[in]	p_eValue	The new value
		 *\param[in]	p_index		The value index
		 *\~french
		 *\brief		Définit le facteur destination alpha
		 *\param[in]	p_eValue	La nouvelle valeur
		 *\param[in]	p_index		L'indice de la valeur
		 */
		inline void SetAlphaDstBlend( BlendOperand p_eValue, uint8_t p_index = 0 )
		{
			m_changed |= m_rtStates[p_index].m_eAlphaDstBlend != p_eValue;
			m_rtStates[p_index].m_eAlphaDstBlend = p_eValue;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RGB blending operation
		 *\param[in]	p_index		The value index
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'opération de blend RGB
		 *\param[in]	p_index		L'indice de la valeur
		 *\return		La valeur
		 */
		inline BlendOperation GetAlphaBlendOp( uint8_t p_index = 0 )const
		{
			return m_rtStates[p_index].m_eAlphaBlendOp;
		}
		/**
		 *\~english
		 *\brief		Sets the RGB blending operation
		 *\param[in]	p_value	The new value
		 *\param[in]	p_index	The value index
		 *\~french
		 *\brief		Définit l'opération de blend RGB
		 *\param[in]	p_value	La nouvelle valeur
		 *\param[in]	p_index	L'indice de la valeur
		 */
		inline void SetAlphaBlendOp( BlendOperation p_value, uint8_t p_index = 0 )
		{
			m_changed |= m_rtStates[p_index].m_eAlphaBlendOp != p_value;
			m_rtStates[p_index].m_eAlphaBlendOp = p_value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the render target write mask
		 *\param[in]	p_index	The value index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le masque d'écriture de la cible de rendu
		 *\param[in]	p_index	L'indice de la valeur
		 *\return		La valeur
		 */
		inline uint8_t GetWriteMask( uint8_t p_index = 0 )const
		{
			return m_rtStates[p_index].m_uiWriteMask;
		}
		/**
		 *\~english
		 *\brief		Sets the render target write mask
		 *\param[in]	p_value	The new value
		 *\param[in]	p_index	The value index
		 *\~french
		 *\brief		Définit le masque d'écriture de la cible de rendu
		 *\param[in]	p_value	La nouvelle valeur
		 *\param[in]	p_index	L'indice de la valeur
		 */
		inline void SetWriteMask( uint8_t p_value, uint8_t p_index = 0 )
		{
			m_changed |= m_rtStates[p_index].m_uiWriteMask != p_value;
			m_rtStates[p_index].m_uiWriteMask = p_value;
		}
		/**
		 *\~english
		 *\brief		Defines the colour writing mask
		 *\param[in]	p_r, p_g, p_b, p_a The mask for each colour component
		 *\~french
		 *\brief		Définit le masque d'écriture des couleurs
		 *\param[in]	p_r, p_g, p_b, p_a	Le masque pour chaque composante
		 */
		inline void SetColourMask( eWRITING_MASK p_r, eWRITING_MASK p_g, eWRITING_MASK p_b, eWRITING_MASK p_a )
		{
			m_changed |= m_eColourMask[0] != p_r;
			m_changed |= m_eColourMask[1] != p_g;
			m_changed |= m_eColourMask[2] != p_b;
			m_changed |= m_eColourMask[3] != p_a;
			m_eColourMask[0] = p_r;
			m_eColourMask[1] = p_g;
			m_eColourMask[2] = p_b;
			m_eColourMask[3] = p_a;
		}
		/**
		 *\~english
		 *\brief		Retrieves the red colour component writing mask
		 *\return		The mask
		 *\~french
		 *\brief		Récupère le masque d'écriture de la composante rouge
		 *\return		Le masque
		 */
		inline eWRITING_MASK GetColourMaskR()const
		{
			return m_eColourMask[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves the green colour component writing mask
		 *\return		The mask
		 *\~french
		 *\brief		Récupère le masque d'écriture de la composante verte
		 *\return		Le masque
		 */
		inline eWRITING_MASK GetColourMaskG()const
		{
			return m_eColourMask[1];
		}
		/**
		 *\~english
		 *\brief		Retrieves the blue colour component writing mask
		 *\return		The mask
		 *\~french
		 *\brief		Récupère le masque d'écriture de la composante bleue
		 *\return		Le masque
		 */
		inline eWRITING_MASK GetColourMaskB()const
		{
			return m_eColourMask[2];
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha colour component writing mask
		 *\return		The mask
		 *\~french
		 *\brief		Récupère le masque d'écriture de la composante alpha
		 *\return		Le masque
		 */
		inline eWRITING_MASK GetColourMaskA()const
		{
			return m_eColourMask[3];
		}

	protected:
		struct C3D_API stRT_BLEND_STATE
		{
			bool m_bEnableBlend;
			BlendOperand m_eRgbSrcBlend;
			BlendOperand m_eRgbDstBlend;
			BlendOperation m_eRgbBlendOp;
			BlendOperand m_eAlphaSrcBlend;
			BlendOperand m_eAlphaDstBlend;
			BlendOperation m_eAlphaBlendOp;
			uint8_t m_uiWriteMask;
		};
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
		C3D_API virtual BlendStateSPtr DoCreateCurrent() = 0;

	protected:
		//!\~english Tells if the blend state has changed	\~french Dit que l'état a changé
		bool m_changed{ true };
		//!\~english Tells if the alpha to coveage is enabled	\~french Dit si l'alpha to coverage est activé
		bool m_bEnableAlphaToCoverage{ false };
		//!\~english Tells ifthe independant blend states are activated	\~french Dit si les états indépendants de mélange sont activés
		bool m_bIndependantBlend{ false };
		//!\~english Le blend colour	\~french La couleur de mélange
		Castor::Colour m_blendFactors;
		//!\~english The sample mask	\~french Le masque d'échantillonage
		uint32_t m_uiSampleMask{ 0xFFFFFFFF };
		//!\~english the blend states	\~french Les états de mélange
		std::array< stRT_BLEND_STATE, 8 > m_rtStates;
		//!\~english	Colours writing mask	\~french	Masque d'écriture des couleurs
		eWRITING_MASK m_eColourMask[4];
		//!\~english	The internal global state used to commit only the changed states.	\~french	Etat interne global, utilisé pour n'appliquer que les changements d'état.
		static BlendStateWPtr m_wCurrentState;
		//!\~english	Shared_pointer to the internal global state, to use reference counting for this static member.	\~french	Pointeur partag2 sur l'état interne global, utilisé pour avoir le comptage de références pour ce membre statique.
		BlendStateSPtr m_currentState;
	};
}

#endif
