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
#ifndef ___C3D_SKELETON_ANIMATION_BONE___
#define ___C3D_SKELETON_ANIMATION_BONE___

#include "SkeletonAnimationObject.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of SkeletonAnimationObject for Bones.
	\~french
	\brief		Implémentation de SkeletonAnimationObject pour les Bones.
	*/
	class SkeletonAnimationBone
		: public SkeletonAnimationObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_animation	The parent animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_animation	L'animation parente.
		 */
		C3D_API explicit SkeletonAnimationBone( SkeletonAnimation & p_animation );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SkeletonAnimationBone();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API SkeletonAnimationBone( SkeletonAnimationBone && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API SkeletonAnimationBone & operator=( SkeletonAnimationBone && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimationBone( SkeletonAnimationBone const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API SkeletonAnimationBone & operator=( SkeletonAnimationBone const & p_rhs ) = delete;
		/**
		 *\~english
		 *\return		The object name.
		 *\~french
		 *\return		Le nom de l'objet.
		 */
		virtual Castor::String const & GetName()const;
		/**
		 *\~english
		 *\brief		Sets the moving object.
		 *\param[in]	p_bone		The bone.
		 *\~french
		 *\brief		Définit l'objet mouvant.
		 *\param[in]	p_bone		L'os.
		 */
		inline void SetBone( BoneSPtr p_bone )
		{
			m_bone = p_bone;
		}
		/**
		 *\~english
		 *\return		The moving object.
		 *\~french
		 *\return		L'objet mouvant.
		 */
		inline BoneSPtr GetBone()const
		{
			return m_bone.lock();
		}

	private:
		//!\~english	The bone affected by the animations.
		//!\~french		L'os affecté par les animations.
		BoneWPtr m_bone;

		friend class BinaryWriter< SkeletonAnimationBone >;
		friend class BinaryParser< SkeletonAnimationBone >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for SkeletonAnimationBone.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour SkeletonAnimationBone.
	*/
	template<>
	struct ChunkTyper< SkeletonAnimationBone >
	{
		static ChunkType const Value = ChunkType::eSkeletonAnimationBone;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MovingBone binary loader.
	\~english
	\brief		Loader binaire de MovingBone.
	*/
	template<>
	class BinaryWriter< SkeletonAnimationBone >
		: public BinaryWriterBase< SkeletonAnimationBone >
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
		C3D_API bool DoWrite( SkeletonAnimationBone const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MovingBone binary loader.
	\~english
	\brief		Loader binaire de MovingBone.
	*/
	template<>
	class BinaryParser< SkeletonAnimationBone >
		: public BinaryParserBase< SkeletonAnimationBone >
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
		C3D_API bool DoParse( SkeletonAnimationBone & p_obj )override;
	};
}

#endif
