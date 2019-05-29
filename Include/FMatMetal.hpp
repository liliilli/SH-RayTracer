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

#include <IMaterial.hpp>
#include <XCommon.hpp>

namespace ray
{

class FMatMetal final : public IMaterial
{
public:
  FMatMetal(const DVec3& color)
    : mColor { color }
  { };
  virtual ~FMatMetal() = default;

  virtual std::optional<std::tuple<DVec3, DVec3, bool>> 
  Scatter(const DRay& intersectedRay, const DVec3& normal) override final;

private:
  DVec3 mColor;
};

} /// ::ray namespace