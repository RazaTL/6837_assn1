#ifndef CURVE_NODE_H_
#define CURVE_NODE_H_

#include <string>
#include <vector>

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/components/MaterialComponent.hpp"

namespace GLOO {

enum class SplineBasis { Bezier, BSpline };

struct CurvePoint {
  glm::vec3 P;
  glm::vec3 T;
};

class CurveNode : public SceneNode {
 public:
  CurveNode(std::vector<glm::vec3>& control_points, SplineBasis basis, bool keyboard_on);
  void Update(double delta_time) override;

 private:
  void ToggleSplineBasis();
  void ConvertGeometry();
  CurvePoint EvalCurve(float t);
  void InitCurve();
  void PlotCurve();
  void PlotControlPoints();
  void PlotTangentLine();

  SplineBasis spline_basis_;

  bool keyboard_on_;

  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<VertexObject> curve_polyline_;
  std::shared_ptr<VertexObject> tangent_line_;

  std::shared_ptr<ShaderProgram> shader_;
  std::shared_ptr<ShaderProgram> polyline_shader_;

  std::vector<Transform*> cp_trans;

  std::vector<glm::vec3> bruh;

  std::shared_ptr<Material> material;

  glm::vec3 ctrl_p_rgb;

  glm::vec3 ctrl_p_rgb_2 = {0, 255, 0};

  std::vector<glm::vec3> ctrl_p;

  const glm::mat4 BBezier {
    1, 0, 0, 0,
    -3, 3, 0, 0,
    3, -6, 3, 0,
    -1, 3, -3, 1,
  };

  const glm::mat4 BBSpline {
    1.0/6.0, 4.0/6.0, 1.0/6.0, 0.0/6.0,
    -3.0/6.0, 0.0/6.0, 3.0/6.0, 0.0/6.0,
    3.0/6.0, -6.0/6.0, 3.0/6.0, 0.0/6.0,
    -1.0/6.0, 3.0/6.0, -3.0/6.0, 1.0/6.0,
  };
  
  const int N_SUBDIV_ = 50;
};
}  // namespace GLOO

#endif
