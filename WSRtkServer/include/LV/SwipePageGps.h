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
		CreatePanel(parentGroupPanel, "GPS Status", 0xFF00FF); // Create the panel for this page
		_panelLabelValue1.Create(_uiPanelPage, "TTime", "00:00:00");					   // Create a panel with label and value
		_panelLabelValue2.Create(_uiPanelPage, "Other", "00:00:00");					   // Create a panel with label and value
		_panelLabelValue3.Create(_uiPanelPage, "Tdsf kdhj sdhf dhf shf Time", "00:00:00"); // Create a panel with label and value
	}
};
