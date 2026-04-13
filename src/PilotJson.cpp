#include <Pilot/PilotJson.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace Pilot {

// ── Object operations ──

PilotJson& PilotJson::put(const PilotString& key, const PilotJson& value) {
    if (m_type != TYPE_OBJECT) {
        m_type = TYPE_OBJECT;
    }
    m_object[key] = value;
    return *this;
}

bool PilotJson::has(const PilotString& key) const {
    return m_type == TYPE_OBJECT && m_object.find(key) != m_object.end();
}

PilotJson PilotJson::get(const PilotString& key) const {
    if (m_type != TYPE_OBJECT) return PilotJson();
    auto it = m_object.find(key);
    return it != m_object.end() ? it->second : PilotJson();
}

PilotString PilotJson::optString(const PilotString& key, const PilotString& def) const {
    auto v = get(key);
    return v.m_type == TYPE_STRING ? v.m_string : def;
}

int PilotJson::optInt(const PilotString& key, int def) const {
    auto v = get(key);
    if (v.m_type == TYPE_INT) return static_cast<int>(v.m_int);
    if (v.m_type == TYPE_DOUBLE) return static_cast<int>(v.m_double);
    return def;
}

bool PilotJson::optBool(const PilotString& key, bool def) const {
    auto v = get(key);
    return v.m_type == TYPE_BOOL ? v.m_bool : def;
}

double PilotJson::optDouble(const PilotString& key, double def) const {
    auto v = get(key);
    if (v.m_type == TYPE_DOUBLE) return v.m_double;
    if (v.m_type == TYPE_INT) return static_cast<double>(v.m_int);
    return def;
}

PilotJson PilotJson::optObject(const PilotString& key) const {
    auto v = get(key);
    return v.m_type == TYPE_OBJECT ? v : PilotJson::object();
}

PilotJson PilotJson::optArray(const PilotString& key) const {
    auto v = get(key);
    return v.m_type == TYPE_ARRAY ? v : PilotJson::array();
}

int PilotJson::objectSize() const {
    return m_type == TYPE_OBJECT ? static_cast<int>(m_object.size()) : 0;
}

// ── Array operations ──

PilotJson& PilotJson::add(const PilotJson& value) {
    if (m_type != TYPE_ARRAY) {
        m_type = TYPE_ARRAY;
    }
    m_array.push_back(value);
    return *this;
}

int PilotJson::length() const {
    return m_type == TYPE_ARRAY ? static_cast<int>(m_array.size()) : 0;
}

PilotJson PilotJson::at(int index) const {
    if (m_type != TYPE_ARRAY || index < 0 || index >= static_cast<int>(m_array.size())) {
        return PilotJson();
    }
    return m_array[index];
}

// ── Value access ──

bool PilotJson::asBool() const {
    if (m_type == TYPE_BOOL) return m_bool;
    if (m_type == TYPE_INT) return m_int != 0;
    return false;
}

int PilotJson::asInt() const {
    if (m_type == TYPE_INT) return static_cast<int>(m_int);
    if (m_type == TYPE_DOUBLE) return static_cast<int>(m_double);
    return 0;
}

int64_t PilotJson::asInt64() const {
    if (m_type == TYPE_INT) return m_int;
    if (m_type == TYPE_DOUBLE) return static_cast<int64_t>(m_double);
    return 0;
}

double PilotJson::asDouble() const {
    if (m_type == TYPE_DOUBLE) return m_double;
    if (m_type == TYPE_INT) return static_cast<double>(m_int);
    return 0.0;
}

PilotString PilotJson::asString() const {
    if (m_type == TYPE_STRING) return m_string;
    if (m_type == TYPE_INT) return pilotToString(m_int);
    if (m_type == TYPE_DOUBLE) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%g", m_double);
        return PilotString(buf);
    }
    if (m_type == TYPE_BOOL) return m_bool ? PilotString("true") : PilotString("false");
    return PilotString();
}

// ── Factory ──

PilotJson PilotJson::object() {
    PilotJson j;
    j.m_type = TYPE_OBJECT;
    return j;
}

PilotJson PilotJson::array() {
    PilotJson j;
    j.m_type = TYPE_ARRAY;
    return j;
}

PilotJson PilotJson::null_value() {
    return PilotJson();
}

// ── Serialization ──

PilotString PilotJson::escapeString(const PilotString& s) {
    PilotString out;
    out.reserve(s.size() + 2);
    out += '"';
    for (char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                    out += buf;
                } else {
                    out += c;
                }
        }
    }
    out += '"';
    return out;
}

void PilotJson::writeJson(PilotString& out) const {
    switch (m_type) {
        case TYPE_NULL:
            out += "null";
            break;
        case TYPE_BOOL:
            out += m_bool ? "true" : "false";
            break;
        case TYPE_INT:
            out += pilotToString(m_int);
            break;
        case TYPE_DOUBLE: {
            char buf[64];
            snprintf(buf, sizeof(buf), "%g", m_double);
            out += buf;
            break;
        }
        case TYPE_STRING:
            out += escapeString(m_string);
            break;
        case TYPE_ARRAY: {
            out += '[';
            for (size_t i = 0; i < m_array.size(); ++i) {
                if (i > 0) out += ',';
                m_array[i].writeJson(out);
            }
            out += ']';
            break;
        }
        case TYPE_OBJECT: {
            out += '{';
            bool first = true;
            for (const auto& kv : m_object) {
                if (!first) out += ',';
                first = false;
                out += escapeString(kv.first);
                out += ':';
                kv.second.writeJson(out);
            }
            out += '}';
            break;
        }
    }
}

