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

#include <Resource/DModelBuffer.hpp>
#include <iostream>
#include <Expr/TZip.h>
#include <Resource/DModelMesh.hpp>
#include <Manager/MModel.hpp>

namespace ray
{

DModelBuffer::DModelBuffer(const DModelBufferId& id, const tinyobj::attrib_t& attrib)
  : mId { id },
    mVertices ( attrib.vertices.size() ),
    mNormals ( attrib.normals.size() ),
    mUV0s ( attrib.texcoords.size() )
{
  static_assert(sizeof(TReal) == sizeof(tinyobj::real_t));
  static_assert(sizeof(DVec3) == sizeof(tinyobj::real_t) * 3);

  // Copy vertices, normal and uv0 into vertices, normals and uv0s list.
  std::memcpy(this->mVertices.data(), attrib.vertices.data(), attrib.vertices.size() * sizeof(TReal));
  std::memcpy(this->mNormals.data(), attrib.normals.data(), attrib.normals.size() * sizeof(TReal));
  std::memcpy(this->mUV0s.data(), attrib.texcoords.data(), attrib.texcoords.size() * sizeof(TReal));
}

bool DModelBuffer::CreateNormalsWith([[maybe_unused]] const std::vector<DMeshId>& meshIds)
{
  // https://computergraphics.stackexchange.com/questions/4031/programmatically-generating-vertex-normals
  // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
  // Not implemented yet.

  // Get pointer of mesh from id.
  std::vector<DModelMesh*> pMeshes (meshIds.size());
  for (TIndex i = 0, size = meshIds.size(); i < size; ++i)
  {
    if (EXPR_SGT(MModel).HasMesh(meshIds[i]) == false)
    {
      std::cerr << "Failed to create normal. Given meshId is not valid.\n";
      return false;
    }
    pMeshes[i] = EXPR_SGT(MModel).GetMesh(meshIds[i]);
  }

  // Resize normal container into this->mVertices.size().
  this->mNormals.resize(this->mVertices.size());

  // Loop each triangle.
  for (auto& pMesh : pMeshes)
  {
    auto& indices = pMesh->GetIndices();
    if (indices.size() % 3 != 0)
    {
      std::cerr << "Failed to create normal. The count of given index size can not be divided by 3.\n";
      return false;
    }

    // Flat normals (temporary)
    using ::dy::math::Cross;
    for (TIndex i = 0, size = indices.size(); i < size; i += 3)
    {
      assert(indices[i+0].mVertexIndex != -1);
      assert(indices[i+1].mVertexIndex != -1);
      assert(indices[i+2].mVertexIndex != -1);

      const std::array<TI32, 3> idx = {indices[i+0].mVertexIndex, indices[i+1].mVertexIndex, indices[i+2].mVertexIndex};
      const auto& p0 = this->mVertices[idx[0]];
      const auto& p1 = this->mVertices[idx[1]];
      const auto& p2 = this->mVertices[idx[2]];

      // Calculate flat normal.
      const auto normal = Cross(p1 - p0, p2 - p0).Normalize();
      
      // Check given (i0, i1, i2) is empty (DVec3{0}).
      // If empty, just insert it into [in]. But if not empty, insert normal into last and get new index from list.
      for (TIndex j = 0; j < 3; ++j)
      {
        if (this->mNormals[idx[j]] == DVec3{})
        {
          this->mNormals[idx[j]] = normal;
          indices[i+j].mNormalIndex = idx[j];
        }
        else
        {
          this->mNormals.emplace_back(normal);
          indices[i+j].mNormalIndex = TI32(this->mNormals.size() - 1);
        }
      }
    }
  }

  return true;
}

TIndex DModelBuffer::GetCountOfVertices() const noexcept
{
  return this->mVertices.size();
}

TIndex DModelBuffer::GetCountOfNormals() const noexcept
{
  return this->mNormals.size();
}

TIndex DModelBuffer::GetCountOfUv0s() const noexcept
{
  return this->mUV0s.size();
}

bool DModelBuffer::HasVertices() const noexcept
{
  return this->mVertices.empty() == false;
}

bool DModelBuffer::HasNormals() const noexcept
{
  return this->GetCountOfNormals() != 0;
}

bool DModelBuffer::HasUv0s() const noexcept
{
  return this->mUV0s.empty() == false;
}

const DModelBuffer::TPointVertices& DModelBuffer::GetVertices() const noexcept
{
  return this->mVertices;
}

const DModelBuffer::TNormals& DModelBuffer::GetNormals() const noexcept
{
  return this->mNormals;
}

const DModelBuffer::TUVs& DModelBuffer::GetUV0s() const noexcept
{
  return this->mUV0s;
}

DModelBufferId DModelBuffer::GetId() const noexcept
{
  return this->mId;
}

} /// ::ray namespace