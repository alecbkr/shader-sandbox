#pragma once

enum class ModelState {
    Empty,
    Building,
    Ready, 
    Invalid,
    Error
};

struct ModelStatus {
    ModelState meshes   = ModelState::Empty;
    ModelState material = ModelState::Empty;
    bool uploadedToRenderer = false; //only to be manipulated by renderer directly
};
