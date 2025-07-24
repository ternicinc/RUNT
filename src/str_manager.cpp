#include "../include/str_manager.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fcntl.h>

StorageManager::StorageManager(const std::string& pool, const std::string& storage_dir)
    : pool_(pool), storage_dir_(storage_dir) {

        log("Initializing StorageManager for pool: " + pool);

        // Ensure storage directory exists
        if (mkdir(storage_dir.c_str(), 0755) != 0 && errno != EEXIST) {
            log("Failed to create storage directory: " + storage_dir);
            throw StorageAllocationError("Failed to create storage directory.");
        }

        // Ensure backups directory exists
        std::string backup_dir = get_storage_path();
        if (mkdir(backup_dir.c_str(), 0755) != 0 && errno != EEXIST) {
            log("Failed to create backup directory: " + backup_dir);
            throw StorageAllocationError("Failed to create backup directory.");
        }

        log("Storage directory ready: " + storage_dir);
        initialize_available_parts();
        restore_backup();
    }


StorageManager::~StorageManager() {
    take_backup();
}

void StorageManager::take_backup() const {
    std::string tmp_path = get_storage_path() + ".tmp";
    std::ofstream out(tmp_path, std::ios::binary);

    if (!out) {
        throw StorageAllocationError("Failed to create backup.");
    }

    size_t count = allocated_parts_.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (const auto& pool : allocated_parts_) {
        size_t len = pool.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(pool.data(), len);
    }

    out.close();
    if (rename(tmp_path.c_str(), get_storage_path().c_str()) != 0) {
        throw StorageAllocationError("Failed to commit backup.");

    }
}

std::string StorageManager::allocate_part() {
    if (available_parts_.empty()) {
        log("No available storage parts in pool.");
        throw StorageAllocationError("No available storage parts in the pool.");
    }

    std::set<std::string>::iterator it = available_parts_.begin();
    std::string part = *it;
    available_parts_.erase(it);
    allocated_parts_.insert(part);

    log("Allocated Storage Part: " + part +  "(Remaining :" + std::to_string(available_parts_.size()) + ")");
    return part;
}

void StorageManager::restore_backup() {
    std::ifstream in(get_storage_path(), std::ios::binary);
    if (!in) return;

    allocated_parts_.clear();


    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));

        std::string part(len, '\0');
        in.read(&part[0], len);

        allocated_parts_.insert(part);
        available_parts_.erase(part);
    }
}

void StorageManager::deallocate_part(const std::string& part) {
    std::set<std::string>::iterator it = allocated_parts_.find(part);
    if (it == allocated_parts_.end()) {
        throw StorageAllocationError("Storage part is not currently allocated.");
    }

    allocated_parts_.erase(it);
    available_parts_.insert(part);

}

bool StorageManager::is_allocated(const std::string& part) const {
    return allocated_parts_.find(part) != allocated_parts_.end();
}

std::vector<std::string> StorageManager::get_allocated_parts() const {
    std::vector<std::string> result;
    result.assign(allocated_parts_.begin(), allocated_parts_.end());
    return result;
}

std::vector<std::string> StorageManager::get_available_parts() const {
    std::vector<std::string> result;
    result.assign(available_parts_.begin(), available_parts_.end());
    return result;
}

void StorageManager::initialize_available_parts() {
    log("Initializing available storage parts for pool: " + pool_);

    available_parts_.clear();

    // Create 100 storage parts, each with its own directory
    for (int i = 0; i < 100; ++i) {
        std::string part_name = "part" + std::to_string(i);
        std::string part_path = storage_dir_ + "/" + part_name;
        
        // Create directory for the part
        if (mkdir(part_path.c_str(), 0755) != 0 && errno != EEXIST) {
            log("Failed to create directory for part: " + part_name);
            continue;
        }

        // Create a data file with default size (1MB)
        std::string data_file = part_path + "/data.bin";
        const size_t default_size = 1024 * 1024; // 1MB
        
        int fd = open(data_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            log("Failed to create data file for part: " + part_name);
            continue;
        }
        
        // Allocate space by seeking to end and writing one byte
        if (ftruncate(fd, default_size) == -1) {
            log("Failed to allocate space for part: " + part_name);
            close(fd);
            continue;
        }
        
        close(fd);
        available_parts_.insert(part_name);
    }

    log("Initialized " + std::to_string(available_parts_.size()) + " available parts");
}

bool StorageManager::is_in_pool(const std::string& part) const {
    // Check if the part exists in the storage directory
    std::string part_path = storage_dir_ + "/" + part;
    struct stat info;
    return (stat(part_path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
}

bool StorageManager::is_valid_part(const std::string& part_name) const {
    if (part_name.size() < 5 || part_name.substr(0, 4) != "part") {
        return false;
    }
    
    try {
        std::stoi(part_name.substr(4));
    } catch (...) {
        return false;
    }
    
    return true;
}
