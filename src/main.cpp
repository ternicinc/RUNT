#include <iostream>
#include <chrono>
#include <iomanip>
#include "ip_manager.hpp"

void log(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::cout << "[" << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S")
              << "." << std::setfill('0') << std::setw(3) << now_ms.count() << "] "
              << message << std::endl;
}

int main() {
    try {
        log("Starting IP allocation program");
        
        {
            log("Creating IP pool 1 (192.168.1.1-100)");
            IPManager pool1("192.168.1.100"); 

            log("Creating IP pool 2 (192.168.2.10-50)");
            IPManager pool2("192.168.2.10-50"); 

            log("Allocating from pool 1");
            std::string ip1 = pool1.allocate_ip();
            log("Allocated IP from pool 1: " + ip1);

            log("Allocating from pool 2");
            std::string ip2 = pool2.allocate_ip();
            log("Allocated IP from pool 2: " + ip2);

            // Show available IPs (for debugging)
            auto available1 = pool1.get_available_ips();
            log("Pool 1 has " + std::to_string(available1.size()) + " available IPs remaining");
            
            auto available2 = pool2.get_available_ips();
            log("Pool 2 has " + std::to_string(available2.size()) + " available IPs remaining");
        }

        log("IP pools destroyed, snapshots saved");
    } catch (const IPAllocationError& e) {
        log("ERROR: " + std::string(e.what()));
        return 1;
    } catch (const std::exception& e) {
        log("UNEXPECTED ERROR: " + std::string(e.what()));
        return 2;
    }

    return 0;
}