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

#include <nlohmann/json_fwd.hpp>
#include <Math/Type/Shape/DTorus.h>
#include <Interface/IHitable.hpp>
#include <XCommon.hpp>
#include <Helper/XJsonCallback.hpp>
#include <Helper/EJsonExistance.hpp>

namespace ray
{
   
/// @class FTorus
/// @brief Sphere type.
class FTorus final : public IHitable, public ::dy::math::DTorus<TReal>
{
public:
  /// @brief Constructor type of FTorus.
  struct PCtor final
  {
    DVec3 mOrigin;
    TReal mDistance;
    TReal mRadius;
    DVec3 mAngle;

    /// @brief Overwrite with given pctor instance and create new PCtor.
    PCtor Overwrite(const PCtor& pctor, const json::FExistanceList& list) const;
  };

  FTorus(const FTorus::PCtor& arg, IMaterial* mat);
  virtual ~FTorus() = default;

  const DQuat& GetQuaternion() const noexcept { return this->mRotQuat; }

  /// @brief Try to getting ray intersected t value list.
  /// If this shape is not intersected with given ray, just return null value.
  /// @param ray Ray of worls-space.
  /// @return When ray intersected to ray, returns TReal list.
  std::optional<TValueResults> GetRayIntersectedTValues(const DRay& ray) const override final;

  /// @brief Diffuse scattering function.
  /// @param ray World space ray to intersect.
  /// @param t Forwarding value t for moving ray origin into surface approximately.
  /// @param normal Microfacet surface's given world-space normal vector.
  /// @return If scattered, return scattered result of diffused reflection.
  std::optional<PScatterResult> TryScatter(const DRay& ray, TReal t, const DVec3& normal) const override final;

  /// @brief Get PCtor instance from FTorus instance.
  FTorus::PCtor GetPCtor() const noexcept;

private:
  DQuat mRotQuat;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FTorus::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FTorus::PCtor& oCtor);

/// @brief FTorus specialized function.
template <> json::FExistanceList JsonCheckExistances<FTorus::PCtor>(const nlohmann::json& json);

} /// ::ray namespace