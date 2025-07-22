#include <iostream>
#include "ip_manager.hpp"

int main() {
    {
        // This will automatically save when exiting scope
        IPManager manager("192.168.1.0/24");
        
        auto ip = manager.allocate_ip();
        std::cout << "Allocated IP: " << ip << std::endl;
        
        // Explicit save if needed
        manager.take_snapshot();
    } 
    
    // New instance will restore previous state
    IPManager manager2("192.168.1.0/24");
    if (manager2.is_allocated("192.168.1.1")) {
        std::cout << "IP 192.168.1.1 is already allocated" << std::endl;
    }

    IPManager manager3("192.168.1.0/24");
    
    // Allocated random IP address, from the pool.
    auto ip2 = manager3.allocate_ip();
    std::cout << "Allocated IP:" << ip2 << std::endl;

    manager3.take_snapshot();
    
    return 0;
}