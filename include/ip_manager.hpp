#ifndef IP_MANAGER_HPP
#define IP_MANAGER_HPP

#include <string>
#include <vector>
#include <set>
#include <stdexcept>

class IPAllocationError : public std::runtime_error {
public:
    explicit IPAllocationError(const std::string& msg) : std::runtime_error(msg) {}
};

class IPManager {
public:
    // Initialize with a subnet (e.g., "192.168.1.0/24")
    explicit IPManager(const std::string& subnet);
    
    // Allocate a new IP address
    std::string allocate_ip();
    
    // Allocate a specific IP address
    void allocate_specific_ip(const std::string& ip);
    
    // Deallocate an IP address
    void deallocate_ip(const std::string& ip);
    
    // Check if an IP is allocated
    bool is_allocated(const std::string& ip) const;
    
    // Get all allocated IPs
    std::vector<std::string> get_allocated_ips() const;
    
    // Get all available IPs
    std::vector<std::string> get_available_ips() const;
    
    // Get the subnet
    std::string get_subnet() const { return subnet_; }

private:
    std::string subnet_;
    std::set<std::string> allocated_ips_;
    std::set<std::string> available_ips_;
    
    void initialize_available_ips();
    bool is_valid_ip(const std::string& ip) const;
    bool is_in_subnet(const std::string& ip) const;
};

#endif // IP_MANAGER_HPP