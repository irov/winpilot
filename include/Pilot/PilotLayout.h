#pragma once

#include <Pilot/PilotLayoutChild.h>
#include <Pilot/PilotJson.h>

namespace Pilot {

class PilotUI;
class PilotButton;
class PilotLabel;
class PilotStat;
class PilotSwitch;
class PilotInput;
class PilotSelect;
class PilotTextarea;
class PilotTable;
class PilotLogs;

class PilotLayout : public PilotLayoutChild {
public:
    enum class Direction { VERTICAL, HORIZONTAL };

    PilotLayout(PilotUI& ui, Direction dir);
    ~PilotLayout() override = default;

    // Sub-layouts
    PilotLayout& addVertical();
    PilotLayout& addHorizontal();
    PilotLayout& addCollapsible(const PilotString& title);

    // Padding
    PilotLayout& addPadding(double weight);

    // Widgets
    PilotButton& addButton(const PilotString& label);
    PilotLabel& addLabel(const PilotString& text);
    PilotStat& addStat(const PilotString& label);
    PilotSwitch& addSwitch(const PilotString& label);
    PilotInput& addInput(const PilotString& label);
    PilotSelect& addSelect(const PilotString& label);
    PilotTextarea& addTextarea(const PilotString& label);
    PilotTable& addTable(const PilotString& label);
    PilotLogs& addLogs(const PilotString& label);

    PilotJson toJson() const override;

private:
    PilotUI& m_ui;
    Direction m_direction;
    PilotVector<PilotUniquePtr<PilotLayoutChild>> m_children;

    // Internal helpers for padding and collapsible
    class PaddingElement : public PilotLayoutChild {
    public:
        PaddingElement(double weight) : m_weight(weight) {}
        PilotJson toJson() const override {
            return PilotJson::object()
                .put("type", PilotJson("padding"))
                .put("weight", PilotJson(m_weight));
        }
    private:
        double m_weight;
    };

    class CollapsibleElement : public PilotLayoutChild {
    public:
        CollapsibleElement(PilotUI& ui, const PilotString& title)
            : m_title(title), m_content(PilotUniquePtr<PilotLayout>(new PilotLayout(ui, Direction::VERTICAL))) {}
        PilotLayout& content() { return *m_content; }
        PilotJson toJson() const override {
            PilotJson contentJson = m_content->toJson();
            return PilotJson::object()
                .put("type", PilotJson("collapsible"))
                .put("title", PilotJson(m_title))
                .put("children", contentJson.optArray("children"));
        }
    private:
        PilotString m_title;
        PilotUniquePtr<PilotLayout> m_content;
    };
};

} // namespace Pilot
