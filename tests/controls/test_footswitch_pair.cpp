#include <catch2/catch_test_macros.hpp>
#include "daisy_seed.h"
#include "footSwitches.h"
#include "footSwitchPair.h"

class TestableFootswitch : public FootswitchManager {
public:
    daisy::Switch& hw() { return footswitch; }
};

struct PairFixture {
    TestableFootswitch a, b;
    FootswitchPair pair{&a, &b};

    PairFixture() {
        daisy::System::ResetClock();
    }

    void idle() {
        a.hw().SimIdle(); b.hw().SimIdle();
        pair.Update();
    }

    void holdBoth(float heldMs) {
        a.hw().SimHold(heldMs); b.hw().SimHold(heldMs);
        pair.Update();
    }
};

// ── ATap / BTap ────────────────────────────────────────────────────────────────

TEST_CASE("FootswitchPair: ATap fires when only A tapped", "[footswitch_pair]") {
    PairFixture f;

    f.a.hw().SimPress(); f.b.hw().SimIdle(); f.pair.Update();
    f.a.hw().SimRelease(100.f); f.b.hw().SimIdle(); f.pair.Update();

    REQUIRE(f.pair.ATap());
}

TEST_CASE("FootswitchPair: BTap fires when only B tapped", "[footswitch_pair]") {
    PairFixture f;

    f.b.hw().SimPress(); f.a.hw().SimIdle(); f.pair.Update();
    f.b.hw().SimRelease(100.f); f.a.hw().SimIdle(); f.pair.Update();

    REQUIRE(f.pair.BTap());
}

TEST_CASE("FootswitchPair: ATap blocked when B is currently pressed", "[footswitch_pair]") {
    PairFixture f;

    f.b.hw().SimPress(); f.pair.Update();
    f.a.hw().SimPress(); f.pair.Update();
    f.a.hw().SimRelease(100.f); f.pair.Update();

    REQUIRE_FALSE(f.pair.ATap());
}

TEST_CASE("FootswitchPair: BTap blocked when A is currently pressed", "[footswitch_pair]") {
    PairFixture f;

    f.a.hw().SimPress(); f.pair.Update();
    f.b.hw().SimPress(); f.pair.Update();
    f.b.hw().SimRelease(100.f); f.pair.Update();

    REQUIRE_FALSE(f.pair.BTap());
}

// ── BothTapped ─────────────────────────────────────────────────────────────────

TEST_CASE("FootswitchPair: BothTapped fires when both tapped simultaneously", "[footswitch_pair]") {
    PairFixture f;

    f.a.hw().SimPress(); f.b.hw().SimPress(); f.pair.Update();
    f.a.hw().SimRelease(100.f); f.b.hw().SimRelease(100.f); f.pair.Update();

    REQUIRE(f.pair.BothTapped());
}

TEST_CASE("FootswitchPair: BothTapped fires when B tapped within tolerance of A", "[footswitch_pair]") {
    PairFixture f;

    // A taps — call BothTapped on the release frame to record lastATap
    f.a.hw().SimPress(); f.b.hw().SimIdle(); f.pair.Update();
    f.a.hw().SimRelease(100.f); f.b.hw().SimIdle(); f.pair.Update();
    f.pair.BothTapped(); // won't fire (B not tapped), but IsTap() records lastATap

    daisy::System::AdvanceMs(100); // 100 ms later — within 250 ms tolerance

    // B taps — BothTapped checked on this exact release frame
    f.b.hw().SimPress(); f.a.hw().SimIdle(); f.pair.Update();
    f.b.hw().SimRelease(100.f); f.a.hw().SimIdle(); f.pair.Update();

    REQUIRE(f.pair.BothTapped()); // bTap=true, |lastATap - lastBTap|=100 <= 250
}

