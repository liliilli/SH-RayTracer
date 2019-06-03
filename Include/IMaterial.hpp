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

#include <XCommon.hpp>
#include <optional>
#include <tuple>

namespace ray
{

/// @class IMaterial
/// @brief Material interface.
class IMaterial
{
public:
  virtual ~IMaterial() = 0;

  /// @brief
  virtual std::optional<std::tuple<DVec3, DVec3, bool>> 
  Scatter(const DRay& intersectedRay, const DVec3& normal) = 0;
};

inline IMaterial::~IMaterial() = default;

} /// ::ray namespace