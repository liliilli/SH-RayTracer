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

#include <array>
#include <nlohmann/json_fwd.hpp>
#include <Math/Type/Shape/DBox.h>
#include <Expr/XEnumConversion.h>
#include <Interface/IHitable.hpp>
#include <XCommon.hpp>
#include <Helper/XJsonCallback.hpp>
#include <Helper/EJsonExistance.hpp>

namespace ray
{

/// @class FBox
/// @brief Box type.
class FBox final : public IHitable, public ::dy::math::DBox<TReal>
{
public:
  /// @brief Constructor type of FBox.
  struct PCtor final
  {
    enum EType { _1, _2, _3 };

    struct PType1 final 
    { 
      DVec3 mOrigin; std::array<TReal, 6> mLength; DVec3 mAngle; 
      PType1 Overwrite(const PType1& pctor, const json::FExistanceList& list) const;
    };
    struct PType2 final 
    { 
      DVec3 mOrigin, mLength, mAngle; 
      PType2 Overwrite(const PType2& pctor, const json::FExistanceList& list) const;
    };
    struct PType3 final 
    { 
      DVec3 mOrigin; TReal mLength; DVec3 mAngle; 
      PType3 Overwrite(const PType3& pctor, const json::FExistanceList& list) const;
    };

    EType mCtorType = EType::_1;
    std::variant<PType1, PType2, PType3> mCtor;

    /// @brief Overwrite with given pctor instance and create new PCtor.
    PCtor Overwrite(const PCtor& pctor, const json::FExistanceList& list) const;
  };

  FBox(const PCtor& ctor, const IMaterial* mat);
  virtual ~FBox() = default;

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

  /// @brief Get PCtor instance from instance, with given type value.
  /// @param type Type value.
  FBox::PCtor GetPCtor(FBox::PCtor::EType type) const noexcept;

private:
  DQuat mRotQuat;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FBox::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FBox::PCtor& oCtor);
void from_json(const nlohmann::json& json, FBox::PCtor::PType1& oCtor);
void from_json(const nlohmann::json& json, FBox::PCtor::PType2& oCtor);
void from_json(const nlohmann::json& json, FBox::PCtor::PType3& oCtor);

/// @brief FBox specialized function.
template <> json::FExistanceList JsonCheckExistances<FBox::PCtor>(const nlohmann::json& json);
template <> json::FExistanceList JsonCheckExistances<FBox::PCtor::PType1>(const nlohmann::json& json);
template <> json::FExistanceList JsonCheckExistances<FBox::PCtor::PType2>(const nlohmann::json& json);
template <> json::FExistanceList JsonCheckExistances<FBox::PCtor::PType3>(const nlohmann::json& json);

} /// ::ray namespace
