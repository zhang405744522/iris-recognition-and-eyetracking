#include "serialport.h"
#include <iostream>
#define _CRT_SECURE_NO_WARNINGS    // sprintf 보안 경고로 인한 컴파일 에러 방지

using namespace std;

CSerialPort::CSerialPort()
{
}

CSerialPort::~CSerialPort()
{
}

// OpenPort에서는 통신할 시리얼포트 번호를 받아 CreateFile을 통해 시리얼포트를 열게 된다. 
// m_hComm에는 통신을 할 포트의 정보가 들어가게 된다.
// 포트가 정상적으로 열리게 되면 True를 반환하고 포트를 열지 못했을 경우에는 False를 반환하게 된다.
bool CSerialPort::OpenPort(CString portname)
{
	m_hComm = CreateFile(L"//./" + portname, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0); //시리얼 포트를 오픈한다. 
	if (m_hComm == INVALID_HANDLE_VALUE)  //정상적으로 포트가 열렸는지 확인
	{
		return false;  //열리지 않았을 경우 false 반환
	}
	else
		return true;   //제대로 열렸을 경우 true 반환
}

// ConfigurePort()에서는 열린 포트에 대한 기본설정을 하게 된다. 
// 포트의 상태를 확인하여 포트가 정상적으로 열려 있는지 닫혀있는지 확인한 후 정상적으로 열려 있을 경우
// 포트 기본값에 대한 설정을 받은 인자값을 통해 하게 된다. 
bool CSerialPort::ConfigurePort(DWORD BaudRate, BYTE ByteSize, DWORD fParity,
	BYTE Parity, BYTE StopBits)
{
	if ((m_bPortReady = GetCommState(m_hComm, &m_dcb)) == 0) //포트의 상태를 확인. 정상적으로 열리지 않았을 경우 false 반환
	{
		printf("\nGetCommState Error\n");
		//MessageBox(L,L"Error", MB_OK + MB_ICONERROR);  
		CloseHandle(m_hComm);
		return false;
	}
	//포트의 대한 기본값을 설정
	m_dcb.BaudRate = BaudRate;
	m_dcb.ByteSize = ByteSize;
	m_dcb.Parity = Parity;
	m_dcb.StopBits = StopBits;
	m_dcb.fBinary = true;
	m_dcb.fDsrSensitivity = false;
	m_dcb.fParity = fParity;
	m_dcb.fOutX = false;
	m_dcb.fInX = false;
	m_dcb.fNull = false;
	m_dcb.fAbortOnError = true;
	m_dcb.fOutxCtsFlow = false;
	m_dcb.fOutxDsrFlow = false;
	m_dcb.fDtrControl = DTR_CONTROL_DISABLE;
	m_dcb.fDsrSensitivity = false;
	m_dcb.fRtsControl = RTS_CONTROL_DISABLE;
	m_dcb.fOutxCtsFlow = false;
	m_dcb.fOutxCtsFlow = false;

	m_bPortReady = SetCommState(m_hComm, &m_dcb);  //포트 상태 확인

	if (m_bPortReady == 0)  //포트의 상태를 확인. 정상일 경우 true 반환 아닐 경우 false 반환
	{
		//MessageBox(L"SetCommState Error");  
		printf("SetCommState Error");
		CloseHandle(m_hComm);
		return false;
	}

	return true;
}

// 통신포트에 대한 Timeout을 설정하는 함수이다. 입력받은 값으로 Timeout값을 설정하게 되는데
// Timeout을 설정하고 싶지 않을 경우 모든 인자값을 0으로 주면 된다.
// 보통의 경우에는 모두 0으로 입력한다.
bool CSerialPort::SetCommunicationTimeouts(DWORD ReadIntervalTimeout,
	DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant,
	DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant) //통신 포트에 관한 Timeout 설정
{
	if ((m_bPortReady = GetCommTimeouts(m_hComm, &m_CommTimeouts)) == 0)
		return false;

	m_CommTimeouts.ReadIntervalTimeout = ReadIntervalTimeout; //통신할때 한바이트가 전송 후 다음 바이트가 전송될때까지의 시간
															  //통신에서 데이터를 읽을 때 Timeout을 사용할 것인지에 대한 여부
	m_CommTimeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;
	//통신에서 데이터를 전송할 때 Timeout을 사용할 것인지에 대한 여부
	m_CommTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;

	m_bPortReady = SetCommTimeouts(m_hComm, &m_CommTimeouts);  //포트 상태 확인

	if (m_bPortReady == 0) //포트 상태가 닫혀 있을 경우 false반환. 아닐 경우 true반환
	{
		//MessageBox(L"StCommTimeouts function failed",L"Com Port Error",MB_OK+MB_ICONERROR);  
		printf("\nStCommTimeouts function failed\n");
		CloseHandle(m_hComm);
		return false;
	}

	return true;
}

// 데이터를 전송할때 쓰는 WriteByte함수이다. 
// 입력받은 인자값을 WriteFile을 통해 포트로 전송하게 된다.
// 전송에 성공하면 true, 전송에 실패하면 false를 반환한다.
bool CSerialPort::WriteByte(char bybyte)
{
	//iBytesWritten=0;
	m_iBytesWritten = 0;

	// 입력한 문자를 다시 콘솔에 출력해주는 구문.
	//cout << bybyte << endl;

	if (WriteFile(m_hComm, &bybyte, sizeof(char), &m_iBytesWritten, NULL) == 0) //입력받은 값을 WriteFile을 통해 포트로 전송한다.
		return false;
	else
		return true;
}


// 포트에 있는 데이터를 읽어올때 쓰는 ReadByte함수이다. 
// ReadFile을 통해 포트에서 데이터를 읽어오며 1바이트씩 읽어오게 된다.
// 데이터를 읽어오는데 성공하면 true를 반환하고 실패했을 경우 false를 반환한다. 
bool CSerialPort::ReadByte(char &resp)
{
	BYTE rx;
	resp = 0;

	DWORD dwBytesTransferred = 0;

	if (ReadFile(m_hComm, &rx, 1, &dwBytesTransferred, 0)) //포트에 존재하는 데이터를 ReadFile을 통해 1바이트씩 읽어온다.
	{
		if (dwBytesTransferred == 1) //데이터를 읽어오는데 성공했을 경우
		{
			resp = rx;  //resp에 데이터를 저장하고 true 반환
			return true;
		}
	}

	return false; //실패했을 경우 false 반환
}

// readByte함수가 두개가 존재하는데 다른 하나는 ReadByte(BYTE* &resp, UINT size)로
// 인자값으로 읽어올 데이터의 크기를 size로 받아서 그 크기만큼 데이터를 읽어온다.
bool CSerialPort::ReadByte(BYTE* &resp, UINT size)
{
	DWORD dwBytesTransferred = 0;

	if (ReadFile(m_hComm, resp, size, &dwBytesTransferred, 0))
	{
		if (dwBytesTransferred == size)
			return true;
	}

	return false;
}

// ClosePort에서는 CloseHandle을 통해 포트를 닫는다.
void CSerialPort::ClosePort()
{
	CloseHandle(m_hComm); //포트를 닫는다.
	return;
}