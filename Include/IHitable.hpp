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

#include <EShapeType.hpp>

namespace ray
{

/// @class IHitable
/// @brief Hitable object interface.
class IHitable
{
public:
  IHitable(EShapeType type)
    : mType { type }
  { }
  virtual ~IHitable() = 0;

  /// @brief Get type.
  EShapeType GetType() const noexcept { return this->mType; }

private:
  EShapeType mType;
};

inline IHitable::~IHitable() = default;

} /// ::ray namespace
