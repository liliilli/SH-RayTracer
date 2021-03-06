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
#include <Math/Type/Micellanous/DClamp.h>

#include <Interface/IMaterial.hpp>
#include <XCommon.hpp>

namespace ray
{

class FMatMetal final : public IMaterial
{
public:
  /// @brief Constructor instance type of FMatMetal.
  struct PCtor final
  {
    DMatId mId;
    DVec3 mColor;
    TReal mRoughness;
  };

  FMatMetal(const PCtor& arg)
    : IMaterial { arg.mId },
      mColor { arg.mColor },
      mRoughness { arg.mRoughness }
  { };
  virtual ~FMatMetal() = default;

  /// @brief Diffuse scattering function.
  /// @param intersectedRay The ray that can be on surface or intersected with surface directly.
  /// @param normal The normal vector of surface.
  /// @return If intersected and could get diffuse scattered reflection (with dialectric), return result.
  std::optional<PScatterResult> Scatter(const DRay& intersectedRay, const DVec3& normal) const override final;

private:
  DVec3 mColor;
  ::dy::math::DClamp<TReal, 0, 1> mRoughness;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FMatMetal::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FMatMetal::PCtor& oCtor);

} /// ::ray namespace