PilotString PilotJson::toString() const {
    PilotString out;
    writeJson(out);
    return out;
}

// ── Parser ──

PilotJson PilotJson::parse(const PilotString& str) {
    Parser p;
    p.data = str.c_str();
    p.pos = 0;
    p.len = str.size();
    p.skipWhitespace();
    if (p.pos >= p.len) return PilotJson();
    return p.parseValue();
}

void PilotJson::Parser::skipWhitespace() {
    while (pos < len && (data[pos] == ' ' || data[pos] == '\t' || data[pos] == '\n' || data[pos] == '\r')) {
        ++pos;
    }
}

char PilotJson::Parser::peek() {
    return pos < len ? data[pos] : '\0';
}

char PilotJson::Parser::advance() {
    return pos < len ? data[pos++] : '\0';
}

PilotJson PilotJson::Parser::parseValue() {
    skipWhitespace();
    char c = peek();
    if (c == '{') return parseObject();
    if (c == '[') return parseArray();
    if (c == '"') return PilotJson(parseStringValue());
    if (c == '-' || (c >= '0' && c <= '9')) return parseNumber();
    return parseLiteral();
}

PilotJson PilotJson::Parser::parseObject() {
    advance(); // '{'
    PilotJson obj = PilotJson::object();
    skipWhitespace();
    if (peek() == '}') { advance(); return obj; }

    while (true) {
        skipWhitespace();
        if (peek() != '"') break;
        PilotString key = parseStringValue();
        skipWhitespace();
        if (peek() == ':') advance();
        PilotJson value = parseValue();
        obj.put(key, value);
        skipWhitespace();
        if (peek() == ',') { advance(); continue; }
        break;
    }

    skipWhitespace();
    if (peek() == '}') advance();
    return obj;
}

PilotJson PilotJson::Parser::parseArray() {
    advance(); // '['
    PilotJson arr = PilotJson::array();
    skipWhitespace();
    if (peek() == ']') { advance(); return arr; }

    while (true) {
        arr.add(parseValue());
        skipWhitespace();
        if (peek() == ',') { advance(); continue; }
        break;
    }

    skipWhitespace();
    if (peek() == ']') advance();
    return arr;
}

PilotString PilotJson::Parser::parseStringValue() {
    advance(); // '"'
    PilotString result;
    while (pos < len) {
        char c = advance();
        if (c == '"') break;
        if (c == '\\') {
            char next = advance();
            switch (next) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case 'u': {
                    char hex[5] = {};
                    for (int i = 0; i < 4 && pos < len; ++i) {
                        hex[i] = advance();
                    }
                    unsigned int codepoint = static_cast<unsigned int>(strtoul(hex, nullptr, 16));
                    if (codepoint < 0x80) {
                        result += static_cast<char>(codepoint);
                    } else if (codepoint < 0x800) {
                        result += static_cast<char>(0xC0 | (codepoint >> 6));
                        result += static_cast<char>(0x80 | (codepoint & 0x3F));
                    } else {
                        result += static_cast<char>(0xE0 | (codepoint >> 12));
                        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                        result += static_cast<char>(0x80 | (codepoint & 0x3F));
                    }
                    break;
                }
                default: result += next; break;
            }
        } else {
            result += c;
        }
    }
    return result;
}

PilotJson PilotJson::Parser::parseNumber() {
    size_t start = pos;
    bool isDouble = false;

    if (peek() == '-') advance();
    while (pos < len && data[pos] >= '0' && data[pos] <= '9') advance();

    if (pos < len && data[pos] == '.') {
        isDouble = true;
        advance();
        while (pos < len && data[pos] >= '0' && data[pos] <= '9') advance();
    }

    if (pos < len && (data[pos] == 'e' || data[pos] == 'E')) {
        isDouble = true;
        advance();
        if (pos < len && (data[pos] == '+' || data[pos] == '-')) advance();
        while (pos < len && data[pos] >= '0' && data[pos] <= '9') advance();
    }

    PilotString numStr(data + start, pos - start);
    if (isDouble) {
        return PilotJson(strtod(numStr.c_str(), nullptr));
    } else {
        int64_t val = strtoll(numStr.c_str(), nullptr, 10);
        return PilotJson(val);
    }
}

PilotJson PilotJson::Parser::parseLiteral() {
    if (pos + 4 <= len && strncmp(data + pos, "true", 4) == 0) {
        pos += 4;
        return PilotJson(true);
    }
    if (pos + 5 <= len && strncmp(data + pos, "false", 5) == 0) {
        pos += 5;
        return PilotJson(false);
    }
    if (pos + 4 <= len && strncmp(data + pos, "null", 4) == 0) {
        pos += 4;
        return PilotJson();
    }
    advance();
    return PilotJson();
}

} // namespace Pilot
