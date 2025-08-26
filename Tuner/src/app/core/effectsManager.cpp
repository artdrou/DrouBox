#include "effectsManager.h"

void EffectManager::AddEffect(EffectBase* effect) {
    effects_.push_back(effect);
}

void EffectManager::NextEffect() {
    activeIndex_ = (activeIndex_ + 1) % effects_.size();
}

EffectBase* EffectManager::GetActiveEffect() {
    return effects_[activeIndex_];
}

void EffectManager::UpdateParameters(const ControlMapper& mapper) {
    if (!effects_.empty())
        effects_[activeIndex_]->UpdateParameters();
}

void EffectManager::Process(const float* in, float* out, size_t size) {
    if (!effects_.empty())
        effects_[activeIndex_]->Process(in, out, size);
    else
        Bypass(in, out, size);
}

void EffectManager::UpdateUI(Controls& controls) {
    if (!effects_.empty())
        effects_[activeIndex_]->UpdateUI();
}

void EffectManager::Bypass(const float* in, float* out, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        out[i] = in[i];
    }
}
