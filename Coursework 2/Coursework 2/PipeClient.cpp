#include "PipeClient.h"

PipeClient::PipeClient()
{
    this->PipeHandle = nullptr;
    this->ExitEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
    this->HaveInput = CreateEventA(NULL, FALSE, FALSE, NULL);
}

bool PipeClient::ConnectPipe()
{
    this->PipeHandle = CreateFileA(
        this->lpszPipename,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (this->PipeHandle == INVALID_HANDLE_VALUE)
    {
        this->connected = false;
        return false;
    }

    this->connected = true;
    this->command = "ready";
    return true;
}

bool PipeClient::SendToServer()
{
    // "Abuse" of return value: if there is no connection, nothing is sent.
    if (!this->connected && this->command != "stop") return true;

    unsigned long nWritten = 0;

    if (!WriteFile(this->PipeHandle, this->command, strlen(this->command) + 1, &nWritten, NULL))
    {
        std::cout << "Unable to write to pipe. ";

        switch (GetLastError())
        {
        case ERROR_NO_DATA:
            printf("Reason: the pipe is being closed. (error %d)\n",
                ERROR_NO_DATA);
            break;
        case ERROR_PIPE_NOT_CONNECTED:
            printf("Reason: the pipe was disconnected. (error %d)\n",
                ERROR_PIPE_NOT_CONNECTED);
            break;
        default:
            std::cout << "Error: " << GetLastError() << std::endl;
        }

        this->connected = false;
        return false;
    }

    // Did the command reach the server correctly?
    if (nWritten != strlen(this->command) + 1)
    {
        printf("Wrote only %d bytes to file.\n", nWritten);
        return false;
    }

    return true;
}

bool PipeClient::ReadFromServer(char* reply, OVERLAPPED Overlapped, HANDLE hEvents[])
{
    unsigned long nRead = 0;

    if (ReadFile(this->PipeHandle, reply, BUFSIZE, &nRead, &Overlapped)) return true;

    int error = GetLastError();

    if (error == ERROR_IO_PENDING)
    {
        switch (WaitForMultipleObjects(2, hEvents, FALSE, TIMEOUT))
        {
        case WAIT_OBJECT_0:
            GetOverlappedResult(this->PipeHandle, &Overlapped, &nRead, FALSE);
            return true;
        case WAIT_OBJECT_0 + 1:
            std::cout << "Reading aborted" << std::endl;
            return false;
        case WAIT_TIMEOUT:
            printf("Server timed out: nothing was received within %dms.\n", TIMEOUT);
            return false;
        default:
            error = GetLastError();
            break;
        }
    }

    std::cout << "Reading from pipe failed. ";
    switch (error)
    {
    case ERROR_PIPE_NOT_CONNECTED:
        std::cout << "Reason: pipe disconnected. ";
        break;
    case ERROR_BROKEN_PIPE:
        std::cout << "Reason: pipe terminated. ";
        break;
    }
    printf("Error: %d\n", error);

    return false;
}

void PipeClient::DisconnectPipe()
{
    std::cout << "Disconnecting pipe." << std::endl;
    this->connected = false;
}

bool PipeClient::GetConnectionStatus()
{
    return this->connected;
}

void PipeClient::SetCommand(const char* command)
{
    this->command = command;
}