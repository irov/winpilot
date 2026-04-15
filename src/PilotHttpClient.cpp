#include "Pilot/PilotHttpClient.h"
#include "Pilot/PilotException.h"
#include "Pilot/PilotLog.h"

#include "PilotWin32.h"
#include <wininet.h>

#include <cstring>
#include <cstdlib>

namespace Pilot {

// ── URL parsing helper ──

static void parseUrl(const PilotString& url, PilotString& host, int& port, PilotString& basePath, bool& secure) {
    PilotString remainder = url;
    secure = false;
    port = 80;

    if (remainder.substr(0, 8) == "https://") {
        secure = true;
        port = 443;
        remainder = remainder.substr(8);
    } else if (remainder.substr(0, 7) == "http://") {
        remainder = remainder.substr(7);
    }

    size_t slashPos = remainder.find('/');
    PilotString hostPort;
    if (slashPos != PilotString::npos) {
        hostPort = remainder.substr(0, slashPos);
        basePath = remainder.substr(slashPos);
    } else {
        hostPort = remainder;
        basePath = PilotString();
    }

    // Trim trailing slash from basePath
    while (!basePath.empty() && basePath.back() == '/') {
        basePath.pop_back();
    }

    size_t colonPos = hostPort.find(':');
    if (colonPos != PilotString::npos) {
        host = hostPort.substr(0, colonPos);
        port = atoi(hostPort.substr(colonPos + 1).c_str());
    } else {
        host = hostPort;
    }
}

// ── Constructor / Destructor ──

PilotHttpClient::PilotHttpClient(const PilotString& baseUrl, const PilotString& apiToken)
    : m_port(80), m_secure(false), m_apiToken(apiToken), m_hSession(nullptr), m_hConnect(nullptr) {
    parseUrl(baseUrl, m_host, m_port, m_basePath, m_secure);

    m_hSession = InternetOpenA("PilotSDK/1.0", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!m_hSession) {
        throw PilotException("Failed to initialize WinINet session");
    }

    m_hConnect = InternetConnectA(
        static_cast<HINTERNET>(m_hSession),
        m_host.c_str(),
        static_cast<INTERNET_PORT>(m_port),
        nullptr, nullptr,
        INTERNET_SERVICE_HTTP,
        0, 0);
    if (!m_hConnect) {
        InternetCloseHandle(static_cast<HINTERNET>(m_hSession));
        m_hSession = nullptr;
        throw PilotException("Failed to connect to " + m_host);
    }
}

PilotHttpClient::~PilotHttpClient() {
    shutdown();
}

void PilotHttpClient::shutdown() {
    if (m_hConnect) {
        InternetCloseHandle(static_cast<HINTERNET>(m_hConnect));
        m_hConnect = nullptr;
    }
    if (m_hSession) {
        InternetCloseHandle(static_cast<HINTERNET>(m_hSession));
        m_hSession = nullptr;
    }
}

// ── Core request execution ──

PilotJson PilotHttpClient::executeRequest(const PilotString& method, const PilotString& path,
                                          const PilotString& headerName, const PilotString& headerValue,
                                          const PilotString& body) {
    if (!m_hConnect) {
        throw PilotException("HTTP client is not connected");
    }

    PilotString fullPath = m_basePath + path;

    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
    if (m_secure) {
        flags |= INTERNET_FLAG_SECURE;
    }

    HINTERNET hRequest = HttpOpenRequestA(
        static_cast<HINTERNET>(m_hConnect),
        method.c_str(),
        fullPath.c_str(),
        nullptr, nullptr, nullptr,
        flags, 0);

    if (!hRequest) {
        throw PilotException("Failed to create HTTP request");
    }

    // Add content type header
    PilotString contentTypeHeader = "Content-Type: application/json; charset=utf-8\r\n";
    HttpAddRequestHeadersA(hRequest, contentTypeHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD);

    // Add custom header (API token or session token)
    if (!headerName.empty()) {
        PilotString customHeader = headerName + PilotString(": ") + headerValue + PilotString("\r\n");
        HttpAddRequestHeadersA(hRequest, customHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD);
    }

    // Send request
    BOOL result;
    if (body.empty()) {
        result = HttpSendRequestA(hRequest, nullptr, 0, nullptr, 0);
    } else {
        result = HttpSendRequestA(hRequest, nullptr, 0,
                                  const_cast<char*>(body.c_str()),
                                  static_cast<DWORD>(body.size()));
    }

    if (!result) {
        DWORD err = GetLastError();
        InternetCloseHandle(hRequest);
        throw PilotException("Network error: WinINet error code " + pilotToString(static_cast<unsigned long>(err)));
    }

    // Read status code
    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                   &statusCode, &statusCodeSize, nullptr);

    // Read response body
    PilotString responseBody;
    char readBuf[4096];
    DWORD bytesRead = 0;
    while (InternetReadFile(hRequest, readBuf, sizeof(readBuf), &bytesRead) && bytesRead > 0) {
        responseBody.append(readBuf, bytesRead);
    }

    InternetCloseHandle(hRequest);

