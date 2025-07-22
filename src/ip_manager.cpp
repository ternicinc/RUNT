#include "ip_manager.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>

IPManager::IPManager(const std::string& subnet, const std::string& snapshot_dir)
    : subnet_(subnet), snapshot_dir_(snapshot_dir) {
    
    // Ensure snapshot directory exists
    mkdir(snapshot_dir_.c_str(), 0755);
    
    initialize_available_ips();
    restore_snapshot();
}

IPManager::~IPManager() {
    take_snapshot();
}

void IPManager::take_snapshot() const {
    std::string tmp_path = get_snapshot_path() + ".tmp";
    std::ofstream out(tmp_path, std::ios::binary);
    
    if (!out) {
        throw IPAllocationError("Failed to create snapshot file");
    }
    
    // Write allocated IPs
    size_t count = allocated_ips_.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (const auto& ip : allocated_ips_) {
        size_t len = ip.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(ip.data(), len);
    }
    
    // Atomic write
    out.close();
    if (rename(tmp_path.c_str(), get_snapshot_path().c_str()) != 0) {
        throw IPAllocationError("Failed to commit snapshot");
    }
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


void IPManager::restore_snapshot() {
    std::ifstream in(get_snapshot_path(), std::ios::binary);
    if (!in) return; // No snapshot exists yet
    
    allocated_ips_.clear();
    
    // Read allocated IPs
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    for (size_t i = 0; i < count; ++i) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        
        std::string ip(len, '\0');
        in.read(&ip[0], len);
        
        allocated_ips_.insert(ip);
        available_ips_.erase(ip);
    }
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
    // First populate all possible IPs
    for (int i = 1; i < 255; i++) {
        std::ostringstream oss;
        oss << "192.168.1." << i;
        available_ips_.insert(oss.str());
    }
    
    // Then remove any that were allocated in a previous session
    for (const auto& ip : allocated_ips_) {
        available_ips_.erase(ip);
    }
}
bool IPManager::is_valid_ip(const std::string& ip) const {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) != 0;
}

bool IPManager::is_in_subnet(const std::string& ip) const {
    return true;
}