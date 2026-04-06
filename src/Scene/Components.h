// src/Scene/Components.h
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <string>

namespace DC
{
	// TagComponent: every entity has a name. Pure data struct.
	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		explicit TagComponent(const std::string& tag) : Tag(tag) {}
	};

	// TransformComponent: position, rotation (euler degrees), scale.
	// Degrees for intuitive editor display. Radians used internally.
	struct TransformComponent
	{
		glm::vec3 Position		= { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation		= { 0.0f, 0.0f, 0.0f }; // degrees
		glm::vec3 Scale			= { 1.0f, 1.0f, 1.0f };

		TransformComponent()	= default;

		// TRS: Translation * Rotation * Scale
		// Order matters, scale first, rotate second, translate third.
		// SRT would rotate around scaled axes, counterintuitive.
		// In GLM column-major, rightmost matrix is applied first.
		// So T*R*S means "apply S, then R, then T" to a column vector.
		glm::mat4 GetTransform() const
		{
			glm::mat4 T = glm::translate(glm::mat4(1.0f), Position);
			glm::mat4 R = glm::eulerAngleXYZ(
				glm::radians(Rotation.x),
				glm::radians(Rotation.y),
				glm::radians(Rotation.z));
			glm::mat4 S = glm::scale(glm::mat4(1.0f), Scale);

			return T * R * S;
		}
	};

	// MeshRendererComponent: placeholder for now.
	// Present now so the editor properties panel can check for it, and tests can verify add/remove behavior.
	struct MeshRendererComponent
	{
		std::string MeshPath;
		std::string MaterialPath;
		bool Visible = true;
	};
}
