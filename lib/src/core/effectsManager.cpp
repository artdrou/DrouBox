#include "effectsManager.h"

void EffectManager::SetBlockSize(size_t blockSize) {
    bufferA_.resize(blockSize, 0.0f);
    bufferB_.resize(blockSize, 0.0f);
}

void EffectManager::AddEffect(EffectBase* effect) {
    effects_.push_back(effect);
}

void EffectManager::NextEffect() {
    if (!effects_.empty()) {
        activeIndex_ = (activeIndex_ + 1) % effects_.size();
        controls_.GetLed(1).BlinkFor(200, 50, System::GetNow());
    }
}

void EffectManager::PreviousEffect() {
    if (!effects_.empty()) {
        activeIndex_ = (activeIndex_ - 1) % effects_.size();
        controls_.GetLed(0).BlinkFor(200, 50, System::GetNow());
    }
}

EffectBase* EffectManager::GetActiveEffect() {
    if (!effects_.empty()) {
        return effects_[activeIndex_];
    }
    else {
        return nullptr;
    }
}

void EffectManager::LockEffect() {
    if (!effects_.empty()) {
        GetActiveEffect()->ToggleControlsLock();
    }
}

void EffectManager::UpdateParameters() {
    if (!effects_.empty()) {
        for (auto* effect : effects_) {
            if (effect->controlsActive)
                effect->UpdateParameters();
        }
    }
}

void EffectManager::Process(const float* in, float* out, size_t size) {
    if (effects_.empty() || globalBypass_) {
        Bypass(in, out, size);
        return;
    }
    std::copy(in, in + size, bufferA_.begin());
    float* currentInput  = bufferA_.data();
    float* currentOutput = bufferB_.data();
    for (auto* effect : effects_) {
        if (effect->effectActive) {
            effect->Process(currentInput, currentOutput, size);
            std::swap(currentInput, currentOutput);
        }
    }
    if (currentInput != out) {
        std::copy(currentInput, currentInput + size, out);
    }
}

void EffectManager::UpdateUI() {
    if (effects_.empty() || globalBypass_) {
        return;
    }
    if (!effects_.empty() && !controls_.GetLed(0).IsBlinking() && !controls_.GetLed(1).IsBlinking())
        effects_[activeIndex_]->UpdateUI();
}

void EffectManager::Bypass(const float* in, float* out, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        out[i] = in[i];
    }
}

float EffectManager::GetActiveUpdateRateMs() const {
    if (!effects_.empty())
        return effects_[activeIndex_]->GetUpdateRateMs();
    return 10.0f;
}

void EffectManager::ProcessControlGestures() {
    // --- Individuels ---
    if (footswitchPair_.ATap()) {
        if (GetActiveEffect() != nullptr) {
            if (GetActiveEffect()->controlsActive)
                GetActiveEffect()->ToggleBypass();
        }  
    };
    if (footswitchPair_.AHeld()) PreviousEffect();
    if (footswitchPair_.BHeld()) NextEffect();

    // // if (bTap) {
    // //     auto* delay = dynamic_cast<DelayEffect*>(GetActiveEffect());
    // //     if (delay) delay->RegisterTap(controls.GetLastTapTime(1));
    // // }
    

    // --- Combinaisons ---
    if (footswitchPair_.BothTapped())  GetActiveEffect()->ToggleControlsLock();
    if (footswitchPair_.BothHeldForTrigger()) globalBypass_ = !globalBypass_;
}