TEST_CASE("FootswitchPair: BothTapped does not fire when taps too far apart", "[footswitch_pair]") {
    PairFixture f;

    f.a.hw().SimPress(); f.b.hw().SimIdle(); f.pair.Update();
    f.a.hw().SimRelease(100.f); f.b.hw().SimIdle(); f.pair.Update();
    f.pair.BothTapped();

    daisy::System::AdvanceMs(500); // beyond 250 ms tolerance

    f.b.hw().SimPress(); f.a.hw().SimIdle(); f.pair.Update();
    f.b.hw().SimRelease(100.f); f.a.hw().SimIdle(); f.pair.Update();

    REQUIRE_FALSE(f.pair.BothTapped());
}

// ── Lockout after BothTapped ───────────────────────────────────────────────────

TEST_CASE("FootswitchPair: ATap blocked during lockout after BothTapped", "[footswitch_pair]") {
    PairFixture f;

    f.a.hw().SimPress(); f.b.hw().SimPress(); f.pair.Update();
    f.a.hw().SimRelease(100.f); f.b.hw().SimRelease(100.f); f.pair.Update();
    f.pair.BothTapped(); // sets lockout

    // Immediately try to tap A — should be locked out
    f.a.hw().SimPress(); f.b.hw().SimIdle(); f.pair.Update();
    f.a.hw().SimRelease(100.f); f.b.hw().SimIdle(); f.pair.Update();

    REQUIRE_FALSE(f.pair.ATap());
}

TEST_CASE("FootswitchPair: ATap works after lockout expires", "[footswitch_pair]") {
    PairFixture f;

    f.a.hw().SimPress(); f.b.hw().SimPress(); f.pair.Update();
    f.a.hw().SimRelease(100.f); f.b.hw().SimRelease(100.f); f.pair.Update();
    f.pair.BothTapped();

    daisy::System::AdvanceMs(300); // default lockout is 250 ms
    f.idle();

    f.a.hw().SimPress(); f.b.hw().SimIdle(); f.pair.Update();
    f.a.hw().SimRelease(100.f); f.b.hw().SimIdle(); f.pair.Update();

    REQUIRE(f.pair.ATap());
}

// ── BothHeldForTrigger ─────────────────────────────────────────────────────────

TEST_CASE("FootswitchPair: BothHeldForTrigger fires when both held long enough", "[footswitch_pair]") {
    PairFixture f; // clock already at 10 ms — press timestamps will be non-zero

    f.a.hw().SimPress(); f.b.hw().SimPress();
    f.pair.Update(); // records lastPressTimeMs for both

    f.holdBoth(1100.f);
    REQUIRE(f.pair.BothHeldForTrigger(1.f));
}

TEST_CASE("FootswitchPair: BothHeldForTrigger fires only once while held", "[footswitch_pair]") {
    PairFixture f;

    f.a.hw().SimPress(); f.b.hw().SimPress();
    f.pair.Update();

    f.holdBoth(1100.f);
    REQUIRE(f.pair.BothHeldForTrigger(1.f));

    f.holdBoth(1500.f);
    REQUIRE_FALSE(f.pair.BothHeldForTrigger(1.f));
}

TEST_CASE("FootswitchPair: BothHeldForTrigger does not fire below threshold", "[footswitch_pair]") {
    PairFixture f;

    f.a.hw().SimPress(); f.b.hw().SimPress();
    f.pair.Update();

    f.holdBoth(500.f);
    REQUIRE_FALSE(f.pair.BothHeldForTrigger(1.f));
}

// ── AHeld / BHeld ─────────────────────────────────────────────────────────────

TEST_CASE("FootswitchPair: AHeld fires when only A held", "[footswitch_pair]") {
    PairFixture f;

    f.a.hw().SimPress(); f.b.hw().SimIdle(); f.pair.Update();
    f.a.hw().SimHold(1100.f); f.b.hw().SimIdle(); f.pair.Update();

    REQUIRE(f.pair.AHeld(1.f));
}

TEST_CASE("FootswitchPair: AHeld blocked when B is pressed", "[footswitch_pair]") {
    PairFixture f;

    f.a.hw().SimPress(); f.b.hw().SimPress(); f.pair.Update();
    f.a.hw().SimHold(1100.f); f.b.hw().SimHold(1100.f); f.pair.Update();

    REQUIRE_FALSE(f.pair.AHeld(1.f));
}
