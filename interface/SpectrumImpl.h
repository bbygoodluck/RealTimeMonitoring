#ifndef __SPECTRUM_IMPL_H__
#define __SPECTRUM_IMPL_H__

class CSpectrumImpl
{
public:
	CSpectrumImpl();
	~CSpectrumImpl();

public:
	void AddSpectrum(int iCount = 0);
	void MakeSpectrum();

public:
	wxRect m_Rect;
	wxVector<SPECTRUM_INFO *> m_lstSpectrum;

private:
	void Clear();

protected:
	int m_Gap = 3;

private:
	int m_xPos;
	int m_yPos;
	int m_Height;
	size_t m_Size;
};
#endif
