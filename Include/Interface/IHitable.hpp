#pragma once
///
/// MIT License
/// Copyright (c) 2018-2019 Jongmin Yun
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
#include <Interface/IMaterial.hpp>
#include <Interface/IObject.hpp>
#include <Shape/EShapeType.hpp>
#include <Object/XFunctionResults.hpp>

namespace ray
{

/// @class IHitable
/// @brief Hitable object interface.
class IHitable : public IObject
{
public:
  IHitable(EShapeType type, const IMaterial*& pMaterial);
  virtual ~IHitable() = 0;

  /// @brief Get type.
  EShapeType GetType() const noexcept;
  /// @brief Get pointer instance of material.
  const IMaterial* GetMaterial() const noexcept;
  /// @brief Check hitable object has 3D AABB.
  bool HasAABB() const noexcept;
  /// @brief Get AABB pointer of hitable object.
  const DAABB* GetAABB() const noexcept;

  /// @brief Try to getting ray intersected t value list.
  /// If this shape is not intersected with given ray, just return null value.
  /// @param ray Ray of worls-space.
  /// @return When ray intersected to ray, returns TReal list.
  using TValueResults = std::vector<PTValueResult>;
  virtual std::optional<TValueResults> GetRayIntersectedTValues(const DRay& ray) const = 0;

  /// @brief Diffuse scattering function.
  /// @param ray World space ray to intersect.
  /// @param t Forwarding value t for moving ray origin into surface approximately.
  /// @param normal Microfacet surface's given world-space normal vector.
  /// @return If scattered, return scattered result of diffused reflection.
  virtual std::optional<PScatterResult> TryScatter(const DRay& ray, TReal t, const DVec3& normal) const = 0;

protected:
  EShapeType mMaterialType;
  const IMaterial* mpMaterial = nullptr;

  std::unique_ptr<DAABB> mAABB = nullptr;
};

inline IHitable::~IHitable() = default;

} /// ::ray namespace
