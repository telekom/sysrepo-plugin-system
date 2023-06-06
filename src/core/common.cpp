#include "common.hpp"

#include <regex>

namespace ietf::sys {
/**
 * @brief Extracts the key from the list XPath.
 *
 * @param list List name.
 * @param key Key name.
 * @param xpath XPath of the list.
 * @return std::string Key value.
 */
const std::string extractListKeyFromXPath(const std::string& list, const std::string& key, const std::string& xpath)
{
    std::string value;

    std::stringstream ss;

    ss << list << "\\[" << key << "='([^']*)'\\]";

    const auto& xpath_expr = ss.str();

    std::regex re(xpath_expr);
    std::smatch xpath_match;

    if (std::regex_search(xpath, xpath_match, re)) {
        value = xpath_match[1];
    } else {
        throw std::runtime_error("Failed to extract key from XPath.");
    }

    return value;
}

/**
 * @brief Get meta value.
 *
 * @param meta Meta collection object.
 * @param name Meta name.
 * @return std::string Meta value.
 */
const std::string getMetaValue(ly::MetaCollection& meta, const std::string& name)
{
    for (const auto& m : meta) {
        if (m.name() == name) {
            return m.valueStr();
        }
    }
    throw std::runtime_error("Failed to get meta value.");
}

/**
 * @brief Convert meta values list to a hash.
 *
 * @param meta Meta collection object.
 * @return std::map<std::string, std::string> Meta values hash.
 */
std::map<std::string, std::string> getMetaValuesHash(ly::MetaCollection meta)
{
    std::map<std::string, std::string> hash;

    for (const auto& m : meta) {
        hash[m.name()] = m.valueStr();
    }

    return hash;
}

}
