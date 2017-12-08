/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonAnimationKeyFrame_H___
#define ___C3D_SkeletonAnimationKeyFrame_H___

#include "Animation/AnimationKeyFrame.hpp"

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	using TransformArray = std::vector< std::pair< SkeletonAnimationObject *, castor::Matrix4x4r > >;
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
	class SkeletonAnimationKeyFrame
		: public AnimationKeyFrame
		, public castor::OwnedBy< SkeletonAnimation >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	timeIndex	When the key frame starts.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	timeIndex	Quand la key frame commence.
		 */
		C3D_API SkeletonAnimationKeyFrame( SkeletonAnimation & skeletonAnimation
			, castor::Milliseconds const & timeIndex = 0_ms );
		/**
		 *\~english
		 *\brief		Adds an animation object.
		 *\param[in]	object		The animation object.
		 *\param[in]	translate	The translation at start time.
		 *\param[in]	rotate		The rotation at start time.
		 *\param[in]	scale		The scaling at start time.
		 *\~french
		 *\brief		Ajoute un objet d'animation.
		 *\param[in]	object		L'objet d'animation.
		 *\param[in]	translate	La translation au temps de début.
		 *\param[in]	rotate		La rotation au temps de début.
		 *\param[in]	scale		L'échelle au temps de début.
		*/
		C3D_API void addAnimationObject( SkeletonAnimationObject & object
			, castor::Point3r const & translate
			, castor::Quaternion const & rotate
			, castor::Point3r const & scale );
		/**
		 *\~english
		 *\brief		Adds an animation object.
		 *\param[in]	object		The animation object.
		 *\param[in]	transform	The transformation at start time.
		 *\~french
		 *\brief		Ajoute un objet d'animation.
		 *\param[in]	object		L'objet d'animation.
		 *\param[in]	transform	La transformation au temps de début.
		*/
		C3D_API void addAnimationObject( SkeletonAnimationObject & object
			, castor::Matrix4x4r const & transform );
		/**
		 *\~english
		 *\return		The iterator matching given animation object.
		 *\~french
		 *\return		L'itérateur correspondant à l'objet d'animation donné.
		 */
		C3D_API TransformArray::const_iterator find( SkeletonAnimationObject const & object )const;
		/**
		 *\~english
		 *\brief		Initialises the keyframe.
		 *\~french
		 *\brief		Initialise la keyframe.
		 */
		C3D_API void initialise()override;
		/**
		 *\~english
		 *\return		The beginning of the animation objects.
		 *\~french
		 *\return		Le début des objets d'animation.
		 */
		inline TransformArray::const_iterator begin()const
		{
			return m_cumulative.begin();
		}
		/**
		 *\~english
		 *\return		The beginning of the animation objects.
		 *\~french
		 *\return		Le début des objets d'animation.
		 */
		inline TransformArray::iterator begin()
		{
			return m_cumulative.begin();
		}
		/**
		 *\~english
		 *\return		The end of the animation objects.
		 *\~french
		 *\return		La fin des objets d'animation.
		 */
		inline TransformArray::const_iterator end()const
		{
			return m_cumulative.end();
		}
		/**
		 *\~english
		 *\return		The end of the animation objects.
		 *\~french
		 *\return		La fin des objets d'animation.
		 */
		inline TransformArray::iterator end()
		{
			return m_cumulative.end();
		}

	private:
		void doSetTimeIndex( castor::Milliseconds const & time )
		{
			m_timeIndex = time;
		}

	private:
		//!\~english	The transformations, per animation object.
		//!\~french		Les transformations, par objet d'animation.
		TransformArray m_transforms;
		//!\~english	The cumulative transformations, per animation object.
		//!\~french		Les transformations cumulatives, par objet d'animation.
		TransformArray m_cumulative;

		friend class BinaryParser< SkeletonAnimationKeyFrame >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date 		07/12/2017
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for SkeletonAnimationKeyFrame.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour SkeletonAnimationKeyFrame.
	*/
	template<>
	struct ChunkTyper< SkeletonAnimationKeyFrame >
	{
		static ChunkType const Value = ChunkType::eSkeletonAnimationKeyFrame;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date 		07/12/2017
	\~english
	\brief		SkeletonAnimationKeyFrame binary loader.
	\~english
	\brief		Loader binaire de SkeletonAnimationKeyFrame.
	*/
	template<>
	class BinaryWriter< SkeletonAnimationKeyFrame >
		: public BinaryWriterBase< SkeletonAnimationKeyFrame >
	{
	protected:
		/**
		 *\~english
		 *\brief		Function used to fill the chunk from specific data.
		 *\param[in]	obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doWrite( SkeletonAnimationKeyFrame const & obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date 		07/12/2017
	\~english
	\brief		SkeletonAnimationKeyFrame binary loader.
	\~english
	\brief		Loader binaire de SkeletonAnimationKeyFrame.
	*/
	template<>
	class BinaryParser< SkeletonAnimationKeyFrame >
		: public BinaryParserBase< SkeletonAnimationKeyFrame >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk.
		 *\param[out]	obj	The object to read.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk.
		 *\param[out]	obj	L'objet à lire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doParse( SkeletonAnimationKeyFrame & obj )override;
	};
}

#endif

