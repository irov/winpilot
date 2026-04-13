#pragma once

#include <Pilot/PilotWidget.h>

namespace Pilot {

class PilotTable : public PilotWidget {
public:
    PilotTable(PilotUI& ui, const PilotString& label)
        : PilotWidget(ui, "table") {
        put("label", PilotJson(label));
    }

    PilotTable& columns(const PilotVector<PilotPair<PilotString, PilotString>>& cols) {
        PilotJson arr = PilotJson::array();
        for (const auto& col : cols) {
            arr.add(PilotJson::object()
                .put("key", PilotJson(col.first))
                .put("label", PilotJson(col.second)));
        }
        put("columns", arr);
        return *this;
    }

    PilotTable& rows(const PilotVector<PilotJson>& rowData) {
        PilotJson arr = PilotJson::array();
        for (const auto& row : rowData) {
            arr.add(row);
        }
        put("rows", arr);
        return *this;
    }
};

} // namespace Pilot
