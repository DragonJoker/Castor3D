/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TreeModel_HPP___
#define ___CTP_TreeModel_HPP___

#include "Aria/Prerequisites.hpp"

#include <wx/dataview.h>

namespace aria
{
	/*
	TreeModel
	Implements the following data model:
		Category	Name		RunDate		RunTime		Status/Ignored/OutOfCastorDate/OutOfSceneDate
	--------------------------------------------------------------------------------------------------
	1:	Common		001-test	2020-11-06	15:35:22	bitmap
	*/
	class TreeModel
		: public wxDataViewModel
	{
	public:
		enum class Column
		{
			eCategory,
			eName,
			eRunDate,
			eRunTime,
			eStatus,
			eCount,
		};

	public:
		TreeModel( Config const & config
			, Renderer renderer );
		~TreeModel();

		TreeModelNode * addCategory( Category category );
		TreeModelNode * addTest( DatabaseTest & test );
		void expandRoots( wxDataViewCtrl * view );
		void instantiate( wxDataViewCtrl * view );

		// helper method for wxLog
		uint32_t getTestId( wxDataViewItem const & item )const;
		uint32_t getRunId( wxDataViewItem const & item )const;
		std::string getName( wxDataViewItem const & item )const;
		db::Date getRunDate( wxDataViewItem const & item )const;
		db::Time getRunTime( wxDataViewItem const & item )const;
		TestStatus getStatus( wxDataViewItem const & item )const;
		std::string getRenderer( wxDataViewItem const & item )const;
		std::string getCategory( wxDataViewItem const & item )const;

		TreeModelNode * GetRootNode()const
		{
			return m_root;
		}

		// helper methods to change the model
		void Delete( wxDataViewItem const & item );

		// Custom comparison
		int Compare( wxDataViewItem const & item1
			, wxDataViewItem const & item2
			, unsigned int column
			, bool ascending )const override;

		// implementation of base class virtuals to define model
		unsigned int GetColumnCount()const override;
		wxString GetColumnType( unsigned int col )const override;
		void GetValue( wxVariant & variant
			, wxDataViewItem const & item
			, unsigned int col )const override;
		bool SetValue( wxVariant const & variant
			, wxDataViewItem const & item
			, unsigned int col ) override;
		wxDataViewItem GetParent( wxDataViewItem const & item )const override;
		bool IsContainer( wxDataViewItem const & item )const override;
		unsigned int GetChildren( wxDataViewItem const & parent
			, wxDataViewItemArray & array )const override;

	private:
		Config const & m_config;
		Renderer m_renderer;
		TreeModelNode * m_root;
		std::map< std::string, TreeModelNode * > m_categories;
	};
}

#endif
