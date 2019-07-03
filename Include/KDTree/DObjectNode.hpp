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
#include <Interface/IHitable.hpp>

namespace ray
{

/// @class DObjectNode
/// @brief Object KDTree node for optimization
class DObjectNode final
{
public:
  /// @brief Build KDTree with given pointer list of triangle from this node.
  /// @param pObjects All valid hitable object pointer list.
  void BuildTree(const std::vector<const IHitable*>& pObjects);

#if 0
  /// @brief Get T with index if given ray that is in mesh's local space can be intersected arbitary triangle node.
  /// @param localRay The ray in local mesh space.
  /// @return If intersected, return T and three index of mesh.
  std::vector<PTriangleResult> GetIntersectedTriangleTValue(const DRay& localRay) const;
#endif

private:
  DAABB mOverallBoundingBox;
  std::unique_ptr<DObjectNode> mLeftNode;
  std::unique_ptr<DObjectNode> mRightNode;
  std::vector<const IHitable*> mpObjects;

  DVec3 mAABBCenter = DVec3{};
  ::dy::math::EAxis mAxis; 
};

} /// ::ray namespace
