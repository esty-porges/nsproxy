#pragma once

#include <map>
#include <string>
#include <memory>
#include "matrix_obj.h"
#include "object_manager.h"

namespace matrix_service {

class MatrixStore {

private:
    MatrixStore() = default;
    std::map<std::string, MatrixObj> matrices_;
    
public:
    static MatrixStore& instance() {
        static MatrixStore instance;
        return instance;
    }

    bool addMatrix(const std::string& name, int rows, int cols) {
        if (ObjectManager::instance().is_registered(name)) {
            return false;
        }

        auto [it, inserted] = matrices_.try_emplace(name, name, rows, cols);
        if (!inserted) {
            return false;
        }

        if (!ObjectManager::instance().register_object(name)) {
            matrices_.erase(name);
            return false;
        }

        return true;
    }

    MatrixObj* getMatrix(const std::string& name) {
        auto it = matrices_.find(name);
        return it != matrices_.end() ? &it->second : nullptr;
    }

    bool removeMatrix(const std::string& name) {
        if (!ObjectManager::instance().is_registered(name)) {
            return false;
        }
        matrices_.erase(name);
        ObjectManager::instance().unregister_object(name);
        return true;
    }

    std::vector<std::string> getRegisteredMatrices() const {
        return ObjectManager::instance().get_registered_objects();
    }


};

} // namespace matrix_service
