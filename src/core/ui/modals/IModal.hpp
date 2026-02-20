#pragma once
#include <string_view>

class IModal {
public:
    virtual ~IModal() = default;

    // Must be stable + unique (used as ImGui popup name)
    virtual std::string_view id() const = 0;

    // Draw the popup contents. Called only when popup is open.
    virtual void draw() = 0;
};