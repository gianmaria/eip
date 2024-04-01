#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Winhttp.h>

#pragma comment(lib, "winhttp.lib")

#include <iostream>
#include <string>
#include <vector>
#include <print>

using std::cout;
using std::endl;
using std::println;
using std::wcout;

using str = std::string; // NOTE: all std::string are utf-8 encoded
using wstr = std::wstring;
using str_cref = std::string const &;
using wstr_cref = std::wstring const &;
template <typename T>
using vec = std::vector<T>;

str to_UTF8(wstr_cref wide_str)
{
    int size = WideCharToMultiByte(
        CP_UTF8, 0, wide_str.c_str(), -1,
        nullptr, 0, nullptr, nullptr);

    if (size == 0)
        return "";

    auto utf8_str = std::string(size, '\0');

    WideCharToMultiByte(
        CP_UTF8, 0, wide_str.c_str(), -1,
        &utf8_str[0], size, nullptr, nullptr);

    return utf8_str;
}

str last_error_as_string(DWORD last_error)
{
    auto constexpr buffer_count = 1024;
    WCHAR buffer[buffer_count]{};

    DWORD size = FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS |
            FORMAT_MESSAGE_MAX_WIDTH_MASK,
        NULL,
        last_error,
        NULL,
        (wchar_t *)&buffer,
        buffer_count,
        NULL);

    return to_UTF8(wstr(buffer, size));
}

int main()
{
    HINTERNET hSession = WinHttpOpen(
        L"A WinHTTP Example Program/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);

    if (hSession == NULL)
    {
        println("WinHttpOpen() failed: {}",
                last_error_as_string(GetLastError()));
        return 1;
    }

    HINTERNET hConnect = WinHttpConnect(
        hSession,
        L"api.ipify.org",
        INTERNET_DEFAULT_HTTPS_PORT,
        0);

    if (hConnect == NULL)
    {
        println("WinHttpConnect() failed: {}",
                last_error_as_string(GetLastError()));
        return 1;
    }

    const wchar_t *accept_types[] = {
        L"text/*",
        NULL};

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"GET",
        L"/",
        L"HTTP/1.1",
        WINHTTP_NO_REFERER,
        accept_types,
        WINHTTP_FLAG_SECURE);

    if (hRequest == NULL)
    {
        println("WinHttpOpenRequest() failed: {}",
                last_error_as_string(GetLastError()));
        return 1;
    }

    if (WinHttpSendRequest(
            hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0,
            0, 0) == FALSE)
    {
        println("WinHttpSendRequest() failed: {}",
                last_error_as_string(GetLastError()));
        return 1;
    }

    if (WinHttpReceiveResponse(hRequest, NULL) == FALSE)
    {
        println("WinHttpReceiveResponse() failed: {}",
                last_error_as_string(GetLastError()));
        return 1;
    }

    DWORD dwDownloaded = 0;
    str data;
    DWORD dwSize;
    do
    {
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
        {
            break;
        }

        str buffer(dwSize, '\0');
        
        if (!WinHttpReadData(
                hRequest,
                (LPVOID)buffer.data(),
                dwSize, 
                &dwDownloaded))
        {
            break;
        }
        
        //std::cout << "  Size: " << dwSize << std::endl;
        //std::cout << "  Downloaded: " << dwDownloaded << std::endl;

        data.append(buffer);

    } while (dwSize > 0);

    println("Your external IP address is: {}", data);

    WinHttpCloseHandle(hRequest);

    WinHttpCloseHandle(hConnect);

    WinHttpCloseHandle(hSession);

    return 0;
}
