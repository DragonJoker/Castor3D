/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Matroska___
#define ___Matroska___

#include <matroska/c/libmatroska_t.h>

namespace libmatroska
{
	class KaxTrackEntry;
	class KaxSegment;
	class KaxSeekHead;
	class KaxTracks;
	class KaxCues;
	class KaxCluster;
	class KaxBlockGroup;
}

namespace Castor
{	namespace Resource
{
	struct EBML_TRACK
	{
		libmatroska::KaxTrackEntry * m_track;
		uint64 m_size;
	};

	struct EBML_TRACK_FRAME
	{
		track_type m_track;
		unsigned char * m_data;
		unsigned int m_size;
		unsigned long long m_duration;
		unsigned long long m_timeIndex;

		EBML_TRACK_FRAME( track_type p_track, unsigned char * p_data, unsigned int p_size, unsigned long long p_duration, unsigned long long p_index)
			:	m_timeIndex( p_index),
				m_data( p_data),
				m_track( p_track),
				m_duration( p_duration),
				m_size( p_size)
		{
		}
	};

	typedef C3DMap(	track_type,	EBML_TRACK *)					TrackMap;
	typedef C3DMap(	track_type,	libmatroska::KaxBlockGroup *)	KaxBlockMap;

	class MatroskaFile
	{
	private:

		libmatroska::KaxSegment * m_segment;
		unsigned long long m_segmentSize;

		EbmlVoid m_dummy;
		libmatroska::KaxSeekHead * m_metaSeek;

		libmatroska::KaxTracks * m_tracks;
		unsigned int m_tracksSize;
		TrackMap m_trackEntries;

		libmatroska::KaxCues * m_cues;
		unsigned int m_cueSize;

		unsigned int m_infoSize;
		unsigned int m_tagsSize;
		unsigned int m_chapterSize;
		unsigned long long m_clusterSize;

		StdIOCallback * m_file;

		KaxBlockMap m_currentGroups;
		KaxBlockMap m_previousGroups;

		bool m_firstFrame;

		unsigned long long m_timeIndex;

		libmatroska::KaxCluster * m_currentCluster;
		unsigned long long m_currentClusterTimeIndex;

		unsigned long long m_trackUID;
		unsigned long long m_currentTrackNumber;

		unsigned long long m_duration;

		bool m_initialised;
		bool m_saveDefaults;

	public:
		MatroskaFile( const String & p_file, bool p_saveDefaults);
		~MatroskaFile();

		void Cleanup();
		void Initialise( const String & p_muxingAppName, const String & p_writingAppName);
		void AddVideoTrack( const String & p_codec, unsigned long long p_width, unsigned long long p_height, bool p_default);
		void AddAudioTrack( const String & p_codec, bool p_default);
		void AddSubsTrack( bool p_default);
		void EndCluster();
		void StartCluster();
		void AddFrame( const EBML_TRACK_FRAME & p_trackFrames);
		void Finalise();

		void SetMediaDuration( long long p_duration) { m_duration = p_duration; }

	protected:
		libmatroska::KaxTrackEntry * _addTrack( track_type p_type, const String & p_codec, bool p_default);
		void _writeHeader();
		void _initialiseSegment();
		void _writeSegmentInfos( const String & p_muxingAppName, const String & p_writingAppName);
	};
}
}

#endif