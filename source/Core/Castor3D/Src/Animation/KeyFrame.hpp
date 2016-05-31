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
#ifndef ___C3D_KEY_FRAME_H___
#define ___C3D_KEY_FRAME_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		The class which manages key frames
	\remark		Key frames are the frames where the animation must be at a precise state
	\~french
	\brief		Classe qui gère une key frame
	\remark		Les key frames sont les frames auxquelles une animation est dans un état précis
	*/
	class KeyFrame
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_timeIndex	When the key frame starts.
		 *\param[in]	p_translate	The translation at start time.
		 *\param[in]	p_rotate	The rotation at start time.
		 *\param[in]	p_scale		The scaling at start time.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_timeIndex	Quand la key frame commence.
		 *\param[in]	p_translate	La translation au temps de début.
		 *\param[in]	p_rotate	La rotation au temps de début.
		 *\param[in]	p_scale		L'échelle au temps de début.
		 */
		KeyFrame( real p_timeIndex = 0, Castor::Point3r const & p_translate = {}, Castor::Quaternion const & p_rotate = {}, Castor::Point3r const & p_scale = {} )
			: m_timeIndex( p_timeIndex )
			, m_translate( p_translate )
			, m_rotate( p_rotate )
			, m_scale( p_scale )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~KeyFrame()
		{
		}
		/**
		 *\~english
		 *\brief		Defines the wanted translation
		 *\param[in]	p_value	The value
		 *\~french
		 *\brief		Définit la translation voulue
		 *\param[in]	p_value	La valeur
		 */
		inline void SetTranslate( Castor::Point3r const & p_value )
		{
			m_translate = p_value;
		}
		/**
		 *\~english
		 *\brief		Defines the wanted rotation
		 *\param[in]	p_value	The value
		 *\~french
		 *\brief		Définit la rotation voulue
		 *\param[in]	p_value	La valeur
		 */
		inline void SetRotate( Castor::Quaternion const & p_value )
		{
			m_rotate = p_value;
		}
		/**
		 *\~english
		 *\brief		Defines the wanted scaling
		 *\param[in]	p_value	The value
		 *\~french
		 *\brief		Définit l'échelle voulue
		 *\param[in]	p_value	La valeur
		 */
		inline void SetScale( Castor::Point3r const & p_value )
		{
			m_scale = p_value;
		}
		/**
		 *\~english
		 *\return		The translation.
		 *\~french
		 *\return		La translation.
		 */
		inline Castor::Point3r const & GetTranslate()const
		{
			return m_translate;
		}
		/**
		 *\~english
		 *\return		The rotation.
		 *\~french
		 *\return		La rotation.
		 */
		inline Castor::Quaternion const & GetRotate()const
		{
			return m_rotate;
		}
		/**
		 *\~english
		 *\return		The scaling.
		 *\~french
		 *\return		L'échelle.
		 */
		inline Castor::Point3r const & GetScale()const
		{
			return m_scale;
		}
		/**
		 *\~english
		 *\brief		Retrieves the start time index
		 *\return		The time index
		 *\~french
		 *\brief		Récupère le temps de départ
		 *\return		Le temps
		 */
		inline real GetTimeIndex()const
		{
			return m_timeIndex;
		}
		/**
		 *\~english
		 *\brief		Defines the start time index
		 *\param[in]	p_value	The time index
		 *\~french
		 *\brief		Définit le temps de départ
		 *\param[in]	p_value	Le temps
		 */
		inline void SetTimeIndex( real p_value )
		{
			m_timeIndex = p_value;
		}

	protected:
		//!\~english The start time index.	\~french L'index de temps de début.
		real m_timeIndex;
		//!\~english The translation at start time.	\~french La translation à l'index de temps de début.
		Castor::Point3r m_translate;
		//!\~english The rotation at start time.	\~french La rotation à l'index de temps de début.
		Castor::Quaternion m_rotate;
		//!\~english The scaling at start time.	\~french L'échelle à l'index de temps de début.
		Castor::Point3r m_scale;

		friend class BinaryWriter< KeyFrame >;
		friend class BinaryParser< KeyFrame >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find eCHUNK_TYPE from a type.
	\remarks	Specialisation for KeyFrame.
	\~french
	\brief		Classe d'aide pour récupéer un eCHUNK_TYPE depuis un type.
	\remarks	Spécialisation pour KeyFrame.
	*/
	template<>
	struct ChunkTyper< KeyFrame >
	{
		static eCHUNK_TYPE const Value = eCHUNK_TYPE_KEYFRAME;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MovingObjectBase binary loader.
	\~english
	\brief		Loader binaire de MovingObjectBase.
	*/
	template<>
	class BinaryWriter< KeyFrame >
		: public BinaryWriterBase< KeyFrame >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to fill the chunk from specific data.
		 *\param[in]	p_obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	p_obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool DoWrite( KeyFrame const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MovingObjectBase binary loader.
	\~english
	\brief		Loader binaire de MovingObjectBase.
	*/
	template<>
	class BinaryParser< KeyFrame >
		: public BinaryParserBase< KeyFrame >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk.
		 *\param[out]	p_obj	The object to read.
		 *\param[in]	p_chunk	The chunk containing data.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk.
		 *\param[out]	p_obj	L'objet à lire.
		 *\param[in]	p_chunk	Le chunk contenant les données.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool DoParse( KeyFrame & p_obj )override;
	};
}

#endif

