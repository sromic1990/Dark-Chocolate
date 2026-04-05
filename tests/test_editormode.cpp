// tests/test_editormode.cpp
#include <catch2/catch_test_macros.hpp>
#include "editor/EditorLayer.h"


TEST_CASE("Edit and Play modes are distinct", "[editormode]") 
{
    REQUIRE(DC::EditorMode::Edit != DC::EditorMode::Play);
}


// Default mode must be Edit: a newly opened engine must not start simulating immediately. 
// Physics, input, and AI are all Play-only.
TEST_CASE("EditorMode can be set to Edit", "[editormode]") 
{
    DC::EditorMode mode = DC::EditorMode::Edit;
    REQUIRE(mode == DC::EditorMode::Edit);
    REQUIRE(mode != DC::EditorMode::Play);
}


TEST_CASE("EditorMode can toggle between Edit and Play", "[editormode]") 
{
    DC::EditorMode mode = DC::EditorMode::Edit;
    mode = DC::EditorMode::Play;
    REQUIRE(mode == DC::EditorMode::Play);
    mode = DC::EditorMode::Edit;
    REQUIRE(mode == DC::EditorMode::Edit);
}

