#include <catch2/catch_test_macros.hpp>
#include "lightmaze/map/map_entity.hh"
#include "model/game_state.hh"
#include <yaml-cpp/yaml.h>

using namespace lightmaze;

TEST_CASE("MapEntity platform serialization", "[MapEntity][serialization]") {
  model::GameState game_state;

  SECTION("Platform parameters serialization") {
    // Initialize with platform parameters using proper GameState method
    MapEntity::PlatformParams platform_params;
    platform_params.top_center_position = Eigen::Vector2f{1.5f, 2.0f};
    platform_params.size = Eigen::Vector2f{3.0f, 0.5f};

    MapEntity::Params params{platform_params};
    auto map_entity_result = game_state.add_entity_and_init<MapEntity>(params);

    REQUIRE(map_entity_result.isOk());
    auto* map_entity = map_entity_result.unwrap();

    // Serialize to YAML
    YAML::Node serialized = map_entity->serialize();

    // Verify serialized content - use CHECK to see all failures
    CHECK(serialized["type"].as<std::string>() == "platform");
    CHECK(serialized["top_center_position"]["x"].as<float>() == 1.5f);
    CHECK(serialized["top_center_position"]["y"].as<float>() == 2.0f);
    CHECK(serialized["size"]["x"].as<float>() == 3.0f);
    CHECK(serialized["size"]["y"].as<float>() == 0.5f);
  }
}

TEST_CASE("MapEntity platform deserialization", "[MapEntity][deserialization]") {
  model::GameState game_state;

  SECTION("Platform parameters deserialization") {
    // Create YAML node manually
    YAML::Node yaml_node;
    yaml_node["type"] = "platform";
    yaml_node["top_center_position"]["x"] = 2.5f;
    yaml_node["top_center_position"]["y"] = 3.0f;
    yaml_node["size"]["x"] = 4.0f;
    yaml_node["size"]["y"] = 0.8f;

    // Initialize from YAML using SerializedMapEntityParams and proper GameState method
    MapEntity::SerializedMapEntityParams serialized_params{yaml_node};
    MapEntity::Params params{serialized_params};

    auto map_entity_result = game_state.add_entity_and_init<MapEntity>(params);
    REQUIRE(map_entity_result.isOk());
    auto* map_entity = map_entity_result.unwrap();

    // Verify the entity was properly initialized - use CHECK to see all failures
    Eigen::Vector2f position = map_entity->get_top_center_position();
    CHECK(position.x() == 2.5f);
    CHECK(position.y() == 3.0f);

    Eigen::Vector2f size = map_entity->get_size();
    CHECK(size.x() == 4.0f);
    CHECK(size.y() == 0.8f);
  }

  SECTION("Invalid YAML node handling") {
    // Test missing type field
    YAML::Node invalid_node;
    invalid_node["size"]["x"] = 1.0f;

    MapEntity::SerializedMapEntityParams serialized_params{invalid_node};
    MapEntity::Params params{serialized_params};

    auto map_entity_result = game_state.add_entity_and_init<MapEntity>(params);
    REQUIRE(map_entity_result.isErr());
    CHECK(map_entity_result.unwrapErr().find("missing 'type' field") != std::string::npos);
  }

  SECTION("Unknown entity type handling") {
    YAML::Node unknown_type_node;
    unknown_type_node["type"] = "unknown_entity_type";

    MapEntity::SerializedMapEntityParams serialized_params{unknown_type_node};
    MapEntity::Params params{serialized_params};

    auto map_entity_result = game_state.add_entity_and_init<MapEntity>(params);
    REQUIRE(map_entity_result.isErr());
    CHECK(map_entity_result.unwrapErr().find("Unknown entity type") != std::string::npos);
  }
}

TEST_CASE("MapEntity round-trip serialization", "[MapEntity][round-trip]") {
  model::GameState game_state;

  SECTION("Platform round-trip test") {
    // Create original entity using proper GameState method
    MapEntity::PlatformParams original_params;
    original_params.top_center_position = Eigen::Vector2f{-1.0f, 5.0f};
    original_params.size = Eigen::Vector2f{2.5f, 0.3f};

    MapEntity::Params params{original_params};
    auto original_entity_result = game_state.add_entity_and_init<MapEntity>(params);
    REQUIRE(original_entity_result.isOk());
    auto* original_entity = original_entity_result.unwrap();

    // Serialize original
    YAML::Node serialized = original_entity->serialize();

    // Create new entity from serialized data using proper GameState method
    MapEntity::SerializedMapEntityParams serialized_params{serialized};
    MapEntity::Params recreated_params{serialized_params};

    auto recreated_entity_result = game_state.add_entity_and_init<MapEntity>(recreated_params);
    REQUIRE(recreated_entity_result.isOk());
    auto* recreated_entity = recreated_entity_result.unwrap();

    // Verify both entities have identical properties - use CHECK to see all failures
    CHECK(original_entity->get_top_center_position().isApprox(
        recreated_entity->get_top_center_position()));
    CHECK(original_entity->get_size().isApprox(
        recreated_entity->get_size()));

    // Verify serialized output is identical - use CHECK to see all failures
    YAML::Node original_serialized = original_entity->serialize();
    YAML::Node recreated_serialized = recreated_entity->serialize();

    CHECK(original_serialized["type"].as<std::string>() ==
          recreated_serialized["type"].as<std::string>());
    CHECK(original_serialized["top_center_position"]["x"].as<float>() ==
          recreated_serialized["top_center_position"]["x"].as<float>());
    CHECK(original_serialized["top_center_position"]["y"].as<float>() ==
          recreated_serialized["top_center_position"]["y"].as<float>());
    CHECK(original_serialized["size"]["x"].as<float>() ==
          recreated_serialized["size"]["x"].as<float>());
    CHECK(original_serialized["size"]["y"].as<float>() ==
          recreated_serialized["size"]["y"].as<float>());
  }
}