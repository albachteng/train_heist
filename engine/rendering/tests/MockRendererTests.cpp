#include "../include/MockRenderer.hpp"
#include <gtest/gtest.h>

/**
 * MockRenderer Tests
 * 
 * These tests define the expected behavior of MockRenderer for testing rendering systems.
 * MockRenderer should record all rendering calls for verification without requiring graphics.
 */

class MockRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockRenderer = std::make_unique<ECS::MockRenderer>();
    }
    
    void TearDown() override {
        mockRenderer.reset();
    }
    
    std::unique_ptr<ECS::MockRenderer> mockRenderer;
};

// Test basic interface compliance
TEST_F(MockRendererTest, InterfaceCompliance) {
    // Should implement all IRenderer methods without crashing
    EXPECT_NO_THROW(mockRenderer->beginFrame());
    EXPECT_NO_THROW(mockRenderer->clear());
    EXPECT_NO_THROW(mockRenderer->endFrame());
    
    EXPECT_NO_THROW(mockRenderer->renderRect(100.0f, 100.0f, 50.0f, 50.0f, 
                                            1.0f, 0.0f, 0.0f, 1.0f));
    
    EXPECT_NO_THROW(mockRenderer->renderSprite(200.0f, 200.0f, 0.0f, 
                                              64.0f, 64.0f, 1));
    
    int width, height;
    EXPECT_NO_THROW(mockRenderer->getScreenSize(width, height));
}

// Test that calls are recorded properly
TEST_F(MockRendererTest, RecordsMethodCalls) {
    mockRenderer->beginFrame();
    mockRenderer->clear();
    mockRenderer->endFrame();
    
    EXPECT_TRUE(mockRenderer->wasMethodCalled("beginFrame"));
    EXPECT_TRUE(mockRenderer->wasMethodCalled("clear"));
    EXPECT_TRUE(mockRenderer->wasMethodCalled("endFrame"));
    EXPECT_FALSE(mockRenderer->wasMethodCalled("nonexistent"));
}

// Test sprite call recording
TEST_F(MockRendererTest, RecordsSpriteCalIs) {
    mockRenderer->renderSprite(100.0f, 200.0f, 1.0f, 64.0f, 32.0f, 42);
    
    EXPECT_EQ(mockRenderer->spriteCalls.size(), 1);
    
    const auto& call = mockRenderer->spriteCalls[0];
    EXPECT_FLOAT_EQ(call.x, 100.0f);
    EXPECT_FLOAT_EQ(call.y, 200.0f);
    EXPECT_FLOAT_EQ(call.z, 1.0f);
    EXPECT_FLOAT_EQ(call.width, 64.0f);
    EXPECT_FLOAT_EQ(call.height, 32.0f);
    EXPECT_EQ(call.textureId, 42);
}

// Test rectangle call recording
TEST_F(MockRendererTest, RecordsRectCalls) {
    mockRenderer->renderRect(50.0f, 75.0f, 100.0f, 200.0f, 
                            0.5f, 0.8f, 0.2f, 0.9f);
    
    EXPECT_EQ(mockRenderer->rectCalls.size(), 1);
    
    const auto& call = mockRenderer->rectCalls[0];
    EXPECT_FLOAT_EQ(call.x, 50.0f);
    EXPECT_FLOAT_EQ(call.y, 75.0f);
    EXPECT_FLOAT_EQ(call.width, 100.0f);
    EXPECT_FLOAT_EQ(call.height, 200.0f);
    EXPECT_FLOAT_EQ(call.red, 0.5f);
    EXPECT_FLOAT_EQ(call.green, 0.8f);
    EXPECT_FLOAT_EQ(call.blue, 0.2f);
    EXPECT_FLOAT_EQ(call.alpha, 0.9f);
}

