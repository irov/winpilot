#pragma once

#include "Pilot/PilotWidget.h"
#include "Pilot/PilotWidgetCallback.h"
#include "Pilot/PilotWidgetActions.h"

namespace Pilot {

class PilotSelect : public PilotWidget {
public:
    PilotSelect(PilotUI& ui, const PilotString& label)
        : PilotWidget(ui, "select") {
        put("label", PilotJson(label));
    }

    PilotSelect& options(const PilotVector<PilotPair<PilotString, PilotString>>& opts) {
        PilotJson arr = PilotJson::array();
        for (const auto& opt : opts) {
            arr.add(PilotJson::object()
                .put("value", PilotJson(opt.first))
                .put("label", PilotJson(opt.second)));
        }
        put("options", arr);
        return *this;
    }

    PilotSelect& defaultValue(const PilotString& v) { put("defaultValue", PilotJson(v)); return *this; }

    PilotSelect& onChange(PilotWidgetCallback* callback);
    PilotSelect& onChange(PilotWidgetCallbackFunc callback);
    PilotSelect& onChange(PilotFunction<void(const PilotSelectAction&)> callback);
};

} // namespace Pilot
