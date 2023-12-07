#ifndef __SPECTRUM_H__
#define __SPECTRUM_H__

class CSpectrumImpl;
class CSpectrum
{
public:
	CSpectrum();
	~CSpectrum();

	void AddSpectrum(int iCount = 0);
	void SetSpectrumRect(const wxRect& rc);
	size_t GetSize();

	wxVector<SPECTRUM_INFO *>::const_iterator GetItemIter(int iIndex);
	SPECTRUM_INFO* GetItemPtr(int iIndex);

private:
	std::unique_ptr<CSpectrumImpl> m_pSpectrumImpl;
};
#endif
