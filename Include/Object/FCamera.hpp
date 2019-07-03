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

#include <vector>
#include <XCommon.hpp>
#include <nlohmann/json_fwd.hpp>

#include <Math/Type/Shape/DRay.h>
#include <Math/Type/Micellanous/DClamp.h>
#include <Interface/IObject.hpp>
#include <Helper/EJsonExistance.hpp>
#include <Helper/XJsonCallback.hpp>

namespace ray
{

/// @class FCamera
/// @brief Camera class that manage image size, eye / forward and screen size to focus.
class FCamera final : public IObject
{
public:
  /// @brief Constructor instance type of FCamera.
  struct PCtor final
  {
    DVec3   mOrigin;
    DVec3   mForwardTo;
    DUVec2  mImgSize;
    TReal   mScreenRatioXy;
    TU32    mSamples;
    DClamp<0, 1000> mAperture;
    DClamp<0, 1000> mFocusDistance;
    DClamp<0, 1000> mSensorSize;
    bool mDepthOfField = false;

    /// @brief Overwrite with given pctor instance and crate new PCtor.
    /// member variable matching order must be same to `JsonCheckExistances` function matching order.
    PCtor Overwrite(const PCtor& pctor, const json::FExistanceList& list) const;
  };

  FCamera(const FCamera::PCtor& arg);

  /// @brief Get image size.
  const DUVec2& GetImageSize() const noexcept;

  /// @brief Get ray calculated by [x, y] of Image size and eye / forward.
  std::vector<DRay> CreateRay(TIndex x, TIndex y) const noexcept;

  /// @brief Set sample value of pixel. (1, 2, 4)
  void SetSamples(TU32 sample);
  /// @brief Get rendering samples of each pixel.
  TU32 GetSamples() const noexcept;
  /// @brief Get PCtor instance from instance.
  FCamera::PCtor GetPCtor() const noexcept;

  /// @brief Check camera instance is using depth of field feature.
  bool IsUsingDepthOfField() const noexcept;

private:
  /// @brief Get sample offset list of given anti-aliasing sample count.
  /// If `samples` value exceeds 8, offset list of sample 8 will be returned.
  /// @param right Screen space unit right vector per pixel
  /// @param up Screen space unit up vector per pixel
  /// @param samples Anti-aliasing sample count.
  /// @return Subpixel offset vector list.
  std::vector<DVec3> GetSampleOffsetsOf(const DVec3& right, const DVec3& up, TU32 samples) const;

  DVec3 mOrigin;
  DVec3 mForward;
  DVec3 mSide;
  DVec3 mUp;

  DVec3 mLowLeftCorner;
  DVec3 mCellRight, mCellUp;
  DUVec2 mScreenSize;

  TU32 mSamples = 4;
  TReal mAperture = 1.0f;
  TReal mDistance = 1.0f;
  TReal mSensorSize = 1.0f;

  bool mIsUsingDepthOfField = false;
};

/// @brief Template function for automatic parsing from json.
/// This initialize FCamera::PCtor instance, except some elements.
void from_json(const nlohmann::json& json, FCamera::PCtor& oCtor);

template <>
json::FExistanceList JsonCheckExistances<FCamera::PCtor>(const nlohmann::json& json);

} /// ::ray namespace