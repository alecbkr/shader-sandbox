#include "core/ui/AssetsInspectorUI.hpp"

#include "core/TextureRegistry.hpp"
#include "object/Texture.hpp"

void AssetsInspectorUI::draw(TextureRegistry* textureRegPtr) {
    for (const Texture* texPtr : textureRegPtr->readTextures()) {
        ImGui::Text("%s\n", texPtr->path.c_str());
    }
}
