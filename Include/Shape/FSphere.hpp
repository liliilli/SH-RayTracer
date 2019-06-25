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
#include <Math/Type/Shape/DSphere.h>
#include <IHitable.hpp>
#include <XCommon.hpp>
#include <Helper/XJsonCallback.hpp>
#include <Helper/EJsonExistance.hpp>

namespace ray
{

/// @class FSphere
/// @brief Sphere type.
class FSphere final : public IHitable, public ::dy::math::DSphere<TReal>
{
public:
  /// @brief Constructor type of FSphere.
  struct PCtor final
  {
    DVec3 mOrigin;
    TReal mRadius;

    /// @brief Overwrite with given pctor instance and create new PCtor.
    PCtor Overwrite(const PCtor& pctor, const json::FExistanceList& list) const;
  };

  FSphere(const DVec3& origin, TReal radius, IMaterial* mat);
  FSphere(const FSphere::PCtor& arg, IMaterial* mat);
  virtual ~FSphere() = default;

  /// @brief Try to getting ray intersected t value list.
  /// If this shape is not intersected with given ray, just return null value.
  /// @param ray Ray of worls-space.
  /// @return When ray intersected to ray, returns TReal list.
  std::optional<TValueResults> GetRayIntersectedTValues(const DRay& ray) const override final;

  std::optional<PScatterResult> TryScatter(const DRay& ray, TReal t) const override final;
  
  /// @brief Get PCtor instance from FSphere instance.
  FSphere::PCtor GetPCtor() const noexcept;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FSphere::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FSphere::PCtor& oCtor);

/// @brief FSphere specialized function.
template <> json::FExistanceList JsonCheckExistances<FSphere::PCtor>(const nlohmann::json& json);

} /// ::ray namespace
