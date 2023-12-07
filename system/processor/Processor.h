#ifndef PROCESSOR_H_INCLUDED
#define PROCESSOR_H_INCLUDED

class CProcessorInfo
{
public:
	explicit CProcessorInfo();
	~CProcessorInfo();

	wxString GetProcessorName() const {
		return _strProcessorInfo;
	}

private:
	void Initialize();

private:
	wxString _strProcessorInfo;
};

#endif // PROCESSOR_H_INCLUDED
