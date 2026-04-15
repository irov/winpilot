#pragma once

#include "Pilot/PilotJson.h"
#include "Pilot/PilotConnectResponse.h"
#include "Pilot/PilotLogEntry.h"
#include "Pilot/PilotMetricEntry.h"

namespace Pilot {

class PilotHttpClient {
public:
    PilotHttpClient(const PilotString& baseUrl, const PilotString& apiToken);
    ~PilotHttpClient();

    void shutdown();

    PilotConnectResponse connect(const PilotString& deviceId, const PilotString& deviceName,
                                 const PilotMap<PilotString, PilotJson>& sessionAttributes);
    PilotConnectResponse pollStatus(const PilotString& requestId);
    bool closeSession(const PilotString& sessionToken);
    PilotJson submitPanel(const PilotString& sessionToken, const PilotJson& layout);
    PilotJson pollActions(const PilotString& sessionToken,
                          const PilotMap<PilotString, PilotJson>* changedAttributes,
                          const PilotVector<PilotLogEntry>& logs,
                          const PilotVector<PilotMetricEntry>& metrics);
    void acknowledgeAction(const PilotString& sessionToken, const PilotString& actionId, const PilotJson& ackPayload);
    void sendLogs(const PilotString& sessionToken, const PilotVector<PilotLogEntry>& logs);
    void sendMetrics(const PilotString& sessionToken, const PilotVector<PilotMetricEntry>& metrics);
    PilotJson getLivePublisherState(const PilotString& sessionToken);

private:
    PilotJson executeRequest(const PilotString& method, const PilotString& path,
                             const PilotString& headerName, const PilotString& headerValue,
                             const PilotString& body);

    PilotString m_host;
    int m_port;
    PilotString m_basePath;
    bool m_secure;
    PilotString m_apiToken;

    void* m_hSession;  // HINTERNET
    void* m_hConnect;  // HINTERNET
};

} // namespace Pilot
