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

#include <KDTree/DTreeNode.hpp>
#include <Math/Utility/XShapeMath.h>
#include <Resource/DModelFace.hpp>

namespace ray
{

void DTreeNode::BuildTree(const std::vector<const DModelFace*>& pTriangles)
{
  this->mTriangles = pTriangles;
  if (this->mTriangles.empty() == true) { return; }
  if (this->mTriangles.size() == 1)
  {
    this->mOverallBoundingBox = pTriangles.front()->mTriangleAABB;
    for (const auto& vertex : pTriangles.front()->mVertex)
    {
      assert(vertex != nullptr);
      this->mBarycentric += *vertex;
    }
    this->mBarycentric /= 3;
    return;
  }

  // First, make overall bounding box from given pTriangles (all items are valid)
  {
    using ::dy::math::GetUnionOf;

    DAABB aabb = this->mTriangles.front()->mTriangleAABB;
    for (TIndex i = 1, size = this->mTriangles.size(); i < size; ++i)
    {
      aabb = GetUnionOf(aabb, this->mTriangles[i]->mTriangleAABB);
    }
    this->mOverallBoundingBox = aabb;
  }

  // Second, Check what axis of given DAABB is longest, so set axis as basis axis of child.
  // LeftNode's pTriangle list is list for triangle that barycentric point's given axis is less than overall barycentric.
  // RightNode's pTriangle list is for equal or bigger than.

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
  
  // Get Overall barycentric point.
  DVec3 barycentric = DVec3{};
  for (const auto& pFace : this->mTriangles)
  {
    assert(pFace != nullptr);
    for (const auto& vertex : pFace->mVertex)
    {
      assert(vertex != nullptr);
      barycentric += *vertex;
    }
  }
  barycentric /= TReal(this->mTriangles.size() * 3); 
  this->mBarycentric = barycentric;
  
  // Group left child and right child following by barycentric and axis.
  std::vector<const DModelFace*> leftChildTriangles;
  std::vector<const DModelFace*> rightChildTriangles;
  const auto basisValue = barycentric[static_cast<std::underlying_type_t<EAxis>>(axis)];
  for (const auto& pFace : this->mTriangles)
  {
    // Get barycentric of pFace vertices.
    DVec3 targetBarycentric = DVec3{};
    for (const auto& vertex : pFace->mVertex)
    {
      assert(vertex != nullptr);
      targetBarycentric += *vertex;
    }
    targetBarycentric /= 3;

    // Compare with `basisValue` and insert pointer into child triangle list.
    const auto targetValue = targetBarycentric[static_cast<std::underlying_type_t<EAxis>>(axis)];
    if (targetValue < basisValue) { leftChildTriangles.emplace_back(pFace); }
    else                          { rightChildTriangles.emplace_back(pFace); }
  }

  // Third, check recursive building condition afterward pre-process.
  if (leftChildTriangles.empty() == true && rightChildTriangles.size() > 0) { leftChildTriangles = rightChildTriangles; }
  if (rightChildTriangles.empty() == true && leftChildTriangles.size() > 0) { rightChildTriangles = leftChildTriangles; }
 
  // Get match counts.
  TIndex matches = 0;
  for (const auto& pLeftFace : leftChildTriangles)
  {
    for (const auto& pRightFace : rightChildTriangles)
    {
      if (pLeftFace == pRightFace) { matches++; }
    }
  }

  // Check condition. If satisfied, try to call child building sequence recursively.
  const TReal leftCond  = (TReal)matches / leftChildTriangles.size();
  const TReal rightCond = (TReal)matches / rightChildTriangles.size();
  if (leftCond < 0.5f && rightCond < 0.5f)
  {
    this->mLeftNode = std::make_unique<DTreeNode>();
    this->mLeftNode->BuildTree(leftChildTriangles);

    this->mRightNode = std::make_unique<DTreeNode>();
    this->mRightNode->BuildTree(rightChildTriangles);
  }
}

} /// ::ray namespace
