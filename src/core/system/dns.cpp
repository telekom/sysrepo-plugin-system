#include "dns.hpp"

using namespace ietf::sys::ipv;
namespace ietf::sys::dns {

DnsServer::DnsServer(int ifindex, std::string name, Address address, uint16_t port)
    : m_name { name }
    , m_address { address }
    , m_port { port }
    , m_ifindex { ifindex } {

    };

std::string DnsServer::getName() { return m_name; };

uint16_t DnsServer::getPort() { return m_port; };

Address* DnsServer::getAddress() { return &m_address; };

void DnsServer::setPort(const uint16_t& port) { this->m_port = port; }

void DnsServer::setAddress(const Address& address) { this->m_address = address; }

std::string DnsServer::getStringAddress() { return m_address.getStringAddr(); };

int DnsServer::getIfindex() { return m_ifindex; };

bool DnsServer::operator==(const DnsServer& other) const
{
    return ((this->m_name == other.m_name) && (this->m_address == other.m_address) && (this->m_port == other.m_port) && (this->m_ifindex == other.m_ifindex));
};

// DnsSearchServer implementation
DnsSearchServer::DnsSearchServer()
    : m_search { false }
{
    m_domain.clear();
}

DnsSearchServer::DnsSearchServer(int ifindex, std::string domain, bool search)
    : m_domain { domain }
    , m_search { search }
    , m_ifindex { ifindex } {};

DnsSearchServer::DnsSearchServer(std::string domain, bool search)
    : m_domain { domain }
    , m_search { search }
    , m_ifindex { SYSTEMD_IFINDEX } {};

std::string DnsSearchServer::getDomain() { return m_domain; }

bool DnsSearchServer::getSearch() { return m_search; }

int DnsSearchServer::getIfIndex() { return m_ifindex; }

void DnsSearchServer::setDomain(std::string domain) { this->m_domain = domain; }

void DnsSearchServer::setSearch(bool search) { this->m_search = search; }

bool DnsSearchServer::operator==(const DnsSearchServer& other) const { return (this->m_domain == other.m_domain) && (this->m_ifindex == other.m_ifindex); }

bool DnsSearchServer::operator!=(const DnsSearchServer& other) const
{
    return !((this->m_domain == other.m_domain) && (this->m_ifindex == other.m_ifindex));
}

// DnsSearchServerList implementation

DnsSearchServerList::DnsSearchServerList()
    : m_ifindex { SYSTEMD_IFINDEX } {};

// not recomended, explicit definition of ifindex
DnsSearchServerList::DnsSearchServerList(int ifindex)
    : m_ifindex { ifindex } {};

bool DnsSearchServerList::addDnsSearchServer(DnsSearchServer srv)
{
    for (DnsSearchServer& server : m_servers) {
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

    m_servers.push_back(srv);
    return true;
}

std::vector<DnsSearchServer> DnsSearchServerList::getAllServers() { return m_servers; }

std::optional<DnsSearchServer> DnsSearchServerList::findDnsSearchServer(const DnsSearchServer& server)
{

    for (DnsSearchServer& srv : m_servers) {
        if (srv == server) {
            return server;
        }
    }

    return std::nullopt;
}

bool DnsSearchServerList::removeDnsSearchServer(const DnsSearchServer& server)
{
    std::vector<DnsSearchServer>::iterator it = m_servers.begin();
    // safe aproach to erase while iterating
    while (it != m_servers.end()) {

        if (*it == server) {
            it = m_servers.erase(it);
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

    for (auto& server : m_servers) {
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
        m_servers.clear();
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
    m_servers.clear();
    for (auto& vc : sdbusData) {
        // filter by ifindex
        int ifindex = vc.get<0>();
        if (ifindex == m_ifindex) {
            m_servers.push_back(
                // store the ifindex ,domain, search
                DnsSearchServer(ifindex, vc.get<1>(), vc.get<2>()));
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
    // ifindex, version (ipv4=2 ipv6=10), byte_array, port, name
    sdbusData = v.get<std::vector<sdbus::Struct<int, int, std::vector<uint8_t>, uint16_t, std::string>>>();
    m_servers.clear();

    for (auto& vc : sdbusData) {

        int ifindex = vc.get<0>();
        if (ifindex == m_ifindex) {

            std::shared_ptr<Address> addr;

            int version = vc.get<1>();
            int port = vc.get<3>();

            if (port == 0) {
                port = 53;
            };

            if (version == 2) {
                addr = std::make_shared<IPV4Address>(vc.get<2>());
            } else if (version == 10) {
                addr = std::make_shared<IPV6Address>(vc.get<2>());
            }

            // Address addr(vc.get<2>());
            m_servers.push_back(DnsServer(ifindex, vc.get<4>(), *addr, port));
        };
    }

    return error;
}

bool DnsServerList::exportListToSdBus()
{
    bool error = false;

    std::vector<sdbus::Struct<int, std::vector<uint8_t>, uint16_t, std::string>> sdbusData;

    for (auto& server : m_servers) {
        sdbusData.push_back(sdbus::Struct<int, std::vector<uint8_t>, uint16_t, std::string>(
            sdbus::make_struct(server.getAddress()->getVersion(), server.getAddress()->byteVector(), server.getPort(), server.getName())));
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
        m_servers.clear();
    }

    return error;
}

int DnsServerList::getIfIndex() { return m_ifindex; };

bool DnsServerList::addDnsServer(DnsServer srv)
{
    for (DnsServer& server : m_servers) {
        if (server.getName().compare(srv.getName()) == 0) {
            // the name is the key in yang
            // allready exists, return false
            return false;
        }
    }

    m_servers.push_back(srv);
    return true;
}

std::vector<DnsServer> DnsServerList::getDnsServerVector() { return m_servers; };

bool DnsServerList::removeDnsServer(const DnsServer& server)
{
    std::vector<DnsServer>::iterator it = m_servers.begin();
    // safe aproach to erase while iterating
    while (it != m_servers.end()) {

        if (*it == server) {
            it = m_servers.erase(it);
            return true;
        } else {
            it++;
        }
    }

    return false;
}

bool DnsServerList::modifyDnsServer(DnsServer server)
{

    for (auto& srv : m_servers) { // name is the key in yang model, no duplicates
        if (srv.getName().compare(server.getName()) == 0) {

            if(srv.getAddress()->getVersion() != server.getAddress()->getVersion()){
                throw std::runtime_error("Server has diferent IPVersion, cannot modify!");
            }

            srv.setAddress(*server.getAddress());
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
        std::shared_ptr<Address> addr = nullptr;

        if (map.find("port") != map.end()) {
            port = stoi(map.at("port"));
        }

        std::string adr = map["address"];
         struct in_addr ip_addr;
         struct in6_addr ip_addr_6;
        //int ip_version = getAddressIPVersion(adr);


        if (inet_pton(AF_INET, adr.c_str(), &ip_addr) == 1) {

            addr = std::make_shared<IPV4Address>(adr);

        } else if (inet_pton(AF_INET6, adr.c_str(), &ip_addr_6) == 1) {
            
            addr = std::make_shared<IPV6Address>(adr);

        } else {
            throw std::runtime_error("Unknown address format!");
        }

        DnsServer server(SYSTEMD_IFINDEX, map["name"], *addr, port);
        return server;
    }

    return std::nullopt;
};

} // end of namespace
