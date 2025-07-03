#pragma once

#include "lvgl.h"
#include "SwipePageBase.h"
#include "PanelLabelValue.h"

///////////////////////////////////////////////////////////////////////////////
// This is a page with a number of controls on it is added to a scrolling panel
class SwipePageGps : public SwipePageBase
{
private:
	PanelLabelValue _panelLabelValue1; // Create an instance of PanelLabelValue
	PanelLabelValue _panelLabelValue2; // Create an instance of PanelLabelValue
	PanelLabelValue _panelLabelValue3; // Create an instance of PanelLabelValue

public:
	void Create(lv_obj_t *parentGroupPanel)
	{
		CreatePanel(parentGroupPanel, "GPS Status", 0xFF00FF);							   // Create the panel for this page
		_panelLabelValue1.Create(_uiPanelPage, "TTime", "00:00:00");					   // Create a panel with label and value
		_panelLabelValue2.Create(_uiPanelPage, "Other", "00:00:00");					   // Create a panel with label and value
		_panelLabelValue3.Create(_uiPanelPage, "More message Time", "00:00:00"); // Create a panel with label and value



		CreateTable(_uiPanelPage, lv_pct(100)); // Create a table with a height of 200 pixels
		AppendRowTitle("Name", TblFormat::Highlight); 
		AppendRowTitle("Apple");
		AppendRowTitle("Banana",TblFormat::Bold | TblFormat::Right);
		AppendRowTitle("Lemon");	
		AppendRowTitle("Grape");
		AppendRowTitle("Melon");
		AppendRowTitle("Peach");
		AppendRowTitle("Nuts");

		//Fill the first column


		SetTableValue( 0, "Price");
		SetTableValue( 1, "$7");
		SetTableValue( 2, "$7");
		SetTableValue( 3, "$6");
		SetTableValue( 4, "If the lvgl library is installed online, you need to copy the demos folder to src");
//		lv_table_set_cell_user_data(_table, 4, 1, (void *)0x1234);
		SetTableValue( 5, "");
		SetTableValue( 6, "$1");
		SetTableValue( 7, "$9");
	}
};
