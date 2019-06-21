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

namespace ray
{

DModelBuffer::DModelBuffer(const tinyobj::attrib_t& attrib)
  : DModelBuffer { ::dy::math::DUuid{true}, attrib }
{ }

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

  // If normals is not created, generate normal using geometry.
  // Created normal list count will be same to `this->mVertices.size()`.
  if (this->mNormals.empty() == true)
  {
    this->CreateNormals();
  }

  // Done?
}

void DModelBuffer::CreateNormals()
{
  // Not implemented yet.
  return;
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

bool DModelBuffer::HasNormals() const noexcept
{
  return this->GetCountOfNormals() != 0;
}

DModelBufferId DModelBuffer::GetId() const noexcept
{
  return this->mId;
}

} /// ::ray namespace