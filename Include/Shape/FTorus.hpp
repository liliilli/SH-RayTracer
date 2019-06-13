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
#include <IHitable.hpp>
#include <XCommon.hpp>

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
  };

  FTorus(const FTorus::PCtor& arg, IMaterial* mat);
  virtual ~FTorus() = default;

  DQuat GetQuaternion() const noexcept { return this->mRotQuat; }

private:
  DQuat mRotQuat;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FTorus::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FTorus::PCtor& oCtor);

} /// ::ray namespace