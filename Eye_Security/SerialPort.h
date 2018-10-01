#include <Windows.h>
#include <atlstr.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 128

class CSerialPort
{
public:
	CSerialPort(void);
	virtual ~CSerialPort(void);

private:
	HANDLE  m_hComm;
	DCB     m_dcb;
	COMMTIMEOUTS m_CommTimeouts;
	bool    m_bPortReady;
	bool    m_bWriteRC;
	bool    m_bReadRC;
	DWORD   m_iBytesWritten;
	DWORD   m_iBytesRead;
	DWORD   m_dwBytesRead;

public:
	void ClosePort();
	bool ReadByte(char &resp);
	bool ReadByte(BYTE* &resp, UINT size);
	bool WriteByte(char bybyte);
	bool OpenPort(CString portname);
	bool SetCommunicationTimeouts(DWORD ReadIntervalTimeout,
		DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant,
		DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant);
	bool ConfigurePort(DWORD BaudRate, BYTE ByteSize, DWORD fParity,
		BYTE  Parity, BYTE StopBits);
};