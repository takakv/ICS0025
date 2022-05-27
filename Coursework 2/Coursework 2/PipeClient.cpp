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
    if (!this->connected && this->command != "stop") return true;

    unsigned long nWritten = 0;

    if (!WriteFile(this->PipeHandle, this->command, strlen(this->command) + 1, &nWritten, NULL))
    {
        std::cout << "Unable to write into file, error " << GetLastError() << std::endl;
        this->serverError = true;
        return false;
    }

    if (nWritten != strlen(this->command) + 1)
    {
        std::cout << "Only " << nWritten << " bytes were written" << std::endl;
        return false;
    }

    return true;
}

bool PipeClient::DisconnectPipe()
{
    std::cout << "Disconnecting pipe" << std::endl;
    this->connected = false;
    return true;
}

bool PipeClient::GetConnectionStatus()
{
    return this->connected;
}

bool PipeClient::GetServerError()
{
    return this->serverError;
}

void PipeClient::SetServerError(bool state)
{
    this->serverError = state;
}

void PipeClient::SetCommand(const char* command)
{
    this->command = command;
}