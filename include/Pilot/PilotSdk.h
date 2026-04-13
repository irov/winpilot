#pragma once

#include <Pilot/PilotConfig.h>
#include <Pilot/PilotSessionStatus.h>
#include <Pilot/PilotLogLevel.h>
#include <Pilot/PilotLogEntry.h>
#include <Pilot/PilotJson.h>
#include <Pilot/PilotUI.h>
#include <Pilot/PilotMetrics.h>
#include <Pilot/PilotActionListener.h>
#include <Pilot/PilotSessionListener.h>

namespace Pilot {

class PilotSdk {
public:
    static constexpr const char* VERSION = "1.0.34";

    static void initialize(const PilotConfig& config);
    static bool isInitialized();
    static PilotSessionStatus status();

    static void addActionListener(PilotActionListener* listener);
    static void removeActionListener(PilotActionListener* listener);
    static void addSessionListener(PilotSessionListener* listener);
    static void removeSessionListener(PilotSessionListener* listener);

    static PilotUI& getUI();
    static PilotMetrics& getMetrics();

    static void connect();
    static void disconnect();

    // Call from your main loop to poll value providers and dispatch queued callbacks
    static void update();

    static void log(PilotLogLevel level, const PilotString& message);
    static void log(PilotLogLevel level, const PilotString& message,
                    const PilotString& category, const PilotString& thread = PilotString());
    static void log(PilotLogLevel level, const PilotString& message,
                    const PilotMap<PilotString, PilotJson>& metadata);
    static void log(PilotLogLevel level, const PilotString& message,
                    const PilotString& category, const PilotString& thread,
                    const PilotMap<PilotString, PilotJson>& metadata);
    static void log(const PilotLogEntry& entry);

    static void event(const PilotString& message);
    static void event(const PilotString& message, const PilotMap<PilotString, PilotJson>& metadata);
    static void event(const PilotString& message, const PilotString& category,
                      const PilotMap<PilotString, PilotJson>& metadata);

    static void revenue(const PilotString& message);
    static void revenue(const PilotString& message, const PilotMap<PilotString, PilotJson>& metadata);
    static void revenue(const PilotString& message, const PilotString& category,
                        const PilotMap<PilotString, PilotJson>& metadata);

    static void changeScreen(const PilotString& screenType, const PilotString& screenName);

    static void acknowledgeAction(const PilotString& actionId, const PilotJson& ackPayload = PilotJson());

    static void shutdown();

private:
    PilotSdk() = delete;
};

} // namespace Pilot