// Test multiple calls are recorded in order
TEST_F(MockRendererTest, RecordsMultipleCalls) {
    mockRenderer->renderRect(10.0f, 20.0f, 30.0f, 40.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    mockRenderer->renderSprite(100.0f, 200.0f, 0.0f, 64.0f, 64.0f, 1);
    mockRenderer->renderRect(50.0f, 60.0f, 70.0f, 80.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    
    EXPECT_EQ(mockRenderer->rectCalls.size(), 2);
    EXPECT_EQ(mockRenderer->spriteCalls.size(), 1);
    
    // Verify first rect call
    EXPECT_FLOAT_EQ(mockRenderer->rectCalls[0].x, 10.0f);
    EXPECT_FLOAT_EQ(mockRenderer->rectCalls[0].red, 1.0f);
    
    // Verify sprite call
    EXPECT_FLOAT_EQ(mockRenderer->spriteCalls[0].x, 100.0f);
    EXPECT_EQ(mockRenderer->spriteCalls[0].textureId, 1);
    
    // Verify second rect call
    EXPECT_FLOAT_EQ(mockRenderer->rectCalls[1].x, 50.0f);
    EXPECT_FLOAT_EQ(mockRenderer->rectCalls[1].green, 1.0f);
}

// Test call count functionality
TEST_F(MockRendererTest, CountsCalls) {
    EXPECT_EQ(mockRenderer->getCallCount("beginFrame"), 0);
    
    mockRenderer->beginFrame();
    mockRenderer->beginFrame();
    mockRenderer->clear();
    
    EXPECT_EQ(mockRenderer->getCallCount("beginFrame"), 2);
    EXPECT_EQ(mockRenderer->getCallCount("clear"), 1);
    EXPECT_EQ(mockRenderer->getCallCount("endFrame"), 0);
}

// Test reset functionality
TEST_F(MockRendererTest, ResetClearsState) {
    mockRenderer->beginFrame();
    mockRenderer->renderRect(10.0f, 20.0f, 30.0f, 40.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    mockRenderer->renderSprite(100.0f, 200.0f, 0.0f, 64.0f, 64.0f, 1);
    
    EXPECT_GT(mockRenderer->methodCalls.size(), 0);
    EXPECT_GT(mockRenderer->rectCalls.size(), 0);
    EXPECT_GT(mockRenderer->spriteCalls.size(), 0);
    
    mockRenderer->reset();
    
    EXPECT_EQ(mockRenderer->methodCalls.size(), 0);
    EXPECT_EQ(mockRenderer->rectCalls.size(), 0);
    EXPECT_EQ(mockRenderer->spriteCalls.size(), 0);
    EXPECT_FALSE(mockRenderer->wasMethodCalled("beginFrame"));
}

// Test screen size simulation
TEST_F(MockRendererTest, SimulatesScreenSize) {
    int width, height;
    mockRenderer->getScreenSize(width, height);
    
    // Should have default values
    EXPECT_GT(width, 0);
    EXPECT_GT(height, 0);
    
    // Should be able to change screen size for testing
    mockRenderer->setScreenSize(1024, 768);
    mockRenderer->getScreenSize(width, height);
    
    EXPECT_EQ(width, 1024);
    EXPECT_EQ(height, 768);
}

// Test edge cases and robustness
TEST_F(MockRendererTest, HandlesEdgeCases) {
    // Zero/negative values should be recorded without crashing
    EXPECT_NO_THROW(mockRenderer->renderRect(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_NO_THROW(mockRenderer->renderRect(-10.0f, -20.0f, -5.0f, -15.0f, -1.0f, 2.0f, 0.5f, 1.5f));
    EXPECT_NO_THROW(mockRenderer->renderSprite(-100.0f, -200.0f, -1.0f, 0.0f, 0.0f, -1));
    
    // Calls should still be recorded
    EXPECT_EQ(mockRenderer->rectCalls.size(), 2);
    EXPECT_EQ(mockRenderer->spriteCalls.size(), 1);
}

// Test typical usage pattern
TEST_F(MockRendererTest, TypicalUsagePattern) {
    // Simulate a typical frame
    mockRenderer->beginFrame();
    mockRenderer->clear();
    
    // Render some content
    mockRenderer->renderRect(100.0f, 100.0f, 50.0f, 50.0f, 0.0f, 1.0f, 0.0f, 1.0f); // Green rect
    mockRenderer->renderSprite(200.0f, 150.0f, 0.0f, 32.0f, 32.0f, 5);              // Sprite
    mockRenderer->renderRect(300.0f, 200.0f, 25.0f, 25.0f, 1.0f, 0.0f, 0.0f, 0.8f); // Red rect
    
    mockRenderer->endFrame();
    
    // Verify the frame was recorded correctly
    EXPECT_TRUE(mockRenderer->wasMethodCalled("beginFrame"));
    EXPECT_TRUE(mockRenderer->wasMethodCalled("clear"));
    EXPECT_TRUE(mockRenderer->wasMethodCalled("endFrame"));
    
    EXPECT_EQ(mockRenderer->rectCalls.size(), 2);
    EXPECT_EQ(mockRenderer->spriteCalls.size(), 1);
    
    // Check rendering order is preserved
    EXPECT_FLOAT_EQ(mockRenderer->rectCalls[0].green, 1.0f); // Green rect first
    EXPECT_EQ(mockRenderer->spriteCalls[0].textureId, 5);    // Sprite second
    EXPECT_FLOAT_EQ(mockRenderer->rectCalls[1].red, 1.0f);   // Red rect third
}

// Test that MockRenderer can be used polymorphically
TEST_F(MockRendererTest, PolymorphicUsage) {
    ECS::IRenderer* renderer = mockRenderer.get();
    
    // Should work through base interface
    EXPECT_NO_THROW(renderer->beginFrame());
    EXPECT_NO_THROW(renderer->renderRect(0.0f, 0.0f, 10.0f, 10.0f, 1.0f, 1.0f, 1.0f, 1.0f));
    EXPECT_NO_THROW(renderer->endFrame());
    
    // Verify calls were recorded
    EXPECT_TRUE(mockRenderer->wasMethodCalled("beginFrame"));
    EXPECT_EQ(mockRenderer->rectCalls.size(), 1);
}

// Test call order verification methods
TEST_F(MockRendererTest, CallOrderVerification) {
    // Test correct sequence verification
    mockRenderer->beginFrame();
    mockRenderer->clear();
    mockRenderer->endFrame();
    
    std::vector<std::string> correctSequence = {"beginFrame", "clear", "endFrame"};
    EXPECT_TRUE(mockRenderer->verifyCallSequence(correctSequence));
    
    // Test incorrect sequence detection
    std::vector<std::string> incorrectSequence = {"clear", "beginFrame", "endFrame"};
    EXPECT_FALSE(mockRenderer->verifyCallSequence(incorrectSequence));
    
    // Test different length sequences
    std::vector<std::string> shortSequence = {"beginFrame", "clear"};
    EXPECT_FALSE(mockRenderer->verifyCallSequence(shortSequence));
    
    std::vector<std::string> longSequence = {"beginFrame", "clear", "endFrame", "clear"};
    EXPECT_FALSE(mockRenderer->verifyCallSequence(longSequence));
    
    // Test getCallSequence returns the actual sequence
    std::vector<std::string> actualSequence = mockRenderer->getCallSequence();
    EXPECT_EQ(actualSequence, correctSequence);
}