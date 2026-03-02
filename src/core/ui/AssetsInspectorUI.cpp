#include "core/ui/AssetsInspectorUI.hpp"

#include "core/TextureRegistry.hpp"
#include "texture/Texture.hpp"

void AssetsInspectorUI::draw(TextureRegistry* textureRegPtr) {
    for (const Texture* texPtr : textureRegPtr->readTextures()) {
        ImGui::Text("%s\n", texPtr->path.c_str());
    }
}
