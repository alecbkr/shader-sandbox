#pragma once
#include <unordered_map>
#include <string>
#include <imgui/imgui.h>

#include "core/ui/modals/IModal.hpp"

class ModalManager {
public:
    ModalManager() = default;

    // You own the modal objects elsewhere (as members); manager stores pointers.
    void registerModal(IModal* modal);

    // Request to open on the next render() call.
    void open(std::string_view modalId);

    // Close current popup (optional convenience).
    void closeCurrent();

    // Call every frame from a central place (e.g., Application UI pass).
    void render();

private:
    std::unordered_map<std::string, IModal*> modals;
    std::string pendingOpen; // empty => none
};