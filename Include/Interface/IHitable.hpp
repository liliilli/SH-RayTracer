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
#include <EShapeType.hpp>
#include <Interface/IObject.hpp>

namespace ray
{

class IHitable; // Forward declaration

/// @class PTValueResult
/// @brief GetRayIntersectedTValues returning type.
class PTValueResult final
{
public:
  TReal       mT;
  EShapeType  mShapeType;
  const IHitable* mpHitable;
  DVec3       mSurfaceNormal;

  PTValueResult(TReal t, EShapeType type, const IHitable* pHitable, const DVec3& normal)
    : mT { t },
      mShapeType { type },
      mpHitable { pHitable },
      mSurfaceNormal { normal }
  { }
};
// Memory alignment optimization?
static_assert(sizeof(PTValueResult) == 32);

class PScatterResult final
{
public:
  DVec3 mReflectionDir;
  DVec3 mAttenuationCol;
  alignas(8) bool mIsScattered;

  PScatterResult(const DVec3& refDir, const DVec3& attCol, bool isScattered)
    : mReflectionDir { refDir },
      mAttenuationCol { attCol },
      mIsScattered { isScattered }
  { }
};
// Memory alignment optimization?
static_assert(sizeof(PScatterResult) == 32);

/// @class IHitable
/// @brief Hitable object interface.
class IHitable : public IObject
{
public:
  IHitable(EShapeType type, IMaterial*& pMaterial)
    : IObject { EObject::Hitable },
      mMaterialType { type },
      mpMaterial{ pMaterial }
  { }
  virtual ~IHitable() = 0;

  /// @brief Get type.
  EShapeType GetType() const noexcept { return this->mMaterialType; }
  /// @brief Get pointer instance of material.
  IMaterial* GetMaterial() const noexcept { return this->mpMaterial; }
  /// @brief Check hitable object has 3D AABB.
  bool HasAABB() const noexcept { return this->mAABB != nullptr; }
  /// @brief Get AABB pointer of hitable object.
  const DAABB* GetAABB() const noexcept { return this->mAABB.get(); }

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
  IMaterial* mpMaterial = nullptr;

  std::unique_ptr<DAABB> mAABB = nullptr;
};

inline IHitable::~IHitable() = default;

} /// ::ray namespace
