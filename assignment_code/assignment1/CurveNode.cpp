#include "CurveNode.hpp"

#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/InputManager.hpp"
#include <glm/gtx/string_cast.hpp>

namespace GLOO {
CurveNode::CurveNode(std::vector<glm::vec3>& control_points, SplineBasis basis, bool keyboard_on): bruh (control_points) {
  // TODO: this node should represent a single spline curve.
  // Think carefully about what data defines a curve and how you can
  // render it.

  // Initialize the VertexObjects and shaders used to render the control points,
  // the curve, and the tangent line.
  sphere_mesh_ = PrimitiveFactory::CreateSphere(0.015f, 25, 25);
  curve_polyline_ = std::make_shared<VertexObject>();
  tangent_line_ = std::make_shared<VertexObject>();
  shader_ = std::make_shared<PhongShader>();
  polyline_shader_ = std::make_shared<SimpleShader>();

  spline_basis_ = basis;
  keyboard_on_ = keyboard_on;


  InitCurve();
  PlotCurve();
  PlotControlPoints();
}

void CurveNode::Update(double delta_time) {

  // Prevent multiple toggle.
  static bool prev_released = true;

  if (keyboard_on_) {
    if (InputManager::GetInstance().IsKeyPressed('T')) {
      if (prev_released) {
        // TODO: implement toggling spline bases.
        ToggleSplineBasis();
      }
      prev_released = false;
    } else if (InputManager::GetInstance().IsKeyPressed('B')) {
      if (prev_released) {
        // TODO: implement converting control point geometry from Bezier to
        // B-Spline basis.
        ConvertGeometry();
      }
      prev_released = false;
    } else if (InputManager::GetInstance().IsKeyPressed('Z')) {
      if (prev_released) {
        // TODO: implement converting control point geometry from B-Spline to
        // Bezier basis.
      }
      prev_released = false;
    } else {
      prev_released = true;
    }
  }
}

void CurveNode::ToggleSplineBasis() {
  // TODO: implement toggling between Bezier and B-Spline bases.
  if (spline_basis_ == SplineBasis::Bezier){
    spline_basis_ = SplineBasis::BSpline;
    PlotCurve();
  }
  else if (spline_basis_ == SplineBasis::BSpline)
  {
    spline_basis_ = SplineBasis::Bezier;
    PlotCurve();
  }
  
}

void CurveNode::ConvertGeometry() {
  // TODO: implement converting the control points between bases.
}

CurvePoint CurveNode::EvalCurve(float t) {
  // TODO: implement evaluating the spline curve at parameter value t.
  glm::vec4 MonomialBasis = {1, t, t*t, t*t*t};

  glm::vec4 MonomialDeriv = {0, 1, 2*t, 3*t*t};

  glm::mat4x3 Geometry = {bruh[0].x, bruh[0].y, bruh[0].z,
                         bruh[1].x, bruh[1].y, bruh[1].z,
                         bruh[2].x, bruh[2].y, bruh[2].z,
                         bruh[3].x, bruh[3].y, bruh[3].z};

  CurvePoint ret;
  
  if (spline_basis_ == SplineBasis::Bezier){
    ret.P = Geometry * BBezier * MonomialBasis;
    glm::vec3 vec = (Geometry * BBezier * MonomialDeriv);
    ret.T = vec/glm::length(vec);
  }
  if (spline_basis_ == SplineBasis::BSpline){
    ret.P = Geometry * BBSpline * MonomialBasis;
    glm::vec3 vec = (Geometry * BBSpline * MonomialDeriv);
    ret.T = vec/glm::length(vec);
  }

  return ret;
}

void CurveNode::InitCurve() {
  // TODO: create all of the  nodes and components necessary for rendering the
  // curve, its control points, and its tangent line. You will want to use the
  // VertexObjects and shaders that are initialized in the class constructor.
  ctrl_p_rgb = {255, 0, 0};

  if (spline_basis_ == SplineBasis::BSpline){
    ctrl_p_rgb = {0, 255, 0};
  }

  material = std::make_shared<Material>(ctrl_p_rgb,ctrl_p_rgb,ctrl_p_rgb,0);

  for (int i = 0; i < 4; i++)
  {
    auto ctrl_p = make_unique<SceneNode>();
    ctrl_p->CreateComponent<ShadingComponent>(shader_);
    ctrl_p->CreateComponent<RenderingComponent>(sphere_mesh_);

    ctrl_p->CreateComponent<MaterialComponent>(material);

    auto& transform = ctrl_p->GetTransform();
    cp_trans.push_back(&transform);

    // ctrl_p->GetComponentPtr<MaterialComponent>()->SetMaterial(material);

    AddChild(std::move(ctrl_p));
  }

  auto positions = make_unique<PositionArray>();
  positions->push_back(glm::vec3(1.f, 0, -1.f));
  positions->push_back(glm::vec3(5.f, 5.f, 5.f));

  auto indices = make_unique<IndexArray>();
  indices->push_back(0);
  indices->push_back(1);

  curve_polyline_->UpdatePositions(std::move(positions));
  curve_polyline_->UpdateIndices(std::move(indices));

  auto shader = std::make_shared<SimpleShader>();

  auto line_node = make_unique<SceneNode>();
  line_node->CreateComponent<ShadingComponent>(shader);

  auto& rc = line_node->CreateComponent<RenderingComponent>(curve_polyline_);
  rc.SetDrawMode(DrawMode::Lines);

  glm::vec3 color(1.f, 1.f, 1.f);
  auto material = std::make_shared<Material>(color, color, color, 0);
  line_node->CreateComponent<MaterialComponent>(material);  

  AddChild(std::move(line_node));
}

void CurveNode::PlotCurve() {
  // TODO: plot the curve by updating the positions of its VertexObject.
  float increment = 1.0/N_SUBDIV_;
  auto pos = make_unique<PositionArray>();
  auto ix = make_unique<IndexArray>();
  for (int i = 0; i < N_SUBDIV_ - 1; i ++){
    glm::vec3 inp = EvalCurve(float(i)*increment).P;
    pos->push_back(inp);
    if (i == 0 || i == N_SUBDIV_ - 1) {
      ix->push_back(i);}
    else {
      ix->push_back(i);
      ix->push_back(i);
  }
  }

  curve_polyline_->UpdatePositions(std::move(pos));
  curve_polyline_->UpdateIndices(std::move(ix));
}

void CurveNode::PlotControlPoints() {

  for (int i = 0; i < 4; i++) {
    cp_trans[i]->SetPosition(bruh[i]);
  }

}

void CurveNode::PlotTangentLine() {
  // TODO: implement plotting a line tangent to the curve.
  // Below is a sample implementation for rendering a line segment
  // onto the screen. Note that this is just an example. This code
  // currently has nothing to do with the spline.

  auto line = std::make_shared<VertexObject>();

  auto positions = make_unique<PositionArray>();
  positions->push_back(glm::vec3(1.f, 0, -1.f));
  positions->push_back(glm::vec3(5.f, 5.f, 5.f));

  auto indices = make_unique<IndexArray>();
  indices->push_back(0);
  indices->push_back(1);

  line->UpdatePositions(std::move(positions));
  line->UpdateIndices(std::move(indices));

  auto shader = std::make_shared<SimpleShader>();

  auto line_node = make_unique<SceneNode>();
  line_node->CreateComponent<ShadingComponent>(shader);

  auto& rc = line_node->CreateComponent<RenderingComponent>(line);
  rc.SetDrawMode(DrawMode::Lines);

  glm::vec3 color(1.f, 1.f, 1.f);
  auto material = std::make_shared<Material>(color, color, color, 0);
  line_node->CreateComponent<MaterialComponent>(material);

  AddChild(std::move(line_node));
}
}  // namespace GLOO
