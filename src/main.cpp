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
    } // Auto-save happens here
    
    // New instance will restore previous state
    IPManager manager2("192.168.1.0/24");
    if (manager2.is_allocated("192.168.1.1")) {
        std::cout << "IP 192.168.1.1 is already allocated" << std::endl;
    }
    
    return 0;
}