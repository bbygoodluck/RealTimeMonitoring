#ifndef NETWORKUSAGEGRAPHVIEW_H_INCLUDED
#define NETWORKUSAGEGRAPHVIEW_H_INCLUDED

#include "../interface/MemoryDC.h"
#include "../common/IUpdate.h"

class CNetworkUsageGraphView : public wxWindow, public IUpdate
{
   typedef struct _GRAPH_DATA_NODE {
		int _iXPos = 0;
		int _iYPos = 0;
		unsigned long _ulData = 0;
		wxColour _col = wxColour(0, 170, 0);
		wxPenStyle _penStyle = wxPENSTYLE_DOT;
		bool _bLineDraw = true;
		bool _bInData = true;
		_GRAPH_DATA_NODE* _next;
	} GRAPH_DATA_NODE;

	enum _CONST_ENUM {
		CONST_ONE_SECOND = 4,
		CONST_TEN_SECOND = CONST_ONE_SECOND * 10,
		CONST_ONE_SECOND_Y_HEIGHT = 3,
		CONST_TEN_SECOND_Y_HEIGHT = 6
	};

public:
	explicit CNetworkUsageGraphView(wxWindow* parent, const int nID, const wxSize& sz, long lStyle = CHILD_WINDOW_STYLE);
	~CNetworkUsageGraphView();

public:
	void ShadowStart();
	void ShadowStop();

private:
	void CreateHeadTailNode();

	void Clear(GRAPH_DATA_NODE* _node);
	void Update();

	void Render(wxDC* pDC);
	void UpdateTenSecondRuler();
	void DrawTenSecondRuler(wxDC* pDC);
	void DrawNetworkUsage(wxDC* pDC);
	void DrawData(wxDC* pDC, GRAPH_DATA_NODE* _head, GRAPH_DATA_NODE* _tail);

	void AddNode(bool IsIn, unsigned int ulData);

	bool CheckAndMaxBPS(unsigned long _value);
	void UpdateNodePosition(GRAPH_DATA_NODE* _head, GRAPH_DATA_NODE* _tail, bool _isIn);
	wxString GetBandWidth(unsigned long _ulbps, bool _bMaxDisp);

private:
	wxRect m_viewRect = wxRect(0, 0, 0, 0);
	wxRect m_viewUsageRect = wxRect(0, 0, 0, 0);
	//MemoryDC
	CMemoryDC m_memDC;
	wxFont font_graphview_;
	//Timer
//	wxTimer m_timer;
	//´«±ÝÄÃ·¯
	wxColour m_colRuler;
	//In Color
	wxColour m_colIn = wxColour(0, 149, 0);
	//Out Color
	wxColour m_colOut = wxColour(249, 124, 0);

	bool bCreatedHeadTailNode_ = false;

	GRAPH_DATA_NODE* ruler_headNode_ = nullptr;
	GRAPH_DATA_NODE* ruler_tailNode_ = nullptr;

	GRAPH_DATA_NODE* headNode_in_ = nullptr;
	GRAPH_DATA_NODE* tailNode_in_ = nullptr;

	GRAPH_DATA_NODE* headNode_out_ = nullptr;
	GRAPH_DATA_NODE* tailNode_out_ = nullptr;

	int nviewport_maxx_ = -1;
	int nviewport_maxy_ = -1;
	int nusageview_x_ = -1;
	int nusageview_y_ = -1;

	int addcount_ = -1;
	const int ten_secound_ = 10;

	unsigned long ulInData_ = 0;
	unsigned long ulInMaxbps_ = 0;

	unsigned long ulOutData_ = 0;
	unsigned long ulOutMaxbps_ = 0;

	unsigned long ulMaxbps_ = 100;

	bool IsMaxIn = true;
	bool IsChangedMaxbps = false;

	double dblDownSpeed_ = 0.0f;
	double dblUpSpeed_ = 0.0f;

	int initMaxbpstime_ = -1;
	wxString strInData_ = wxT("");
	wxString strOutData_ = wxT("");

private:
	void OnPaint(wxPaintEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnTimer(wxTimerEvent& event);

	void UpdateData(wxCommandEvent& event) override;
	wxDECLARE_EVENT_TABLE();

};

#endif // NETWORKUSAGEGRAPHVIEW_H_INCLUDED
