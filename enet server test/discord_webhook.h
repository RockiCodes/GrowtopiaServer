#pragma once
#include <winhttp.h>

void webhook(std::string username, std::string title, std::string desc) {
    HINTERNET hSession = WinHttpOpen(L"Fontesie/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);

    HINTERNET hConnect = WinHttpConnect(hSession,
        L"discordapp.com",
        INTERNET_DEFAULT_HTTPS_PORT,
        0);

    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
        L"POST",
        L"/api/webhooks/id/random shit",
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

    std::string color = "16711680"; // Decimal color
    std::string request_body = "{\"username\": \"" + username + "\",\"content\": null,\"embeds\": [{\"title\": \"" + title + "\",\"description\": \"" + desc + "\",\"footer\": {\"text\": \"Server Logs\"},\"color\": " + color + " }], \"attachments\": []}";

    BOOL bResults = WinHttpSendRequest(hRequest,
        L"Content-Type: application/json\r\n",
        (DWORD)-1L,
        (LPVOID)request_body.c_str(),
        (DWORD)request_body.length(),
        (DWORD)request_body.length(),
        0);

    if (bResults) {
        WinHttpReceiveResponse(hRequest, NULL);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
}

//usage
//webhook("Drop Log", "Algonix + "is a bot", "Terrible");