#pragma once

enum class ModelState {
    Empty,
    Building,
    Ready, 
    Error
};

struct ModelStatus {
    ModelState meshes   = ModelState::Empty;
    ModelState material = ModelState::Empty;
    bool wasSentToRenderer = false;
};