    if (statusCode < 200 || statusCode >= 300) {
        PilotString detail = responseBody;
        PilotJson errorJson = PilotJson::parse(responseBody);
        PilotString detailStr = errorJson.optString("detail");
        if (!detailStr.empty()) {
            detail = detailStr;
        }
        throw PilotException(static_cast<int>(statusCode),
                             PilotString("HTTP ") + pilotToString(statusCode) + PilotString(": ") + detail);
    }

    if (responseBody.empty()) return PilotJson::object();
    return PilotJson::parse(responseBody);
}

// ── Client endpoints ──

PilotConnectResponse PilotHttpClient::connect(const PilotString& deviceId, const PilotString& deviceName,
                                               const PilotMap<PilotString, PilotJson>& sessionAttributes) {
    PilotJson body = PilotJson::object()
        .put("device_id", PilotJson(deviceId))
        .put("device_name", PilotJson(deviceName));

    if (!sessionAttributes.empty()) {
        PilotJson attrs = PilotJson::object();
        for (const auto& kv : sessionAttributes) {
            attrs.put(kv.first, kv.second);
        }
        body.put("session_attributes", attrs);
    }

    PilotJson resp = executeRequest("POST", "/api/client/connect", "X-Api-Token", m_apiToken, body.toString());
    return PilotConnectResponse::fromJson(resp);
}

PilotConnectResponse PilotHttpClient::pollStatus(const PilotString& requestId) {
    PilotJson resp = executeRequest("GET", "/api/client/poll-status/" + requestId,
                                    "X-Api-Token", m_apiToken, "");
    return PilotConnectResponse::fromJson(resp);
}

bool PilotHttpClient::closeSession(const PilotString& sessionToken) {
    PilotJson resp = executeRequest("POST", "/api/client/session/close",
                                    "X-Session-Token", sessionToken, "");
    return resp.optBool("ok", false);
}

PilotJson PilotHttpClient::submitPanel(const PilotString& sessionToken, const PilotJson& layout) {
    PilotJson body = PilotJson::object().put("layout", layout);
    return executeRequest("POST", "/api/client/session/panel",
                          "X-Session-Token", sessionToken, body.toString());
}

PilotJson PilotHttpClient::pollActions(const PilotString& sessionToken,
                                        const PilotMap<PilotString, PilotJson>* changedAttributes,
                                        const PilotVector<PilotLogEntry>& logs,
                                        const PilotVector<PilotMetricEntry>& metrics) {
    PilotJson body = PilotJson::object();

    if (changedAttributes && !changedAttributes->empty()) {
        PilotJson attrs = PilotJson::object();
        for (const auto& kv : *changedAttributes) {
            attrs.put(kv.first, kv.second);
        }
        body.put("session_attributes", attrs);
    }

    if (!logs.empty()) {
        PilotJson logsArr = PilotJson::array();
        for (const auto& entry : logs) {
            logsArr.add(entry.toJson());
        }
        body.put("logs", logsArr);
    }

    if (!metrics.empty()) {
        PilotJson metricsArr = PilotJson::array();
        for (const auto& entry : metrics) {
            metricsArr.add(entry.toJson());
        }
        body.put("metrics", metricsArr);
    }

    PilotString bodyStr = (body.objectSize() > 0) ? body.toString() : PilotString();
    return executeRequest("POST", "/api/client/session/actions/poll",
                          "X-Session-Token", sessionToken, bodyStr);
}

void PilotHttpClient::acknowledgeAction(const PilotString& sessionToken,
                                         const PilotString& actionId,
                                         const PilotJson& ackPayload) {
    PilotJson body = PilotJson::object()
        .put("action_id", PilotJson(actionId))
        .put("ack_payload", ackPayload.isNull() ? PilotJson::object() : ackPayload);

    executeRequest("POST", "/api/client/session/actions/ack",
                   "X-Session-Token", sessionToken, body.toString());
}

void PilotHttpClient::sendLogs(const PilotString& sessionToken, const PilotVector<PilotLogEntry>& logs) {
    if (logs.empty()) return;

    PilotJson logsArr = PilotJson::array();
    for (const auto& entry : logs) {
        logsArr.add(entry.toJson());
    }

    PilotJson body = PilotJson::object().put("logs", logsArr);
    executeRequest("POST", "/api/client/session/logs",
                   "X-Session-Token", sessionToken, body.toString());
}

void PilotHttpClient::sendMetrics(const PilotString& sessionToken, const PilotVector<PilotMetricEntry>& metrics) {
    if (metrics.empty()) return;

    PilotJson metricsArr = PilotJson::array();
    for (const auto& entry : metrics) {
        metricsArr.add(entry.toJson());
    }

    PilotJson body = PilotJson::object().put("metrics", metricsArr);
    executeRequest("POST", "/api/client/session/metrics",
                   "X-Session-Token", sessionToken, body.toString());
}

PilotJson PilotHttpClient::getLivePublisherState(const PilotString& sessionToken) {
    return executeRequest("GET", "/api/client/session/live/publisher",
                          "X-Session-Token", sessionToken, "");
}

} // namespace Pilot
