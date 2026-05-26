#include <catch2/catch_test_macros.hpp>
#include "daisy_seed.h"
#include "footSwitches.h"

// Exposes the internal daisy::Switch so tests can drive hw state directly.
// footswitch is protected in FootswitchManager (test-only visibility change).
class TestableFootswitch : public FootswitchManager {
public:
    daisy::Switch& hw() { return footswitch; }
};

TEST_CASE("FootswitchManager: IsTap true after short press-release", "[footswitch]") {
    daisy::System::ResetClock();
    TestableFootswitch fs;

    fs.hw().SimPress();
    fs.Update();

    fs.hw().SimRelease(200.f);
    fs.Update();

    REQUIRE(fs.IsTap(1000));
}

TEST_CASE("FootswitchManager: IsTap false after long press", "[footswitch]") {
    daisy::System::ResetClock();
    TestableFootswitch fs;

    fs.hw().SimPress();
    fs.Update();

    // holdMs_ is updated in Update() while Pressed() — must simulate the hold frame
    fs.hw().SimHold(1200.f);
    fs.Update();

    fs.hw().SimRelease(1200.f);
    fs.Update();

    REQUIRE_FALSE(fs.IsTap(1000));
}

TEST_CASE("FootswitchManager: IsTap false when still held", "[footswitch]") {
    daisy::System::ResetClock();
    TestableFootswitch fs;

    fs.hw().SimPress();
    fs.Update();
    fs.hw().SimHold(100.f);
    fs.Update();

    REQUIRE_FALSE(fs.IsTap(1000));
}

TEST_CASE("FootswitchManager: IsHeldForTrigger fires once at threshold", "[footswitch]") {
    daisy::System::ResetClock();
    TestableFootswitch fs;

    fs.hw().SimPress();
    fs.Update();

    fs.hw().SimHold(500.f);
    fs.Update();
    REQUIRE_FALSE(fs.IsHeldForTrigger(1.f));

    fs.hw().SimHold(1100.f);
    fs.Update();
    REQUIRE(fs.IsHeldForTrigger(1.f));

    // Still held — must NOT re-trigger
    fs.hw().SimHold(1500.f);
    fs.Update();
    REQUIRE_FALSE(fs.IsHeldForTrigger(1.f));
}

TEST_CASE("FootswitchManager: IsHeldForTrigger resets after release", "[footswitch]") {
    daisy::System::ResetClock();
    TestableFootswitch fs;

    fs.hw().SimPress(); fs.Update();
    fs.hw().SimHold(1100.f); fs.Update();
    REQUIRE(fs.IsHeldForTrigger(1.f));

    // holdTriggered_ resets only when IsHeldForTrigger() is called while released.
    // In production this happens every audio frame — simulate that here.
    fs.hw().SimRelease(1100.f); fs.Update();
    REQUIRE_FALSE(fs.IsHeldForTrigger(1.f)); // resets holdTriggered_
    fs.hw().SimIdle(); fs.Update();

    // Second press — should trigger again
    fs.hw().SimPress(); fs.Update();
    fs.hw().SimHold(1100.f); fs.Update();
    REQUIRE(fs.IsHeldForTrigger(1.f));
}

TEST_CASE("FootswitchManager: IsPressed reflects hw state", "[footswitch]") {
    daisy::System::ResetClock();
    TestableFootswitch fs;

    fs.hw().SimIdle();
    fs.Update();
    REQUIRE_FALSE(fs.IsPressed());

    fs.hw().SimPress();
    fs.Update();
    REQUIRE(fs.IsPressed());

    fs.hw().SimHold(200.f);
    fs.Update();
    REQUIRE(fs.IsPressed());

    fs.hw().SimRelease(200.f);
    fs.Update();
    REQUIRE_FALSE(fs.IsPressed());
}
