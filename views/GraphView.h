#ifndef LINEGRAPHVIEW_H_INCLUDED
#define LINEGRAPHVIEW_H_INCLUDED

#include "../interface/MemoryDC.h"
#include "../common/IUpdate.h"

class CGraphView : public wxWindow, public IUpdate
{
	typedef struct _GRAPH_DATA_NODE {
		int _iXPos = 0;
		int _iYPos = 0;
		unsigned long _ulData = 0;
		wxColour _col = wxColour(0, 170, 0);
		wxPenStyle _penStyle = wxPENSTYLE_DOT;
		bool _bLineDraw = true;
		_GRAPH_DATA_NODE* _next;
	} GRAPH_DATA_NODE;

	enum _CONST_ENUM {
		CONST_ONE_SECOND = 4,
		CONST_TEN_SECOND = CONST_ONE_SECOND * 10,
		CONST_ONE_SECOND_Y_HEIGHT = 3,
		CONST_TEN_SECOND_Y_HEIGHT = 6
	};

public:
	explicit CGraphView(wxWindow* parent, const int nID, const wxSize& sz, long lStyle = CHILD_WINDOW_STYLE);
	~CGraphView();

	void SetCoreIndex(unsigned int index) {
		coreindex_ = index;
	}

	unsigned int GetCoreIndex() { return coreindex_; }

private:
	void Render(wxDC* pDC);
	void ClearData();
	void DrawGraph(wxDC* pDC);
	void DrawPixelLine(wxDC* pDC, int x1, int y1, int x2, int y2, const wxColour& col);
	void DisplayCorePercent(wxDC* pDC);

	void UpdateGraphData();

	private:
	wxRect m_viewRect = wxRect(0, 0, 0, 0);
	//MemoryDC
	CMemoryDC m_memDC;
	wxFont font_graphview_;

	//눈금컬러
	wxColour m_colRuler;
	unsigned int coreindex_;

	GRAPH_DATA_NODE* headNode_ = nullptr;
	GRAPH_DATA_NODE* tailNode_ = nullptr;

	int nviewport_maxx_ = -1;
	int nviewport_maxy_ = -1;

	unsigned long percent_usage_ = 0;
	int addcount_ = -1;
	const int ten_secound_ = 10;

private:
	void OnPaint(wxPaintEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnSize(wxSizeEvent& event);

	void UpdateData(wxCommandEvent& event) override;
	wxDECLARE_EVENT_TABLE();

};

#endif // LINEGRAPHVIEW_H_INCLUDED
