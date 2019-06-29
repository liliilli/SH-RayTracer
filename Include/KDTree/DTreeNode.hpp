#pragma once
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

#include <memory>
#include <XCommon.hpp>
#include <Object/XFunctionResults.hpp>

namespace ray
{

class DModelFace; // Forward declaration

/// @class DTreeNode
/// @brief Model Triangle KDTree node for optimization
class DTreeNode final
{
public:
  /// @brief Build KDTree with given pointer list of triangle from this node.
  /// @param pTriangles All valid triangle pointer list.
  void BuildTree(const std::vector<const DModelFace*>& pTriangles);

  /// @brief Get T with index if given ray that is in mesh's local space can be intersected arbitary triangle node.
  /// @param localRay The ray in local mesh space.
  /// @return If intersected, return T and three index of mesh.
  std::vector<PTriangleResult> GetIntersectedTriangleTValue(const DRay& localRay) const;

private:
  DAABB mOverallBoundingBox;
  std::unique_ptr<DTreeNode>      mLeftNode;
  std::unique_ptr<DTreeNode>      mRightNode;
  std::vector<const DModelFace*>  mTriangles;

  DVec3 mBarycentric = DVec3{};
  ::dy::math::EAxis mAxis; 
};

} /// ::ray namespace
