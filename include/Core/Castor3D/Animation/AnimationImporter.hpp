/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimationImporter_H___
#define ___C3D_AnimationImporter_H___

#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimationModule.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include "Castor3D/Animation/Interpolator.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

#include <CastorUtils/Data/Path.hpp>

namespace c3d
{
	class AnimationImporter
		: public OwnedBy< Engine >
	{
	public:
		virtual ~AnimationImporter() = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	prefix	The prefix used for logging.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	prefix	Le préfixe utilisé pour le logging.
		 */
		C3D_API explicit AnimationImporter( Engine & engine
			, String const & prefix );
		/**
		 *\~english
		 *\brief		SkeletonAnimation import function.
		 *\param[out]	animation	Receives the imported data.
		 *\param[in]	file		The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SkeletonAnimation.
		 *\param[out]	animation	Reçoit les données importées.
		 *\param[in]	file		Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importData( SkeletonAnimation & animation
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		SkeletonAnimation import function.
		 *\param[out]	animation	Receives the imported data.
		 *\param[in]	pathFile	The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SkeletonAnimation.
		 *\param[out]	animation	Reçoit les données importées.
		 *\param[in]	pathFile	Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool importData( SkeletonAnimation & animation
			, Path const & pathFile
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		MeshAnimation import function.
		 *\param[out]	animation	Receives the imported data.
		 *\param[in]	file		The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de MeshAnimation.
		 *\param[out]	animation	Reçoit les données importées.
		 *\param[in]	file		Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importData( MeshAnimation & animation
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		MeshAnimation import function.
		 *\param[out]	animation	Receives the imported data.
		 *\param[in]	pathFile	The location of the file to import.
		 *\param[in]	parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de MeshAnimation.
		 *\param[out]	animation	Reçoit les données importées.
		 *\param[in]	pathFile	Le chemin vers le fichier à importer.
		 *\param[in]	parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool importData( MeshAnimation & animation
			, Path const & pathFile
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		SceneNodeAnimation import function.
		 *\param[out]	animation		Receives the imported data.
		 *\param[in]	file			The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SceneNodeAnimation.
		 *\param[out]	animation		Reçoit les données importées.
		 *\param[in]	file			Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importData( SceneNodeAnimation & animation
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		SceneNodeAnimation import function.
		 *\param[out]	animation		Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SceneNodeAnimation.
		 *\param[out]	animation		Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool importData( SceneNodeAnimation & animation
			, Path const & pathFile
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		TextureAnimation import function.
		 *\param[out]	animation		Receives the imported data.
		 *\param[in]	file			The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de TextureAnimation.
		 *\param[out]	animation		Reçoit les données importées.
		 *\param[in]	file			Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool importData( TextureAnimation & animation
			, ImporterFile * file
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		TextureAnimation import function.
		 *\param[out]	animation		Receives the imported data.
		 *\param[in]	pathFile		The location of the file to import.
		 *\param[in]	parameters		Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de TextureAnimation.
		 *\param[out]	animation		Reçoit les données importées.
		 *\param[in]	pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	parameters		Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool importData( TextureAnimation & animation
			, Path const & pathFile
			, Parameters const & parameters );

		String getPrefix()const noexcept
		{
			return m_prefix;
		}

		template< typename AnimationT, typename KeyFrameT, typename FuncT >
		static void synchroniseKeys( c3d::Map< c3d::Milliseconds, c3d::Point3f > const & translates
			, c3d::Map< c3d::Milliseconds, c3d::Quaternion > const & rotates
			, c3d::Map< c3d::Milliseconds, c3d::Point3f > const & scales
			, c3d::NodeTransform const & defaultTransform
			, uint32_t fps
			, c3d::Milliseconds minTime
			, c3d::Milliseconds maxTime
			, AnimationT & animation
			, c3d::Map< c3d::Milliseconds, c3d::UniquePtr< KeyFrameT > > & keyframes
			, FuncT fillKeyFrame )
		{
			c3d::InterpolatorT< c3d::Point3f, c3d::InterpolatorType::eLinear > pointInterpolator;
			c3d::InterpolatorT< c3d::Quaternion, c3d::InterpolatorType::eLinear > quatInterpolator;
			// Limit the key frames per second to 60, to spare RAM...
			auto wantedFps = std::min< int64_t >( 60, int64_t( fps ) );
			c3d::Milliseconds step{ 1000 / wantedFps };

			for ( auto time = minTime; time <= maxTime; time += step )
			{
				auto translate = AnimationImporter::doInterpolate( time, pointInterpolator, translates, defaultTransform.translate );
				auto rotate = AnimationImporter::doInterpolate( time, quatInterpolator, rotates, defaultTransform.rotate );
				auto scale = AnimationImporter::doInterpolate( time, pointInterpolator, scales, defaultTransform.scale );
				fillKeyFrame( AnimationImporter::doGetKeyFrame( time - minTime, animation, keyframes )
					, translate, rotate, scale );
			}
		}

	protected:
		/**
		 *\~english
		 *\brief		SkeletonAnimation import Function.
		 *\param[out]	skeleton	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SkeletonAnimation.
		 *\param[out]	skeleton	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool doImportSkeleton( SkeletonAnimation & skeleton ) = 0;
		/**
		 *\~english
		 *\brief		MeshAnimation import function.
		 *\param[out]	mesh	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de MeshAnimation.
		 *\param[out]	mesh	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool doImportMesh( MeshAnimation & mesh ) = 0;
		/**
		 *\~english
		 *\brief		SceneNodeAnimation import Function.
		 *\param[out]	node	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de SceneNodeAnimation.
		 *\param[out]	node	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool doImportNode( SceneNodeAnimation & node ) = 0;
		/**
		 *\~english
		 *\brief		TextureAnimation import Function.
		 *\param[out]	texture	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de TextureAnimation.
		 *\param[out]	texture	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool doImportTexture( TextureAnimation & texture ) = 0;

	private:
		template< typename KeyFrameT, typename AnimationT >
		static KeyFrameT & doGetKeyFrame( c3d::Milliseconds const & time
			, AnimationT & animation
			, c3d::Map< c3d::Milliseconds, c3d::UniquePtr< KeyFrameT > > & keyframes )
		{
			auto it = keyframes.find( time );
			if ( it == keyframes.end() )
				it = keyframes.emplace( time
					, c3d::makeUnique< KeyFrameT >( animation, time ) ).first;
			return *it->second;
		}

		template< typename DataT >
		static void doFindValue( c3d::Milliseconds time
			, typename c3d::Map< c3d::Milliseconds, DataT > const & map
			, typename c3d::Map< c3d::Milliseconds, DataT >::const_iterator & prv
			, typename c3d::Map< c3d::Milliseconds, DataT >::const_iterator & cur )
		{
			if ( map.empty() )
			{
				prv = map.end();
				cur = map.end();
			}
			else
			{
				cur = std::find_if( map.begin(), map.end()
					, [&time]( auto const & pair )
					{
							return pair.first > time;
					} );

				if ( cur == map.end() )
					--cur;

				prv = cur;

				if ( prv != map.begin() )
					prv--;
			}
		}

		template< typename DataT >
		static DataT doInterpolate( c3d::Milliseconds const & time
			, c3d::Interpolator< DataT > const & interpolator
			, c3d::Map< c3d::Milliseconds, DataT > const & values
			, DataT const & defaultValue )
		{
			if ( values.empty() )
				return defaultValue;

			if ( values.size() == 1 )
				return values.begin()->second;

			DataT result;
			auto prv = values.begin();
			auto cur = values.begin();
			AnimationImporter::doFindValue( time, values, prv, cur );

			if ( prv != cur )
			{
				auto dt = cur->first - prv->first;
				auto factor = float( ( time - prv->first ).count() ) / float( dt.count() );
				result = interpolator.interpolate( prv->second, cur->second, factor );
			}
			else
			{
				result = prv->second;
			}

			return result;
		}


	protected:
		String m_prefix;
		ImporterFile * m_file{};
		//!\~english Import configuration parameters.
		//!\~french Paramètres de configuration de l'import.
		Parameters m_parameters;
	};
}

#endif
