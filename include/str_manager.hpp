#ifndef STR_MANAGER_HPP
#define STR_MANAGER_HPP

#include <string>
#include <set>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <iomanip>

class StorageAllocationError : public std::runtime_error {
public:
    explicit StorageAllocationError(const std::string& msg) : std::runtime_error(msg) {}
};

class StorageManager {
public:
    explicit StorageManager(const std::string& pool, const std::string& storage_dir = "./storage_data");
    ~StorageManager();

    std::string allocate_str();
    void deallocate_str(const std::string& part_name);
    bool is_allocated(const std::string& part_name) const;

    std::vector<std::string> get_allocated_parts() const;
    std::vector<std::string> get_available_parts() const;

    void take_backup() const;
    void restore_backup();

    std::string get_pool() const { return pool_; }

private:
    std::string pool_;
    std::string storage_dir;
    std::set<std::string> allocated_parts;
    std::set<std::string> available_parts;

    void initialize_available_parts();
    bool is_valid_part(const std::string& part_name) const;
    bool is_in_pool(const std::string& part_name) const;

    std::string get_storage_path() const {
        return storage_dir + "/backups";
    }
    void log(const std::string& message) const {
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);
        auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::cout << "[STORAGE_MANAGER][" << pool_ << "][" 
                  << std::put_time(std::localtime(&now_time), "%H:%M:%S")
                  << "." << std::setfill('0') << std::setw(3) << now_ms.count() << "] "
                  << message << std::endl;
    }
};

#endif // STR_MANAGER_HPP