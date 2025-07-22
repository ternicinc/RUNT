#ifndef IP_MANAGER_HPP
#define IP_MANAGER_HPP

#include <string>
#include <set>
#include <vector>
#include <stdexcept>
#include <chrono>

class IPAllocationError : public std::runtime_error {
public:
    explicit IPAllocationError(const std::string& msg) : std::runtime_error(msg) {}
};

class IPManager {
public:
    explicit IPManager(const std::string& subnet, const std::string& snapshot_dir = "./data");
    ~IPManager();
    
    std::string allocate_ip();
    void allocate_specific_ip(const std::string& ip);
    void deallocate_ip(const std::string& ip);
    bool is_allocated(const std::string& ip) const;
    
    std::vector<std::string> get_allocated_ips() const;
    std::vector<std::string> get_available_ips() const;
    
    void take_snapshot() const;
    void restore_snapshot();
    
    std::string get_subnet() const { return subnet_; }

private:
    std::string subnet_;
    std::string snapshot_dir_;
    std::set<std::string> allocated_ips_;
    std::set<std::string> available_ips_;
    
    void initialize_available_ips();
    bool is_valid_ip(const std::string& ip) const;
    bool is_in_subnet(const std::string& ip) const;
    
    std::string get_snapshot_path() const {
        return snapshot_dir_ + "/ip_snapshot.dat";
    }
};

#endif // IP_MANAGER_HPP