#include "ip_manager.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <arpa/inet.h>

IPManager::IPManager(const std::string& subnet) : subnet_(subnet) {
    initialize_available_ips();
}

std::string IPManager::allocate_ip() {
    if (available_ips_.empty()) {
        throw IPAllocationError("No available IP addresses in pool");
    }
    
    std::set<std::string>::iterator it = available_ips_.begin();
    std::string ip = *it;
    available_ips_.erase(it);
    allocated_ips_.insert(ip);
    return ip;
}

void IPManager::allocate_specific_ip(const std::string& ip) {
    if (!is_valid_ip(ip)) {  // Fixed missing parenthesis
        throw IPAllocationError("Invalid IP address format");
    }
    
    if (!is_in_subnet(ip)) {
        throw IPAllocationError("IP address not in subnet");
    }
    
    if (allocated_ips_.find(ip) != allocated_ips_.end()) {
        throw IPAllocationError("IP address already allocated");
    }
    
    std::set<std::string>::iterator it = available_ips_.find(ip);
    if (it == available_ips_.end()) {
        throw IPAllocationError("IP address not available");
    }
    
    available_ips_.erase(it);
    allocated_ips_.insert(ip);
}

void IPManager::deallocate_ip(const std::string& ip) {
    std::set<std::string>::iterator it = allocated_ips_.find(ip);
    if (it == allocated_ips_.end()) {
        throw IPAllocationError("IP address not currently allocated");
    }
    
    allocated_ips_.erase(it);
    available_ips_.insert(ip);
}

bool IPManager::is_allocated(const std::string& ip) const {
    return allocated_ips_.find(ip) != allocated_ips_.end();
}

std::vector<std::string> IPManager::get_allocated_ips() const {
    std::vector<std::string> result;
    result.assign(allocated_ips_.begin(), allocated_ips_.end());
    return result;
}

std::vector<std::string> IPManager::get_available_ips() const {
    std::vector<std::string> result;
    result.assign(available_ips_.begin(), available_ips_.end());
    return result;
}

void IPManager::initialize_available_ips() {
    for (int i = 1; i < 255; i++) {
        std::ostringstream oss;
        oss << "192.168.1." << i;
        available_ips_.insert(oss.str());
    }
}

bool IPManager::is_valid_ip(const std::string& ip) const {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) != 0;
}

bool IPManager::is_in_subnet(const std::string& ip) const {
    return true;
}