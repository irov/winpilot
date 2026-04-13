#pragma once

#include "Pilot/PilotTypes.h"
#include <cstdint>

namespace Pilot {

class PilotJson {
public:
    enum Type {
        TYPE_NULL,
        TYPE_BOOL,
        TYPE_INT,
        TYPE_DOUBLE,
        TYPE_STRING,
        TYPE_ARRAY,
        TYPE_OBJECT
    };

    PilotJson() : m_type(TYPE_NULL), m_bool(false), m_int(0), m_double(0.0) {}
    PilotJson(bool v) : m_type(TYPE_BOOL), m_bool(v), m_int(0), m_double(0.0) {}
    PilotJson(int v) : m_type(TYPE_INT), m_bool(false), m_int(v), m_double(0.0) {}
    PilotJson(int64_t v) : m_type(TYPE_INT), m_bool(false), m_int(v), m_double(0.0) {}
    PilotJson(double v) : m_type(TYPE_DOUBLE), m_bool(false), m_int(0), m_double(v) {}
    PilotJson(const char* v) : m_type(TYPE_STRING), m_bool(false), m_int(0), m_double(0.0), m_string(v ? v : "") {}
    PilotJson(const PilotString& v) : m_type(TYPE_STRING), m_bool(false), m_int(0), m_double(0.0), m_string(v) {}

    Type type() const { return m_type; }
    bool isNull() const { return m_type == TYPE_NULL; }

    // Object operations
    PilotJson& put(const PilotString& key, const PilotJson& value);
    bool has(const PilotString& key) const;
    PilotJson get(const PilotString& key) const;
    PilotString optString(const PilotString& key, const PilotString& def = PilotString()) const;
    int optInt(const PilotString& key, int def = 0) const;
    bool optBool(const PilotString& key, bool def = false) const;
    double optDouble(const PilotString& key, double def = 0.0) const;
    PilotJson optObject(const PilotString& key) const;
    PilotJson optArray(const PilotString& key) const;
    int objectSize() const;
    const PilotMap<PilotString, PilotJson>& objectEntries() const { return m_object; }

    // Array operations
    PilotJson& add(const PilotJson& value);
    int length() const;
    PilotJson at(int index) const;

    // Value access
    bool asBool() const;
    int asInt() const;
    int64_t asInt64() const;
    double asDouble() const;
    PilotString asString() const;

    // Serialization
    PilotString toString() const;

    // Parsing
    static PilotJson parse(const PilotString& str);

    // Factory
    static PilotJson object();
    static PilotJson array();
    static PilotJson null_value();

private:
    Type m_type;
    bool m_bool;
    int64_t m_int;
    double m_double;
    PilotString m_string;
    PilotVector<PilotJson> m_array;
    PilotMap<PilotString, PilotJson> m_object;

    void writeJson(PilotString& out) const;
    static PilotString escapeString(const PilotString& s);

    struct Parser {
        const char* data;
        size_t pos;
        size_t len;

        PilotJson parseValue();
        PilotJson parseObject();
        PilotJson parseArray();
        PilotString parseStringValue();
        PilotJson parseNumber();
        PilotJson parseLiteral();
        void skipWhitespace();
        char peek();
        char advance();
    };
};

} // namespace Pilot
