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
#include <tinyobj/tiny_obj_loader.h>
#include <XCommon.hpp>
#include <Id/DModelBufferId.hpp>

namespace ray
{

/// @class DModelBuffer
/// @brief Buffer class of each model. Buffer has serial vertices, normal and texture uv(w)s.
/// Also, searching time can be optimized by using KD-Tree.
class DModelBuffer final
{
public:
  DModelBuffer(const tinyobj::attrib_t& attrib);
  DModelBuffer(const DModelBufferId& id, const tinyobj::attrib_t& attrib);

  /// @brief
  TIndex GetCountOfVertices() const noexcept;

  /// @brief
  TIndex GetCountOfNormals() const noexcept;

  /// @brief
  TIndex GetCountOfUv0s() const noexcept;

  /// @brief
  bool HasNormals() const noexcept;

  /// @brief 
  DModelBufferId GetId() const noexcept;

private:
  /// @brief Create normals from given geomtery `mVertices`, with triangle.
  void CreateNormals();

  using TPointVertices = std::vector<DVec3>;
  using TNormals = std::vector<DVec3>;
  using TUVs = std::vector<DVec2>;

  DModelBufferId  mId;
  TPointVertices  mVertices;
  TNormals        mNormals;
  TUVs            mUV0s;
};

} /// ::ray namespace