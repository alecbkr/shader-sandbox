#include "core/ui/modals/ModalManager.hpp"

void ModalManager::registerModal(IModal* modal) {
    if (!modal) return;
    modals[std::string(modal->id())] = modal;
}

void ModalManager::open(std::string_view modalId) {
    pendingOpen = std::string(modalId);
}

void ModalManager::closeCurrent() {
    ImGui::CloseCurrentPopup();
}

void ModalManager::render() {
    // If something requested to open, open it now (must happen during a frame).
    if (!pendingOpen.empty()) {
        auto it = modals.find(pendingOpen);
        if (it != modals.end() && it->second) {
            ImGui::OpenPopup(it->second->id().data());
        }
        pendingOpen.clear();
    }

    // Draw any modal that is currently open.
    for (auto& [id, modal] : modals) {
        if (!modal) continue;

        // If it's open, draw it.
        if (ImGui::IsPopupOpen(id.c_str(), ImGuiPopupFlags_None)) {
            // You can tweak flags per-modal if you want, but this is a good default.
            ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;

            if (ImGui::BeginPopupModal(id.c_str(), nullptr, flags)) {
                modal->draw();
                ImGui::EndPopup();
            }
        }
    }
}