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
#ifndef ___C3D_Sampler___
#define ___C3D_Sampler___

#include "Castor3DPrerequisites.hpp"

#include "BinaryParser.hpp"

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
			: public Castor::Loader< Sampler, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextLoader( Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
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
		/*!
		\author		Sylvain DOREMUS
		\date		08/04/2014
		\~english
		\brief		Sampler loader
		\~english
		\brief		Loader de Sampler
		*/
		class C3D_API BinaryParser
			: public Castor3D::BinaryParser< Sampler >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'accès au dossier courant
			 */
			BinaryParser( Castor::Path const & p_path );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques
			 *\param[in]	p_obj	L'objet à écrire
			 *\param[out]	p_chunk	Le chunk à remplir
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			virtual bool Fill( Sampler const & p_obj, BinaryChunk & p_chunk )const;
			/**
			 *\~english
			 *\brief		Function used to retrieve specific data from the chunk
			 *\param[out]	p_obj	The object to read
			 *\param[in]	p_chunk	The chunk containing data
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
			 *\param[out]	p_obj	L'objet à lire
			 *\param[in]	p_chunk	Le chunk contenant les données
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			virtual bool Parse( Sampler & p_obj, BinaryChunk & p_chunk )const;
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
		C3D_API Sampler( Engine * p_engine, Castor::String const & p_name = Castor::cuEmptyString );
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
		 *\param[in]	p_eDimension	The texture dimension
		 *\param[in]	p_index		The sampler index
		 *\return		\p true if ok
		 *\~french
		 *\brief		Applique le sampler
		 *\param[in]	p_eDimension	La dimension de la texture
		 *\param[in]	p_index		L'index du sampler
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Bind( eTEXTURE_DIMENSION p_eDimension, uint32_t p_index ) = 0;
		/**
		 *\~english
		 *\brief		Removes the sampler
		 *\~french
		 *\brief		Enlève le sampler
		 */
		C3D_API virtual void Unbind() = 0;
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
		inline eINTERPOLATION_MODE GetInterpolationMode( eINTERPOLATION_FILTER p_eFilter )const
		{
			return m_eInterpolationModes[p_eFilter];
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
		inline void SetInterpolationMode( eINTERPOLATION_FILTER p_eFilter, eINTERPOLATION_MODE p_mode )
		{
			m_eInterpolationModes[p_eFilter] = p_mode;
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
		inline eWRAP_MODE GetWrappingMode( eTEXTURE_UVW p_eUVW )const
		{
			return m_eWrapModes[p_eUVW];
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
		inline void SetWrappingMode( eTEXTURE_UVW p_eUVW, eWRAP_MODE p_mode )
		{
			m_eWrapModes[p_eUVW] = p_mode;
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
		/**
		 *\~english
		 *\brief		Retrieves the Engine
		 *\~french
		 *\brief		Récupère l'Engine
		 */
		virtual Engine * GetEngine()const
		{
			return m_engine;
		}

	private:
		//!\~english The core engine	\~french Le moteur
		Engine * m_engine;
		//!\~english Sampler interpolation modes	\~french Modes d'interpolation du sampler
		eINTERPOLATION_MODE m_eInterpolationModes[eINTERPOLATION_FILTER_COUNT];
		//!\~english Sampler wrapping modes	\~french Modes de wrapping du sampler
		eWRAP_MODE m_eWrapModes[eTEXTURE_UVW_COUNT];
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
