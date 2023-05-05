#include "dns.hpp"

namespace ietf::sys::dns {

// Address class
Address::Address(std::string address)
    : address { address }

{
    // ipv4 / ipv6
    struct in_addr ip_addr;

    if (inet_pton(AF_INET, address.c_str(), &ip_addr) == 1) {
        // its ipv4
        BYTE_SIZE = 4;
        version = 2;

        uint8_t* bytes = (uint8_t*)&ip_addr.s_addr;
        for (int i = 0; i < BYTE_SIZE; i++) {
            dns.ipv4_address[i] = bytes[i];
        };

    } else if (inet_pton(AF_INET6, address.c_str(), &ip_addr) == 1) {
        // its ipv6
        BYTE_SIZE = 16;
        version = 10;

        uint8_t* bytes = (uint8_t*)&ip_addr.s_addr;
        for (int i = 0; i < BYTE_SIZE; i++) {
            dns.ipv6_address[i] = bytes[i];
        };
    } else {
        // error
        throw std::runtime_error("Failed to create address, Wrong format!");
    };
}

Address::Address(std::vector<uint8_t> bytes)
{

    if (bytes.size() == 4) {
        // ipv4
        version = 2;
        BYTE_SIZE = 4;

        for (int i = 0; i < BYTE_SIZE; i++) {

            dns.ipv4_address[i] = bytes[i];
            address.append(std::to_string(bytes[i]));

            if (i < (BYTE_SIZE - 1)) {
                address.append(".");
            };
        }

    } else if (bytes.size() == 16) {
        // ipv6

        version = 10;
        BYTE_SIZE = 16;

        for (int i = 0; i < BYTE_SIZE; i++) {

            dns.ipv6_address[i] = bytes[i];
            address.append(std::to_string(bytes[i]));

            if (i < (BYTE_SIZE - 1)) {
                address.append("::");
            };
        }
    } else {
        throw std::runtime_error("Unknown address type");
    }
}

int Address::getVersion() { return version; };

std::string Address::getStringAddr() { return this->address; };

std::vector<uint8_t> Address::byteVector()
{
    std::vector<uint8_t> bytes;
    for (int i = 0; i < BYTE_SIZE; i++) {
        // push from ipv6, cause its longer and contains ipv4
        bytes.push_back(dns.ipv6_address[i]);
    }
    return bytes;
};

bool Address::operator==(const Address& other) const
{

    // first check the version
    if (this->version != other.version) {
        return false;
    }

    // check byte size
    if (this->BYTE_SIZE != other.BYTE_SIZE) {
        return false;
    }

    // check byte array
    for (int i = 0; i < BYTE_SIZE; i++) {
        if (this->dns.ipv6_address[i] != other.dns.ipv6_address[i]) {
            return false;
        }
    }

    // finaly if all ok, return true, they are equal
    return true;
}

// End of address implementation

DnsServer::DnsServer(int ifindex, std::string name, Address address, uint16_t port)
    : Name { name }
    , address { address }
    , Port { port }
    , m_ifindex { ifindex } {

    };

std::string DnsServer::getName() { return Name; };

uint16_t DnsServer::getPort() { return Port; };

Address DnsServer::getAddress() { return address; };

void DnsServer::setPort(const uint16_t& port) { this->Port = port; }

void DnsServer::setAddress(const Address& address) { this->address = address; }

std::string DnsServer::getStringAddress() { return this->address.getStringAddr(); };

int DnsServer::getIfindex() { return m_ifindex; };

bool DnsServer::operator==(const DnsServer& other) const
{
    return ((this->Name == other.Name) && (this->address == other.address) && (this->Port == other.Port) && (this->m_ifindex == other.m_ifindex));
};

// DnsSearchServer implementation
DnsSearchServer::DnsSearchServer()
    : Search { false }
{
    Domain.clear();
}

DnsSearchServer::DnsSearchServer(std::string domain, bool search)
    : Domain { domain }
    , Search { search }
    , m_ifindex { SYSTEMD_IFINDEX } {};

std::string DnsSearchServer::getDomain() { return Domain; }

bool DnsSearchServer::getSearch() { return Search; }

int DnsSearchServer::getIfIndex() { return m_ifindex; }

void DnsSearchServer::setDomain(std::string domain) { this->Domain = domain; }

void DnsSearchServer::setSearch(bool search) { this->Search = search; }

bool DnsSearchServer::operator==(const DnsSearchServer& other) const
{
    return (this->Domain == other.Domain) && (this->m_ifindex == other.m_ifindex);
}

bool DnsSearchServer::operator!=(const DnsSearchServer& other) const
{
    return !((this->Domain == other.Domain) && (this->m_ifindex == other.m_ifindex));
}

// DnsSearchServerList implementation

DnsSearchServerList::DnsSearchServerList()
    : m_ifindex { SYSTEMD_IFINDEX } {};

// not recomended, explicit definition of ifindex
DnsSearchServerList::DnsSearchServerList(int ifindex)
    : m_ifindex { ifindex } {};

bool DnsSearchServerList::addDnsSearchServer(DnsSearchServer srv)
{
    for (DnsSearchServer& server : servers) {
        if (server == srv) {
            // allready exists or wrong ifindex
            if (server.getSearch() != srv.getSearch()) {
                server.setSearch(srv.getSearch());
                // just modified, dont push to vector
                return true;
            }
            return false;
        }
    }

    servers.push_back(srv);
    return true;
}

std::vector<DnsSearchServer> DnsSearchServerList::getAllServers() { return servers; }

std::optional<DnsSearchServer> DnsSearchServerList::findDnsSearchServer(const DnsSearchServer& server)
{

    for (DnsSearchServer& srv : servers) {
        if (srv == server) {
            return server;
        }
    }

    return std::nullopt;
}

bool DnsSearchServerList::removeDnsSearchServer(const DnsSearchServer& server)
{
    std::vector<DnsSearchServer>::iterator it = servers.begin();
    // safe aproach to erase while iterating
    while (it != servers.end()) {

        if (*it == server) {
            it = servers.erase(it);
            return true;
        } else {
            it++;
        }
    }

    return false;
};

bool DnsSearchServerList::compareDnsSearchServer(const DnsSearchServer& s1, const DnsSearchServer& s2) { return (s1 == s2); };

bool DnsSearchServerList::exportListToSdBus()
{

    bool error = false;

    std::vector<sdbus::Struct<std::string, bool>> sdbusData;

    for (auto& server : servers) {
        sdbusData.push_back(sdbus::Struct<std::string, bool>(sdbus::make_struct(server.getDomain(), server.getSearch())));
    };

    const char* destinationName = "org.freedesktop.resolve1";
    const char* objectPath = "/org/freedesktop/resolve1";
    const char* interfaceName = "org.freedesktop.resolve1.Manager";

    try {
        auto proxy = sdbus::createProxy(destinationName, objectPath);
        proxy->callMethod("SetLinkDomains").onInterface(interfaceName).withArguments(m_ifindex, sdbusData);
    } catch (sdbus::Error& e) {
        SRPLG_LOG_ERR("%s", e.getMessage().c_str());
        error = true;
    };

    // clear container after succsess
    if (!error) {
        servers.clear();
    }

    return error;
}

bool DnsSearchServerList::importListFromSdBus()
{

    bool error = false;

    std::vector<sdbus::Struct<int32_t, std::string, bool>> sdbusData;
    const char* destinationName = "org.freedesktop.resolve1";
    const char* objectPath = "/org/freedesktop/resolve1";
    const char* interfaceName = "org.freedesktop.resolve1.Manager";
    sdbus::Variant v;
    try {
        auto proxy = sdbus::createProxy(destinationName, objectPath);
        v = proxy->getProperty("Domains").onInterface(interfaceName);
    } catch (sdbus::Error& e) {
        SRPLG_LOG_ERR("%s", e.getMessage().c_str());
        error = true;
    }
    sdbusData = v.get<std::vector<sdbus::Struct<int32_t, std::string, bool>>>();
    servers.clear();
    for (auto& vc : sdbusData) {
        // filter by ifindex
        int ifindex = vc.get<0>();
        if (ifindex == m_ifindex) {
            servers.push_back(
                // store the ifindex ,domain, search
                DnsSearchServer(vc.get<1>(), vc.get<2>()));
        };
    };

    return error;
}

// end of DnsSearchServerList initialization

// DnsServerList initialization

DnsServerList::DnsServerList()
    : m_ifindex { SYSTEMD_IFINDEX } {};
DnsServerList::DnsServerList(int ifindex)
    : m_ifindex { ifindex } {};

bool DnsServerList::importListFromSdBus()
{

    bool error = false;

    std::vector<sdbus::Struct<int, int, std::vector<uint8_t>, uint16_t, std::string>> sdbusData;
    const char* destinationName = "org.freedesktop.resolve1";
    const char* objectPath = "/org/freedesktop/resolve1";
    const char* interfaceName = "org.freedesktop.resolve1.Manager";
    sdbus::Variant v;

    try {
        auto proxy = sdbus::createProxy(destinationName, objectPath);
        v = proxy->getProperty("DNSEx").onInterface(interfaceName);
    } catch (sdbus::Error& e) {
        SRPLG_LOG_ERR("%s", e.getMessage().c_str());
        error = true;
    }

    sdbusData = v.get<std::vector<sdbus::Struct<int, int, std::vector<uint8_t>, uint16_t, std::string>>>();
    servers.clear();

    for (auto& vc : sdbusData) {
        Address adr(vc.get<2>());
        // SRPLG_LOG_ERR("GET ADDR: ", "%s", adr.getStringAddr().c_str());
        // SRPLG_LOG_ERR("GET PORT: ", "%d", vc.get<3>());
        // SRPLG_LOG_ERR("GET IFIND: ", "%d", vc.get<0>());
        // SRPLG_LOG_ERR("GET NAME: ", "%s", vc.get<4>().c_str());
        // SRPLG_LOG_ERR("GET VERS: ", "%d", vc.get<1>());
        int ifindex = vc.get<0>();
        if (ifindex == m_ifindex) {
            int port = vc.get<3>();
            if (port == 0) {
                port = 53;
            };
            Address addr(vc.get<2>());
            servers.push_back(DnsServer(ifindex, vc.get<4>(), addr, port));
        };
    }

    return error;
}

bool DnsServerList::exportListToSdBus()
{
    bool error = false;

    std::vector<sdbus::Struct<int, std::vector<uint8_t>, uint16_t, std::string>> sdbusData;

    for (auto& server : servers) {
        sdbusData.push_back(sdbus::Struct<int, std::vector<uint8_t>, uint16_t, std::string>(
            sdbus::make_struct(server.getAddress().getVersion(), server.getAddress().byteVector(), server.getPort(), server.getName())));
    };

    const char* destinationName = "org.freedesktop.resolve1";
    const char* objectPath = "/org/freedesktop/resolve1";
    const char* interfaceName = "org.freedesktop.resolve1.Manager";

    try {
        auto proxy = sdbus::createProxy(destinationName, objectPath);
        proxy->callMethod("SetLinkDNSEx").onInterface(interfaceName).withArguments(m_ifindex, sdbusData);
    } catch (sdbus::Error& e) {
        SRPLG_LOG_ERR("%s", e.getMessage().c_str());
        error = true;
    };

    // clear container after succsess
    if (!error) {
        servers.clear();
    }

    return error;
}

int DnsServerList::getIfIndex() { return m_ifindex; };

bool DnsServerList::addDnsServer(DnsServer srv)
{
    for (DnsServer& server : servers) {
        if (server.getName().compare(srv.getName()) == 0) {
            // the name is the key in yang
            // allready exists, return false
            return false;
        }
    }

    servers.push_back(srv);
    return true;
}

std::vector<DnsServer> DnsServerList::getDnsServerVector() { return servers; };

bool DnsServerList::removeDnsServer(const DnsServer& server)
{
    std::vector<DnsServer>::iterator it = servers.begin();
    // safe aproach to erase while iterating
    while (it != servers.end()) {

        if (*it == server) {
            it = servers.erase(it);
            return true;
        } else {
            it++;
        }
    }

    return false;
}

bool DnsServerList::modifyDnsServer(DnsServer server)
{

    for (auto& srv : servers) { // name is the key in yang model, no duplicates
        if (srv.getName().compare(server.getName()) == 0) {

            srv.setAddress(server.getAddress());
            srv.setPort(server.getPort());
            return true;
        }
    }

    return false;
}

// end of DnsServerList init

// function to take from the changed node, return DnsServer
std::optional<DnsServer> getServerFromChangedNode(const libyang::DataNode& changedNode)
{

    libyang::DataNode recursiveNode(changedNode);
    libyang::SchemaNode schema = recursiveNode.schema();
    std::unordered_map<std::string, std::string> map;

    // starting from the lowest point address or port
    if (schema.name().compare("address") == 0 || schema.name().compare("port") == 0) {

        // set the node pointer to server node
        while (schema.name().compare("server") != 0) {
            recursiveNode = recursiveNode.parent().value();
            schema = recursiveNode.schema();
        };

        for (libyang::DataNode node : recursiveNode.childrenDfs()) {

            // jump the first node, cause its server
            if (node.schema().name().compare("server") == 0)
                continue;

            if (node.isTerm()) {
                {
                    std::string key = node.schema().name().data();
                    std::string value = node.asTerm().valueStr().data();
                    map.insert(std::make_pair(key, value));
                }

            } else {

                {
                    std::string value = node.schema().name().data();
                    std::string key = "transport";
                    map.insert(std::make_pair(key, value));
                }
            }
        }

        uint16_t port = 53;

        if (map.find("port") != map.end()) {
            port = stoi(map.at("port"));
        }

        Address addr(map["address"]);
        DnsServer server(SYSTEMD_IFINDEX, map["name"], addr, port);

        return server;
    }

    return std::nullopt;
};

} // end of namespace
