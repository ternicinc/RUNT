#include <iostream>
#include "ip_manager.hpp"

int main() {
    try {
        IPManager manager("192.168.1.0/24");
        
        auto ip1 = manager.allocate_ip();
        std::cout << "Allocated IP: " << ip1 << std::endl;
        
        manager.allocate_specific_ip("192.168.1.100");
        
        manager.deallocate_ip(ip1);
        
        auto allocated = manager.get_allocated_ips();
        std::cout << "Allocated IPs: ";
        for (const auto& ip : allocated) {
            std::cout << ip << " ";
        }
        std::cout << std::endl;
        
    } catch (const IPAllocationError& e) {
        std::cerr << "IP Allocation Error: " << e.what() << std::endl;
    }
    
    return 0;
}
