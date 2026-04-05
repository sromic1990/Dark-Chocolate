// tests/test_transform.cpp
#include <catch2/catch_test_macros.hpp>
#include "scene/Components.h"
#include <glm/glm.hpp>
#include "catch2/catch_approx.hpp"

// Helper: are two mat4 values approximately equal?
// Floating-point matrix operations accumulate small errors.
// We allow 1e-5 tolerance rather than requiring bit-exact equality.
static bool mat4Approx(const glm::mat4& matrix1, const glm::mat4& matrix2, float eps = 1e-5f)
{
	for (int col = 0; col < 4; ++col)
	{
		for (int row = 0; row < 4; ++row)
		{
			if (std::abs(matrix1[col][row] - matrix2[col][row]) > eps) return false;
		}
	}
	return true;
}

// Default position (0,0,0), rotation (0,0,0), scale (1,1,1) must produce the identity matrix.
// Transforming a point will leave it unchanged.
TEST_CASE("Default transform is identity matrix", "[transform]")
{
	DC::TransformComponent tc;
	REQUIRE(mat4Approx(tc.GetTransform(), glm::mat4(1.0f)));
}

// Translation appears in the last column of the matrix (col index 3).
// mat[col][row] is column-major. Translation: mat[3][0..2]
// So, moving an object by (3,5,7) should not put these values in column 3 of the matrix.
// When applied to a homogenous point (x, y, z, 1), the matrix multiplication adds (3,5,7) to (x, y, z)
TEST_CASE("Translation appears in last column of matrix", "[transform]")
{
	DC::TransformComponent tc;
	tc.Position = { 3.0f, 5.0f, 7.0f };
	auto mat = tc.GetTransform();
	// mat[3] is column 3. mat[3][0] is row 0 of column 3 = world X.
	REQUIRE(mat[3][0] == Catch::Approx(3.0f));
	REQUIRE(mat[3][1] == Catch::Approx(5.0f));
	REQUIRE(mat[3][2] == Catch::Approx(7.0f));
	REQUIRE(mat[3][3] == Catch::Approx(1.0f)); // homogenous w component
}

// Scale appears on the matrix diagonal (columns 0, 1, 2 row matching)
// Uniform scale of 2: the x, y, z basis vectors each double in length.
TEST_CASE("Uniform scale appears on matrix diagonal", "[transform]")
{
	DC::TransformComponent tc;
	tc.Scale = { 2.0f, 2.0f, 2.0f };
	auto mat = tc.GetTransform();
	REQUIRE(mat[0][0] == Catch::Approx(2.0f));
	REQUIRE(mat[1][1] == Catch::Approx(2.0f));
	REQUIRE(mat[2][2] == Catch::Approx(2.0f));
}

// 90 degree Y rotation: X basis should point in -Z direction.
// In right-handed coordinates, rotating 90 degrees around Y = X -> -Z, Z-> X
// The first column of the rotation matrix (the X basis after rotation) should be approx. (0, 0, -1)
TEST_CASE("90-degree Y rotation rotates X basis to -Z", "[transform]")
{
	DC::TransformComponent tc;
	tc.Rotation = { 0.0f, 90.0f, 0.0f };
	auto mat = tc.GetTransform();
	REQUIRE(mat[0][0] == Catch::Approx(0.0f).margin(1e-5f));
	REQUIRE(mat[0][1] == Catch::Approx(0.0f).margin(1e-5f));
	REQUIRE(mat[0][2] == Catch::Approx(-1.0f).margin(1e-5f));
}

// TRS order: Translations * Rotation * Scale.
// A translated-and-scaled object: scale first then translate.
// Translation in column 3 must be preserved regardless of scale.
TEST_CASE("TRS order is correct, scale then translate", "[transform]")
{
	DC::TransformComponent tc;
	tc.Position = {10.0f, 0.0f, 0.0f};
	tc.Scale = {2.0f, 2.0f, 2.0f};
	auto mat = tc.GetTransform();
	// Translation (column 3) should be 10, not 20.
	// If order were wrong (SRT instead of TRS), scale would multiply the translation, and we would get 20.
	REQUIRE(mat[3][0] == Catch::Approx(10.0f));
	REQUIRE(mat[3][1] == Catch::Approx(0.0f));
}