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
#ifndef ___C3D_Sampler___
#define ___C3D_Sampler___

#include "Castor3DPrerequisites.hpp"


#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/03/2013
	\version	0.7.0
	\~english
	\brief		Defines a sampler for a texture
	\~french
	\brief		Définit un sampler pour une texture
	*/
	class Sampler
		: public Castor::OwnedBy< Engine >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0
		\date		21/03/2014
		\~english
		\brief		Sampler loader
		\remark
		\~french
		\brief		Loader de Sampler
		*/
		class TextLoader
			: public Castor::TextLoader< Sampler >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextLoader( Castor::String const & p_tabs, Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief			Writes a sampler into a text file
			 *\param[in]		p_sampler	The sampler to save
			 *\param[in,out]	p_file		The file where to save the sampler
			 *\~french
			 *\brief			Ecrit un échantillonneur dans un fichier texte
			 *\param[in]		p_sampler	L'échantillonneur
			 *\param[in,out]	p_file		Le fichier
			 */
			C3D_API virtual bool operator()( Sampler const & p_sampler, Castor::TextFile & p_file );
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	    The sampler name
		 *\param[in]	p_engine	The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	    Le nom du sampler
		 *\param[in]	p_engine	Le moteur
		 */
		C3D_API Sampler( Engine & p_engine, Castor::String const & p_name = Castor::cuEmptyString );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Sampler();
		/**
		 *\~english
		 *\brief		Initialises the sampler
		 *\return		\p true if ok
		 *\~french
		 *\brief		Initialise le sampler
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanups the sampler
		 *\~french
		 *\brief		Nettoie le sampler
		 */
		C3D_API virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Applies the sampler
		 *\param[in]	p_index	The sampler index
		 *\return		\p true if ok
		 *\~french
		 *\brief		Applique le sampler
		 *\param[in]	p_index	L'index du sampler
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Bind( uint32_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		Removes the sampler
		 *\param[in]	p_index	The sampler index
		 *\~french
		 *\brief		Enlève le sampler
		 *\param[in]	p_index	L'index du sampler
		 */
		C3D_API virtual void Unbind( uint32_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the interpolation mode for given filter
		 *\param[in]	p_eFilter	The concerned filter
		 *\return		The interpolation mode
		 *\~french
		 *\brief		Récupère le mode d'interpolation pour le filtre donné
		 *\param[in]	p_eFilter	Le filtre concerné
		 *\return		Le mode d'interpolation
		 */
		inline InterpolationMode GetInterpolationMode( InterpolationFilter p_eFilter )const
		{
			return m_eInterpolationModes[uint32_t( p_eFilter )];
		}
		/**
		 *\~english
		 *\brief		Defines the interpolation mode for given filter
		 *\param[in]	p_eFilter	The concerned filter
		 *\param[in]	p_mode		The wanted mode
		 *\~french
		 *\brief		Définit le mode d'interpolation pour le filtre donné
		 *\param[in]	p_eFilter	Le filtre concerné
		 *\param[in]	p_mode		Le mode souhaité
		 */
		inline void SetInterpolationMode( InterpolationFilter p_eFilter, InterpolationMode p_mode )
		{
			m_eInterpolationModes[uint32_t( p_eFilter )] = p_mode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the wrapping mode for given dimension
		 *\param[in]	p_eUVW	The dimension
		 *\return		The wrapping mode
		 *\~french
		 *\brief		Récupère le mode de wrap pour la dimension donnée
		 *\param[in]	p_eUVW	La dimension
		 *\return		Le mode de wrap
		 */
		inline WrapMode GetWrappingMode( TextureUVW p_eUVW )const
		{
			return m_eWrapModes[uint32_t( p_eUVW )];
		}
		/**
		 *\~english
		 *\brief		Defines the wrapping mode for given dimension
		 *\param[in]	p_eUVW	The dimension
		 *\param[in]	p_mode	The wrapping mode
		 *\~french
		 *\brief		Définit le mode de wrap pour la dimension donnée
		 *\param[in]	p_eUVW	La dimension
		 *\param[in]	p_mode	Le mode de wrap
		 */
		inline void SetWrappingMode( TextureUVW p_eUVW, WrapMode p_mode )
		{
			m_eWrapModes[uint32_t( p_eUVW )] = p_mode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the maximal anisotropy filtering value
		 \return		The value
		 *\~french
		 *\brief		Récupère la valeur maximale de filtrage anisotropique
		 *\return		La valeur
		 */
		inline real GetMaxAnisotropy()const
		{
			return m_rMaxAnisotropy;
		}
		/**
		 *\~english
		 *\brief		Defines the maximal anisotropy filtering value
		 *\param[in]	p_rValue	The new value
		 *\~french
		 *\brief		Définit la valeur maximale de filtrage anisotropique
		 *\param[in]	p_rValue	La nouvelle valeur
		 */
		inline void SetMaxAnisotropy( real p_rValue )
		{
			m_rMaxAnisotropy = p_rValue;
		}
		/**
		 *\~english
		 *\brief		Retrieves the minimal LOD level
		 \return		The level
		 *\~french
		 *\brief		Récupère le niveau minimal pour le LOD
		 *\return		Le niveau
		 */
		inline real GetMinLod()const
		{
			return m_rMinLod;
		}
		/**
		 *\~english
		 *\brief		Defines the minimal LOD level
		 *\param[in]	p_rLod	The new value
		 *\~french
		 *\brief		Définit le niveau minimal pour le LOD
		 *\param[in]	p_rLod	La nouvelle valeur
		 */
		inline void SetMinLod( real p_rLod )
		{
			m_rMinLod = p_rLod;
		}
		/**
		 *\~english
		 *\brief		Retrieves the maximal LOD level
		 \return		The level
		 *\~french
		 *\brief		Récupère le niveau maximal pour le LOD
		 *\return		Le niveau
		 */
		inline real GetMaxLod()const
		{
			return m_rMaxLod;
		}
		/**
		 *\~english
		 *\brief		Defines the maximal LOD level
		 *\param[in]	p_rLod	The new value
		 *\~french
		 *\brief		Définit le niveau maximal pour le LOD
		 *\param[in]	p_rLod	La nouvelle valeur
		 */
		inline void SetMaxLod( real p_rLod )
		{
			m_rMaxLod = p_rLod;
		}
		/**
		 *\~english
		 *\brief		Retrieves the LOD bias
		 \return		The level
		 *\~french
		 *\brief		Récupère le LOD bias
		 *\return		Le niveau
		 */
		inline real GetLodBias()const
		{
			return m_rLodBias;
		}
		/**
		 *\~english
		 *\brief		Defines the LOD bias
		 *\param[in]	p_rLod	The new value
		 *\~french
		 *\brief		Définit le LOD bias
		 *\param[in]	p_rLod	La nouvelle valeur
		 */
		inline void SetLodBias( real p_rLod )
		{
			m_rLodBias = p_rLod;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture border colour
		 *\return		The colour
		 *\~french
		 *\brief		Récupère la couleur de bord de la texture
		 *\return		La couleur
		 */
		inline	Castor::Colour const & GetBorderColour()const
		{
			return m_clrBorderColour;
		}
		/**
		 *\~english
		 *\brief		Defines the texture border colour
		 *\param[in]	p_crColour	The new value
		 *\~french
		 *\brief		Définit la couleur de bord de la texture
		 *\param[in]	p_crColour	La nouvelle valeur
		 */
		inline	void SetBorderColour( Castor::Colour const & p_crColour )
		{
			m_clrBorderColour = p_crColour;
		}
		/**
		 *\~english
		 *\brief		Retrieves the sampler name
		 \return		The name
		 *\~french
		 *\brief		Récupère le nom de l'échantillonneur
		 *\return		Le nom
		 */
		inline Castor::String const & GetName()const
		{
			return m_name;
		}
		/**
		 *\~english
		 *\brief		Defines the sampler name
		 *\param[in]	p_name	The name
		 *\~french
		 *\brief		Récupère le nom de l'échantillonneur
		 *\param[in]	p_name	Le nom
		 */
		inline void SetName( Castor::String const & p_name )
		{
			m_name = p_name;
		}

	private:
		//!\~english Sampler interpolation modes	\~french Modes d'interpolation du sampler
		InterpolationMode m_eInterpolationModes[uint32_t( InterpolationFilter::Count )];
		//!\~english Sampler wrapping modes	\~french Modes de wrapping du sampler
		WrapMode m_eWrapModes[uint32_t( TextureUVW::Count )];
		//!\~english Minimal LOD Level	\~french Niveau de LOD minimal
		real m_rMinLod;
		//!\~english Maximal LOD Level	\~french Niveau de LOD maximal
		real m_rMaxLod;
		//!\~english The texture LOD offset	\~french Le décalage de Lod de la texture
		real m_rLodBias;
		//!\~english Texture border colour	\~french Couleur des bords de la texture
		Castor::Colour m_clrBorderColour;
		//!\~english Maximal anisotropic filtering value	\~french Valeur maximale pour le filtrage anisotropique
		real m_rMaxAnisotropy;
		//!\~english The sampler name	\~french Le nom de l'échantillonneur
		Castor::String m_name;
	};
}

#endif
