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
    }
}

void EffectManager::PreviousEffect() {
    if (!effects_.empty()) {
        activeIndex_ = (activeIndex_ - 1) % effects_.size();
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
    if (!effects_.empty())
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
    // bool aTap   = controls_.GetFootswitch(0).IsTap(1000);
    // bool aHold  = controls_.GetFootswitch(0).IsHeldForTrigger(1000);
    // bool bTap   = controls_.GetFootswitch(1).IsTap(1000);
    // bool bHold  = controls_.GetFootswitch(1).IsHeldForTrigger(1000);

    // bool bothTap  = (controls_.GetFootswitch(0).IsTap(1000)  && controls_.GetFootswitch(1).IsTap(1000));
    // bool bothHold = (controls_.GetFootswitch(0).IsHeldForTrigger(1000) && controls_.GetFootswitch(1).IsHeldForTrigger(1000));


    // // --- Individuels ---
    // if (aTap && !bothTap) {
    //     if (GetActiveEffect() != nullptr) {
    //         GetActiveEffect()->ToggleBypass();
    //     }  
    // };
    // if (aHold && !bothHold) PreviousEffect();

    // // if (bTap) {
    // //     auto* delay = dynamic_cast<DelayEffect*>(GetActiveEffect());
    // //     if (delay) delay->RegisterTap(controls.GetLastTapTime(1));
    // // }
    // if (bHold) NextEffect();

    // // --- Combinaisons ---
    // if (bothTap)  GetActiveEffect()->ToggleControlsLock();
    // if (bothHold) globalBypass_ = !globalBypass_;
}
