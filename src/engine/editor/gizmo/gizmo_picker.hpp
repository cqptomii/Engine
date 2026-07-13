#ifndef GIZMO_PICKER_HPP
#define GIZMO_PICKER_HPP

#include "engine/editor/gizmo/gizmo_axis.hpp"
#include "engine/editor/picking/manipulation_mode.hpp"
#include "engine/editor/picking/selection_context.hpp"
#include "engine/math/ray.hpp"
#include "engine/rendering/utils/camera_data.hpp"

/**
 * @brief Test a world-space ray against the transform gizmo handles.
 *
 * The gizmo is placed/oriented/scaled exactly like TransformGizmoRenderer.
 * For TRANSLATE / SCALE the handles are the three axis segments; for ROTATE
 * they are the three rings.
 *
 * @return the closest hit axis, or GizmoAxis::NONE when nothing is grabbed.
 */
GizmoAxis pick_gizmo_axis(
    const Ray& ray,
    const SelectionContext& ctx,
    ManipulationMode mode,
    const CameraData& camera);

#endif // GIZMO_PICKER_HPP
