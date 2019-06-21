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
#include <Id/DMeshId.hpp>

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

  /// @brief Get count of vertices.
  TIndex GetCountOfVertices() const noexcept;
  /// @brief Get count of normals.
  TIndex GetCountOfNormals() const noexcept;
  /// @brief Get count of UV0 texture coordinate.
  TIndex GetCountOfUv0s() const noexcept;
  
  /// @brief Check buffer has vertices.
  bool HasVertices() const noexcept;
  /// @brief Check buffer has normals.
  bool HasNormals() const noexcept;
  /// @brief Check buffer has uv.
  bool HasUv0s() const noexcept;

  /// @brief Get model buffer id.
  DModelBufferId GetId() const noexcept;

  /// @brief Create normals from given geometry `mVertices`, with triangle.
  /// @param meshIds Valid mesh id list.
  /// @return If successful, return true.
  bool CreateNormalsWith(const std::vector<DMeshId>& meshIds);

private:
  using TPointVertices = std::vector<DVec3>;
  using TNormals = std::vector<DVec3>;
  using TUVs = std::vector<DVec2>;

  DModelBufferId  mId;
  TPointVertices  mVertices;
  TNormals        mNormals;
  TUVs            mUV0s;
};

} /// ::ray namespace