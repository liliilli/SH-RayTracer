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

#include <optional>
#include <tuple>
#include <string>
#include <Math/Type/Micellanous/DUuid.h>

#include <Id/DMatId.hpp>
#include <XCommon.hpp>
#include <Object/XFunctionResults.hpp>

namespace ray
{

/// @class IMaterial
/// @brief Material interface.
class IMaterial
{
public:
  IMaterial(const DMatId& id);
  virtual ~IMaterial() = 0;

  /// @brief Diffuse scattering function.
  /// @param intersectedRay The ray that can be on surface or intersected with surface directly.
  /// @param normal The normal vector of surface.
  /// @return If intersected and could get diffuse scattered reflection (with dialectric), return result.
  virtual std::optional<PScatterResult> Scatter(const DRay& intersectedRay, const DVec3& normal) const = 0;

  /// @brief Get ID instance.
  const DMatId& GetId() const noexcept;

private:
  DMatId mId;
};

inline IMaterial::~IMaterial() = default;

} /// ::ray namespace