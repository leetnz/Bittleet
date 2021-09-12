#include "catch.hpp"

TEST_CASE("Template Tests", "[temp]" ) 
{ 
  // Setup goes here
  int value = 0;

  SECTION("Simple assertion") {
    REQUIRE(0 == value);
  }
 
  SECTION("Add to value") {
    value += 11;
    REQUIRE(11 == value);
  }

  SECTION("Sections are independent") {
    value += 5;
    REQUIRE(5 == value);
  }

  // Teardown goes here
}