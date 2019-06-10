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

#include <Math/Type/Shape/DPlane.h>
#include <IHitable.hpp>
#include <XCommon.hpp>

namespace ray
{

/// @class FPlane
/// @brief Plane type.
class FPlane final : public IHitable, public ::dy::math::DPlane<TReal>
{
public:
  /// @brief Construct DPlane with normalized vector and just d.
  FPlane(const DVec3& normal, TReal d, std::unique_ptr<IMaterial>&& mat);
  /// @brief Construt DPlane with (pos2 - pos1) and (pos3 - pos1) vector.
  FPlane(const DVec3& pos1, const DVec3& pos2, const DVec3& pos3, std::unique_ptr<IMaterial>&& mat);

  virtual ~FPlane() = default;
};

} /// ::ray namespace
