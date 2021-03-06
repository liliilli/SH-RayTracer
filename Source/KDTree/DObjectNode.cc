///
/// MIT License
/// Copyright (c) 2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <KDTree/DObjectNode.hpp>
#include <Math/Utility/XShapeMath.h>
#include <Resource/DModelFace.hpp>

namespace ray
{

void DObjectNode::BuildTree(const std::vector<const IHitable*>& pObjects)
{
  this->mpObjects = pObjects;
  if (this->mpObjects.empty() == true) { return; }
  if (this->mpObjects.size() == 1)
  {
    this->mOverallBoundingBox = *this->mpObjects.front()->GetAABB();
  }

  // First, make overall bounding box from given pObjects (all items are valid)
  {
    using ::dy::math::GetUnionOf;

    DAABB aabb = *this->mpObjects.front()->GetAABB();
    for (TIndex i = 1, size = this->mpObjects.size(); i < size; ++i)
    {
      aabb = GetUnionOf(aabb, *this->mpObjects[i]->GetAABB());
    }
    this->mOverallBoundingBox = aabb;
  }

  // Second, Check what axis of given DAABB is longest, so set axis as basis axis of child.
  // LeftNode's pObjects list is list for triangle that center point's given axis is less than overall AABB center.
  // RightNode's pObjects list is for equal or bigger than.

  // Get axis value from overall AABB.
  using ::dy::math::EAxis;
  EAxis axis = EAxis::X;
  {
    const auto lenVec = this->mOverallBoundingBox.GetLength();
    if (lenVec[0] < lenVec[1])
    {
      if (lenVec[1] < lenVec[2])  { axis = EAxis::Z; }
      else                        { axis = EAxis::Y; }
    }
    else { axis = EAxis::X; }
  }
  this->mAxis = axis;
     
  // Get Overall AABB's center point as a criteria.
  const DVec3 center = (this->mOverallBoundingBox.GetMin() + this->mOverallBoundingBox.GetMax()) / 2;
 
  // Group left child and right child following by aabb center point and axis.
  std::vector<const IHitable*> leftChildObjs;
  std::vector<const IHitable*> rightChildObjs;
  const auto basisValue = center[static_cast<std::underlying_type_t<EAxis>>(axis)];
  for (const auto& pObject : this->mpObjects)
  {
    // Get center point of given object's aabb.
    const auto& aabb = pObject->GetAABB();
    assert(aabb != nullptr);
    const DVec3 targetCenter = (aabb->GetMin() + aabb->GetMax()) / 2;

    // Compare with `basisValue` and insert pointer into child object list.
    const auto targetValue = targetCenter[static_cast<std::underlying_type_t<EAxis>>(axis)];
    if (targetValue < basisValue) { leftChildObjs.emplace_back(pObject); }
    else                          { rightChildObjs.emplace_back(pObject); }
  }

  // Third, check recursive building condition afterward pre-process.
  if (leftChildObjs.empty() == true && rightChildObjs.size() > 0) { leftChildObjs = rightChildObjs; }
  if (rightChildObjs.empty() == true && leftChildObjs.size() > 0) { rightChildObjs = leftChildObjs; }

  // Get match counts.
  TIndex matches = 0;
  for (const auto& pLeft : leftChildObjs)
  {
    for (const auto& pRight : rightChildObjs)
    {
      if (pLeft == pRight) { matches++; }
    }
  }

  // Check condition. If satisfied, try to call child building sequence recursively.
  const TReal leftCond  = (TReal)matches / leftChildObjs.size();
  const TReal rightCond = (TReal)matches / rightChildObjs.size();
  if (leftCond < 0.5f && rightCond < 0.5f)
  {
    this->mLeftNode = std::make_unique<DObjectNode>();
    this->mLeftNode->BuildTree(leftChildObjs);

    this->mRightNode = std::make_unique<DObjectNode>();
    this->mRightNode->BuildTree(rightChildObjs);
  }
}

IHitable::TValueResults DObjectNode::GetIntersectedTriangleTValue(const DRay& ray) const
{
  using ::dy::math::IsRayIntersected;
  // Check It is leaf node or not. If leaf node, we can use moller algorithm to find T.
  // If not, we just forward `localRay` into children.
  if (this->mLeftNode != nullptr || this->mRightNode != nullptr)
  {
    // Check AABB optionally. If not passed, regard ray as not intersecting potential overall AABB region.
    if (IsRayIntersected(ray, this->mOverallBoundingBox) == false) { return {}; }

    assert(this->mLeftNode != nullptr);
    assert(this->mRightNode != nullptr);
    const auto leftTs   = this->mLeftNode->GetIntersectedTriangleTValue(ray);
    const auto rightTs  = this->mRightNode->GetIntersectedTriangleTValue(ray);

    IHitable::TValueResults tResult;
    tResult.insert(tResult.end(), EXPR_BIND_BEGIN_END(leftTs));
    tResult.insert(tResult.end(), EXPR_BIND_BEGIN_END(rightTs));
    return tResult;
  }

  // If node is left, use moller algorithm to triangles.
  // Use Möller–Trumbore intersection algorithm
  // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
  IHitable::TValueResults tResult;
  for (const auto& pObject : this->mpObjects)
  {
    const auto optTValues = pObject->GetRayIntersectedTValues(ray);

    if (optTValues.has_value() == false) { continue; }
    tResult.insert(tResult.end(), EXPR_BIND_BEGIN_END((*optTValues)));
  }
 
  return tResult;
}

} /// ::ray namespace
