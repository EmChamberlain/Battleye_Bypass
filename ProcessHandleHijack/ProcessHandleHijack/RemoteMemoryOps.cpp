#include "RemoteMemoryOps.hpp"

using namespace std;

int HandleGatewayClient::ConnectPipe() {
	while (1) { // Try to open a named pipe; wait for it, if necessary. 
		m_pipeHandle = CreateFile(m_lpszPipename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (m_pipeHandle != INVALID_HANDLE_VALUE)
			break; // Break if the pipe handle is valid. 

		if (GetLastError() != ERROR_PIPE_BUSY) { // Exit if an error other than ERROR_PIPE_BUSY occurs. 
			cout << "Could not open pipe. GetLastError: " << dec << GetLastError() << endl;
			return -1;
		}

		// All pipe instances are busy, so wait. 
		if (!WaitNamedPipe(m_lpszPipename, PIPEWAITTIMOUTIFBUSY)) {
			cout << "Could not open pipe: 20 second wait timed out." << endl;
			return -1;
		}
	}

	cout << "Pipe connected." << endl;

	HandleGatewayClient::SetPipeMode(PIPE_READMODE_BYTE);

	return 0;
}

int HandleGatewayClient::SetPipeMode(DWORD mode) {
	// Should be PIPE_READMODE_BYTE or PIPE_READMODE_MESSAGE
	// See SetNamedPipeHandleState on MSDN for more info: https://msdn.microsoft.com/en-us/library/windows/desktop/aa365787(v=vs.85).aspx
	BOOL fSuccess = FALSE;
	fSuccess = SetNamedPipeHandleState(m_pipeHandle, &mode, NULL, NULL);
	if (!fSuccess) {
		cout << "SetNamedPipeHandleState failed. GetLastError: " << GetLastError() << endl;
		return -1;
	}

	cout << "Pipe mode set." << endl;
	return 0;
}

bool HandleGatewayClient::RequestReadProcessMemory(RMORequestRPM rpmRequest) {
	cout << "Sending RPM request (" << sizeof(rpmRequest) << " bytes) message." << endl;

	BOOL fSuccess = FALSE;
	DWORD bytesWritten = 0;
	fSuccess = WriteFile(m_pipeHandle, &rpmRequest, sizeof(rpmRequest), &bytesWritten, NULL);

	if (!fSuccess) {
		cout << "WriteFile to pipe failed. GetLastError: " << dec << GetLastError() << endl;
		return false;
	}

	cout << "Request sent (" << bytesWritten << " bytes written)." << endl;

	return true;
}

RMOResponseRPM HandleGatewayClient::ReceiveReadProcessMemory() {
	RMOResponseRPM response;
	BOOL fSuccess = FALSE;
	DWORD bytesRead = 0;

	do { // Read from the pipe.
		fSuccess = ReadFile(m_pipeHandle, &response, sizeof(RMOResponseRPM), &bytesRead, NULL);

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
		cout << "ReadFile failed while receiving ReadProcessMemory output. GetLastError: " << dec << GetLastError() << endl;

	return response;
}

RMOResponseRPM HandleGatewayClient::RemoteReadProcessMemory(RMORequestRPM rpmRequest) {
	RMOResponseRPM response;

	if (HandleGatewayClient::RequestReadProcessMemory(rpmRequest)) {
		response = HandleGatewayClient::ReceiveReadProcessMemory();
	}

	return response;
}

BOOL HandleGatewayClient::DisconnectPipe() {
	return CloseHandle(m_pipeHandle);
}